var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var fs = require('fs');
var path = require('path');
var zip = require('zip');
var io = require('io');
var http = require('http');
var coroutine = require('coroutine');

var vmid = require('coroutine').vmid;
var pathname = path.join(__dirname, 'test_dir' + vmid);

var ms = new io.MemoryStream();
var ms1 = new io.MemoryStream();

describe("zip", () => {
    var zipfile;
    var file;
    var efile1,
        efile2,
        efile3;
    before(() => {
        if (!fs.exists(pathname)) {
            fs.mkdir(pathname);
        }
    });
    after(() => {
        if (efile1 && fs.exists(efile1))
            fs.unlink(efile1);

        if (efile2 && fs.exists(efile2))
            fs.unlink(efile2);

        if (efile3 && fs.exists(efile3))
            fs.unlink(efile3);

        if (fs.exists(path.join(__dirname, 'unzip_test.js.extract')))
            fs.unlink(path.join(__dirname, 'unzip_test.js.extract'));

        if (fs.exists(path.join(__dirname, 'unzip_test.zip' + vmid)))
            fs.unlink(path.join(__dirname, 'unzip_test.zip' + vmid));

        // Clean up any remaining files in test directory
        if (fs.exists(pathname)) {
            var files = fs.readdir(pathname);
            for (var i = 0; i < files.length; i++) {
                fs.unlink(path.join(pathname, files[i]));
            }
            fs.rmdir(pathname);
        }

        ms.close();
        ms1.close();
    });
    afterEach(() => {
        if (zipfile) {
            zipfile.close();
            zipfile = null;
        }
        if (file) {
            file.close();
            file = null;
        }
    })

    it("write file to zip", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid), "w");
        zipfile.write(path.join(__dirname, 'unzip_test.js'), 'unzip_test.js');
        zipfile.close();

        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));

        assert.equal('unzip_test.js', zipfile.namelist()[0]);
    });

    it("is zip file", () => {
        assert.equal(zip.isZipFile(path.join(__dirname, 'unzip_test.js')), false);
        assert.equal(zip.isZipFile(path.join(__dirname, 'unzip_test.zip' + vmid)), true);
    });

    it("zip filename in unicode", () => {
        var fname = "测试.zip" + vmid;
        var f = zip.open(path.join(__dirname, fname), "w");
        f.write(path.join(__dirname, 'unzip_test.js'), 'unzip_test.js');
        f.close();
        assert.equal(zip.isZipFile(path.join(__dirname, fname)), true);
        fs.unlink(path.join(__dirname, fname));
    })

    it("namelist & infolist & getinfo", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        var namelist = zipfile.namelist();
        var infolist = zipfile.infolist();
        var info = zipfile.getinfo("unzip_test.js")

        assert.equal(namelist[0], "unzip_test.js");

        assert.equal(infolist.length, 1);
        assert.equal(infolist[0].file_size, fs.stat(path.join(__dirname, "unzip_test.js")).size);
        assert.equal(infolist[0].filename, fs.stat(path.join(__dirname, "unzip_test.js")).name);
        assert.equal(infolist[0].password, false);
        assert.equal(infolist[0].compress_type, "Deflate");

        assert.equal(infolist[0].password, info.password);
        assert.equal(infolist[0].compress_type, info.compress_type);
        assert.equal(infolist[0].filename, info.filename);
        assert.equal(infolist[0].file_size, info.file_size);
        assert.equal(infolist[0].date.getTime(), info.date.getTime());

        assert.notLessThan(new Date().getTime() - info.date.getTime(), 0);
        assert.lessThan(new Date().getTime() - info.date.getTime(), 60000);
    });

    it("open zip file & read", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        var buf = zipfile.read('unzip_test.js');

        file = fs.openFile(path.join(__dirname, 'unzip_test.js'));
        var buf1 = file.readAll();

        assert.deepEqual(buf, buf1);
    });

    it("open zip buffer & read", () => {
        file = fs.openFile(path.join(__dirname, 'unzip_test.zip' + vmid));
        var buf = file.readAll();
        file.close();

        zipfile = zip.open(buf);
        buf = zipfile.read('unzip_test.js');

        file = fs.openFile(path.join(__dirname, 'unzip_test.js'));
        var buf1 = file.readAll();

        assert.deepEqual(buf, buf1);
    });

    it("open zip stream & read", () => {
        file = fs.openFile(path.join(__dirname, 'unzip_test.zip' + vmid));
        var buf = file.readAll();
        ms.write(buf);
        ms.rewind();
        file.close();

        zipfile = zip.open(ms);
        buf = zipfile.read('unzip_test.js');

        file = fs.openFile(path.join(__dirname, 'unzip_test.js'));
        var buf1 = file.readAll();

        assert.deepEqual(buf, buf1);
    });

    it("write buffer & stream to zip", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid), "w");
        file = fs.openFile(path.join(__dirname, 'unzip_test.js'));

        var buf = file.readAll();
        zipfile.write(buf, 'unzip_test.js.bak');

        var rep = new http.Request();
        rep.write(Buffer.from("0123456789"));

        rep.sendTo(ms1);
        ms1.rewind();

        zipfile.write(ms1, "http.rq");
        zipfile.close();

        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        var buf2 = zipfile.read('unzip_test.js.bak');

        assert.deepEqual(buf, buf2);

        buf2 = zipfile.read('http.rq');
        assert.equal(buf2.toString(), 'GET / HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
    });

    it("readall in zip", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        var datas = zipfile.readAll();

        file = fs.openFile(path.join(__dirname, 'unzip_test.js'));
        var buf = file.readAll();

        assert.equal(datas[0].data.length, buf.length);
        assert.equal(datas[0].data.toString(), buf.toString());
        assert.equal(datas[0].file_size, buf.length);
        assert.equal(datas[0].filename, "unzip_test.js.bak");

        ms1.rewind();
        buf = ms1.read();
        assert.deepEqual(datas[1].data, buf);
        assert.equal(datas[1].file_size, buf.length);
        assert.equal(datas[1].filename, "http.rq");
    });

    it("extract zip to file", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        zipfile.extract('unzip_test.js.bak', path.join(__dirname, 'unzip_test.js.extract'));

        file = fs.openFile(path.join(__dirname, 'unzip_test.js.extract'));
        var buf = file.readAll();
        file.close();
        file = fs.openFile(path.join(__dirname, 'unzip_test.js'));
        var buf1 = file.readAll();

        assert.deepEqual(buf, buf1);
    });

    it("extract zip to stream", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        var stream = new io.MemoryStream();
        zipfile.extract('unzip_test.js.bak', stream);

        stream.rewind();
        file = fs.openFile(path.join(__dirname, 'unzip_test.js'));
        var buf = file.readAll();
        var buf1 = stream.read();

        assert.deepEqual(buf, buf1);
    });

    it("extract all in zip", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));

        zipfile.extractAll(pathname);

        efile1 = pathname + '/unzip_test.js.bak';
        efile2 = pathname + '/http.rq';

        assert.equal(fs.exists(efile1), true);
        assert.equal(fs.exists(efile2), true);

        assert.equal(fs.readTextFile(efile1), fs.readTextFile(path.join(__dirname, 'unzip_test.js')));
        assert.equal(fs.readTextFile(efile2), 'GET / HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
    });

    it("write file append to zip", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid), 'a');
        var buf = new Buffer('hello world');
        zipfile.write(buf, 'buf.txt');
        zipfile.close();

        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        assert.equal(zipfile.read('buf.txt').toString(), 'hello world');
    });

    it("read and write with password", () => {
        var password = "1234";
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid), 'a');
        var buf = new Buffer('password test');
        zipfile.write(buf, 'password.txt', password);
        zipfile.close();

        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid));
        assert.equal(zipfile.read('password.txt', password).toString(), 'password test');
    })

    it("zip with codec", () => {
        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid), 'w', "gbk");
        var buf = new Buffer('codec test');
        zipfile.write(buf, '密码.txt');
        zipfile.close();

        zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' + vmid), "r", "gbk");
        assert.equal('密码.txt', zipfile.namelist()[0]);
        assert.equal('密码.txt', zipfile.infolist()[0].filename);
        assert.equal('密码.txt', zipfile.getinfo('密码.txt').filename);
        assert.equal(zipfile.read('密码.txt').toString(), 'codec test');
        assert.equal(zipfile.readAll()[0].filename, '密码.txt');

        zipfile.extractAll(pathname);
        efile3 = pathname + '/密码.txt';
        assert.equal(fs.exists(efile3), true);
    })

    it("bugfix: read from empty file", () => {
        zip.open(new io.MemoryStream());
    });

    it("long entry name", () => {
        var archive = path.join(__dirname, 'unzip_test_longname.zip' + vmid);
        var name = 'a'.repeat(300) + '.txt';

        var zf = zip.open(archive, 'w');
        zf.write(new Buffer('long name'), name);
        zf.close();

        zf = zip.open(archive);
        assert.equal(zf.namelist()[0], name);
        assert.equal(zf.infolist()[0].filename, name);
        assert.equal(zf.getinfo(name).filename, name);
        assert.equal(zf.read(name).toString(), 'long name');
        zf.close();

        fs.unlink(archive);
    });

    it("empty entry", () => {
        var archive = path.join(__dirname, 'unzip_test_emptyentry.zip' + vmid);

        var zf = zip.open(archive, 'w');
        zf.write(new Buffer(0), 'empty.txt');
        zf.close();

        zf = zip.open(archive);
        assert.equal(zf.read('empty.txt').length, 0);
        assert.equal(zf.readAll()[0].data.length, 0);
        zf.close();

        fs.unlink(archive);
    });

    it("lookup entries after the first one", () => {
        var archive = path.join(__dirname, 'unzip_test_lookup.zip' + vmid);

        var zf = zip.open(archive, 'w');
        for (var i = 0; i < 50; i++)
            zf.write(new Buffer('data-' + i), 'f' + i + '.txt');
        zf.close();

        zf = zip.open(archive);
        // each read locates an entry by name; the last one used to cost a
        // full central-directory scan per lookup
        for (var i = 49; i >= 0; i--)
            assert.equal(zf.read('f' + i + '.txt').toString(), 'data-' + i);
        assert.equal(zf.getinfo('f49.txt').file_size, 'data-49'.length);
        zf.close();

        fs.unlink(archive);
    });

    it("mounted archive reads past the entry cache", () => {
        // more entries than the decompressed-entry LRU holds, so the first
        // ones are evicted and must be read from the pinned archive again
        var total = 1500;
        var mount = '/unzip_test_cache.zip' + vmid;
        var ms = new io.MemoryStream();
        var zf = zip.open(ms, 'w');

        for (var i = 0; i < total; i++)
            zf.write(new Buffer('v' + i), 'f' + i + '.txt');
        zf.close();

        ms.rewind();
        fs.setZipFS(mount, ms.readAll());

        try {
            for (var i = 0; i < total; i++)
                assert.equal(fs.readTextFile(mount + '$/f' + i + '.txt'), 'v' + i);

            assert.equal(fs.readTextFile(mount + '$/f0.txt'), 'v0');
            assert.equal(fs.stat(mount + '$/f0.txt').size, 'v0'.length);
        } finally {
            fs.clearZipFS(mount);
            ms.close();
        }
    });
});

