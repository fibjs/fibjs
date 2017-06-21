var test = require("test");
var coroutine = require('coroutine');
test.setup();

var fs = require('fs');

var vmid = coroutine.vmid;

function unlink(pathname) {
    try {
        fs.rmdir(pathname);
    } catch (e) {}
}

var pathname = 'test_dir' + vmid;
var pathname1 = 'test1_dir' + vmid;

var win = require("os").type() == "Windows";

describe('fs', () => {
    before(() => {
        unlink(pathname);
        unlink(pathname1);
    });

    it("stat", () => {
        var st = fs.stat('.');

        assert.equal(st.isDirectory(), true);
        assert.equal(st.isFile(), false);
        assert.equal(st.isExecutable(), true);
        assert.equal(st.isReadable(), true);
        assert.equal(st.isWritable(), true);
    });

    it("file open & close", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');
        assert.doesNotThrow(() => {
            f.read(100);
        });
        f.close();
        assert.throws(() => {
            f.read(100);
        });

        assert.ok(fs.exists(__dirname + '/fs_test.js'));
    });

    it("file.stat", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');

        assert.deepEqual(f.stat().toJSON(), fs.stat(__dirname + '/fs_test.js').toJSON());
    });

    it("mkdir", () => {
        fs.mkdir(pathname, 511);
        assert.equal(fs.exists(pathname), true);

        if (!win) {
            var st = fs.stat(pathname);
            assert.equal(st.mode & 511, 511);
        }
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

    it("file.size", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');
        var st = fs.stat(__dirname + '/fs_test.js');
        assert.equal(st.size, f.size());
        f.close();
    });

    if (!win) {
        it("fs.chmod", () => {
            var st = fs.stat(__dirname + '/fs_test.js');
            var oldm = st.mode;

            fs.chmod(__dirname + '/fs_test.js', 511);
            var st = fs.stat(__dirname + '/fs_test.js');
            assert.equal(st.mode & 511, 511);

            fs.chmod(__dirname + '/fs_test.js', oldm);
            var st = fs.stat(__dirname + '/fs_test.js');
            assert.equal(st.mode & 511, oldm & 511);
        });

        it("file.chmod", () => {
            var f = fs.openFile(__dirname + '/fs_test.js');
            var st = fs.stat(__dirname + '/fs_test.js');
            var oldm = st.mode;

            f.chmod(511);
            var st = fs.stat(__dirname + '/fs_test.js');
            assert.equal(st.mode & 511, 511);

            f.chmod(oldm);
            var st = fs.stat(__dirname + '/fs_test.js');
            assert.equal(st.mode & 511, oldm & 511);

            f.close();
        });

        it("fs.lchmod", () => {
            var fn = __dirname + '/fs_test.js.symlink';

            fs.symlink(__dirname + '/fs_test.js', fn);
            fs.lchmod(fn, 511);

            var st = fs.stat(__dirname + '/fs_test.js');
            var oldm = st.mode;
            var st = fs.lstat(fn);

            assert.notEqual(st.mode & 511, oldm & 511);
            assert.equal(st.mode & 511, 511);

            fs.unlink(fn);
        });

        it("fs.chown", () => {
            var fn = __dirname + '/fs_test.js' + vmid;
            fs.writeFile(fn, 'chown test');
            if (require('os').userInfo().username != 'root')
                assert.throws(() => {
                    fs.chown(fn, 23, 45)
                });
            else {
                assert.doesNotThrow(function() {
                    fs.chown(fn, 23, 45)
                });
                var st = fs.stat(fn);
                assert.equal(st.uid, 23);
                assert.equal(st.gid, 45);
            }
            fs.unlink(fn);
        });

        it("fs.lchown", () => {
            var fn = __dirname + '/fs_test.js.symlink';

            fs.symlink(__dirname + '/fs_test.js', fn);
            if (require('os').userInfo().username != 'root')
                assert.throws(() => {
                    fs.lchown(fn, 23, 45)
                });
            else {
                assert.doesNotThrow(function() {
                    fs.lchown(fn, 23, 45)
                });
                var st = fs.stat(__dirname + '/fs_test.js');
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
        var f = fs.openFile(__dirname + '/fs_test.js');

        var f1 = fs.openFile(__dirname + '/fs_test.js.bak' + vmid, 'w+');
        f1.write(f.read(f.size()));

        f1.rewind();
        var b = f1.read(f1.size() + 100);
        assert.equal(true, f1.eof());
        assert.equal(f1.size(), b.length);

        f.close();
        f1.close();
        fs.unlink(__dirname + '/fs_test.js.bak' + vmid);
    });

    it("readFile", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');

        var s = fs.readFile(__dirname + "/fs_test.js");
        assert.deepEqual(s, f.read(f.size()));

        f.close();
    });

    it("readTextFile", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');

        var s = fs.readTextFile(__dirname + "/fs_test.js");
        assert.equal(s, f.read(f.size()).toString());

        f.close();
    });

    it("openTextStream", () => {
        var f = fs.openTextStream(__dirname + '/fs_test.js');
        f.EOL = '\n';

        var a = fs.readTextFile(__dirname + "/fs_test.js").replace(/\r/g, "").split("\n");
        var b = f.readLines();
        assert.deepEqual(a, b);

        f.close();
    });

    it("tell", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');
        var st = fs.stat(__dirname + '/fs_test.js');

        f.read(f.size());

        assert.equal(st.size, f.tell());
        f.rewind();
        assert.equal(0, f.tell());

        f.close();
    });

    it("seek", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');
        f.seek(f.size() + 10, fs.SEEK_SET);
        assert.equal(f.tell(), f.size() + 10);
        f.seek(10, fs.SEEK_SET);
        var b = f.read(f.size());
        assert.equal(f.size() - 10, b.length);
        f.close();
    });

    it("seek 64 bits", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');
        f.seek(f.size() + 8589934592, fs.SEEK_SET);
        assert.equal(f.tell(), f.size() + 8589934592);
        f.close();
    });

    it("copyTo", () => {
        var f = fs.openFile(__dirname + '/fs_test.js');
        var f1 = fs.openFile(__dirname + '/fs_test.js.bak' + vmid, 'w');

        var s = f.copyTo(f1, 100);
        assert.equal(s, 100);
        assert.equal(f1.size(), 100);
        f.copyTo(f1);
        assert.equal(f1.size(), f.size());

        f.close();
        f1.close();

        fs.unlink(__dirname + '/fs_test.js.bak' + vmid);
    });

    it("readdir", () => {
        var fl = fs.readdir(__dirname + '/vm_test').toJSON();
        fl.sort((a, b) => {
            if (a > b)
                return 1;
            if (a < b)
                return -1;
            return 0;
        });
        var sz = fl.length;
        assert.greaterThan(sz, 3);
        assert.equal(fl[sz - 2], 't1.js');
        assert.equal(fl[sz - 1], 't2.js');
    });

    it("writeFile & appendFile", () => {
        var fn = __dirname + '/fs_test.js' + vmid;

        fs.writeFile(fn, 'data to be written');
        assert.equal(fs.readFile(fn), 'data to be written');
        fs.appendFile(fn, 'data to be appended');
        assert.equal(fs.readFile(fn), 'data to be writtendata to be appended');

        fs.unlink(fn);
    });

    it("link", () => {
        var fn = __dirname + '/fs_test.js' + vmid;
        var fn1 = __dirname + '/fs_test.js.link' + vmid
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
        var fn = __dirname + '/fs_test.js' + vmid;
        fs.writeFile(fn, 'truncate test');
        assert.equal(fs.stat(fn).size, 13);
        fs.truncate(fn, 100);
        assert.equal(fs.stat(fn).size, 100);
        fs.truncate(fn, 10);
        assert.equal(fs.stat(fn).size, 10);
        fs.unlink(fn);
    });

    it("symlink & & lstat & readlink & realpath", () => {
        var fn = __dirname + '/fs_test.js';
        var fn1 = __dirname + '/fs_test.js.symlink' + vmid;
        fs.symlink(fn, fn1);
        if (!win) {
            assert.ok(fs.lstat(fn1).isSymbolicLink());
            assert.equal(fs.readlink(fn1), fn);
            assert.equal(fs.realpath(fn1), fn);
        }
        assert.equal(fs.readFile(fn).toString(), fs.readFile(fn1).toString());
        fs.unlink(fn1);
    })

    it("access", () => {
        var fn = __dirname + '/fs_test.js';
        var fn1 = __dirname + '/fs_test.js.access' + vmid;
        assert.doesNotThrow(() => {
            fs.access(fn, fs.constants.F_OK | fs.constants.W_OK | fs.constants.R_OK);
        });
        assert.throws(() => {
            fs.access(fn1, fs.constants.F_OK);
        })
    })
});

argv.length && test.run(console.DEBUG);