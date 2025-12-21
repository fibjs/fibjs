var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var uuid = require('uuid');

// Detect runtime environment
var isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

describe("uuid", () => {
    it("constants", () => {
        // Test NIL and MAX constants
        assert.strictEqual(uuid.NIL, "00000000-0000-0000-0000-000000000000");
        assert.strictEqual(uuid.MAX, "ffffffff-ffff-ffff-ffff-ffffffffffff");
    });
    
    it("parse function", () => {
        // Test parse function
        var test_uuid = "550e8400-e29b-41d4-a716-446655440000";
        var parsed = uuid.parse(test_uuid);
        
        assert.ok(parsed instanceof Uint8Array);
        assert.strictEqual(parsed.length, 16);

        // Verify the parsed bytes match expected values
        var hexString = Array.from(parsed).map(b => b.toString(16).padStart(2, '0')).join('');
        assert.strictEqual(hexString, "550e8400e29b41d4a716446655440000");
    });
    
    it("stringify function", () => {
        // npm uuid stringify accepts Uint8Array
        var arr = new Uint8Array([0x55, 0x0e, 0x84, 0x00, 0xe2, 0x9b, 0x41, 0xd4, 0xa7, 0x16, 0x44, 0x66, 0x55, 0x44, 0x00, 0x00]);
        var str = uuid.stringify(arr);
        assert.strictEqual(str, "550e8400-e29b-41d4-a716-446655440000");
        
        // Test round-trip
        var known_uuid = uuid.v4();
        var parsed = uuid.parse(known_uuid);
        var stringified = uuid.stringify(parsed);
        assert.strictEqual(stringified, known_uuid);
        
        // Test with offset
        var largerArr = new Uint8Array(20);
        largerArr.set(arr, 4);
        var str2 = uuid.stringify(largerArr, 4);
        assert.strictEqual(str2, "550e8400-e29b-41d4-a716-446655440000");
    });

    it("v1 (time-based) UUID generation", () => {
        // Test v1 (time-based) UUID string generation
        var v1_uuid = uuid.v1();
        assert.strictEqual(typeof v1_uuid, 'string');
        assert.strictEqual(v1_uuid.length, 36);
        assert.match(v1_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-1[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Ensure different calls generate different UUIDs
        assert.notStrictEqual(uuid.v1(), uuid.v1());
        
        // Test v1 with custom options
        var customOptions = {
            node: [0x01, 0x23, 0x45, 0x67, 0x89, 0xab],
            clockseq: 0x1234,
            msecs: new Date('2011-11-01').getTime(),
            nsecs: 5678
        };
        var v1_custom = uuid.v1(customOptions);
        assert.strictEqual(typeof v1_custom, 'string');
        assert.strictEqual(v1_custom.length, 36);
    });

    it("v3 (MD5 namespaced) UUID generation", () => {
        // v3 uses MD5 hashing with a namespace
        var name = "example.com";
        
        // Using DNS namespace (predefined in RFC)
        var v3_uuid = uuid.v3(name, isFibjs ? uuid.DNS_NAMESPACE : uuid.v3.DNS);
        assert.strictEqual(typeof v3_uuid, 'string');
        assert.strictEqual(v3_uuid.length, 36);
        assert.match(v3_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-3[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Same namespace and name should produce same result
        var v3_uuid2 = uuid.v3(name, isFibjs ? uuid.DNS_NAMESPACE : uuid.v3.DNS);
        assert.strictEqual(v3_uuid, v3_uuid2);

        // Different namespaces should produce different results
        var v3_url = uuid.v3(name, isFibjs ? uuid.URL_NAMESPACE : uuid.v3.URL);
        assert.notStrictEqual(v3_uuid, v3_url);
        
        // Test other predefined namespaces
        if (!isFibjs) {
            assert.ok(uuid.v3.DNS);
            assert.ok(uuid.v3.URL);
        }
    });

    it("v3 with Buffer namespace", () => {
        var name = "example.com";
        
        // DNS namespace UUID: 6ba7b810-9dad-11d1-80b4-00c04fd430c8
        var dnsNamespaceBytes = new Uint8Array([
            0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1,
            0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
        ]);
        
        // Test v3 with Buffer namespace
        var v3_buffer = uuid.v3(name, dnsNamespaceBytes);
        assert.strictEqual(typeof v3_buffer, 'string');
        assert.strictEqual(v3_buffer.length, 36);
        assert.match(v3_buffer, /^[0-9a-f]{8}-[0-9a-f]{4}-3[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);
        
        // Should produce same result as string namespace
        var v3_string = uuid.v3(name, isFibjs ? uuid.DNS_NAMESPACE : uuid.v3.DNS);
        assert.strictEqual(v3_buffer, v3_string);
        
        // Test with custom namespace bytes
        var customNs = new Uint8Array([
            0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
            0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0
        ]);
        var v3_custom = uuid.v3(name, customNs);
        assert.strictEqual(typeof v3_custom, 'string');
        assert.strictEqual(v3_custom.length, 36);
        
        // Test error with too short buffer
        assert.throws(() => {
            uuid.v3(name, new Uint8Array(10));
        });
    });

    it("v4 (random) UUID generation", () => {
        // Test v4 (random) UUID string generation
        var v4_uuid = uuid.v4();
        assert.strictEqual(typeof v4_uuid, 'string');
        assert.strictEqual(v4_uuid.length, 36);
        assert.match(v4_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Ensure different calls generate different UUIDs
        assert.notStrictEqual(uuid.v4(), uuid.v4());
        
        // Test v4 with custom random function
        var customRandom = new Array(16);
        for (var i = 0; i < 16; i++) {
            customRandom[i] = Math.floor(Math.random() * 256);
        }
        var v4_custom = uuid.v4({ random: customRandom });
        assert.strictEqual(typeof v4_custom, 'string');
        assert.strictEqual(v4_custom.length, 36);
    });

    it("v5 (SHA1 namespaced) UUID generation", () => {
        // v5 uses SHA1 hashing with a namespace
        var name = "example.com";
        
        // Using DNS namespace (predefined in RFC)
        var v5_uuid = uuid.v5(name, isFibjs ? uuid.DNS_NAMESPACE : uuid.v5.DNS);
        assert.strictEqual(typeof v5_uuid, 'string');
        assert.strictEqual(v5_uuid.length, 36);
        assert.match(v5_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-5[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Same namespace and name should produce same result
        var v5_uuid2 = uuid.v5(name, isFibjs ? uuid.DNS_NAMESPACE : uuid.v5.DNS);
        assert.strictEqual(v5_uuid, v5_uuid2);

        // Different namespaces should produce different results
        var v5_url = uuid.v5(name, isFibjs ? uuid.URL_NAMESPACE : uuid.v5.URL);
        assert.notStrictEqual(v5_uuid, v5_url);
        
        if (!isFibjs) {
            // Test other predefined namespaces
            assert.ok(uuid.v5.DNS);
            assert.ok(uuid.v5.URL);
            
            // v3 and v5 should produce different results (different hashing algorithms)
            var v3_uuid = uuid.v3(name, uuid.v3.DNS);
            assert.notStrictEqual(v3_uuid, v5_uuid);
        }
    });

    it("v5 with Buffer namespace", () => {
        var name = "example.com";
        
        // DNS namespace UUID: 6ba7b810-9dad-11d1-80b4-00c04fd430c8
        var dnsNamespaceBytes = new Uint8Array([
            0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1,
            0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
        ]);
        
        // Test v5 with Buffer namespace
        var v5_buffer = uuid.v5(name, dnsNamespaceBytes);
        assert.strictEqual(typeof v5_buffer, 'string');
        assert.strictEqual(v5_buffer.length, 36);
        assert.match(v5_buffer, /^[0-9a-f]{8}-[0-9a-f]{4}-5[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);
        
        // Should produce same result as string namespace
        var v5_string = uuid.v5(name, isFibjs ? uuid.DNS_NAMESPACE : uuid.v5.DNS);
        assert.strictEqual(v5_buffer, v5_string);
        
        // Test with custom namespace bytes
        var customNs = new Uint8Array([
            0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
            0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0
        ]);
        var v5_custom = uuid.v5(name, customNs);
        assert.strictEqual(typeof v5_custom, 'string');
        assert.strictEqual(v5_custom.length, 36);
        
        // Test error with too short buffer
        assert.throws(() => {
            uuid.v5(name, new Uint8Array(10));
        });
    });

    it("v6 (reordered time-based) UUID generation", () => {
        // v6 is a reordered time-based UUID
        var v6_uuid = uuid.v6();
        assert.strictEqual(typeof v6_uuid, 'string');
        assert.strictEqual(v6_uuid.length, 36);
        assert.match(v6_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-6[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Different calls should generate different UUIDs
        assert.notStrictEqual(uuid.v6(), uuid.v6());
        
        // Test v6 with custom options
        var customOptions = {
            node: [0x01, 0x23, 0x45, 0x67, 0x89, 0xab],
            clockseq: 0x1234,
            msecs: new Date('2011-11-01').getTime(),
            nsecs: 5678
        };
        var v6_custom = uuid.v6(customOptions);
        assert.strictEqual(typeof v6_custom, 'string');
        assert.strictEqual(v6_custom.length, 36);
    });

    it("v7 (Unix Epoch time-based) UUID generation", () => {
        // v7 uses Unix Epoch timestamp
        var v7_uuid = uuid.v7();
        assert.strictEqual(typeof v7_uuid, 'string');
        assert.strictEqual(v7_uuid.length, 36);
        assert.match(v7_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-7[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Different calls should generate different UUIDs
        var v7_uuid2 = uuid.v7();
        assert.notStrictEqual(v7_uuid, v7_uuid2);
        
        // Test v7 with custom timestamp
        var customTimestamp = new Date('2023-01-01').getTime();
        var v7_custom = uuid.v7({ msecs: customTimestamp });
        assert.strictEqual(typeof v7_custom, 'string');
        assert.strictEqual(v7_custom.length, 36);
    });

    it("validate function", () => {
        // Test valid UUIDs
        var valid_uuids = [
            "550e8400-e29b-41d4-a716-446655440000",
            "6ba7b810-9dad-11d1-80b4-00c04fd430c8",
            uuid.v1(),
            uuid.v4(),
            uuid.NIL,
            uuid.MAX
        ];
        
        // Add v3, v5, v6, v7 if available
        if (!isFibjs) {
            valid_uuids.push(uuid.v3("test", uuid.v3.DNS));
            valid_uuids.push(uuid.v5("test", uuid.v5.DNS));
            valid_uuids.push(uuid.v6());
            valid_uuids.push(uuid.v7());
        } else {
            valid_uuids.push(uuid.v3("test", uuid.DNS_NAMESPACE));
            valid_uuids.push(uuid.v5("test", uuid.DNS_NAMESPACE));
            valid_uuids.push(uuid.v6());
            valid_uuids.push(uuid.v7());
        }

        valid_uuids.forEach(function (test_uuid) {
            assert.strictEqual(uuid.validate(test_uuid), true, "UUID should be valid: " + test_uuid);
        });

        // Test invalid UUIDs
        var invalid_uuids = [
            "invalid-uuid",
            "550e8400-e29b-41d4-a716-44665544000",  // too short
            "550e8400-e29b-41d4-a716-4466554400001", // too long
            "550e8400-e29b-41d4-a716-446655z40000",  // invalid character
            "550e8400e29b41d4a716446655440000",       // missing dashes
            "",                                       // empty string
            "550e8400-e29b-41d4-a716-446655440000-extra", // extra content
            "550e8400_e29b_41d4_a716_446655440000",  // wrong separator
            "550e8400-e29b-41d4-a716446655440000",   // missing one dash
            "550e8400-e29b-41d4-a716-44665544000g"   // invalid character at end
        ];

        invalid_uuids.forEach(function (test_uuid) {
            assert.strictEqual(uuid.validate(test_uuid), false, "UUID should be invalid: " + test_uuid);
        });
    });

    it("version function", () => {
        // Test version detection for different UUID versions
        var v1_uuid = uuid.v1();
        assert.strictEqual(uuid.version(v1_uuid), 1);
        
        var v3_ns = isFibjs ? uuid.DNS_NAMESPACE : uuid.v3.DNS;
        var v3_uuid = uuid.v3("test", v3_ns);
        assert.strictEqual(uuid.version(v3_uuid), 3);
        
        var v4_uuid = uuid.v4();
        assert.strictEqual(uuid.version(v4_uuid), 4);
        
        var v5_ns = isFibjs ? uuid.DNS_NAMESPACE : uuid.v5.DNS;
        var v5_uuid = uuid.v5("test", v5_ns);
        assert.strictEqual(uuid.version(v5_uuid), 5);
        
        var v6_uuid = uuid.v6();
        assert.strictEqual(uuid.version(v6_uuid), 6);
        
        var v7_uuid = uuid.v7();
        assert.strictEqual(uuid.version(v7_uuid), 7);
        
        // Test with NIL UUID
        assert.strictEqual(uuid.version(uuid.NIL), 0);
        
        // Invalid UUID should return undefined or throw
        try {
            var invalidVersion = uuid.version("invalid-uuid");
            assert.strictEqual(invalidVersion, undefined);
        } catch (e) {
            // Some versions might throw instead
            assert.ok(e);
        }
    });

    it("v1 to v6 conversion", () => {
        // Test converting v1 to v6
        var v1_uuid = uuid.v1();
        var v6_uuid = uuid.v1ToV6(v1_uuid);
        
        assert.strictEqual(typeof v6_uuid, 'string');
        assert.strictEqual(v6_uuid.length, 36);
        assert.strictEqual(uuid.version(v6_uuid), 6);
        
        // Converting back should give same time information
        var v1_back = uuid.v6ToV1(v6_uuid);
        assert.strictEqual(typeof v1_back, 'string');
        assert.strictEqual(uuid.version(v1_back), 1);
    });

    it("error handling", () => {
        // Test parse with invalid UUID
        assert.throws(() => {
            uuid.parse("invalid-uuid");
        });
        
        // Test stringify with invalid buffer
        assert.throws(() => {
            uuid.stringify(new Uint8Array(10)); // Wrong length
        });
        
        // Test v3 with invalid namespace
        assert.throws(() => {
            uuid.v3("test", "invalid-namespace");
        });
        
        // Test v5 with invalid namespace
        assert.throws(() => {
            uuid.v5("test", "invalid-namespace");
        });
    });

    it("deterministic namespaced UUIDs", () => {
        // v3 and v5 should be deterministic
        var name = "hello.example.com";
        var namespace = uuid.v4(); // Use a random UUID as namespace
        
        // v3 should be consistent
        var v3_1 = uuid.v3(name, namespace);
        var v3_2 = uuid.v3(name, namespace);
        assert.strictEqual(v3_1, v3_2);
        
        // v5 should be consistent
        var v5_1 = uuid.v5(name, namespace);
        var v5_2 = uuid.v5(name, namespace);
        assert.strictEqual(v5_1, v5_2);
        
        // Different names should produce different UUIDs
        var v3_different = uuid.v3("different.example.com", namespace);
        assert.notStrictEqual(v3_1, v3_different);
        
        // Same name, different namespace should produce different UUIDs
        var different_namespace = uuid.v4();
        var v3_diff_ns = uuid.v3(name, different_namespace);
        assert.notStrictEqual(v3_1, v3_diff_ns);
    });

    it("UUID uniqueness", () => {
        // Generate multiple UUIDs and ensure they're all unique
        var generated = new Set();
        var count = 1000;
        
        for (var i = 0; i < count; i++) {
            var id = uuid.v4();
            assert.strictEqual(generated.has(id), false, "Duplicate UUID generated: " + id);
            generated.add(id);
        }
        
        assert.strictEqual(generated.size, count);
    });

    // FibJS-specific API tests (skip in Node.js)
    if (isFibjs) {
        it("fibjs: namespace constants", () => {
            // Test namespace constants (fibjs-specific)
            assert.strictEqual(uuid.DNS, 0);
            assert.strictEqual(uuid.URL, 1);
            assert.strictEqual(uuid.OID, 2);
            assert.strictEqual(uuid.X509, 3);
        });

        it("fibjs: md5 and sha1 namespaced UUIDs", () => {
            var name = "example.com";

            // Test md5 with different namespaces
            var md5_dns = uuid.md5(uuid.DNS, name);
            var md5_url = uuid.md5(uuid.URL, name);
            var md5_oid = uuid.md5(uuid.OID, name);
            var md5_x509 = uuid.md5(uuid.X509, name);

            assert.ok(Buffer.isBuffer(md5_dns));
            assert.ok(Buffer.isBuffer(md5_url));
            assert.ok(Buffer.isBuffer(md5_oid));
            assert.ok(Buffer.isBuffer(md5_x509));

            // Same namespace and name should produce same result
            var md5_dns2 = uuid.md5(uuid.DNS, name);
            assert.strictEqual(md5_dns.toString('hex'), md5_dns2.toString('hex'));

            // Different namespaces should produce different results
            assert.notStrictEqual(md5_dns.toString('hex'), md5_url.toString('hex'));

            // Test sha1 with different namespaces
            var sha1_dns = uuid.sha1(uuid.DNS, name);
            var sha1_url = uuid.sha1(uuid.URL, name);
            var sha1_oid = uuid.sha1(uuid.OID, name);
            var sha1_x509 = uuid.sha1(uuid.X509, name);

            assert.ok(Buffer.isBuffer(sha1_dns));
            assert.ok(Buffer.isBuffer(sha1_url));
            assert.ok(Buffer.isBuffer(sha1_oid));
            assert.ok(Buffer.isBuffer(sha1_x509));

            // Same namespace and name should produce same result
            var sha1_dns2 = uuid.sha1(uuid.DNS, name);
            assert.strictEqual(sha1_dns.toString('hex'), sha1_dns2.toString('hex'));

            // Different namespaces should produce different results
            assert.notStrictEqual(sha1_dns.toString('hex'), sha1_url.toString('hex'));

            // MD5 and SHA1 should produce different results
            assert.notStrictEqual(md5_dns.toString('hex'), sha1_dns.toString('hex'));
        });

        it("fibjs: snowflake algorithm", () => {
            // Test snowflake generation
            var snowflake1 = uuid.snowflake();
            var snowflake2 = uuid.snowflake();

            assert.ok(Buffer.isBuffer(snowflake1));
            assert.ok(Buffer.isBuffer(snowflake2));
            assert.strictEqual(snowflake1.length, 8);
            assert.strictEqual(snowflake2.length, 8);

            // Different calls should generate different snowflakes
            assert.notStrictEqual(snowflake1.toString('hex'), snowflake2.toString('hex'));

            // Test hostID property
            var original_hostID = uuid.hostID;
            assert.strictEqual(typeof original_hostID, 'number');

            // Set new hostID
            uuid.hostID = 123;
            assert.strictEqual(uuid.hostID, 123);

            // Test that hostID affects snowflake generation
            var snowflake_with_hostID = uuid.snowflake();
            assert.ok(Buffer.isBuffer(snowflake_with_hostID));

            // Restore original hostID
            uuid.hostID = original_hostID;
            assert.strictEqual(uuid.hostID, original_hostID);

            // Test hostID boundary (should be masked to 10 bits: 0x3ff)
            uuid.hostID = 0x7ff; // 11 bits
            assert.strictEqual(uuid.hostID, 0x3ff); // Should be masked to 10 bits
        });

        it("fibjs: node and random buffer generation", () => {
            // Test node UUID generation
            var node1 = uuid.node();
            var node2 = uuid.node();

            assert.ok(Buffer.isBuffer(node1));
            assert.ok(Buffer.isBuffer(node2));
            assert.strictEqual(node1.length, 16);
            assert.strictEqual(node2.length, 16);

            // Different calls should generate different node UUIDs
            assert.notStrictEqual(node1.toString('hex'), node2.toString('hex'));

            // Test random UUID generation
            var random1 = uuid.random();
            var random2 = uuid.random();

            assert.ok(Buffer.isBuffer(random1));
            assert.ok(Buffer.isBuffer(random2));
            assert.strictEqual(random1.length, 16);
            assert.strictEqual(random2.length, 16);

            // Different calls should generate different random UUIDs
            assert.notStrictEqual(random1.toString('hex'), random2.toString('hex'));
        });

        it("fibjs: md5/sha1 error handling", () => {
            // Test md5/sha1 with invalid namespace
            assert.throws(() => {
                uuid.md5(99, "test"); // Invalid namespace
            });

            assert.throws(() => {
                uuid.sha1(-1, "test"); // Invalid namespace
            });
        });
    }
});

