var test = require("test");
test.setup();

var rpc = require('rpc');
var http = require('http');
var mq = require('mq');
var net = require('net');
var io = require('io');
var encoding = require('encoding');
var os = require('os');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

var m = new http.Request();

var jr;

m.value = 'test/tttt/tttt/';
m.setHeader("Content-Type", "application/json, charset=utf-8;");
m.body.write(encoding.json.encode({
	method: 'aaaa',
	params: [100, 200],
	id: 1234
}));

describe("rpc", function() {
	var ss = [];

	after(function() {
		ss.forEach(function(s) {
			s.close();
		});
	});

	it("function", function() {
		jr = rpc.json(function(m, p1, p2) {
			m.value = '';
			return p1 + ',' + p2;
		});

		jr.invoke(m);

		m.response.body.rewind();
		assert.equal(m.response.body.read().toString(),
			'{"id":1234,"result":"100,200"}');
	});

	it("map", function() {
		m.value = 'test';
		jr = rpc.json({
			test: {
				aaaa: function(m, p1, p2) {
					m.value = '';
					return p1 + ',' + p2;
				}
			}
		});

		jr.invoke(m);

		m.response.body.rewind();
		assert.equal(m.response.body.read().toString(),
			'{"id":1234,"result":"100,200"}');
	});

	it("params", function() {
		jr = rpc.json({
			'xhr': {
				test: {
					fun: function(v, a, b) {
						return a + b;
					}
				}
			}
		});

		m = new http.Request();

		m.value = '/xhr/test';
		m.setHeader("Content-Type", "application/json");
		m.body.write(encoding.json.encode({
			method: 'fun',
			params: [100, 200],
			id: 1234
		}));

		jr.invoke(m);

		m.response.body.rewind();
		assert.equal(m.response.body.read().toString(),
			'{"id":1234,"result":300}');
	});

	it("Task", function(){
		assert.throws(function(){
			var task = rpc.open("./not_exists.js");
		});

		assert.throws(function(){
			var task = rpc.open("../not_exists.js");
		});

		assert.throws(function(){
			var task = rpc.open("http://127.0.0.1/not_exists.js");
		});

		var task = rpc.open("/not_exists.js");
		var task1 = task.func;
		var task2 = task['1234'];

		assert.throws(function(){
			task();
		});

		var task = rpc.open("module/c4.js");
		assert.equal(task.foo(), 1);

		var n = 0;
		coroutine.start(function(){
			n = 1;
		});

		task.foo();
		assert.equal(n, 1);
	});

	it("over tcp", function() {
		var hdlr = new http.Handler(rpc.json({
			update: function(v) {
				return 1;
			}
		}));
		hdlr.crossDomain = true;
		var svr = new net.TcpServer(8090 + base_port, hdlr);
		ss.push(svr.socket);
		svr.asyncRun();

		var s = new net.Socket();
		s.connect('127.0.0.1', 8090 + base_port);

		var bs = new io.BufferedStream(s);
		bs.EOL = '\r\n';

		for (var i = 0; i < 50; i++) {
			var req = new http.Request();
			req.addHeader("content-type", "application/json");
			req.body.write('{"method":"update","params":[{}]}');
			req.sendTo(s);
			req.response.readFrom(bs);
		}
	});

	it("Garbage Collection", function() {
		GC();
		GC();

		var no1 = os.memoryUsage().nativeObjects.objects;

		rpc.json({});

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);
	});
});

// test.run(console.DEBUG);