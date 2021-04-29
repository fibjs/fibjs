var test = require("test");
var coroutine = require('coroutine');
var path = require('path');
var fs = require('fs');
var zip = require('zip');
var io = require('io');
var {
    ensureDirectoryExisted
} = require('./_helpers/process');

test.setup();

var vmid = coroutine.vmid;
var isWin32 = process.platform === 'win32';

function rmdir(pathname) {
    try {
        fs.rmdir(pathname);
    } catch (e) { }
}

var pathname = 'test_dir' + vmid;
var pathname1 = 'test1_dir' + vmid;
var pathname2 = 'test2_dir' + vmid;

var win = require("os").type() == "Windows";
var linux = require("os").type() == "Linux";

function assert_stat_property(statObj) {
    assert.isNumber(statObj.dev)
    assert.isNumber(statObj.mode)
    assert.isNumber(statObj.nlink)
    assert.isNumber(statObj.uid)
    assert.isNumber(statObj.gid)
    assert.isNumber(statObj.rdev)
    assert.isNumber(statObj.blksize)
    assert.isNumber(statObj.ino)
    assert.isNumber(statObj.size)
    assert.isNumber(statObj.blocks)
    assert.isNumber(statObj.atimeMs)
    assert.isNumber(statObj.mtimeMs)
    assert.isNumber(statObj.ctimeMs)
    assert.isNumber(statObj.birthtimeMs)

    assert.ok(statObj.atime instanceof Date)
    assert.ok(statObj.mtime instanceof Date)
    assert.ok(statObj.ctime instanceof Date)
    assert.ok(statObj.birthtime instanceof Date)
}

describe('fs', () => {
    before(() => {
        rmdir(pathname);
        rmdir(pathname1);
    });

    after(() => {
        try {
            fs.unlink(path.join(__dirname, 'unzip_test.zip'));
        } catch (e) { }
    });

    describe("stat", () => {
        it('file', () => {
            var st = fs.stat('.');

            assert_stat_property(st);

            assert.equal(st.isDirectory(), true);
            assert.equal(st.isFile(), false);
            if (process.platform === 'win32') {
                assert.equal(st.isExecutable(), false);
            } else {
                assert.equal(st.isExecutable(), true);
            }
            assert.equal(st.isReadable(), true);
            assert.equal(st.isWritable(), true);


            st = fs.stat(path.join(__dirname, 'abc', '../'));

            assert.equal(st.isDirectory(), true);
            assert.equal(st.isFile(), false);
            if (process.platform === 'win32') {
                assert.equal(st.isExecutable(), false);
            } else {
                assert.equal(st.isExecutable(), true);
            }
            assert.equal(st.isReadable(), true);
            assert.equal(st.isWritable(), true);
        });

        it('directory', () => {
            var test_dir = path.resolve(__dirname, `./fs_files/dir${coroutine.vmid}`);

            rmdir(test_dir);
            ensureDirectoryExisted(test_dir);
            var st = fs.stat(test_dir);
            rmdir(test_dir);

            assert_stat_property(st);

            assert.equal(st.isDirectory(), true);
            assert.equal(st.isFile(), false);
            if (process.platform === 'win32') {
                assert.equal(st.isExecutable(), false);
            } else {
                assert.equal(st.isExecutable(), true);
            }
            assert.equal(st.isReadable(), true);
            assert.equal(st.isWritable(), true);
        });

        it('error.code', () => {
            var code;

            try {
                fs.stat('aaaa');
            } catch (e) {
                code = e.code;
            }

            assert.equal(code, "ENOENT");
        })
    });

    it("file open & close", () => {
        var fd = fs.open(path.join(__dirname, 'fs_test.js'));

        assert.isNumber(fd);
        assert.greaterThan(fd, -1);
        assert.doesNotThrow(() => {
            fs.close(fd);
        });

        if (!isWin32) {
            assert.doesNotThrow(() => {
                fs.close(fd);
            });
        }

        var fd1 = fs.open(path.join(__dirname, 'fs_test.js'));
        var fd2 = fs.open(path.join(__dirname, 'fs_test.js'));

        assert.greaterThan(fd2, fd1);
        fs.close(fd1);
        fs.close(fd2);
    });

    it("file openSync & closeSync", () => {
        var fd = fs.openSync(path.join(__dirname, 'fs_test.js'));
        assert.isNumber(fd);
        assert.greaterThan(fd, -1);
        assert.doesNotThrow(() => {
            fs.closeSync(fd);
        });
        if (!isWin32) {
            assert.doesNotThrow(() => {
                fs.closeSync(fd);
            });
        }

        var fd1 = fs.openSync(path.join(__dirname, 'fs_test.js'));
        var fd2 = fs.openSync(path.join(__dirname, 'fs_test.js'));

        assert.greaterThan(fd2, fd1);
        fs.closeSync(fd1);
        fs.closeSync(fd2);
    });

    it("file openFile & close", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        assert.doesNotThrow(() => {
            f.read(100);
        });
        f.close();
        assert.throws(() => {
            f.read(100);
        });

        assert.ok(fs.exists(path.join(__dirname, 'fs_test.js')));
    });

    it("file.stat", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));

        assert.deepEqual(f.stat().toJSON(), fs.stat(path.join(__dirname, 'fs_test.js')).toJSON());
    });

    it("mkdir", () => {
        fs.mkdir(pathname);
        assert.equal(fs.exists(pathname), true);

        if (!win) {
            var st = fs.stat(pathname);
            assert.equal(st.mode & 0o777, 0o755);
        }
    });

    it("mkdir with fstat mode", () => {
        const mode = 0o511;
        fs.mkdir(pathname2, mode);
        assert.equal(fs.exists(pathname2), true);

        if (!win) {
            var st = fs.stat(pathname2);
            assert.equal(st.mode & 0o777, mode);
        }

        fs.rmdir(pathname2);
        assert.equal(fs.exists(pathname2), false);
    });

    it("rename", () => {
        fs.rename(pathname, pathname1);
        assert.equal(fs.exists(pathname), false);
        assert.equal(fs.exists(pathname1), true);
    });

    it("rmdir", () => {
        fs.rmdir(pathname1);
        assert.equal(fs.exists(pathname1), false);
    });

    it("mkdir recursive", () => {
        var recursive_path = path.join(pathname, pathname);
        fs.mkdir(recursive_path, {
            recursive: true
        });
        assert.equal(fs.exists(recursive_path), true);

        fs.rmdir(recursive_path);
        fs.rmdir(pathname);
    });

    it("file.size", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var st = fs.stat(path.join(__dirname, 'fs_test.js'));
        assert.equal(st.size, f.size());
        f.close();
    });

    if (!win) {
        it("fs.chmod", () => {
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            var oldm = st.mode;

            fs.chmod(path.join(__dirname, 'fs_test.js'), 511);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, 511);

            fs.chmod(path.join(__dirname, 'fs_test.js'), oldm);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, oldm & 511);
        });

        it("file.chmod", () => {
            var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            var oldm = st.mode;

            f.chmod(511);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, 511);

            f.chmod(oldm);
            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            assert.equal(st.mode & 511, oldm & 511);

            f.close();
        });

        it("fs.lchmod", () => {
            if (linux)
                return;
            var fn = path.join(__dirname, 'fs_test.js.symlink');
            try {
                fs.unlink(fn);
            } catch (e) { }

            fs.symlink(path.join(__dirname, 'fs_test.js'), fn);
            fs.lchmod(fn, 511);

            var st = fs.stat(path.join(__dirname, 'fs_test.js'));
            var oldm = st.mode;
            var st = fs.lstat(fn);

            assert.notEqual(st.mode & 511, oldm & 511);
            assert.equal(st.mode & 511, 511);

            fs.unlink(fn);
        });

        it("fs.chown", () => {
            var fn = path.join(__dirname, 'fs_test.js' + vmid);
            fs.writeFile(fn, 'chown test');
            if (require('os').userInfo().username != 'root')
                assert.throws(() => {
                    fs.chown(fn, 23, 45)
                });
            else {
                assert.doesNotThrow(() => fs.chown(fn, 23, 45));
                var st = fs.stat(fn);
                assert.equal(st.uid, 23);
                assert.equal(st.gid, 45);
            }
            fs.unlink(fn);
        });

        it("fs.lchown", () => {
            var fn = path.join(__dirname, 'fs_test.js.symlink');

            if (fs.exists(fn))
                fs.unlink(fn);
            fs.symlink(path.join(__dirname, 'fs_test.js'), fn);
            if (require('os').userInfo().username != 'root')
                assert.throws(() => {
                    fs.lchown(fn, 23, 45)
                });
            else {
                assert.doesNotThrow(() => fs.lchown(fn, 23, 45));
                var st = fs.stat(path.join(__dirname, 'fs_test.js'));
                var lst = fs.lstat(fn);
                assert.notEqual(st.uid, 23);
                assert.notEqual(st.gid, 45);
                assert.equal(lst.uid, 23);
                assert.equal(lst.gid, 45);
            }
            fs.unlink(fn);
        });
    }

    it("file read & write", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));

        var f1 = fs.openFile(path.join(__dirname, 'fs_test.js.bak' + vmid), 'w+');
        f1.write(f.read(f.size()));

        f1.rewind();
        var b = f1.read(f1.size() + 100n);
        assert.equal(true, f1.eof());
        assert.equal(f1.size(), b.length);

        f.close();
        f1.close();
        fs.unlink(path.join(__dirname, 'fs_test.js.bak' + vmid));
    });

    it("readFile", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var d = f.readAll();

        var s = fs.readFile(path.join(__dirname, "fs_test.js"));
        assert.deepEqual(s, d);

        s = fs.readFile(path.join(__dirname, "fs_test.js"), "utf8");
        assert.deepEqual(s, d.toString());

        f.close();
    });

    it("readTextFile", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));

        var s = fs.readTextFile(path.join(__dirname, "fs_test.js"));
        assert.equal(s, f.read(f.size()).toString());

        f.close();
    });

    it("openTextStream", () => {
        var f = fs.openTextStream(path.join(__dirname, 'fs_test.js'));
        f.EOL = '\n';

        var a = fs.readTextFile(path.join(__dirname, "fs_test.js")).replace(/\r/g, "").split("\n");
        var b = f.readLines();
        assert.deepEqual(a, b);

        f.close();
    });

    it("tell", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var st = fs.stat(path.join(__dirname, 'fs_test.js'));

        f.read(f.size());

        assert.equal(st.size, f.tell());
        f.rewind();
        assert.equal(0, f.tell());

        f.close();
    });

    it("seek", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        f.seek(f.size() + 10n, fs.SEEK_SET);
        assert.equal(f.tell(), f.size() + 10n);
        f.seek(10, fs.SEEK_SET);
        var b = f.read(f.size());
        assert.equal(f.size() - 10n, b.length);
        f.close();
    });

    it("seek 64 bits", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        f.seek(f.size() + 8589934592n, fs.SEEK_SET);
        assert.equal(f.tell(), f.size() + 8589934592n);
        f.close();
    });

    it("copyTo", () => {
        var f = fs.openFile(path.join(__dirname, 'fs_test.js'));
        var f1 = fs.openFile(path.join(__dirname, 'fs_test.js.bak' + vmid), 'w');

        var s = f.copyTo(f1, 100);
        assert.equal(s, 100);
        assert.equal(f1.size(), 100);
        f.copyTo(f1);
        assert.equal(f1.size(), f.size());

        f.close();
        f1.close();

        fs.unlink(path.join(__dirname, 'fs_test.js.bak' + vmid));
    });

    it("zip folder", () => {
        function save_zip(n) {
            var zipfile = zip.open(path.join(__dirname, 'unzip_test.zip'), "w");
            zipfile.write(new Buffer('test ' + n), 'test.txt');
            zipfile.write(new Buffer(`module.exports = "fibjs-test-require-${n}"`), 'test.js');
            zipfile.close();
        }

        function test_zip(n, first_test_n = n) {
            assert.equal(fs.readTextFile(path.join(__dirname, "unzip_test.zip$", "test.txt")),
                "test " + n);
            assert.equal(require('./unzip_test.zip$/test.js'), `fibjs-test-require-${first_test_n}`)
        }

        var first_test_n = 1;

        save_zip(first_test_n);
        coroutine.sleep(1000);
        test_zip(first_test_n);

        save_zip(2);
        test_zip(1, first_test_n);

        coroutine.sleep(4000);
        test_zip(2, first_test_n);
    });

    it("zip data", () => {
        function save_zip(n) {
            var stream = new io.MemoryStream();
            var zipfile = zip.open(stream, "w");
            zipfile.write(new Buffer('test ' + n), 'test.txt');
            zipfile.close();

            stream.rewind();
            fs.setZipFS("/unzip_test.zip", stream.readAll());
        }

        function test_zip(n) {
            assert.equal(fs.readTextFile(path.join("/unzip_test.zip$", "test.txt")),
                "test " + n);
        }

        save_zip(1);
        test_zip(1);

        save_zip(2);
        test_zip(2);

        /**
         * content of zip-file(e.g. named with 'safename.zip') would be cached when
         * read by 'fs' module, and expire after 3000ms, so here we sleep 4000ms to
         * wait cached 'unzip_test.zip' expired
         */
        coroutine.sleep(4000);
        test_zip(2);

        fs.clearZipFS("/unzip_test.zip");

        assert.throws(() => {
            test_zip(2);
        });
    });

    describe('read', () => {
        var fd;
        before(() => fd = fs.open(path.join(__dirname, 'fs_files', 'read.txt')));
        after(() => fs.close(fd));

        describe('fiber sync', () => {
            it('zero read', () => {
                const buf = new Buffer(1);
                let bytes = fs.read(fd, buf);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));

                bytes = fs.read(fd, buf, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));

                bytes = fs.read(fd, buf, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));

                bytes = fs.read(fd, buf, 0, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));
            });

            it('full read', () => {
                const buf = new Buffer(15);
                const bytes = fs.read(fd, buf, 0, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('abcdefg\nhijklmn'));
            });

            it('repeat read', () => {
                const buf1 = new Buffer(15);
                const buf2 = new Buffer(15);
                const buf3 = new Buffer(1);
                const bytes1 = fs.read(fd, buf1, 0, 15, 0);
                const bytes2 = fs.read(fd, buf2, 0, 15, 0);
                const bytes3 = fs.read(fd, buf3, 0, 1);
                assert.equal(bytes1, 15);
                assert.equal(bytes2, 15);
                assert.equal(bytes3, 0);
                assert.deepEqual(buf1, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf2, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf3, new Buffer(1));
            });

            it('offset error read', () => {
                const buf = new Buffer(1);
                assert.throws(() => fs.read(fd, buf, 1, 1, 0));
                assert.throws(() => fs.read(fd, buf, 2, 1, 0));
                assert.throws(() => fs.read(fd, buf, -1, 1, 0));
                assert.doesNotThrow(() => fs.read(fd, buf, 0, 1, 0));
            });

            it('beyond buffer error read', () => {
                const buf = new Buffer(4);
                assert.throws(() => fs.read(fd, buf, 2, 3, 0));
                assert.throws(() => fs.read(fd, buf, 3, 2, 0));
                assert.throws(() => fs.read(fd, buf, 0, -1, 0));
                assert.doesNotThrow(() => fs.read(fd, buf, 2, 2, 0));
            });

            it('spec offset read', () => {
                const buf = new Buffer(16);
                buf.write('x');
                let bytes = fs.read(fd, buf, 1, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('xabcdefg\nhijklmn'));
            });

            it('spec position read', () => {
                const buf = new Buffer(14);
                let bytes = fs.read(fd, buf, 0, 14, 1);
                assert.equal(bytes, 14);
                assert.deepEqual(buf, new Buffer('bcdefg\nhijklmn'));
            });
        });

        describe('block sync', () => {
            it('zero read', () => {
                const buf = new Buffer(1);
                let bytes = fs.read(fd, buf);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));

                bytes = fs.read(fd, buf, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));

                bytes = fs.read(fd, buf, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));

                bytes = fs.read(fd, buf, 0, 0, 0);
                assert.equal(bytes, 0);
                assert.deepEqual(buf, new Buffer(1));
            });

            it('full read', () => {
                const buf = new Buffer(15);
                const bytes = fs.readSync(fd, buf, 0, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('abcdefg\nhijklmn'));
            });

            it('repeat read', () => {
                const buf1 = new Buffer(15);
                const buf2 = new Buffer(15);
                const buf3 = new Buffer(1);
                const bytes1 = fs.readSync(fd, buf1, 0, 15, 0);
                const bytes2 = fs.readSync(fd, buf2, 0, 15, 0);
                const bytes3 = fs.readSync(fd, buf3, 0, 1);
                assert.equal(bytes1, 15);
                assert.equal(bytes2, 15);
                assert.equal(bytes3, 0);
                assert.deepEqual(buf1, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf2, new Buffer('abcdefg\nhijklmn'));
                assert.deepEqual(buf3, new Buffer(1));
            });

            it('offset error read', () => {
                const buf = new Buffer(1);
                assert.throws(() => fs.readSync(fd, buf, 1, 15, 0));
                assert.throws(() => fs.readSync(fd, buf, 2, 15, 0));
                assert.throws(() => fs.readSync(fd, buf, -1, 1, 0));
                assert.doesNotThrow(() => fs.readSync(fd, buf, 0, 1, 0));
            });

            it('beyond buffer error read', () => {
                const buf = new Buffer(4);
                assert.throws(() => fs.readSync(fd, buf, 2, 3, 0));
                assert.throws(() => fs.readSync(fd, buf, 3, 2, 0));
                assert.doesNotThrow(() => fs.readSync(fd, buf, 2, 2, 0));
            });

            it('spec offset read', () => {
                const buf = new Buffer(16);
                buf.write('x');
                let bytes = fs.readSync(fd, buf, 1, 15, 0);
                assert.equal(bytes, 15);
                assert.deepEqual(buf, new Buffer('xabcdefg\nhijklmn'));
            });

            it('spec position read', () => {
                const buf = new Buffer(14);
                let bytes = fs.readSync(fd, buf, 0, 14, 1);
                assert.equal(bytes, 14);
                assert.deepEqual(buf, new Buffer('bcdefg\nhijklmn'));
            });
        });

        describe('event callback', () => {
            it('zero read', done => {
                const buf = new Buffer(1);
                fs.read(fd, buf, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 0);
                        assert.deepEqual(buf, new Buffer(1));
                        fs.read(fd, buf, 0, (err, bytes) => {
                            if (err) done(err)
                            else {
                                assert.equal(bytes, 0);
                                assert.deepEqual(buf, new Buffer(1));
                                fs.read(fd, buf, 0, 0, (err, bytes) => {
                                    if (err) done(err)
                                    else {
                                        assert.equal(bytes, 0);
                                        assert.deepEqual(buf, new Buffer(1));
                                        fs.read(fd, buf, 0, 0, 0, (err, bytes) => {
                                            if (err) done(err)
                                            else {
                                                assert.equal(bytes, 0);
                                                assert.deepEqual(buf, new Buffer(1));
                                                done();
                                            }
                                        });
                                    }
                                });
                            }
                        });
                    }
                });
            });

            it('full read', done => {
                const buf = new Buffer(15);
                fs.read(fd, buf, 0, 15, 0, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 15);
                        assert.deepEqual(buf, new Buffer('abcdefg\nhijklmn'));
                        done();
                    }
                });
            });

            it('repeat read', done => {
                const buf1 = new Buffer(15);
                const buf2 = new Buffer(15);
                const buf3 = new Buffer(1);
                fs.read(fd, buf1, 0, 15, 0, (err, bytes1) => {
                    if (err) done(err)
                    else {
                        fs.read(fd, buf2, 0, 15, 0, (err, bytes2) => {
                            if (err) done(err)
                            else {
                                fs.read(fd, buf2, 3, 1, (err, bytes3) => {
                                    if (err) done(err)
                                    else {
                                        assert.equal(bytes1, 15);
                                        assert.equal(bytes2, 15);
                                        assert.equal(bytes3, 0);
                                        assert.deepEqual(buf1, new Buffer('abcdefg\nhijklmn'));
                                        assert.deepEqual(buf2, new Buffer('abcdefg\nhijklmn'));
                                        assert.deepEqual(buf3, new Buffer(1));
                                        done();
                                    }
                                });
                            }
                        });
                    }
                });
            });

            it('offset error read', done => {
                const buf = new Buffer(1);
                fs.read(fd, buf, 1, 1, 0, (err, bytes) => {
                    if (err) {
                        fs.read(fd, buf, 2, 1, 0, (err, bytes) => {
                            if (err) {
                                fs.read(fd, buf, -1, 1, 0, (err, byts) => {
                                    if (err) {
                                        fs.read(fd, buf, 0, 1, 0, (err, byts) => {
                                            if (err) done(err)
                                            else
                                                done();
                                        });
                                    } else done(new Error('should throws'));
                                });
                            } else done(new Error('should throws'));
                        });
                    } else done(new Error('should throws'));
                });
            });

            it('beyond buffer error read', done => {
                const buf = new Buffer(4);
                fs.read(fd, buf, 2, 3, 0, (err, bytes) => {
                    if (err) {
                        fs.read(fd, buf, 3, 2, 0, (err, bytes) => {
                            if (err) {
                                fs.read(fd, buf, 2, 2, 0, (err, byts) => {
                                    if (err) done(err)
                                    else
                                        done();
                                });
                            } else done(new Error('should throws'));
                        });
                    } else done(new Error('should throws'));
                });
            });

            it('spec offset read', done => {
                const buf = new Buffer(16);
                buf.write('x');
                fs.read(fd, buf, 1, 15, 0, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 15);
                        assert.deepEqual(buf, new Buffer('xabcdefg\nhijklmn'));
                        done();
                    }
                });
            });

            it('spec position read', done => {
                const buf = new Buffer(14);
                fs.read(fd, buf, 0, 14, 1, (err, bytes) => {
                    if (err) done(err)
                    else {
                        assert.equal(bytes, 14);
                        assert.deepEqual(buf, new Buffer('bcdefg\nhijklmn'));
                        done();
                    }
                });
            });
        });
    });

    it("readdir", () => {
        var fl = fs.readdir(path.join(__dirname, 'vm_test'));
        fl.sort();
        var sz = fl.length;
        assert.greaterThan(sz, 3);
        assert.equal(fl[sz - 4], 't1.js');
        assert.equal(fl[sz - 3], 't2.js');
        assert.equal(fl[sz - 2], 'test_refresh.js');
    });

    it("writeFile & appendFile", () => {
        var fn = path.join(__dirname, 'fs_test.js' + vmid);

        fs.writeFile(fn, 'data to be written');
        assert.equal(fs.readFile(fn), 'data to be written');
        fs.appendFile(fn, 'data to be appended');
        assert.equal(fs.readFile(fn), 'data to be writtendata to be appended');

        fs.unlink(fn);
    });

    it("link", () => {
        var fn = path.join(__dirname, 'fs_test.js' + vmid);
        var fn1 = path.join(__dirname, 'fs_test.js.link' + vmid);
        fs.writeFile(fn, 'link test');
        fs.link(fn, fn1);
        assert.deepEqual(fs.stat(fn).ctime, fs.stat(fn1).ctime);
        assert.deepEqual(fs.stat(fn).atime, fs.stat(fn1).atime);
        assert.deepEqual(fs.stat(fn).mtime, fs.stat(fn1).mtime);
        assert.equal(fs.stat(fn).mode, fs.stat(fn1).mode);
        assert.equal(fs.stat(fn).size, fs.stat(fn1).size);
        fs.unlink(fn);
        assert.equal(fs.readFile(fn1), 'link test');
        fs.unlink(fn1);
    });

    it("truncate", () => {
        var fn = path.join(__dirname, 'fs_test.js' + vmid);
        fs.writeFile(fn, 'truncate test');
        assert.equal(fs.stat(fn).size, 13);
        fs.truncate(fn, 100);
        assert.equal(fs.stat(fn).size, 100);
        fs.truncate(fn, 10);
        assert.equal(fs.stat(fn).size, 10);
        fs.unlink(fn);
    });

    describe("symlink & lstat & readlink & realpath", () => {
        function proc() {
            var fn = path.join(__dirname, 'fs_test.js');
            var fn1 = path.join(__dirname, 'fs_test.js.symlink' + vmid);
            fs.symlink(fn, fn1);
            var _lstat = fs.lstat(fn1);

            assert_stat_property(_lstat);

            assert.ok(_lstat.isSymbolicLink());
            assert.equal(fs.readlink(fn1), fn);
            assert.equal(fs.realpath(fn1), fn);
            assert.equal(fs.readFile(fn).toString(), fs.readFile(fn1).toString());
            if (win) {
                var dir = path.join(__dirname, 'dirtest');
                var dir1 = path.join(__dirname, 'dirtestsymlink');
                var file = path.join(dir, 'file');
                var file1 = path.join(dir1, 'file');
                fs.mkdir(dir, 511);
                fs.writeFile(file, 'symlink test');
                fs.symlink(dir, dir1, 'junction');
                assert.equal(fs.readFile(file1).toString(), 'symlink test');
                fs.unlink(file1);
                fs.rmdir(dir1);
                fs.rmdir(dir);
            }
            fs.unlink(fn1);
        }
        it("number version", () => {
            proc(false);
        });
    })

    it("access", () => {
        var fn = path.join(__dirname, 'fs_test.js');
        var fn1 = path.join(__dirname, 'fs_test.js.access' + vmid);
        assert.doesNotThrow(() => {
            fs.access(fn, fs.constants.F_OK | fs.constants.W_OK | fs.constants.R_OK);
        });
        assert.throws(() => {
            fs.access(fn1, fs.constants.F_OK);
        })
    })
});

require.main === module && test.run(console.DEBUG);