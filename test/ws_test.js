var test = require("test");
test.setup();

var ws = require('ws');
var io = require('io');
var http = require('http');
var mq = require('mq');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

describe('ws', () => {
    var ss = [];

    after(() => {
        ss.forEach((s) => {
            s.close();
        });
    });

    describe('Message', () => {
        function load_msg(data) {
            var ms = new io.MemoryStream();
            ms.write(data);
            ms.rewind();

            var msg = new ws.Message();
            msg.readFrom(ms);

            return {
                masked: msg.masked,
                type: msg.type,
                compress: msg.compress,
                data: msg.body.readAll().toString()
            };
        }

        describe("readFrom", () => {
            it("simple", () => {
                assert.deepEqual(load_msg([0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f]), {
                    "masked": false,
                    "type": ws.TEXT,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("mask", () => {
                assert.deepEqual(load_msg([0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
                    "masked": true,
                    "type": ws.TEXT,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("ping", () => {
                assert.deepEqual(load_msg([0x89, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f]), {
                    "masked": false,
                    "type": ws.PING,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("pong", () => {
                assert.deepEqual(load_msg([0x8a, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
                    "masked": true,
                    "type": ws.PONG,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("split", () => {
                assert.deepEqual(load_msg([0x01, 0x03, 0x48, 0x65, 0x6c, 0x80, 0x02, 0x6c, 0x6f]), {
                    "masked": false,
                    "type": ws.TEXT,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x07, 0xf2, 0x48, 0xcd, 0xc9, 0xc9, 0x07, 0x00]), {
                    "masked": false,
                    "type": ws.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("split compress", () => {
                assert.deepEqual(load_msg([0x41, 0x03, 0xf2, 0x48, 0xcd, 0x80, 0x04, 0xc9, 0xc9, 0x07, 0x00]), {
                    "masked": false,
                    "type": ws.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("no compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x0b, 0x00, 0x05, 0x00, 0xfa, 0xff, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00]), {
                    "masked": false,
                    "type": ws.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("BFINAL compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x08, 0xf3, 0x48, 0xcd, 0xc9, 0xc9, 0x07, 0x00, 0x00]), {
                    "masked": false,
                    "type": ws.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("2 blocks compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x0d, 0xf2, 0x48, 0x05, 0x00, 0x00, 0x00, 0xff, 0xff, 0xca, 0xc9, 0xc9, 0x07, 0x00]), {
                    "masked": false,
                    "type": ws.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("binary", () => {
                var s = "";
                for (var i = 0; i < 20; i++)
                    s += "0123456789";

                var buf = new Buffer([0x82, 0x7e, 0, 0]);
                buf.writeUInt16BE(s.length, 2);
                buf.append(s);

                assert.deepEqual(load_msg(buf), {
                    "masked": false,
                    "type": ws.BINARY,
                    "compress": false,
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
                    "compress": false,
                    "data": s
                });
            });

            it("throw", () => {
                assert.throws(() => {
                    load_msg([0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c])
                });

                assert.throws(() => {
                    load_msg([0x01, 0x03, 0x48, 0x65, 0x6c]);
                });
            });
        });

        it("sendTo", () => {
            function test_msg(n, masked, compress) {
                var msg = new ws.Message();
                msg.type = ws.TEXT;
                msg.masked = masked;
                msg.compress = compress;

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

            function test_msg_1(n) {
                test_msg(n, false, false);
                test_msg(n, true, false);
                test_msg(n, false, true);
                test_msg(n, true, true);
            }

            test_msg_1(10);
            test_msg_1(100);
            test_msg_1(125);
            test_msg_1(126);
            test_msg_1(65535);
            test_msg_1(65536);
        });
    });

    describe('WebSocketHandler', () => {
        function connect(compress) {
            var headers = {
                "Upgrade": "websocket",
                "Connection": "Upgrade",
                "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
                "Sec-WebSocket-Version": "13"
            };

            if (compress)
                headers["Sec-WebSocket-Extensions"] = "permessage-deflate";

            var rep = http.get("http://127.0.0.1:" + (8813 + base_port) + "/ws", {
                headers
            });

            assert.equal(rep.firstHeader("Sec-WebSocket-Accept"), "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
            assert.equal(rep.firstHeader("Upgrade"), "websocket");
            if (compress)
                assert.equal(rep.firstHeader("Sec-WebSocket-Extensions"), "permessage-deflate");
            assert.equal(rep.statusCode, 101);
            assert.equal(rep.upgrade, true);
            return rep.stream;
        }

        function test_msg(s, n, compress) {
            var msg = new ws.Message();
            msg.type = ws.TEXT;
            msg.compress = compress;
            msg.masked = true;

            var buf = new Buffer(n);
            for (var i = 0; i < n; i++) {
                buf[i] = (i % 10) + 0x30;
            }

            msg.body.write(buf);

            msg.sendTo(s);

            var msg = new ws.Message();
            msg.readFrom(s);

            if (compress)
                assert.isTrue(msg.compress);

            assert.equal(msg.body.readAll().toString(), buf.toString());
        }

        it("server", () => {
            var httpd = new http.Server(8813 + base_port, {
                "/ws": ws.upgrade((s) => {
                    s.onmessage = function (msg) {
                        if (msg.data === "Going Away")
                            msg.stream.close();
                        else if (msg.data === "close")
                            this.close(3000, "remote");
                        else
                            this.send(msg.data);
                    };
                })
            });
            ss.push(httpd.socket);
            httpd.run(() => {});
        });

        describe("handshake", () => {
            it("missing Upgrade header.", () => {
                var rep = http.get("http://127.0.0.1:" + (8813 + base_port) + "/ws", {
                    headers: {
                        "Connection": "Upgrade",
                        "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
                        "Sec-WebSocket-Version": "13"
                    }
                });

                assert.equal(rep.statusCode, 500);
            });

            it("invalid connection header.", () => {
                var rep = http.get("http://127.0.0.1:" + (8813 + base_port) + "/ws", {
                    headers: {
                        "Upgrade": "websocket",
                        "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
                        "Sec-WebSocket-Version": "13"
                    }
                });

                assert.equal(rep.statusCode, 500);
            });

            it("missing Sec-WebSocket-Key header.", () => {
                var rep = http.get("http://127.0.0.1:" + (8813 + base_port) + "/ws", {
                    headers: {
                        "Upgrade": "websocket",
                        "Connection": "Upgrade",
                        "Sec-WebSocket-Version": "13"
                    }
                });

                assert.equal(rep.statusCode, 500);
            });

            it("missing Sec-WebSocket-Version header.", () => {
                var rep = http.get("http://127.0.0.1:" + (8813 + base_port) + "/ws", {
                    headers: {
                        "Upgrade": "websocket",
                        "Connection": "Upgrade",
                        "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ=="
                    }
                });

                assert.equal(rep.statusCode, 500);
            });

            it("support Sec-WebSocket-Extensions header.", () => {
                var rep = http.get("http://127.0.0.1:" + (8813 + base_port) + "/ws", {
                    headers: {
                        "Upgrade": "websocket",
                        "Connection": "Upgrade",
                        "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
                        "Sec-WebSocket-Version": "13",
                        "Sec-WebSocket-Extensions": "permessage-deflate"
                    }
                });

                assert.equal(rep.statusCode, 101);
                assert.equal(rep.firstHeader("Sec-WebSocket-Extensions"), "permessage-deflate");

                var rep = http.get("http://127.0.0.1:" + (8813 + base_port) + "/ws", {
                    headers: {
                        "Upgrade": "websocket",
                        "Connection": "Upgrade",
                        "Sec-WebSocket-Key": "dGhlIHNhbXBsZSBub25jZQ==",
                        "Sec-WebSocket-Version": "13",
                        "Sec-WebSocket-Extensions": "permessage-deflate; client_max_window_bits"
                    }
                });

                assert.equal(rep.statusCode, 101);
                assert.equal(rep.firstHeader("Sec-WebSocket-Extensions"), "permessage-deflate");
            });
        });

        it("echo", () => {
            var s = connect();

            test_msg(s, 10, false);
            test_msg(s, 100, false);
            test_msg(s, 125, false);
            test_msg(s, 126, false);
            test_msg(s, 65535, false);
            test_msg(s, 65536, false);

            s.stream.close();
        });

        xit("echo compress", () => {
            var s = connect(true);

            test_msg(s, 10, true);
            test_msg(s, 100, true);
            test_msg(s, 125, true);
            test_msg(s, 126, true);
            test_msg(s, 65535, true);
            test_msg(s, 65536, true);

            s.stream.close();
        });

        it("ping", () => {
            var s = connect();

            var body = "hello";
            var msg = new ws.Message();
            msg.type = ws.PING;
            msg.body.write(body);
            msg.sendTo(s);

            var msg = new ws.Message();
            msg.readFrom(s);

            assert.equal(msg.type, ws.PONG);
            assert.equal(msg.body.readAll().toString(), body);

            s.close();
        });


        it("close", () => {
            var s = connect();

            var msg = new ws.Message();
            msg.type = ws.CLOSE;

            msg.sendTo(s);

            var msg = new ws.Message();
            var closed = false;

            try {
                msg.readFrom(s);
            } catch (e) {
                closed = true;
            }

            if (!closed)
                assert.equal(msg.type, ws.CLOSE);

            assert.throws(() => {
                msg.readFrom(s);
            });

            s.close();
        });

        it("non-control opcode", () => {
            var s = connect();

            var body = "hello";
            var msg = new ws.Message();
            msg.type = 5;
            msg.sendTo(s);

            msg = new ws.Message();
            msg.type = ws.PING;
            msg.body.write(body);
            msg.sendTo(s);

            msg = new ws.Message();
            msg.readFrom(s);

            assert.equal(msg.type, ws.PONG);
            assert.equal(msg.body.readAll().toString(), body);

            s.close();
        });

        it("drop other type message", () => {
            var s = connect();

            var body = "hello";
            var msg = new ws.Message();
            msg.type = ws.PONG;
            msg.sendTo(s);

            msg = new ws.Message();
            msg.type = ws.PING;
            msg.body.write(body);
            msg.sendTo(s);

            msg = new ws.Message();
            msg.readFrom(s);

            assert.equal(msg.type, ws.PONG);
            assert.equal(msg.body.readAll().toString(), body);

            s.close();
        });

    });

    describe('WebSocket', () => {
        it("server", () => {
            var httpd = new http.Server(8814 + base_port, {
                "/ws": ws.upgrade((s, req) => {
                    assert.equal(req.firstHeader("upgrade"), "websocket");
                    s.onmessage = function (msg) {
                        assert.isTrue(msg.compress);
                        if (msg.data === "Going Away")
                            msg.stream.close();
                        else if (msg.data === "close")
                            this.close(3000, "remote");
                        else
                            this.send(msg.data);
                    };
                })
            });
            ss.push(httpd.socket);
            httpd.run(() => {});
        });

        it('init property', () => {
            var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            assert.equal(s.url, "ws://127.0.0.1:" + (8814 + base_port) + "/ws");
            assert.equal(s.protocol, "test");
            // assert.equal(s.readyState, ws.CONNECTING);

            s.onopen = () => {
                s.close();
            };
        });

        it('onopen', () => {
            var t = false;
            var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                t = true;
            };

            // assert.equal(s.readyState, ws.CONNECTING);
            assert.isFalse(t);

            for (var i = 0; i < 1000 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(t);
            assert.equal(s.readyState, ws.OPEN);

            s.close();
            s.close();
        });

        it('send/onmessage', () => {
            var t = false;
            var msg;
            var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.send('123');
            };

            s.onmessage = (m) => {
                assert.isTrue(m.compress);
                msg = m;
                t = true;
            };

            for (var i = 0; i < 1000 && !t; i++)
                coroutine.sleep(1);

            assert.equal(msg.data, '123');

            t = false;
            s.send(new Buffer('456'));

            for (var i = 0; i < 1000 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(Buffer.isBuffer(msg.data));
            assert.equal(msg.data.toString(), '456');

            s.close();
        });

        it('send/on("message")', () => {
            var httpd = new http.Server(8815 + base_port, {
                "/ws": ws.upgrade((s) => {
                    s.on("message", function (msg) {
                        if (msg.data === "Going Away")
                            msg.stream.close();
                        else if (msg.data === "close")
                            this.close(3000, "remote");
                        else
                            this.send(msg.data);
                    });
                })
            });
            ss.push(httpd.socket);
            httpd.run(() => {});

            var t = false;
            var msg;
            var s = new ws.Socket("ws://127.0.0.1:" + (8815 + base_port) + "/ws", "test");
            s.on("open", () => {
                s.send('123');
            });

            s.on("message", (m) => {
                msg = m;
                t = true;
            });

            for (var i = 0; i < 1000 && !t; i++)
                coroutine.sleep(1);

            assert.equal(msg.data, '123');

            t = false;
            s.send(new Buffer('456'));

            for (var i = 0; i < 1000 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(Buffer.isBuffer(msg.data));
            assert.equal(msg.data.toString(), '456');

            s.close();
        });

        it('close/onclose', () => {
            var tc = false;
            var msg;
            var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.close(1000, '123');
            };

            s.onclose = (e) => {
                assert.equal(e.code, 1000);
                assert.equal(e.reason, "123");
                tc = true;
            };

            for (var i = 0; i < 1000 && !tc; i++)
                coroutine.sleep(1);

            assert.isTrue(tc);
            assert.equal(s.readyState, ws.CLOSED);
        });

        it('remote close', () => {
            var tc = false;
            var msg;
            var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.send('close');
            };

            s.onclose = (e) => {
                // assert.equal(e.code, 3000);
                // assert.equal(e.reason, "remote");
                tc = true;
            };

            for (var i = 0; i < 1000 && !tc; i++)
                coroutine.sleep(1);

            assert.isTrue(tc);
            assert.equal(s.readyState, ws.CLOSED);
        });

        it('Going Away', () => {
            var te = false;
            var tc = false;
            var msg;
            var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.send('Going Away');
            };

            s.onerror = (e) => {
                assert.equal(e.code, 1001);
                te = true;
            };

            s.onclose = (e) => {
                assert.equal(e.code, 1006);
                tc = true;
            };

            for (var i = 0; i < 1000 && !tc; i++)
                coroutine.sleep(1);

            assert.isTrue(te);
            assert.isTrue(tc);
            assert.equal(s.readyState, ws.CLOSED);
        });

        it('keep the sequence', () => {
            var ev = new coroutine.Event();
            var cnt = 1000;
            var n = 0;

            var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                console.time('ws');
                for (var i = 0; i < cnt; i++)
                    s.send(i);
            };

            s.onmessage = (msg) => {
                assert.equal(msg.data, n);
                n++;
                if (n == cnt)
                    ev.set();
            };

            ev.wait();
            console.timeEnd('ws');
        });

        describe('onerror', () => {
            it("open", () => {
                var t = false;
                var te = false;
                var tc = false;
                var s = new ws.Socket("ws://127.0.0.1:" + (18814 + base_port) + "/ws", "test");

                // assert.equal(s.readyState, ws.CONNECTING);
                assert.isFalse(t);

                s.onopen = () => {
                    t = true;
                };

                s.onerror = (e) => {
                    assert.equal(e.code, 1002);
                    te = true;
                };

                s.onclose = (e) => {
                    assert.equal(e.code, 1006);
                    tc = true;
                };

                for (var i = 0; i < 1000 && !tc; i++)
                    coroutine.sleep(1);

                assert.isFalse(t);
                assert.isTrue(te);
                assert.isTrue(tc);
                assert.equal(s.readyState, ws.CLOSED);
            });

            it("entry/handshake", () => {
                var t = false;
                var te = false;
                var tc = false;
                var s = new ws.Socket("ws://127.0.0.1:" + (8814 + base_port) + "/ws1", "test");

                // assert.equal(s.readyState, ws.CONNECTING);
                assert.isFalse(t);

                s.onopen = () => {
                    t = true;
                };

                s.onerror = (e) => {
                    assert.equal(e.code, 1002);
                    te = true;
                };

                s.onclose = (e) => {
                    assert.equal(e.code, 1006);
                    tc = true;
                };

                for (var i = 0; i < 1000 && !tc; i++)
                    coroutine.sleep(1);

                assert.isFalse(t);
                assert.isTrue(te);
                assert.isTrue(tc);
                assert.equal(s.readyState, ws.CLOSED);
            });
        });

    });
});

repl && test.run(console.DEBUG);