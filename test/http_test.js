var test = require("test");
test.setup();

var io = require('io');
var fs = require('fs');
var http = require('http');
var net = require('net');
var encoding = require('encoding');
var zlib = require('zlib');
var coroutine = require("coroutine");

var ssl = require("ssl");
var crypto = require("crypto");

var pk = new crypto.PKey();
pk.genRsaKey(1024);

var crt = new crypto.X509Req("CN=localhost", pk).sign("CN=baoz.me", pk);
var ca = new crypto.X509Req("CN=baoz.me", pk).sign("CN=baoz.me", pk, {
	ca: true
});

var ca_pem = ca.dump()[0];

function Step() {
	this.step = 0;
	this.wait = function(n) {
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


describe("http", function() {
	describe("headers", function() {
		var d = new http.Request().headers;

		it("add", function() {
			d.add('a', '100');
			d.add('b', '200');
			d.add('a', '300');
			d.add('c', '400');
			d.add('d', '500');
			d.add('c', '600');
			d.add('d', '700');

			assert.equal(d['a'], '100');

			assert.equal(d.has('a'), true);
			assert.equal(d.first('a'), '100');
		});

		it("all", function() {
			var a = d.all('a');
			assert.deepEqual(a.toArray(), ['100', '300']);
		});

		it("remove", function() {
			d.remove('a');
			assert.isFalse(d.has('a'));
			assert.isNull(d.first('a'));

			assert.isUndefined(d['a']);

			assert.equal(d.first('c'), '400');

			var a = d.all('c');
			assert.deepEqual(a.toArray(), ['400', '600']);
		});

		it("set", function() {
			d.set('c', '800');

			var a = d.all('c');
			assert.equal(a.length, 1);
			assert.equal(a[0], '800');

			assert.equal(d.first('c'), '800');
		});

		it("add({})", function() {
			d.add({
				d: "900",
				b: "1000"
			});

			var a = d.all('d');
			assert.deepEqual(a.toArray(), ['500', '700', '900']);

			a = d.all('b');
			assert.deepEqual(a.toArray(), ['200', '1000']);
		});

		it("set({})", function() {
			d.set({
				d: "900",
				b: "1000"
			});

			var a = d.all('d');
			assert.deepEqual(a.toArray(), ['900']);

			a = d.all('b');
			assert.deepEqual(a.toArray(), ['1000']);
		});

		it("set other type", function() {
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

	describe("cookie", function() {
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

		it("build", function() {
			for (var i = 0; i < cases.length; i++)
				assert.equal(build(cases[i][0]), cases[i][1]);
		});

		it("parse", function() {
			for (var i = 0; i < cases.length; i++)
				assert.deepEqual(parse(cases[i][1]), cases[i][0]);

			assert.throws(function() {
				new http.Cookie().parse("");
			});

			assert.throws(function() {
				new http.Cookie().parse("aaa");
			});

			assert.throws(function() {
				new http.Cookie().parse("=");
			});

			assert.throws(function() {
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

		it("match", function() {
			for (var i = 0; i < match_cases.length; i++) {
				assert.equal(new http.Cookie(match_cases[i][0]).match(match_cases[i][1]),
					match_cases[i][2]);
			}
		});
	});

	describe("parse", function() {
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

		function get_response(txt) {
			var ms = new io.MemoryStream();
			var bs = new io.BufferedStream(ms);
			bs.EOL = "\r\n";

			bs.writeText(txt);
			ms.seek(0, fs.SEEK_SET);

			var req = new http.Response();
			req.readFrom(bs);
			return req;
		}

		it("bad request", function() {
			var bad_reqs = [
				" GET / HTTP/1.0\r\nkeepalive: close\r\n\r\n",
				"GET ? HTTP/1.0\r\nkeepalive: close\r\n\r\n",
				"GET / \r\nkeepalive: close\r\n\r\n",
				"GET / HTTP/a.0\r\nkeepalive: close\r\n\r\n",
				"GET / HTT/1.0\r\nkeepalive: close\r\n\r\n",
				"GET / HTTP/1.\r\nkeepalive: close\r\n\r\n"
			];

			function readreq(u) {
				assert.throws(function() {
					get_request(u);
				});
			}

			bad_reqs.forEach(readreq);
		});

		it("bad response", function() {
			assert.throws(function() {
				get_response("HTTP/1.0\r\n\r\n");
			});

			assert.throws(function() {
				get_response("HTTP/1.0 \r\n\r\n");
			});

			assert.throws(function() {
				get_response("HTTP/1.0,200\r\n\r\n");
			});

			assert.throws(function() {
				get_response("HTTP/1.0 1\r\n\r\n");
			});

			assert.throws(function() {
				get_response("HTTP/1.0 1111\r\n\r\n");
			});

			assert.throws(function() {
				get_response("HTTP/1.0 asd\r\n\r\n");
			});
		});

		it("proxy request", function() {
			var req = get_request("GET http://www.com/index.html HTTP/1.0\r\nkeepalive: close\r\n\r\n");
			assert.equal("http://www.com/index.html", req.address);
			assert.equal("/index.html", req.value);

			var req = get_request("GET http://www.com.index.html HTTP/1.0\r\nkeepalive: close\r\n\r\n");
			assert.equal("http://www.com.index.html", req.address);
			assert.equal("", req.value);
		});

		it("basic format", function() {
			var req = get_request("GET / HTTP/1.0\r\nhead1: 100\r\nhead2: 200\r\nContent-type:test\r\nContent-length:    10\r\n\r\n0123456789");

			assert.equal('100', req.headers['head1']);
			assert.equal('200', req.headers['head2']);
			assert.equal(10, req.length);
			assert.equal('test', req.headers['content-type']);
			assert.equal('0123456789', req.body.read());

			assert.equal(req.stream.size(), 94);
			req.clear();
			assert.equal(req.stream, null);

			var r = get_response("HTTP/1.0 200\r\n\r\n");
			assert.equal(r.status, 200);
			assert.equal(r.protocol, 'HTTP/1.0');
		});

		it("keep-alive", function() {
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

		it("request cookie", function() {
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

		it("response cookie", function() {

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

		it("query", function() {
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

		it("form", function() {
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
		it("chunk", function() {
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

	});

	describe("encode", function() {
		it("request", function() {
			var rep = new http.Request();
			rep.body.write("0123456789");

			var ms = new io.MemoryStream();

			rep.sendTo(ms);
			ms.rewind();
			assert.equal(ms.read(), 'GET / HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
		});

		it("response", function() {
			var ms = new io.MemoryStream();

			var rep = new http.Response();
			rep.body.write("0123456789");

			rep.sendTo(ms);
			ms.rewind();
			assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
		});

		it("response header only", function() {
			var ms = new io.MemoryStream();

			var rep = new http.Response();
			rep.body.write("0123456789");

			rep.sendHeader(ms);
			ms.rewind();
			assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n');
		});

		it("address", function() {
			var rep = new http.Request();
			rep.body.write("0123456789");
			rep.address = "/docs/";
			rep.value = "/docs/";

			var ms = new io.MemoryStream();

			rep.sendTo(ms);
			ms.rewind();
			assert.equal(ms.read(), 'GET /docs/ HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
		});

		it("query", function() {
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

		it("response cookies", function() {
			var ms = new io.MemoryStream();

			var rep = new http.Response();
			rep.addCookie(new http.Cookie("test", "value"));
			rep.addCookie(new http.Cookie("test1", "value1"));

			rep.sendTo(ms);
			ms.rewind();
			assert.equal(ms.read(), 'HTTP/1.1 200 OK\r\nSet-Cookie: test=value\r\nSet-Cookie: test1=value1\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n');
		});
	});

	describe("handler", function() {
		var svr, hdr;
		var c, bs;
		var st;

		before(function() {
			hdr = new http.Handler(function(r) {
				if (r.value == '/throw')
					throw new Error('throw test');
				else if (r.value == '/not_found')
					r.response.status = 404;
				else if (r.value == '/remote_close') {
					st.step = 1;
					st.wait(2);
				}
			});
			svr = new net.TcpServer(8881, hdr);

			svr.asyncRun();
		});

		after(function() {
			svr.socket.close();
		});

		beforeEach(function() {
			c = new net.Socket();
			c.connect('127.0.0.1', 8881);

			bs = new io.BufferedStream(c);
			bs.EOL = "\r\n";
		});

		afterEach(function() {
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

		it("normal request", function() {
			c.write("GET / HTTP/1.0\r\n\r\n");
			var req = get_response();
			assert.equal(req.status, 200);

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

		it("bad request(error 400)", function() {
			c.write("GET /\r\n\r\n");
			var req = get_response();
			assert.equal(req.status, 400);

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

		it("error 404", function() {
			c.write("GET /not_found HTTP/1.0\r\n\r\n");
			var req = get_response();
			assert.equal(req.status, 404);

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

		it("error 500", function() {
			c.write("GET /throw HTTP/1.0\r\n\r\n");
			var req = get_response();
			assert.equal(req.status, 500);

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

		it("remote close when response", function() {
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

		it("remote close when request", function() {
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

		it("reset stats", function() {
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
	});

	describe("file handler", function() {
		var hfHandler = new http.fileHandler('./');
		var url = 'test.html';
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
				fs.unlink('test.html');
			} catch (e) {};
			try {
				fs.unlink('test.html.gz');
			} catch (e) {};
		}

		before(clean);
		after(clean);

		it("file not found", function() {
			rep = hfh_test(url);
			assert.equal(404, rep.status);
			rep.clear();
		});

		it("normal", function() {
			fs.writeFile('test.html', 'test html file');

			rep = hfh_test(url);
			assert.equal(200, rep.status);
			assert.equal(14, rep.length);

			assert.deepEqual(new Date(rep.firstHeader('Last-Modified')), fs.stat('test.html').mtime);
		});

		it("not modified", function() {
			var rep1 = hfh_test(url, {
				'If-Modified-Since': rep.firstHeader('Last-Modified')
			});
			assert.equal(304, rep1.status);
			rep1.clear();
			rep.clear();
		});

		it("changed", function() {
			var rep = hfh_test(url, {
				'If-Modified-Since': new Date('1998-04-14 12:12:12')
			});
			assert.equal(200, rep.status);
			assert.equal(14, rep.length);
			assert.equal('text/html', rep.firstHeader('Content-Type'));
			rep.clear();
		});

		it("pre-gzip", function() {
			var sgz = 'gz test file';
			var gz = fs.open('test.html.gz', 'w');
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

		it("don't gzip small file", function() {
			fs.unlink('test.html.gz');

			var rep = hfh_test(url, {
				'Accept-Encoding': 'deflate,gzip'
			});
			assert.equal(200, rep.status);
			assert.equal(null, rep.firstHeader('Content-Encoding'));
			rep.clear();
		});
	});

	describe("server/request", function() {
		var svr;

		before(function() {
			svr = new http.Server(8882, function(r) {
				if (r.address != "/gzip_test") {
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
		});

		after(function() {
			svr.socket.close();
		});

		describe("request", function() {
			it("simple", function() {
				assert.equal(http.request("GET", "http://127.0.0.1:8882/request").body.read().toString(),
					"/request");
			});

			it("body", function() {
				assert.equal(http.request("GET", "http://127.0.0.1:8882/request:", "body").body.read().toString(),
					"/request:body");
			});

			it("header", function() {
				assert.equal(http.request("GET", "http://127.0.0.1:8882/request:", {
					"test_header": "header"
				}).body.read().toString(), "/request:header");
			});

			it("gzip", function() {
				assert.equal(http.get("http://127.0.0.1:8882/gzip_test").body.read().toString(),
					"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
			});
		});

		describe("get", function() {
			it("simple", function() {
				assert.equal(http.get("http://127.0.0.1:8882/request").body.read().toString(),
					"/request");
			});

			it("header", function() {
				assert.equal(http.get("http://127.0.0.1:8882/request:", {
					"test_header": "header"
				}).body.read().toString(), "/request:header");
			});
		});

		describe("post", function() {
			it("body", function() {
				assert.equal(http.post("http://127.0.0.1:8882/request:", "body").body.read().toString(),
					"/request:body");
			});

			it("header", function() {
				assert.equal(http.post("http://127.0.0.1:8882/request:", "", {
					"test_header": "header"
				}).body.read().toString(), "/request:header");
			});
		});
	});

	describe("https server/https request", function() {

		var svr;

		before(function() {
			ssl.ca.load(ca_pem);

			svr = new http.HttpsServer(crt, pk, 8883, function(r) {
				if (r.address != "/gzip_test") {
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
		});

		after(function() {
			svr.socket.close();
			ssl.ca.clear();
		});

		describe("request", function() {
			it("simple", function() {
				assert.equal(http.request("GET", "https://127.0.0.1:8883/request").body.read().toString(),
					"/request");
			});

			it("body", function() {
				assert.equal(http.request("GET", "https://127.0.0.1:8883/request:", "body").body.read().toString(),
					"/request:body");
			});

			it("header", function() {
				assert.equal(http.request("GET", "https://127.0.0.1:8883/request:", {
					"test_header": "header"
				}).body.read().toString(), "/request:header");
			});

			it("gzip", function() {
				assert.equal(http.get("https://127.0.0.1:8883/gzip_test").body.read().toString(),
					"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
			});
		});

		describe("get", function() {
			it("simple", function() {
				assert.equal(http.get("https://127.0.0.1:8883/request").body.read().toString(),
					"/request");
			});

			it("header", function() {
				assert.equal(http.get("https://127.0.0.1:8883/request:", {
					"test_header": "header"
				}).body.read().toString(), "/request:header");
			});
		});

		describe("post", function() {
			it("body", function() {
				assert.equal(http.post(
						"https://127.0.0.1:8883/request:", "body").body
					.read().toString(), "/request:body");
			});

			it("header", function() {
				assert.equal(http.post(
					"https://127.0.0.1:8883/request:", "", {
						"test_header": "header"
					}).body.read().toString(), "/request:header");
			});
		});
	});
});

//test.run(console.DEBUG);