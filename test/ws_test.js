var test = require("test");
test.setup();

var websocket = require('websocket');
var io = require('io');
var http = require('http');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

describe('websocket', function() {
	var ss = [];

	after(function() {
		ss.forEach(function(s) {
			s.close();
		});
	});

	function load_msg(data) {
		var ms = new io.MemoryStream();
		ms.write(data);
		ms.rewind();

		var msg = new websocket.Message();
		msg.readFrom(ms);

		return {
			masked: msg.masked,
			type: msg.type,
			data: msg.body.readAll().toString()
		};
	}

	it("readFrom", function() {
		assert.deepEqual(load_msg([0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f]), {
			"masked": false,
			"type": websocket.TEXT,
			"data": "Hello"
		});

		assert.deepEqual(load_msg([0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
			"masked": true,
			"type": websocket.TEXT,
			"data": "Hello"
		});

		assert.deepEqual(load_msg([0x89, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f]), {
			"masked": false,
			"type": websocket.PING,
			"data": "Hello"
		});

		assert.deepEqual(load_msg([0x8a, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58]), {
			"masked": true,
			"type": websocket.PONG,
			"data": "Hello"
		});

		assert.deepEqual(load_msg([0x01, 0x03, 0x48, 0x65, 0x6c, 0x80, 0x02, 0x6c, 0x6f]), {
			"masked": false,
			"type": websocket.TEXT,
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
			"type": websocket.BINARY,
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
			"type": websocket.BINARY,
			"data": s
		});

		assert.throws(function() {
			load_msg([0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c])
		});

		assert.throws(function() {
			load_msg([0x01, 0x03, 0x48, 0x65, 0x6c]);
		});
	});

	it("sendTo", function() {
		function test_msg(n, masked) {
			var msg = new websocket.Message();
			msg.type = websocket.TEXT;
			msg.masked = masked;

			var buf = new Buffer(n);
			for (var i = 0; i < n; i++) {
				buf[i] = (i % 10) + 0x30;
			}

			msg.body.write(buf);
			var ms = new io.MemoryStream();

			msg.sendTo(ms);
			ms.rewind();

			var msg = new websocket.Message();
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

	it("handshake", function() {
		function test_msg(n, masked) {
			var msg = new websocket.Message();
			msg.type = websocket.TEXT;
			msg.masked = masked;

			var buf = new Buffer(n);
			for (var i = 0; i < n; i++) {
				buf[i] = (i % 10) + 0x30;
			}

			msg.body.write(buf);

			msg.sendTo(rep.stream);

			var msg = new websocket.Message();
			msg.readFrom(rep.stream);

			assert.equal(msg.body.readAll().toString(), buf.toString());
		}

		var httpd = new http.Server(8810 + base_port, new websocket.Handler(function(v) {
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

		assert.throws(function() {
			test_msg(10);
		});
	});

	it("connect", function() {
		function test_msg(n, masked) {
			var msg = new websocket.Message();
			msg.type = websocket.TEXT;
			msg.masked = masked;

			var buf = new Buffer(n);
			for (var i = 0; i < n; i++) {
				buf[i] = (i % 10) + 0x30;
			}

			msg.body.write(buf);

			msg.sendTo(s);

			var msg = new websocket.Message();
			msg.readFrom(s);

			assert.equal(msg.body.readAll().toString(), buf.toString());
		}

		var s = websocket.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

		test_msg(10, true);
		test_msg(100, true);
		test_msg(125, true);
		test_msg(126, true);
		test_msg(65535, true);
		test_msg(65536, true);

	});

	it("ping", function() {
		var s = websocket.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

		var body = "hello";
		var msg = new websocket.Message();
		msg.type = websocket.PING;
		msg.body.write(body);
		msg.sendTo(s);

		var msg = new websocket.Message();
		msg.readFrom(s);

		assert.equal(msg.type, websocket.PONG);
		assert.equal(msg.body.readAll().toString(), body);
	});

	it("close", function() {
		var s = websocket.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

		var msg = new websocket.Message();
		msg.type = websocket.CLOSE;

		msg.sendTo(s);

		var msg = new websocket.Message();

		assert.throws(function() {
			msg.readFrom(s);
		});
	});

	it("non-control opcode", function() {
		var s = websocket.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

		var msg = new websocket.Message();
		msg.type = 5;
		msg.sendTo(s);

		var msg = new websocket.Message();
		assert.throws(function() {
			msg.readFrom(s);
		});
	});

	it("drop other type message", function() {
		var s = websocket.connect("ws://127.0.0.1:" + (8810 + base_port) + "/");

		var msg = new websocket.Message();
		msg.type = websocket.PONG;
		msg.sendTo(s);

		var msg = new websocket.Message();
		assert.throws(function() {
			msg.readFrom(s);
		});
	});

	it("remote close", function() {
		var httpd = new http.Server(8811 + base_port, new websocket.Handler(function(v) {
			v.stream.close();
		}));
		ss.push(httpd.socket);
		httpd.asyncRun();

		var s = websocket.connect("ws://127.0.0.1:" + (8811 + base_port) + "/");

		var msg = new websocket.Message();
		msg.type = websocket.TEXT;

		msg.sendTo(s);

		var msg = new websocket.Message();
		assert.throws(function() {
			msg.readFrom(s);
		});
	});
});

//test.run(console.DEBUG);