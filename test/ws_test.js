var test = require("test");
test.setup();

var ws = require('ws');
var io = require('io');
var http = require('http');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

describe('ws', () => {
    var ss = [];

    after(() => {
        ss.forEach((s) => {
            s.close();
        });
    });

    function load_msg(data) {
        var ms = new io.MemoryStream();
        ms.write(data);
        ms.rewind();

        var msg = new ws.Message();
        msg.readFrom(ms);

        return {
            masked: msg.masked,
            type: msg.type,
            data: msg.body.readAll().toString()
        };
    }

    it("readFrom", () => {
        assert.deepEqual(load_msg([0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f]), {
            "masked": false,
            "type": ws.TEXT,
            "data": "Hello"
        });

        assert.deepEqual(load_msg([0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
            "masked": true,
            "type": ws.TEXT,
            "data": "Hello"
        });

        assert.deepEqual(load_msg([0x89, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f]), {
            "masked": false,
            "type": ws.PING,
            "data": "Hello"
        });

        assert.deepEqual(load_msg([0x8a, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
            "masked": true,
            "type": ws.PONG,
            "data": "Hello"
        });

        assert.deepEqual(load_msg([0x01, 0x03, 0x48, 0x65, 0x6c, 0x80, 0x02, 0x6c, 0x6f]), {
            "masked": false,
            "type": ws.TEXT,
            "data": "Hello"
        });

        var s = "";
        for (var i = 0; i < 20; i++)
            s += "0123456789";

        var buf = new Buffer([0x82, 0x7e, 0, 0]);
        buf.writeUInt16BE(s.length, 2);
        buf.append(s);

        assert.deepEqual(load_msg(buf), {
            "masked": false,
            "type": ws.BINARY,
            "data": s
        });

        var s = "";
        for (var i = 0; i < 20000; i++)
            s += "0123456789";

        var buf = new Buffer([0x82, 0x7f, 0, 0, 0, 0, 0, 0, 0, 0]);
        buf.writeInt64BE(s.length, 2);
        buf.append(s);

        assert.deepEqual(load_msg(buf), {
            "masked": false,
            "type": ws.BINARY,
            "data": s
        });

        assert.throws(() => {
            load_msg([0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c])
        });

        assert.throws(() => {
            load_msg([0x01, 0x03, 0x48, 0x65, 0x6c]);
        });
    });

    it("sendTo", () => {
        function test_msg(n, masked) {
            var msg = new ws.Message();
            msg.type = ws.TEXT;
            msg.masked = masked;

            var buf = new Buffer(n);
            for (var i = 0; i < n; i++) {
                buf[i] = (i % 10) + 0x30;
            }

            msg.body.write(buf);
            var ms = new io.MemoryStream();

            msg.sendTo(ms);
            ms.rewind();

            var msg = new ws.Message();
            msg.readFrom(ms);

            assert.equal(msg.body.readAll().toString(), buf.toString());
        }

        test_msg(10);
        test_msg(10, true);
        test_msg(100);
        test_msg(100, true);
        test_msg(125);
        test_msg(125, true);
        test_msg(126);
        test_msg(126, true);
        test_msg(65535);
        test_msg(65535, true);
        test_msg(65536);
        test_msg(65536, true);
    });

    it("handshake", () => {
        function test_msg(n, masked) {
            var msg = new ws.Message();
            msg.type = ws.TEXT;
            msg.masked = masked;

            var buf = new Buffer(n);
            for (var i = 0; i < n; i++) {
                buf[i] = (i % 10) + 0x30;
            }

            msg.body.write(buf);

            msg.sendTo(rep.stream);

            var msg = new ws.Message();
            msg.readFrom(rep.stream);

            assert.equal(msg.body.readAll().toString(), buf.toString());
        }

        var httpd = new http.Server(8810 + base_port, new ws.Handler((v) => {
            v.response.body = v.body;
        }));
        ss.push(httpd.socket);
        httpd.asyncRun();

        var rep = http.get("http://127.0.0.1:" + (8810 + base_port) + "/", {
            "Upgrade": "websocket",
            "Connection": "Upgrade",
            "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
            "Sec-WebSocket-Version": "13"
        });

        assert.equal(rep.firstHeader("Sec-WebSocket-Accept"), "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
        assert.equal(rep.firstHeader("Upgrade"), "websocket");
        assert.equal(rep.status, 101);
        assert.equal(rep.upgrade, true);

        test_msg(10, true);
        test_msg(100, true);
        test_msg(125, true);
        test_msg(126, true);
        test_msg(65535, true);
        test_msg(65536, true);

        var rep = http.get("http://127.0.0.1:" + (8810 + base_port) + "/", {
            "Connection": "Upgrade",
            "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
            "Sec-WebSocket-Version": "13"
        });

        assert.equal(rep.status, 500);

        var rep = http.get("http://127.0.0.1:" + (8810 + base_port) + "/", {
            "Upgrade": "websocket",
            "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
            "Sec-WebSocket-Version": "13"
        });

        assert.equal(rep.status, 500);

        var rep = http.get("http://127.0.0.1:" + (8810 + base_port) + "/", {
            "Upgrade": "websocket",
            "Connection": "Upgrade",
            "Sec-WebSocket-Version": "13"
        });

        assert.equal(rep.status, 500);

        var rep = http.get("http://127.0.0.1:" + (8810 + base_port) + "/", {
            "Upgrade": "websocket",
            "Connection": "Upgrade",
            "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ=="
        });

        assert.equal(rep.status, 500);

        assert.throws(() => {
            test_msg(10);
        });
    });

    it("connect", () => {
        function test_msg(n, masked) {
            var msg = new ws.Message();
            msg.type = ws.TEXT;
            msg.masked = masked;

            var buf = new Buffer(n);
            for (var i = 0; i < n; i++) {
                buf[i] = (i % 10) + 0x30;
            }

            msg.body.write(buf);

            msg.sendTo(s);

            var msg = new ws.Message();
            msg.readFrom(s);

            assert.equal(msg.body.readAll().toString(), buf.toString());
        }

        var s = ws.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

        test_msg(10, true);
        test_msg(100, true);
        test_msg(125, true);
        test_msg(126, true);
        test_msg(65535, true);
        test_msg(65536, true);

    });

    it("ping", () => {
        var s = ws.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

        var body = "hello";
        var msg = new ws.Message();
        msg.type = ws.PING;
        msg.body.write(body);
        msg.sendTo(s);

        var msg = new ws.Message();
        msg.readFrom(s);

        assert.equal(msg.type, ws.PONG);
        assert.equal(msg.body.readAll().toString(), body);
    });

    it("close", () => {
        var s = ws.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

        var msg = new ws.Message();
        msg.type = ws.CLOSE;

        msg.sendTo(s);

        var msg = new ws.Message();

        assert.throws(() => {
            msg.readFrom(s);
        });
    });

    it("non-control opcode", () => {
        var s = ws.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

        var msg = new ws.Message();
        msg.type = 5;
        msg.sendTo(s);

        var msg = new ws.Message();
        assert.throws(() => {
            msg.readFrom(s);
        });
    });

    it("drop other type message", () => {
        var s = ws.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

        var msg = new ws.Message();
        msg.type = ws.PONG;
        msg.sendTo(s);

        var msg = new ws.Message();
        assert.throws(() => {
            msg.readFrom(s);
        });
    });

    it("remote close", () => {
        var httpd = new http.Server(8811 + base_port, new ws.Handler((v) => {
            v.stream.close();
        }));
        ss.push(httpd.socket);
        httpd.asyncRun();

        var s = ws.connect("ws://127.0.0.1:" + (8811 + base_port) + "/");

        var msg = new ws.Message();
        msg.type = ws.TEXT;

        msg.sendTo(s);

        var msg = new ws.Message();
        assert.throws(() => {
            msg.readFrom(s);
        });
    });

    it("onerror", () => {
        var err_500 = 0;

        var hdlr = new ws.Handler((v) => {
            throw new Error("test error");
        });

        hdlr.onerror({
            500: (v) => {
                err_500++;
                console.error(v.lastError);
            }
        });

        var httpd = new http.Server(8812 + base_port, hdlr);
        ss.push(httpd.socket);
        httpd.asyncRun();

        var s = ws.connect("ws://127.0.0.1:" + (8812 + base_port) + "/");

        var msg = new ws.Message();
        msg.type = ws.TEXT;

        msg.sendTo(s);

        for (var i = 0; i < 100 && err_500 == 0; i++)
            coroutine.sleep(1);

        assert.equal(err_500, 1);
    });

    describe('WebSocket', () => {
        it('init property', () => {
            var s = new ws.Socket("ws://127.0.0.1:" + (8810 + base_port) + "/", "test");
            assert.equal(s.url, "ws://127.0.0.1:" + (8810 + base_port) + "/");
            assert.equal(s.protocol, "test");
            assert.equal(s.readyState, ws.CONNECTING);
        });

        it('onopen', () => {
            var t = false;
            var s = new ws.Socket("ws://127.0.0.1:" + (8810 + base_port) + "/", "test");
            s.onopen = () => {
                t = true;
            };

            assert.isFalse(t);
            assert.equal(s.readyState, ws.CONNECTING);

            for (var i = 0; i < 100 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(t);
            assert.equal(s.readyState, ws.OPEN);
        });

        it('onerror', () => {
            var t = false;
            var te = false;
            var s = new ws.Socket("ws://127.0.0.1:" + (18810 + base_port) + "/", "test");

            assert.equal(s.readyState, ws.CONNECTING);
            assert.isFalse(t);

            s.onopen = () => {
                t = true;
            };

            s.onerror = (e) => {
                te = true;
            };

            for (var i = 0; i < 100 && !t && !te; i++)
                coroutine.sleep(1);

            assert.isFalse(t);
            assert.isTrue(te);
            assert.equal(s.readyState, ws.CLOSED);
        });

        it('send/onmessage', () => {
            var t = false;
            var msg;
            var s = new ws.Socket("ws://127.0.0.1:" + (8810 + base_port) + "/", "test");
            s.onopen = () => {
                s.send('123');
            };

            s.onmessage = (m) => {
                msg = m;
                t = true;
            };

            for (var i = 0; i < 100 && !t; i++)
                coroutine.sleep(1);

            assert.equal(msg.data, '123');

            t = false;
            s.send(new Buffer('456'));

            for (var i = 0; i < 100 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(Buffer.isBuffer(msg.data));
            assert.equal(msg.data.toString(), '456');
        });
    });
});

// test.run(console.DEBUG);
