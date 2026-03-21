var { describe, it, xit, after } = require('node:test');
var assert = require('assert');

var io = require('io');
var http = require('http');
var mq = require('mq');
var coroutine = require('coroutine');
var test_util = require('./test_util');

var base_port = coroutine.vmid * 10000;

describe('ws', () => {
    after(test_util.cleanup);

    describe('Message', () => {
        function load_msg(data) {
            var ms = new io.MemoryStream();
            ms.write(data);
            ms.rewind();

            var msg = new WebSocket.Message();
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
                    "type": WebSocket.TEXT,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("mask", () => {
                assert.deepEqual(load_msg([0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
                    "masked": true,
                    "type": WebSocket.TEXT,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("ping", () => {
                assert.deepEqual(load_msg([0x89, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f]), {
                    "masked": false,
                    "type": WebSocket.PING,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("pong", () => {
                assert.deepEqual(load_msg([0x8a, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
                    "masked": true,
                    "type": WebSocket.PONG,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("split", () => {
                assert.deepEqual(load_msg([0x01, 0x03, 0x48, 0x65, 0x6c, 0x80, 0x02, 0x6c, 0x6f]), {
                    "masked": false,
                    "type": WebSocket.TEXT,
                    "compress": false,
                    "data": "Hello"
                });
            });

            it("compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x07, 0xf2, 0x48, 0xcd, 0xc9, 0xc9, 0x07, 0x00]), {
                    "masked": false,
                    "type": WebSocket.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("split compress", () => {
                assert.deepEqual(load_msg([0x41, 0x03, 0xf2, 0x48, 0xcd, 0x80, 0x04, 0xc9, 0xc9, 0x07, 0x00]), {
                    "masked": false,
                    "type": WebSocket.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("no compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x0b, 0x00, 0x05, 0x00, 0xfa, 0xff, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00]), {
                    "masked": false,
                    "type": WebSocket.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("BFINAL compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x08, 0xf3, 0x48, 0xcd, 0xc9, 0xc9, 0x07, 0x00, 0x00]), {
                    "masked": false,
                    "type": WebSocket.TEXT,
                    "compress": true,
                    "data": "Hello"
                });
            });

            it("2 blocks compress", () => {
                assert.deepEqual(load_msg([0xc1, 0x0d, 0xf2, 0x48, 0x05, 0x00, 0x00, 0x00, 0xff, 0xff, 0xca, 0xc9, 0xc9, 0x07, 0x00]), {
                    "masked": false,
                    "type": WebSocket.TEXT,
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
                buf = Buffer.concat([buf, new Buffer(s)]);

                assert.deepEqual(load_msg(buf), {
                    "masked": false,
                    "type": WebSocket.BINARY,
                    "compress": false,
                    "data": s
                });

                var s = "";
                for (var i = 0; i < 20000; i++)
                    s += "0123456789";

                var buf = new Buffer([0x82, 0x7f, 0, 0, 0, 0, 0, 0, 0, 0]);
                buf.writeIntBE(s.length, 4, 6);
                buf = Buffer.concat([buf, new Buffer(s)]);

                assert.deepEqual(load_msg(buf), {
                    "masked": false,
                    "type": WebSocket.BINARY,
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
                var msg = new WebSocket.Message();
                msg.type = WebSocket.TEXT;
                msg.masked = masked;
                msg.compress = compress;

                var buf = Buffer.alloc(n);
                for (var i = 0; i < n; i++) {
                    buf[i] = (i % 10) + 0x30;
                }

                msg.write(buf);
                var ms = new io.MemoryStream();

                msg.sendTo(ms);
                ms.rewind();

                var msg = new WebSocket.Message();
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

        describe("clone", () => {
            it("should clone basic WebSocketMessage", () => {
                var msg = new WebSocket.Message();
                msg.type = WebSocket.TEXT;
                msg.masked = true;
                msg.compress = false;
                msg.write(Buffer.from("Hello WebSocket"));

                var cloned = msg.clone();

                assert.equal(cloned.type, WebSocket.TEXT);
                assert.equal(cloned.masked, true);
                assert.equal(cloned.compress, false);
                cloned.body.rewind();
                assert.equal(cloned.body.readAll().toString(), "Hello WebSocket");
            });

            it("should clone WebSocketMessage with compress", () => {
                var msg = new WebSocket.Message();
                msg.type = WebSocket.BINARY;
                msg.masked = false;
                msg.compress = true;
                msg.write(Buffer.from([0x01, 0x02, 0x03, 0x04]));

                var cloned = msg.clone();

                assert.equal(cloned.type, WebSocket.BINARY);
                assert.equal(cloned.masked, false);
                assert.equal(cloned.compress, true);
                cloned.body.rewind();
                assert.deepEqual(cloned.body.readAll(), Buffer.from([0x01, 0x02, 0x03, 0x04]));
            });

            it("original should not be affected by clone modification", () => {
                var msg = new WebSocket.Message();
                msg.type = WebSocket.TEXT;
                msg.masked = true;
                msg.maxSize = 1024;
                msg.write(Buffer.from("original"));

                var cloned = msg.clone();
                cloned.masked = false;
                cloned.maxSize = 2048;

                assert.equal(msg.masked, true);
                assert.equal(msg.maxSize, 1024);
            });

            it("should clone PING message", () => {
                var msg = new WebSocket.Message();
                msg.type = WebSocket.PING;
                msg.write(Buffer.from("ping data"));

                var cloned = msg.clone();

                assert.equal(cloned.type, WebSocket.PING);
                cloned.body.rewind();
                assert.equal(cloned.body.readAll().toString(), "ping data");
            });

            it("should clone PONG message", () => {
                var msg = new WebSocket.Message();
                msg.type = WebSocket.PONG;
                msg.write(Buffer.from("pong data"));

                var cloned = msg.clone();

                assert.equal(cloned.type, WebSocket.PONG);
                cloned.body.rewind();
                assert.equal(cloned.body.readAll().toString(), "pong data");
            });
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
            var msg = new WebSocket.Message();
            msg.type = WebSocket.TEXT;
            msg.compress = compress;
            msg.masked = true;

            var buf = Buffer.alloc(n);
            for (var i = 0; i < n; i++) {
                buf[i] = (i % 10) + 0x30;
            }

            msg.write(buf);

            msg.sendTo(s);

            var msg = new WebSocket.Message();
            msg.readFrom(s);

            if (compress)
                assert.isTrue(msg.compress);

            assert.equal(msg.body.readAll().toString(), buf.toString());
        }

        it("server", () => {
            var httpd = new http.Server(8813 + base_port, {
                "/ws": WebSocket.upgrade((s) => {
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
            test_util.push(httpd.socket);
            httpd.start();
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
                assert.equal(rep.firstHeader("Sec-WebSocket-Extensions"), null);
                rep.socket.close();

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
                assert.equal(rep.firstHeader("Sec-WebSocket-Extensions"), null);
                rep.socket.close();
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
            var msg = new WebSocket.Message();
            msg.type = WebSocket.PING;
            msg.write(Buffer.from(body));
            msg.sendTo(s);

            var msg = new WebSocket.Message();
            msg.readFrom(s);

            assert.equal(msg.type, WebSocket.PONG);
            assert.equal(msg.body.readAll().toString(), body);

            s.close();
        });


        it("close", () => {
            var s = connect();

            var msg = new WebSocket.Message();
            msg.type = WebSocket.CLOSE;

            msg.sendTo(s);

            var msg = new WebSocket.Message();
            var closed = false;

            try {
                msg.readFrom(s);
            } catch (e) {
                closed = true;
            }

            if (!closed)
                assert.equal(msg.type, WebSocket.CLOSE);

            assert.throws(() => {
                msg.readFrom(s);
            });

            s.close();
        });

        it("non-control opcode", () => {
            var s = connect();

            var body = "hello";
            var msg = new WebSocket.Message();
            msg.type = 5;
            msg.sendTo(s);

            msg = new WebSocket.Message();
            msg.type = WebSocket.PING;
            msg.write(Buffer.from(body));
            msg.sendTo(s);

            msg = new WebSocket.Message();
            msg.readFrom(s);

            assert.equal(msg.type, WebSocket.PONG);
            assert.equal(msg.body.readAll().toString(), body);

            s.close();
        });

        it("drop other type message", () => {
            var s = connect();

            var body = "hello";
            var msg = new WebSocket.Message();
            msg.type = WebSocket.PONG;
            msg.sendTo(s);

            msg = new WebSocket.Message();
            msg.type = WebSocket.PING;
            msg.write(Buffer.from(body));
            msg.sendTo(s);

            msg = new WebSocket.Message();
            msg.readFrom(s);

            assert.equal(msg.type, WebSocket.PONG);
            msg.body.rewind();
            assert.equal(msg.body.readAll().toString(), body);

            s.close();
        });

    });

    describe('WebSocket', () => {
        it("server", () => {
            var httpd = new http.Server(8814 + base_port, [(req) => {
                var apiToken = req.firstHeader("api-token");
                if (apiToken === "valid")
                    return;
                if (apiToken === "invalid") {
                    req.response.statusCode = 403;
                    return req.end();
                }
            }, (req) => {
                var sessionId = req.cookies["sessionId"];
                if (sessionId === "valid")
                    return;
                if (sessionId === "invalid") {
                    req.response.statusCode = 403;
                    return req.end();
                }
            }, {
                "/ws": WebSocket.upgrade({
                    perMessageDeflate: true
                }, (s, req) => {
                    s.onmessage = function (msg) {
                        if (msg.data === "Going Away")
                            msg.stream.close();
                        else if (msg.data === "close")
                            this.close(3000, "remote");
                        else if (msg.data === "many") {
                            for (var i = 0; i < 1025; i++)
                                s.send(Buffer.alloc(1024 * 64));
                        } else
                            this.send(msg.data);
                    };
                }),
                "/set-cookie": (req) => {
                    var json = req.json()
                    req.response.addCookie({
                        name: "sessionId",
                        value: json.value
                    });
                }
            }]);
            test_util.push(httpd.socket);
            httpd.start();
        });

        it('init property', () => {
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            assert.equal(s.url, "ws://127.0.0.1:" + (8814 + base_port) + "/ws");
            assert.equal(s.protocol, "test");
            // assert.equal(s.readyState, WebSocket.CONNECTING);

            s.onopen = () => {
                s.close();
            };
        });

        it('onopen', () => {
            var t = false;
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                t = true;
            };

            // assert.equal(s.readyState, WebSocket.CONNECTING);
            assert.isFalse(t);

            for (var i = 0; i < 2000 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(t);
            assert.equal(s.readyState, WebSocket.OPEN);

            s.close();
            s.close();
        });

        it('send/onmessage', () => {
            var t = false;
            var msg;
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.send('123');
            };

            s.onmessage = (m) => {
                msg = m;
                t = true;
            };

            for (var i = 0; i < 2000 && !t; i++)
                coroutine.sleep(1);

            assert.isFalse(msg.compress);
            assert.equal(msg.data, '123');

            t = false;
            s.send(new Buffer('456'));

            for (var i = 0; i < 2000 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(Buffer.isBuffer(msg.data));
            assert.equal(msg.data.toString(), '456');

            s.close();
        });

        it('header authority', () => {
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", {
                protocol: "test",
                headers: {
                    "api-token": "valid"
                }
            });
            assert.equal(s.url, "ws://127.0.0.1:" + (8814 + base_port) + "/ws");
            assert.equal(s.protocol, "test");

            var opened = false;
            s.onopen = () => {
                opened = true;
                s.close();
            };

            for (var i = 0; i < 2000 && !opened; i++) {
                console.log(opened);
                coroutine.sleep(5);
            }

            assert.equal(s.readyState, WebSocket.CLOSED);
            assert.isTrue(opened);

            var s1 = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", {
                protocol: "test",
                headers: {
                    "api-token": "invalid"
                }
            });
            assert.equal(s1.url, "ws://127.0.0.1:" + (8814 + base_port) + "/ws");
            assert.equal(s1.protocol, "test");
            var errored = false;
            s1.onerror = () => {
                errored = true;
                s1.close();
            };

            for (var i = 0; i < 2000 && !errored; i++)
                coroutine.sleep(5);

            assert.equal(s1.readyState, WebSocket.CLOSED);
            assert.isTrue(errored);
        });

        it("specialize httpClient", () => {
            var hc = new http.Client();
            hc.post("http://127.0.0.1:" + (8814 + base_port) + "/set-cookie", {
                json: {
                    value: "valid"
                }
            })
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", {
                protocol: "test",
                httpClient: hc
            });
            assert.equal(s.url, "ws://127.0.0.1:" + (8814 + base_port) + "/ws");
            assert.equal(s.protocol, "test");

            var opened = false;
            s.onopen = () => {
                opened = true;
                s.close();
            };
            coroutine.sleep(100);
            assert.equal(s.readyState, WebSocket.CLOSED);
            assert.isTrue(opened);

            hc.post("http://127.0.0.1:" + (8814 + base_port) + "/set-cookie", {
                json: {
                    value: "invalid"
                }
            })
            var s1 = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", {
                protocol: "test",
                httpClient: hc
            });
            assert.equal(s1.url, "ws://127.0.0.1:" + (8814 + base_port) + "/ws");
            assert.equal(s1.protocol, "test");
            var opened1 = false;
            s1.onopen = () => {
                opened1 = true;
                s1.close();
            };
            coroutine.sleep(100);
            assert.equal(s1.readyState, WebSocket.CLOSED);
            assert.isFalse(opened1);
        })

        it('many compressed message', () => {
            var cnt = 0;
            var sz = 0;
            var msg;
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.send('many');
            };

            s.onmessage = m => {
                cnt++;
                sz += m.data.length;
            };

            s.onerror = e => {
                console.error(e);
            };

            for (var i = 0; i < 10000 && cnt < 1025; i++)
                coroutine.sleep(1);

            s.close();

            assert.equal(sz, 1024 * 1025 * 64);
        });

        it('perMessageDeflate', () => {
            var t = false;
            var msg;
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", {
                perMessageDeflate: true
            });
            s.onopen = () => {
                s.send('perMessageDeflate');
            };

            s.onmessage = (m) => {
                msg = m;
                t = true;
            };

            for (var i = 0; i < 2000 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(msg.compress);
            assert.equal(msg.data, 'perMessageDeflate');

            s.close();
        });

        it('upgrade perMessageDeflate', () => {
            var httpd = new http.Server(8819 + base_port, {
                "/ws": WebSocket.upgrade({
                    perMessageDeflate: true
                }, (s) => {
                    s.on("message", function (msg) {
                        this.send(msg.data);
                    });
                })
            });
            test_util.push(httpd.socket);
            httpd.start();

            var t = false;
            var msg;
            var s = new WebSocket("ws://127.0.0.1:" + (8819 + base_port) + "/ws");
            s.onopen = () => {
                s.send('perMessageDeflate');
            };

            s.onmessage = (m) => {
                msg = m;
                t = true;
            };

            for (var i = 0; i < 2000 && !t; i++)
                coroutine.sleep(1);

            assert.isFalse(msg.compress);
            assert.equal(msg.data, 'perMessageDeflate');

            s.close();
        });

        it('send/on("message")', () => {
            var httpd = new http.Server(8815 + base_port, {
                "/ws": WebSocket.upgrade((s) => {
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
            test_util.push(httpd.socket);
            httpd.start();

            var t = false;
            var msg;
            var s = new WebSocket("ws://127.0.0.1:" + (8815 + base_port) + "/ws", "test");
            s.on("open", () => {
                s.send('123');
            });

            s.on("message", (m) => {
                msg = m;
                t = true;
            });

            for (var i = 0; i < 2000 && !t; i++)
                coroutine.sleep(1);

            assert.equal(msg.data, '123');

            t = false;
            s.send(new Buffer('456'));

            for (var i = 0; i < 2000 && !t; i++)
                coroutine.sleep(1);

            assert.isTrue(Buffer.isBuffer(msg.data));
            assert.equal(msg.data.toString(), '456');

            s.close();
        });

        it('close/onclose', () => {
            var tc = false;
            var close_code, close_reason;
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.close(1000, '123');
            };

            s.onclose = (e) => {
                close_code = e.code;
                close_reason = e.reason;
                tc = true;
            };

            for (var i = 0; i < 2000 && !tc; i++)
                coroutine.sleep(5);

            assert.isTrue(tc);
            assert.equal(s.readyState, WebSocket.CLOSED);
            assert.equal(close_code, 1000);
            assert.equal(close_reason, "123");
        });

        it('remote close', () => {
            var tc = false;
            var msg;
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.send('close');
            };

            s.onclose = (e) => {
                tc = true;
            };

            for (var i = 0; i < 2000 && !tc; i++)
                coroutine.sleep(5);

            assert.isTrue(tc);
            assert.equal(s.readyState, WebSocket.CLOSED);
        });

        it('Going Away', () => {
            var te = false;
            var tc = false;
            var err_code, close_code;
            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                s.send('Going Away');
            };

            s.onerror = (e) => {
                err_code = e.code;
                te = true;
            };

            s.onclose = (e) => {
                close_code = e.code;
                tc = true;
            };

            for (var i = 0; i < 2000 && !tc; i++)
                coroutine.sleep(5);

            assert.isTrue(te);
            assert.isTrue(tc);
            assert.equal(err_code, 1001);
            assert.equal(close_code, 1006);
            assert.equal(s.readyState, WebSocket.CLOSED);
        });

        it('keep the sequence', () => {
            var ev = new coroutine.Event();
            var cnt = 1000;
            var n = 0;
            var seq_err = null;

            var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws", "test");
            s.onopen = () => {
                console.time('ws');
                for (var i = 0; i < cnt; i++)
                    s.send(i);
            };

            s.onmessage = (msg) => {
                if (msg.data != n && seq_err === null)
                    seq_err = 'out of sequence: expected ' + n + ', got ' + msg.data;
                n++;
                if (n == cnt)
                    ev.set();
            };

            s.onerror = () => { ev.set(); };

            ev.wait();
            console.timeEnd('ws');
            s.close();
            assert.isNull(seq_err, seq_err);
            assert.equal(n, cnt);
        });

        describe('onerror', () => {
            it("open", () => {
                var t = false;
                var te = false;
                var tc = false;
                var err_code, close_code;
                var s = new WebSocket("ws://127.0.0.1:" + (18814 + base_port) + "/ws", "test");

                s.onopen = () => {
                    t = true;
                };

                s.onerror = (e) => {
                    err_code = e.code;
                    te = true;
                };

                s.onclose = (e) => {
                    close_code = e.code;
                    tc = true;
                };

                for (var i = 0; i < 2000 && !tc; i++)
                    coroutine.sleep(5);

                assert.isFalse(t);
                assert.isTrue(te);
                assert.isTrue(tc);
                assert.equal(s.readyState, WebSocket.CLOSED);
                assert.equal(err_code, 1002);
                assert.equal(close_code, 1006);
            });

            it("entry/handshake", () => {
                var t = false;
                var te = false;
                var tc = false;
                var err_code, close_code;
                var s = new WebSocket("ws://127.0.0.1:" + (8814 + base_port) + "/ws1", "test");

                s.onopen = () => {
                    t = true;
                };

                s.onerror = (e) => {
                    err_code = e.code;
                    te = true;
                };

                s.onclose = (e) => {
                    close_code = e.code;
                    tc = true;
                };

                for (var i = 0; i < 2000 && !tc; i++)
                    coroutine.sleep(5);

                assert.isFalse(t);
                assert.isTrue(te);
                assert.isTrue(tc);
                assert.equal(s.readyState, WebSocket.CLOSED);
                assert.equal(err_code, 1002);
                assert.equal(close_code, 1006);
            });
        });

        describe('gc', () => {
            it("gc on close", () => {
                var t = false;

                test_util.gc();

                var no1 = test_util.countObject('WebSocket');
                var httpd = new http.Server(8816 + base_port, {
                    "/ws": WebSocket.upgrade((s, req) => {
                        s.onmessage = e => { }
                    })
                });

                test_util.push(httpd.socket);
                httpd.start();

                assert.equal(test_util.countObject('WebSocket'), no1);

                var s = new WebSocket("ws://127.0.0.1:" + (8816 + base_port) + "/ws", "test");

                s.onopen = () => {
                    t = true;
                };

                s.onmessage = e => { }

                for (var i = 0; i < 2000 && !t; i++)
                    coroutine.sleep(1);

                s.close();
                assert.equal(test_util.countObject('WebSocket'), no1 + 2);

                s = undefined;
                test_util.gc();
                coroutine.sleep(5);
                test_util.gc();
                assert.equal(test_util.countObject('WebSocket'), no1);
            });

            it("not gc in closure", () => {
                var t = false;

                test_util.gc();

                var no1 = test_util.countObject('WebSocket');
                var httpd = new http.Server(8817 + base_port, {
                    "/ws": WebSocket.upgrade((s, req) => {
                        test_util.push(s);
                        s.send(new Date());
                    })
                });

                test_util.push(httpd.socket);
                httpd.start();

                test_util.gc();
                assert.equal(test_util.countObject('WebSocket'), no1);

                var s;

                function test() {
                    s = new WebSocket("ws://127.0.0.1:" + (8817 + base_port) + "/ws", "test");
                    s.onmessage = e => {
                        t = true;
                        s.close();
                    }
                }

                test();

                test_util.gc();
                assert.equal(test_util.countObject('WebSocket'), no1 + 2);
                assert.isTrue(t);

                s = undefined;

                test_util.gc();
                assert.equal(test_util.countObject('WebSocket'), no1 + 1);
            });

            it("gc on event socket", () => {
                var t = false;

                test_util.gc();

                var httpd = new http.Server(8818 + base_port, {
                    "/ws": WebSocket.upgrade((s, req) => { })
                });

                test_util.push(httpd.socket);
                httpd.start();

                var no1 = test_util.countObject('WebSocket');

                var s = new WebSocket("ws://127.0.0.1:" + (8818 + base_port) + "/ws", "test");

                s.onopen = () => {
                    s.close();
                    s = undefined;
                    t = true;
                };

                for (var i = 0; i < 2000 && !t; i++)
                    coroutine.sleep(1);

                test_util.gc();
                assert.equal(test_util.countObject('WebSocket'), no1);
            });
        });
    });
});

