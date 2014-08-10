var test = require("test");
test.setup();

var ssl = require("ssl");
var crypto = require("crypto");
var net = require("net");
var fs = require('fs');
var os = require('os');
var coroutine = require('coroutine');

var net_config = {
	family: net.AF_INET6,
	address: '::1'
};

if (os.type == 'Windows' && os.version < "6.0")
	net_config = {
		family: net.AF_INET,
		address: '127.0.0.1'
	};

var pk = new crypto.PKey();
pk.genRsaKey(1024);

var crt = new crypto.X509Req("CN=localhost", pk).sign("CN=baoz.me", pk);
var ca = new crypto.X509Req("CN=baoz.me", pk).sign("CN=baoz.me", pk, {
	ca: true
});

var ca_pem = ca.dump()[0];

function del(f) {
	try {
		fs.unlink(f);
	} catch (e) {}
}

describe('ssl', function() {
	var sss;

	it("echo server", function() {
		sss = new ssl.Socket(crt, pk);
		sss.verification = ssl.VERIFY_NONE;

		var svr = new net.TcpServer(9080, function(s) {
			var ss;
			var buf;

			try {
				ss = sss.accept(s);
			} catch (e) {
				s.close();
				return;
			}

			try {
				while (buf = ss.read())
					ss.write(buf);
			} finally {
				ss.close();
				s.close();
			}
		});
		svr.asyncRun();
	});

	function test_handshake() {
		var s = new net.Socket();
		s.connect("127.0.0.1", 9080);

		var ss = new ssl.Socket();
		ss.connect(s);
		ss.close();
		s.close();
	}

	function test_client_cert() {
		var s = new net.Socket();
		s.connect("127.0.0.1", 9080);

		var ss = new ssl.Socket(crt, pk);
		ss.connect(s);
		ss.close();
		s.close();
	}

	it("client verify", function() {
		ssl.verification = ssl.VERIFY_OPTIONAL;
		test_handshake();

		ssl.verification = ssl.VERIFY_REQUIRED;
		assert.throws(test_handshake);

		ssl.ca.load(ca_pem);
		test_handshake();
	});

	it("server verify", function() {
		sss.verification = ssl.VERIFY_REQUIRED;
		assert.throws(test_client_cert);

		sss.ca.load(ca_pem);
		test_client_cert();

		assert.throws(test_handshake);
	});

	it("echo", function() {
		sss.verification = ssl.VERIFY_NONE;

		for (var i = 0; i < 10; i++) {
			var s1 = new net.Socket();
			s1.connect("127.0.0.1", 9080);

			var ss = new ssl.Socket();
			ss.connect(s1);

			ss.write(new Buffer("GET / HTTP/1.0"));
			assert.equal("GET / HTTP/1.0", ss.read());

			ss.close();
			s1.close();
		}
	});

	it("copyTo", function() {
		var str = "012345678901234567890123456789";

		for (var i = 0; i < 10; i++)
			str = str + str;

		var sss = new ssl.Socket(crt, pk);
		sss.verification = ssl.VERIFY_NONE;

		var svr = new net.TcpServer(9082, function(s) {
			var ss = sss.accept(s);

			fs.writeFile('net_temp_000001', str);
			var f = fs.open('net_temp_000001');
			assert.equal(f.copyTo(ss), str.length);

			f.close();
			ss.close();
			s.close();
		});
		svr.asyncRun();

		function t_conn() {
			var c1 = new net.Socket();
			c1.connect('127.0.0.1', 9082);

			var ss = new ssl.Socket();
			ss.connect(c1);

			var f1 = fs.open('net_temp_000002', 'w');
			assert.equal(ss.copyTo(f1), str.length);

			ss.close();
			c1.close();
			f1.close();

			assert.equal(str, fs.readFile('net_temp_000002'));
		}

		for (var i = 0; i < 10; i++) {
			str = str + str;
			t_conn();
		}

		del('net_temp_000001');
		del('net_temp_000002');
	});

	it("Handler", function() {
		var svr = new net.TcpServer(9083, new ssl.Handler(crt, pk, function(s) {
			var buf;

			while (buf = s.read())
				s.write(buf);
		}));
		svr.asyncRun();

		for (var i = 0; i < 10; i++) {
			var s1 = new net.Socket();
			s1.connect("127.0.0.1", 9083);

			var ss = new ssl.Socket();
			ss.connect(s1);

			ss.write(new Buffer("GET / HTTP/1.0"));
			assert.equal("GET / HTTP/1.0", ss.read());

			ss.close();
			s1.close();
		}
	});

	it("Server", function() {
		var svr = new ssl.Server(crt, pk, 9084, function(s) {
			var buf;

			while (buf = s.read())
				s.write(buf);
		});
		svr.asyncRun();

		for (var i = 0; i < 10; i++) {
			var s1 = new net.Socket();
			s1.connect("127.0.0.1", 9084);

			var ss = new ssl.Socket();
			ss.connect(s1);

			ss.write(new Buffer("GET / HTTP/1.0"));
			assert.equal("GET / HTTP/1.0", ss.read());

			ss.close();
			s1.close();
		}
	});
});

//test.run(console.DEBUG);