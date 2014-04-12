var test = require("test");
test.setup();

var crypto = require("crypto");
var hash = require("hash");
var fs = require("fs");
var encoding = require("encoding");

describe('crypto', function() {
	it("random", function() {
		assert.notEqual(crypto.randomBytes(8).hex(), crypto.randomBytes(8).hex());
	});

	it("pseudoRandomBytes", function() {
		assert.notEqual(crypto.pseudoRandomBytes(8).hex(), crypto.pseudoRandomBytes(8).hex());
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
		var rsa256_pem = "-----BEGIN RSA PRIVATE KEY-----\n" +
			"MIGsAgEAAiEAgDHZmZ0s46SYWi0KB6fipQ1+Hv1iim8eHs6I4gqq9/ECAwEAAQIg\n" +
			"YImpzWWwrUmBNb9fIz7JjODAdp2qObSpFlThbJw5dEECEQDPvyHiP0oNIjE+i7hi\n" +
			"suA5AhEAnfh3HJNKmMbX0CJWFezleQIRAKNSzU20mgAXSIW2jKAzpqkCEQCaVo0Z\n" +
			"UBbL2Uo1QbbVyRQRAhEAqLmEjCZeYSKSQUESvmDNlg==\n" +
			"-----END RSA PRIVATE KEY-----\n"

		var ec_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
			"MIHcAgEBBEIB+QhtQdd9bjWeN2mgq6qoqW51ygslLwP+gwTCSP4ZVpcU0pxwigXm\n" +
			"Ioa7Zzr2Q0OOjzdH/vDIaV9FzOySWSKTVJOgBwYFK4EEACOhgYkDgYYABAE3zTRb\n" +
			"rr5Rsa0xqF4Mn0gBoETTQ5zt6zd5O+LqyiXVUteEWIXZ0sJvdQZvhdZfk9VypQGN\n" +
			"skZ8xvcNVJL6PHNMCwEgR6q67gepHZBFAd75Vt/lCJJOB7SnVHk8Dfu5t49q4Gb2\n" +
			"EdX30QGqQ71qihogY6Jqlmn8aJAZGwyNHPeX+n+F1A==\n" +
			"-----END EC PRIVATE KEY-----\n";

		it("RSA PEM import/export", function() {
			var pk = new crypto.PKey();
			pk.importKey(rsa256_pem);
			assert.equal(pk.exportPem(), rsa256_pem);
		});

		it("RSA Der import/export", function() {
			var pk = new crypto.PKey();
			pk.importKey(rsa256_pem);
			var der = pk.exportDer();
			pk.importKey(der);
			assert.equal(pk.exportPem(), rsa256_pem);
		});

		it("RSA publicKey", function() {
			var pk = new crypto.PKey();
			pk.importKey(rsa256_pem);
			assert.isTrue(pk.isPrivate());

			var pk1 = pk.publicKey();
			assert.isFalse(pk1.isPrivate());

			var pk2 = new crypto.PKey();
			pk2.importKey(pk1.exportPem());
			assert.isFalse(pk2.isPrivate());

			assert.equal(pk1.exportPem(), pk2.exportPem());

			pk2.importKey(pk1.exportDer());
			assert.isFalse(pk2.isPrivate());

			assert.equal(pk1.exportPem(), pk2.exportPem());
		});

		it("RSA gen_key", function() {
			var pk = new crypto.PKey();
			var pk1 = new crypto.PKey();
			pk.genRsaKey(512);
			pk1.genRsaKey(512);

			assert.notEqual(pk.exportPem(), pk1.exportPem());
		});

		it("RSA encrypt/decrypt", function() {
			var pk = new crypto.PKey();
			pk.importKey(rsa256_pem);

			var pk1 = pk.publicKey();

			var d = pk1.encrypt(new Buffer("abcdefg"));
			assert.equal(pk.decrypt(d).toString(), "abcdefg");

			assert.throws(function() {
				pk1.decrypt(d);
			});
		});

		it("RSA sign/verify", function() {
			var pk = new crypto.PKey();
			pk.importKey(rsa256_pem);

			var pk1 = pk.publicKey();

			var md = hash.md5(new Buffer("abcdefg")).digest();
			var md1 = hash.md5(new Buffer("abcdefg1")).digest();
			var d = pk.sign(md);
			assert.isTrue(pk1.verify(d, md));
			assert.isFalse(pk1.verify(d, md1));

			assert.throws(function() {
				pk1.sign(md);
			});
		});

		it("EC PEM import/export", function() {
			var pk = new crypto.PKey();
			pk.importKey(ec_pem);
			assert.equal(pk.exportPem(), ec_pem);
		});

		it("EC Der import/export", function() {
			var pk = new crypto.PKey();
			pk.importKey(ec_pem);
			var der = pk.exportDer();
			pk.importKey(der);
			assert.equal(pk.exportPem(), ec_pem);
		});

		it("EC publicKey", function() {
			var pk = new crypto.PKey();
			pk.importKey(ec_pem);
			assert.isTrue(pk.isPrivate());

			var pk1 = pk.publicKey();
			assert.isFalse(pk1.isPrivate());

			var pk2 = new crypto.PKey();
			pk2.importKey(pk1.exportPem());
			assert.isFalse(pk2.isPrivate());

			assert.equal(pk1.exportPem(), pk2.exportPem());

			pk2.importKey(pk1.exportDer());
			assert.isFalse(pk2.isPrivate());

			assert.equal(pk1.exportPem(), pk2.exportPem());
		});

		it("EC gen_key", function() {
			var pk = new crypto.PKey();
			var pk1 = new crypto.PKey();
			pk.genEcKey();
			pk1.genEcKey();

			assert.notEqual(pk.exportPem(), pk1.exportPem());
		});

		it("EC sign/verify", function() {
			var pk = new crypto.PKey();
			pk.importKey(ec_pem);

			var pk1 = pk.publicKey();

			var md = hash.md5(new Buffer("abcdefg")).digest();
			var md1 = hash.md5(new Buffer("abcdefg1")).digest();
			var d = pk.sign(md);
			assert.isTrue(pk1.verify(d, md));
			assert.isFalse(pk1.verify(d, md1));

			assert.throws(function() {
				pk1.sign(md);
			});
		});

	});
});

//test.run(console.DEBUG);