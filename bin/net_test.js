var test = require("test");
test.setup();

var net = require('net');
var fs = require('fs');
var os = require('os');
var coroutine = require('coroutine');

var net_config = {
	family : net.AF_INET6,
	address : '::1'
};

if (os.type == 'Windows' && os.version < "6.0")
	net_config = {
		family : net.AF_INET,
		address : '127.0.0.1'
	};

function del(f) {
	try {
		fs.unlink(f);
	} catch (e) {
	}
}

describe(
		"net",
		function() {
			it("backend", function() {
				console.log('Backend:', net.backend());
			});

			it("echo",
					function() {
						function connect(c) {
							console.log(c.remoteAddress, c.remotePort, "->",
									c.localAddress, c.localPort);
							try {
								var b;

								while (b = c.recv())
									c.send(b);
							} finally {
								c.close();
								c.dispose();
							}
						}

						function accept(s) {
							while (1)
								connect.start(s.accept());
						}

						var s = new net.Socket(net_config.family,
								net.SOCK_STREAM);
						s.bind(8080);
						s.listen();
						accept.start(s);

						function conn_socket() {
							var s1 = new net.Socket(net_config.family,
									net.SOCK_STREAM);
							s1.connect(net_config.address, 8080);
							console.log(s1.remoteAddress, s1.remotePort, "<-",
									s1.localAddress, s1.localPort);
							s1.send(new Buffer("GET / HTTP/1.0"));
							assert.equal("GET / HTTP/1.0", s1.recv());
							s1.close();
							s1.dispose();
						}

						function conn() {
							var s1 = net.connect(net_config.address, 8080,
									net_config.family);
							console.log(s1.remoteAddress, s1.remotePort, "<-",
									s1.localAddress, s1.localPort);
							s1.send(new Buffer("GET / HTTP/1.0"));
							assert.equal("GET / HTTP/1.0", s1.recv());
							s1.close();
							s1.dispose();
						}

						conn_socket();
						conn();
					});

			it(
					"copyTo",
					function() {
						var str = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

						for (var i = 0; i < 8; i++)
							str = str + str;

						function accept1(s) {
							while (true) {
								var c = s.accept();

								// c.write(new Buffer(str));

								fs.writeFile('net_temp_000001', str);
								var f = fs.open('net_temp_000001');
								assert.equal(f.copyTo(c), str.length);
								f.close();
								c.close();
							}
						}

						var s1 = new net.Socket(net_config.family,
								net.SOCK_STREAM);
						s1.bind(8081);
						s1.listen();
						accept1.start(s1);

						function t_conn() {
							var c1 = new net.Socket();
							c1.connect('127.0.0.1', 8081);

							var f1 = fs.open('net_temp_000002', 'w');
							assert.equal(c1.copyTo(f1), str.length);
							c1.close();
							f1.close();

							assert.equal(str, fs.readFile('net_temp_000002'));
						}

						for (var i = 0; i < 100; i++)
							t_conn();

						for (var i = 0; i < 10; i++)
							str = str + str;

						t_conn();

						del('net_temp_000001');
						del('net_temp_000002');
					});

			it("read & recv", function() {
				function accept2(s) {
					while (true) {
						var c = s.accept();

						c.write(new Buffer('a'));
						coroutine.sleep(100);
						c.write(new Buffer('a'));
						coroutine.sleep(100);
						c.write(new Buffer('b'));
						coroutine.sleep(100);
						c.write(new Buffer('c'));
						coroutine.sleep(100);
						c.write(new Buffer('d'));
						coroutine.sleep(100);

						c.close();
					}
				}

				var s2 = new net.Socket(net_config.family, net.SOCK_STREAM);
				s2.bind(8082);
				s2.listen();
				accept2.start(s2);

				var c1 = new net.Socket();
				c1.connect('127.0.0.1', 8082);
				assert.equal('a', c1.recv(100));
				assert.equal('abc', c1.read(3));
				assert.equal('d', c1.read(3));
			});

			it("bind same port", function() {
				new net.TCPServer(8811, function(c) {
				});
				assert.throws(function() {
					new net.TCPServer(8811, function(c) {
					});
				});
			});

			it("stats", function() {
				var svr = new net.TCPServer(8812, function(c) {
					var d;
					while (d = c.read(100))
						c.write(d);
				});
				svr.asyncRun();

				assert.deepEqual({
					"total" : 0,
					"connections" : 0,
					"accept" : 0,
					"close" : 0
				}, svr.stats.toJSON());

				var c1 = new net.Socket();
				c1.connect('127.0.0.1', 8812);

				coroutine.sleep(10);
				assert.deepEqual({
					"total" : 1,
					"connections" : 1,
					"accept" : 1,
					"close" : 0
				}, svr.stats.toJSON());

				svr.stats.reset();
				assert.deepEqual({
					"total" : 1,
					"connections" : 1,
					"accept" : 0,
					"close" : 0
				}, svr.stats.toJSON());

				c1.close();

				svr.stats.reset();
				coroutine.sleep(10);
				assert.deepEqual({
					"total" : 1,
					"connections" : 0,
					"accept" : 0,
					"close" : 1
				}, svr.stats.toJSON());
			});

			xdescribe("Smtp", function() {
				var s;

				it("new & connect", function() {
					s = new net.Smtp();
					s.connect("smtp.ym.163.com", 25);
					s.socket.close();
				});

				it("net.openSmtp", function() {
					s = net.openSmtp("smtp.exmail.qq.com", 25);
				});

				it("command", function() {
					assert.equal(s.command("HELO", "baoz.me").substr(0, 4),
							"250 ");

					assert.throws(function() {
						s.command("FUCK", "baoz.me");
					});
				});

				it("hello", function() {
					s.hello();
				});

				it("login", function() {
					s.login("lion@baoz.cn", "");
				});

				it("from", function() {
					s.from("lion@baoz.cn");
				});

				it("to", function() {
					s.to("lion@baoz.cn");
				});

				it("data", function() {
					s.data("from:lion@baoz.cn\r\nto:lion@baoz.cn\r\nsubject:test title\r\n\r\ntest text");
				});

				it("quit", function() {
					s.quit();
				});
			});
		});

// test.run(console.DEBUG);
