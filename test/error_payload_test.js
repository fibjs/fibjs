var { describe, it } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var io = require('io');
var os = require('os');
var path = require('path');

// Writing a handle that was opened read-only is EBADF on POSIX; Windows
// refuses the write with EPERM (libuv maps ERROR_ACCESS_DENIED to it).
const write_denied = process.platform === 'win32' ? 'EPERM' : 'EBADF';

var vmid = process.pid || 0;
var missing = path.join(__dirname, '__missing_error_payload_' + vmid);

describe('error payload', () => {
    it('keeps parameter summaries on sync errors', () => {
        assert.throws(() => fs.readFileSync(missing), (err) => {
            assert.equal(err.code, 'ENOENT');
            assert.equal(err.syscall, 'open');
            assert.equal(err.path, missing);
            assert.deepEqual(err.args, { flags: 'r' });
            return true;
        });
    });

    it('keeps parameter summaries on callback errors', (done) => {
        fs.readFile(missing, (err) => {
            try {
                assert.ok(err instanceof Error);
                assert.equal(err.code, 'ENOENT');
                assert.deepEqual(err.args, { flags: 'r' });
                done();
            } catch (e) {
                done(e);
            }
        });
    });

    it('keeps parameter summaries on promise errors', async () => {
        await assert.rejects(() => fs.promises.readFile(missing), (err) => {
            assert.equal(err.code, 'ENOENT');
            assert.deepEqual(err.args, { flags: 'r' });
            return true;
        });
    });

    it('exposes args as a frozen diagnostic view', () => {
        assert.throws(() => fs.readFileSync(missing), (err) => {
            assert.ok(Object.isFrozen(err.args));
            assert.throws(() => {
                'use strict';
                err.args.flags = 'x';
            }, TypeError);
            return true;
        });
    });

    it('omits args when no summary was attached', () => {
        assert.throws(() => fs.readFileSync(), (err) => {
            assert.isUndefined(err.args);
            return true;
        });
    });

    it('renders Buffer objects as <Buffer len=N>', async () => {
        // A real I/O failure (a write on a read-only stream) renders the buffer
        // through the object summary instead of a bare <Object Buffer>.
        var f = fs.openFile(__filename, 'r');

        try {
            await assert.rejects(async () => {
                await f.write(Buffer.alloc(64, 0x41));
            }, (err) => {
                assert.equal(err.code, write_denied);
                assert.equal(err.args.buffer, '<Buffer len=64>');
                return true;
            });
        } finally {
            await f.close();
        }
    });

    it('builds no summary for normal close', async () => {
        // Closing a handle is a normal end of the operation: the state error it
        // produces must not grow summaries that the error path would discard.
        var fh = fs.open(__filename, 'r');
        await fh.close();

        var closedError = null;
        try {
            await fh.write(Buffer.alloc(16, 0x42));
        } catch (err) {
            closedError = err;
        }

        assert.ok(closedError, 'writing to a closed handle must fail');
        assert.isUndefined(closedError.args);

        var stream = fs.createWriteStream(path.join(os.tmpdir(), 'fibjs_error_payload_closed_' + vmid));
        await stream.close();

        closedError = null;
        try {
            await stream.write(Buffer.alloc(16, 0x42));
        } catch (err) {
            closedError = err;
        }

        assert.ok(closedError, 'writing to a closed stream must fail');
        assert.isUndefined(closedError.args);
    });

    it('renders stream objects as <Stream ClassName>', async () => {
        // A real I/O failure on the write leg names the target stream, so the
        // summary shows the class family instead of a bare <Object FileStream>.
        // /dev/full is the portable-ish sink whose writes always fail.
        if (!fs.exists('/dev/full'))
            return;

        var src = fs.createReadStream(__filename);
        var dest = fs.createWriteStream('/dev/full');

        try {
            await assert.rejects(async () => {
                await io.copyStream(src, dest);
            }, (err) => {
                assert.equal(err.args.to, '<Stream FileStream>');
                return true;
            });
        } finally {
            await src.close();
            await dest.close();
        }
    });

    it('builds no summary when a copy ends by close', async () => {
        // Closing the destination is the documented way to end a copy; the
        // internal code it produces must stay free of summaries.
        var target = path.join(os.tmpdir(), 'fibjs_error_payload_copyclose_' + vmid);
        var src = fs.createReadStream(__filename);
        var dest = fs.createWriteStream(target);

        await dest.close();

        var closedError = null;
        try {
            await io.copyStream(src, dest);
        } catch (err) {
            closedError = err;
        } finally {
            await src.close();
            try {
                fs.unlink(target);
            } catch (e) {
            }
        }

        assert.ok(closedError, 'copying into a closed stream must fail');
        assert.isUndefined(closedError.args);
    });

    it('keeps concurrent async summaries on their own errors', async () => {
        var tasks = [];
		var i;

        // Concurrent file-pool operations must not cross-talk: every failure
        // carries its own path and flags, no matter how the completions
        // interleave with successful reads on the same worker pool.
        for (i = 0; i < 16; i++) {
            (function (name) {
                tasks.push(fs.promises.readFile(name).then(() => {
                    throw new Error('unexpected success: ' + name);
                }, (err) => {
                    assert.equal(err.code, 'ENOENT');
                    assert.equal(err.path, name);
                    assert.deepEqual(err.args, { flags: 'r' });
                    return name;
                }));
            })(path.join(__dirname, '__missing_error_payload_' + vmid + '_' + i));
        }

        for (i = 0; i < 16; i++)
            tasks.push(fs.promises.readFile(__filename).then((data) => {
                assert.ok(data.length > 0);
            }));

        var names = await Promise.all(tasks);

        for (i = 0; i < 16; i++)
            assert.ok(names[i].endsWith('_' + i));
    });

    it('keeps the error code on the callback path', (done) => {
        // The callback path builds the JS error on the JS thread from a
        // captured description; the payload restored next to it carries the
        // code, so it must not be dropped on the way.
        var f = path.join(os.tmpdir(), 'fibjs_error_payload_paths_' + vmid + '.txt');
        var fd = fs.open(f, 'w');

        fs.write(fd, 'hello', 0, 'utf8', () => {
            fs.close(fd, () => {
                var rd = fs.open(f, 'r');
                var buf = Buffer.alloc(1);
                var syncErr = null;

                try {
                    fs.read(rd, buf, 5, 1, 0);
                } catch (e) {
                    syncErr = e;
                }

                assert.equal(syncErr.code, 'ERR_OUT_OF_RANGE');
                assert.equal(syncErr.name, 'RangeError');

                fs.read(rd, buf, 5, 1, 0, (err) => {
                    done(() => {
                        assert.equal(err.name, syncErr.name);
                        assert.equal(err.code, syncErr.code);
                        assert.equal(err.number, syncErr.number);

                        fs.close(rd, () => {
                            try {
                                fs.unlink(f);
                            } catch (e) {
                            }
                        });
                    });
                });
            });
        });
    });
});
