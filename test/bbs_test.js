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
                    var sk = crypto.generateKey("Bls12381G2");
                    sk.bbs_suite = suite;
                    var pk = sk.publicKey;

                    var messages = ["message1", "message2"];

                    var sig = sk.bbsSign(messages);
                    assert.isTrue(pk.bbsVerify(messages, sig));
                });

                it("with header", () => {
                    var sk = crypto.generateKey("Bls12381G2");
                    sk.bbs_suite = suite;
                    var pk = sk.publicKey;

                    var messages = ["message1", "message2"];

                    var sig = sk.bbsSign(messages, {
                        header: Buffer.from("aabbccdd")
                    });
                    assert.isFalse(pk.bbsVerify(messages, sig));
                    assert.isTrue(pk.bbsVerify(messages, sig, {
                        header: Buffer.from("aabbccdd")
                    }));

                    assert.isTrue(pk.bbsVerify(messages, sig, {
                        header: "aabbccdd"
                    }));

                    var sig = sk.bbsSign(messages, {
                        header: "aabbccdd"
                    });
                    assert.isFalse(pk.bbsVerify(messages, sig));
                    assert.isTrue(pk.bbsVerify(messages, sig, {
                        header: "aabbccdd"
                    }));
                });

                describe('test suites', () => {
                    fs.readdir(path.join(__dirname, 'bbs_files', suite, 'signature')).forEach(function (file) {
                        var _case = require(path.join(__dirname, 'bbs_files', suite, 'signature', file));

                        it(_case.caseName, () => {
                            var sk = crypto.PKey.from({
                                "kty": "OKP",
                                "crv": "Bls12381G2",
                                "d": hex.decode(_case.signerKeyPair.secretKey).base64()
                            });
                            sk.bbs_suite = suite;

                            var pk = crypto.PKey.from({
                                "kty": "OKP",
                                "crv": "Bls12381G2",
                                "x": hex.decode(_case.signerKeyPair.publicKey).base64()
                            });
                            pk.bbs_suite = suite;

                            if (_case.result.valid) {
                                var json_key = sk.json();
                                assert.equal(base64.decode(json_key.x).hex(), _case.signerKeyPair.publicKey);
                            }

                            var sig = sk.bbsSign(_case.messages.map(m => hex.decode(m)), {
                                header: hex.decode(_case.header)
                            });
                            assert.isTrue(sk.bbsVerify(_case.messages.map(m => hex.decode(m)), sig, {
                                header: hex.decode(_case.header)
                            }));

                            var result = pk.bbsVerify(_case.messages.map(m => hex.decode(m)), hex.decode(_case.signature), {
                                header: hex.decode(_case.header)
                            });
                            assert.equal(result, _case.result.valid);
                        });
                    });
                });
            });

            describe('proof', () => {
                it("sign/verify", () => {
                    var sk = crypto.generateKey("Bls12381G2");
                    sk.bbs_suite = suite;
                    var pk = sk.publicKey;

                    var messages = ["message1", "message2", "message3"];
                    var idx = [1, 3];
                    var revealedMessages = idx.map(i => messages[i - 1]);

                    var sig = sk.bbsSign(messages);
                    var proof = pk.proofGen(sig, messages, idx);
                    assert.isTrue(pk.proofVerify(revealedMessages, idx, proof));
                });

                it("with proof_header", () => {
                    var sk = crypto.generateKey("Bls12381G2");
                    sk.bbs_suite = suite;
                    var pk = sk.publicKey;

                    var messages = ["message1", "message2", "message3"];
                    var idx = [1, 3];
                    var revealedMessages = idx.map(i => messages[i - 1]);

                    var sig = sk.bbsSign(messages);
                    var proof = pk.proofGen(sig, messages, idx, {
                        proof_header: Buffer.from("aabbccdd")
                    });

                    assert.isFalse(pk.proofVerify(revealedMessages, idx, proof));
                    assert.isTrue(pk.proofVerify(revealedMessages, idx, proof, {
                        proof_header: Buffer.from("aabbccdd")
                    }));

                    assert.isTrue(pk.proofVerify(revealedMessages, idx, proof, {
                        proof_header: "aabbccdd"
                    }));

                    var proof = pk.proofGen(sig, messages, idx, {
                        proof_header: "aabbccdd"
                    });
                    assert.isFalse(pk.proofVerify(revealedMessages, idx, proof));
                    assert.isTrue(pk.proofVerify(revealedMessages, idx, proof, {
                        proof_header: "aabbccdd"
                    }));
                });

                describe('test suites', () => {
                    fs.readdir(path.join(__dirname, 'bbs_files', suite, 'proof')).forEach(function (file) {
                        var _case = require(path.join(__dirname, 'bbs_files', suite, 'proof', file));

                        it(_case.caseName, () => {
                            var pk = crypto.PKey.from({
                                "kty": "OKP",
                                "crv": "Bls12381G2",
                                "x": hex.decode(_case.signerPublicKey).base64()
                            });
                            pk.bbs_suite = suite;

                            var result = pk.proofVerify(Object.values(_case.revealedMessages).map(m => hex.decode(m)),
                                Object.keys(_case.revealedMessages).map(m => Number(m) + 1), hex.decode(_case.proof), {
                                header: hex.decode(_case.header),
                                proof_header: hex.decode(_case.presentationHeader)
                            });

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
