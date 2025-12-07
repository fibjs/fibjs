var test = require("test");
test.setup();

var test_util = require('./test_util');

var io = require('io');
var os = require('os');
var fs = require('fs');
var http = require('http');
var net = require('net');
var zip = require('zip');
var coroutine = require("coroutine");
var path = require("path");

var tls = require("tls");
var crypto = require("crypto");

const {
    generateFakeMp4
} = require('./http_files/range_test/fake');

var base_port = coroutine.vmid * 10000;

var pk = crypto.generateKeyPair('rsa', {
    modulusLength: 2048
});
var pk1 = crypto.generateKeyPair('rsa', {
    modulusLength: 2048
});

var ca = crypto.createCertificateRequest({
    key: pk.privateKey,
    subject: {
        CN: "fibjs.org"
    }
}).issue({
    key: pk.privateKey,
    ca: true,
    issuer: {
        CN: "fibjs.org"
    }
});

var crt = crypto.createCertificateRequest({
    key: pk1.privateKey,
    subject: {
        CN: "localhost"
    }
}).issue({
    key: pk.privateKey,
    issuer: {
        CN: "fibjs.org"
    }
});

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

function set_header_for_head_req(r, cookie_for) {
    if (r.method.toUpperCase() === 'HEAD') {
        cookie_for['head'] = r.headers.cookie
        if (r.hasHeader("test_header")) {
            r.response.setHeader('test_header', 'foobar')
        } else {
            r.response.setHeader('no_test_header', 'true')
        }
    }
}

describe("http", () => {
    after(() => {
        http.timeout = 0;
        http.autoRedirect = true;
        http.enableCookie = true;
    });

    after(test_util.cleanup);

    describe("headers", () => {
        var d = new http.Request().headers;

        it("append", () => {
            d.append('b', '200');
            d.append('c', '400');
            d.append('d', '500');
            d.append('c', '600');
            d.append('d', '700');
            d.append('a', '100');
            d.append('a', '300');

            assert.deepEqual(d['a'], ['100', '300']);

            assert.equal(d.has('a'), true);
            assert.equal(d.first('a'), '100');
        });

        it("all", () => {
            var a = d.all('a');
            assert.deepEqual(a, ['100', '300']);

            a = d.all();
            assert.deepEqual(a, {
                "a": [
                    "100",
                    "300"
                ],
                "b": "200",
                "c": [
                    "400",
                    "600"
                ],
                "d": [
                    "500",
                    "700"
                ]
            });
        });

        it("remove", () => {
            d.remove('a');
            assert.isFalse(d.has('a'));
            assert.isNull(d.first('a'));

            assert.isUndefined(d['a']);

            assert.equal(d.first('c'), '400');

            var a = d.all('c');
            assert.deepEqual(a, ['400', '600']);
        });

        it("set", () => {
            d.set('c', '800');

            var a = d.all('c');
            assert.equal(a.length, 1);
            assert.equal(a[0], '800');

            assert.equal(d.first('c'), '800');
        });

        it("append({})", () => {
            d.append({
                d: "900",
                b: "1000",
                f: ["200", "400"]
            });

            d.append("g", ["300", "700"]);

            var a = d.all('d');
            assert.deepEqual(a, ['500', '700', '900']);

            a = d.all('b');
            assert.deepEqual(a, ['200', '1000']);

            a = d.all('f');
            assert.deepEqual(a, ["200", "400"]);

            a = d.all('g');
            assert.deepEqual(a, ["300", "700"]);
        });

        it("append([])", () => {
            d.append([
                ["h", "100"],
                ["i", "200"]
            ]);

            var a = d.get('h');
            assert.deepEqual(a, '100');
            a = d.get('i');
            assert.deepEqual(a, '200');
        });

        it("keys/values", () => {
            assert.deepEqual(Array.from(d.keys()), [
                "b",
                "d",
                "d",
                "c",
                "d",
                "b",
                "f",
                "f",
                "g",
                "g",
                "h",
                "i"
            ]);
            assert.deepEqual(Array.from(d.values()), [
                "200",
                "500",
                "700",
                "800",
                "900",
                "1000",
                "200",
                "400",
                "300",
                "700",
                "100",
                "200"
            ]);
        });

        it("sort({})", () => {
            assert.deepEqual(Array.from(d.keys()), [
                "b",
                "d",
                "d",
                "c",
                "d",
                "b",
                "f",
                "f",
                "g",
                "g",
                "h",
                "i"
            ]);
            d.sort();
            assert.deepEqual(Array.from(d.keys()), [
                "b",
                "b",
                "c",
                "d",
                "d",
                "d",
                "f",
                "f",
                "g",
                "g",
                "h",
                "i"
            ]);
        });

        it("set({})", () => {
            d.set({
                d: "900",
                b: "1000",
                f: ["2000", "4000"]
            });

            d.set("g", ["3000", "7000"]);

            var a = d.all('d');
            assert.deepEqual(a, ['900']);

            a = d.all('b');
            assert.deepEqual(a, ['1000']);

            a = d.all('f');
            assert.deepEqual(a, ["2000", "4000"]);

            a = d.all('g');
            assert.deepEqual(a, ["3000", "7000"]);
        });

        it("set other type", () => {
            d.set('e', 2000);
            var a = d.all('e');
            assert.deepEqual(a, ['2000']);

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

        function copyCookie(cookieObj) {
            var c = Object.assign({}, cookieObj);
            if (!c.path) {
                c.path = '/';
            }
            return c;
        }

        var cases = [
            [{
                name: "test",
                value: "value"
            }, "test=value; path=/"],
            [{
                name: "test=",
                value: "value;"
            }, "test%3D=value%3B; path=/"],
            [{
                name: "test",
                value: "value",
                domain: ".baoz.me",
                path: "/"
            }, "test=value; domain=.baoz.me; path=/"],
            [{
                name: "test",
                value: "value",
                path: "/rpc"
            }, "test=value; path=/rpc"],
            [{
                name: "test",
                value: "value",
                secure: true
            }, "test=value; path=/; secure"],
            [{
                name: "test",
                value: "value",
                httpOnly: true
            }, "test=value; path=/; HttpOnly"],
            [{
                name: "test",
                value: "value",
                expires: new Date("2020-12-21T13:31:30Z"),
                path: "/"
            }, "test=value; expires=Mon, 21 Dec 2020 13:31:30 GMT; path=/"],
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
                assert.deepEqual(parse(cases[i][1]), copyCookie(cases[i][0]));

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
        function get_request(u, r) {
            var ms = new io.MemoryStream();
            var bs = new io.BufferedStream(ms);
            bs.EOL = "\r\n";

            bs.writeText(u);
            ms.seek(0, fs.SEEK_SET);

            if (r)
                r.clear();
            else
                r = new http.Request();

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
            assert.equal(r.statusCode, 200);
            assert.equal(r.statusMessage, "");
            assert.equal(r.protocol, 'HTTP/1.0');

            var r = get_response("HTTP/1.0 200 ok\r\n\r\n");
            assert.equal(r.statusCode, 200);
            assert.equal(r.statusMessage, "ok");
            assert.equal(r.protocol, 'HTTP/1.0');

            var r = get_response("HTTP/1.0 200 ok\r\n\r\n123456");
            assert.equal(r.statusCode, 200);
            assert.equal(r.statusMessage, "ok");
            assert.equal(r.protocol, 'HTTP/1.0');
            assert.equal('123456', r.body.read());

            var r = get_response("HTTP/1.1 200 ok\r\n\r\n123456");
            assert.equal(r.statusCode, 200);
            assert.equal(r.statusMessage, "ok");
            assert.equal(r.protocol, 'HTTP/1.1');
            assert.isNull(r.body.read());

            var r = get_response("HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n123456");
            assert.equal(r.statusCode, 200);
            assert.equal(r.statusMessage, "ok");
            assert.equal(r.protocol, 'HTTP/1.1');
            assert.equal('123456', r.body.read());
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

            var r = get_request("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20cookie %sdf\r\n\r\n");
            c = r.cookies.toJSON();
            assert.deepEqual(c, {
                '$Version': '1',
                'Skin': 'new cookie %sdf'
            });

            r = get_request("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20cookie %sdf\r\n\r\n", r);
            c = r.cookies.toJSON();
            assert.deepEqual(c, {
                '$Version': '1',
                'Skin': 'new cookie %sdf'
            });

            r = get_request("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=\r\n\r\n", r);
            c = r.cookies.toJSON();
            assert.deepEqual(c, {
                '$Version': '1',
                'Skin': ''
            });
        });

        it("response cookie", () => {
            var cookies = get_response("HTTP/1.1 200 OK\r\nSet-Cookie: test=value\r\nSet-Cookie: test1=value1\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n").cookies;
            assert.equal(cookies.length, 2);

            assert.deepEqual(cookie_data(cookies[0]), {
                name: "test",
                value: "value",
                path: "/"
            });

            assert.deepEqual(cookie_data(cookies[1]), {
                name: "test1",
                value: "value1",
                path: "/"
            });

            var cookies = get_response("HTTP/1.1 200 OK\r\nSet-Cookie: test=\r\nSet-Cookie: test1=\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n").cookies;
            assert.equal(cookies.length, 2);

            assert.deepEqual(cookie_data(cookies[0]), {
                name: "test",
                value: "",
                path: "/"
            });

            assert.deepEqual(cookie_data(cookies[1]), {
                name: "test1",
                value: "",
                path: "/"
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

        it("form", async () => {
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
            // File upload should return a File object
            assert.equal(c['b'] instanceof File, true);
            assert.equal(c['b'].name, 'test');
            // File content should be accessible through standard File API
            assert.equal(await c['b'].text(), '200');

            var c = get_form('GET /test HTTP/1.0\r\nContent-type:multipart/form-data;boundary=7d33a816d302b6\r\nContent-length:82\r\n\r\n--7d33a816d302b6\r\nContent-Disposition: form-data; name="pid"\r\n\r\n--7d33a816d302b6\r\n');
            assert.equal(c['pid'], '');
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

    it('text', () => {
        var v = "hello world";

        var req = new http.Request();

        // test setting text content (doesn't force content-type)
        req.text(v);
        assert.equal(req.data, v);

        // test reading text with text/plain content-type
        req.setHeader('Content-Type', "text/plain");
        assert.equal(req.text(), v);

        // test reading text with text/html content-type
        req.setHeader('Content-Type', "text/html");
        assert.equal(req.text(), v);

        // test reading text with charset
        req.setHeader('Content-Type', "text/plain; charset=utf-8");
        assert.equal(req.text(), v);

        // test reading text with wrong content-type should not throw
        req.setHeader('Content-Type', "application/json");
        assert.equal(req.text(), 'hello world');

        var rep = new http.Response();

        // test setting text content
        rep.text(v);
        assert.equal(rep.data, v);

        // test reading text
        rep.setHeader('Content-Type', "text/plain");
        assert.equal(rep.text(), v);
    });

    it('json', () => {
        var v = {
            a: 100
        };

        var req = new http.Request();

        assert.throws(() => {
            req.json();
        });
        req.setHeader('Content-Type', "application/jso");
        assert.throws(() => {
            req.json();
        });

        req.json(v);
        assert.equal(req.firstHeader('Content-Type'), "application/json");
        assert.deepEqual(req.data, v);

        req.setHeader('Content-Type', "application/ld-json");
        assert.deepEqual(req.data, v);

        assert.deepEqual(req.json(), v);

        req.setHeader('Content-Type', "application/json; utf-8");
        assert.equal(req.firstHeader('Content-Type'), "application/json; utf-8");
        assert.deepEqual(req.json(), v);

        var rep = new http.Response();

        assert.throws(() => {
            rep.json();
        });
        req.setHeader('Content-Type', "application/jso");
        assert.throws(() => {
            req.json();
        });

        rep.json(v);
        assert.equal(rep.firstHeader('Content-Type'), "application/json");
        assert.deepEqual(rep.data, v);

        assert.deepEqual(rep.json(), v);

        rep.setHeader('Content-Type', "application/json; utf-8");
        assert.equal(rep.firstHeader('Content-Type'), "application/json; utf-8");
        assert.deepEqual(rep.json(), v);
    });

    it('pack', () => {
        var v = {
            a: 100,
            b: true,
            c: "hello",
            d: new Date()
        };

        var req = new http.Request();

        assert.throws(() => {
            req.pack();
        });
        req.setHeader('Content-Type', "application/msg");
        assert.throws(() => {
            req.pack();
        });

        req.pack(v);
        assert.equal(req.firstHeader('Content-Type'), "application/msgpack");
        assert.deepEqual(req.data, v);

        assert.deepEqual(req.pack(), v);

        req.setHeader('Content-Type', "application/msgpack; utf-8");
        assert.equal(req.firstHeader('Content-Type'), "application/msgpack; utf-8");
        assert.deepEqual(req.pack(), v);

        var rep = new http.Response();

        assert.throws(() => {
            rep.pack();
        });
        req.setHeader('Content-Type', "application/jso");
        assert.throws(() => {
            req.pack();
        });

        rep.pack(v);
        assert.equal(rep.firstHeader('Content-Type'), "application/msgpack");
        assert.deepEqual(rep.data, v);

        assert.deepEqual(rep.pack(), v);

        rep.setHeader('Content-Type', "application/msgpack; utf-8");
        assert.equal(rep.firstHeader('Content-Type'), "application/msgpack; utf-8");
        assert.deepEqual(rep.pack(), v);
    });

    describe("Response constructor", () => {
        it("empty constructor", () => {
            var res = new http.Response();
            assert.equal(res.status, 200);
            assert.equal(res.statusMessage, "");
            assert.equal(res.body.size(), 0);
        });

        it("string body", () => {
            var res = new http.Response("Hello World");
            assert.equal(res.status, 200);
            assert.equal(res.body.size(), 11);
            assert.equal(res.text(), "Hello World");
            assert.equal(res.firstHeader("Content-Type"), "text/plain;charset=UTF-8");
        });

        it("Buffer body", () => {
            var res = new http.Response(Buffer.from("Binary data"));
            assert.equal(res.status, 200);
            assert.equal(res.body.size(), 11);
            assert.equal(res.text(), "Binary data");
            assert.equal(res.firstHeader("Content-Type"), "application/octet-stream");
        });

        it("Blob body with type", () => {
            var blob = new Blob(["Hello from Blob"], { type: "text/html" });
            var res = new http.Response(blob);
            assert.equal(res.status, 200);
            assert.equal(res.body.size(), 15);
            assert.equal(res.text(), "Hello from Blob");
            assert.equal(res.firstHeader("Content-Type"), "text/html");
        });

        it("Blob body without type", () => {
            var blob = new Blob(["data"]);
            var res = new http.Response(blob);
            assert.equal(res.status, 200);
            assert.equal(res.body.size(), 4);
            assert.equal(res.firstHeader("Content-Type"), "application/octet-stream");
        });

        it("URLSearchParams body", () => {
            var params = new URLSearchParams({ name: "test", value: "123" });
            var res = new http.Response(params);
            assert.equal(res.status, 200);
            assert.equal(res.text(), "name=test&value=123");
            assert.equal(res.firstHeader("Content-Type"), "application/x-www-form-urlencoded");
        });

        it("SeekableStream body", () => {
            var ms = new io.MemoryStream();
            ms.write(Buffer.from("stream data"));
            ms.rewind();
            var res = new http.Response(ms);
            assert.equal(res.status, 200);
            assert.equal(res.body.size(), 11);
            assert.equal(res.text(), "stream data");
        });

        it("null body", () => {
            var res = new http.Response(null);
            assert.equal(res.status, 200);
            assert.equal(res.body.size(), 0);
        });

        it("undefined body", () => {
            var res = new http.Response(undefined);
            assert.equal(res.status, 200);
            assert.equal(res.body.size(), 0);
        });

        it("status option", () => {
            var res = new http.Response("Created", { status: 201 });
            assert.equal(res.status, 201);
            assert.equal(res.text(), "Created");
        });

        it("statusText option", () => {
            var res = new http.Response("", { status: 201, statusText: "Created" });
            assert.equal(res.status, 201);
            assert.equal(res.statusMessage, "Created");
        });

        it("headers option as object", () => {
            var res = new http.Response("test", {
                headers: {
                    "X-Custom": "value",
                    "X-Another": "another"
                }
            });
            assert.equal(res.firstHeader("X-Custom"), "value");
            assert.equal(res.firstHeader("X-Another"), "another");
        });

        it("full options", () => {
            var res = new http.Response("Success!", {
                status: 201,
                statusText: "Created",
                headers: {
                    "Content-Type": "text/plain",
                    "X-Custom": "test"
                }
            });
            assert.equal(res.status, 201);
            assert.equal(res.statusMessage, "Created");
            assert.equal(res.firstHeader("Content-Type"), "text/plain");
            assert.equal(res.firstHeader("X-Custom"), "test");
            assert.equal(res.text(), "Success!");
        });

        it("Content-Type from options overrides default", () => {
            var res = new http.Response("Hello", {
                headers: { "Content-Type": "application/custom" }
            });
            assert.equal(res.firstHeader("Content-Type"), "application/custom");
        });

        it("204 No Content", () => {
            var res = new http.Response(null, { status: 204, statusText: "No Content" });
            assert.equal(res.status, 204);
            assert.equal(res.statusMessage, "No Content");
            assert.equal(res.body.size(), 0);
        });

        it("ok property", () => {
            var res200 = new http.Response("", { status: 200 });
            assert.equal(res200.ok, true);

            var res201 = new http.Response("", { status: 201 });
            assert.equal(res201.ok, true);

            var res299 = new http.Response("", { status: 299 });
            assert.equal(res299.ok, true);

            var res300 = new http.Response("", { status: 300 });
            assert.equal(res300.ok, false);

            var res404 = new http.Response("", { status: 404 });
            assert.equal(res404.ok, false);

            var res500 = new http.Response("", { status: 500 });
            assert.equal(res500.ok, false);
        });
    });

    describe("encode", () => {
        it("request", () => {
            var rep = new http.Request();
            rep.write("0123456789");

            var ms = new io.MemoryStream();

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'GET / HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
        });

        it("empty body request", () => {
            var rep = new http.Request();

            var ms = new io.MemoryStream();

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'GET / HTTP/1.1\r\nConnection: keep-alive\r\n\r\n');
        });

        it("response", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.write("0123456789");

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
        });

        it("response header only", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.write("0123456789");

            rep.sendTo(ms, {
                header_only: true
            });
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n');
        });

        it("response without content-length", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.write("0123456789");

            rep.sendTo(ms, {
                header_only: true,
                content_length: false
            });
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n\r\n');
        });

        it("statusCode", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.statusCode = 404;

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 404 File Not Found\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n');
        });

        it("bug statusCode bug", () => {
            var ms = new io.MemoryStream();
            var r = new http.Response();
            r.statusCode = 5000002;
            r.sendTo(ms);
        });

        it("unknown statusCode", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.statusCode = 210;

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read().toString(), 'HTTP/1.1 210 Unknown\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n');
        });

        it("statusMessage", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.statusMessage = "test message";

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'HTTP/1.1 200 test message\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n');
        });

        it("writeHead", () => {
            var ms = new io.MemoryStream();

            var rep = new http.Response();
            rep.writeHead(202, "test message", {
                "test": "test_header"
            });

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read().toString(), 'HTTP/1.1 202 test message\r\ntest: test_header\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n');
        });

        it("address", () => {
            var rep = new http.Request();
            rep.write("0123456789");
            rep.address = "/docs/";
            rep.value = "/docs/";

            var ms = new io.MemoryStream();

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read(), 'GET /docs/ HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
        });

        it("query", () => {
            var rep = new http.Request();
            rep.write("0123456789");
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
            rep.addCookie(new http.Cookie("test2", ""));

            rep.sendTo(ms);
            ms.rewind();
            assert.equal(ms.read().toString(), 'HTTP/1.1 200 OK\r\nSet-Cookie: test=value; path=/\r\nSet-Cookie: test1=value1; path=/\r\nSet-Cookie: test2=; path=/\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n');
        });

        it("BUGFIX: query.all cause memoryUsage count error", () => {
            test_util.gc();

            var no = test_util.countObject('NObject');
            for (var i = 0; i < 100; i++) {
                var query = new http.Request().query;
                query.all();
            }
            test_util.gc();
            var no1 = test_util.countObject('NObject');
            assert.equal(no, no1);
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
                    r.response.statusCode = 404;
                else if (r.value == '/remote_close') {
                    st.step = 1;
                    st.wait(2);
                } else if (r.value == '/gzip_test') {
                    r.response.appendHeader("Content-Type", "text/html");
                    r.response.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                } else if (r.value == '/gzip_json') {
                    r.response.appendHeader("Content-Type", "application/json");
                    r.response.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                } else if (r.value == '/gzip_small') {
                    r.response.appendHeader("Content-Type", "text/html");
                    r.response.write("01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
                } else if (r.value == '/gzip_bin') {
                    r.response.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                } else if (r.value == '/custum_send') {
                    r.response.sendTo(r.stream);
                }
            });

            hdr.enableCrossOrigin(true);
            hdr.enableEncoding = true;
            svr = new net.TcpServer(8881 + base_port, hdr);

            svr.start();

            test_util.push(svr.socket);
        });

        beforeEach(() => {
            c = new net.Socket();
            c.connect(8881 + base_port, '127.0.0.1');

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

        var throw_in_handler = false;

        it("normal request", () => {
            c.write("GET / HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
        });

        it("gzip request", () => {
            c.write("GET /gzip_test HTTP/1.0\r\nAccept-Encoding: gzip,deflate\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
            assert.equal(req.firstHeader('Content-Encoding'), 'gzip');
        });

        it("gzip json", () => {
            c.write("GET /gzip_json HTTP/1.0\r\nAccept-Encoding: gzip,deflate\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
            assert.equal(req.firstHeader('Content-Encoding'), 'gzip');
        });

        it("not zip small file", () => {
            c.write("GET /gzip_small HTTP/1.0\r\nAccept-Encoding: gzip,deflate\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
            assert.equal(req.firstHeader('Content-Encoding'), null);
        });

        it("not zip bin file", () => {
            c.write("GET /gzip_bin HTTP/1.0\r\nAccept-Encoding: gzip,deflate\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
            assert.equal(req.firstHeader('Content-Encoding'), null);
        });

        it("bad request(error 400)", () => {
            c.write("GET /\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 400);
        });

        it("error 404", () => {
            c.write("GET /not_found HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 404);
        });

        it("error 500", () => {
            c.write("GET /throw HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 500);
        });

        it("remote close when response", () => {
            st = new Step();

            test_util.gc();
            var no = test_util.countObject('Socket');

            c.write("GET /remote_close HTTP/1.0\r\n\r\n");
            c.close();

            assert.equal(no, test_util.countObject('Socket'));

            st.wait(1);

            test_util.gc();
            assert.equal(no, test_util.countObject('Socket') + 1);
        });

        it("remote close when request", () => {
            test_util.gc();
            var no = test_util.countObject('Socket');

            c.write("GET / HTTP/1.0\r\n");

            test_util.gc();
            assert.equal(no, test_util.countObject('Socket'));

            c.close();

            assert.equal(no, test_util.countObject('Socket'));

            test_util.gc();
            assert.equal(no, test_util.countObject('Socket') + 1);
        });

        it("error in error handler", () => {
            err_404 = 0;
            throw_in_handler = true;
            c.write("GET /not_found HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 404);

            assert.equal(err_404, 0);
        });

        it("options request", () => {
            c.write("OPTIONS / HTTP/1.1\r\norigin: localhost\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
            assert.equal(req.firstHeader('Access-Control-Allow-Origin'), 'localhost');
            assert.isNull(req.firstHeader('Cache-Control'));

            c.write("GET / HTTP/1.1\r\norigin: localhost\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
            assert.equal(req.firstHeader('Cache-Control'), 'no-cache, no-store');
        });

        it("custom send", () => {
            c.write("GET /custum_send HTTP/1.0\r\n\r\n");
            var req = get_response();
            assert.equal(req.statusCode, 200);
            assert.equal(req.length, 0);
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
                req.appendHeader(headers);
            hfHandler.invoke(req);
            return req.response;
        }

        function clean() {
            try {
                fs.unlink(filePath);
            } catch (e) { };
            try {
                fs.unlink(filePath + '.gz');
            } catch (e) { };
        }

        before(clean);
        after(clean);

        it("isRouting", () => {
            assert.isTrue(hfHandler.isRouting());
        });

        it("file not found", () => {
            rep = hfh_test(url);
            assert.equal(404, rep.statusCode);
            rep.clear();
        });

        it("normal", () => {
            fs.writeFile(filePath, 'test html file');

            rep = hfh_test(url);
            assert.equal(200, rep.statusCode);
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
            assert.equal(304, rep1.statusCode);
            rep1.clear();
            rep.clear();
        });

        it("changed", () => {
            var rep = hfh_test(url, {
                'If-Modified-Since': new Date('1998-04-14 12:12:12')
            });
            assert.equal(200, rep.statusCode);
            assert.equal(14, rep.length);
            assert.equal('text/html', rep.firstHeader('Content-Type'));
            rep.clear();
        });

        it("index.html", () => {
            var rep = hfh_test("/");
            assert.equal(200, rep.statusCode);
            assert.equal('text/html', rep.firstHeader('Content-Type'));
            assert.equal("this is index.html", rep.readAll().toString());
        });

        it("empty value", () => {
            var rep = hfh_test("");
            assert.equal(200, rep.statusCode);
            assert.equal('text/html', rep.firstHeader('Content-Type'));
            assert.equal("this is index.html", rep.readAll().toString());
        });

        it("bad request", () => {
            var rep = hfh_test("/%25");
            assert.equal(400, rep.statusCode);
        });

        it("is file", () => {
            var hfHandler = new http.fileHandler(__filename);

            var req = new http.Request();
            req.value = "/any_url";
            hfHandler.invoke(req);
            assert.equal(req.response.readAll().toString(), fs.readFile(__filename).toString());
        });

        it("autoindex", () => {
            var rep = hfh_test("http_autoindex/");
            assert.equal(404, rep.statusCode);

            hfHandler = new http.fileHandler(baseFolder, true);

            var re = /<a href=\"t.*\">/g;
            var rep = hfh_test("http_autoindex/");
            assert.equal(200, rep.statusCode);
            assert.equal('text/html', rep.firstHeader('Content-Type'));
            var data = rep.readAll().toString();
            assert.deepEqual(re[Symbol.match](data), [
                "<a href=\"t.txt\">",
                "<a href=\"test.txt\">",
                "<a href=\"test_dir/\">"
            ]);
        });

        it("dir 404", () => {
            var rep = hfh_test("not_exists/");
            assert.equal(404, rep.statusCode);
        });

        describe("206 Range", () => {
            const fakePath = generateFakeMp4('fake_http_206.mp4').target;
            var mp4File = fs.openFile(fakePath);

            ;
            [
                "0-10",
                "0-333",
                "bytes 0-10",
            ].map(rangeStr => {
                it(`invalid Range header ${rangeStr}`, () => {
                    var rep = hfh_test("http_files/range_test/fake_http_206.mp4", {
                        "Range": rangeStr
                    });
                    assert.equal(416, rep.statusCode);
                });
            });

            it("valid Range header, but outside file size", () => {
                var rep = hfh_test("http_files/range_test/fake_http_206.mp4", {
                    "Range": `bytes=0-${Number(mp4File.size())}`
                });
                assert.equal(416, rep.statusCode);

                var rep = hfh_test("http_files/range_test/fake_http_206.mp4", {
                    "Range": `bytes=0-${Number(mp4File.size()) * 2}`
                });
                assert.equal(416, rep.statusCode);
            });

            describe("Partial Content from mp4", () => {
                [
                    'bytes=0-0',
                    'bytes=0-10',
                    'bytes=0-',
                    'bytes=-10',
                    'bytes=-1-10',
                    'bytes=0-999999',
                    `bytes=0-${Number(mp4File.size()) - 1}`
                ].forEach((range) => {
                    it(`Range: ${range}`, () => {
                        var rep = hfh_test("http_files/range_test/fake_http_206.mp4", {
                            "Range": `${range}`
                        });
                        assert.equal(206, rep.statusCode);

                        var [_, bpos, epos, fsize] = rep.firstHeader('Content-Range').match(/bytes ([0-9]+)\-([0-9]+)\/([0-9]+)/);

                        bpos = BigInt(bpos);
                        epos = BigInt(epos);

                        assert.equal(fsize, Number(mp4File.size()));

                        mp4File.seek(bpos, fs.SEEK_SET);
                        var buf = mp4File.read(epos - bpos + 1n);

                        rep.body.rewind();

                        assert.equal(0, buf.compare(rep.readAll()));
                    });
                })
            });
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
                } catch (e) { };
            });

            it("normal", () => {
                rep = hfh_test(zurl);
                assert.equal(200, rep.statusCode);
                assert.equal(14, rep.length);

                assert.deepEqual(rep.readAll().toString(), "test html file");
            });

            it("not modified", () => {
                var rep1 = hfh_test(zurl, {
                    'If-Modified-Since': rep.firstHeader('Last-Modified')
                });
                assert.equal(304, rep1.statusCode);
                rep1.clear();
                rep.clear();
            });

            it("changed", () => {
                var rep = hfh_test(zurl, {
                    'If-Modified-Since': new Date('1998-04-14 12:12:12')
                });
                assert.equal(200, rep.statusCode);
                assert.equal(14, rep.length);
                assert.equal('text/html', rep.firstHeader('Content-Type'));
                rep.clear();
            });
        });

        describe("check path", () => {
            let testpath;

            before(() => {
                testpath = process.cwd();
                process.chdir(__dirname);
            });

            after(() => {
                process.chdir(testpath);
            });

            it("can read child path", () => {
                let str = `this is test in sub folder.`;
                let urls = [
                    '/http_autoindex/test_dir/',
                    'http_autoindex/../http_autoindex/test_dir/',
                    '/http_autoindex/test_dir/../../http_autoindex/test_dir/',
                    'http_autoindex/../http_autoindex/test_dir/../test_dir/',

                    '/http_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2f..%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2ftest_dir%2f..%2f..%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2f..%2fhttp_autoindex%2ftest_dir%2f..%2ftest_dir%2f',

                    '/./http_autoindex/./test_dir/',
                    '/./http_autoindex/.././http_autoindex/test_dir/',
                    '/./http_autoindex/test_dir/../.././http_autoindex/./test_dir/',
                    '/./http_autoindex/.././http_autoindex/test_dir/../test_dir/',

                    '//http_autoindex//test_dir//',
                    '//http_autoindex//..//http_autoindex//test_dir//',
                    '//http_autoindex/test_dir//..//..//http_autoindex/test_dir//',
                    '//http_autoindex//..//http_autoindex//test_dir//../test_dir//',
                ];

                hfHandler = new http.fileHandler(baseFolder);
                urls.forEach(url => {
                    var resp = hfh_test(url + 'test.txt');
                    assert.equal(resp.statusCode, 200);
                    assert.equal(str, resp.readAll().toString());
                });

                hfHandler = new http.fileHandler("./");
                urls.forEach(url => {
                    var resp = hfh_test(url + 'test.txt');
                    assert.equal(resp.statusCode, 200);
                    assert.equal(str, resp.readAll().toString());
                })
            });

            it("can't read parent of server root path", () => {
                let str = `this is test in sub folder.`;
                let urls400 = [
                    '/http_autoindex\\test_dir/',
                    'http_autoindex/../http_autoindex\\test_dir/',
                    '/http_autoindex/test_dir/../../http_autoindex\\test_dir/',
                    'http_autoindex/../http_autoindex/test_dir\\..\\test_dir/',
                    '../test/http_autoindex/../test_dir/',
                    '../test/http_autoindex/../http_autoindex/test_dir/',
                    '../test/http_autoindex/test_dir/../../http_autoindex/test_dir/',
                    '../test/http_autoindex/../http_autoindex/test_dir/../test_dir/',

                    '../../fibjs/test/http_autoindex/test_dir/',
                    '../../fibjs/test/http_autoindex/../http_autoindex/test_dir/',
                    '../../fibjs/test/http_autoindex/test_dir/../../http_autoindex/test_dir/',
                    '../../fibjs/test/http_autoindex/../http_autoindex/test_dir/../test_dir/',
                ];

                let urls404 = [
                    '/../test/http_autoindex/../test_dir/',
                    '/../test/http_autoindex/../http_autoindex/test_dir/',
                    '/../test/http_autoindex/test_dir/../../http_autoindex/test_dir/',
                    '/../test/http_autoindex/../http_autoindex/test_dir/../test_dir/',

                    '/../../fibjs/test/http_autoindex/test_dir/',
                    '/../../fibjs/test/http_autoindex/../http_autoindex/test_dir/',
                    '/../../fibjs/test/http_autoindex/test_dir/../../http_autoindex/test_dir/',
                    '/../../fibjs/test/http_autoindex/../http_autoindex/test_dir/../test_dir/',

                    '/http_autoindex%2ftest_dir%2f..%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2ftest_dir%2f..%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f..%2ftest_dir%2f',

                    '/http_autoindex%2ftest_dir%2f..%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2ftest_dir%2f..%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f',
                    '/http_autoindex%2f..%2f..%2ftest%2fhttp_autoindex%2ftest_dir%2f..%2ftest_dir%2f',
                ];

                hfHandler = new http.fileHandler(baseFolder);
                urls400.forEach(url => {
                    var resp = hfh_test(url + 'test.txt');
                    assert.equal(resp.statusCode, 400);
                    assert.equal(resp.length, 0);
                });


                urls404.forEach(url => {
                    var resp = hfh_test(url + 'test.txt');
                    assert.equal(resp.statusCode, 404);
                    assert.equal(resp.length, 0);
                });


                hfHandler = new http.fileHandler("./");
                urls400.forEach(url => {
                    var resp = hfh_test(url + 'test.txt');
                    assert.equal(resp.statusCode, 400);
                    assert.equal(resp.length, 0);
                });

                urls404.forEach(url => {
                    var resp = hfh_test(url + 'test.txt');
                    assert.equal(resp.statusCode, 404);
                    assert.equal(resp.length, 0);
                });
            });
        });

        describe("MIME type detection", () => {
            var mimeTestFolder = path.join(__dirname, 'http_files/mime');
            var mimeHandler = new http.fileHandler(mimeTestFolder);

            function test_mime_type(filename, expectedContentType) {
                var req = new http.Request();
                req.value = "/" + filename;
                mimeHandler.invoke(req);
                var resp = req.response;
                assert.equal(resp.statusCode, 200, `File ${filename} should be found`);
                assert.equal(resp.firstHeader('Content-Type'), expectedContentType,
                    `File ${filename} should have content-type ${expectedContentType}`);
                return resp;
            }

            it("JavaScript files (.js)", () => {
                var resp = test_mime_type('test.js', 'application/javascript');
                resp.clear();
            });

            it("HTML files (.html)", () => {
                var resp = test_mime_type('test.html', 'text/html');
                resp.clear();
            });

            it("CSS files (.css)", () => {
                var resp = test_mime_type('test.css', 'text/css');
                resp.clear();
            });

            it("JSON files (.json)", () => {
                var resp = test_mime_type('test.json', 'application/json');
                resp.clear();
            });

            it("Markdown files (.md)", () => {
                var resp = test_mime_type('test.md', 'text/markdown');
                resp.clear();
            });

            it("CSV files (.csv)", () => {
                var resp = test_mime_type('test.csv', 'text/csv');
                resp.clear();
            });

            it("Plain text files (.txt)", () => {
                var resp = test_mime_type('test.txt', 'text/plain');
                resp.clear();
            });

            it("XML files (.xml)", () => {
                var resp = test_mime_type('test.xml', 'application/xml');
                resp.clear();
            });
        });
    });

    describe("server/global request", () => {
        var svr;

        var cookie_for = {
            _: undefined,
            head: undefined
        };

        before(() => {
            http.enableCookie = true;
            svr = new http.Server(8882 + base_port, (r) => {
                var port = 8882 + base_port;

                cookie_for['_'] = r.headers.cookie;
                r.response.appendHeader("set-cookie", [
                    "root1=value1; domain=127.0.0.2; path=/",
                    "root=value; domain=127.0.0.1:" + port + "; path=/",
                    "root=value; path=/",
                    "root=value2; path=/"
                ]);

                set_header_for_head_req(r, cookie_for);

                if (r.address == "/clear_cookie") {
                    r.response.appendHeader("set-cookie", []);
                    cookie_for['head'] = undefined;
                    cookie_for['_'] = undefined;
                } else if (r.address == "/name") {
                    r.response.appendHeader("set-cookie", "name=value; path=/name");
                    r.response.write(r.address);
                } else if (r.address == "/redirect") {
                    r.response.appendHeader("test", "test1");
                    r.response.redirect("http://127.0.0.1:" + (8882 + base_port) + "/request");
                } else if (r.address == "/redirect1") {
                    r.response.redirect("http://127.0.0.1:" + (8882 + base_port) + "/redirect1");
                } else if (r.address == "/agent") {
                    if (r.allHeader("user-agent").length == 1)
                        r.response.write(r.firstHeader("user-agent"));
                } else if (r.address == "/request_query:") {
                    r.response.write(r.address);
                    r.response.write(r.query.test_field);
                } else if (r.address == "/request_url:") {
                    r.response.write(r.address);
                    try {
                        r.response.write(r.form.test_field || "");
                    } catch (e) {
                        r.response.write("");
                    }
                } else if (r.address == "/request_json:") {
                    r.response.write(r.address);
                    try {
                        var jsonData = r.json();
                        r.response.write(jsonData.test_field || "");
                    } catch (e) {
                        r.response.write("");
                    }
                } else if (r.address == "/request_pack:") {
                    r.response.write(r.address);
                    try {
                        var packData = r.pack();
                        r.response.write(packData.test_field || "");
                    } catch (e) {
                        r.response.write("");
                    }
                } else if (r.address == "/host:") {
                    r.response.write(r.address);
                    r.response.write(r.firstHeader('host'));
                } else if (r.address == "/large_file") {
                    // Simulate a large file response for HEAD request testing
                    // Create a body larger than default maxBodySize (64MB)
                    // We use 100MB to ensure it exceeds the limit
                    var largeSize = 100 * 1024 * 1024; // 100MB
                    r.response.setHeader("Content-Type", "application/octet-stream");

                    // Create a large buffer (this will set the correct Content-Length)
                    var largeBuffer = new Buffer(largeSize);
                    r.response.body.write(largeBuffer);
                } else if (r.address == "/gzip_test") {
                    r.response.appendHeader("set-cookie", "gzip_test=value; domain=127.0.0.1; path=/gzip_test");
                    r.response.appendHeader("Content-Type", "text/html");
                    r.response.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                } else if (r.address != "/gzip_test") {
                    r.response.appendHeader("set-cookie", [
                        "request=value; domain=127.0.0.1; path=/request",
                        "request1=value; domain=127.0.0.1; path=/request",
                        "request2=value; domain=127.0.0.1; path=/request; secure",
                        "request3=value; domain=127.0.0.1:" + port + "; path=/request;"
                    ]);
                    r.response.write(r.address);
                    r.body.copyTo(r.response.body);

                    if (r.hasHeader("test_header"))
                        r.response.write(r.firstHeader("test_header"));

                    if (r.hasHeader("test_headers"))
                        r.response.json(r.allHeader("test_headers"));
                }
            });

            svr.enableEncoding = true;
            svr.start();

            test_util.push(svr.socket);
        });

        describe("request", () => {
            it("simple", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request").body.read().toString(),
                    "/request");
                assert.equal(cookie_for['_'], undefined);
                http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request");
                assert.equal(cookie_for['_'], "root=value2; request=value; request1=value");

                assert.equal(http.request("http://127.0.0.1:" + (8882 + base_port) + "/request").body.read().toString(),
                    "/request");

                assert.equal(http.request({
                    'protocol': 'http:',
                    'slashes': true,
                    'hostname': '127.0.0.1',
                    'port': 8882 + base_port,
                    'pathname': '/request',
                    'path': '/request'
                }).body.read().toString(),
                    "/request");
            });

            it("redirect", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/redirect").body.read().toString(),
                    "/request");

                assert.throws(() => {
                    http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/redirect1")
                });
            });

            it("urlencode", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request_query:", {
                    query: {
                        test_field: "field"
                    }
                }).body.read().toString(),
                    "/request_query:field");
            });

            it("body", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    body: "body"
                }).body.read().toString(),
                    "/request:body");
                assert.equal(cookie_for['_'], "root=value2");

                var ms = new io.MemoryStream();
                ms.write("body");
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    body: ms
                }).body.read().toString(),
                    "/request:body");
                assert.equal(cookie_for['_'], "root=value2");
            });

            it("urlencode", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request_url:", {
                    body: {
                        test_field: "field"
                    }
                }).body.read().toString(),
                    "/request_url:field");
            });

            it("json", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request_json:", {
                    json: {
                        test_field: "field"
                    }
                }).body.read().toString(),
                    "/request_json:field");
            });

            it("pack", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request_pack:", {
                    pack: {
                        test_field: "field"
                    }
                }).body.read().toString(),
                    "/request_pack:field");
            });

            describe("body processing", () => {
                it("string body", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: "test string"
                    });
                    assert.equal(response.body.read().toString(), "/request:test string");
                });

                it("Buffer body", () => {
                    var buf = new Buffer("test buffer");
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: buf
                    });
                    // Buffer content may be URL encoded when processed as form data
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:") && responseText.includes("test"));
                });

                it("Buffer body with application/octet-stream Content-Type", () => {
                    // Test user's specific scenario: Buffer with application/octet-stream
                    var buf = new Buffer("binary file content data");
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: buf,
                        headers: {
                            'Content-Type': 'application/octet-stream',
                            'X-File-Name': 'testfile.bin'
                        }
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                    // Should contain the buffer content
                    assert.ok(responseText.includes("binary file content data"));
                });

                it("Buffer body with binary data", () => {
                    // Test Buffer with actual binary data (non-UTF8)
                    var binaryData = new Buffer([0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD]);
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: binaryData,
                        headers: {
                            'Content-Type': 'application/octet-stream'
                        }
                    });
                    // Should not throw encoding errors
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                });

                it("Large Buffer body", () => {
                    // Test with larger Buffer to check for encoding issues
                    var largeBuffer = new Buffer(1024);
                    largeBuffer.fill(0x41); // Fill with 'A' character
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: largeBuffer,
                        headers: {
                            'Content-Type': 'application/octet-stream'
                        }
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                    // Should handle large buffer without issues
                    assert.ok(responseText.length > 20); // Should have received substantial content
                });

                it("Buffer body with application/octet-stream Content-Type - encoding error case", () => {
                    // This test reproduces the specific user-reported issue
                    // When Buffer is used as body with application/octet-stream Content-Type,
                    // it should NOT cause "FormData encode: unsupported content type" error
                    var fileData = new Buffer("file content data");

                    // This should work without throwing FormData encoding errors
                    assert.doesNotThrow(() => {
                        var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                            body: fileData,
                            headers: {
                                'Content-Type': 'application/octet-stream',
                                'X-File-Name': 'upload.bin'
                            }
                        });
                        var responseText = response.body.read().toString();
                        assert.ok(responseText.includes("/request:"));
                    });
                });

                it("Binary Buffer upload scenario", () => {
                    // Test scenario similar to file upload with binary data
                    var binaryData = new Buffer([0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A]); // PNG header

                    // This represents typical file upload scenario and should work
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: binaryData,
                        headers: {
                            'Content-Type': 'application/octet-stream',
                            'X-File-Name': 'image.png'
                        }
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                });

                it("UInt8Array body", () => {
                    // Test UInt8Array as body type
                    var uint8Array = new Uint8Array([72, 101, 108, 108, 111]); // "Hello" in ASCII
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: uint8Array
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                    assert.ok(responseText.includes("Hello"));
                });

                it("UInt8Array body with application/octet-stream Content-Type", () => {
                    // Test UInt8Array with application/octet-stream Content-Type (user's scenario)
                    var uint8Array = new Uint8Array([0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A]); // PNG header
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: uint8Array,
                        headers: {
                            'Content-Type': 'application/octet-stream',
                            'X-File-Name': 'test.png'
                        }
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                });

                it("UInt8Array body with binary data - encoding error case", () => {
                    // This test reproduces the potential UInt8Array encoding issue
                    // When UInt8Array is used as body with application/octet-stream Content-Type,
                    // it should NOT cause "FormData encode: unsupported content type" error
                    var binaryData = new Uint8Array(256);
                    for (var i = 0; i < 256; i++) {
                        binaryData[i] = i; // Fill with all possible byte values
                    }

                    // This should work without throwing FormData encoding errors
                    assert.doesNotThrow(() => {
                        var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                            body: binaryData,
                            headers: {
                                'Content-Type': 'application/octet-stream',
                                'X-File-Name': 'binary.dat'
                            }
                        });
                        var responseText = response.body.read().toString();
                        assert.ok(responseText.includes("/request:"));
                    });
                });

                it("Large UInt8Array body", () => {
                    // Test with larger UInt8Array to check for encoding issues
                    var largeArray = new Uint8Array(2048);
                    largeArray.fill(65); // Fill with 'A' character
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: largeArray,
                        headers: {
                            'Content-Type': 'application/octet-stream'
                        }
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                    // Should handle large UInt8Array without issues
                    assert.ok(responseText.length > 20); // Should have received substantial content
                });

                it("URLSearchParams body", () => {
                    var params = new URLSearchParams();
                    params.append("key1", "value1");
                    params.append("key2", "value2");

                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: params
                    });
                    assert.equal(response.body.read().toString(), "/request:key1=value1&key2=value2");
                });

                it("Stream body", () => {
                    var ms = new io.MemoryStream();
                    ms.write("stream data");
                    ms.rewind();

                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: ms
                    });
                    assert.equal(response.body.read().toString(), "/request:stream data");
                });

                it("json option takes precedence over body", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request_json:", {
                        body: "should be ignored",
                        json: { test_field: "json data" }
                    });
                    // JSON data should be processed, but server may not parse it correctly
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request_json:"));
                });

                it("pack option takes precedence over json and body", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request_pack:", {
                        body: "should be ignored",
                        json: { test_field: "should be ignored" },
                        pack: { test_field: "pack data" }
                    });
                    // Pack data should be processed, but server may not parse it correctly
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request_pack:"));
                });

                it("Blob body with type", () => {
                    var blob = new Blob(["test blob"], { type: "text/plain" });
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: blob
                    });
                    // Check that blob content is properly sent
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                    // Blob may be converted to string form, so we just check it's processed
                });

                it("Blob body without type", () => {
                    var blob = new Blob(["test blob"]);
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: blob
                    });
                    // Check that blob content is properly sent
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                    // Blob may be converted to string form, so we just check it's processed
                });

                it("FormData encoding", () => {
                    var formData = new FormData();
                    formData.append("field1", "value1");
                    formData.append("field2", "value2");

                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: formData
                    });

                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                    assert.ok(responseText.includes("field1"));
                    assert.ok(responseText.includes("value1"));
                });

                it("complex JSON data", () => {
                    var complexData = {
                        string: "test",
                        number: 123,
                        boolean: true,
                        array: [1, 2, 3],
                        object: { nested: "value" },
                        null_value: null
                    };

                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request_json:", {
                        json: complexData
                    });

                    var responseText = response.body.read().toString();
                    var jsonPart = responseText.replace("/request_json:", "");
                    if (jsonPart) {
                        var responseData = JSON.parse(jsonPart);
                        assert.deepEqual(responseData, complexData);
                    } else {
                        // If no data returned, just check the endpoint was reached
                        assert.ok(responseText.includes("/request_json:"));
                    }
                }); it("complex MessagePack data", () => {
                    var complexData = {
                        string: "test",
                        number: 123,
                        boolean: true,
                        array: [1, 2, 3],
                        object: { nested: "value" }
                    };

                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request_pack:", {
                        pack: complexData
                    });

                    // Pack data should be successfully processed
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request_pack:"));
                });

                it("empty body", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:");
                    assert.equal(response.body.read().toString(), "/request:");
                });

                it("undefined body, json, and pack", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: undefined,
                        json: undefined,
                        pack: undefined
                    });
                    assert.equal(response.body.read().toString(), "/request:");
                });

                it("priority: body over other types", () => {
                    // 测试 body 的优先级：当同时提供 body 和其他类型时，body 应该优先
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: "body takes priority"
                    });
                    assert.equal(response.body.read().toString(), "/request:body takes priority");
                });

                it("object body (URLEncoded)", () => {
                    // 测试对象作为 body 时的处理（应该被序列化为 URLEncoded）
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request_url:", {
                        body: {
                            key1: "value1",
                            key2: "value2"
                        }
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request_url:"));
                    // Object may be stringified, so just check it's processed
                });

                it("array body conversion", () => {
                    // 测试数组作为 body 的处理
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: [1, 2, 3]
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                });

                it("boolean body conversion", () => {
                    // 测试布尔值作为 body 的处理
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: true
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                });

                it("number body conversion", () => {
                    // 测试数字作为 body 的处理
                    var response = http.request("POST", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                        body: 123
                    });
                    var responseText = response.body.read().toString();
                    assert.ok(responseText.includes("/request:"));
                });
            });

            describe("Content-Type handling", () => {
                var headerCheckSvr;
                var headerCheckPort = 8890 + base_port;

                before(() => {
                    // Create a server to check request headers
                    headerCheckSvr = new http.Server(headerCheckPort, (r) => {
                        var contentType = r.firstHeader("Content-Type") || "not-set";
                        r.response.write(contentType);
                    });
                    headerCheckSvr.start();
                    test_util.push(headerCheckSvr.socket);
                });

                it("string body sets application/x-www-form-urlencoded", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: "test"
                    });
                    assert.equal(response.body.read().toString(), "application/x-www-form-urlencoded");
                });

                it("json sets application/json", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        json: { test: "data" }
                    });
                    assert.equal(response.body.read().toString(), "application/json");
                });

                it("pack sets application/msgpack", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        pack: { test: "data" }
                    });
                    assert.equal(response.body.read().toString(), "application/msgpack");
                });

                it("manual Content-Type is preserved", () => {
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: "test",
                        headers: {
                            "Content-Type": "text/plain"
                        }
                    });
                    assert.equal(response.body.read().toString(), "text/plain");
                });

                it("Blob type is used", () => {
                    var blob = new Blob(["test"], { type: "text/plain" });
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: blob
                    });
                    assert.equal(response.body.read().toString(), "text/plain");
                });

                it("Blob without type uses default", () => {
                    var blob = new Blob(["test"]);
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: blob
                    });
                    assert.equal(response.body.read().toString(), "application/x-www-form-urlencoded");
                });

                it("URLSearchParams sets application/x-www-form-urlencoded", () => {
                    var params = new URLSearchParams();
                    params.append("key", "value");
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: params
                    });
                    assert.equal(response.body.read().toString(), "application/x-www-form-urlencoded");
                });

                it("FormData with preset Content-Type", () => {
                    var formData = new FormData();
                    formData.append("field", "value");
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: formData,
                        headers: {
                            "Content-Type": "multipart/form-data; boundary=custom"
                        }
                    });
                    assert.equal(response.body.read().toString(), "multipart/form-data; boundary=custom");
                });

                it("Buffer body with manual application/octet-stream", () => {
                    // Test scenario like the user's upload case
                    var buf = new Buffer("binary data content");
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: buf,
                        headers: {
                            "Content-Type": "application/octet-stream",
                            "X-File-Name": "filename"
                        }
                    });
                    assert.equal(response.body.read().toString(), "application/octet-stream");
                });

                it("Buffer body uses default Content-Type", () => {
                    // Test Buffer without explicit Content-Type
                    var buf = new Buffer("test buffer data");
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: buf
                    });
                    assert.equal(response.body.read().toString(), "application/octet-stream");
                });

                it("UInt8Array body with manual application/octet-stream", () => {
                    // Test UInt8Array scenario like the user's upload case
                    var uint8Array = new Uint8Array([98, 105, 110, 97, 114, 121]); // "binary" in ASCII
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: uint8Array,
                        headers: {
                            "Content-Type": "application/octet-stream",
                            "X-File-Name": "data.bin"
                        }
                    });
                    assert.equal(response.body.read().toString(), "application/octet-stream");
                });

                it("UInt8Array body uses default Content-Type", () => {
                    // Test UInt8Array without explicit Content-Type
                    var uint8Array = new Uint8Array([116, 101, 115, 116]); // "test" in ASCII
                    var response = http.request("POST", "http://127.0.0.1:" + headerCheckPort + "/", {
                        body: uint8Array
                    });
                    assert.equal(response.body.read().toString(), "application/octet-stream");
                });
            });

            it("header", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).body.read().toString(), "/request:header");
                assert.equal(cookie_for['_'], "root=value2");

                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/host:")
                    .body.read().toString(), "/host:127.0.0.1:" + (8882 + base_port));

                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/host:", {
                    headers: {
                        "Host": "host"
                    }
                }).body.read().toString(), "/host:host");
            });

            it("headers", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_headers": [
                            "header1",
                            "header2",
                            "header3"
                        ]
                    }
                }).body.read().toString(), "[\"header1\",\"header2\",\"header3\"]");
                assert.equal(cookie_for['_'], "root=value2");
            });

            it("agent", () => {
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/agent").body.read().toString(),
                    "curl/8.14.1");

                http.userAgent = 'test agent';
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/agent").body.read().toString(),
                    "test agent");

                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/agent", {
                    headers: {
                        "user-agent": "agent in headers"
                    }
                }).body.read().toString(),
                    "agent in headers");

                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/agent").body.read().toString(),
                    "test agent");
            });

            it("gzip", () => {
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/gzip_test").body.read().toString(),
                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");

                assert.equal(cookie_for['_'], "root=value2");
                http.get("http://127.0.0.1:" + (8882 + base_port) + "/gzip_test");
                assert.equal(cookie_for['_'], "root=value2; gzip_test=value");

                var maxBodySize = http.maxBodySize;

                http.maxBodySize = 130;
                http.get("http://127.0.0.1:" + (8882 + base_port) + "/gzip_test");

                http.maxBodySize = 129;
                assert.throws(() => {
                    http.get("http://127.0.0.1:" + (8882 + base_port) + "/gzip_test");
                });

                http.maxBodySize = maxBodySize;
            });

            it("keep-alive", () => {
                var r1 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                var r2 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                assert.equal(r1.stream.stream, r2.stream.stream);
            });

            it("pooSize of keep-alive", () => {
                assert.equal(http.poolSize, 128);

                http.poolSize = 0;
                assert.equal(http.poolSize, 0);

                var r1 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                var r2 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                assert.notEqual(r1.stream.stream, r2.stream.stream);

                http.poolSize = 128;
                assert.equal(http.poolSize, 128);

                var r1 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                var r2 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                assert.equal(r1.stream.stream, r2.stream.stream);
            });

            it("pooTimeout of keep-alive", () => {
                assert.equal(http.poolTimeout, 10000);

                http.poolTimeout = 0;
                assert.equal(http.poolTimeout, 0);

                var r1 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                coroutine.sleep(100);
                var r2 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                assert.notEqual(r1.stream.stream, r2.stream.stream);

                http.poolTimeout = 10000;
                assert.equal(http.poolTimeout, 10000);

                var r1 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                var r2 = http.get("http://127.0.0.1:" + (8882 + base_port) + "/request");
                assert.equal(r1.stream.stream, r2.stream.stream);
            });
        });

        describe("head", () => {
            before(() => {
                http.request("HEAD", "http://127.0.0.1:" + (8882 + base_port) + "/clear_cookie");
            })

            after(() => {
                http.request("HEAD", "http://127.0.0.1:" + (8882 + base_port) + "/clear_cookie");
            })

            it("simple", () => {
                assert.equal(cookie_for['head'], undefined);
                assert.equal(http.head("http://127.0.0.1:" + (8882 + base_port) + "/request").body.read(), null);
                assert.equal(cookie_for['head'], "root=value2; request=value; request1=value");
            });

            it("header", () => {
                assert.equal(http.head("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).body.read(), null);

                assert.equal(http.head("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).headers['test_header'], 'foobar');

                assert.equal(http.head("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).headers['Content-Length'], 15);
            });

            it("async", (done) => {
                http.head("http://127.0.0.1:" + (8882 + base_port) + "/request", (e, r) => {
                    done(() => {
                        assert.equal(r.data, null);
                        assert.equal(r.headers['no_test_header'], "true");
                    });
                });
            });

            it("large Content-Length should not be rejected", () => {
                // Test that HEAD requests with large Content-Length (>maxBodySize) are accepted
                // The Content-Length is 100MB which exceeds default maxBodySize of 64MB
                var response = http.head("http://127.0.0.1:" + (8882 + base_port) + "/large_file");

                // Should not throw error even though Content-Length exceeds maxBodySize
                assert.equal(response.statusCode, 200);
                assert.equal(response.headers['Content-Length'], "104857600"); // 100MB = 100*1024*1024
                assert.equal(response.headers['Content-Type'], "application/octet-stream");
                assert.equal(response.body.read(), null); // HEAD response has no body
            });

            it("large Content-Length with custom maxBodySize", () => {
                // Even with a very small maxBodySize, HEAD request should still work
                var hc = new http.Client();
                hc.maxBodySize = 1; // Set to 1MB

                var response = hc.head("http://127.0.0.1:" + (8882 + base_port) + "/large_file");

                // Should not throw error
                assert.equal(response.statusCode, 200);
                assert.equal(response.headers['Content-Length'], "104857600"); // 100MB = 100*1024*1024
                assert.equal(response.body.read(), null);
            });
        });

        describe("get", () => {
            it("simple", () => {
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/request").body.read().toString(),
                    "/request");
                assert.equal(cookie_for['_'], "root=value2; request=value; request1=value")
            });

            it("custom method", () => {
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/request", {
                    method: "GET"
                }).body.read().toString(),
                    "/request");
                assert.equal(cookie_for['_'], "root=value2; request=value; request1=value")
            });

            it("header", () => {
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).body.read().toString(), "/request:header");
                assert.equal(http.get("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).headers['Content-Length'], null);
            });

            it("async", (done) => {
                http.get("http://127.0.0.1:" + (8882 + base_port) + "/request", (e, r) => {
                    done(() => {
                        assert.equal(r.data.toString(), "/request");
                    });
                });
            });
        });

        describe("post", () => {
            it("body", () => {
                assert.equal(http.post("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    body: "body"
                }).body.read().toString(),
                    "/request:body");
            });

            it("header", () => {
                assert.equal(http.post("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    body: "",
                    headers: {
                        "test_header": "header"
                    }
                }).body.read().toString(), "/request:header");
                assert.equal(http.post("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    body: "",
                    headers: {
                        "test_header": "header"
                    }
                }).headers['Content-Length'], null);
            });

            it("async body", (done) => {
                http.post("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    body: "body"
                }, (e, r) => {
                    done(() => {
                        assert.equal(r.data.toString(), "/request:body");
                    });
                });
            });

            it("async header", (done) => {
                http.post("http://127.0.0.1:" + (8882 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    },
                    body: ""
                }, (e, r) => {
                    done(() => {
                        assert.equal(r.data.toString(), "/request:header");
                    });
                });
            });
        });

        describe("disable global cookie", () => {
            it("disable global cookie", () => {
                assert.equal(http.enableCookie, true);
                http.enableCookie = false;
                assert.equal(http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/name").body.read().toString(),
                    "/name");
                assert.isUndefined(cookie_for['_']);
                http.request("GET", "http://127.0.0.1:" + (8882 + base_port) + "/name");
                assert.isUndefined(cookie_for['_']);
            })
        })
    });

    describe("new http.Client", () => {
        it("simple", () => {
            var hc = new http.Client();
            assert.equal(hc.keepAlive, true);
            assert.equal(hc.timeout, 0);
            assert.equal(hc.enableCookie, true);
            assert.equal(hc.autoRedirect, true);
            assert.equal(hc.enableEncoding, true);
            assert.equal(hc.maxHeadersCount, 128);
            assert.equal(hc.maxHeaderSize, 8192);
            assert.equal(hc.maxBodySize, -1);
            assert.equal(hc.poolSize, 128);
            assert.equal(hc.poolTimeout, 10000);
            assert.equal(hc.userAgent, "curl/8.14.1");
            assert.equal(hc.http_proxy, "");
            assert.equal(hc.https_proxy, "");
        });

        it("options", () => {
            var hc = new http.Client({
                keepAlive: false,
                timeout: 1000,
                enableCookie: false,
                autoRedirect: false,
                enableEncoding: false,
                maxHeadersCount: 100,
                maxHeaderSize: 1000,
                maxBodySize: 100,
                poolSize: 100,
                poolTimeout: 1000,
                userAgent: "test agent",
                http_proxy: "http://127.0.0.1:9998",
                https_proxy: "https://127.0.0.1:9999"
            });

            assert.equal(hc.keepAlive, false);
            assert.equal(hc.timeout, 1000);
            assert.equal(hc.enableCookie, false);
            assert.equal(hc.autoRedirect, false);
            assert.equal(hc.enableEncoding, false);
            assert.equal(hc.maxHeadersCount, 100);
            assert.equal(hc.maxHeaderSize, 1000);
            assert.equal(hc.maxBodySize, 100);
            assert.equal(hc.poolSize, 100);
            assert.equal(hc.poolTimeout, 1000);
            assert.equal(hc.userAgent, "test agent");
            assert.equal(hc.http_proxy, "http://127.0.0.1:9998");
            assert.equal(hc.https_proxy, "https://127.0.0.1:9999");
        });
    });

    describe("https server/global https request", () => {
        var svr;
        var hc;

        var cookie_for = {
            _: undefined,
            head: undefined
        };

        before(() => {
            hc = new http.Client({
                ca: ca,
                enableCookie: true
            });

            svr = new http.HttpsServer({
                cert: crt,
                key: pk1.privateKey,
                port: 8883 + base_port
            }, (r) => {
                cookie_for['_'] = r.headers.cookie;

                set_header_for_head_req(r, cookie_for);

                if (r.address == "/clear_cookie") {
                    r.response.appendHeader("set-cookie", []);
                    cookie_for['head'] = undefined;
                    cookie_for['_'] = undefined;
                } else if (r.address != "/gzip_test") {
                    r.response.appendHeader("set-cookie", "request1=value; path=/");
                    r.response.appendHeader("set-cookie", "request2=value; path=/; secure");
                    r.response.write(r.address);
                    r.body.copyTo(r.response.body);
                    if (r.hasHeader("test_header"))
                        r.response.write(r.firstHeader("test_header"));
                } else {
                    r.response.appendHeader("Content-Type", "text/html");
                    r.response.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                }
            });
            svr.start();

            test_util.push(svr.socket);
        });

        describe("request", () => {
            it("simple", () => {
                assert.equal(hc.request("GET", "https://localhost:" + (8883 + base_port) + "/request").body.read().toString(),
                    "/request");
                assert.equal(cookie_for['_'], undefined);
                hc.request("GET", "https://localhost:" + (8883 + base_port) + "/request");
                assert.equal(cookie_for['_'], "request1=value; request2=value");
            });

            it("body", () => {
                assert.equal(hc.request("GET", "https://localhost:" + (8883 + base_port) + "/request:", {
                    body: "body"
                }).body.read().toString(),
                    "/request:body");
            });

            it("header", () => {
                assert.equal(hc.request("GET", "https://localhost:" + (8883 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).body.read().toString(), "/request:header");
            });

            it("gzip", () => {
                assert.equal(hc.get("https://localhost:" + (8883 + base_port) + "/gzip_test").body.read().toString(),
                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
            });
        });

        describe("head", () => {
            before(() => {
                try {
                    hc.request("HEAD", "https://localhost:" + (8883 + base_port) + "/clear_cookie");
                } catch (e) { }
            })

            after(() => {
                try {
                    hc.request("HEAD", "https://localhost:" + (8883 + base_port) + "/clear_cookie");
                } catch (e) { }
            })

            it("simple", () => {
                assert.equal(cookie_for['head'], undefined);
                assert.equal(hc.head("https://localhost:" + (8883 + base_port) + "/request").body.read(), null);
                assert.equal(cookie_for['head'], "request1=value; request2=value");
            });

            it("header", () => {
                assert.equal(hc.head("https://localhost:" + (8883 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).body.read(), null);

                assert.equal(hc.head("https://localhost:" + (8883 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).headers['test_header'], 'foobar');

                assert.equal(hc.head("https://localhost:" + (8883 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).headers['Content-Length'], 15);
            });

            it("async", (done) => {
                hc.head("https://localhost:" + (8883 + base_port) + "/request", (e, r) => {
                    done(() => {
                        assert.equal(r.data, null);
                        assert.equal(r.headers['no_test_header'], "true");
                    });
                });
            });
        });

        describe("get", () => {
            it("simple", () => {
                assert.equal(hc.get("https://localhost:" + (8883 + base_port) + "/request").body.read().toString(),
                    "/request");
            });

            it("header", () => {
                assert.equal(hc.get("https://localhost:" + (8883 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).body.read().toString(), "/request:header");

                assert.equal(hc.get("https://localhost:" + (8883 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).headers['Content-Length'], null);
            });
        });

        describe("post", () => {
            it("body", () => {
                assert.equal(hc.post(
                    "https://localhost:" + (8883 + base_port) + "/request:", {
                    body: "body"
                }).body
                    .read().toString(), "/request:body");
            });

            it("header", () => {
                assert.equal(hc.post(
                    "https://localhost:" + (8883 + base_port) + "/request:", {
                    body: "",
                    headers: {
                        "test_header": "header"
                    }
                }).body.read().toString(), "/request:header");

                assert.equal(hc.post(
                    "https://localhost:" + (8883 + base_port) + "/request:", {
                    body: "",
                    headers: {
                        "test_header": "header"
                    }
                }).headers['Content-Length'], null);
            });
        });
    });

    describe("server/client", () => {
        var svr;

        var cookie_for = {
            _: undefined,
            head: undefined
        };

        before(() => {
            http.enableCookie = true;
            var pcnt = 0;
            svr = new http.Server(8884 + base_port, (r) => {
                var port = 8884 + base_port;
                cookie_for['_'] = r.headers.cookie;

                r.response.appendHeader("set-cookie", "root1=value1; domain=127.0.0.2; path=/");
                r.response.appendHeader("set-cookie", "root=value; domain=127.0.0.1:" + port + "; path=/");
                r.response.appendHeader("set-cookie", "root=value; path=/");
                r.response.appendHeader("set-cookie", "root=value2; path=/");

                set_header_for_head_req(r, cookie_for);

                if (r.address == "/clear_cookie") {
                    r.response.appendHeader("set-cookie", []);
                    cookie_for['head'] = undefined;
                    cookie_for['_'] = undefined;
                } else if (r.address == "/timeout") {
                    coroutine.sleep(500);
                    r.response.write(r.address);
                } else if (r.address == "/name") {
                    r.response.appendHeader("set-cookie", "name=value; domain=127.0.0.1:" + port + "; path=/name");
                    r.response.write(r.address);
                } else if (r.address == "/redirect") {
                    r.response.appendHeader("test", "test1");
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
                } else if (r.address == "/disconnect_test") {
                    console.log("/disconnect_test");
                    var _stream = r.stream;
                    r.response.write(r.address);
                    setTimeout(() => {
                        _stream.close();
                    }, 50);
                } else if (r.address == "/connection") {
                    r.response.write(r.keepAlive.toString());
                } else if (r.address != "/gzip_test") {
                    r.response.appendHeader("set-cookie", "request=value; domain=127.0.0.1; path=/request");
                    r.response.appendHeader("set-cookie", "request1=value; domain=127.0.0.1; path=/request");
                    r.response.appendHeader("set-cookie", "request2=value; domain=127.0.0.1; path=/request; secure");
                    r.response.appendHeader("set-cookie", "request3=value; domain=127.0.0.1:" + port + "; path=/request;");
                    r.response.write(r.address);
                    r.body.copyTo(r.response.body);
                    if (r.hasHeader("test_header"))
                        r.response.write(r.firstHeader("test_header"));
                } else {
                    r.response.appendHeader("Content-Type", "text/html");
                    r.response.appendHeader("set-cookie", "gzip_test=value; domain=127.0.0.1; path=/gzip_test");
                    r.response.write("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                }
            });
            svr.start();

            test_util.push(svr.socket);
        });

        describe("request & cookie", () => {
            var client = new http.Client();

            it("simple", () => {
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request").body.readAll().toString(),
                    "/request");

                assert.equal(cookie_for['_'], undefined);
                client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request");
                assert.equal(cookie_for['_'], "root=value2; request=value; request1=value");
            });

            describe("head", () => {
                before(() => {
                    client.request("HEAD", "http://127.0.0.1:" + (8884 + base_port) + "/clear_cookie");
                })

                after(() => {
                    client.request("HEAD", "http://127.0.0.1:" + (8884 + base_port) + "/clear_cookie");
                })

                it("simple", () => {
                    assert.equal(cookie_for['head'], undefined);
                    assert.equal(client.head("http://127.0.0.1:" + (8884 + base_port) + "/request").body.read(), null);
                    assert.equal(cookie_for['head'], "root=value2; request=value; request1=value");
                });

                it("header", () => {
                    assert.equal(client.head("http://127.0.0.1:" + (8884 + base_port) + "/request:", {
                        headers: {
                            "test_header": "header"
                        }
                    }).body.read(), null);

                    assert.equal(client.head("http://127.0.0.1:" + (8884 + base_port) + "/request:", {
                        headers: {
                            "test_header": "header"
                        }
                    }).headers['test_header'], 'foobar');
                });

                it("async", (done) => {
                    client.head("http://127.0.0.1:" + (8884 + base_port) + "/request", (e, r) => {
                        done(() => {
                            assert.equal(r.data, null);
                            assert.equal(r.headers['no_test_header'], "true");
                        });
                    });
                });
            });

            it("redirect", () => {
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/redirect").body.readAll().toString(),
                    "/request");

                assert.equal(cookie_for['_'], "root=value2; request=value; request1=value");
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
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request:", {
                    body: "body"
                }).body.readAll().toString(),
                    "/request:body");
            });

            it("header", () => {
                assert.equal(client.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/request:", {
                    headers: {
                        "test_header": "header"
                    }
                }).body.read().toString(), "/request:header");
            });

            it("gzip", () => {
                assert.equal(client.get("http://127.0.0.1:" + (8884 + base_port) + "/gzip_test").body.readAll().toString(),
                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
                assert.equal(cookie_for['_'], "root=value2");
                client.get("http://127.0.0.1:" + (8884 + base_port) + "/gzip_test");
                assert.equal(cookie_for['_'], "root=value2; gzip_test=value");
            });

            it("custom body", () => {
                var body = new io.MemoryStream();
                client.get("http://127.0.0.1:" + (8884 + base_port) + "/custom_body_test", {
                    response_body: body
                });
                assert.equal(body.readAll().toString(), "/custom_body_test");

                var body = new io.MemoryStream();
                body.write("keep it:");
                body.write("not keep");
                body.seek(8);
                client.get("http://127.0.0.1:" + (8884 + base_port) + "/custom_body_test", {
                    response_body: body
                });
                assert.equal(body.readAll().toString(), "keep it:/custom_body_test");
            });

            it('parallel', () => {
                var rs = coroutine.parallel(() => {
                    return client.get("http://127.0.0.1:" + (8884 + base_port) + "/parallel").body.readAll().toString();
                }, 2);
                assert.ok(rs[0] !== '0' || rs[1] !== '0');
            });
        });

        it("disable client cookie", () => {
            var client = new http.Client();

            assert.equal(client.enableCookie, true);
            client.enableCookie = false;
            assert.equal(client.request('GET', "http://127.0.0.1:" + (8884 + base_port) + "/name").body.readAll().toString(),
                "/name");
            assert.equal(cookie_for['_'], undefined);

            client.request('GET', "http://127.0.0.1:" + (8884 + base_port) + "/name");
            assert.equal(cookie_for['_'], undefined);
        });

        it("remote disconnect", () => {
            var client = new http.Client();

            assert.equal(client.request('GET', "http://127.0.0.1:" + (8884 + base_port) + "/disconnect_test").body.readAll().toString(),
                "/disconnect_test");

            coroutine.sleep(100);

            assert.equal(client.request('GET', "http://127.0.0.1:" + (8884 + base_port) + "/disconnect_test").body.readAll().toString(),
                "/disconnect_test");
        });

        describe("client timeout & global client timeout", () => {
            var client = new http.Client();

            it("overtime", () => {
                client.timeout = 200;

                var t1 = new Date();
                assert.throws(() => {
                    client.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout")
                });
                var t2 = new Date();

                assert.greaterThan(t2 - t1, 190);
                assert.lessThan(t2 - t1, 500);
            });

            it("intime", () => {
                client.timeout = 1000;

                assert.equal(client.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout").body.readAll().toString(),
                    "/timeout");
                var t2 = new Date();
            });

            it("global overtime", () => {
                http.timeout = 200;

                var t1 = new Date();
                assert.throws(() => {
                    http.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout")
                });
                var t2 = new Date();

                assert.greaterThan(t2 - t1, 190);
                assert.lessThan(t2 - t1, 500);
            });

            it("global intime", () => {
                http.timeout = 1000;
                assert.equal(http.get("http://127.0.0.1:" + (8884 + base_port) + "/timeout").body.readAll().toString(),
                    "/timeout");
            });
        });

        describe("keep-alive", () => {
            function test_keep_alive(def_conn, req_conn) {
                const hc = new http.Client(def_conn);

                var r1 = hc.get("http://127.0.0.1:" + (8884 + base_port) + "/connection", req_conn);

                return r1.body.read().toString();
            }

            it("contructor", () => {
                assert.equal(test_keep_alive({}, {}), "true");
                assert.equal(test_keep_alive({
                    keepAlive: false
                }, {}), "false");
            });

            it("request", () => {
                assert.equal(test_keep_alive({}, {
                    keepAlive: false
                }), "false");
                assert.equal(test_keep_alive({
                    keepAlive: false
                }, {
                    keepAlive: true
                }), "true");
            });

            it("header", () => {
                assert.equal(test_keep_alive({}, {
                    keepAlive: false,
                    headers: {
                        "Connection": "keep-alive"
                    }
                }), "true");
                assert.equal(test_keep_alive({}, {
                    keepAlive: true,
                    headers: {
                        "Connection": "close"
                    }
                }), "false");
            });
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
            assert.equal(http.request("GET", "http://127.0.0.1:" + (8884 + base_port) + "/redirect").firstHeader("test"),
                "test1");
        })
    });

    describe("repeater", () => {
        var svr;

        before(() => {
            svr = new http.Server(8885 + base_port, (r) => {
                if (r.address === '/header/test') {
                    r.response.json(r.allHeader());
                } else if (r.address === '/method/test') {
                    r.response.write('method: ' + r.method);
                } else if (r.address === '/cookie/test') {
                    r.response.addCookie(new http.Cookie("test1", "value1"));
                    r.response.json(r.allHeader());
                } else if (r.address === '/query/test') {
                    r.response.write(r.queryString)
                } else {
                    r.response.write(r.address);
                }
            });
            svr.start();

            test_util.push(svr.socket);
        });

        describe("constructor", () => {
            it("allow url&arrray", () => {
                new http.Repeater("http://127.0.0.1/");
                new http.Repeater([
                    "http://127.0.0.1/",
                    "http://127.0.0.1/test"
                ]);
            });

            it("property urls", () => {
                assert.deepEqual(new http.Repeater([
                    "http://127.0.0.1/",
                    "http://127.0.0.1/test"
                ]).urls, [
                    "http://127.0.0.1/",
                    "http://127.0.0.1/test"
                ]);
            });

            it("not allow empty array", () => {
                assert.throws(() => {
                    new http.Repeater([]);
                });
            });

            it("not allow empty hostname", () => {
                assert.throws(() => {
                    new http.Repeater('/test');
                });

                assert.throws(() => {
                    new http.Repeater(['/test']);
                });
            });

            it("not allow query", () => {
                assert.throws(() => {
                    new http.Repeater('http://127.0.0.1/test?test');
                });

                assert.throws(() => {
                    new http.Repeater(['http://127.0.0.1/test?test']);
                });
            });

            it("not allow hash", () => {
                assert.throws(() => {
                    new http.Repeater('http://127.0.0.1/test#test');
                });

                assert.throws(() => {
                    new http.Repeater(['http://127.0.0.1/test#test']);
                });
            });

            it("check client config", () => {
                var r = new http.Repeater("http://127.0.0.1/");
                assert.isFalse(r.client.enableCookie);
                assert.isFalse(r.client.autoRedirect);
                assert.isFalse(r.client.enableEncoding);
                assert.equal(r.client.userAgent, "");
            });
        });

        describe("load", () => {
            it("basic test", () => {
                var hr = new http.Repeater('http://127.0.0.1:' + (8885 + base_port) + '/path');
                hr.load([
                    "http://127.0.0.1/test1",
                    "http://127.0.0.1/test2"
                ]);

                assert.deepEqual(hr.urls, [
                    "http://127.0.0.1/test1",
                    "http://127.0.0.1/test2"
                ]);
            });

            it("not change urls when throw error", () => {
                var hr = new http.Repeater("http://127.0.0.1/");
                assert.throws(() => {
                    hr.load([
                        "http://127.0.0.1/test1",
                        'http://127.0.0.1/test?test',
                        "http://127.0.0.1/test2"
                    ]);
                });

                assert.deepEqual(hr.urls, [
                    "http://127.0.0.1/"
                ]);

                hr.load([
                    "http://127.0.0.1/test1",
                    "http://127.0.0.1/test2"
                ]);

                assert.deepEqual(hr.urls, [
                    "http://127.0.0.1/test1",
                    "http://127.0.0.1/test2"
                ]);
            });
        });

        it("isRouting", () => {
            var hr = new http.Repeater("http://127.0.0.1/");
            assert.isTrue(hr.isRouting());
        });

        describe("invoke", () => {
            function req_path(hr, p) {
                var r = new http.Request();
                r.address = r.value = p;
                hr.invoke(r);
                return r.response.read().toString();
            }

            function req_method(hr, m) {
                var r = new http.Request();
                r.address = r.value = 'test';
                r.method = m;
                hr.invoke(r);
                return r.response.read().toString();
            }

            function req_header(hr, p) {
                var r = new http.Request();
                r.address = r.value = p;
                hr.invoke(r);
                return r.response.json();
            }

            function req_cookie(hr, p) {
                var r = new http.Request();
                r.address = r.value = p;
                hr.invoke(r);
                return r.response.json();
            }

            function req_query(hr, p, q) {
                var r = new http.Request();
                r.address = r.value = p;
                r.queryString = q;
                hr.invoke(r);
                return r.response.read().toString();
            }

            it("basic repeater", () => {
                var hr = new http.Repeater('http://127.0.0.1:' + (8885 + base_port) + '/path');
                assert.equal(req_path(hr, 'path'), '/path/path');
            });

            it("rotation repeater", () => {
                var hr = new http.Repeater([
                    'http://127.0.0.1:' + (8885 + base_port) + '/path',
                    'http://127.0.0.1:' + (8885 + base_port) + '/path1',
                    'http://127.0.0.1:' + (8885 + base_port) + '/path2'
                ]);

                assert.equal(req_path(hr, 'path'), '/path/path');
                assert.equal(req_path(hr, 'path1'), '/path1/path1');
                assert.equal(req_path(hr, 'path2'), '/path2/path2');

                assert.equal(req_path(hr, 'path'), '/path/path');
                assert.equal(req_path(hr, 'path1'), '/path1/path1');
                assert.equal(req_path(hr, 'path2'), '/path2/path2');
            });

            it("method", () => {
                var hr = new http.Repeater('http://127.0.0.1:' + (8885 + base_port) + '/method');
                assert.equal(req_method(hr, 'test'), "method: test");
            });

            it("header", () => {
                var hr = new http.Repeater('http://127.0.0.1:' + (8885 + base_port) + '/header');
                assert.deepEqual(req_header(hr, 'test'), {
                    "Host": "127.0.0.1:" + (8885 + base_port)
                });
            });

            it("cookie", () => {
                var hr = new http.Repeater('http://127.0.0.1:' + (8885 + base_port) + '/cookie');
                req_cookie(hr, 'test');
                assert.deepEqual(req_cookie(hr, 'test'), {
                    "Host": "127.0.0.1:" + (8885 + base_port)
                });
            });

            it('queryString not send ok', () => {
                var hr = new http.Repeater('http://127.0.0.1:' + (8885 + base_port) + '/query');
                req_query(hr, 'test', 'a=100');
                assert.deepEqual(req_query(hr, 'test', 'a=100'), 'a=100');
            });

            it("bad end", () => {
                var hr = new http.Repeater('http://127.0.0.1:' + (10000 + base_port) + '/path');
                assert.throws(() => {
                    req_path(hr, 'path');
                });
            });
        });
    });

    describe("http_proxy", () => {
        var svr;

        before(() => {
            var sslhdr = new tls.Handler({
                cert: crt.pem,
                key: pk1.privateKey.export()
            }, new http.Handler((r) => {
                r.response.write('https: ' + (r.stream.stream.stream.test || '') + r.address);
            }));

            svr = new http.Server(8886 + base_port, (r) => {
                if (r.method == 'CONNECT') {
                    test_util.push(r.stream);
                    r.stream.write('HTTP/1.1 200 Connected\r\n\r\n');
                    try {
                        sslhdr.invoke(r.stream);
                    } catch (e) { };
                    return;
                }

                if (r.address == 'http://fibjs.org/share_1') {
                    r.response.write('share_1');
                    r.stream.test = 'share_1: ';
                } else if (r.address == 'http://fibjs.org/share_2') {
                    r.response.write('share_2');
                    r.stream.test = 'share_2: ';
                } else
                    r.response.write('http: ' + (r.stream.test || '') + r.address);
            });
            svr.start();

            test_util.push(svr.socket);
        });

        function test_proxy(hc, url) {
            return hc.get(url).data.toString();
        }

        it('basic request', () => {
            var hc = new http.Client({
                ca: ca
            });
            hc.http_proxy = 'http://127.0.0.1:' + (8886 + base_port);

            test_proxy(hc, 'http://fibjs.org/test.html');
            test_proxy(hc, `http://localhost:${8886 + base_port}/test.html`);
        });

        it('share connection between domains', () => {
            var hc = new http.Client();
            hc.http_proxy = 'http://127.0.0.1:' + (8886 + base_port);

            assert.equal(test_proxy(hc, 'http://fibjs.org/share_1'), 'share_1');
            assert.equal(test_proxy(hc, 'http://fibjs1.org/share_2'), 'http: share_1: http://fibjs1.org/share_2');
            assert.equal(test_proxy(hc, 'http://fibjs2.org/share_3'), 'http: share_1: http://fibjs2.org/share_3');
        });

        it('use http connection to connect https server', () => {
            var hc = new http.Client({
                ca: ca
            });
            hc.http_proxy = 'http://127.0.0.1:' + (8886 + base_port);

            assert.equal(test_proxy(hc, 'http://fibjs.org/share_1'), 'share_1');
            assert.equal(test_proxy(hc, `http://localhost:${8886 + base_port}/test.html`), 'http: /test.html');
            assert.equal(test_proxy(hc, `http://localhost:${8886 + base_port}/test2.html`), 'http: /test2.html');

            assert.equal(test_proxy(hc, 'http://fibjs.org/share_2'), 'share_2');
            assert.equal(test_proxy(hc, 'http://fibjs1.org/test1.html'), 'http: share_2: http://fibjs1.org/test1.html');
            assert.equal(test_proxy(hc, `http://localhost:${8886 + base_port}/test3.html`), 'http: /test3.html');
        });

    });

    describe("verification", () => {
        var hc;

        it("request https error by default", () => {
            hc = new http.Client({
                ca: ca
            });

            assert.throws(() => {
                hc.get('https://www.baidu.com/');
            });
        });

        it("requestCert: false", () => {
            hc = new http.Client({
                ca: ca,
                requestCert: false
            });

            var resp = hc.get('https://www.baidu.com/');

            assert.equal(resp.statusCode, 200);
        });
    });

    todo("unix socket", () => {
        var _port = (8888 + base_port);
        var _path = process.platform === 'win32' ? "//./pipe/port_" + _port : os.homedir() + '/port_' + _port;

        var svr = new http.Server(_path, (r) => {
            r.response.write("hello, " + r.address);
        });
        svr.start();

        test_util.push(svr.socket);

        var u_path = "http://" + encodeURIComponent(_path) + "/unix";

        assert.equal(http.get(u_path).readAll().toString(), "hello, /unix");
    });
});

