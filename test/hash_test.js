var test = require("test");
test.setup();

var hash = require('hash');

describe("hash", () => {
    function hash_test(o) {
        assert.equal(o.hash, hash.digest(hash[o.name], o.text).digest().hex());
        assert.equal(o.hash, hash.digest(hash[o.name],
            new Buffer(o.text)).digest().hex());
        assert.equal(o.hash, hash[o.name.toLowerCase()](o.text).digest().hex());
        assert.equal(o.hash, hash.digest(hash[o.name]).digest(o.text).hex());
    }

    function hmac_test(o) {
        assert.equal(o.hmac, hash.hmac(hash[o.name], o.key).digest(o.text).hex());
        assert.equal(o.hmac, hash['hmac_' + o.name.toLowerCase()](o.key).digest(o.text).hex());
    }

    it("md2", () => {
        var digest_case = [{
            name: 'MD2',
            text: '',
            hash: '83 50 e5 a3 e2 4c 15 3d f2 27 5c 9f 80 69 27 73'
        }, {
            name: 'MD2',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '03 d8 5a 0d 62 9d 2c 44 2e 98 75 25 31 9f c4 71'
        }, {
            name: 'MD2',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: '6b 89 0c 92 92 66 8c db bf da 00 a4 eb f3 1f 05'
        }];

        digest_case.forEach(hash_test);
    });

    it("md4", () => {
        var digest_case = [{
            name: 'MD4',
            text: '',
            hash: '31 d6 cf e0 d1 6a e9 31 b7 3c 59 d7 e0 c0 89 c0'
        }, {
            name: 'MD4',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '1b ee 69 a4 6b a8 11 18 5c 19 47 62 ab ae ae 90'
        }, {
            name: 'MD4',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'b8 6e 13 0c e7 02 8d a5 9e 67 2d 56 ad 01 13 df'
        }];

        digest_case.forEach(hash_test);
    });

    it("md5", () => {
        var digest_case = [{
            name: 'MD5',
            text: '',
            hash: 'd4 1d 8c d9 8f 00 b2 04 e9 80 09 98 ec f8 42 7e'
        }, {
            name: 'MD5',
            text: 'a',
            hash: '0c c1 75 b9 c0 f1 b6 a8 31 c3 99 e2 69 77 26 61'
        }, {
            name: 'MD5',
            text: 'abc',
            hash: '90 01 50 98 3c d2 4f b0 d6 96 3f 7d 28 e1 7f 72'
        }, {
            name: 'MD5',
            text: 'abcdefghijklmnopqrstuvwxyz',
            hash: 'c3 fc d3 d7 61 92 e4 00 7d fb 49 6c ca 67 e1 3b'
        }, {
            name: 'MD5',
            text: 'message digest',
            hash: 'f9 6b 69 7d 7c b7 93 8d 52 5a 2f 31 aa f1 61 d0'
        }];

        digest_case.forEach(hash_test);
    });

    it("sha1", () => {
        var digest_case = [{
            name: 'SHA1',
            text: '',
            hash: 'da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 90 af d8 07 09'
        }, {
            name: 'SHA1',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '2f d4 e1 c6 7a 2d 28 fc ed 84 9e e1 bb 76 e7 39 1b 93 eb 12'
        }, {
            name: 'SHA1',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: 'de 9f 2c 7f d2 5e 1b 3a fa d3 e8 5a 0b d1 7d 9b 10 0d b4 b3'
        }];

        digest_case.forEach(hash_test);
    });

    it("sha224", () => {
        var digest_case = [{
            name: 'SHA224',
            text: '',
            hash: 'd1 4a 02 8c 2a 3a 2b c9 47 61 02 bb 28 82 34 c4 15 a2 b0 1f 82 8e a6 2a c5 b3 e4 2f'
        }, {
            name: 'SHA224',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '73 0e 10 9b d7 a8 a3 2b 1c b9 d9 a0 9a a2 32 5d 24 30 58 7d db c0 c3 8b ad 91 15 25'
        }, {
            name: 'SHA224',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: '61 9c ba 8e 8e 05 82 6e 9b 8c 51 9c 0a 5c 68 f4 fb 65 3e 8a 3d 8a a0 4b b2 c8 cd 4c'
        }];

        digest_case.forEach(hash_test);
    });

    it("sha256", () => {
        var digest_case = [{
            name: 'SHA256',
            text: '',
            hash: 'e3 b0 c4 42 98 fc 1c 14 9a fb f4 c8 99 6f b9 24 27 ae 41 e4 64 9b 93 4c a4 95 99 1b 78 52 b8 55'
        }, {
            name: 'SHA256',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: 'd7 a8 fb b3 07 d7 80 94 69 ca 9a bc b0 08 2e 4f 8d 56 51 e4 6d 3c db 76 2d 02 d0 bf 37 c9 e5 92'
        }, {
            name: 'SHA256',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: 'ef 53 7f 25 c8 95 bf a7 82 52 65 29 a9 b6 3d 97 aa 63 15 64 d5 d7 89 c2 b7 65 44 8c 86 35 fb 6c'
        }];

        digest_case.forEach(hash_test);
    });

    it("sha384", () => {
        var digest_case = [{
            name: 'SHA384',
            text: '',
            hash: '38 b0 60 a7 51 ac 96 38 4c d9 32 7e b1 b1 e3 6a 21 fd b7 11 14 be 07 43 4c 0c c7 bf 63 f6 e1 da 27 4e de bf e7 6f 65 fb d5 1a d2 f1 48 98 b9 5b'
        }, {
            name: 'SHA384',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: 'ca 73 7f 10 14 a4 8f 4c 0b 6d d4 3c b1 77 b0 af d9 e5 16 93 67 54 4c 49 40 11 e3 31 7d bf 9a 50 9c b1 e5 dc 1e 85 a9 41 bb ee 3d 7f 2a fb c9 b1'
        }, {
            name: 'SHA384',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: 'ed 89 24 81 d8 27 2c a6 df 37 0b f7 06 e4 d7 bc 1b 57 39 fa 21 77 aa e6 c5 0e 94 66 78 71 8f c6 7a 7a f2 81 9a 02 1c 2f c3 4e 91 bd b6 34 09 d7'
        }];

        digest_case.forEach(hash_test);
    });

    it("sha512", () => {
        var digest_case = [{
            name: 'SHA512',
            text: '',
            hash: 'cf 83 e1 35 7e ef b8 bd f1 54 28 50 d6 6d 80 07 d6 20 e4 05 0b 57 15 dc 83 f4 a9 21 d3 6c e9 ce 47 d0 d1 3c 5d 85 f2 b0 ff 83 18 d2 87 7e ec 2f 63 b9 31 bd 47 41 7a 81 a5 38 32 7a f9 27 da 3e'
        }, {
            name: 'SHA512',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '07 e5 47 d9 58 6f 6a 73 f7 3f ba c0 43 5e d7 69 51 21 8f b7 d0 c8 d7 88 a3 09 d7 85 43 6b bb 64 2e 93 a2 52 a9 54 f2 39 12 54 7d 1e 8a 3b 5e d6 e1 bf d7 09 78 21 23 3f a0 53 8f 3d b8 54 fe e6'
        }, {
            name: 'SHA512',
            text: 'The quick brown fox jumps over the lazy dog.',
            hash: '91 ea 12 45 f2 0d 46 ae 9a 03 7a 98 9f 54 f1 f7 90 f0 a4 76 07 ee b8 a1 4d 12 89 0c ea 77 a1 bb c6 c7 ed 9c f2 05 e6 7b 7f 2b 8f d4 c7 df d3 a7 a8 61 7e 45 f3 c4 63 d4 81 c7 e5 86 c3 9a c1 ed'
        }];

        digest_case.forEach(hash_test);
    });

    it("ripemd160", () => {
        var digest_case = [{
            name: 'RIPEMD160',
            text: '',
            hash: '9c 11 85 a5 c5 e9 fc 54 61 28 08 97 7e e8 f5 48 b2 25 8d 31'
        }, {
            name: 'RIPEMD160',
            text: 'The quick brown fox jumps over the lazy dog',
            hash: '37 f3 32 f6 8d b7 7b d9 d7 ed d4 96 95 71 ad 67 1c f9 dd 3b'
        }, {
            name: 'RIPEMD160',
            text: 'The quick brown fox jumps over the lazy cog',
            hash: '13 20 72 df 69 09 33 83 5e b8 b6 ad 0b 77 e7 b6 f1 4a ca d7'
        }];

        digest_case.forEach(hash_test);
    });

    it("md5_hmac", () => {
        var hmac_case = [{
            name: 'MD5',
            key: '',
            text: '',
            hmac: '74 e6 f7 29 8a 9c 2d 16 89 35 f5 8c 00 1b ad 88'
        }, {
            name: 'MD5',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: '80 07 07 13 46 3e 77 49 b9 0c 2d c2 49 11 e2 75'
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sha1_hmac", () => {
        var hmac_case = [{
            name: 'SHA1',
            key: '',
            text: '',
            hmac: 'fb db 1d 1b 18 aa 6c 08 32 4b 7d 64 b7 1f b7 63 70 69 0e 1d'
        }, {
            name: 'SHA1',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: 'de 7c 9b 85 b8 b7 8a a6 bc 8a 7a 36 f7 0a 90 70 1c 9d b4 d9'
        }];

        hmac_case.forEach(hmac_test);
    });

    it("sha256_hmac", () => {
        var hmac_case = [{
            name: 'SHA256',
            key: '',
            text: '',
            hmac: 'b6 13 67 9a 08 14 d9 ec 77 2f 95 d7 78 c3 5f c5 ff 16 97 c4 93 71 56 53 c6 c7 12 14 42 92 c5 ad'
        }, {
            name: 'SHA256',
            key: 'key',
            text: 'The quick brown fox jumps over the lazy dog',
            hmac: 'f7 bc 83 f4 30 53 84 24 b1 32 98 e6 aa 6f b1 43 ef 4d 59 a1 49 46 17 59 97 47 9d bc 2d 1a 3c d8'
        }];

        hmac_case.forEach(hmac_test);
    });

    it("ripemd160_hmac", () => {
        var hmac_case = [{
            name: 'RIPEMD160',
            key: 'Jefe',
            text: 'what do ya want for nothing?',
            hmac: 'dd a6 c0 21 3a 48 5a 9e 24 f4 74 20 64 a7 f0 33 b4 3c 40 69'
        }];

        hmac_case.forEach(hmac_test);
    });

});

// test.run(console.DEBUG);