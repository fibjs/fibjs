var test = require("test");
test.setup();

var fs = require('fs');
var path = require('path');
var io = require('io');
var net = require('net');
var mq = require('mq');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

describe("buffered stream", () => {
    var s;
    var f;
    var ss;

    before(() => {
        s = '0123456789\r\n';

        for (var i = 0; i < 13; i++)
            s = s + s;

        var f = fs.openFile(path.join(__dirname, "test0000" + base_port), 'w');
        f.write(s);
        f.close();
    });

    after(() => {
        fs.unlink(path.join(__dirname, "test0000" + base_port));
        ss.close();
    });

    function t_read(f, sz) {
        var r = new io.BufferedStream(f);
        var p = 0;
        var d;

        while (p < s.length) {
            d = r.read(sz);
            if (!d)
                break;
            assert.equal(d.toString(), s.substring(p, p + sz));
            p += sz;
        }
        f.close();
    }

    it("block size", () => {
        for (var i = 3; i < 100000; i *= 3)
            t_read(fs.openFile(path.join(__dirname, "test0000" + base_port)), i);
    });

    it("buffered tcp stream", () => {
        function accept1(s) {
            while (true) {
                var c = s.accept();
                var f = fs.openFile(path.join(__dirname, "test0000" + base_port));
                f.copyTo(c);
                f.close();
                c.close();
            }
        }

        ss = new net.Socket();
        ss.bind(8182 + base_port);
        ss.listen();
        coroutine.start(accept1, ss);

        for (var i = 3; i < 100000; i *= 3) {
            var conn = new net.Socket();
            conn.connect('127.0.0.1', 8182 + base_port);
            t_read(conn, i);
        }
    });

    it("readline", () => {
        f = fs.openFile(path.join(__dirname, "test0000" + base_port));
        var r = new io.BufferedStream(f);
        r.EOL = '\r\n';

        var n = 0;
        var s1;

        while ((s1 = r.readLine()) !== null) {
            assert.equal('0123456789', s1);
            n++;
        }
        assert.equal(8192, n);
        f.close();

        f = fs.openFile(path.join(__dirname, "test0000" + base_port));
        var r = new io.BufferedStream(f);
        r.EOL = '\r\n';

        assert.equal(r.readLine(10), '0123456789');
        assert.throws(() => {
            r.readLine(9);
        });

        f.close();
    });

    it("charset", () => {
        fs.unlink(path.join(__dirname, "test0000" + base_port));

        f = fs.openFile(path.join(__dirname, "test0000" + base_port), "w+");
        var r = new io.BufferedStream(f);
        r.EOL = '\r\n';

        assert.equal(r.charset, "utf-8");

        f.write("哈哈哈\r\n");
        f.rewind();
        assert.equal(r.readLine(), "哈哈哈");

        r.charset = "gbk";

        f.rewind();
        f.truncate(0);
        r.writeText("嘿嘿嘿");
        r.writeLine("哈哈哈");
        f.rewind();
        assert.equal(f.readAll().toString("gbk"), "嘿嘿嘿哈哈哈\r\n");

        f.rewind();
        assert.equal(r.readText(6), "嘿嘿嘿");
        assert.equal(r.readLine(), "哈哈哈");

        f.close();
    });
});

require.main === module && test.run(console.DEBUG);