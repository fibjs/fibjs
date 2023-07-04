var test = require("test");
test.setup();

var fs = require('fs');
var path = require('path');
var crypto = require('crypto');
var hex = require('hex');
var base64 = require('base64');

describe('bbs signature', () => {
    describe('signature', () => {
        fs.readdir(path.join(__dirname, 'bbs_sha/signature')).forEach(function (file) {
            var _case = require(path.join(__dirname, 'bbs_sha/signature', file));

            it(_case.caseName, () => {
                var sk = crypto.PKey.from({
                    "kty": "EC",
                    "crv": "BLS12381_G2",
                    "d": hex.decode(_case.signerKeyPair.secretKey).base64()
                });

                var pk = crypto.PKey.from({
                    "kty": "EC",
                    "crv": "BLS12381_G2",
                    "x": hex.decode(_case.signerKeyPair.publicKey).base64()
                });

                if (_case.result.valid) {
                    var json_key = sk.json();
                    assert.equal(base64.decode(json_key.x).hex(), _case.signerKeyPair.publicKey);
                }

                var sig = sk.bbsSign(hex.decode(_case.header), _case.messages.map(m => hex.decode(m)));
                assert.isTrue(sk.bbsVerify(hex.decode(_case.header), _case.messages.map(m => hex.decode(m)), sig));

                var result = pk.bbsVerify(hex.decode(_case.header), _case.messages.map(m => hex.decode(m)), hex.decode(_case.signature));
                assert.equal(result, _case.result.valid);
            });
        });
    });

    describe('proof', () => {
        fs.readdir(path.join(__dirname, 'bbs_sha/proof')).forEach(function (file) {
            var _case = require(path.join(__dirname, 'bbs_sha/proof', file));

            it(_case.caseName, () => {
                var pk = crypto.PKey.from({
                    "kty": "EC",
                    "crv": "BLS12381_G2",
                    "x": hex.decode(_case.signerPublicKey).base64()
                });

                var result = pk.proofVerify(hex.decode(_case.header), hex.decode(_case.presentationHeader),
                    Object.values(_case.revealedMessages).map(m => hex.decode(m)),
                    Object.keys(_case.revealedMessages).map(m => Number(m) + 1), hex.decode(_case.proof));

                assert.equal(result, _case.result.valid);
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);
