var test = require("test");
test.setup();

var ssl = require("ssl");
var crypto = require("crypto");
var net = require("net");
var fs = require('fs');
var os = require('os');
var coroutine = require('coroutine');

var rsa1024_pem = "-----BEGIN RSA PRIVATE KEY-----\n" +
	"MIICXQIBAAKBgQDSbmW8qlarL0lLu1XYcg+ocJgcuq5K7EgLcXyy2shAsko7etmZ\n" +
	"NP3opOeGw58E7tjVsjuadPQ8Hf+9wd316RYwShklDDsy4Hwp4z9afnj56UmvkM0u\n" +
	"TlsymlIbFftme6aJcYbnX9fdin78Rsa0MbzQbFdeLHsy5zKjsrbm4TS5bwIDAQAB\n" +
	"AoGAagN2O9NxMHL1MTMi75WfL9Pxvl+KWXKqZSF6mjzAsF9iKI8euyHIXYFepzU8\n" +
	"kual1RsjDhCnzvWqFvZplW8lXqrHf/P+rS/9Y4gBUw6pjnI/DnFIRwWHRvrUHHSC\n" +
	"fWOdTCIKdOTkgLZuGFuhEY3RMIW0WSYejjLtftwy0RVxAzkCQQDprgbWqZ/BaafV\n" +
	"uKKA3shUWWRst/2hV7qDus6YfEj6GfUZHEoNJW4BSuZHUiG4Cdxr0zTLtIP7tNSz\n" +
	"rCM7FbFrAkEA5ofkxFKdPBD0CQHMb9q13AMHUVe0rJ+hSjqqIBrmqApUOneyAcMV\n" +
	"76M0QyIQnI2p3POa4Qu/7XChDwRVl7LlDQJBANplxohsAh5fI/hQVriA/tQus/gU\n" +
	"QdzARFaHijzjs8Tj67mrQd5lhBl7KhuwPEloFfVEcUyNiuj9yeme0VKQZL8CQQCh\n" +
	"qCfm99vk1Cqc6lL3GRKwPrtx8iPzbVlIWU0ViGe47M1V1rvP+oK2cebjMM8fSUQV\n" +
	"egpgx8GF+pYmlq6C22M9AkB2oRCDE2Y5UWsXizvvqFOp4LqgQOKVrwS+lVvYXZm/\n" +
	"AA4uMJDro6IBkrUGJgYepTLQ16o2a2WwtK4ERlBm+pnC\n" +
	"-----END RSA PRIVATE KEY-----\n";

var ca_pem = "-----BEGIN CERTIFICATE-----\n" +
	"MIICJzCCAZCgAwIBAgIAMA0GCSqGSIb3DQEBBQUAMDExCzAJBgNVBAYTAkNOMRAw\n" +
	"DgYDVQQKEwdiYW96LmNuMRAwDgYDVQQDEwdiYW96Lm1lMB4XDTE0MDQyMTEzMTYw\n" +
	"MVoXDTE1MDQyMTEzMTYwMVowMTELMAkGA1UEBhMCQ04xEDAOBgNVBAoTB2Jhb3ou\n" +
	"Y24xEDAOBgNVBAMTB2Jhb3oubWUwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGB\n" +
	"ANJuZbyqVqsvSUu7VdhyD6hwmBy6rkrsSAtxfLLayECySjt62Zk0/eik54bDnwTu\n" +
	"2NWyO5p09Dwd/73B3fXpFjBKGSUMOzLgfCnjP1p+ePnpSa+QzS5OWzKaUhsV+2Z7\n" +
	"polxhudf192KfvxGxrQxvNBsV14sezLnMqOytubhNLlvAgMBAAGjUDBOMAwGA1Ud\n" +
	"EwQFMAMBAQEwHQYDVR0OBBYEFDMQn0OgAthuzps7thMvxtSSYb7eMB8GA1UdIwQY\n" +
	"MBaAFDMQn0OgAthuzps7thMvxtSSYb7eMA0GCSqGSIb3DQEBBQUAA4GBAAqIhEYc\n" +
	"fImzOtloyWUY87T0GtvQUE2YQLiXTx56pzk/QT4pya+3MmUpkLet55QtXXITMt4F\n" +
	"KEB0TYVF01aB5Bg2xpuwRowIn1YGDLs9XQt8TaQ7p8c67JEjhH3a9dzTDCi+HAX4\n" +
	"qIKQc4ofAGJjiATiB7Dqctk3h2bNS06y2vC6\n" +
	"-----END CERTIFICATE-----\n";

var crt_pem = "-----BEGIN CERTIFICATE-----\n" +
	"MIICJjCCAY+gAwIBAgIAMA0GCSqGSIb3DQEBBQUAMDExCzAJBgNVBAYTAkNOMRAw\n" +
	"DgYDVQQKEwdiYW96LmNuMRAwDgYDVQQDEwdiYW96Lm1lMB4XDTE0MDQyMTEzMTgz\n" +
	"MFoXDTE1MDQyMTEzMTgzMFowMzELMAkGA1UEBhMCQ04xEDAOBgNVBAoTB2Jhb3ou\n" +
	"Y24xEjAQBgNVBAMTCWxvY2FsaG9zdDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkC\n" +
	"gYEA0m5lvKpWqy9JS7tV2HIPqHCYHLquSuxIC3F8strIQLJKO3rZmTT96KTnhsOf\n" +
	"BO7Y1bI7mnT0PB3/vcHd9ekWMEoZJQw7MuB8KeM/Wn54+elJr5DNLk5bMppSGxX7\n" +
	"ZnumiXGG51/X3Yp+/EbGtDG80GxXXix7Mucyo7K25uE0uW8CAwEAAaNNMEswCQYD\n" +
	"VR0TBAIwADAdBgNVHQ4EFgQUMxCfQ6AC2G7Omzu2Ey/G1JJhvt4wHwYDVR0jBBgw\n" +
	"FoAUMxCfQ6AC2G7Omzu2Ey/G1JJhvt4wDQYJKoZIhvcNAQEFBQADgYEAj1cEcKNR\n" +
	"uxKjcmy0JXBiM1ADKBFEpmREPmR9oiUXiB7UcNzEDZuh49hHYELl/cxCC6utF4Zq\n" +
	"wS/7NDim6qrL5H5cUPgaqYhP5sWES6sPhcDgTgF4QtHfdJdbDWjbN3wpzHaOujiA\n" +
	"BNezFdEB4uBhbTvkdAoMzklsGyc39Gegb5w=\n" +
	"-----END CERTIFICATE-----\n";

var net_config = {
	family: net.AF_INET6,
	address: '::1'
};

if (os.type == 'Windows' && os.version < "6.0")
	net_config = {
		family: net.AF_INET,
		address: '127.0.0.1'
	};

var crt = new crypto.X509Cert();
crt.load(crt_pem);

var pk = new crypto.PKey();
pk.importKey(rsa1024_pem);

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

		for (i = 0; i < 10; i++) {
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

		for (i = 0; i < 10; i++) {
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
});

//test.run(console.DEBUG);