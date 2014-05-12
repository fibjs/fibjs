var test = require("test");
test.setup();

var mq = require('mq');
var net = require('net');
var io = require('io');

var m = new mq.Message();
var v = new Buffer('abcd');

function hdlr1(v) {
	n = n | 1;
}

function hdlr2(v) {
	n = n | 2;
}

function hdlr3(v) {
	n = n | 4;
}

describe("mq", function() {
	it("create Message", function() {
		var m = new mq.Message();
	});

	describe("function handler", function() {
		var n = 0;

		function fun3(v) {
			n = n | 4;
		}

		function fun2(v) {
			n = n | 2;
			return mq.jsHandler(fun3);
		}

		function fun1(v) {
			n = n | 1;
			return fun2;
		}

		var jv = mq.jsHandler(fun1);

		it("direct invoke", function() {
			n = 0;
			jv.invoke(v);
			assert.equal(3, n);
		});

		it("recursion invoke", function() {
			n = 0;
			mq.invoke(jv, v);
			assert.equal(7, n);
		});

		it("JSHandler return error result", function() {
			assert.throws(function() {
				new mq.JSHandler(function(v) {
					return 100;
				}).invoke(v);
			});

			assert.throws(function() {
				mq.invoke(function(v) {
					return 100;
				}, v);
			});
		});
	});

	describe("object handler", function() {
		var o = mq.jsHandler({
			a: hdlr1,
			b: hdlr2,
			c: hdlr3,
			d: {
				d1: hdlr1,
				d2: {
					d3: hdlr2,
					d4: hdlr3,
					d5: 1123
				}
			}
		});

		var ot = {
			'a': 1,
			'/b': 2,
			'.c': 4,
			'a.a': 1,
			'a/a': 1,
			'd.d1': 1,
			'd\\d1': 1,
			'd.d2.d3': 2,
			'd.d2.d4': 4
		};

		it("invoke path", function() {
			for (t in ot) {
				n = 0;
				m.value = t;
				o.invoke(m);
				assert.equal(ot[t], n);
			}
		});

		it("error path", function() {
			assert.throws(function() {
				m.value = 'asd';
				o.invoke(m);
			});

			assert.throws(function() {
				m.value = 'd.d2.d5';
				o.invoke(m);
			});
		});
	})

	describe("chain handler", function() {
		it("chain invoke",
			function() {
				var chain = new mq.Chain([hdlr1, hdlr2,
					mq.jsHandler(hdlr3)
				]);

				n = 0;
				chain.invoke(v);
				assert.equal(7, n);
			});

		it("params", function() {
			function chain_params(v, p1, p2) {
				assert.equal(v.value, '');
				assert.equal(v.params.length, 2);
				assert.equal(v.params[0], "123");
				assert.equal(v.params[1], "b1234");
				assert.equal(p1, "123");
				assert.equal(p2, "b1234");
			}

			var chain1 = new mq.Chain([chain_params, chain_params,
				mq.jsHandler(chain_params)
			]);

			m.value = '';
			m.params.resize(2);
			m.params[0] = '123';
			m.params[1] = 'b1234';
			mq.invoke(chain1, m);
		});

		it("Message", function() {
			var handler = new mq.Chain([

				function(v) {
					return {};
				},
				function(v) {
					assert.isObject(v.result);
					return "aaa" + v.result;
				}
			]);

			var req = new mq.Message();
			mq.invoke(handler, req);
			assert.equal("aaa[object Object]", req.result);

			var handler = new mq.Chain([

				function(v) {
					v.params[0] = {};
				},
				function(v) {
					assert.isObject(v.params[0]);
				}
			]);

			var req = new mq.Message();
			req.params.push("aaasssssssssssssss");
			mq.invoke(handler, req);
		});
	});

	describe("routing handler", function() {
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
			'^b$': mq.jsHandler(hdlr2),
			'^params/(([0-9]+)\.(([a-z])?[0-9]+)\.html)$': params,
			'^params0/[0-9]+\.html$': params0,
			'^params1/([0-9]+)\.html$': params1,
			'^params2/([0-9]+)\.(([a-z])?[0-9]+)\.html$': params2,
			'^params3/(([0-9]+)\.(([a-z])?[0-9]+([a-z]([0-9]+)))\.html)$': params3
		});

		it("simple path", function() {
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

		it("regex path", function() {
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

		it("error path", function() {
			n = 0;
			m.value = 'd';
			assert.throws(function() {
				mq.invoke(r, m);
			});
		});

		it("object param", function() {
			var req = new mq.Message();
			req.params.resize(1);
			req.params[0] = [];

			mq.jsHandler(function t(request, d) {
				assert.isArray(d);
				return "ok";
			}).invoke(req);
		});

		it("path to value", function() {
			var r = new mq.Routing({
				"^/api/a$": function(v) {},
				"^/api/a(/.*)$": function(v) {}
			});

			var m = new mq.Message();
			m.value = '/api/a';
			mq.invoke(r, m);
			assert.equal('', m.value);

			m.value = '/api/a/test';
			mq.invoke(r, m);
			assert.equal('/test', m.value);
		});
	});

	it("await", function() {
		var n = 100;

		mq.invoke(mq.jsHandler(function(r) {
			var aw = mq.await();

			function delayend() {
				assert.equal(n, 100);
				n = 200;
				aw.end();
			}
			delayend.start();

			return aw;
		}), m);
		assert.equal(n, 200);
	});

	it("PacketHandler", function() {
		var s = new net.TcpServer(8884, new mq.PacketHandler(function(r) {
			var d = r.body.readAll();
			r.clear();
			r.body.write(new Buffer(d.toString().toUpperCase()));
		}));

		s.asyncRun();

		var c = new net.Socket();
		c.connect('127.0.0.1', 8884);
		var r = new io.BufferedStream(c);

		r.writePacket(new Buffer('abcdefg'));
		var b = r.readPacket();
		assert.equal(b.toString(), 'ABCDEFG');

		r.writePacket(new Buffer('abcdefg'));
		var b = r.readPacket();
		assert.equal(b.toString(), 'ABCDEFG');
	});
});

//test.run(console.DEBUG);