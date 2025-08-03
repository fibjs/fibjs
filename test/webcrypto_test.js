const { describe, it } = require('node:test');
const assert = require('assert');
const crypto = require('crypto');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

describe("webcrypto", () => {
    it("global.crypto should equal require('crypto').webcrypto", () => {
        assert.strictEqual(global.crypto, require('crypto').webcrypto);
    });

    describe("Basic WebCrypto", () => {
        it("getRandomValues", () => {
            const arrays = [
                'Int8Array',
                'Int16Array',
                'Int32Array',
                'BigInt64Array',
                'Uint8Array',
                'Uint8ClampedArray',
                'Uint16Array',
                'Uint32Array',
                'BigUint64Array',
            ];

            for (const array of arrays) {
                const ctor = globalThis[array];
                const result = global.crypto.getRandomValues(new ctor(8));

                // Check type
                assert.strictEqual(result.constructor, ctor);
                // Check length
                assert.strictEqual(result.length, 8);

                // Test overflow protection (65536 bytes limit)
                const maxlength = 65536 / ctor.BYTES_PER_ELEMENT;
                assert.throws(() => {
                    global.crypto.getRandomValues(new ctor(maxlength + 1));
                });

                // Test empty array
                assert.strictEqual(global.crypto.getRandomValues(new ctor(0)).length, 0);
            }
        });

        it("randomUUID", () => {
            const iterations = 256;
            const uuids = new Set();

            for (let i = 0; i < iterations; i++) {
                const uuid = global.crypto.randomUUID();
                // Check for collisions
                assert.strictEqual(uuids.has(uuid), false);
                uuids.add(uuid);

                // Validate UUID version (v4)
                const version = parseInt(uuid.split('-')[2].charAt(0), 16);
                assert.strictEqual(version, 4);

                // Validate UUID format (xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx)
                const uuidRegex = /^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i;
                assert(uuidRegex.test(uuid), `Invalid UUID format: ${uuid}`);

                // Check specific version and variant bits
                const versionBits = parseInt(uuid.split('-')[2].slice(0, 1), 16);
                assert.strictEqual(versionBits, 4); // Version 4

                const variantBits = parseInt(uuid.split('-')[3].slice(0, 1), 16);
                assert.strictEqual(variantBits & 0b1100, 0b1000); // Variant bits should be 10xx
            }
        });
    });

    describe("SubtleCrypto", () => {
        it("subtle alias", () => {
            assert.strictEqual(crypto.subtle, global.crypto.subtle);
        });

        describe("digest", () => {
            const testData = {
                empty: Buffer.alloc(0),
                short: Buffer.from([1, 2, 3, 4]),  // Use small fixed input for test vector verification
                medium: Buffer.from("Hello, WebCrypto!"),
                long: Buffer.alloc(1024).fill('A')
            };

            const algorithms = {
                'SHA-1': 20,
                'SHA-256': 32,
                'SHA-384': 48,
                'SHA-512': 64
            };

            // Test vectors calculated using Node.js crypto module for verification
            const testVectors = {
                'SHA-1': {
                    empty: [218, 57, 163, 238, 94, 107, 75, 13, 50, 85, 191, 239, 149, 96, 24, 144, 175, 216, 7, 9],
                    short: [18, 218, 218, 31, 255, 77, 71, 135, 173, 227, 51, 49, 71, 32, 44, 59, 68, 62, 55, 111],
                },
                'SHA-256': {
                    empty: [227, 176, 196, 66, 152, 252, 28, 20, 154, 251, 244, 200, 153, 111, 185, 36, 39, 174, 65, 228, 100, 155, 147, 76, 164, 149, 153, 27, 120, 82, 184, 85],
                    short: [159, 100, 167, 71, 225, 185, 127, 19, 31, 171, 182, 180, 71, 41, 108, 155, 111, 2, 1, 231, 159, 179, 197, 53, 110, 108, 119, 232, 155, 106, 128, 106],
                }
            };

            it("should digest data with all supported algorithms", async () => {
                for (const [algorithm, expectedLength] of Object.entries(algorithms)) {
                    for (const [name, data] of Object.entries(testData)) {
                        const hash = await global.crypto.subtle.digest(algorithm, data);
                        assert(hash instanceof ArrayBuffer);
                        assert(hash.byteLength > 0);

                        // Verify the hash length matches the algorithm
                        assert.strictEqual(hash.byteLength, expectedLength);

                        // Verify known test vectors
                        if (testVectors[algorithm] && testVectors[algorithm][name]) {
                            const hashArray = Array.from(new Uint8Array(hash));
                            assert.deepStrictEqual(hashArray, testVectors[algorithm][name]);
                        }
                    }
                }
            });

            it("should handle case-insensitive algorithm names", async () => {
                const data = new Uint8Array([1, 2, 3, 4]);

                const hash1 = await global.crypto.subtle.digest("SHA-256", data);
                const hash2 = await global.crypto.subtle.digest("sha-256", data);
                const hash3 = await global.crypto.subtle.digest("Sha-256", data);

                assert.deepStrictEqual(new Uint8Array(hash1), new Uint8Array(hash2));
                assert.deepStrictEqual(new Uint8Array(hash1), new Uint8Array(hash3));
            });

            it("should accept different input types", async () => {
                const data = [1, 2, 3, 4];

                // Test with Uint8Array
                const hash1 = await global.crypto.subtle.digest("SHA-256", new Uint8Array(data));

                // Test with ArrayBuffer
                const buffer = new ArrayBuffer(4);
                const view = new Uint8Array(buffer);
                view.set(data);
                const hash2 = await global.crypto.subtle.digest("SHA-256", buffer);

                // Test with Buffer
                const hash3 = await global.crypto.subtle.digest("SHA-256", Buffer.from(data));

                assert.deepStrictEqual(new Uint8Array(hash1), new Uint8Array(hash2));
                assert.deepStrictEqual(new Uint8Array(hash1), new Uint8Array(hash3));
            });
        });

        describe("ECDSA Key Operations", () => {
            const keyParams = {
                name: "ECDSA",
                namedCurve: "P-256"
            };

            it("should generate ECDSA key pair", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                assert(key.privateKey instanceof CryptoKey);
                assert(key.publicKey instanceof CryptoKey);

                // Verify key attributes
                assert.strictEqual(key.privateKey.type, "private");
                assert.strictEqual(key.publicKey.type, "public");
                assert.strictEqual(key.privateKey.algorithm.name, "ECDSA");
                assert.strictEqual(key.privateKey.algorithm.namedCurve, "P-256");
                assert.deepStrictEqual(key.privateKey.usages, ["sign"]);
                assert.deepStrictEqual(key.publicKey.usages, ["verify"]);
            });

            it("should not include 'verify' when it is not in keyUsages", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign"]
                );

                assert.deepStrictEqual(key.privateKey.usages, ["sign"]);
                assert.deepStrictEqual(key.publicKey.usages, []);
            });

            it("should set public key extractable to true by default", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    false,
                    ["sign", "verify"]
                );

                assert.strictEqual(key.privateKey.extractable, false);
                assert.strictEqual(key.publicKey.extractable, true);
            });

            it("should throw if unsupported key type", async () => {
                await assert.rejects(async () => {
                    await global.crypto.subtle.generateKey(
                        {
                            name: "UNKNOWN",
                            namedCurve: "P-256"
                        },
                        true,
                        ["sign", "verify"]
                    );
                });
            });

            it("should throw if unsupported curve", async () => {
                await assert.rejects(async () => {
                    await global.crypto.subtle.generateKey(
                        {
                            name: "ECDSA",
                            namedCurve: "UNKNOWN"
                        },
                        true,
                        ["sign", "verify"]
                    );
                });
            });

            it("should throw if unsupported key usage", async () => {
                await assert.rejects(async () => {
                    await global.crypto.subtle.generateKey(
                        keyParams,
                        true,
                        ["sign", "verify", "encrypt"]
                    );
                });
            });

            it("should throw if 'sign' is not included in keyUsages", async () => {
                await assert.rejects(async () => {
                    await global.crypto.subtle.generateKey(
                        keyParams,
                        true,
                        ["verify"]
                    );
                });
            });

            it("should accept Uint8Array as keydata", async () => {
                const spki = new Uint8Array([
                    48, 57, 48, 19, 6, 7, 42, 134, 72, 206, 61, 2, 1, 6, 8, 42, 134, 72, 206, 61, 3, 1, 7,
                    3, 34, 0, 3, 177, 130, 208, 28, 236, 189, 76, 208, 22, 192, 87, 0, 150, 156, 100, 169,
                    16, 57, 58, 235, 105, 88, 42, 213, 60, 69, 172, 129, 233, 98, 252, 160
                ]);

                const publicKey = await global.crypto.subtle.importKey(
                    'spki',
                    spki,
                    {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    },
                    true,
                    ["verify"]
                );

                assert(publicKey instanceof CryptoKey);
                assert.strictEqual(publicKey.type, "public");
            });

            it("should import/export keys in different formats", async () => {
                // Generate a key pair first
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                // Test JWK format
                const jwkPrivate = await global.crypto.subtle.exportKey("jwk", key.privateKey);
                const jwkPublic = await global.crypto.subtle.exportKey("jwk", key.publicKey);

                assert.strictEqual(typeof jwkPrivate, "object");
                assert.strictEqual(typeof jwkPublic, "object");
                assert.strictEqual(jwkPrivate.kty, "EC");
                assert.strictEqual(jwkPublic.kty, "EC");

                // Re-import the exported keys
                const importedPrivate = await global.crypto.subtle.importKey(
                    "jwk",
                    jwkPrivate,
                    keyParams,
                    true,
                    ["sign"]
                );

                const importedPublic = await global.crypto.subtle.importKey(
                    "jwk",
                    jwkPublic,
                    keyParams,
                    true,
                    ["verify"]
                );

                assert(importedPrivate instanceof CryptoKey);
                assert(importedPublic instanceof CryptoKey);

                // Test PKCS8 format
                const pkcs8Key = await global.crypto.subtle.exportKey("pkcs8", key.privateKey);
                assert(pkcs8Key instanceof ArrayBuffer);
                const importedPkcs8Key = await global.crypto.subtle.importKey(
                    "pkcs8",
                    pkcs8Key,
                    keyParams,
                    true,
                    ["sign"]
                );
                assert(importedPkcs8Key instanceof CryptoKey);

                // Test SPKI format
                const spkiKey = await global.crypto.subtle.exportKey("spki", key.publicKey);
                assert(spkiKey instanceof ArrayBuffer);
                const importedSpkiKey = await global.crypto.subtle.importKey(
                    "spki",
                    spkiKey,
                    keyParams,
                    true,
                    ["verify"]
                );
                assert(importedSpkiKey instanceof CryptoKey);

                // Test Raw format
                const rawKey = await global.crypto.subtle.exportKey("raw", key.publicKey);
                assert(rawKey instanceof ArrayBuffer);
                const importedRawKey = await global.crypto.subtle.importKey(
                    "raw",
                    rawKey,
                    {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    },
                    true,
                    ["verify"]
                );
                assert(importedRawKey instanceof CryptoKey);
            });

            it("should throw if algorithm name is not matching", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                const jwkPublic = await global.crypto.subtle.exportKey("jwk", key.publicKey);

                await assert.rejects(async () => {
                    await global.crypto.subtle.importKey("jwk", jwkPublic, {
                        name: "RSA-PSS",
                        namedCurve: "P-256"
                    }, true, ["verify"]);
                });
            });

            it("should throw if namedCurve is not matching", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                const jwkPublic = await global.crypto.subtle.exportKey("jwk", key.publicKey);

                await assert.rejects(async () => {
                    await global.crypto.subtle.importKey("jwk", jwkPublic, {
                        name: "ECDSA",
                        namedCurve: "P-384"
                    }, true, ["verify"]);
                });
            });

            it("should throw if key type and usage mismatch", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                const jwkPublic = await global.crypto.subtle.exportKey("jwk", key.publicKey);
                const jwkPrivate = await global.crypto.subtle.exportKey("jwk", key.privateKey);

                // Public key cannot be used for signing
                await assert.rejects(async () => {
                    await global.crypto.subtle.importKey("jwk", jwkPublic, keyParams, true, ["sign"]);
                });

                // Private key cannot be used for verification
                await assert.rejects(async () => {
                    await global.crypto.subtle.importKey("jwk", jwkPrivate, keyParams, true, ["verify"]);
                });
            });

            it("should sign and verify data", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                const data = new Uint8Array([1, 2, 3, 4]);
                const signature = await global.crypto.subtle.sign(
                    {
                        name: "ECDSA",
                        hash: "SHA-256"
                    },
                    key.privateKey,
                    data
                );

                const isValid = await global.crypto.subtle.verify(
                    {
                        name: "ECDSA",
                        hash: "SHA-256"
                    },
                    key.publicKey,
                    signature,
                    data
                );

                assert.strictEqual(isValid, true);
            });

            it("should support hash parameter as object", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                const data = new Uint8Array([1, 2, 3, 4]);
                const signature = await global.crypto.subtle.sign(
                    {
                        name: "ECDSA",
                        hash: {
                            name: "SHA-256"
                        }
                    },
                    key.privateKey,
                    data
                );

                const isValid = await global.crypto.subtle.verify(
                    {
                        name: "ECDSA",
                        hash: {
                            name: "SHA-256"
                        }
                    },
                    key.publicKey,
                    signature,
                    data
                );

                assert.strictEqual(isValid, true);
            });

            it("should verify signature with node crypto module", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                const jwkPublic = await global.crypto.subtle.exportKey("jwk", key.publicKey);
                const jwkPrivate = await global.crypto.subtle.exportKey("jwk", key.privateKey);

                const data = new Uint8Array([1, 2, 3, 4]);

                // Use WebCrypto to sign
                const signature = await global.crypto.subtle.sign(
                    {
                        name: "ECDSA",
                        hash: "SHA-256"
                    },
                    key.privateKey,
                    data
                );

                // Verify with Node.js crypto
                const verified = crypto.verify(
                    "sha256",
                    data,
                    {
                        format: 'jwk',
                        key: jwkPublic,
                        dsaEncoding: 'ieee-p1363'
                    },
                    Buffer.from(signature)
                );

                assert.strictEqual(verified, true);
            });

            it("should verify Node.js signature with WebCrypto", async () => {
                const key = await global.crypto.subtle.generateKey(
                    keyParams,
                    true,
                    ["sign", "verify"]
                );

                const jwkPublic = await global.crypto.subtle.exportKey("jwk", key.publicKey);
                const jwkPrivate = await global.crypto.subtle.exportKey("jwk", key.privateKey);

                const data = new Uint8Array([1, 2, 3, 4]);

                // Use Node.js crypto to sign
                const signature = crypto.sign("sha256", data, {
                    format: 'jwk',
                    key: jwkPrivate,
                    dsaEncoding: 'ieee-p1363'
                });

                // Verify with WebCrypto
                const isValid = await global.crypto.subtle.verify(
                    {
                        name: "ECDSA",
                        hash: "SHA-256"
                    },
                    key.publicKey,
                    signature,
                    data
                );

                assert.strictEqual(isValid, true);
            });
        });

        describe("Ed25519 Key Operations", () => {
            // Test vectors from Node.js test fixtures
            const testVectors = {
                Ed25519: {
                    pkcs8: Buffer.from(
                        '302e020100300506032b657004220420f3c8f4c48df878146e8cd3bf6df4e50e389b' +
                        'a7074e15c2352dcd5d308d4ca81f', 'hex'),
                    spki: Buffer.from(
                        '302a300506032b6570032100d8e18963d809d487d9549accaec6742e7eeba24d8a0d' +
                        '3b14b7e3caea06893dcc', 'hex'),
                    data: Buffer.from(
                        '2b7ed0bc7795694ab4acd35903fe8cd7d80f6a1c8688a6c3414409457514a1457855bb' +
                        'b219e30a1beea8fe869082d99fc8282f9050d024e59eaf0730ba9db70a', 'hex'),
                    signature: Buffer.from(
                        '3d90de5e5743dfc28225bfadb341b116cbf8a3f1ceedbf4adc350ef5d3471843a418' +
                        '614dcb6e614862614cf7af1496f9340b3c844ea4dceab1d3d155eb7ecc00', 'hex')
                }
            };

            it("should generate Ed25519 key pair", async () => {
                const key = await global.crypto.subtle.generateKey(
                    {
                        name: "Ed25519"
                    },
                    true,
                    ["sign", "verify"]
                );

                assert(key.privateKey instanceof CryptoKey);
                assert(key.publicKey instanceof CryptoKey);

                // Verify key attributes
                assert.strictEqual(key.privateKey.type, "private");
                assert.strictEqual(key.publicKey.type, "public");
                assert.strictEqual(key.privateKey.algorithm.name, "Ed25519");
                assert.deepStrictEqual(key.privateKey.usages, ["sign"]);
                assert.deepStrictEqual(key.publicKey.usages, ["verify"]);
            });

            it("should import Ed25519 keys from test vectors", async () => {
                const { Ed25519 } = testVectors;

                const publicKey = await global.crypto.subtle.importKey(
                    'spki',
                    Ed25519.spki,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                const privateKey = await global.crypto.subtle.importKey(
                    'pkcs8',
                    Ed25519.pkcs8,
                    { name: "Ed25519" },
                    true,
                    ["sign"]
                );

                assert(publicKey instanceof CryptoKey);
                assert(privateKey instanceof CryptoKey);
                assert.strictEqual(publicKey.type, "public");
                assert.strictEqual(privateKey.type, "private");
                assert.strictEqual(publicKey.algorithm.name, "Ed25519");
                assert.strictEqual(privateKey.algorithm.name, "Ed25519");
            });

            it("should export and re-import Ed25519 keys", async () => {
                const { Ed25519 } = testVectors;

                // Import original keys
                const originalPublicKey = await global.crypto.subtle.importKey(
                    'spki',
                    Ed25519.spki,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                const originalPrivateKey = await global.crypto.subtle.importKey(
                    'pkcs8',
                    Ed25519.pkcs8,
                    { name: "Ed25519" },
                    true,
                    ["sign"]
                );

                // Export keys in different formats
                const spkiExported = await global.crypto.subtle.exportKey("spki", originalPublicKey);
                const pkcs8Exported = await global.crypto.subtle.exportKey("pkcs8", originalPrivateKey);
                const jwkPublicExported = await global.crypto.subtle.exportKey("jwk", originalPublicKey);
                const jwkPrivateExported = await global.crypto.subtle.exportKey("jwk", originalPrivateKey);

                // Verify SPKI and PKCS8 exports match originals
                assert.deepStrictEqual(new Uint8Array(spkiExported), new Uint8Array(Ed25519.spki));
                assert.deepStrictEqual(new Uint8Array(pkcs8Exported), new Uint8Array(Ed25519.pkcs8));

                // Re-import keys
                const reimportedPublicKey = await global.crypto.subtle.importKey(
                    'jwk',
                    jwkPublicExported,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                const reimportedPrivateKey = await global.crypto.subtle.importKey(
                    'jwk',
                    jwkPrivateExported,
                    { name: "Ed25519" },
                    true,
                    ["sign"]
                );

                assert(reimportedPublicKey instanceof CryptoKey);
                assert(reimportedPrivateKey instanceof CryptoKey);
            });

            it("should sign and verify with Ed25519", async () => {
                const { Ed25519 } = testVectors;

                const publicKey = await global.crypto.subtle.importKey(
                    'spki',
                    Ed25519.spki,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                const privateKey = await global.crypto.subtle.importKey(
                    'pkcs8',
                    Ed25519.pkcs8,
                    { name: "Ed25519" },
                    true,
                    ["sign"]
                );

                // Test signing
                const signature = await global.crypto.subtle.sign(
                    { name: "Ed25519" },
                    privateKey,
                    Ed25519.data
                );

                // Test verification
                const isValid = await global.crypto.subtle.verify(
                    { name: "Ed25519" },
                    publicKey,
                    signature,
                    Ed25519.data
                );

                assert.strictEqual(isValid, true);
            });

            it("should verify test vector signature", async () => {
                const { Ed25519 } = testVectors;

                const publicKey = await global.crypto.subtle.importKey(
                    'spki',
                    Ed25519.spki,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                // Verify known signature from test vectors
                const isValid = await global.crypto.subtle.verify(
                    { name: "Ed25519" },
                    publicKey,
                    Ed25519.signature,
                    Ed25519.data
                );

                assert.strictEqual(isValid, true);
            });

            it("should fail verification with wrong data", async () => {
                const { Ed25519 } = testVectors;

                const publicKey = await global.crypto.subtle.importKey(
                    'spki',
                    Ed25519.spki,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                // Alter the data
                const wrongData = Buffer.from(Ed25519.data);
                wrongData[0] = 255 - wrongData[0];

                const isValid = await global.crypto.subtle.verify(
                    { name: "Ed25519" },
                    publicKey,
                    Ed25519.signature,
                    wrongData
                );

                assert.strictEqual(isValid, false);
            });

            it("should fail verification with wrong signature", async () => {
                const { Ed25519 } = testVectors;

                const publicKey = await global.crypto.subtle.importKey(
                    'spki',
                    Ed25519.spki,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                // Alter the signature
                const wrongSignature = Buffer.from(Ed25519.signature);
                wrongSignature[0] = 255 - wrongSignature[0];

                const isValid = await global.crypto.subtle.verify(
                    { name: "Ed25519" },
                    publicKey,
                    wrongSignature,
                    Ed25519.data
                );

                assert.strictEqual(isValid, false);
            });

            it("should throw with wrong key usage", async () => {
                const { Ed25519 } = testVectors;

                const publicKey = await global.crypto.subtle.importKey(
                    'spki',
                    Ed25519.spki,
                    { name: "Ed25519" },
                    true,
                    ["verify"]
                );

                const privateKey = await global.crypto.subtle.importKey(
                    'pkcs8',
                    Ed25519.pkcs8,
                    { name: "Ed25519" },
                    true,
                    ["sign"]
                );

                // Try to sign with public key
                await assert.rejects(async () => {
                    await global.crypto.subtle.sign(
                        { name: "Ed25519" },
                        publicKey,
                        Ed25519.data
                    );
                });

                // Try to verify with private key
                await assert.rejects(async () => {
                    await global.crypto.subtle.verify(
                        { name: "Ed25519" },
                        privateKey,
                        Ed25519.signature,
                        Ed25519.data
                    );
                });
            });

            it("should throw with unsupported algorithm", async () => {
                await assert.rejects(async () => {
                    await global.crypto.subtle.generateKey(
                        {
                            name: "Ed25520" // Intentionally typo to test error handling
                        },
                        true,
                        ["sign", "verify"]
                    );
                });
            });
        });

        describe("String Algorithm Parameter Compatibility", () => {
            it("should support string algorithm in generateKey", async () => {
                const key = await global.crypto.subtle.generateKey(
                    "Ed25519",  // String format
                    true,
                    ["sign", "verify"]
                );

                assert(key.privateKey instanceof CryptoKey);
                assert(key.publicKey instanceof CryptoKey);
                assert.strictEqual(key.privateKey.algorithm.name, "Ed25519");
                assert.strictEqual(key.publicKey.algorithm.name, "Ed25519");
            });

            it("should support string algorithm in sign/verify", async () => {
                const key = await global.crypto.subtle.generateKey(
                    { name: "Ed25519" },
                    true,
                    ["sign", "verify"]
                );

                const data = new Uint8Array([1, 2, 3, 4]);
                
                // Sign with string algorithm
                const signature = await global.crypto.subtle.sign(
                    "Ed25519",  // String format
                    key.privateKey,
                    data
                );

                // Verify with string algorithm
                const isValid = await global.crypto.subtle.verify(
                    "Ed25519",  // String format
                    key.publicKey,
                    signature,
                    data
                );

                assert.strictEqual(isValid, true);
            });

            it("should support string algorithm in importKey", async () => {
                const key = await global.crypto.subtle.generateKey(
                    { name: "Ed25519" },
                    true,
                    ["sign", "verify"]
                );

                const rawKey = await global.crypto.subtle.exportKey("raw", key.publicKey);

                const importedKey = await global.crypto.subtle.importKey(
                    "raw",
                    rawKey,
                    "Ed25519",  // String format
                    true,
                    ["verify"]
                );

                assert(importedKey instanceof CryptoKey);
                assert.strictEqual(importedKey.algorithm.name, "Ed25519");
                assert.strictEqual(importedKey.type, "public");
            });

            it("should support mixed string/object formats", async () => {
                // Generate key with string
                const key1 = await global.crypto.subtle.generateKey(
                    "Ed25519",
                    true,
                    ["sign", "verify"]
                );

                // Generate key with object
                const key2 = await global.crypto.subtle.generateKey(
                    { name: "Ed25519" },
                    true,
                    ["sign", "verify"]
                );

                const data = new Uint8Array([1, 2, 3, 4]);

                // Sign with string, verify with object
                const sig1 = await global.crypto.subtle.sign("Ed25519", key1.privateKey, data);
                const valid1 = await global.crypto.subtle.verify({ name: "Ed25519" }, key1.publicKey, sig1, data);

                // Sign with object, verify with string
                const sig2 = await global.crypto.subtle.sign({ name: "Ed25519" }, key2.privateKey, data);
                const valid2 = await global.crypto.subtle.verify("Ed25519", key2.publicKey, sig2, data);

                assert.strictEqual(valid1, true);
                assert.strictEqual(valid2, true);
            });
        });
    });

    // fibjs-specific feature tests - only run in fibjs environment
    if (isFibjs) {
        describe("fibjs Enhanced WebCrypto Features", () => {
            // Pre-generated test key pair for cross-module compatibility testing
            const testKeys = crypto.generateKeyPairSync("ec", {
                namedCurve: "P-256",
                publicKeyEncoding: {
                    format: "jwk"
                },
                privateKeyEncoding: {
                    format: "jwk"
                }
            });

            describe("Enhanced JWK Compatibility", () => {
                it("should support exact JWK import/export compatibility", async () => {
                    const importedPublicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["verify"]);

                    const importedPrivateKey = await global.crypto.subtle.importKey("jwk", testKeys.privateKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["sign"]);

                    const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);
                    const exportedPrivateKey = await global.crypto.subtle.exportKey("jwk", importedPrivateKey);

                    // In fibjs, expect exact JWK match
                    assert.deepStrictEqual(exportedPublicKey, testKeys.publicKey);
                    assert.deepStrictEqual(exportedPrivateKey, testKeys.privateKey);
                });
            });

            describe("Enhanced Raw Format Support", () => {
                it("should export raw format compatible with Node.js crypto", async () => {
                    const importedPublicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["verify"]);

                    const exportedPublicKey = await global.crypto.subtle.exportKey("raw", importedPublicKey);

                    // In fibjs, we can directly compare with Node.js crypto raw export
                    const pk = crypto.createPublicKey({
                        key: testKeys.publicKey
                    });

                    const nodeRawKey = pk.export({ format: "raw" });
                    assert.deepStrictEqual(new Uint8Array(exportedPublicKey), new Uint8Array(nodeRawKey));
                });

                it("should import raw format from Node.js crypto", async () => {
                    const pk = crypto.createPublicKey({
                        key: testKeys.publicKey,
                        format: 'jwk'
                    });

                    // In fibjs, Node.js crypto supports raw export directly
                    const rawKey = pk.export({ format: "raw" });

                    const importedPublicKey = await global.crypto.subtle.importKey("raw", rawKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["verify"]);

                    const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);

                    // Compare key components
                    assert.strictEqual(exportedPublicKey.kty, testKeys.publicKey.kty);
                    assert.strictEqual(exportedPublicKey.crv, testKeys.publicKey.crv);
                    assert.strictEqual(exportedPublicKey.x, testKeys.publicKey.x);
                    assert.strictEqual(exportedPublicKey.y, testKeys.publicKey.y);
                });
            });

            describe("Enhanced Algorithm Support", () => {
                it("should handle case-insensitive algorithm names in sign/verify", async () => {
                    const privateKey = await global.crypto.subtle.importKey("jwk", testKeys.privateKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["sign"]);

                    const publicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["verify"]);

                    const data = new Uint8Array([1, 2, 3, 4]);

                    // Test with lowercase algorithm name
                    const signature = await global.crypto.subtle.sign({
                        name: "ecdsa",  // lowercase
                        hash: "SHA-256"
                    }, privateKey, data);

                    const verified = await global.crypto.subtle.verify({
                        name: "ecdsa",  // lowercase
                        hash: "SHA-256"
                    }, publicKey, signature, data);

                    assert.strictEqual(verified, true);
                });
            });
        });
    }    // Pre-generated test key pair for cross-module compatibility testing
    const testKeys = crypto.generateKeyPairSync("ec", {
        namedCurve: "P-256",
        publicKeyEncoding: {
            format: "jwk"
        },
        privateKeyEncoding: {
            format: "jwk"
        }
    });

    describe("Cross-module Key Import/Export Tests", () => {
        it("should import/export JWK keys maintaining compatibility", async () => {
            const importedPublicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["verify"]);

            const importedPrivateKey = await global.crypto.subtle.importKey("jwk", testKeys.privateKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["sign"]);

            const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);
            const exportedPrivateKey = await global.crypto.subtle.exportKey("jwk", importedPrivateKey);

            // Compare key components, ignoring extra WebCrypto properties
            assert.strictEqual(exportedPublicKey.kty, testKeys.publicKey.kty);
            assert.strictEqual(exportedPublicKey.crv, testKeys.publicKey.crv);
            assert.strictEqual(exportedPublicKey.x, testKeys.publicKey.x);
            assert.strictEqual(exportedPublicKey.y, testKeys.publicKey.y);

            assert.strictEqual(exportedPrivateKey.kty, testKeys.privateKey.kty);
            assert.strictEqual(exportedPrivateKey.crv, testKeys.privateKey.crv);
            assert.strictEqual(exportedPrivateKey.x, testKeys.privateKey.x);
            assert.strictEqual(exportedPrivateKey.y, testKeys.privateKey.y);
            assert.strictEqual(exportedPrivateKey.d, testKeys.privateKey.d);
        });



        it("should export PKCS8 format", async () => {
            const importedPrivateKey = await global.crypto.subtle.importKey("jwk", testKeys.privateKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["sign"]);

            const exportedPrivateKey = await global.crypto.subtle.exportKey("pkcs8", importedPrivateKey);

            const sk = crypto.createPrivateKey({
                key: testKeys.privateKey,
                format: 'jwk'
            });

            const referenceKey = sk.export({ format: "der", type: "pkcs8" });

            assert.deepStrictEqual(new Uint8Array(exportedPrivateKey), new Uint8Array(referenceKey));
        });

        it("should import PKCS8 format", async () => {
            const sk = crypto.createPrivateKey({
                key: testKeys.privateKey,
                format: 'jwk'
            });

            const pkcs8Data = sk.export({ format: "der", type: "pkcs8" });

            const importedPrivateKey = await global.crypto.subtle.importKey("pkcs8", pkcs8Data, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["sign"]);

            const exportedPrivateKey = await global.crypto.subtle.exportKey("jwk", importedPrivateKey);

            // Compare key components
            assert.strictEqual(exportedPrivateKey.kty, testKeys.privateKey.kty);
            assert.strictEqual(exportedPrivateKey.crv, testKeys.privateKey.crv);
            assert.strictEqual(exportedPrivateKey.d, testKeys.privateKey.d);
        });

        it("should export SPKI format", async () => {
            const importedPublicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["verify"]);

            const exportedPublicKey = await global.crypto.subtle.exportKey("spki", importedPublicKey);

            const pk = crypto.createPublicKey({
                key: testKeys.publicKey,
                format: 'jwk'
            });

            const referenceKey = pk.export({ format: "der", type: "spki" });

            assert.deepStrictEqual(new Uint8Array(exportedPublicKey), new Uint8Array(referenceKey));
        });

        it("should import SPKI format", async () => {
            const pk = crypto.createPublicKey({
                key: testKeys.publicKey,
                format: 'jwk'
            });

            const spkiData = pk.export({ format: "der", type: "spki" });

            const importedPublicKey = await global.crypto.subtle.importKey("spki", spkiData, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["verify"]);

            const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);

            // Compare key components
            assert.strictEqual(exportedPublicKey.kty, testKeys.publicKey.kty);
            assert.strictEqual(exportedPublicKey.crv, testKeys.publicKey.crv);
            assert.strictEqual(exportedPublicKey.x, testKeys.publicKey.x);
            assert.strictEqual(exportedPublicKey.y, testKeys.publicKey.y);
        });

        it("should export raw format", async () => {
            const importedPublicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["verify"]);

            const exportedPublicKey = await global.crypto.subtle.exportKey("raw", importedPublicKey);

            // Create reference raw key from SPKI format
            const pk = crypto.createPublicKey({
                key: testKeys.publicKey,
                format: 'jwk'
            });

            // Export as SPKI first, then extract raw bytes
            const spki = pk.export({ format: "der", type: "spki" });

            // For P-256, the raw public key is the last 65 bytes of SPKI (1 + 32 + 32)
            // The raw format is: 0x04 + x-coordinate (32 bytes) + y-coordinate (32 bytes)
            const referenceRawKey = spki.slice(-65);

            assert.deepStrictEqual(new Uint8Array(exportedPublicKey), new Uint8Array(referenceRawKey));
        });



        it("should import raw format", async () => {
            const pk = crypto.createPublicKey({
                key: testKeys.publicKey,
                format: 'jwk'
            });

            // Export as SPKI first, then extract raw bytes (uncompressed point format)
            const spkiData = pk.export({ format: "der", type: "spki" });

            // For P-256, the raw key is the last 65 bytes of SPKI (1 byte + 32 bytes x + 32 bytes y)
            const rawKey = spkiData.slice(-65);

            const importedPublicKey = await global.crypto.subtle.importKey("raw", rawKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["verify"]);

            const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);

            // Compare key components
            assert.strictEqual(exportedPublicKey.kty, testKeys.publicKey.kty);
            assert.strictEqual(exportedPublicKey.crv, testKeys.publicKey.crv);
            assert.strictEqual(exportedPublicKey.x, testKeys.publicKey.x);
            assert.strictEqual(exportedPublicKey.y, testKeys.publicKey.y);
        });


    });

    describe("Standalone Sign/Verify Tests", () => {
        it("should sign data and verify with Node.js crypto", async () => {
            const privateKey = await global.crypto.subtle.importKey("jwk", testKeys.privateKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["sign"]);

            const data = new Uint8Array([1, 2, 3, 4]);
            const signature = await global.crypto.subtle.sign({
                name: "ECDSA",
                hash: "SHA-256"
            }, privateKey, data);

            const verified = crypto.verify("sha256", data, {
                format: 'jwk',
                key: testKeys.publicKey,
                dsaEncoding: 'ieee-p1363'
            }, Buffer.from(signature));

            assert.strictEqual(verified, true);
        });

        it("should verify Node.js crypto signatures", async () => {
            const publicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["verify"]);

            const data = new Uint8Array([1, 2, 3, 4]);
            const signature = crypto.sign("sha256", data, {
                format: 'jwk',
                key: testKeys.privateKey,
                dsaEncoding: 'ieee-p1363'
            });

            const verified = await global.crypto.subtle.verify({
                name: "ECDSA",
                hash: "SHA-256"
            }, publicKey, signature, data);

            assert.strictEqual(verified, true);
        });

        it("should support hash parameter as object in standalone tests", async () => {
            const publicKey = await global.crypto.subtle.importKey("jwk", testKeys.publicKey, {
                name: "ECDSA",
                namedCurve: "P-256"
            }, true, ["verify"]);

            const data = new Uint8Array([1, 2, 3, 4]);
            const signature = crypto.sign("sha256", data, {
                format: 'jwk',
                key: testKeys.privateKey,
                dsaEncoding: 'ieee-p1363'
            });

            const verified = await global.crypto.subtle.verify({
                name: "ECDSA",
                hash: {
                    name: "SHA-256"
                }
            }, publicKey, signature, data);

            assert.strictEqual(verified, true);
        });
    });


});
