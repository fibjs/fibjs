var test = require("test");
test.setup();

var crypto = require("crypto");
var hash = require("hash");
var fs = require("fs");
var os = require("os");
var encoding = require("encoding");

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

var pub_rsa1024_pem = "-----BEGIN PUBLIC KEY-----\n" +
	"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDSbmW8qlarL0lLu1XYcg+ocJgc\n" +
	"uq5K7EgLcXyy2shAsko7etmZNP3opOeGw58E7tjVsjuadPQ8Hf+9wd316RYwShkl\n" +
	"DDsy4Hwp4z9afnj56UmvkM0uTlsymlIbFftme6aJcYbnX9fdin78Rsa0MbzQbFde\n" +
	"LHsy5zKjsrbm4TS5bwIDAQAB\n" +
	"-----END PUBLIC KEY-----\n";

var ec_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
	"MIHcAgEBBEIB+QhtQdd9bjWeN2mgq6qoqW51ygslLwP+gwTCSP4ZVpcU0pxwigXm\n" +
	"Ioa7Zzr2Q0OOjzdH/vDIaV9FzOySWSKTVJOgBwYFK4EEACOhgYkDgYYABAE3zTRb\n" +
	"rr5Rsa0xqF4Mn0gBoETTQ5zt6zd5O+LqyiXVUteEWIXZ0sJvdQZvhdZfk9VypQGN\n" +
	"skZ8xvcNVJL6PHNMCwEgR6q67gepHZBFAd75Vt/lCJJOB7SnVHk8Dfu5t49q4Gb2\n" +
	"EdX30QGqQ71qihogY6Jqlmn8aJAZGwyNHPeX+n+F1A==\n" +
	"-----END EC PRIVATE KEY-----\n";

var pub_ec_pem = "-----BEGIN PUBLIC KEY-----\n" +
	"MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQBN800W66+UbGtMaheDJ9IAaBE00Oc\n" +
	"7es3eTvi6sol1VLXhFiF2dLCb3UGb4XWX5PVcqUBjbJGfMb3DVSS+jxzTAsBIEeq\n" +
	"uu4HqR2QRQHe+Vbf5QiSTge0p1R5PA37ubePauBm9hHV99EBqkO9aooaIGOiapZp\n" +
	"/GiQGRsMjRz3l/p/hdQ=\n" +
	"-----END PUBLIC KEY-----\n";

var ca1 = "-----BEGIN CERTIFICATE-----\n" +
	"MIIDcjCCAlqgAwIBAgIBETANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\n" +
	"MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\n" +
	"MTIwNTEwMTMyMzQxWhcNMjIwNTExMTMyMzQxWjA6MQswCQYDVQQGEwJOTDERMA8G\n" +
	"A1UEChMIUG9sYXJTU0wxGDAWBgNVBAMTD3d3dy5leGFtcGxlLmNvbTCCASIwDQYJ\n" +
	"KoZIhvcNAQEBBQADggEPADCCAQoCggEBALk8SsXIo46QF6SeUqpxdSZhgOfHtW2M\n" +
	"/6q2QSa3vhGtXHMWDGQRSAT/1uE7BduJu7OXCdUcFN1ohzmwPXHL4nbQGtgYLYAb\n" +
	"VPblRJrxy69hLt9JDZ0Jt+2x/Tz9PPokz12/fORT5yW16kQi6SbT6iCUnuZhZ7ou\n" +
	"B2cLAy+iCe3wM48LzhDvZ6TGCNrB7cI/10rdFT35XhyBYEY+tbM9L6beRxy8kq7r\n" +
	"3ydrFla33OzRVVelbux1JfW3e9+r0jpakZh9lxcLEwqna0qLwUcw+zr4QQTVwd+4\n" +
	"Hb97AaVlouAeNremXMwwWvjNb83xGWIlygHjNX/6IPXc/WmyagB9F/cCAwEAAaOB\n" +
	"gTB/MAkGA1UdEwQCMAAwHQYDVR0OBBYEFH3knGvm+XF9RtISPa1rHf3CqnhMMB8G\n" +
	"A1UdIwQYMBaAFLRa5KWz3tJS9rnVppUP6z68x/3/MDIGA1UdEQQrMCmCC2V4YW1w\n" +
	"bGUuY29tggtleGFtcGxlLm5ldIINKi5leGFtcGxlLm9yZzANBgkqhkiG9w0BAQUF\n" +
	"AAOCAQEATwnLetXu9e9iDdx7ooXWjMqVtGvaEVuSAHUTucoLzur7wx/iP38hdHni\n" +
	"5rzaBuUvb/ZVxnM5z0i8DS8M0noGw0pM2UhdoNBzieTUhR2Wmg5XmcZvHSEnH40F\n" +
	"KehAroI5aMOXB888k0wa3y+mpFVIf3yMGski2iTNkjnGiuywjfVpgmfLBO7eU0GW\n" +
	"wSfcL/4z+tMOuNQyqYQoU6Xw0YnVopjnFpG7nMBBjoxYrP/j3S56q7C5cXatDycz\n" +
	"96kp08B2wL8GQHwO1aR8iuIybhau2mQfsFV8293xpLpEfLOZWNI0bgDql2wUOvIQ\n" +
	"HgqiSRB2AfTyyBj9zGNGEosJG/GU5g==\n" +
	"-----END CERTIFICATE-----\n";

var ca2 = "-----BEGIN CERTIFICATE-----\n" +
	"MIIDQjCCAiqgAwIBAgIBCDANBgkqhkiG9w0BAQ4FADA7MQswCQYDVQQGEwJOTDER\n" +
	"MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\n" +
	"MTEwMjEyMTQ0NDA3WhcNMjEwMjEyMTQ0NDA3WjA/MQswCQYDVQQGEwJOTDERMA8G\n" +
	"A1UEChMIUG9sYXJTU0wxHTAbBgNVBAMTFFBvbGFyU1NMIENlcnQgU0hBMjI0MIIB\n" +
	"IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuTxKxcijjpAXpJ5SqnF1JmGA\n" +
	"58e1bYz/qrZBJre+Ea1ccxYMZBFIBP/W4TsF24m7s5cJ1RwU3WiHObA9ccvidtAa\n" +
	"2BgtgBtU9uVEmvHLr2Eu30kNnQm37bH9PP08+iTPXb985FPnJbXqRCLpJtPqIJSe\n" +
	"5mFnui4HZwsDL6IJ7fAzjwvOEO9npMYI2sHtwj/XSt0VPfleHIFgRj61sz0vpt5H\n" +
	"HLySruvfJ2sWVrfc7NFVV6Vu7HUl9bd736vSOlqRmH2XFwsTCqdrSovBRzD7OvhB\n" +
	"BNXB37gdv3sBpWWi4B42t6ZczDBa+M1vzfEZYiXKAeM1f/og9dz9abJqAH0X9wID\n" +
	"AQABo00wSzAJBgNVHRMEAjAAMB0GA1UdDgQWBBR95Jxr5vlxfUbSEj2tax39wqp4\n" +
	"TDAfBgNVHSMEGDAWgBS0WuSls97SUva51aaVD+s+vMf9/zANBgkqhkiG9w0BAQ4F\n" +
	"AAOCAQEAuJsK0bTRpM4FOUJ6O3te/ZdXijZgQjnQ5gycfi8rvu/nRTR3SHoQSv12\n" +
	"ykI5JTz6GfhjbOc2J5rsBs7k9ywuxjbBJb2rCari2k7errX1up6QJFI0lpZhTCa1\n" +
	"V2WxEO0TK1SQztMhy4zTTGzl4XgiFj/hvvHuXTlIoeaARvRG8nkDPvH8UUfZBeiF\n" +
	"gRsLT/qFnc7ndlpv2pifQ/HzLy9XKKpwFIJ/1WkUjPmCti+m37VrDkPJlpFkPYuo\n" +
	"FxWaiEKk0JDAo6Lh3faVbTudcaYeniwe2/Zfk0Ms7VNwVVBW382WbNWRD7Gn9LcX\n" +
	"nR8L9gv4/ud83sEgt/xpE7riYZulYg==\n" +
	"-----END CERTIFICATE-----\n";

var pk = "-----BEGIN PUBLIC KEY-----\n" +
	"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuTxKxcijjpAXpJ5SqnF1\n" +
	"JmGA58e1bYz/qrZBJre+Ea1ccxYMZBFIBP/W4TsF24m7s5cJ1RwU3WiHObA9ccvi\n" +
	"dtAa2BgtgBtU9uVEmvHLr2Eu30kNnQm37bH9PP08+iTPXb985FPnJbXqRCLpJtPq\n" +
	"IJSe5mFnui4HZwsDL6IJ7fAzjwvOEO9npMYI2sHtwj/XSt0VPfleHIFgRj61sz0v\n" +
	"pt5HHLySruvfJ2sWVrfc7NFVV6Vu7HUl9bd736vSOlqRmH2XFwsTCqdrSovBRzD7\n" +
	"OvhBBNXB37gdv3sBpWWi4B42t6ZczDBa+M1vzfEZYiXKAeM1f/og9dz9abJqAH0X\n" +
	"9wIDAQAB\n" +
	"-----END PUBLIC KEY-----\n";

var req1 = "-----BEGIN CERTIFICATE REQUEST-----\n" +
	"MIIBcDCB2gIBADAxMQswCQYDVQQGEwJDTjEQMA4GA1UEChMHYmFvei5jbjEQMA4G\n" +
	"A1UEAxMHYmFvei5tZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA0m5lvKpW\n" +
	"qy9JS7tV2HIPqHCYHLquSuxIC3F8strIQLJKO3rZmTT96KTnhsOfBO7Y1bI7mnT0\n" +
	"PB3/vcHd9ekWMEoZJQw7MuB8KeM/Wn54+elJr5DNLk5bMppSGxX7ZnumiXGG51/X\n" +
	"3Yp+/EbGtDG80GxXXix7Mucyo7K25uE0uW8CAwEAAaAAMA0GCSqGSIb3DQEBBQUA\n" +
	"A4GBACx6pUvyyU3YmwR42MjRT9q2cN5KVt5oWCdSgOC+WTNi4FeMRCxPC8ExNkzN\n" +
	"mn71AolBNc1ve5cgyr3CGN3mYK86heH2y0x4xApN/IJeUCdAtzMFvLbvkEBSLtkz\n" +
	"G3f1zTP01fknEN1WH762dXrisQONk6qmbga6ytMo7KbJ0Xgl\n" +
	"-----END CERTIFICATE REQUEST-----\n";

var data = new Buffer([0x9b, 0x4c, 0x7b, 0xce, 0x7a, 0xbd, 0x0a, 0x13,
	0x61, 0xfb, 0x17, 0xc2, 0x06, 0x12, 0x0c, 0xed
]);

var art1 = "+-----[lion]------+\n" +
	"|    .+.          |\n" +
	"|      o.         |\n" +
	"|     .. +        |\n" +
	"|      Eo =       |\n" +
	"|        S + .    |\n" +
	"|       o B . .   |\n" +
	"|        B o..    |\n" +
	"|         *...    |\n" +
	"|        .o+...   |\n" +
	"+-----------------+\n";

var art2 = "+-----------------+\n" +
	"|    .+.          |\n" +
	"|      o.         |\n" +
	"|     .. +        |\n" +
	"|      Eo =       |\n" +
	"|        S + .    |\n" +
	"|       o B . .   |\n" +
	"|        B o..    |\n" +
	"|         *...    |\n" +
	"|        .o+...   |\n" +
	"+-----------------+\n";

var art3 = "+01234567890123456+\n" +
	"|    .+.          |\n" +
	"|      o.         |\n" +
	"|     .. +        |\n" +
	"|      Eo =       |\n" +
	"|        S + .    |\n" +
	"|       o B . .   |\n" +
	"|        B o..    |\n" +
	"|         *...    |\n" +
	"|        .o+...   |\n" +
	"+-----------------+\n";

describe('crypto', function() {
	it("random", function() {
		assert.notEqual(crypto.randomBytes(8).hex(), crypto.randomBytes(8).hex());
	});

	it("pseudoRandomBytes", function() {
		assert.notEqual(crypto.pseudoRandomBytes(8).hex(), crypto.pseudoRandomBytes(8).hex());
	});

	it("randomArt", function() {
		assert.notEqual(crypto.randomArt(data, "lion"), art1);
		assert.notEqual(crypto.randomArt(data, ""), art2);
		assert.notEqual(crypto.randomArt(data, "01234567890123456789"), art3);
	});

	describe('Cipher', function() {
		function test_cipher(provider, file) {
			it(file, function() {
				var cases = encoding.jsonDecode(fs.readFile("crypto_case/" + file + ".json"));

				cases.forEach(function(item) {
					var c;
					var mode = crypto[item[2]];

					if (item.length == 7)
						c = new crypto.Cipher(provider, mode,
							encoding.hexDecode(item[3]), encoding.hexDecode(item[6]));
					else if (item.length == 6)
						c = new crypto.Cipher(provider, mode,
							encoding.hexDecode(item[3]));

					if (mode == crypto.CBC)
						c.paddingMode(crypto.NOPADDING);
					assert.equal(c[item[1]](encoding.hexDecode(item[4])).hex(), item[5], item[0]);
				});

			});
		}

		test_cipher(crypto.CAMELLIA, "camellia");
		test_cipher(crypto.DES, "des");
		test_cipher(crypto.DES_EDE3, "des_ede3");
		test_cipher(crypto.BLOWFISH, "blowfish");
		test_cipher(crypto.ARC4, "arc4");

		describe('AES', function() {
			test_cipher(crypto.AES, "aes.cbc");
			test_cipher(crypto.AES, "aes.cfb");
			test_cipher(crypto.AES, "aes.ecb");
		});
	});

	describe("PKey", function() {
		describe("RSA", function() {
			it("PEM import/export", function() {
				var pk = new crypto.PKey();
				pk.importKey(rsa1024_pem);
				assert.equal(pk.exportPem(), rsa1024_pem);
			});

			it("toString", function() {
				var pk = new crypto.PKey();
				pk.importKey(rsa1024_pem);
				assert.equal(pk, rsa1024_pem);
			});

			it("Der import/export", function() {
				var pk = new crypto.PKey();
				pk.importKey(rsa1024_pem);
				var der = pk.exportDer();
				pk.importKey(der);
				assert.equal(pk.exportPem(), rsa1024_pem);
			});

			it("import publicKey", function() {
				var pk = new crypto.PKey();
				pk.importKey(pub_rsa1024_pem);
				assert.isFalse(pk.isPrivate());

				assert.equal(pk, pub_rsa1024_pem);

				var pk1 = new crypto.PKey();
				pk1.importKey(pk.exportDer());
				assert.isFalse(pk1.isPrivate());

				assert.equal(pk1.exportPem(), pub_rsa1024_pem);
			});

			it("publicKey", function() {
				var pk = new crypto.PKey();
				pk.importKey(rsa1024_pem);
				assert.isTrue(pk.isPrivate());

				var pk1 = pk.publicKey;
				assert.isFalse(pk1.isPrivate());

				assert.equal(pk1, pub_rsa1024_pem);
			});

			it("clone", function() {
				var pk = new crypto.PKey();
				pk.importKey(rsa1024_pem);

				var pk1 = pk.clone();

				assert.equal(pk1.exportPem(), pk.exportPem());
			});

			it("gen_key", function() {
				var pk = new crypto.PKey();
				var pk1 = new crypto.PKey();
				pk.genRsaKey(512);
				pk1.genRsaKey(512);

				assert.notEqual(pk.exportPem(), pk1.exportPem());
			});

			it("encrypt/decrypt", function() {
				var pk = new crypto.PKey();
				pk.importKey(rsa1024_pem);

				var pk1 = pk.publicKey;

				var d = pk1.encrypt("abcdefg");
				assert.equal(pk.decrypt(d).toString(), "abcdefg");

				assert.throws(function() {
					pk1.decrypt(d);
				});
			});

			it("sign/verify", function() {
				var pk = new crypto.PKey();
				pk.importKey(rsa1024_pem);

				var pk1 = pk.publicKey;

				var md = hash.md5("abcdefg").digest();
				var md1 = hash.md5("abcdefg1").digest();
				var d = pk.sign(md);
				assert.isTrue(pk1.verify(d, md));
				assert.isFalse(pk1.verify(d, md1));

				assert.throws(function() {
					pk1.sign(md);
				});
			});
		});

		describe("EC", function() {
			it("PEM import/export", function() {
				var pk = new crypto.PKey();
				pk.importKey(ec_pem);
				assert.equal(pk.exportPem(), ec_pem);
			});

			it("toString", function() {
				var pk = new crypto.PKey();
				pk.importKey(ec_pem);
				assert.equal(pk, ec_pem);
			});

			it("Der import/export", function() {
				var pk = new crypto.PKey();
				pk.importKey(ec_pem);
				var der = pk.exportDer();
				pk.importKey(der);
				assert.equal(pk.exportPem(), ec_pem);
			});

			it("import publicKey", function() {
				var pk = new crypto.PKey();
				pk.importKey(pub_ec_pem);
				assert.isFalse(pk.isPrivate());

				assert.equal(pk, pub_ec_pem);

				var pk1 = new crypto.PKey();
				pk1.importKey(pk.exportDer());
				assert.isFalse(pk1.isPrivate());

				assert.equal(pk1.exportPem(), pub_ec_pem);
			});

			it("publicKey", function() {
				var pk = new crypto.PKey();
				pk.importKey(ec_pem);
				assert.isTrue(pk.isPrivate());

				var pk1 = pk.publicKey;
				assert.isFalse(pk1.isPrivate());

				assert.equal(pk1, pub_ec_pem);
			});

			it("clone", function() {
				var pk = new crypto.PKey();
				pk.importKey(ec_pem);

				var pk1 = pk.clone();

				assert.equal(pk1.exportPem(), pk.exportPem());
			});

			it("gen_key", function() {
				var pk = new crypto.PKey();
				var pk1 = new crypto.PKey();
				pk.genEcKey();
				pk1.genEcKey();

				assert.notEqual(pk.exportPem(), pk1.exportPem());
			});

			it("sign/verify", function() {
				var pk = new crypto.PKey();
				pk.importKey(ec_pem);

				var pk1 = pk.publicKey;

				var md = hash.md5("abcdefg").digest();
				var md1 = hash.md5("abcdefg1").digest();
				var d = pk.sign(md);
				assert.isTrue(pk1.verify(d, md));
				assert.isFalse(pk1.verify(d, md1));

				assert.throws(function() {
					pk1.sign(md);
				});
			});
		});

		it("name", function() {
			var pk = new crypto.PKey();

			assert.equal(pk.name, "invalid PK");

			pk.importKey(rsa1024_pem);
			assert.equal(pk.name, "RSA");

			pk.importKey(ec_pem);
			assert.equal(pk.name, "EC");
		});

		it("keySize", function() {
			var pk = new crypto.PKey();

			assert.equal(pk.keySize, 0);

			pk.importKey(rsa1024_pem);
			assert.equal(pk.keySize, 1024);

			pk.importKey(ec_pem);
			assert.equal(pk.keySize, 521);
		});

	});

	describe("X509 Cert", function() {
		var cert = new crypto.X509Cert();

		it("load", function() {
			var fl = fs.readdir('cert_files/');
			fl.forEach(function(s) {
				if (!s.isDirectory() && s.name.match(/\.crt/))
					cert.load(fs.readFile('cert_files/' + s.name));
			});
		});

		it("clear/export", function() {
			var s = cert.dump();
			cert.clear();
			assert.deepEqual(cert.dump(), []);

			s.forEach(function(c) {
				cert.load(c);
			});

			assert.deepEqual(cert.dump(), s);

			cert.clear();
			cert.load(s.join('\n'));

			assert.deepEqual(cert.dump(), s);
		});

		it("load file", function() {
			var s = cert.dump();
			cert.clear();
			assert.deepEqual(cert.dump(), []);

			var fl = fs.readdir('cert_files/');
			fl.forEach(function(s) {
				if (!s.isDirectory() && s.name.match(/\.crt/))
					cert.loadFile('cert_files/' + s.name);
			});

			assert.deepEqual(cert.dump(), s);
		});

		it("certdata.txt", function() {
			cert.clear();
			assert.deepEqual(cert.dump(), []);

			cert.load(fs.readFile('cert_files/certdata.txt'));
			var s = cert.dump();
			assert.notDeepEqual(s, []);

			cert.clear();
			assert.deepEqual(cert.dump(), []);

			cert.load(fs.readFile('cert_files/ca-bundle.crt'));
			var s1 = cert.dump();

			assert.deepEqual(s.slice(23), s1);
		});

		it("root ca", function() {
			cert.clear();
			assert.deepEqual(cert.dump(), []);

			cert.load(fs.readFile('cert_files/ca-bundle.crt'));
			var s = cert.dump();

			cert.clear();
			assert.deepEqual(cert.dump(), []);

			cert.loadRootCerts();
			var s1 = cert.dump();

			assert.deepEqual(s, s1.slice(23));
		});

		it("load root ca times", function() {
			function count(ca) {
				var cnt = 1;
				var ca1;

				while (ca1 = ca.next) {
					cnt++;
					ca = ca1;
				}

				return cnt;
			}

			cert.clear();
			assert.deepEqual(cert.dump(), []);

			cert.loadRootCerts();
			var cnt1 = count(cert);

			cert.loadRootCerts();
			assert.equal(cnt1, count(cert));

			cert.clear();
			assert.deepEqual(cert.dump(), []);

			cert.loadRootCerts();
			assert.equal(cnt1, count(cert));
		});

		it("unknown format", function() {
			assert.throws(function() {
				cert.load('cert_files/certdata.txt');
			});
		});

		it("info", function() {
			cert.clear();
			assert.deepEqual(cert.dump(), []);

			cert.load(ca1);
			assert.equal(cert.version, 3);
			assert.equal(cert.serial, "17");
			assert.equal(cert.issuer, "C=NL, O=PolarSSL, CN=PolarSSL Test CA");
			assert.equal(cert.subject, "C=NL, O=PolarSSL, CN=www.example.com");
			assert.deepEqual(cert.notBefore, new Date("May 10 13:23:41 2012 GMT"));
			assert.deepEqual(cert.notAfter, new Date("May 11 13:23:41 2022 GMT"));
			assert.equal(cert.ca, false);
			assert.equal(cert.pathlen, 0);
			assert.equal(cert.usage, "");
			assert.equal(cert.type, "");
			assert.equal(cert.publicKey, pk);

			cert.load(ca2);

			var cert1 = cert.next;
			assert.equal(cert1.issuer, "C=NL, O=PolarSSL, CN=PolarSSL Test CA");
			assert.equal(cert1.subject, "C=NL, O=PolarSSL, CN=PolarSSL Cert SHA224");
			assert.deepEqual(cert1.notBefore, new Date("Feb 12 14:44:07 2011 GMT"));
			assert.deepEqual(cert1.notAfter, new Date("Feb 12 14:44:07 2021 GMT"));
			assert.equal(cert1.ca, false);
			assert.equal(cert1.pathlen, 0);
			assert.equal(cert1.usage, "");
			assert.equal(cert1.type, "");
			assert.equal(cert1.publicKey, pk);
		});
	});

	describe("X509 Crl", function() {
		var crl = new crypto.X509Crl();

		it("load", function() {
			var fl = fs.readdir('crl_files/');
			fl.forEach(function(s) {
				if (!s.isDirectory() && s.name.match(/\.pem/))
					crl.load(fs.readFile('crl_files/' + s.name));
			});
		});

		it("clear/export", function() {
			var s = crl.dump();

			crl.clear();
			assert.deepEqual(crl.dump(), []);

			s.forEach(function(c) {
				crl.load(c);
			});

			assert.deepEqual(crl.dump(), s);

			crl.clear();
			crl.load(s.join('\n'));

			assert.deepEqual(crl.dump(), s);
		});

		it("load x509 crl file", function() {
			var s = crl.dump();
			crl.clear();

			var fl = fs.readdir('crl_files/');
			fl.forEach(function(s) {
				if (!s.isDirectory() && s.name.match(/\.pem/))
					crl.loadFile('crl_files/' + s.name);
			});

			assert.deepEqual(crl.dump(), s);
		});

	});

	describe("X509 Req", function() {
		var req = new crypto.X509Req();

		it("load", function() {
			var fl = fs.readdir('req_files/');
			fl.forEach(function(s) {
				if (!s.isDirectory() && s.name.match(/\.req/))
					req.load(fs.readFile('req_files/' + s.name));
			});
		});

		it("toString", function() {
			assert.equal(req.exportPem(), req);
		});

		it("import/export pem", function() {
			var s = req.exportPem();

			var req1 = new crypto.X509Req();
			req1.load(s);

			assert.equal(req1.exportPem(), s);
		});

		it("import/export der", function() {
			var s = req.exportDer();

			var req1 = new crypto.X509Req();
			req1.load(s);

			assert.deepEqual(req1.exportDer().toJSON(), s.toJSON());
		});

		it("create", function() {
			var pk = new crypto.PKey();
			pk.importKey(rsa1024_pem);

			req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk);

			assert.deepEqual(req.exportPem(), req1);
		})

		it("info", function() {
			req.load(req1);
			assert.equal(req.subject, "C=CN, O=baoz.cn, CN=baoz.me");
			assert.equal(req.publicKey, pub_rsa1024_pem);
		});
	});

	describe("CA sign/verify", function() {
		var req;
		var ca;
		var pk;

		before(function() {
			pk = new crypto.PKey();
			pk.importKey(rsa1024_pem);
			req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk);
		});

		it("sign", function() {
			var cert = req.sign("C=CN, O=baoz.cn", pk);
			assert.equal(cert.issuer, "C=CN, O=baoz.cn");
			assert.equal(cert.subject, "C=CN, O=baoz.cn, CN=baoz.me");
			assert.equal(cert.publicKey, pub_rsa1024_pem);
			assert.equal(cert.serial, "1");
		});

		it("ca/pathlen", function() {
			var cert = req.sign("C=CN, O=baoz.cn", pk);
			assert.isFalse(cert.ca);
			assert.equal(cert.pathlen, 0);

			cert = req.sign("C=CN, O=baoz.cn", pk, {
				ca: true,
				pathlen: 10
			});
			assert.isTrue(cert.ca);
			assert.equal(cert.pathlen, 11);
		});

		it("before/after", function() {
			var cert = req.sign("C=CN, O=baoz.cn", pk);
			assert.deepEqual(os.dateAdd(cert.notBefore, 1, "year"), cert.notAfter);

			cert = req.sign("C=CN, O=baoz.cn", pk, {
				notBefore: new Date("2014-12-20 20:20:20")
			});
			assert.deepEqual(cert.notBefore, new Date("2014-12-20 20:20:20"));
			assert.deepEqual(cert.notAfter, new Date("2015-12-20 20:20:20"));

			cert = req.sign("C=CN, O=baoz.cn", pk, {
				notBefore: new Date("2014-12-20 20:20:20"),
				notAfter: new Date("2018-12-20 20:20:20")
			});
			assert.deepEqual(cert.notBefore, new Date("2014-12-20 20:20:20"));
			assert.deepEqual(cert.notAfter, new Date("2018-12-20 20:20:20"));
		});

		it("usage/type", function() {
			var cert = req.sign("C=CN, O=baoz.cn", pk);
			assert.equal(cert.usage, "");
			assert.equal(cert.type, "");

			cert = req.sign("C=CN, O=baoz.cn", pk, {
				usage: "",
				type: ""
			});
			assert.equal(cert.usage, "");
			assert.equal(cert.type, "");

			cert = req.sign("C=CN, O=baoz.cn", pk, {
				usage: "digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment, keyAgreement, keyCertSign, cRLSign",
				type: "client, server, email, objsign, reserved, sslCA, emailCA, objCA"
			});
			assert.equal(cert.usage, "digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment, keyAgreement, keyCertSign, cRLSign");
			assert.equal(cert.type, "client, server, email, objsign, reserved, sslCA, emailCA, objCA");
		});

		it("self-sign", function() {
			req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk);
			ca = req.sign("C=CN, O=baoz.cn, CN=baoz.me", pk, {
				ca: true
			});
		});

		it("verify", function() {
			assert.isTrue(ca.verify(ca));
			assert.isFalse(ca.verify(req.sign("C=CN, O=baoz.cn", pk)));
		});
	});
});

//test.run(console.DEBUG);