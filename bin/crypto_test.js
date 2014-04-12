var test = require("test");
test.setup();

var crypto = require("crypto");
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
		var rsa128_pem = "-----BEGIN RSA PRIVATE KEY-----\n" +
			"MGMCAQACEQCK7VZZvuzNBbJQ7tZ1y9fNAgMBAAECEESzCLhoiYD4TJ7+EczDyKEC\n" +
			"CQDRllawen5N0wIJAKmxOGcc0J/fAgkAzfoKSHaaGrECCQCkgYf6fnbJ5QIIdzqd\n" +
			"8E48G5U=\n" +
			"-----END RSA PRIVATE KEY-----\n";

		var ec_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
			"MIHcAgEBBEIB+QhtQdd9bjWeN2mgq6qoqW51ygslLwP+gwTCSP4ZVpcU0pxwigXm\n" +
			"Ioa7Zzr2Q0OOjzdH/vDIaV9FzOySWSKTVJOgBwYFK4EEACOhgYkDgYYABAE3zTRb\n" +
			"rr5Rsa0xqF4Mn0gBoETTQ5zt6zd5O+LqyiXVUteEWIXZ0sJvdQZvhdZfk9VypQGN\n" +
			"skZ8xvcNVJL6PHNMCwEgR6q67gepHZBFAd75Vt/lCJJOB7SnVHk8Dfu5t49q4Gb2\n" +
			"EdX30QGqQ71qihogY6Jqlmn8aJAZGwyNHPeX+n+F1A==\n" +
			"-----END EC PRIVATE KEY-----\n";

		it("RSA PEM import/export", function() {
			var pk = new crypto.PKey();
			pk.import(rsa128_pem);
			assert.equal(pk.exportPem(), rsa128_pem);
		});

		it("RSA Der import/export", function() {
			var pk = new crypto.PKey();
			pk.import(rsa128_pem);
			var der = pk.exportDer();
			pk.import(der);
			assert.equal(pk.exportPem(), rsa128_pem);
		});

		it("RSA publicKey", function() {
			var pk = new crypto.PKey();
			pk.import(rsa128_pem);
			assert.isTrue(pk.isPrivate());

			var pk1 = pk.publicKey();
			assert.isFalse(pk1.isPrivate());

			var pk2 = new crypto.PKey();
			pk2.import(pk1.exportPem());
			assert.isFalse(pk2.isPrivate());

			assert.equal(pk1.exportPem(), pk2.exportPem());

			pk2.import(pk1.exportDer());
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

		it("EC PEM import/export", function() {
			var pk = new crypto.PKey();
			pk.import(ec_pem);
			assert.equal(pk.exportPem(), ec_pem);
		});

		it("EC Der import/export", function() {
			var pk = new crypto.PKey();
			pk.import(ec_pem);
			var der = pk.exportDer();
			pk.import(der);
			assert.equal(pk.exportPem(), ec_pem);
		});

		it("EC publicKey", function() {
			var pk = new crypto.PKey();
			pk.import(ec_pem);
			assert.isTrue(pk.isPrivate());

			var pk1 = pk.publicKey();
			assert.isFalse(pk1.isPrivate());

			var pk2 = new crypto.PKey();
			pk2.import(pk1.exportPem());
			assert.isFalse(pk2.isPrivate());

			assert.equal(pk1.exportPem(), pk2.exportPem());

			pk2.import(pk1.exportDer());
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
	});
});

//test.run(console.DEBUG);