var test = require("test");
test.setup();

var fs = require('fs');
var path = require('path');
var crypto = require('crypto');
var hex = require('hex');
var base64 = require('base64');

describe('bbs signature', () => {
    function add_suite(suite) {
        describe(suite, () => {
            describe('signature', () => {
                it("sign/verify", () => {
                    var messages = ["message1", "message2"];
                    var keys = crypto.generateKeyPair("Bls12381G2");

                    var sig = crypto.bbsSign(messages, {
                        key: keys.privateKey,
                        suite: suite
                    });

                    assert.isTrue(crypto.bbsVerify(messages, {
                        key: keys.publicKey,
                        suite: suite
                    }, sig));
                });

                it("with header", () => {
                    var messages = ["message1", "message2"];
                    var keys = crypto.generateKeyPair("Bls12381G2");

                    var sig = crypto.bbsSign(messages, {
                        key: keys.privateKey,
                        suite: suite,
                        header: Buffer.from("aabbccdd")
                    });

                    assert.isFalse(crypto.bbsVerify(messages, {
                        key: keys.publicKey,
                        suite: suite
                    }, sig));

                    assert.isTrue(crypto.bbsVerify(messages, {
                        key: keys.publicKey,
                        suite: suite,
                        header: Buffer.from("aabbccdd")
                    }, sig));

                    assert.isTrue(crypto.bbsVerify(messages, {
                        key: keys.publicKey,
                        suite: suite,
                        header: "aabbccdd"
                    }, sig));

                    var sig = crypto.bbsSign(messages, {
                        key: keys.privateKey,
                        suite: suite,
                        header: "aabbccdd"
                    });

                    assert.isFalse(crypto.bbsVerify(messages, {
                        key: keys.publicKey,
                        suite: suite
                    }, sig));

                    assert.isTrue(crypto.bbsVerify(messages, {
                        key: keys.publicKey,
                        suite: suite,
                        header: Buffer.from("aabbccdd")
                    }, sig));

                    assert.isTrue(crypto.bbsVerify(messages, {
                        key: keys.publicKey,
                        suite: suite,
                        header: "aabbccdd"
                    }, sig));
                });

                describe('test suites', () => {
                    fs.readdir(path.join(__dirname, 'bbs_files', suite, 'signature')).forEach(function (file) {
                        var _case = require(path.join(__dirname, 'bbs_files', suite, 'signature', file));

                        it(_case.caseName, () => {
                            var sk = crypto.createPrivateKey({
                                key: hex.decode(_case.signerKeyPair.secretKey),
                                format: "raw",
                                namedCurve: "Bls12381G2"
                            });

                            var pk = crypto.createPublicKey({
                                key: hex.decode(_case.signerKeyPair.publicKey),
                                format: "raw",
                                namedCurve: "Bls12381G2"
                            });

                            if (_case.result.valid) {
                                var json_key = sk.export({
                                    format: "jwk"
                                });
                                assert.equal(base64.decode(json_key.x).hex(), _case.signerKeyPair.publicKey);
                            }

                            var sig = crypto.bbsSign(_case.messages.map(m => hex.decode(m)), {
                                key: sk,
                                suite: suite,
                                header: hex.decode(_case.header)
                            });
                            assert.isTrue(crypto.bbsVerify(_case.messages.map(m => hex.decode(m)), {
                                key: sk,
                                suite: suite,
                                header: hex.decode(_case.header)
                            }, sig));

                            var result = crypto.bbsVerify(_case.messages.map(m => hex.decode(m)), {
                                key: pk,
                                suite: suite,
                                header: hex.decode(_case.header)
                            }, hex.decode(_case.signature));

                            assert.equal(result, _case.result.valid);
                        });
                    });
                });
            });

            describe('proof', () => {
                it("sign/verify", () => {
                    var messages = ["message1", "message2", "message3"];
                    var idx = [1, 3];
                    var revealedMessages = idx.map(i => messages[i - 1]);

                    var keys = crypto.generateKeyPair("Bls12381G2");

                    var sig = crypto.bbsSign(messages, {
                        key: keys.privateKey,
                        suite: suite
                    });

                    var proof = crypto.proofGen(sig, messages, idx, {
                        key: keys.publicKey,
                        suite: suite
                    });

                    assert.isTrue(crypto.proofVerify(revealedMessages, idx, {
                        key: keys.publicKey,
                        suite: suite
                    }, proof));
                });

                it("with proof_header", () => {
                    var messages = ["message1", "message2", "message3"];
                    var idx = [1, 3];
                    var revealedMessages = idx.map(i => messages[i - 1]);

                    var keys = crypto.generateKeyPair("Bls12381G2");

                    var sig = crypto.bbsSign(messages, {
                        key: keys.privateKey,
                        suite: suite
                    });

                    var proof = crypto.proofGen(sig, messages, idx, {
                        key: keys.publicKey,
                        suite: suite,
                        proof_header: Buffer.from("aabbccdd")
                    });

                    assert.isFalse(crypto.proofVerify(revealedMessages, idx, {
                        key: keys.publicKey,
                        suite: suite
                    }, proof));

                    assert.isTrue(crypto.proofVerify(revealedMessages, idx, {
                        key: keys.publicKey,
                        suite: suite,
                        proof_header: Buffer.from("aabbccdd")
                    }, proof));

                    assert.isTrue(crypto.proofVerify(revealedMessages, idx, {
                        key: keys.publicKey,
                        suite: suite,
                        proof_header: "aabbccdd"
                    }, proof));

                    var proof = crypto.proofGen(sig, messages, idx, {
                        key: keys.publicKey,
                        suite: suite,
                        proof_header: "aabbccdd"
                    });

                    assert.isFalse(crypto.proofVerify(revealedMessages, idx, {
                        key: keys.publicKey,
                        suite: suite
                    }, proof));

                    assert.isTrue(crypto.proofVerify(revealedMessages, idx, {
                        key: keys.publicKey,
                        suite: suite,
                        proof_header: "aabbccdd"
                    }, proof));
                });

                describe('test suites', () => {
                    fs.readdir(path.join(__dirname, 'bbs_files', suite, 'proof')).forEach(function (file) {
                        var _case = require(path.join(__dirname, 'bbs_files', suite, 'proof', file));

                        it(_case.caseName, () => {
                            var pk = crypto.createPublicKey({
                                key: hex.decode(_case.signerPublicKey),
                                format: "raw",
                                namedCurve: "Bls12381G2"
                            });

                            var result = crypto.proofVerify(Object.values(_case.revealedMessages).map(m => hex.decode(m)),
                                Object.keys(_case.revealedMessages).map(m => Number(m) + 1), {
                                key: pk,
                                suite: suite,
                                header: hex.decode(_case.header),
                                proof_header: hex.decode(_case.presentationHeader)
                            }, hex.decode(_case.proof));

                            assert.equal(result, _case.result.valid);
                        });
                    });
                });
            });
        });
    }

    add_suite("Bls12381Sha256");
    add_suite("Bls12381Shake256");
});

require.main === module && test.run(console.DEBUG);
