var test = require("test");
test.setup();

var test_util = require('./test_util');

var io = require('io');
var fs = require('fs');
var http = require('http');
var net = require('net');
var encoding = require('encoding');
var zlib = require('zlib');
var zip = require('zip');
var coroutine = require("coroutine");
var path = require("path");

var ssl = require("ssl");
var crypto = require("crypto");

var base_port = coroutine.vmid * 10000;

var pk = new crypto.PKey();
pk.genRsaKey(1024);

var crt = new crypto.X509Req("CN=localhost", pk).sign("CN=baoz.me", pk);
var ca = new crypto.X509Req("CN=baoz.me", pk).sign("CN=baoz.me", pk, {
    ca: true
});

var ca_pem = ca.dump()[0];

function Step() {
    this.step = 0;
    this.wait = (n) => {
        while (this.step != n)
            coroutine.sleep(1);
        this.step++;
    };
}

function cookie_data(cookie) {
    var o = {
        name: cookie.name,
        value: cookie.value
    };

    if (isFinite(cookie.expires))
        o.expires = cookie.expires;
    if (cookie.domain != "")
        o.domain = cookie.domain;
    if (cookie.path != "")
        o.path = cookie.path;
    if (cookie.secure)
        o.secure = true;
    if (cookie.httpOnly)
        o.httpOnly = true;

    return o;
}


describe("http", () => {
    var ss = [];

    after(() => {
        http.cookies.resize(0);
        http.autoRedirect = true;
        http.enableCookie = true;

        ss.forEach((s) => {
            s.close();
        });
    });

    describe("headers", () => {
        var d = new http.Request().headers;

        it("add", () => {
            d.add('a', '100');
            d.add('b', '200');
            d.add('a', '300');
            d.add('c', '400');
            d.add('d', '500');
            d.add('c', '600');
            d.add('d', '700');

            assert.deepEqual(d['a'], ['100', '300']);

            assert.equal(d.has('a'), true);
            assert.equal(d.first('a'), '100');
        });

        it("all", () => {
            var a = d.all('a');
            assert.deepEqual(a.toArray(), ['100', '300']);
        });

        it("remove", () => {
            d.remove('a');
            assert.isFalse(d.has('a'));
            assert.isNull(d.first('a'));

            assert.isUndefined(d['a']);

            assert.equal(d.first('c'), '400');

            var a = d.all('c');
            assert.deepEqual(a.toArray(), ['400', '600']);
        });

        it("set", () => {
            d.set('c', '800');

            var a = d.all('c');
            assert.equal(a.length, 1);
            assert.equal(a[0], '800');

            assert.equal(d.first('c'), '800');
        });

        it("add({})", () => {
            d.add({
                d: "900",
                b: "1000"
            });

            var a = d.all('d');
            assert.deepEqual(a.toArray(), ['500', '700', '900']);

            a = d.all('b');
            assert.deepEqual(a.toArray(), ['200', '1000']);
        });

        it("set({})", () => {
            d.set({
                d: "900",
                b: "1000"
            });

            var a = d.all('d');
            assert.deepEqual(a.toArray(), ['900']);

            a = d.all('b');
            assert.deepEqual(a.toArray(), ['1000']);
        });

        it("set other type", () => {
            d.set('e', 2000);
            var a = d.all('e');
            assert.deepEqual(a.toArray(), ['2000']);

            d.set('f', 214748364700);
            assert.equal(d['f'], '214748364700');

            d.set('f', 214.123);
            assert.equal(d['f'], '214.123');

            var t = new Date('2012-12-12T12:12:12Z');
            d.set('f', t);

            assert.deepEqual(d['f'], "Wed, 12 Dec 2012 12:12:12 GMT");
            assert.deepEqual(new Date(d['f']), t);
        });
    });

    describe("cookie", () => {
        function build(opt) {
            return new http.Cookie(opt).toString();
        }

        function parse(str) {
            var cookie = new http.Cookie();
            cookie.parse(str);
            return cookie_data(cookie);
        }

        var cases = [
            [{
                name: "test",
                value: "value"
            }, "test=value"],
            [{
                name: "test=",
                value: "value;"
            }, "test%3D=value%3B"],
            [{
                name: "test",
                value: "value",
                domain: ".baoz.me"
            }, "test=value; domain=.baoz.me"],
            [{
                name: "test",
                value: "value",
                path: "/rpc"
            }, "test=value; path=/rpc"],
            [{
                name: "test",
                value: "value",
                secure: true
            }, "test=value; secure"],
            [{
                name: "test",
                value: "value",
                httpOnly: true
            }, "test=value; HttpOnly"],
            [{
                name: "test",
                value: "value",
                expires: new Date("2020-12-21T13:31:30Z")
            }, "test=value; expires=Mon, 21 Dec 2020 13:31:30 GMT"],
            [{
                name: "test",
                value: "value",
                domain: ".baoz.me",
                path: "/rpc",
                expires: new Date("2020-12-21T13:31:30Z"),
                secure: true,
                httpOnly: true
            }, "test=value; expires=Mon, 21 Dec 2020 13:31:30 GMT; domain=.baoz.me; path=/rpc; secure; HttpOnly"]
        ];

        it("build", () => {
            for (var i = 0; i < cases.length; i++)
                assert.equal(build(cases[i][0]), cases[i][1]);
        });

        it("parse", () => {
            for (var i = 0; i < cases.length; i++)
                assert.deepEqual(parse(cases[i][1]), cases[i][0]);

            assert.throws(() => {
                new http.Cookie().parse("");
            });

            assert.throws(() => {
                new http.Cookie().parse("aaa");
            });

            assert.throws(() => {
                new http.Cookie().parse("=");
            });

            assert.throws(() => {
                new http.Cookie().parse(";");
            });
        });

        var match_cases = [
            [{
                name: "test",
                value: "value"
            }, "http://www.baoz.me/", true],
            [{
                name: "test",
                value: "value",
                path: '/'
            }, "http://www.baoz.me/", true],
            [{
                name: "test",
                value: "value",
                path: '/path/'
            }, "http://www.baoz.me/path/to", true],
            [{
                name: "test",
                value: "value",
                path: '/path/'
            }, "http://www.baoz.me/path", true],
            [{
                name: "test",
                value: "value",
                path: '/path'
            }, "http://www.baoz.me/path/to", true],
            [{
                name: "test",
                value: "value",
                path: '/path'
            }, "http://www.baoz.me/path", true],
            [{
                name: "test",
                value: "value",
                path: '/path'
            }, "http://www.baoz.me/", false],
            [{
                name: "test",
                value: "value",
                path: '/path'
            }, "http://www.baoz.me/path1", false],
            [{
                name: "test",
                value: "value",
                path: '/path//////'
            }, "http://www.baoz.me/path", true],
            [{
                name: "test",
                value: "value",
                path: '///////'
            }, "http://www.baoz.me/path", true],
            [{
                name: "test",
                value: "value",
                domain: ".baoz.me"
            }, "http://www.baoz.me/", true],
            [{
                name: "test",
                value: "value",
                domain: ".baoz.me"
            }, "http://baoz.me/", true],
            [{
                name: "test",
                value: "value",
                domain: "baoz.me"
            }, "http://www.baoz.me/", true],
            [{
                name: "test",
                value: "value",
                domain: "baoz.me"
            }, "http://baoz.me/", true],
            [{
                name: "test",
                value: "value",
                domain: ".baoz.me"
            }, "http://www.baoz1.me/", false],
            [{
                name: "test",
                value: "value",
                domain: ".....baoz.me"
            }, "http://www.baoz.me/", true],
            [{
                name: "test",
                value: "value",
                domain: "....."
            }, "http://www.baoz.me/", true],
            [{
                name: "test",
                value: "value",
                domain: ".me"
            }, "http://www.baoz.me/", false]
        ];

        it("match", () => {
            for (var i = 0; i < match_cases.length; i++) {
                assert.equal(new http.Cookie(match_cases[i][0]).match(match_cases[i][1]),
                    match_cases[i][2]);
            }
        });
    });

    describe("parse", () => {
        function get_request(u) {
            var ms = new io.MemoryStream();
            var bs = new io.BufferedStream(ms);
            bs.EOL = "\r\n";

            bs.writeText(u);
            ms.seek(0, fs.SEEK_SET);

            var r = new http.Request();

            r.readFrom(bs);

            return r;
        }

        function get_response(txt, opts) {
            var ms = new io.MemoryStream();
            var bs = new io.BufferedStream(ms);
            bs.EOL = "\r\n";

            bs.writeText(txt);
            ms.seek(0, fs.SEEK_SET);

            var req = new http.Response();

            opts = opts || {};
            for (var k in opts)
                req[k] = opts[k];

            req.readFrom(bs);
            return req;
        }

        it("bad request", () => {
            var bad_reqs = [
                " GET / HTTP/1.0\r\nkeepalive: close\r\n\r\n",
                "GET ? HTTP/1.0\r\nkeepalive: close\r\n\r\n",
                "GET / \r\nkeepalive: close\r\n\r\n",
                "GET / HTTP/a.0\r\nkeepalive: close\r\n\r\n",
                "GET / HTT/1.0\r\nkeepalive: close\r\n\r\n",
                "GET / HTTP/1.\r\nkeepalive: close\r\n\r\n"
            ];

            function readreq(u) {
                assert.throws(() => {
                    get_request(u);
                });
            }

            bad_reqs.forEach(readreq);
        });

        it("bad response", () => {
            assert.throws(() => {
                get_response("HTTP/1.0\r\n\r\n");
            });

            assert.throws(() => {
                get_response("HTTP/1.0 \r\n\r\n");
            });

            assert.throws(() => {
                get_response("HTTP/1.0,200\r\n\r\n");
            });

            assert.throws(() => {
                get_response("HTTP/1.0 1\r\n\r\n");
            });

            assert.throws(() => {
                get_response("HTTP/1.0 1111\r\n\r\n");
            });

            assert.throws(() => {
                get_response("HTTP/1.0 asd\r\n\r\n");
            });
        });

        it("proxy request", () => {
            var req = get_request("GET http://www.com/index.html HTTP/1.0\r\nkeepalive: close\r\n\r\n");
            assert.equal("http://www.com/index.html", req.address);
            assert.equal("/index.html", req.value);

            var req = get_request("GET http://www.com.index.html HTTP/1.0\r\nkeepalive: close\r\n\r\n");
            assert.equal("http://www.com.index.html", req.address);
            assert.equal("", req.value);
        });

        it("basic format", () => {
            var req = get_request("GET / HTTP/1.0\r\nhead1: 100\r\nhead2: 200\r\nContent-type:test\r\nContent-length:    10\r\n\r\n0123456789");

            assert.equal('100', req.headers['head1']);
            assert.equal('200', req.headers['head2']);
            assert.equal(10, req.length);
            assert.equal('test', req.headers['content-type']);
            assert.equal('0123456789', req.body.read());

            assert.equal(req.socket.size(), 94);
            req.clear();
            assert.equal(req.stream, null);
            assert.equal(req.socket, null);

            var r = get_response("HTTP/1.0 200\r\n\r\n");
            assert.equal(r.status, 200);
            assert.equal(r.protocol, 'HTTP/1.0');
        });

        it("keep-alive", () => {
            var keep_reqs = {
                "GET / HTTP/1.0\r\n\r\n": false,
                "GET / HTTP/1.0\r\nconnection: close\r\n\r\n": false,
                "GET / HTTP/1.0\r\nconnection: keep-alive\r\n\r\n": true,
                "GET / HTTP/1.1\r\n\r\n": true,
                "GET / HTTP/1.1\r\nconnection: close\r\n\r\n": false,
                "GET / HTTP/1.1\r\nconnection: keep-alive\r\n\r\n": true,
            };

            for (var n in keep_reqs) {
                var req = get_request(n);
                assert.equal(keep_reqs[n],
                    req.keepAlive);
            }
        });

        it("request cookie", () => {
            function get_cookie(txt) {
                return get_request(txt).cookies.toJSON();
            }

            var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new;\r\n\r\n");
            assert.deepEqual(c, {
                '$Version': '1',
                'Skin': 'new'
            });

            var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20\r\n\r\n");
            assert.deepEqual(c, {
                '$Version': '1',
                'Skin': 'new '
            });

            var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20cookie %sdf\r\n\r\n");
            assert.deepEqual(c, {
                '$Version': '1',
                'Skin': 'new cookie %sdf'
            });
        });

        it("response cookie", () => {

            var cookies = get_response("HTTP/1.1 200 OK\r\nSet-Cookie: test=value\r\nSet-Cookie: test1=value1\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n").cookies;
            assert.equal(cookies.length, 2);

            assert.deepEqual(cookie_data(cookies[0]), {
                name: "test",
                value: "value"
            });

            assert.deepEqual(cookie_data(cookies[1]), {
                name: "test1",
                value: "value1"
            });
        });

        it("query", () => {
            function get_query(txt) {
                return get_request(txt).query.toJSON();
            }

            var c = get_query("GET /test?a=100&b=200 HTTP/1.0\r\n\r\n");
            assert.deepEqual(c, {
                'a': '100',
                'b': '200'
            });

            var c = get_query("GET /test?a&b HTTP/1.0\r\n\r\n");
            assert.deepEqual(c, {
                'a': '',
                'b': ''
            });

            var c = get_query("GET /test?a&&&&&&&&&&&&&&&&&&&&&&&&&b HTTP/1.0\r\n\r\n");
            assert.deepEqual(c, {
                'a': '',
                'b': ''
            });

            var c = get_query("GET /test? HTTP/1.0\r\n\r\n");
            assert.deepEqual(c, {});
        });

        it("form", () => {
            function get_form(txt) {
                return get_request(txt).form;
            }

            var c = get_form("GET /test HTTP/1.0\r\nContent-type:application/x-www-form-urlencoded\r\nContent-length:11\r\n\r\na=100&b=200");
            assert.deepEqual(c.toJSON(), {
                'a': '100',
                'b': '200'
            });

            var c = get_form("GET /test HTTP/1.0\r\nContent-type:application/x-www-form-urlencoded\r\nContent-length:3\r\n\r\na&b");
            assert.deepEqual(c.toJSON(), {
                'a': '',
                'b': ''
            });

            var c = get_form("GET /test HTTP/1.0\r\nContent-type:application/x-www-form-urlencoded\r\nContent-length:27\r\n\r\na&&&&&&&&&&&&&&&&&&&&&&&&&b");
            assert.deepEqual(c.toJSON(), {
                'a': '',
                'b': ''
            });

            var c = get_form("GET /test HTTP/1.0\r\nContent-type:application/x-www-form-urlencoded\r\nContent-length:15\r\n\r\na=100&b=200+300");
            assert.deepEqual(c.toJSON(), {
                'a': '100',
                'b': '200 300'
            });

            var c = get_form('GET /test HTTP/1.0\r\nContent-type:multipart/form-data;boundary=7d33a816d302b6\r\nContent-length:150\r\n\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="a"\r\n\r\n100\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="b"\r\n\r\n200\r\n--7d33a816d302b6\r\n');
            assert.deepEqual(c.toJSON(), {
                'a': '100',
                'b': '200'
            });

            var c = get_form('GET /test HTTP/1.0\r\nContent-type:multipart/form-data;boundary=7d33a816d302b6\r\nContent-length:201\r\n\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="a"\r\n\r\n100\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="b";filename="test"\r\nContent-Transfer-Encoding: base64\r\n\r\n200\r\n--7d33a816d302b6\r\n');
            assert.equal(c['a'], '100');
            assert.equal(c['b'].fileName, 'test');
            assert.equal(c['b'].contentTransferEncoding, 'base64');
            assert.equal(c['b'].body.read().toString(), '200');
        });

        it("chunk", () => {
            function chunk(data) {
                return data.length.toString(16) + '\r\n' + data + '\r\n';
            }

            var datas = ['This is the first chunk',
                'This is the second chunk',
                'This is the third chunk',
                'This is the fourth chunk',
                'This is the 5th chunk', ''
            ];

            var rep = get_response('HTTP/1.1 200\r\nConnection: close\r\nTransfer-encoding: chunked\r\n\r\n' + datas.map(chunk).join(''));
            assert.equal(datas.join(''), rep.body.read());
        });


        describe('maxBodySize', () => {
            var data_1024 = '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef';

            function get_data(sz) {
                var data = 'HTTP/1.1 200\r\nConnection: close\r\nContent-Length: ' + sz + '\r\n\r\n';

                assert.equal(data_1024.length, 1024);

                while (sz >= 1024) {
                    data += data_1024;
                    sz -= 1024;
                }

                if (sz > 0)
                    data += data_1024.substr(1, sz);

                return data;
            }

            function get_chunk_data(sz) {
                function chunk(data) {
                    return data.length.toString(16) + '\r\n' + data + '\r\n';
                }

                var data = 'HTTP/1.1 200\r\nConnection: close\r\nTransfer-encoding: chunked\r\n\r\n';

                assert.equal(data_1024.length, 1024);

                while (sz >= 1024) {
                    data += chunk(data_1024);
                    sz -= 1024;
                }

                if (sz > 0)
                    data += chunk(data_1024.substr(1, sz));

                data += chunk("");
                return data;
            }

            it('modify', () => {
                var req = new http.Request();
                assert.equal(req.maxBodySize, 64);
                req.maxBodySize = -1;
                assert.equal(req.maxBodySize, -1);

                var req = new http.Response();
                assert.equal(req.maxBodySize, 64);
                req.maxBodySize = -1;
                assert.equal(req.maxBodySize, -1);
            })

            it('max size', () => {
                var rep = get_response(get_data(1024 * 1024), {
                    maxBodySize: 1
                });
                assert.equal(rep.length, 1024 * 1024);
            })

            it('more then max size', () => {
                assert.throws(() => {
                    get_response(get_data(1024 * 1024 + 1), {
                        maxBodySize: 1
                    });
                });
            });

            it("no limit", () => {
                var rep = get_response(get_data(1024 * 1024 + 1), {
                    maxBodySize: -1
                });
                assert.equal(rep.length, 1024 * 1024 + 1);
            });

            describe('chunk', () => {
                it('max size', () => {
                    var rep = get_response(get_chunk_data(1024 * 1024), {
                        maxBodySize: 1
                    });
                    assert.equal(rep.length, 1024 * 1024);
                })

                it('more then max size', () => {
                    assert.throws(() => {
                        get_response(get_chunk_data(1024 * 1024 + 1), {
                            maxBodySize: 1
                        });
                    });
                });

                it("no limit", () => {
                    var rep = get_response(get_chunk_data(1024 * 1024 + 1), {
                        maxBodySize: -1
                    });
                    assert.equal(rep.length, 1024 * 1024 + 1);
                });
            });
        });
    });

    describe("encode", () => {
        it("request", () => {
            var rep = new http.Request();
            rep.body.write("0123456789");

            var ms = new io.MemoryStream();

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'GET / HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
        });

        it("response", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.body.write("0123456789");

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
        });

        it("response header only", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.body.write("0123456789");

            rep.sendHeader(ms);
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n');
        });

        it("address", () => {
            var rep = new http.Request();
            rep.body.write("0123456789");
            rep.address = "/docs/";
            rep.value = "/docs/";

            var ms = new io.MemoryStream();

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'GET /docs/ HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
        });

        it("query", () => {
            var rep = new http.Request();
            rep.body.write("0123456789");
            rep.address = "/docs";
            rep.value = "/docs";
            rep.queryString = "page=100&style=wap";

            var ms = new io.MemoryStream();

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'GET /docs?page=100&style=wap HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
        });

        it("response cookies", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.addCookie(new http.Cookie("test", "value"));
            rep.addCookie(new http.Cookie("test1", "value1"));

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nSet-Cookie: test=value\r\nSet-Cookie: test1=value1\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n');
        });
    });

    describe("handler", () => {
        var svr, hdr;
        var c, bs;
        var st;

        before(() => {
            hdr = new http.Handler((r) => {
                if (r.value == '/throw')
                    throw new Error('throw test');
                else if (r.value == '/not_found')
                    r.response.status = 404;
                else if (r.value == '/remote_close') {
                    st.step = 1;
                    st.wait(2);
                }
            });
            svr = new net.TcpServer(8881 + base_port, hdr);

            svr.asyncRun();

            ss.push(svr.socket);
        });

        beforeEach(() => {
            c = new net.Socket();
            c.connect('127.0.0.1', 8881 + base_port);

            bs = new io.BufferedStream(c);
            bs.EOL = "\r\n";
        });

        afterEach(() => {
            c.close();
            bs.close();
        });

        function get_response() {
            var req = new http.Response();
            req.readFrom(bs);
            return req;
        }

        function getStats(hdr) {
            var o = hdr.stats.toJSON();
            delete o.totalTime;
            return o;
        }

        var throw_in_handler = false;
        var err_400 = 0;
        var err_404 = 0;
        var err_500 = 0;

        it("onerror", () => {
            hdr.onerror({
                400: (v) => {
                    err_400++;
                },
                404: (v) => {
                    if (throw_in_handler)
                        throw new Error('throw in error handler');
                    err_404++;
                },
                500: (v) => {
                    console.error(v.lastError);
                    err_500++;
                }
            });
        });

        it("normal request", () => {
            c.write("GET / HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.status, 200);

            coroutine.sleep(100);
            assert.deepEqual(getStats(hdr), {
                "total": 1,
                "pendding": 0,
                "request": 1,
                "response": 1,
                "error": 0,
                "error_400": 0,
                "error_404": 0,
                "error_500": 0
            });
        });

        it("bad request(error 400)", () => {
            c.write("GET /\r\n\r\n");
            var req = get_response();
            assert.equal(req.status, 400);

            coroutine.sleep(100);
            assert.equal(err_400, 1);
            assert.deepEqual(getStats(hdr), {
                "total": 2,
                "pendding": 0,
                "request": 2,
                "response": 2,
                "error": 1,
                "error_400": 1,
                "error_404": 0,
                "error_500": 0
            });
        });

        it("error 404", () => {
            c.write("GET /not_found HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.status, 404);

            coroutine.sleep(100);
            assert.equal(err_404, 1);
            assert.deepEqual(getStats(hdr), {
                "total": 3,
                "pendding": 0,
                "request": 3,
                "response": 3,
                "error": 1,
                "error_400": 1,
                "error_404": 1,
                "error_500": 0
            });
        });

        it("error 500", () => {
            c.write("GET /throw HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.status, 500);

            coroutine.sleep(100);
            assert.equal(err_500, 1);
            assert.deepEqual(getStats(hdr), {
                "total": 4,
                "pendding": 0,
                "request": 4,
                "response": 4,
                "error": 2,
                "error_400": 1,
                "error_404": 1,
                "error_500": 1
            });
        });

        it("remote close when response", () => {
            st = new Step();

            c.write("GET /remote_close HTTP/1.0\r\n\r\n");
            c.close();

            st.wait(1);

            assert.deepEqual(getStats(hdr), {
                "total": 5,
                "pendding": 1,
                "request": 5,
                "response": 4,
                "error": 2,
                "error_400": 1,
                "error_404": 1,
                "error_500": 1
            });

            st.wait(3);
            coroutine.sleep(50);
            assert.deepEqual(getStats(hdr), {
                "total": 5,
                "pendding": 0,
                "request": 5,
                "response": 5,
                "error": 2,
                "error_400": 1,
                "error_404": 1,
                "error_500": 1
            });
        });

        it("remote close when request", () => {
            c.write("GET / HTTP/1.0\r\n");
            c.close();

            coroutine.sleep(10);
            assert.deepEqual(getStats(hdr), {
                "total": 6,
                "pendding": 0,
                "request": 6,
                "response": 6,
                "error": 2,
                "error_400": 1,
                "error_404": 1,
                "error_500": 1
            });
        });

        it("reset stats", () => {
            hdr.stats.reset();
            assert.deepEqual(getStats(hdr), {
                "total": 6,
                "pendding": 0,
                "request": 0,
                "response": 0,
                "error": 0,
                "error_400": 0,
                "error_404": 0,
                "error_500": 0
            });
        });

        it("error in error handler", () => {
            err_404 = 0;
            throw_in_handler = true;
            c.write("GET /not_found HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.status, 404);

            assert.equal(err_404, 0);
        });

    });

    describe("file handler", () => {
        var baseFolder = __dirname;
        var hfHandler = new http.fileHandler(baseFolder);
        var url = base_port + 'test.html';
        var filePath = path.join(baseFolder, url);
        var rep;

        function hfh_test(url, headers) {
            var req = new http.Request();
            req.value = url;
            if (headers)
                req.addHeader(headers);
            hfHandler.invoke(req);
            return req.response;
        }

        function clean() {
            try {
                fs.unlink(filePath);
            } catch (e) {};
            try {
                fs.unlink(filePath + '.gz');
            } catch (e) {};
        }

        before(clean);
        after(clean);

        it("file not found", () => {
            rep = hfh_test(url);
            assert.equal(404, rep.status);
            rep.clear();
        });

        it("normal", () => {
            fs.writeFile(filePath, 'test html file');

            rep = hfh_test(url);
            assert.equal(200, rep.status);
            assert.equal(14, rep.length);

            var a = new Date(rep.firstHeader('Last-Modified'));
            var b = fs.stat(filePath).mtime;

            a = JSON.stringify(a).split('.')[0];
            b = JSON.stringify(b).split('.')[0];

            assert.equal(a, b);
        });

        it("not modified", () => {
            var rep1 = hfh_test(url, {
                'If-Modified-Since': rep.firstHeader('Last-Modified')
            });
            assert.equal(304, rep1.status);
            rep1.clear();
            rep.clear();
        });

        it("changed", () => {
            var rep = hfh_test(url, {
                'If-Modified-Since': new Date('1998-04-14 12:12:12')
            });
            assert.equal(200, rep.status);
            assert.equal(14, rep.length);
            assert.equal('text/html', rep.firstHeader('Content-Type'));
            rep.clear();
        });

        it("index.html", () => {
            var rep = hfh_test("/");
            assert.equal(200, rep.status);
            assert.equal("this is index.html", rep.readAll().toString());
        });

        it("pre-gzip", () => {
            var sgz = 'gz test file';
            var gz = fs.openFile(filePath + '.gz', 'w');
            gz.write(zlib.gzip(new Buffer(sgz)));
            gz.close();

            var rep = hfh_test(url, {
                'Accept-Encoding': 'deflate,gzip'
            });
            assert.equal(200, rep.status);
            assert.equal('gzip', rep.firstHeader('Content-Encoding'));
            assert.equal('text/html', rep.firstHeader('Content-Type'));
            rep.body.rewind();
            assert.equal(sgz, zlib.gunzip(rep.body.readAll()).toString());
            rep.body.close();
            rep.clear();
        });

        it("don't gzip small file", () => {
            fs.unlink(filePath + '.gz');

            var rep = hfh_test(url, {
                'Accept-Encoding': 'deflate,gzip'
            });
            assert.equal(200, rep.status);
            assert.equal(null, rep.firstHeader('Content-Encoding'));
            rep.clear();
        });

        describe("zip virtual file", () => {
            var zurl = base_port + 'test.html.zip$/test.html';

            before(() => {
                var zipfile = zip.open(filePath + '.zip', "w");
                zipfile.write(filePath, 'test.html');
                zipfile.close();
            });

            after(() => {
                try {
                    fs.unlink(filePath + '.zip');
                } catch (e) {};
            });

            it("normal", () => {
                rep = hfh_test(zurl);
                assert.equal(200, rep.status);
                assert.equal(14, rep.length);

                assert.deepEqual(rep.readAll().toString(), "test html file");
            });

            it("not modified", () => {
                var rep1 = hfh_test(zurl, {
                    'If-Modified-Since': rep.firstHeader('Last-Modified')
                });
                assert.equal(304, rep1.status);
                rep1.clear();
                rep.clear();
            });

            it("changed", () => {
                var rep = hfh_test(zurl, {
                    'If-Modified-Since': new Date('1998-04-14 12:12:12')
                });
                assert.equal(200, rep.status);
                assert.equal(14, rep.length);
                assert.equal('text/html', rep.firstHeader('Content-Type'));
                rep.clear();
            });
        });
    });

    describe("server/global request", () => {
        var svr;
        var cookie;

        before(() => {
            http.enableCookie = true;
            svr = new http.Server(8882 + base_port, (r) => {
                var port = 8882 + base_port;
                cookie = r.headers.cookie;

                r.response.addHeader("set-cookie", "root1=value1; domain=127.0.0.2; path=/");
                r.response.addHeader("set-cookie", "root=value; domain=127.0.0.1:" + port + "; path=/");
                r.response.addHeader("set-cookie", "root=value; path=/");
                r.response.addHeader("set-cookie", "root=value2; path=/");

                if (r.address == "/name") {
                    r.response.addHeader("set-cookie", "name=value; path=/name");
                    r.response.body.write(r.address);
                } else if (r.address == "/redirect") {
                    r.response.redirect("http://127.0.0.1:" + (8882 + base_port) + "/request");
                } else if (r.address == "/redirect1") {
                    r.response.redirect("http://127.0.0.1:" + (8882 + base_port) + "/redirect1");
                } else if (r.address != "/gzip_test") {
                    r.response.addHeader("set-cookie", "request=value; domain=127.0.0.1; path=/request");
                    r.response.addHeader("set-cookie", "request1=value; domain=127.0.0.1; path=/request");
                    r.response.addHeader("set-cookie", "request2=value; domain=127.0.0.1; path=/request; secure");
                    r.response.addHeader("set-cookie", "request3=value; domain=127.0.0.1:" + port + "; path=/request;");
                    r.response.body.write(r.address);
                    r.body.copyTo(r.response.body);
                    if (r.hasHeader("test_header"))
                        r.response.body.write(r.firstHeader("test_header"));
                } else {
                    r.response.addHeader("set-cookie", "gzip_test=value; domain=127.0.0.1; path=/gzip_test");
                    r.response.addHeader("Content-Type", "text/html");
                    r.response.body.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                }
            });
            svr.asyncRun();

            ss.push(svr.socket);
        });

        describe("request", () => {
            it("simple", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request").body.read().toString(),
                    "/request");
                assert.equal(cookie, undefined);
                http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request");
                assert.equal(cookie, "root=value2; request=value; request1=value");
            });

            it("redirect", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/redirect").body.read().toString(),
                    "/request");

                assert.throws(() => {
                    http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/redirect1")
                });
            });

            it("body", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request:", "body").body.read().toString(),
                    "/request:body");
                assert.equal(cookie, "root=value2");
            });

            it("header", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    "test_header": "header"
                }).body.read().toString(), "/request:header");
                assert.equal(cookie, "root=value2");
            });

            it("gzip", () => {
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/gzip_test").body.read().toString(),
                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");

                assert.equal(cookie, "root=value2");
                http.get("http://127.0.0.1:" + (8882 + base_port) + "/gzip_test");
                assert.equal(cookie, "root=value2; gzip_test=value");
            });
        });

        describe("get", () => {
            it("simple", () => {
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/request").body.read().toString(),
                    "/request");
                assert.equal(cookie, "root=value2; request=value; request1=value")
            });

            it("header", () => {
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    "test_header": "header"
                }).body.read().toString(), "/request:header");
            });
        });

        describe("post", () => {
            it("body", () => {
                assert.equal(http.post("http://127.0.0.1:" + (8882 + base_port) + "/request:", "body").body.read().toString(),
                    "/request:body");
            });

            it("header", () => {
                assert.equal(http.post("http://127.0.0.1:" + (8882 + base_port) + "/request:", "", {
                    "test_header": "header"
                }).body.read().toString(), "/request:header");
            });
        });

        describe("disable global cookie", () => {
            it("disable global cookie", () => {
                assert.equal(http.enableCookie, true);
                http.enableCookie = false;
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/name").body.read().toString(),
                    "/name");
                assert.equal(cookie, "root=value2");
                http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/name");
                assert.equal(cookie, "root=value2");
            })
        })
    });

    describe("https server/global https request", () => {
        var svr;
        var cookie;

        before(() => {
            ssl.ca.load(ca_pem);

            http.enableCookie = true;
            svr = new http.HttpsServer(crt, pk, 8883 + base_port, (r) => {
                cookie = r.headers.cookie;
                if (r.address != "/gzip_test") {
                    r.response.addHeader("set-cookie", "request1=value; path=/");
                    r.response.addHeader("set-cookie", "request2=value; path=/; secure");
                    r.response.body.write(r.address);
                    r.body.copyTo(r.response.body);
                    if (r.hasHeader("test_header"))
                        r.response.body.write(r.firstHeader("test_header"));
                } else {
                    r.response.addHeader("Content-Type", "text/html");
                    r.response.body.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                }
            });
            svr.asyncRun();

            ss.push(svr.socket);
        });

        after(() => {
            ssl.ca.clear();
        });

        describe("request", () => {
            it("simple", () => {
                assert.equal(http.request("GET", "https://localhost:" + (8883 + base_port) + "/request").body.read().toString(),
                    "/request");
                assert.equal(cookie, undefined);
                http.request("GET", "https://localhost:" + (8883 + base_port) + "/request");
                assert.equal(cookie, "request1=value; request2=value");
            });

            it("body", () => {
                assert.equal(http.request("GET", "https://localhost:" + (8883 + base_port) + "/request:", "body").body.read().toString(),
                    "/request:body");
            });

            it("header", () => {
                assert.equal(http.request("GET", "https://localhost:" + (8883 + base_port) + "/request:", {
                    "test_header": "header"
                }).body.read().toString(), "/request:header");
            });

            it("gzip", () => {
                assert.equal(http.get("https://localhost:" + (8883 + base_port) + "/gzip_test").body.read().toString(),
                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
            });
        });

        describe("get", () => {
            it("simple", () => {
                assert.equal(http.get("https://localhost:" + (8883 + base_port) + "/request").body.read().toString(),
                    "/request");
            });

            it("header", () => {
                assert.equal(http.get("https://localhost:" + (8883 + base_port) + "/request:", {
                    "test_header": "header"
                }).body.read().toString(), "/request:header");
            });
        });

        describe("post", () => {
            it("body", () => {
                assert.equal(http.post(
                        "https://localhost:" + (8883 + base_port) + "/request:", "body").body
                    .read().toString(), "/request:body");
            });

            it("header", () => {
                assert.equal(http.post(
                    "https://localhost:" + (8883 + base_port) + "/request:", "", {
                        "test_header": "header"
                    }).body.read().toString(), "/request:header");
            });
        });
    });

    describe("server/client", () => {
        var svr;
        var cookie;

        before(() => {
            http.enableCookie = true;
            var pcnt = 0;
            svr = new http.Server(8884 + base_port, (r) => {
                var port = 8884 + base_port;
                cookie = r.headers.cookie;

                r.response.addHeader("set-cookie", "root1=value1; domain=127.0.0.2; path=/");
                r.response.addHeader("set-cookie", "root=value; domain=127.0.0.1:" + port + "; path=/");
                r.response.addHeader("set-cookie", "root=value; path=/");
                r.response.addHeader("set-cookie", "root=value2; path=/");

                if (r.address == "/timeout") {
                    coroutine.sleep(500);
                    r.response.body.write(r.address);
                } else if (r.address == "/name") {
                    r.response.addHeader("set-cookie", "name=value; domain=127.0.0.1:" + port + "; path=/name");
                    r.response.body.write(r.address);
                } else if (r.address == "/redirect") {
                    r.response.redirect("request");
                } else if (r.address == "/redirect/a/b/c") {
                    r.response.redirect("/d");
                } else if (r.address == "/redirect/a/b/d") {
                    r.response.redirect("e");
                } else if (r.address == "/redirect/a/b/f") {
                    r.response.redirect("../g");
                } else if (r.address == "/redirect1") {
                    r.response.redirect("http://127.0.0.1:" + (8884 + base_port) + "/redirect1");
                } else if (r.address == '/parallel') {
                    var n = pcnt++;
                    coroutine.sleep(100);
                    var n1 = pcnt++;
                    r.response.write(new String(n1 - n - 1));
                } else if (r.address != "/gzip_test") {
                    r.response.addHeader("set-cookie", "request=value; domain=127.0.0.1; path=/request");
                    r.response.addHeader("set-cookie", "request1=value; domain=127.0.0.1; path=/request");
                    r.response.addHeader("set-cookie", "request2=value; domain=127.0.0.1; path=/request; secure");
                    r.response.addHeader("set-cookie", "request3=value; domain=127.0.0.1:" + port + "; path=/request;");
                    r.response.body.write(r.address);
                    r.body.copyTo(r.response.body);
                    if (r.hasHeader("test_header"))
                        r.response.body.write(r.firstHeader("test_header"));
                } else {
                    r.response.addHeader("Content-Type", "text/html");
                    r.response.addHeader("set-cookie", "gzip_test=value; domain=127.0.0.1; path=/gzip_test");
                    r.response.body.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                }
            });
            svr.asyncRun();

            ss.push(svr.socket);
        });

        describe("request & cookie", () => {
            var client = new http.Client();

            it("simple", () => {
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request").body.readAll().toString(),
                    "/request");

                assert.equal(cookie, undefined);
                client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request");
                assert.equal(cookie, "root=value2; request=value; request1=value");
            });

            it("redirect", () => {
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/redirect").body.readAll().toString(),
                    "/request");

                assert.equal(cookie, "root=value2; request=value; request1=value");
                assert.throws(() => {
                    client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/redirect1")
                });
            });

            it("check cookie validity", () => {
                client.cookies.push(0);
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/redirect").body.readAll().toString(),
                    "/request");
            });

            it("body", () => {
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request:", "body").body.readAll().toString(),
                    "/request:body");
            });

            it("header", () => {
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request:", {
                    "test_header": "header"
                }).body.read().toString(), "/request:header");
            });

            it("gzip", () => {
                assert.equal(client.get("http://127.0.0.1:" + (8884 + base_port) + "/gzip_test").body.readAll().toString(),
                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                assert.equal(cookie, "root=value2");
                client.get("http://127.0.0.1:" + (8884 + base_port) + "/gzip_test");
                assert.equal(cookie, "root=value2; gzip_test=value");
            });

            it('parallel', () => {
                var rs = coroutine.parallel(() => {
                    return client.get("http://127.0.0.1:" + (8884 + base_port) + "/parallel").body.readAll().toString();
                }, 2);
                assert.ok(rs[0] !== '0' || rs[1] !== '0');
            });
        });

        describe("disable client cookie", () => {
            var client = new http.Client();

            it("disable cookie", () => {
                assert.equal(client.enableCookie, true);
                client.enableCookie = false;
                assert.equal(client.request('GET', "http://127.0.0.1:" + (8884 + base_port) + "/name").body.readAll().toString(),
                    "/name");
                assert.equal(cookie, undefined);

                client.request('GET', "http://127.0.0.1:" + (8884 + base_port) + "/name");
                assert.equal(cookie, undefined);
            })
        });

        describe("client timeout & global client timeout", () => {
            var client = new http.Client();

            it("overtime", () => {
                client.timeout = 200;

                var no = test_util.countObject('Timer');
                assert.throws(() => {
                    client.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout")
                });
                for (var i = 0; i < 1000 && no !== test_util.countObject('Timer'); i++)
                    coroutine.sleep(1);
                assert.equal(no, test_util.countObject('Timer'));
            });

            it("intime", () => {
                client.timeout = 1000;
                var no = test_util.countObject('Timer');
                assert.equal(client.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout").body.readAll().toString(),
                    "/timeout");
                for (var i = 0; i < 1000 && no !== test_util.countObject('Timer'); i++)
                    coroutine.sleep(1);
                assert.equal(no, test_util.countObject('Timer'));
            });

            it("global overtime", () => {
                http.timeout = 200;
                assert.throws(() => {
                    http.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout")
                });
            });

            it("global intime", () => {
                http.timeout = 1000;
                assert.equal(http.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout").body.readAll().toString(),
                    "/timeout");
            });

            it("autoredirect", () => {
                assert.equal(http.get('http://127.0.0.1:' + (8884 + base_port) + '/redirect/a/b/c').body.readAll().toString(),
                    "/d");
                assert.equal(http.get('http://127.0.0.1:' + (8884 + base_port) + '/redirect/a/b/d').body.readAll().toString(),
                    "/redirect/a/b/e");
                assert.equal(http.get('http://127.0.0.1:' + (8884 + base_port) + '/redirect/a/b/f').body.readAll().toString(),
                    "/redirect/a/g");
            });

            it("disable autoredirect", () => {
                http.autoRedirect = false;
                var resp = http.get('http://127.0.0.1:' + (8884 + base_port) + '/redirect');
                assert.equal(resp.headers.location, "request");
            })
        });
    });
});

repl && test.run(console.DEBUG);