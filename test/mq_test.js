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

var p2r_tests = {
    "/": {
        "/": [
            "/"
        ],
        "/route": null
    },
    "/test": {
        "/test": [
            "/test"
        ],
        "/route": null,
        "/test/route": null,
        "/test/": [
            "/test/"
        ]
    },
    "/test/": {
        "/test": [
            "/test"
        ],
        "/test/": [
            "/test/"
        ],
        "/test//": null
    },
    "/:test": {
        "/route": [
            "/route",
            "route"
        ],
        "/another": [
            "/another",
            "another"
        ],
        "/something/else": null,
        "/route.json": [
            "/route.json",
            "route.json"
        ],
        "/something%2Felse": [
            "/something%2Felse",
            "something%2Felse"
        ],
        "/something%2Felse%2Fmore": [
            "/something%2Felse%2Fmore",
            "something%2Felse%2Fmore"
        ],
        "/;,:@&=+$-_.!~*()": [
            "/;,:@&=+$-_.!~*()",
            ";,:@&=+$-_.!~*()"
        ]
    },
    "/:test?": {
        "/route": [
            "/route",
            "route"
        ],
        "/route/nested": null,
        "/": [
            "/"
        ],
        "//": null
    },
    "/:test?/bar": {
        "/foo/bar": [
            "/foo/bar",
            "foo"
        ]
    },
    "/:test?-bar": {
        "/-bar": [
            "/-bar"
        ],
        "/foo-bar": [
            "/foo-bar",
            "foo"
        ]
    },
    "/:test+": {
        "/": null,
        "/route": [
            "/route",
            "route"
        ],
        "/some/basic/route": [
            "/some/basic/route",
            "some/basic/route"
        ],
        "//": null
    },
    "/:test(\\d+)+": {
        "/abc/456/789": null,
        "/123/456/789": [
            "/123/456/789",
            "123/456/789"
        ]
    },
    "/route.:ext(json|xml)+": {
        "/route": null,
        "/route.json": [
            "/route.json",
            "json"
        ],
        "/route.xml.json": [
            "/route.xml.json",
            "xml.json"
        ],
        "/route.html": null
    },
    "/:test*": {
        "/": [
            "/"
        ],
        "//": null,
        "/route": [
            "/route",
            "route"
        ],
        "/some/basic/route": [
            "/some/basic/route",
            "some/basic/route"
        ]
    },
    "/route.:ext([a-z]+)*": {
        "/route": [
            "/route"
        ],
        "/route.json": [
            "/route.json",
            "json"
        ],
        "/route.json.xml": [
            "/route.json.xml",
            "json.xml"
        ],
        "/route.123": null
    },
    "/:test(\\d+)": {
        "/123": [
            "/123",
            "123"
        ],
        "/abc": null,
        "/123/abc": null
    },
    "/:test(.*)": {
        "/anything/goes/here": [
            "/anything/goes/here",
            "anything/goes/here"
        ],
        "/;,:@&=/+$-_.!/~*()": [
            "/;,:@&=/+$-_.!/~*()",
            ";,:@&=/+$-_.!/~*()"
        ]
    },
    "/:route([a-z]+)": {
        "/abcde": [
            "/abcde",
            "abcde"
        ],
        "/12345": null
    },
    "/:route(this|that)": {
        "/this": [
            "/this",
            "this"
        ],
        "/that": [
            "/that",
            "that"
        ],
        "/foo": null
    },
    "/:path(abc|xyz)*": {
        "/abc": [
            "/abc",
            "abc"
        ],
        "/abc/abc": [
            "/abc/abc",
            "abc/abc"
        ],
        "/xyz/xyz": [
            "/xyz/xyz",
            "xyz/xyz"
        ],
        "/abc/xyz": [
            "/abc/xyz",
            "abc/xyz"
        ],
        "/abc/xyz/abc/xyz": [
            "/abc/xyz/abc/xyz",
            "abc/xyz/abc/xyz"
        ],
        "/xyzxyz": null
    },
    "test": {
        "test": [
            "test"
        ],
        "/test": null
    },
    ":test": {
        "route": [
            "route",
            "route"
        ],
        "/route": null,
        "route/": [
            "route/",
            "route"
        ]
    },
    ":test?": {
        "route": [
            "route",
            "route"
        ],
        "/route": null,
        "": [
            ""
        ],
        "route/foobar": null
    },
    "/test.json": {
        "/test.json": [
            "/test.json"
        ],
        "/route.json": null
    },
    "/:test.json": {
        "/.json": null,
        "/test.json": [
            "/test.json",
            "test"
        ],
        "/route.json": [
            "/route.json",
            "route"
        ],
        "/route.json.json": [
            "/route.json.json",
            "route.json"
        ]
    },
    "/test.:format": {
        "/test.html": [
            "/test.html",
            "html"
        ],
        "/test.hbs.html": null
    },
    "/test.:format.:format": {
        "/test.html": null,
        "/test.hbs.html": [
            "/test.hbs.html",
            "hbs",
            "html"
        ]
    },
    "/test.:format+": {
        "/test.html": [
            "/test.html",
            "html"
        ],
        "/test.hbs.html": [
            "/test.hbs.html",
            "hbs.html"
        ]
    },
    "/test.:format.": {
        "/test.html.": [
            "/test.html.",
            "html"
        ],
        "/test.hbs.html": null
    },
    "/:test.:format": {
        "/route.html": [
            "/route.html",
            "route",
            "html"
        ],
        "/route": null,
        "/route.html.json": [
            "/route.html.json",
            "route.html",
            "json"
        ]
    },
    "/:test.:format?": {
        "/route": [
            "/route",
            "route"
        ],
        "/route.json": [
            "/route.json",
            "route",
            "json"
        ],
        "/route.json.html": [
            "/route.json.html",
            "route.json",
            "html"
        ]
    },
    "/(\\d+)": {
        "/123": [
            "/123",
            "123"
        ],
        "/abc": null,
        "/123/abc": null
    },
    "/(\\d+)?": {
        "/": [
            "/"
        ],
        "/123": [
            "/123",
            "123"
        ]
    },
    "/(.*)": {
        "/": [
            "/"
        ],
        "/route": [
            "/route",
            "route"
        ],
        "/route/nested": [
            "/route/nested",
            "route/nested"
        ]
    },
    "/test(.*)": {
        "/test": [
            "/test"
        ],
        "/test/": [
            "/test/",
            "/"
        ],
        "/test/route": [
            "/test/route",
            "/route",
        ],
        "/test/route/nested": [
            "/test/route/nested",
            "/route/nested"
        ]
    },
    "/route\\(\\\\(\\d+\\\\)\\)": {
        "/route(\\123\\)": [
            "/route(\\123\\)",
            "123\\"
        ]
    },
    "/\\(testing\\)": {
        "/testing": null,
        "/(testing)": [
            "/(testing)"
        ]
    },
    "/.+\\*?=^!:${}[]|": {
        "/.+*?=^!:${}[]|": [
            "/.+*?=^!:${}[]|"
        ]
    },
    "/*": {
        "": null,
        "/": [
            "/"
        ],
        "/foo/bar": [
            "/foo/bar",
            "foo/bar"
        ]
    },
    "/foo/*": {
        "": null,
        "/test": null,
        "/foo": null,
        "/foo/": [
            "/foo/"
        ],
        "/foo/bar": [
            "/foo/bar",
            "bar"
        ]
    },
    "/:foo/*": {
        "": null,
        "/test": null,
        "/foo": null,
        "/foo/": [
            "/foo/",
            "foo"
        ],
        "/foo/bar": [
            "/foo/bar",
            "foo",
            "bar"
        ]
    },
    "/(apple-)?icon-:res(\\d+).png": {
        "/icon-240.png": [
            "/icon-240.png",
            undefined,
            "240"
        ],
        "/apple-icon-240.png": [
            "/apple-icon-240.png",
            "apple-",
            "240"
        ]
    },
    "/:foo/:bar": {
        "/match/route": [
            "/match/route",
            "match",
            "route"
        ]
    },
    "/:remote([\\w-.]+)/:user([\\w-]+)": {
        "/endpoint/user": [
            "/endpoint/user",
            "endpoint",
            "user"
        ],
        "/endpoint/user-name": [
            "/endpoint/user-name",
            "endpoint",
            "user-name"
        ],
        "/foo.bar/user-name": [
            "/foo.bar/user-name",
            "foo.bar",
            "user-name"
        ]
    },
    "/:foo\\?": {
        "/route?": [
            "/route?",
            "route"
        ]
    },
    "/:foo+baz": {
        "/foobaz": [
            "/foobaz",
            "foo"
        ],
        "/foo/barbaz": [
            "/foo/barbaz",
            "foo/bar"
        ],
        "/baz": null
    },
    "/:pre?baz": {
        "/foobaz": [
            "/foobaz",
            "foo"
        ],
        "/baz": [
            "/baz"
        ]
    },
    "/:foo\\(:bar?\\)": {
        "/hello(world)": [
            "/hello(world)",
            "hello",
            "world"
        ],
        "/hello()": [
            "/hello()",
            "hello"
        ]
    },
    "/:postType(video|audio|text)(\\+.+)?": {
        "/video": [
            "/video",
            "video"
        ],
        "/video+test": [
            "/video+test",
            "video",
            "+test"
        ],
        "/video+": null
    },
    "/:foo": {
        "/café": [
            "/café",
            "café"
        ]
    }
};

var p2r_sub_tests = {
    "/test|/hello": {
        "/": null,
        "/test/hello": [
            "/hello"
        ]
    },
    "/test|/": {
        "/": null,
        "/test/": [
            "/"
        ],
        "/test": [
            ""
        ]
    }
};

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
            assert.equal(v.value, 'params0/999.html');
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
            assert.equal('/api/a', m.value);

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

        describe("path to regexp", () => {
            function test_route(p, v) {
                var r = null;
                var rt = new mq.Routing();
                rt.append(p, function () {
                    r = Array.prototype.slice.call(arguments);
                    r[0] = v;
                });

                rt.append("^.*$", () => {});

                var m = new mq.Message();
                m.value = v;

                mq.invoke(rt, m);
                return r;
            }

            function test_one(k) {
                it(k, () => {
                    var cases = p2r_tests[k];
                    for (u in cases)
                        assert.deepEqual(test_route(k, u), cases[u]);
                });
            }

            for (k in p2r_tests)
                test_one(k);

            function test_sub_route(p, p1, v) {
                var r = null;
                var rt = new mq.Routing();
                var sub_routing = {};

                sub_routing[p1] = function (req) {
                    r = Array.prototype.slice.call(arguments);
                    r[0] = req.value;
                };

                sub_routing["^.*$"] = () => {};

                rt.append(p, sub_routing);
                rt.append("^.*$", () => {});

                var m = new mq.Message();
                m.value = v;

                mq.invoke(rt, m);
                return r;
            }

            function test_one_sub(k) {
                it(k, () => {
                    var ks = k.split('|');
                    var cases = p2r_sub_tests[k];
                    for (u in cases)
                        assert.deepEqual(test_sub_route(ks[0], ks[1], u), cases[u]);
                });
            }

            for (k in p2r_sub_tests)
                test_one_sub(k);
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

        it("memory leak 1", () => {
            var no1 = test_util.countObject('Chain');
            var no2 = test_util.countObject('Routing');
            (() => {
                var hdlr = new mq.Chain([
                    () => {
                        return hdlr
                    },
                    new mq.Routing({
                        'a': () => {
                            return hdlr
                        }
                    })
                ]);

                assert.equal(no1 + 1, test_util.countObject('Chain'));
                assert.equal(no2 + 1, test_util.countObject('Routing'));
            })();

            GC();
            assert.equal(no1, test_util.countObject('Chain'));
            assert.equal(no2, test_util.countObject('Routing'));
        });

        it("memory leak 2", () => {
            var c;
            var closed = false;

            var no1 = test_util.countObject('Chain');
            var no2 = test_util.countObject('Routing');
            var no3 = test_util.countObject('Buffer');

            (() => {
                var data = new Buffer();
                var svr = new http.Server(8892, new mq.Chain([
                    () => {
                        return hdlr
                    },
                    new mq.Routing({
                        'a': (r) => {
                            r.response.write(data);
                            return hdlr
                        }
                    })
                ]));

                assert.equal(no1 + 1, test_util.countObject('Chain'));
                assert.equal(no2 + 1, test_util.countObject('Routing'));
                assert.equal(no3 + 1, test_util.countObject('Buffer'));

                svr.asyncRun();

                setTimeout(() => {
                    c = net.connect('tcp://127.0.0.1:8892');
                    svr.stop();
                    closed = true;
                }, 10);
            })();

            GC();
            assert.equal(closed, false);
            assert.equal(no1 + 1, test_util.countObject('Chain'));
            assert.equal(no2 + 1, test_util.countObject('Routing'));
            assert.equal(no3 + 1, test_util.countObject('Buffer'));

            coroutine.sleep(100);

            GC();
            assert.equal(closed, true);
            assert.equal(no1 + 1, test_util.countObject('Chain'));
            assert.equal(no2 + 1, test_util.countObject('Routing'));
            assert.equal(no3 + 1, test_util.countObject('Buffer'));

            c.close();
            coroutine.sleep(10);

            GC();
            assert.equal(no1, test_util.countObject('Chain'));
            assert.equal(no2, test_util.countObject('Routing'));
            assert.equal(no3, test_util.countObject('Buffer'));
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