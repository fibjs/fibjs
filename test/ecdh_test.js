const { describe, it } = require('node:test');
const assert = require('assert');
const crypto = require('crypto');

describe('ECDH', () => {
    // Test createECDH basic functionality
    describe('createECDH', () => {
        it('should throw when curve argument is undefined', () => {
            assert.throws(
                () => crypto.createECDH(),
                /Parameter not optional|argument must be of type string/
            );
        });

        it('should throw for invalid curve name', () => {
            assert.throws(
                () => crypto.createECDH('badcurve'),
                /Invalid EC curve name/
            );
        });

        it('should create ECDH instance for valid curves', () => {
            const testCurves = ['prime256v1', 'secp256k1', 'secp384r1', 'secp521r1', 'SM2'];

            for (const curve of testCurves) {
                const ecdh = crypto.createECDH(curve);
                assert(ecdh);
                assert.strictEqual(typeof ecdh.generateKeys, 'function');
                assert.strictEqual(typeof ecdh.computeSecret, 'function');
                assert.strictEqual(typeof ecdh.getPrivateKey, 'function');
                assert.strictEqual(typeof ecdh.getPublicKey, 'function');
                assert.strictEqual(typeof ecdh.setPrivateKey, 'function');
                assert.strictEqual(typeof ecdh.setPublicKey, 'function');
            }
        });
    });

    // Test key generation and retrieval
    describe('Key Generation and Retrieval', () => {
        it('should generate keys successfully', () => {
            const testCurves = ['prime256v1', 'secp256k1', 'SM2'];

            for (const curve of testCurves) {
                const ecdh = crypto.createECDH(curve);
                const publicKey = ecdh.generateKeys();

                assert(Buffer.isBuffer(publicKey));
                assert(publicKey.length > 0);

                // Should be able to get keys after generation
                const privateKey = ecdh.getPrivateKey();
                const publicKeyAgain = ecdh.getPublicKey();

                assert(Buffer.isBuffer(privateKey));
                assert(Buffer.isBuffer(publicKeyAgain));
                assert(privateKey.length > 0);
                assert(publicKeyAgain.length > 0);
                assert(publicKey.equals(publicKeyAgain));
            }
        });

        it('should generate keys with different encodings', () => {
            const ecdh = crypto.createECDH('prime256v1');

            // Generate with hex encoding
            const hexKey = ecdh.generateKeys('hex');
            assert.strictEqual(typeof hexKey, 'string');
            assert(hexKey.length > 0);
            assert(/^[0-9a-f]+$/i.test(hexKey));

            // Generate with base64 encoding  
            const ecdh2 = crypto.createECDH('prime256v1');
            const base64Key = ecdh2.generateKeys('base64');
            assert.strictEqual(typeof base64Key, 'string');
            assert(base64Key.length > 0);

            // Generate with binary encoding
            const ecdh3 = crypto.createECDH('prime256v1');
            const binaryKey = ecdh3.generateKeys('binary');
            assert.strictEqual(typeof binaryKey, 'string');
            assert(binaryKey.length > 0);

            // Generate with buffer (default)
            const ecdh4 = crypto.createECDH('prime256v1');
            const bufferKey = ecdh4.generateKeys();
            assert(Buffer.isBuffer(bufferKey));
            assert(bufferKey.length > 0);
        });

        it('should generate keys with different point formats', () => {
            const ecdh1 = crypto.createECDH('prime256v1');
            const ecdh2 = crypto.createECDH('prime256v1');
            const ecdh3 = crypto.createECDH('prime256v1');

            // Test default format (uncompressed)
            const uncompressed = ecdh1.generateKeys();
            assert(Buffer.isBuffer(uncompressed));
            assert.strictEqual(uncompressed[0], 4); // Uncompressed format starts with 0x04

            // Test compressed format
            const compressed = ecdh2.generateKeys('buffer', 'compressed');
            assert(Buffer.isBuffer(compressed));
            const firstByte = compressed[0];
            assert(firstByte === 2 || firstByte === 3); // Compressed format starts with 0x02 or 0x03

            // Test hybrid format
            const hybrid = ecdh3.generateKeys('buffer', 'hybrid');
            assert(Buffer.isBuffer(hybrid));
            const hybridFirstByte = hybrid[0];
            assert(hybridFirstByte === 6 || hybridFirstByte === 7); // Hybrid format starts with 0x06 or 0x07

            // Test with hex encoding and different formats
            const ecdh4 = crypto.createECDH('secp256k1');
            const hexUncompressed = ecdh4.generateKeys('hex', 'uncompressed');
            assert.strictEqual(typeof hexUncompressed, 'string');
            assert.strictEqual(hexUncompressed.substring(0, 2), '04');

            const ecdh5 = crypto.createECDH('secp256k1');
            const hexCompressed = ecdh5.generateKeys('hex', 'compressed');
            assert.strictEqual(typeof hexCompressed, 'string');
            assert(hexCompressed.substring(0, 2) === '02' || hexCompressed.substring(0, 2) === '03');
        });

        it('should generate SM2 keys correctly', () => {
            const ecdh = crypto.createECDH('SM2');

            // Test key generation
            const publicKey = ecdh.generateKeys();
            assert(Buffer.isBuffer(publicKey));
            assert.strictEqual(publicKey.length, 65); // SM2 uncompressed public key length

            const privateKey = ecdh.getPrivateKey();
            assert(Buffer.isBuffer(privateKey));
            assert.strictEqual(privateKey.length, 32); // SM2 private key length

            // Test different point formats
            const compressed = ecdh.getPublicKey('hex', 'compressed');
            const uncompressed = ecdh.getPublicKey('hex', 'uncompressed');
            const hybrid = ecdh.getPublicKey('hex', 'hybrid');

            assert.strictEqual(compressed.length, 66); // 33 bytes * 2 hex chars
            assert.strictEqual(uncompressed.length, 130); // 65 bytes * 2 hex chars
            assert.strictEqual(hybrid.length, 130); // 65 bytes * 2 hex chars

            // Check format markers
            assert.strictEqual(uncompressed.substring(0, 2), '04');
            assert(compressed.substring(0, 2) === '02' || compressed.substring(0, 2) === '03');
            assert(hybrid.substring(0, 2) === '06' || hybrid.substring(0, 2) === '07');
        });

        it('should throw when getting keys from uninitialized ECDH', () => {
            const ecdh = crypto.createECDH('secp256k1');

            assert.throws(() => {
                ecdh.getPublicKey();
            }, /^Error: Failed to get ECDH public key$/);

            assert.throws(() => {
                ecdh.getPrivateKey();
            }, /^Error: Failed to get ECDH private key$/);
        });

        it('should throw for invalid format parameter in generateKeys', () => {
            const ecdh = crypto.createECDH('prime256v1');

            assert.throws(
                () => ecdh.generateKeys('buffer', 'invalid'),
                /Invalid ECDH format/
            );

            assert.throws(
                () => ecdh.generateKeys('hex', 10),
                /Invalid ECDH format/
            );
        });
    });

    // Test key setting and format conversion
    describe('Key Setting and Format Conversion', () => {
        it('should set and get private key correctly', () => {
            const ecdh = crypto.createECDH('secp256k1');

            // A valid private key for secp256k1 curve
            const cafebabeKey = 'cafebabe'.repeat(8);
            const cafebabePubPtUnComp =
                '04672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3' +
                '2e02c7f93d13dc2732b760ca377a5897b9dd41a1c1b29dc0442fdce6d0a04d1d';

            ecdh.setPrivateKey(cafebabeKey, 'hex');
            assert.strictEqual(ecdh.getPrivateKey('hex'), cafebabeKey);

            // Public key should be generated automatically
            assert.strictEqual(ecdh.getPublicKey('hex'), cafebabePubPtUnComp);
        });

        it('should get private and public keys in different encodings', () => {
            const ecdh = crypto.createECDH('prime256v1');
            ecdh.generateKeys();

            // Test getting private key in different encodings
            const privateKeyBuffer = ecdh.getPrivateKey();
            const privateKeyHex = ecdh.getPrivateKey('hex');
            const privateKeyBase64 = ecdh.getPrivateKey('base64');
            const privateKeyBinary = ecdh.getPrivateKey('binary');

            assert(Buffer.isBuffer(privateKeyBuffer));
            assert.strictEqual(typeof privateKeyHex, 'string');
            assert.strictEqual(typeof privateKeyBase64, 'string');
            assert.strictEqual(typeof privateKeyBinary, 'string');

            // All should represent the same key
            assert.strictEqual(privateKeyBuffer.toString('hex'), privateKeyHex);
            assert.strictEqual(privateKeyBuffer.toString('base64'), privateKeyBase64);
            assert.strictEqual(privateKeyBuffer.toString('binary'), privateKeyBinary);

            // Test getting public key in different encodings
            const publicKeyBuffer = ecdh.getPublicKey();
            const publicKeyHex = ecdh.getPublicKey('hex');
            const publicKeyBase64 = ecdh.getPublicKey('base64');
            const publicKeyBinary = ecdh.getPublicKey('binary');

            assert(Buffer.isBuffer(publicKeyBuffer));
            assert.strictEqual(typeof publicKeyHex, 'string');
            assert.strictEqual(typeof publicKeyBase64, 'string');
            assert.strictEqual(typeof publicKeyBinary, 'string');

            // All should represent the same key
            assert.strictEqual(publicKeyBuffer.toString('hex'), publicKeyHex);
            assert.strictEqual(publicKeyBuffer.toString('base64'), publicKeyBase64);
            assert.strictEqual(publicKeyBuffer.toString('binary'), publicKeyBinary);
        });

        it('should handle different point formats', () => {
            const ecdh = crypto.createECDH('prime256v1');
            ecdh.generateKeys();

            // Test different point formats
            const uncompressed = ecdh.getPublicKey('buffer', 'uncompressed');
            const compressed = ecdh.getPublicKey('buffer', 'compressed');
            const hybrid = ecdh.getPublicKey('buffer', 'hybrid');

            // Uncompressed format starts with 0x04
            assert.strictEqual(uncompressed[0], 4);

            // Compressed format starts with 0x02 or 0x03
            const firstByte = compressed[0];
            assert(firstByte === 2 || firstByte === 3);

            // Hybrid format starts with 0x06 or 0x07
            const hybridFirstByte = hybrid[0];
            assert(hybridFirstByte === 6 || hybridFirstByte === 7);
        });

        it('should throw for invalid point format', () => {
            const ecdh = crypto.createECDH('prime256v1');
            ecdh.generateKeys();

            assert.throws(
                () => ecdh.getPublicKey('buffer', 10),
                /Invalid ECDH format/
            );
        });

        it('should validate private key for curve', () => {
            const ecdh = crypto.createECDH('secp256k1');
            const cafebabeKey = 'cafebabe'.repeat(8);
            ecdh.setPrivateKey(cafebabeKey, 'hex');

            // Invalid private keys for secp256k1 curve
            const errMessage = /Private key is not valid for specified curve/;
            const invalidKeys = [
                '0000000000000000000000000000000000000000000000000000000000000000',
                'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141',
                'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF'
            ];

            for (const invalidKey of invalidKeys) {
                assert.throws(() => {
                    ecdh.setPrivateKey(invalidKey, 'hex');
                }, errMessage);

                // Verify object state did not change
                assert.strictEqual(ecdh.getPrivateKey('hex'), cafebabeKey);
            }
        });
    });

    // Test shared secret computation
    describe('Shared Secret Computation', () => {
        it('should compute shared secret correctly', () => {
            const ecdh1 = crypto.createECDH('prime256v1');
            const ecdh2 = crypto.createECDH('prime256v1');

            const key1 = ecdh1.generateKeys();
            const key2 = ecdh2.generateKeys('hex');

            const secret1 = ecdh1.computeSecret(key2.toString('hex'), 'hex', 'base64');
            const secret2 = ecdh2.computeSecret(key1.toString('hex'), 'hex', 'buffer');

            // Shared secrets should be identical
            assert.strictEqual(secret1, secret2.toString('base64'));
        });

        it('should handle different encoding combinations for computeSecret', () => {
            const ecdh1 = crypto.createECDH('prime256v1');
            const ecdh2 = crypto.createECDH('prime256v1');

            ecdh1.generateKeys();
            const pubKey = ecdh2.generateKeys();

            // Test different input/output encoding combinations
            const secret1 = ecdh1.computeSecret(pubKey.toString('hex'), 'hex'); // hex input, buffer output
            const secret2 = ecdh1.computeSecret(pubKey.toString('hex'), 'hex'); // hex input, buffer output
            const secret3 = ecdh1.computeSecret(pubKey.toString('base64'), 'base64', 'hex'); // base64 input, hex output
            const secret4 = ecdh1.computeSecret(pubKey.toString('binary'), 'binary', 'base64'); // binary input, base64 output

            assert(Buffer.isBuffer(secret1));
            assert(Buffer.isBuffer(secret2));
            assert.strictEqual(typeof secret3, 'string');
            assert.strictEqual(typeof secret4, 'string');

            // All should produce the same underlying secret
            assert(secret1.equals(secret2));
            assert.strictEqual(secret1.toString('hex'), secret3);
            assert.strictEqual(secret1.toString('base64'), secret4);
        });

        it('should compute SM2 shared secret correctly', () => {
            const ecdh1 = crypto.createECDH('SM2');
            const ecdh2 = crypto.createECDH('SM2');

            const key1 = ecdh1.generateKeys();
            const key2 = ecdh2.generateKeys();

            // Compute shared secrets
            const secret1 = ecdh1.computeSecret(key2.toString('hex'), 'hex');
            const secret2 = ecdh2.computeSecret(key1.toString('hex'), 'hex');

            // Shared secrets should be identical
            assert(Buffer.isBuffer(secret1));
            assert(Buffer.isBuffer(secret2));
            assert(secret1.equals(secret2));
            assert.strictEqual(secret1.length, 32); // SM2 shared secret is 32 bytes

            // Test with different encodings
            const hexSecret1 = ecdh1.computeSecret(key2.toString('hex'), 'hex', 'hex');
            const base64Secret1 = ecdh1.computeSecret(key2.toString('base64'), 'base64', 'base64');

            assert.strictEqual(typeof hexSecret1, 'string');
            assert.strictEqual(typeof base64Secret1, 'string');
            assert.strictEqual(secret1.toString('hex'), hexSecret1);
            assert.strictEqual(secret1.toString('base64'), base64Secret1);
        });

        it('should validate public key for same curve', () => {
            const ecdh1 = crypto.createECDH('prime256v1');
            const ecdh2 = crypto.createECDH('secp256k1');

            ecdh1.generateKeys();
            const key2 = ecdh2.generateKeys();

            // Should throw when using key from different curve
            assert.throws(
                () => ecdh1.computeSecret(key2, 'latin1', 'buffer'),
                {
                    code: 'ERR_CRYPTO_ECDH_INVALID_PUBLIC_KEY',
                    name: 'Error',
                    message: 'Public key is not valid for specified curve'
                }
            );
        });

        it('should compute secret with known test vectors', () => {
            const ecdh = crypto.createECDH('secp256k1');

            // Test with known values
            const cafebabeKey = 'cafebabe'.repeat(8);
            const peerPubPtComp = '02c6b754b20826eb925e052ee2c25285b162b51fdca732bcf67e39d647fb6830ae';
            const peerPubPtUnComp =
                '04c6b754b20826eb925e052ee2c25285b162b51fdca732bcf67e39d647fb6830ae' +
                'b651944a574a362082a77e3f2b5d9223eb54d7f2f76846522bf75f3bedb8178e';
            const expectedSecret = '1da220b5329bbe8bfd19ceef5a5898593f411a6f12ea40f2a8eead9a5cf59970';

            ecdh.setPrivateKey(cafebabeKey, 'hex');

            // Test with compressed public key
            const secret1 = ecdh.computeSecret(peerPubPtComp, 'hex', 'hex');
            assert.strictEqual(secret1, expectedSecret);

            // Test with uncompressed public key
            const secret2 = ecdh.computeSecret(peerPubPtUnComp, 'hex', 'hex');
            assert.strictEqual(secret2, expectedSecret);

            // Verify keys remained unchanged after computation
            assert.strictEqual(ecdh.getPrivateKey('hex'), cafebabeKey);
        });

        it('should handle invalid public keys', () => {
            const ecdh = crypto.createECDH('prime256v1');
            ecdh.generateKeys();

            const invalidKey = Buffer.alloc(65);
            invalidKey.fill(0);

            assert.throws(
                () => ecdh.computeSecret(invalidKey),
                {
                    code: 'ERR_CRYPTO_ECDH_INVALID_PUBLIC_KEY',
                    name: 'Error',
                    message: 'Public key is not valid for specified curve'
                }
            );
        });
    });

    // Test public key setting and conversion
    describe('Public Key Setting and Conversion', () => {
        it('should set and get public key in different formats', () => {
            const ecdh = crypto.createECDH('secp256k1');

            const cafebabePubPtComp = '03672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3';
            const cafebabePubPtUnComp =
                '04672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3' +
                '2e02c7f93d13dc2732b760ca377a5897b9dd41a1c1b29dc0442fdce6d0a04d1d';

            // Set compressed public key
            ecdh.setPublicKey(cafebabePubPtComp, 'hex');
            assert.strictEqual(ecdh.getPublicKey('hex'), cafebabePubPtUnComp);
            assert.strictEqual(ecdh.getPublicKey('hex', 'compressed'), cafebabePubPtComp);

            // Set uncompressed public key
            ecdh.setPublicKey(cafebabePubPtUnComp, 'hex');
            assert.strictEqual(ecdh.getPublicKey('hex'), cafebabePubPtUnComp);
            assert.strictEqual(ecdh.getPublicKey('hex', 'compressed'), cafebabePubPtComp);
        });

        it('should handle setPublicKey/setPrivateKey interaction', () => {
            const ecdh = crypto.createECDH('prime256v1');

            // Generate initial key pair
            ecdh.generateKeys();
            const originalPrivate = ecdh.getPrivateKey();
            const originalPublic = ecdh.getPublicKey();

            // Create another ECDH to get different public key
            const ecdh2 = crypto.createECDH('prime256v1');
            const otherPublic = ecdh2.generateKeys();

            // Set public key to other party's key
            ecdh.setPublicKey(otherPublic);

            // Private key should remain unchanged
            assert(ecdh.getPrivateKey().equals(originalPrivate));

            // But public key should be different
            assert(!ecdh.getPublicKey().equals(originalPublic));
            assert(ecdh.getPublicKey().equals(otherPublic));
        });

        it('should validate public key matches private key for computation', () => {
            const ecdh = crypto.createECDH('secp256k1');

            const cafebabeKey = 'cafebabe'.repeat(8);
            const peerPubPtComp = '02c6b754b20826eb925e052ee2c25285b162b51fdca732bcf67e39d647fb6830ae';

            ecdh.setPrivateKey(cafebabeKey, 'hex');

            // Set public key to peer's key (breaks the key pair relationship)
            ecdh.setPublicKey(peerPubPtComp, 'hex');

            // This should fail because public key doesn't match private key
            assert.throws(() => {
                ecdh.computeSecret(peerPubPtComp, 'hex', 'hex');
            }, /Invalid key pair/);
        });
    });

    // Test ECDH.convertKey static method
    describe('ECDH.convertKey', () => {
        it('should throw when key argument is undefined', () => {
            const ECDH = crypto.ECDH || crypto.createECDH('prime256v1').constructor;

            assert.throws(
                () => ECDH.convertKey(),
                /(Parameter not optional|argument must be of type)/
            );
        });

        it('should throw when curve argument is undefined', () => {
            const ECDH = crypto.ECDH || crypto.createECDH('prime256v1').constructor;
            const cafebabePubPtComp = '03672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3';

            assert.throws(
                () => ECDH.convertKey(cafebabePubPtComp),
                /(Parameter not optional|argument must be of type)/
            );
        });

        it('should throw for invalid curve name', () => {
            const ECDH = crypto.ECDH || crypto.createECDH('prime256v1').constructor;
            const cafebabePubPtComp = '03672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3';

            assert.throws(
                () => ECDH.convertKey(cafebabePubPtComp, 'badcurve'),
                /Invalid EC curve name/
            );
        });

        it('should convert key formats correctly', () => {
            const ECDH = crypto.ECDH || crypto.createECDH('secp256k1').constructor;

            const cafebabePubPtComp = '03672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3';
            const cafebabePubPtUnComp =
                '04672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3' +
                '2e02c7f93d13dc2732b760ca377a5897b9dd41a1c1b29dc0442fdce6d0a04d1d';

            // Convert to different formats
            const uncompressed = ECDH.convertKey(cafebabePubPtComp, 'secp256k1', 'hex', 'buffer', 'uncompressed');
            const compressed = ECDH.convertKey(cafebabePubPtComp, 'secp256k1', 'hex', 'buffer', 'compressed');
            const hybrid = ECDH.convertKey(cafebabePubPtComp, 'secp256k1', 'hex', 'buffer', 'hybrid');

            assert.strictEqual(uncompressed[0], 4);
            let firstByte = compressed[0];
            assert(firstByte === 2 || firstByte === 3);
            firstByte = hybrid[0];
            assert(firstByte === 6 || firstByte === 7);

            // Convert from hex to hex
            const uncompressedHex = ECDH.convertKey(cafebabePubPtComp, 'secp256k1', 'hex', 'hex', 'uncompressed');
            const compressedHex = ECDH.convertKey(cafebabePubPtComp, 'secp256k1', 'hex', 'hex', 'compressed');

            assert.strictEqual(uncompressedHex, cafebabePubPtUnComp);
            assert.strictEqual(compressedHex, cafebabePubPtComp);
        });

        it('should throw for invalid format parameter', () => {
            const ECDH = crypto.ECDH || crypto.createECDH('secp256k1').constructor;
            const cafebabePubPtComp = '03672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3';

            assert.throws(
                () => ECDH.convertKey(cafebabePubPtComp, 'secp256k1', 'hex', 'hex', 10),
                /Invalid ECDH format.*10/
            );
        });
    });

    // Test edge cases and error handling
    describe('Edge Cases and Error Handling', () => {
        it('should handle buffer operations correctly', () => {
            const ecdh1 = crypto.createECDH('prime256v1');
            const ecdh2 = crypto.createECDH('prime256v1');

            const key1 = ecdh1.generateKeys();
            const key2 = ecdh2.generateKeys();

            // Compute secret with buffer inputs/outputs
            const secret1 = ecdh1.computeSecret(key2);
            const secret2 = ecdh2.computeSecret(key1);

            assert(Buffer.isBuffer(secret1));
            assert(Buffer.isBuffer(secret2));
            assert(secret1.equals(secret2));
        });

        it('should handle multiple curves consistently', () => {
            const testCurves = ['prime256v1', 'secp384r1', 'secp521r1', 'SM2'];

            for (const curve of testCurves) {
                const ecdh = crypto.createECDH(curve);
                const publicKey = ecdh.generateKeys();

                assert(Buffer.isBuffer(publicKey));
                assert(publicKey.length > 0);

                const privateKey = ecdh.getPrivateKey();
                assert(Buffer.isBuffer(privateKey));
                assert(privateKey.length > 0);
            }
        });

        it('should not leak OpenSSL errors', () => {
            const ECDH = crypto.ECDH || crypto.createECDH('secp521r1').constructor;
            const badKey = 'f'.repeat(128);

            // This should throw but not leave errors on OpenSSL error stack
            assert.throws(
                () => ECDH.convertKey(badKey, 'secp521r1', 'hex', 'hex', 'compressed'),
                /Failed to convert Buffer to EC_POINT/
            );

            // Subsequent crypto operations should work fine
            const ecdh = crypto.createECDH('secp521r1');
            const key = ecdh.generateKeys();
            assert(Buffer.isBuffer(key));
        });

        it('should not leak OpenSSL errors after invalid computeSecret and allow subsequent operations', () => {
            const hashes = crypto.getHashes();
            assert(hashes.includes('sha256'), 'sha256 hash must be supported');

            const ecdh = crypto.createECDH('prime256v1');
            const invalidKey = Buffer.alloc(65);
            invalidKey.fill(0);

            ecdh.generateKeys();

            // This should throw but not leave errors on OpenSSL error stack
            assert.throws(
                () => ecdh.computeSecret(invalidKey),
                {
                    code: 'ERR_CRYPTO_ECDH_INVALID_PUBLIC_KEY',
                    name: 'Error',
                    message: 'Public key is not valid for specified curve'
                }
            );

            // Check that subsequent crypto operations work fine
            // This is a simple test to ensure OpenSSL error stack is clean
            const testData = Buffer.from('test data');
            const hash = crypto.createHash('sha256');
            hash.update(testData);
            const digest = hash.digest();

            assert(Buffer.isBuffer(digest));
            assert.strictEqual(digest.length, 32); // SHA-256 produces 32 bytes
        });

        it('should handle all supported curves', () => {
            const ecdhCurves = ['prime256v1', 'secp256k1', 'secp384r1', 'secp521r1'];

            for (const curveName of ecdhCurves) {
                const ecdh = crypto.createECDH(curveName);
                const publicKey = ecdh.generateKeys();

                assert(Buffer.isBuffer(publicKey));
                assert(publicKey.length > 0);

                const privateKey = ecdh.getPrivateKey();
                assert(Buffer.isBuffer(privateKey));
                assert(privateKey.length > 0);

                // Test that we can set the same keys back
                const ecdh2 = crypto.createECDH(curveName);
                ecdh2.setPrivateKey(privateKey);
                assert(ecdh2.getPublicKey().equals(publicKey));
            }
        });

        it('should handle setPrivateKey with different encodings', () => {
            const ecdh = crypto.createECDH('secp256k1');
            const cafebabeKey = 'cafebabe'.repeat(8);

            // Test setting with different encodings
            ecdh.setPrivateKey(cafebabeKey, 'hex');
            const originalPubKey = ecdh.getPublicKey('hex');

            // Set with base64
            const ecdh2 = crypto.createECDH('secp256k1');
            ecdh2.setPrivateKey(Buffer.from(cafebabeKey, 'hex').toString('base64'), 'base64');
            assert.strictEqual(ecdh2.getPublicKey('hex'), originalPubKey);

            // Set with binary
            const ecdh3 = crypto.createECDH('secp256k1');
            ecdh3.setPrivateKey(Buffer.from(cafebabeKey, 'hex').toString('binary'), 'binary');
            assert.strictEqual(ecdh3.getPublicKey('hex'), originalPubKey);

            // Set with buffer
            const ecdh4 = crypto.createECDH('secp256k1');
            ecdh4.setPrivateKey(Buffer.from(cafebabeKey, 'hex'));
            assert.strictEqual(ecdh4.getPublicKey('hex'), originalPubKey);
        });

        it('should handle setPublicKey with different encodings', () => {
            const cafebabePubPtComp = '03672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3';
            const cafebabePubPtUnComp =
                '04672a31bfc59d3f04548ec9b7daeeba2f61814e8ccc40448045007f5479f693a3' +
                '2e02c7f93d13dc2732b760ca377a5897b9dd41a1c1b29dc0442fdce6d0a04d1d';

            // Test setting with different encodings
            const ecdh1 = crypto.createECDH('secp256k1');
            ecdh1.setPublicKey(cafebabePubPtComp, 'hex');

            const ecdh2 = crypto.createECDH('secp256k1');
            ecdh2.setPublicKey(Buffer.from(cafebabePubPtComp, 'hex').toString('base64'), 'base64');
            assert(ecdh2.getPublicKey().equals(ecdh1.getPublicKey()));

            const ecdh3 = crypto.createECDH('secp256k1');
            ecdh3.setPublicKey(Buffer.from(cafebabePubPtComp, 'hex').toString('binary'), 'binary');
            assert(ecdh3.getPublicKey().equals(ecdh1.getPublicKey()));

            const ecdh4 = crypto.createECDH('secp256k1');
            ecdh4.setPublicKey(Buffer.from(cafebabePubPtComp, 'hex'));
            assert(ecdh4.getPublicKey().equals(ecdh1.getPublicKey()));
        });

        it('should reject setting public key from different curve', () => {
            const ecdh1 = crypto.createECDH('prime256v1');
            const ecdh2 = crypto.createECDH('secp256k1');

            ecdh1.setPrivateKey(Buffer.from('cafebabe'.repeat(8), 'hex'));
            const pubKey2 = ecdh2.generateKeys();

            // Should throw when setting public key from different curve
            assert.throws(() => {
                ecdh1.setPublicKey(pubKey2);
            }, { message: 'Failed to convert Buffer to EC_POINT' });
        });
    });
});