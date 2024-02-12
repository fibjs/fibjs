var test = require("test");
test.setup();

var hash = require('hash');
var crypto = require('crypto');

var rsa4096_pem = "-----BEGIN RSA PRIVATE KEY-----\n" +
    "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\n" +
    "-----END RSA PRIVATE KEY-----\n";

var pub_rsa4096_pem = "-----BEGIN PUBLIC KEY-----\n" +
    "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAiAUwIJyWHEnusONGDVCg\n" +
    "uTr3FkkVoSiDJ2mmFYYibt1paXpIiEKQy+OwykFcOG0Ew9UGEEvveRQbKN+bpUV5\n" +
    "uH6q7pgnfDubA2VSLST7lHcXJd2as4DNyFMtgXLOj0dVSk4NduWlVgY2fUu17sJJ\n" +
    "BmysYjCjmB76P92DLfIvlI+4OYEOFSLQCG56KA8ZvP9HTnHiI0Lnerjf14cjr1Ub\n" +
    "gw0ce9KMJ96LLnZp9Q6hBCXalagDo9MofuyScpFX1X0ixirQ2VtomzTvle9EIdfm\n" +
    "at6EoLzxz8YNfl6mllvCVrlFJw1DvXfxLlkDR9I+te1B89L7Eljt90VBKmzjjJiK\n" +
    "muhmW8HhJMpe3VBG7t8c3J+F/cJLx7ZDk6/XNhxWPbzL9SNhTRg6015YDsX01bFy\n" +
    "i1GUHdFocPmzgZ6nw3anFT85+q7l9M4yGhRQbfUWQ4rT0zCFobJZQeO4ZyiBM0fu\n" +
    "AGhln0MogIdFaoh7xmVggL6XxJVOApXXGV7Z8xDjzbLpBI2UHiX8W33ZbrXYiDex\n" +
    "vfBQuR2SFw372h/LCF+fOI8kxZr9BNU1RSf26RCX0Mi0Fdbz0eeXLa3N42fGAxHY\n" +
    "UCN5M4WEtAPcdQjQAZ7gZ+Voy9nNjMoQMWzzH84XHUI1byA8T795CnVmKMiHyCWE\n" +
    "nOGV1ptmZrzoZoa6x6bOHukCAwEAAQ==\n" +
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


var sm2_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
    "MHcCAQEEIH3EUWpWsnLGl6SkGBnG5lPEIvdyql56aHQMCCt7xDqCoAoGCCqBHM9V\n" +
    "AYItoUQDQgAE1KnIoMvdNODUrcEzQNnHbplwxNNyuHwIUnU0oNQ/0R1z97YIe/k8\n" +
    "HX6wrPMUazfS1PVd/A9R8gadvlURQ3lufg==\n" +
    "-----END EC PRIVATE KEY-----\n";

var pub_sm2_pem = "-----BEGIN PUBLIC KEY-----\n" +
    "MFkwEwYHKoZIzj0CAQYIKoEcz1UBgi0DQgAE1KnIoMvdNODUrcEzQNnHbplwxNNy\n" +
    "uHwIUnU0oNQ/0R1z97YIe/k8HX6wrPMUazfS1PVd/A9R8gadvlURQ3lufg==\n" +
    "-----END PUBLIC KEY-----\n";

describe("hash", () => {
    function hash_test(o) {
        assert.equal(o.hash, hash.digest(hash[o.name], o.text).digest().hex());
        assert.equal(o.hash, hash.digest(hash[o.name]).update(o.text).digest().hex());
        assert.equal(o.hash, hash[o.name.toLowerCase()](o.text).digest().hex());
        assert.equal(o.hash, hash[o.name.toLowerCase()]().update(o.text).digest().hex());
        assert.equal(o.hash, crypto.createHash(o.name).update(o.text).digest().hex());

        assert.equal(o.base64, hash.digest(hash[o.name], o.text).digest().base64());
        assert.equal(o.base64, hash.digest(hash[o.name]).update(o.text).digest().base64());
        assert.equal(o.base64, hash[o.name.toLowerCase()](o.text).digest().base64());
        assert.equal(o.base64, hash[o.name.toLowerCase()]().update(o.text).digest().base64());
        assert.equal(o.base64, crypto.createHash(o.name).update(o.text).digest().base64());

        assert.equal(o.hash, hash.digest(hash[o.name], o.text).digest('buffer').hex());
        assert.equal(o.hash, hash.digest(hash[o.name]).update(o.text).digest('buffer').hex());
        assert.equal(o.hash, hash[o.name.toLowerCase()](o.text).digest('buffer').hex());
        assert.equal(o.hash, hash[o.name.toLowerCase()]().update(o.text).digest('buffer').hex());
        assert.equal(o.hash, crypto.createHash(o.name).update(o.text).digest('buffer').hex());

        assert.equal(o.base64, hash.digest(hash[o.name], o.text).digest('buffer').base64());
        assert.equal(o.base64, hash.digest(hash[o.name]).update(o.text).digest('buffer').base64());
        assert.equal(o.base64, hash[o.name.toLowerCase()](o.text).digest('buffer').base64());
        assert.equal(o.base64, hash[o.name.toLowerCase()]().update(o.text).digest('buffer').base64());
        assert.equal(o.base64, crypto.createHash(o.name).update(o.text).digest('buffer').base64());

        assert.equal(o.hash, hash.digest(hash[o.name], o.text).digest('hex'));
        assert.equal(o.hash, hash.digest(hash[o.name]).update(o.text).digest('hex'));
        assert.equal(o.hash, hash[o.name.toLowerCase()](o.text).digest('hex'));
        assert.equal(o.hash, hash[o.name.toLowerCase()]().update(o.text).digest('hex'));
        assert.equal(o.hash, crypto.createHash(o.name).update(o.text).digest('hex'));

        assert.equal(o.base64, hash.digest(hash[o.name], o.text).digest('base64'));
        assert.equal(o.base64, hash.digest(hash[o.name]).update(o.text).digest('base64'));
        assert.equal(o.base64, hash[o.name.toLowerCase()](o.text).digest('base64'));
        assert.equal(o.base64, hash[o.name.toLowerCase()]().update(o.text).digest('base64'));
        assert.equal(o.base64, crypto.createHash(o.name).update(o.text).digest('base64'));

        if (hash[o.name] <= hash.SM2) {
            var s = crypto.createHash(o.name).update(o.text).sign(rsa4096_pem);
            assert.ok(crypto.createHash(o.name).update(o.text).verify(pub_rsa4096_pem, s));
            assert.ok(new crypto.PKey(pub_rsa4096_pem).verify(crypto.createHash(o.name).update(o.text).digest(), s, { alg: hash[o.name] }));
        }

        var s = crypto.createHash(o.name).update(o.text).sign(ec_pem);
        assert.ok(crypto.createHash(o.name).update(o.text).verify(pub_ec_pem, s));
        assert.ok(new crypto.PKey(pub_ec_pem).verify(crypto.createHash(o.name).update(o.text).digest(), s));

        var s = crypto.createHash(o.name).update(o.text).sign(sm2_pem);
        assert.ok(crypto.createHash(o.name).update(o.text).verify(pub_sm2_pem, s));
        assert.ok(new crypto.PKey(pub_sm2_pem).verify(crypto.createHash(o.name).update(o.text).digest(), s));

        if (o.text != '')
            assert.equal(o.hash, hash.digest(hash[o.name], o.text.substr(0, 1)).update(o.text.substr(1)).digest().hex());
    }

    function hmac_test(o) {
        assert.equal(o.hmac, hash.hmac(hash[o.name], o.key, o.text).digest().hex());
        assert.equal(o.hmac, hash.hmac(hash[o.name], o.key).update(o.text).digest().hex());
        assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()](o.key, o.text).digest().hex());
        assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()](o.key).update(o.text).digest().hex());
        assert.equal(o.hmac, crypto.createHmac(o.name, o.key).update(o.text).digest().hex());

        assert.equal(o.base64, hash.hmac(hash[o.name], o.key, o.text).digest().base64());
        assert.equal(o.base64, hash.hmac(hash[o.name], o.key).update(o.text).digest().base64());
        assert.equal(o.base64, hash['hmac_' + o.name.toLowerCase()](o.key, o.text).digest().base64());
        assert.equal(o.base64, hash['hmac_' + o.name.toLowerCase()](o.key).update(o.text).digest().base64());
        assert.equal(o.base64, crypto.createHmac(o.name, o.key).update(o.text).digest().base64());

        assert.equal(o.hmac, hash.hmac(hash[o.name], o.key, o.text).digest('buffer').hex());
        assert.equal(o.hmac, hash.hmac(hash[o.name], o.key).update(o.text).digest('buffer').hex());
        assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()](o.key, o.text).digest('buffer').hex());
        assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()](o.key).update(o.text).digest('buffer').hex());
        assert.equal(o.hmac, crypto.createHmac(o.name, o.key).update(o.text).digest('buffer').hex());

        assert.equal(o.base64, hash.hmac(hash[o.name], o.key, o.text).digest('buffer').base64());
        assert.equal(o.base64, hash.hmac(hash[o.name], o.key).update(o.text).digest('buffer').base64());
        assert.equal(o.base64, hash['hmac_' + o.name.toLowerCase()](o.key, o.text).digest('buffer').base64());
        assert.equal(o.base64, hash['hmac_' + o.name.toLowerCase()](o.key).update(o.text).digest('buffer').base64());
        assert.equal(o.base64, crypto.createHmac(o.name, o.key).update(o.text).digest('buffer').base64());

        assert.equal(o.hmac, hash.hmac(hash[o.name], o.key, o.text).digest('hex'));
        assert.equal(o.hmac, hash.hmac(hash[o.name], o.key).update(o.text).digest('hex'));
        assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()](o.key, o.text).digest('hex'));
        assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()](o.key).update(o.text).digest('hex'));
        assert.equal(o.hmac, crypto.createHmac(o.name, o.key).update(o.text).digest('hex'));

        assert.equal(o.base64, hash.hmac(hash[o.name], o.key, o.text).digest('base64'));
        assert.equal(o.base64, hash.hmac(hash[o.name], o.key).update(o.text).digest('base64'));
        assert.equal(o.base64, hash['hmac_' + o.name.toLowerCase()](o.key, o.text).digest('base64'));
        assert.equal(o.base64, hash['hmac_' + o.name.toLowerCase()](o.key).update(o.text).digest('base64'));
        assert.equal(o.base64, crypto.createHmac(o.name, o.key).update(o.text).digest('base64'));

        if (hash[o.name] <= hash.SM2) {
            var s = crypto.createHmac(o.name, o.key).update(o.text).sign(rsa4096_pem);
            assert.ok(crypto.createHmac(o.name, o.key).update(o.text).verify(pub_rsa4096_pem, s));
            assert.ok(new crypto.PKey(pub_rsa4096_pem).verify(crypto.createHmac(o.name, o.key).update(o.text).digest(), s, { alg: hash[o.name] }));
        }

        var s = crypto.createHmac(o.name, o.key).update(o.text).sign(ec_pem);
        assert.ok(crypto.createHmac(o.name, o.key).update(o.text).verify(pub_ec_pem, s));
        assert.ok(new crypto.PKey(pub_ec_pem).verify(crypto.createHmac(o.name, o.key).update(o.text).digest(), s));

        var s = crypto.createHmac(o.name, o.key).update(o.text).sign(sm2_pem);
        assert.ok(crypto.createHmac(o.name, o.key).update(o.text).verify(pub_sm2_pem, s));
        assert.ok(new crypto.PKey(pub_sm2_pem).verify(crypto.createHmac(o.name, o.key).update(o.text).digest(), s));
    }

    it('getHashes', () => {
        assert.deepEqual(crypto.getHashes(), [
            "RSA-MD5",
            "RSA-RIPEMD160",
            "RSA-SHA1",
            "RSA-SHA1-2",
            "RSA-SHA224",
            "RSA-SHA256",
            "RSA-SHA3-224",
            "RSA-SHA3-256",
            "RSA-SHA3-384",
            "RSA-SHA3-512",
            "RSA-SHA384",
            "RSA-SHA512",
            "RSA-SHA512/224",
            "RSA-SHA512/256",
            "RSA-SM3",
            "blake2b512",
            "blake2s256",
            "id-rsassa-pkcs1-v1_5-with-sha3-224",
            "id-rsassa-pkcs1-v1_5-with-sha3-256",
            "id-rsassa-pkcs1-v1_5-with-sha3-384",
            "id-rsassa-pkcs1-v1_5-with-sha3-512",
            "md5",
            "md5-sha1",
            "md5WithRSAEncryption",
            "ripemd",
            "ripemd160",
            "ripemd160WithRSA",
            "rmd160",
            "sha1",
            "sha1WithRSAEncryption",
            "sha224",
            "sha224WithRSAEncryption",
            "sha256",
            "sha256WithRSAEncryption",
            "sha3-224",
            "sha3-256",
            "sha3-384",
            "sha3-512",
            "sha384",
            "sha384WithRSAEncryption",
            "sha512",
            "sha512-224",
            "sha512-224WithRSAEncryption",
            "sha512-256",
            "sha512-256WithRSAEncryption",
            "sha512WithRSAEncryption",
            "shake128",
            "shake256",
            "sm3",
            "sm3WithRSAEncryption",
            "ssl3-md5",
            "ssl3-sha1"
        ]);
    });

    it("md5", () => {
        var digest_case = [{
            name: 'MD5',
            text: '',
            hash: 'd41d8cd98f00b204e9800998ecf8427e',
            base64: '1B2M2Y8AsgTpgAmY7PhCfg=='
        }, {
            name: 'MD5',
            text: 'a',
            hash: '0cc175b9c0f1b6a831c399e269772661',
            base64: 'DMF1ucDxtqgxw5niaXcmYQ=='
        }, {
            name: 'MD5',
            text: 'abc',
            hash: '900150983cd24fb0d6963f7d28e17f72',
            base64: 'kAFQmDzST7DWlj99KOF/cg=='
        }, {
            name: 'MD5',
            text: 'abcdefghijklmnopqrstuvwxyz',
            hash: 'c3fcd3d76192e4007dfb496cca67e13b',
            base64: 'w/zT12GS5AB9+0lsymfhOw=='
        }, {
            name: 'MD5',
            text: 'message digest',
            hash: 'f96b697d7cb7938d525a2f31aaf161d0',
            base64: '+WtpfXy3k41SWi8xqvFh0A=='
        }];

        digest_case.forEach(hash_test);
    });

    it("sha1", () => {
        var digest_case = [{
            name: 'SHA1',
            text: '',
            hash: 'da39a3ee5e6b4b0d3255bfef95601890afd80709',
            base64: '2jmj7l5rSw0yVb/vlWAYkK/YBwk='
        }, {
            name: 'SHA1',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '2fd4e1c67a2d28fced849ee1bb76e7391b93eb12',
            base64: 'L9ThxnotKPzthJ7hu3bnORuT6xI='
        }, {
            name: 'SHA1',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3',
            base64: '3p8sf9JeGzr60+haC9F9mxANtLM='
        }];

        digest_case.forEach(hash_test);
    });

    it("sha224", () => {
        var digest_case = [{
            name: 'SHA224',
            text: '',
            hash: 'd14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f',
            base64: '0UoCjCo6K8lHYQK7KII0xBWisB+CjqYqxbPkLw=='
        }, {
            name: 'SHA224',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525',
            base64: 'cw4Qm9eooyscudmgmqIyXSQwWH3bwMOLrZEVJQ=='
        }, {
            name: 'SHA224',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: '619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c',
            base64: 'YZy6jo4Fgm6bjFGcClxo9PtlPoo9iqBLssjNTA=='
        }];

        digest_case.forEach(hash_test);
    });

    it("sha256", () => {
        var digest_case = [{
            name: 'SHA256',
            text: '',
            hash: 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855',
            base64: '47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU='
        }, {
            name: 'SHA256',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: 'd7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592',
            base64: '16j7swfXgJRpypq8sAguT41WUeRtPNt2LQLQvzfJ5ZI='
        }, {
            name: 'SHA256',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: 'ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c',
            base64: '71N/JciVv6eCUmUpqbY9l6pjFWTV14nCt2VEjIY1+2w='
        }];

        digest_case.forEach(hash_test);
    });

    it("sha384", () => {
        var digest_case = [{
            name: 'SHA384',
            text: '',
            hash: '38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b',
            base64: 'OLBgp1GsljhM2TJ+sbHjaiH9txEUvgdDTAzHv2P24donTt6/529l+9Ua0vFImLlb'
        }, {
            name: 'SHA384',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: 'ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1',
            base64: 'ynN/EBSkj0wLbdQ8sXewr9nlFpNnVExJQBHjMX2/mlCcseXcHoWpQbvuPX8q+8mx'
        }, {
            name: 'SHA384',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: 'ed892481d8272ca6df370bf706e4d7bc1b5739fa2177aae6c50e946678718fc67a7af2819a021c2fc34e91bdb63409d7',
            base64: '7YkkgdgnLKbfNwv3BuTXvBtXOfohd6rmxQ6UZnhxj8Z6evKBmgIcL8NOkb22NAnX'
        }];

        digest_case.forEach(hash_test);
    });

    it("sha512", () => {
        var digest_case = [{
            name: 'SHA512',
            text: '',
            hash: 'cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e',
            base64: 'z4PhNX7vuL3xVChQ1m2AB9Yg5AULVxXcg/SpIdNs6c5H0NE8XYXysP+DGNKHfuwvY7kxvUdBeoGlODJ6+SfaPg=='
        }, {
            name: 'SHA512',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6',
            base64: 'B+VH2VhvanP3P7rAQ17XaVEhj7fQyNeIownXhUNru2Quk6JSqVTyORJUfR6KO17W4b/XCXghIz+gU489uFT+5g=='
        }, {
            name: 'SHA512',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: '91ea1245f20d46ae9a037a989f54f1f790f0a47607eeb8a14d12890cea77a1bbc6c7ed9cf205e67b7f2b8fd4c7dfd3a7a8617e45f3c463d481c7e586c39ac1ed',
            base64: 'keoSRfINRq6aA3qYn1Tx95DwpHYH7rihTRKJDOp3obvGx+2c8gXme38rj9TH39OnqGF+RfPEY9SBx+WGw5rB7Q=='
        }];

        digest_case.forEach(hash_test);
    });

    it("ripemd160", () => {
        var digest_case = [{
            name: 'RIPEMD160',
            text: '',
            hash: '9c1185a5c5e9fc54612808977ee8f548b2258d31',
            base64: 'nBGFpcXp/FRhKAiXfuj1SLIljTE='
        }, {
            name: 'RIPEMD160',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '37f332f68db77bd9d7edd4969571ad671cf9dd3b',
            base64: 'N/My9o23e9nX7dSWlXGtZxz53Ts='
        }, {
            name: 'RIPEMD160',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: '132072df690933835eb8b6ad0b77e7b6f14acad7',
            base64: 'EyBy32kJM4NeuLatC3fntvFKytc='
        }];

        digest_case.forEach(hash_test);
    });

    it("sm3", () => {
        var digest_case = [{
            name: 'SM3',
            text: '',
            hash: '1ab21d8355cfa17f8e61194831e81a8f22bec8c728fefb747ed035eb5082aa2b',
            base64: 'GrIdg1XPoX+OYRlIMegajyK+yMco/vt0ftA161CCqis='
        }, {
            name: 'SM3',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '5fdfe814b8573ca021983970fc79b2218c9570369b4859684e2e4c3fc76cb8ea',
            base64: 'X9/oFLhXPKAhmDlw/HmyIYyVcDabSFloTi5MP8dsuOo='
        }, {
            name: 'SM3',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'ca27d14a42fc04c1e5ecf574a95a8c2d70ecb5805e9b429026ccac8f28b20098',
            base64: 'yifRSkL8BMHl7PV0qVqMLXDstYBem0KQJsysjyiyAJg='
        }];

        digest_case.forEach(hash_test);
    });

    it("sha3_256", () => {
        var digest_case = [{
            name: 'SHA3_256',
            text: '',
            hash: 'a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a',
            base64: 'p//G+L8e12ZRwUdWoGHWYvWA/03kO0n6gtgKS4D4Q0o='
        }, {
            name: 'SHA3_256',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04',
            base64: 'aQcN2gGXXIwSDDqtobKCOU5/Ay+pzzL0yyJZoIl9/AQ='
        }, {
            name: 'SHA3_256',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'cc80b0b13ba89613d93f02ee7ccbe72ee26c6edfe577f22e63a1380221caedbc',
            base64: 'zICwsTuolhPZPwLufMvnLuJsbt/ld/IuY6E4AiHK7bw='
        }];

        digest_case.forEach(hash_test);
    });

    it("sha3_384", () => {
        var digest_case = [{
            name: 'SHA3_384',
            text: '',
            hash: '0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004',
            base64: 'DGOnW4ReT30BEH2FLkwkhcUaUKqqlPxhmV5xu+6YOirDcTgxJkrbR/tr0eBY1fAE'
        }, {
            name: 'SHA3_384',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '7063465e08a93bce31cd89d2e3ca8f602498696e253592ed26f07bf7e703cf328581e1471a7ba7ab119b1a9ebdf8be41',
            base64: 'cGNGXgipO84xzYnS48qPYCSYaW4lNZLtJvB79+cDzzKFgeFHGnunqxGbGp69+L5B'
        }, {
            name: 'SHA3_384',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'e414797403c7d01ab64b41e90df4165d59b7f147e4292ba2da336acba242fd651949eb1cfff7e9012e134b40981842e1',
            base64: '5BR5dAPH0Bq2S0HpDfQWXVm38UfkKSui2jNqy6JC/WUZSesc//fpAS4TS0CYGELh'
        }];

        digest_case.forEach(hash_test);
    });

    it("sha3_512", () => {
        var digest_case = [{
            name: 'SHA3_512',
            text: '',
            hash: 'a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26',
            base64: 'pp9zzKI6msXItWfcGFp1bpfJghZP4lhZ4NHcwUdcgKYVshI68fX5TBHj6UAsOsVY9QAZnZW20+MBdYWGKB3NJg=='
        }, {
            name: 'SHA3_512',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '01dedd5de4ef14642445ba5f5b97c15e47b9ad931326e4b0727cd94cefc44fff23f07bf543139939b49128caf436dc1bdee54fcb24023a08d9403f9b4bf0d450',
            base64: 'Ad7dXeTvFGQkRbpfW5fBXke5rZMTJuSwcnzZTO/ET/8j8Hv1QxOZObSRKMr0Ntwb3uVPyyQCOgjZQD+bS/DUUA=='
        }, {
            name: 'SHA3_512',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: '28e361fe8c56e617caa56c28c7c36e5c13be552b77081be82b642f08bb7ef085b9a81910fe98269386b9aacfd2349076c9506126e198f6f6ad44c12017ca77b1',
            base64: 'KONh/oxW5hfKpWwox8NuXBO+VSt3CBvoK2QvCLt+8IW5qBkQ/pgmk4a5qs/SNJB2yVBhJuGY9vatRMEgF8p3sQ=='
        }];

        digest_case.forEach(hash_test);
    });

    it("shake128", () => {
        var digest_case = [{
            name: 'SHAKE128',
            text: '',
            hash: '7f9c2ba4e88f827d616045507605853e',
            base64: 'f5wrpOiPgn1hYEVQdgWFPg=='
        }, {
            name: 'SHAKE128',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: 'f4202e3c5852f9182a0430fd8144f0a7',
            base64: '9CAuPFhS+RgqBDD9gUTwpw=='
        }, {
            name: 'SHAKE128',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: '22fd225fb8f2c8d0d2097e1f8d38b6a9',
            base64: 'Iv0iX7jyyNDSCX4fjTi2qQ=='
        }];

        digest_case.forEach(hash_test);
    });

    it("shake256", () => {
        var digest_case = [{
            name: 'SHAKE256',
            text: '',
            hash: '46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762f',
            base64: 'RrndKwuojRMjOz/rdD7rJD/NUupiuBuCtQwnZG7Vdi8='
        }, {
            name: 'SHAKE256',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '2f671343d9b2e1604dc9dcf0753e5fe15c7c64a0d283cbbf722d411a0e36f6ca',
            base64: 'L2cTQ9my4WBNydzwdT5f4Vx8ZKDSg8u/ci1BGg429so='
        }, {
            name: 'SHAKE256',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'fffcaac0606c0edb7bc0d15f033accb68538159016e5ae8470bf9ebea89fa6c9',
            base64: '//yqwGBsDtt7wNFfAzrMtoU4FZAW5a6EcL+evqifpsk='
        }];

        digest_case.forEach(hash_test);
    });

    it("blake2s", () => {
        var digest_case = [{
            name: 'BLAKE2S',
            text: '',
            hash: '69217a3079908094e11121d042354a7c1f55b6482ca1a51e1b250dfd1ed0eef9',
            base64: 'aSF6MHmQgJThESHQQjVKfB9VtkgsoaUeGyUN/R7Q7vk='
        }, {
            name: 'BLAKE2S',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '606beeec743ccbeff6cbcdf5d5302aa855c256c29b88c8ed331ea1a6bf3c8812',
            base64: 'YGvu7HQ8y+/2y8311TAqqFXCVsKbiMjtMx6hpr88iBI='
        }, {
            name: 'BLAKE2S',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: '94662583a600a12dff357c0a6f1b514a710ef0f587a38e8d2e4d7f67e9c81667',
            base64: 'lGYlg6YAoS3/NXwKbxtRSnEO8PWHo46NLk1/Z+nIFmc='
        }];

        digest_case.forEach(hash_test);
    });

    it("blake2b", () => {
        var digest_case = [{
            name: 'BLAKE2B',
            text: '',
            hash: '786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce',
            base64: 'eGoC90IBWQPGxv2FJVLScpEvR0DhWEdhiobiF/cfVBnSXhAxr+5YUxOJZESTTrBLkDpoWxRIt1XVb3Aa/pvizg=='
        }, {
            name: 'BLAKE2B',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: 'a8add4bdddfd93e4877d2746e62817b116364a1fa7bc148d95090bc7333b3673f82401cf7aa2e4cb1ecd90296e3f14cb5413f8ed77be73045b13914cdcd6a918',
            base64: 'qK3Uvd39k+SHfSdG5igXsRY2Sh+nvBSNlQkLxzM7NnP4JAHPeqLkyx7NkCluPxTLVBP47Xe+cwRbE5FM3NapGA=='
        }, {
            name: 'BLAKE2B',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'af438eea5d8cdb209336a7e85bf58090dc21b49d823f89a7d064c119f127bd361af9c7d109edda0f0e91bdce078d1d86b8e6f25727c98f6d3bb6f50acb2dd376',
            base64: 'r0OO6l2M2yCTNqfoW/WAkNwhtJ2CP4mn0GTBGfEnvTYa+cfRCe3aDw6Rvc4HjR2GuObyVyfJj207tvUKyy3Tdg=='
        }];

        digest_case.forEach(hash_test);
    });

    it("md5_hmac", () => {
        var hmac_case = [{
            name: 'MD5',
            key: '',
            text: '',
            hmac: '74e6f7298a9c2d168935f58c001bad88',
            base64: 'dOb3KYqcLRaJNfWMAButiA=='
        }, {
            name: 'MD5',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: '80070713463e7749b90c2dc24911e275',
            base64: 'gAcHE0Y+d0m5DC3CSRHidQ=='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sha1_hmac", () => {
        var hmac_case = [{
            name: 'SHA1',
            key: '',
            text: '',
            hmac: 'fbdb1d1b18aa6c08324b7d64b71fb76370690e1d',
            base64: '+9sdGxiqbAgyS31ktx+3Y3BpDh0='
        }, {
            name: 'SHA1',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: 'de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9',
            base64: '3nybhbi3iqa8ino29wqQcBydtNk='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sha256_hmac", () => {
        var hmac_case = [{
            name: 'SHA256',
            key: '',
            text: '',
            hmac: 'b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad',
            base64: 'thNnmggU2ex3L5XXeMNfxf8Wl8STcVZTxscSFEKSxa0='
        }, {
            name: 'SHA256',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: 'f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8',
            base64: '97yD9DBThCSxMpjmqm+xQ+9NWaFJRhdZl0edvC0aPNg='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("ripemd160_hmac", () => {
        var hmac_case = [{
            name: 'RIPEMD160',
            key: 'Jefe',
            text: 'what do ya want for nothing?',
            hmac: 'dda6c0213a485a9e24f4742064a7f033b43c4069',
            base64: '3abAITpIWp4k9HQgZKfwM7Q8QGk='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sm3_hmac", () => {
        var hmac_case = [{
            name: 'SM3',
            key: '',
            text: '',
            hmac: '0d23f72ba15e9c189a879aefc70996b06091de6e64d31b7a84004356dd915261',
            base64: 'DSP3K6FenBiah5rvxwmWsGCR3m5k0xt6hABDVt2RUmE='
        }, {
            name: 'SM3',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: 'bd4a34077888162b210645b8ebf74b9af357303789357a27c7fc457244ebd398',
            base64: 'vUo0B3iIFishBkW46/dLmvNXMDeJNXonx/xFckTr05g='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sha3_256_hmac", () => {
        var hmac_case = [{
            name: 'SHA3_256',
            key: '',
            text: '',
            hmac: 'e841c164e5b4f10c9f3985587962af72fd607a951196fc92fb3a5251941784ea',
            base64: '6EHBZOW08QyfOYVYeWKvcv1gepURlvyS+zpSUZQXhOo='
        }, {
            name: 'SHA3_256',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: '8c6e0683409427f8931711b10ca92a506eb1fafa48fadd66d76126f47ac2c333',
            base64: 'jG4Gg0CUJ/iTFxGxDKkqUG6x+vpI+t1m12Em9HrCwzM='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sha3_384_hmac", () => {
        var hmac_case = [{
            name: 'SHA3_384',
            key: '',
            text: '',
            hmac: 'adca89f07bbfbeaf58880c1572379ea2416568fd3b66542bd42599c57c4567e6ae086299ea216c6f3e7aef90b6191d24',
            base64: 'rcqJ8Hu/vq9YiAwVcjeeokFlaP07ZlQr1CWZxXxFZ+auCGKZ6iFsbz5675C2GR0k'
        }, {
            name: 'SHA3_384',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: 'aa739ad9fcdf9be4a04f06680ade7a1bd1e01a0af64accb04366234cf9f6934a0f8589772f857681fcde8acc256091a2',
            base64: 'qnOa2fzfm+SgTwZoCt56G9HgGgr2SsywQ2YjTPn2k0oPhYl3L4V2gfzeiswlYJGi'
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sha3_512_hmac", () => {
        var hmac_case = [{
            name: 'SHA3_512',
            key: '',
            text: '',
            hmac: 'cbcf45540782d4bc7387fbbf7d30b3681d6d66cc435cafd82546b0fce96b367ea79662918436fba442e81a01d0f9592dfcd30f7a7a8f1475693d30be4150ca84',
            base64: 'y89FVAeC1Lxzh/u/fTCzaB1tZsxDXK/YJUaw/OlrNn6nlmKRhDb7pELoGgHQ+Vkt/NMPenqPFHVpPTC+QVDKhA=='
        }, {
            name: 'SHA3_512',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: '237a35049c40b3ef5ddd960b3dc893d8284953b9a4756611b1b61bffcf53edd979f93547db714b06ef0a692062c609b70208ab8d4a280ceee40ed8100f293063',
            base64: 'I3o1BJxAs+9d3ZYLPciT2ChJU7mkdWYRsbYb/89T7dl5+TVH23FLBu8KaSBixgm3AgirjUooDO7kDtgQDykwYw=='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("blake2s_hmac", () => {
        var hmac_case = [{
            name: 'BLAKE2S',
            key: '',
            text: '',
            hmac: 'eaf4bb25938f4d20e72656bbbc7a9bf63c0c18537333c35bdb67db1402661acd',
            base64: '6vS7JZOPTSDnJla7vHqb9jwMGFNzM8Nb22fbFAJmGs0='
        }, {
            name: 'BLAKE2S',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: 'f93215bb90d4af4c3061cd932fb169fb8bb8a91d0b4022baea1271e1323cd9a0',
            base64: '+TIVu5DUr0wwYc2TL7Fp+4u4qR0LQCK66hJx4TI82aA='
        }];

        hmac_case.forEach(hmac_test);
    });

    it("blake2b_hmac", () => {
        var hmac_case = [{
            name: 'BLAKE2B',
            key: '',
            text: '',
            hmac: '198cd2006f66ff83fbbd913f78aca2251caf4f19fe9475aade8cf2091b99a68466775177424f58286886cbae8229644cec747237d4b721735485e17372fdf59c',
            base64: 'GYzSAG9m/4P7vZE/eKyiJRyvTxn+lHWq3ozyCRuZpoRmd1F3Qk9YKGiGy66CKWRM7HRyN9S3IXNUheFzcv31nA=='
        }, {
            name: 'BLAKE2B',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: '92294f92c0dfb9b00ec9ae8bd94d7e7d8a036b885a499f149dfe2fd2199394aaaf6b8894a1730cccb2cd050f9bcf5062a38b51b0dab33207f8ef35ae2c9df51b',
            base64: 'kilPksDfubAOya6L2U1+fYoDa4haSZ8Unf4v0hmTlKqva4iUoXMMzLLNBQ+bz1Bio4tRsNqzMgf47zWuLJ31Gw=='
        }];

        hmac_case.forEach(hmac_test);
    });
});

require.main === module && test.run(console.DEBUG);