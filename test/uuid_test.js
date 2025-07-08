var test = require("test");
test.setup();

var uuid = require('uuid');
var encoding = require('encoding');

describe("uuid", () => {
    it("constants", () => {
        // Test namespace constants
        assert.equal(uuid.DNS, 0);
        assert.equal(uuid.URL, 1);
        assert.equal(uuid.OID, 2);
        assert.equal(uuid.X509, 3);

        // Test NIL and MAX constants
        assert.equal(uuid.NIL, "00000000-0000-0000-0000-000000000000");
        assert.equal(uuid.MAX, "ffffffff-ffff-ffff-ffff-ffffffffffff");
    });

    it("v1 and v4 string generation", () => {
        // Test v1 (time-based) UUID string generation
        var v1_uuid = uuid.v1();
        assert.isString(v1_uuid);
        assert.equal(v1_uuid.length, 36);
        assert.match(v1_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-1[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Test v4 (random) UUID string generation
        var v4_uuid = uuid.v4();
        assert.isString(v4_uuid);
        assert.equal(v4_uuid.length, 36);
        assert.match(v4_uuid, /^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i);

        // Ensure different calls generate different UUIDs
        assert.notEqual(uuid.v1(), uuid.v1());
        assert.notEqual(uuid.v4(), uuid.v4());
    });

    it("parse function", () => {
        // Test parse function
        var test_uuid = "550e8400-e29b-41d4-a716-446655440000";
        var parsed = uuid.parse(test_uuid);
        assert.isTrue(Buffer.isBuffer(parsed));
        assert.equal(parsed.length, 16);

        // Verify the parsed bytes match expected values
        assert.equal(parsed.toString('hex'), "550e8400e29b41d4a716446655440000");

        // Test round-trip with known UUID
        var known_uuid = uuid.v4();
        var parsed_known = uuid.parse(known_uuid);
        assert.isTrue(Buffer.isBuffer(parsed_known));
        assert.equal(parsed_known.length, 16);
    });

    it("md5 and sha1 namespaced UUIDs", () => {
        var name = "example.com";

        // Test md5 with different namespaces
        var md5_dns = uuid.md5(uuid.DNS, name);
        var md5_url = uuid.md5(uuid.URL, name);
        var md5_oid = uuid.md5(uuid.OID, name);
        var md5_x509 = uuid.md5(uuid.X509, name);

        assert.isTrue(Buffer.isBuffer(md5_dns));
        assert.isTrue(Buffer.isBuffer(md5_url));
        assert.isTrue(Buffer.isBuffer(md5_oid));
        assert.isTrue(Buffer.isBuffer(md5_x509));

        // Same namespace and name should produce same result
        var md5_dns2 = uuid.md5(uuid.DNS, name);
        assert.equal(md5_dns.toString('hex'), md5_dns2.toString('hex'));

        // Different namespaces should produce different results
        assert.notEqual(md5_dns.toString('hex'), md5_url.toString('hex'));

        // Test sha1 with different namespaces
        var sha1_dns = uuid.sha1(uuid.DNS, name);
        var sha1_url = uuid.sha1(uuid.URL, name);
        var sha1_oid = uuid.sha1(uuid.OID, name);
        var sha1_x509 = uuid.sha1(uuid.X509, name);

        assert.isTrue(Buffer.isBuffer(sha1_dns));
        assert.isTrue(Buffer.isBuffer(sha1_url));
        assert.isTrue(Buffer.isBuffer(sha1_oid));
        assert.isTrue(Buffer.isBuffer(sha1_x509));

        // Same namespace and name should produce same result
        var sha1_dns2 = uuid.sha1(uuid.DNS, name);
        assert.equal(sha1_dns.toString('hex'), sha1_dns2.toString('hex'));

        // Different namespaces should produce different results
        assert.notEqual(sha1_dns.toString('hex'), sha1_url.toString('hex'));

        // MD5 and SHA1 should produce different results
        assert.notEqual(md5_dns.toString('hex'), sha1_dns.toString('hex'));
    });

    it("snowflake algorithm", () => {
        // Test snowflake generation
        var snowflake1 = uuid.snowflake();
        var snowflake2 = uuid.snowflake();

        assert.isTrue(Buffer.isBuffer(snowflake1));
        assert.isTrue(Buffer.isBuffer(snowflake2));
        assert.equal(snowflake1.length, 8);
        assert.equal(snowflake2.length, 8);

        // Different calls should generate different snowflakes
        assert.notEqual(snowflake1.toString('hex'), snowflake2.toString('hex'));

        // Test hostID property
        var original_hostID = uuid.hostID;
        assert.isNumber(original_hostID);

        // Set new hostID
        uuid.hostID = 123;
        assert.equal(uuid.hostID, 123);

        // Test that hostID affects snowflake generation
        var snowflake_with_hostID = uuid.snowflake();
        assert.isTrue(Buffer.isBuffer(snowflake_with_hostID));

        // Restore original hostID
        uuid.hostID = original_hostID;
        assert.equal(uuid.hostID, original_hostID);

        // Test hostID boundary (should be masked to 10 bits: 0x3ff)
        uuid.hostID = 0x7ff; // 11 bits
        assert.equal(uuid.hostID, 0x3ff); // Should be masked to 10 bits
    });

    it("node and random buffer generation", () => {
        // Test node UUID generation
        var node1 = uuid.node();
        var node2 = uuid.node();

        assert.isTrue(Buffer.isBuffer(node1));
        assert.isTrue(Buffer.isBuffer(node2));
        assert.equal(node1.length, 16);
        assert.equal(node2.length, 16);

        // Different calls should generate different node UUIDs
        assert.notEqual(node1.toString('hex'), node2.toString('hex'));

        // Test random UUID generation
        var random1 = uuid.random();
        var random2 = uuid.random();

        assert.isTrue(Buffer.isBuffer(random1));
        assert.isTrue(Buffer.isBuffer(random2));
        assert.equal(random1.length, 16);
        assert.equal(random2.length, 16);

        // Different calls should generate different random UUIDs
        assert.notEqual(random1.toString('hex'), random2.toString('hex'));
    });

    it("error handling", () => {
        // Test parse with invalid UUID
        assert.throws(() => {
            uuid.parse("invalid-uuid");
        });

        // Test md5/sha1 with invalid namespace
        assert.throws(() => {
            uuid.md5(99, "test"); // Invalid namespace
        });

        assert.throws(() => {
            uuid.sha1(-1, "test"); // Invalid namespace
        });
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

        valid_uuids.forEach(function (test_uuid) {
            assert.isTrue(uuid.validate(test_uuid), "UUID should be valid: " + test_uuid);
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
            assert.isFalse(uuid.validate(test_uuid), "UUID should be invalid: " + test_uuid);
        });
    });

    it("original test", () => {
        var id = uuid.node();
        var id1 = uuid.random();

        assert.notEqual(id.toString(), id1.toString());
    });
});

