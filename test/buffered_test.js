var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

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
            try {
                while (true) {
                    var c = s.accept();
                    var f = fs.openFile(path.join(__dirname, "test0000" + base_port));
                    f.copyTo(c);
                    f.close();
                    c.close();
                }
            } catch (e) { };
        }

        ss = new net.Socket();
        console.log(ss);
        ss.bind(8182 + base_port);
        ss.listen();
        coroutine.start(accept1, ss);

        for (var i = 3; i < 100000; i *= 3) {
            var conn = new net.Socket();
            conn.connect(8182 + base_port, '127.0.0.1');
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

        r.charset = "EUC-JP";

        f.rewind();
        f.truncate(0);
        r.writeText("我是好人");
        r.writeLine("哈哈哈");
        f.rewind();
        assert.equal(f.readAll().toString("EUC-JP"), "我是好人哈哈哈\r\n");

        f.rewind();
        assert.equal(r.readText(8), "我是好人");
        assert.equal(r.readLine(), "哈哈哈");

        f.close();
    });

    it("write return value validation", () => {
        var tempFile = path.join(__dirname, 'buffered_write_test' + base_port);

        try {
            var f = fs.openFile(tempFile, 'w+');
            var buffered = new io.BufferedStream(f);

            // Test writeText return value
            var testText = 'Hello, world!';
            var result = buffered.writeText(testText);
            assert.equal(result, 13);

            // Test writeLine return value
            var testLine = 'Test Line';
            result = buffered.writeLine(testLine);
            assert.equal(result, 10); // Default EOL is \n unless EOL is explicitly set

            // Verify the content was written correctly
            f.rewind();
            var content = f.readAll().toString();
            assert.ok(content.includes(testText));
            assert.ok(content.includes(testLine));

            f.close();
        } finally {
            try {
                fs.unlink(tempFile);
            } catch (e) { }
        }
    });

    describe("stream close event", () => {
        it("EOF autoDestroy triggers close", () => {
            var tmpFile = path.join(__dirname, '_test_close_buf_' + coroutine.vmid);
            fs.writeFile(tmpFile, 'hello world');
            try {
                var f = fs.openFile(tmpFile);
                var bs = new io.BufferedStream(f);
                var events = [];
                var done = new coroutine.Event();
                bs.on('data', () => { events.push('data'); });
                bs.on('end', () => { events.push('end'); });
                bs.on('close', () => { events.push('close'); done.set(); });
                done.wait();
                assert.ok(events.indexOf('end') >= 0);
                assert.ok(events.indexOf('close') >= 0);
            } finally {
                try { fs.unlink(tmpFile); } catch (e) { }
            }
        });

        it("destroy() triggers close", () => {
            var tmpFile = path.join(__dirname, '_test_close_buf2_' + coroutine.vmid);
            fs.writeFile(tmpFile, 'hello world');
            try {
                var f = fs.openFile(tmpFile);
                var bs = new io.BufferedStream(f);
                var done = new coroutine.Event();
                var closed = false;
                bs.on('close', () => { closed = true; done.set(); });
                bs.destroy();
                done.wait();
                assert.strictEqual(closed, true);
            } finally {
                try { fs.unlink(tmpFile); } catch (e) { }
            }
        });
    });
});

