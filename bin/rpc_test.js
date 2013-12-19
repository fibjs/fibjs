var test = require("test");
test.setup();

var rpc = require('rpc');
var http = require('http');
var mq = require('mq');
var net = require('net');
var io = require('io');
var encoding = require('encoding');

var m = new http.Request();

m.value = 'test/tttt/tttt/';
m.setHeader("Content-Type", "application/json, charset=utf-8;");
m.body.write(new Buffer(encoding.jsonEncode({
	method: 'aaaa',
	params: [100, 200],
	id: 1234
})));

describe(
	"rpc",
	function() {
		it("function", function() {
			var jr = rpc.json(function(m, p1, p2) {
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
			m.body.write(new Buffer(encoding.jsonEncode({
				method: 'fun',
				params: [100, 200],
				id: 1234
			})));

			jr.invoke(m);

			m.response.body.rewind();
			assert.equal(m.response.body.read().toString(),
				'{"id":1234,"result":300}');
		});

		it(
			"encodeURI",
			function() {
				m = new http.Request();

				m.value = '/xhr/test';
				m.setHeader("Content-Type",
					"application/x-www-form-urlencoded");
				m.body.write(new Buffer("jsonrpc=" + encoding.encodeURIComponent(encoding
					.jsonEncode({
						method: 'fun',
						params: [100, 200],
						id: 1234
					}))));

				jr.invoke(m);

				m.response.body.rewind();
				assert
					.equal(
						m.response.body.read().toString(),
						'<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><script>window.name=\"{\\\"id\\\":1234,\\\"result\\\":300}\";</script></html>');
			});

		it("over tcp", function() {
			var hdlr = http.handler(rpc.json({
				update: function(v) {
					return 1;
				}
			}));
			hdlr.crossDomain = true;
			new net.TcpServer(8090, hdlr).asyncRun();

			var s = new net.Socket();
			s.connect('127.0.0.1', 8090);

			var bs = new io.BufferedStream(s);
			bs.EOL = '\r\n';

			for (i = 0; i < 50; i++) {
				var req = new http.Request();
				req.addHeader("content-type", "application/json");
				req.body.write(new Buffer(
					'{"method":"update","params":[{}]}'));
				req.sendTo(s);
				req.response.readFrom(bs);
			}
		});
	});

//test.run();