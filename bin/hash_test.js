var test = require("test");
test.setup();

var hash = require('hash');

describe(
		"hash",
		function() {

			function hash_test(o) {
				assert.equal(o.hash, hash.digest(hash[o.name], o.text).digest()
						.hex());
				assert.equal(o.hash, hash.digest(hash[o.name],
						new Buffer(o.text)).digest().hex());
				assert.equal(o.hash, hash[o.name.toLowerCase()](o.text)
						.digest().hex());
				assert.equal(o.hash, hash.digest(hash[o.name]).digest(o.text)
						.hex());
			}

			function hmac_test(o) {
				assert.equal(o.hmac, hash.hmac(hash[o.name], o.key).digest(
						o.text).hex());
				assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()]
						(o.key).digest(o.text).hex());
			}

			it("md2", function() {
				var digest_case = [ {
					name : 'MD2',
					text : '',
					hash : '8350e5a3e24c153df2275c9f80692773'
				}, {
					name : 'MD2',
					text : 'The quick brown fox jumps over the lazy dog',
					hash : '03d85a0d629d2c442e987525319fc471'
				}, {
					name : 'MD2',
					text : 'The quick brown fox jumps over the lazy cog',
					hash : '6b890c9292668cdbbfda00a4ebf31f05'
				} ];

				digest_case.forEach(hash_test);
			});

			it("md4", function() {
				var digest_case = [ {
					name : 'MD4',
					text : '',
					hash : '31d6cfe0d16ae931b73c59d7e0c089c0'
				}, {
					name : 'MD4',
					text : 'The quick brown fox jumps over the lazy dog',
					hash : '1bee69a46ba811185c194762abaeae90'
				}, {
					name : 'MD4',
					text : 'The quick brown fox jumps over the lazy cog',
					hash : 'b86e130ce7028da59e672d56ad0113df'
				} ];

				digest_case.forEach(hash_test);
			});

			it("md5", function() {
				var digest_case = [ {
					name : 'MD5',
					text : '',
					hash : 'd41d8cd98f00b204e9800998ecf8427e'
				}, {
					name : 'MD5',
					text : 'a',
					hash : '0cc175b9c0f1b6a831c399e269772661'
				}, {
					name : 'MD5',
					text : 'abc',
					hash : '900150983cd24fb0d6963f7d28e17f72'
				}, {
					name : 'MD5',
					text : 'abcdefghijklmnopqrstuvwxyz',
					hash : 'c3fcd3d76192e4007dfb496cca67e13b'
				}, {
					name : 'MD5',
					text : 'message digest',
					hash : 'f96b697d7cb7938d525a2f31aaf161d0'
				} ];

				digest_case.forEach(hash_test);
			});

			it("sha1", function() {
				var digest_case = [ {
					name : 'SHA1',
					text : '',
					hash : 'da39a3ee5e6b4b0d3255bfef95601890afd80709'
				}, {
					name : 'SHA1',
					text : 'The quick brown fox jumps over the lazy dog',
					hash : '2fd4e1c67a2d28fced849ee1bb76e7391b93eb12'
				}, {
					name : 'SHA1',
					text : 'The quick brown fox jumps over the lazy cog',
					hash : 'de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3'
				} ];

				digest_case.forEach(hash_test);
			});

			it(
					"sha224",
					function() {
						var digest_case = [
								{
									name : 'SHA224',
									text : '',
									hash : 'd14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f'
								},
								{
									name : 'SHA224',
									text : 'The quick brown fox jumps over the lazy dog',
									hash : '730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525'
								},
								{
									name : 'SHA224',
									text : 'The quick brown fox jumps over the lazy dog.',
									hash : '619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c'
								} ];

						digest_case.forEach(hash_test);
					});

			it(
					"sha256",
					function() {
						var digest_case = [
								{
									name : 'SHA256',
									text : '',
									hash : 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'
								},
								{
									name : 'SHA256',
									text : 'The quick brown fox jumps over the lazy dog',
									hash : 'd7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592'
								},
								{
									name : 'SHA256',
									text : 'The quick brown fox jumps over the lazy dog.',
									hash : 'ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c'
								} ];

						digest_case.forEach(hash_test);
					});

			it(
					"sha384",
					function() {
						var digest_case = [
								{
									name : 'SHA384',
									text : '',
									hash : '38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b'
								},
								{
									name : 'SHA384',
									text : 'The quick brown fox jumps over the lazy dog',
									hash : 'ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1'
								},
								{
									name : 'SHA384',
									text : 'The quick brown fox jumps over the lazy dog.',
									hash : 'ed892481d8272ca6df370bf706e4d7bc1b5739fa2177aae6c50e946678718fc67a7af2819a021c2fc34e91bdb63409d7'
								} ];

						digest_case.forEach(hash_test);
					});

			it(
					"sha512",
					function() {
						var digest_case = [
								{
									name : 'SHA512',
									text : '',
									hash : 'cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e'
								},
								{
									name : 'SHA512',
									text : 'The quick brown fox jumps over the lazy dog',
									hash : '07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6'
								},
								{
									name : 'SHA512',
									text : 'The quick brown fox jumps over the lazy dog.',
									hash : '91ea1245f20d46ae9a037a989f54f1f790f0a47607eeb8a14d12890cea77a1bbc6c7ed9cf205e67b7f2b8fd4c7dfd3a7a8617e45f3c463d481c7e586c39ac1ed'
								} ];

						digest_case.forEach(hash_test);
					});

			it("ripemd160", function() {
				var digest_case = [ {
					name : 'RIPEMD160',
					text : '',
					hash : '9c1185a5c5e9fc54612808977ee8f548b2258d31'
				}, {
					name : 'RIPEMD160',
					text : 'The quick brown fox jumps over the lazy dog',
					hash : '37f332f68db77bd9d7edd4969571ad671cf9dd3b'
				}, {
					name : 'RIPEMD160',
					text : 'The quick brown fox jumps over the lazy cog',
					hash : '132072df690933835eb8b6ad0b77e7b6f14acad7'
				} ];

				digest_case.forEach(hash_test);
			});

			it("md5_hmac", function() {
				var hmac_case = [ {
					name : 'MD5',
					key : '',
					text : '',
					hmac : '74e6f7298a9c2d168935f58c001bad88'
				}, {
					name : 'MD5',
					key : 'key',
					text : 'The quick brown fox jumps over the lazy dog',
					hmac : '80070713463e7749b90c2dc24911e275'
				} ];

				hmac_case.forEach(hmac_test);
			});

			it("sha1_hmac", function() {
				var hmac_case = [ {
					name : 'SHA1',
					key : '',
					text : '',
					hmac : 'fbdb1d1b18aa6c08324b7d64b71fb76370690e1d'
				}, {
					name : 'SHA1',
					key : 'key',
					text : 'The quick brown fox jumps over the lazy dog',
					hmac : 'de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9'
				} ];

				hmac_case.forEach(hmac_test);
			});

			it(
					"sha256_hmac",
					function() {
						var hmac_case = [
								{
									name : 'SHA256',
									key : '',
									text : '',
									hmac : 'b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad'
								},
								{
									name : 'SHA256',
									key : 'key',
									text : 'The quick brown fox jumps over the lazy dog',
									hmac : 'f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8'
								} ];

						hmac_case.forEach(hmac_test);
					});

			it("ripemd128_hmac", function() {
				var hmac_case = [ {
					name : 'RIPEMD128',
					key : 'Jefe',
					text : 'what do ya want for nothing?',
					hmac : '875f828862b6b334b427c55f9f7ff09b'
				} ];

				hmac_case.forEach(hmac_test);
			});

			it("ripemd160_hmac", function() {
				var hmac_case = [ {
					name : 'RIPEMD160',
					key : 'Jefe',
					text : 'what do ya want for nothing?',
					hmac : 'dda6c0213a485a9e24f4742064a7f033b43c4069'
				} ];

				hmac_case.forEach(hmac_test);
			});

		});

//test.run();
