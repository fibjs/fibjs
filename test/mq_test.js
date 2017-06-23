var test = require("test");
test.setup();

var test_util = require('./test_util');

var mq = require('mq');
var http = require('http');
var net = require('net');
var io = require('io');
var os = require('os');
var util = require('util');
var coroutine = require('coroutine');

var m = new mq.Message();
var htm = new http.Request();
var v = new Buffer('abcd');

var n;

function hdlr1(v) {
    n = n | 1;
}

function hdlr2(v) {
    n = n | 2;
}

function hdlr3(v) {
    n = n | 4;
}

describe("mq", () => {
    it("create Message", () => {
        var m = new mq.Message();
    });

    var ss = [];

    after(() => {
        ss.forEach((s) => {
            s.close();
        });
    });

    describe("function handler", () => {
        var n = 0;

        function fun3(v) {
            n = n | 4;
        }

        function fun2(v) {
            n = n | 2;
            return new mq.Handler(fun3);
        }

        function fun1(v) {
            n = n | 1;
            return fun2;
        }

        var jv = new mq.Handler(fun1);

        it("direct invoke", () => {
            n = 0;
            jv.invoke(v);
            assert.equal(3, n);
        });

        it("recursion invoke", () => {
            n = 0;
            mq.invoke(jv, v);
            assert.equal(7, n);
        });
    });

    describe("chain handler", () => {
        it("chain invoke", () => {
            var chain = new mq.Chain([hdlr1, hdlr2, hdlr3]);

            n = 0;
            chain.invoke(v);
            assert.equal(7, n);
        });

        it("create by Handler", () => {
            var chain = new mq.Handler([hdlr1, hdlr2, hdlr3]);

            n = 0;
            chain.invoke(v);
            assert.equal(7, n);
        });

        it("params", () => {
            function chain_params(v, p1, p2) {
                assert.equal(v.value, '');
                assert.equal(v.params.length, 2);
                assert.equal(v.params[0], "123");
                assert.equal(v.params[1], "b1234");
                assert.equal(p1, "123");
                assert.equal(p2, "b1234");
            }

            var chain1 = new mq.Chain([chain_params, chain_params, chain_params]);

            m.value = '';
            m.params.resize(2);
            m.params[0] = '123';
            m.params[1] = 'b1234';
            mq.invoke(chain1, m);
        });

        it("Message", () => {
            var handler = new mq.Chain([
                (v) => {
                    v.params[0] = {};
                },
                (v) => {
                    assert.isObject(v.params[0]);
                }
            ]);

            var req = new mq.Message();
            req.params.push("aaasssssssssssssss");
            mq.invoke(handler, req);
        });

        it("end chain", () => {
            var n = 0;
            var handler = new mq.Chain([
                (v) => {
                    n = 1;
                },
                (v) => {
                    v.end();
                },
                (v) => {
                    n = 3;
                }
            ]);

            var req = new mq.Message();
            mq.invoke(handler, req);
            assert.equal(1, n);

            var handler1 = new mq.Chain([
                (v) => {
                    n = 2;
                },
                (v) => {
                    v.response.end();
                },
                (v) => {
                    n = 4;
                }
            ]);

            var req = new mq.Message();
            mq.invoke(handler1, req);
            assert.equal(2, n);
        });

        it("memory leak", () => {
            var svr = new net.TcpServer(8888, () => {});
            ss.push(svr.socket);

            GC();
            var no1 = test_util.countObject('Chain');

            svr.handler = new mq.Chain([(v) => {}, (v) => {}]);

            GC();
            assert.equal(no1 + 1, test_util.countObject('Chain'));

            svr.handler = (v) => {};
            svr = undefined;

            GC();
            assert.equal(no1, test_util.countObject('Chain'));
        });
    });

    describe("routing handler", () => {
        function params(v, p1, p2) {
            assert.equal(v.value, '123.a456.html');
            assert.equal(v.params.length, 2);
            assert.equal(v.params[0], "123");
            assert.equal(v.params[1], "a456");
            assert.equal(p1, "123");
            assert.equal(p2, "a456");
            n = 'param: ' + p1 + ',' + p2;
        }

        function params0(v) {
            assert.equal(v.value, '');
            assert.equal(v.params.length, 0);
            n = 'param0';
        }

        function params1(v, p1) {
            assert.equal(v.value, '789');
            assert.equal(v.params.length, 1);
            assert.equal(v.params[0], "789");
            assert.equal(p1, "789");
            n = 'param1: ' + p1;
        }

        function params2(v, p1, p2) {
            assert.equal(v.value, '');
            assert.equal(v.params.length, 2);
            assert.equal(v.params[0], "123");
            assert.equal(v.params[1], "b456");
            assert.equal(p1, "123");
            assert.equal(p2, "b456");
            n = 'param2: ' + p1 + ',' + p2;
        }

        function params3(v, p1, p2) {
            assert.equal(v.value, '123.b456c789.html');
            assert.equal(v.params.length, 2);
            assert.equal(v.params[0], "123");
            assert.equal(v.params[1], "b456c789");
            assert.equal(p1, "123");
            assert.equal(p2, "b456c789");
            n = 'param3: ' + p1 + ',' + p2;
        }

        var r = new mq.Routing({
            '^a$': hdlr1,
            '^c$': hdlr3,
            '^b$': hdlr2,
            '^params/(([0-9]+)\.(([a-z])?[0-9]+)\.html)$': params,
            '^params0/[0-9]+\.html$': params0,
            '^params1/([0-9]+)\.html$': params1,
            '^params2/([0-9]+)\.(([a-z])?[0-9]+)\.html$': params2,
            '^params3/(([0-9]+)\.(([a-z])?[0-9]+([a-z]([0-9]+)))\.html)$': params3
        });

        after(() => {
            r.dispose();
        })

        it("simple path", () => {
            n = 0;
            m.value = 'a';
            mq.invoke(r, m);
            assert.equal(1, n);

            n = 0;
            m.value = 'b';
            mq.invoke(r, m);
            assert.equal(2, n);

            n = 0;
            m.value = 'c';
            mq.invoke(r, m);
            assert.equal(4, n);
        });

        it("regex path", () => {
            m.value = 'params/123.a456.html';
            mq.invoke(r, m);
            assert.equal("param: 123,a456", n);

            m.value = 'params0/999.html';
            mq.invoke(r, m);
            assert.equal("param0", n);

            m.value = 'params1/789.html';
            mq.invoke(r, m);
            assert.equal("param1: 789", n);

            m.value = 'params2/123.b456.html';
            mq.invoke(r, m);
            assert.equal("param2: 123,b456", n);

            m.value = 'params3/123.b456c789.html';
            mq.invoke(r, m);
            assert.equal("param3: 123,b456c789", n);
        });

        it("error path", () => {
            n = 0;
            m.value = 'd';
            assert.throws(() => {
                mq.invoke(r, m);
            });
        });

        it("object param", () => {
            var req = new mq.Message();
            req.params.resize(1);
            req.params[0] = [];

            new mq.Handler(function t(request, d) {
                assert.isArray(d);
                return "ok";
            }).invoke(req);
        });

        it("path to value", () => {
            var r = new mq.Routing({
                "^/api/a$": (v) => {},
                "^/api/a(/.*)$": (v) => {}
            });

            var m = new mq.Message();
            m.value = '/api/a';
            mq.invoke(r, m);
            assert.equal('', m.value);

            m.value = '/api/a/test';
            mq.invoke(r, m);
            assert.equal('/test', m.value);
        });

        it("get/post method", () => {
            var val = 0;
            var r = new mq.Routing();

            r.get({
                "^/a$": (v) => {
                    val = 1
                }
            });

            r.post("^/a$", (v) => {
                val = 2;
            })

            r.all("^/b$", (v) => {
                val = 3;
            })

            htm.method = "GET";
            htm.value = "/a";
            mq.invoke(r, htm);
            assert.equal(val, 1);

            htm.method = "POST";
            htm.value = "/a";
            mq.invoke(r, htm);
            assert.equal(val, 2);

            htm.method = "ANY_METHOD";
            htm.value = "/b";
            mq.invoke(r, htm);
            assert.equal(val, 3);
        });

        describe("order", () => {
            it("map append", () => {
                var r = new mq.Routing({
                    "^/api/a(/.*)$": (v) => {},
                    "^/api/(.*)$": (v) => {}
                });

                var m = new mq.Message();
                m.value = '/api/a/';
                mq.invoke(r, m);
                assert.equal('/', m.value);
            });

            it("append", () => {
                var r = new mq.Routing();

                r.append("^/api/a(/.*)$", (v) => {});
                r.append("^/api/(.*)$", (v) => {});

                var m = new mq.Message();
                m.value = '/api/a/';
                mq.invoke(r, m);
                assert.equal('/', m.value);
            });

            it("append route", () => {
                var r1 = new mq.Routing({
                    "^/api/a(/.*)$": (v) => {},
                    "^/api/(.*)$": (v) => {}
                });

                var r = new mq.Routing();
                r.append(r1);

                var m = new mq.Message();
                m.value = '/api/a/';
                mq.invoke(r, m);
                assert.equal('/', m.value);
            });
        });

        it("memory leak", () => {
            var svr = new net.TcpServer(8890, () => {});
            ss.push(svr.socket);

            GC();
            var no1 = test_util.countObject('Routing');

            svr.handler = new mq.Routing({
                "^/api/a$": (v) => {},
                "^/api/a(/.*)$": (v) => {}
            });

            GC();
            assert.equal(no1 + 1, test_util.countObject('Routing'));

            svr.handler = (v) => {};
            svr = undefined;

            GC();
            assert.equal(no1, test_util.countObject('Routing'));
        });
    });

    it("await", () => {
        var n = 100;

        mq.invoke((r) => {
            var aw = mq.await();

            function delayend() {
                assert.equal(n, 100);
                n = 200;
                aw.end();
            }
            coroutine.start(delayend);

            return aw;
        }, m);
        assert.equal(n, 200);

        n = 300;
        mq.invoke((r) => {
            var aw = mq.await();

            assert.equal(n, 300);
            n = 400;
            aw.end();

            return aw;
        }, m);
        assert.equal(n, 400);
    });

    it("sync(func)", () => {
        var n = 100;

        mq.invoke(util.sync((v, done) => {
            function delayend() {
                assert.equal(n, 100);
                n = 200;
                done();
            }
            setTimeout(delayend, 10);
        }), m);
        assert.equal(n, 200);

        n = 300;
        mq.invoke(util.sync((v, done) => {
            assert.equal(n, 300);
            n = 400;
            done();
        }), m);
        assert.equal(n, 400);
    });
});

repl && test.run(console.DEBUG);