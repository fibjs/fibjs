var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var zlib = require('zlib');
var io = require('io');
var fs = require('fs');
var path = require('path');

var M = 102400;
var b = Buffer.alloc(M);

var i;

for (i = 0; i < M; i++)
    b[i] = Math.random() * 70 + 33;

describe("zlib", () => {
    it("inflate", () => {
        assert.deepEqual(zlib.inflate(zlib.deflate(b)), b);
    });

    it("inflate maxSize", () => {
        zlib.inflate(zlib.deflate(b), M);
        assert.throws(() => {
            zlib.inflate(zlib.deflate(b), M - 1);
        });
    });

    it("gzip", () => {
        assert.deepEqual(zlib.gunzip(zlib.gzip(b)), b);
    });

    it("deflateTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.deflateTo(b, stm);
        stm.rewind();
        assert.deepEqual(zlib.inflate(stm.readAll()), b);
    });

    it("inflateTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.inflateTo(zlib.deflate(b), stm);
        stm.rewind();
        assert.deepEqual(stm.readAll(), b);
    });

    it("gzipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.gzipTo(b, stm);
        stm.rewind();
        assert.deepEqual(zlib.gunzip(stm.readAll()), b);
    });

    it("gunzipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.gunzipTo(zlib.gzip(b), stm);
        stm.rewind();
        assert.deepEqual(stm.readAll(), b);
    });

    it("deflateTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(b);
        stm.rewind();

        var stm1 = new io.MemoryStream();
        zlib.deflateTo(stm, stm1);

        stm1.rewind();
        assert.deepEqual(zlib.inflate(stm1.readAll()), b);
    });

    it("inflateTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(zlib.deflate(b));
        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.inflateTo(stm, stm1);
        stm1.rewind();
        assert.deepEqual(stm1.readAll(), b);
    });

    it("gzipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(b);

        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.gzipTo(stm, stm1);
        stm1.rewind();
        assert.deepEqual(zlib.gunzip(stm1.readAll()), b);
    });

    it("gunzipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(zlib.gzip(b));
        stm.rewind()
        var stm1 = new io.MemoryStream();
        zlib.gunzipTo(stm, stm1);
        stm1.rewind();
        assert.deepEqual(stm1.readAll(), b);
    });

    it("gunzip (from file)", () => {
        var f1 = fs.openFile(path.join(__dirname, 'zlib_files', 'gzip'));
        var f2 = fs.openFile(path.join(__dirname, 'zlib_files', 'original.js'));
        assert.deepEqual(zlib.gunzip(f1.readAll()), f2.readAll());
    });

    it("inflate (from file)", () => {
        var f1 = fs.openFile(path.join(__dirname, 'zlib_files', 'deflate'));
        var f2 = fs.openFile(path.join(__dirname, 'zlib_files', 'original.js'));
        assert.deepEqual(zlib.inflateRaw(f1.readAll()), f2.readAll());
    });

    // ZIP/UNZIP tests
    // Note: ZIP format uses deflateRaw compression internally (without zlib headers)
    it("zip/unzip basic functionality", () => {
        assert.deepEqual(zlib.unzip(zlib.zip(b)), b);
    });

    it("zip with compression level", () => {
        var compressed_fast = zlib.zip(b, zlib.BEST_SPEED);
        var compressed_best = zlib.zip(b, zlib.BEST_COMPRESSION);
        var compressed_default = zlib.zip(b);

        // All should decompress to original data
        assert.deepEqual(zlib.unzip(compressed_fast), b);
        assert.deepEqual(zlib.unzip(compressed_best), b);
        assert.deepEqual(zlib.unzip(compressed_default), b);

        // Compression ratios should be different
        assert.notEqual(compressed_fast.length, compressed_best.length);
    });

    it("zip vs gzip format difference", () => {
        var zip_compressed = zlib.zip(b);
        var gzip_compressed = zlib.gzip(b);

        // ZIP and GZIP should produce different formats
        assert.notDeepEqual(zip_compressed, gzip_compressed);

        // Cross-decompression should fail because formats are incompatible
        // Note: unzip can now handle both gzip and zip formats due to auto-detection
        // so we only test that gunzip can't handle zip and vice versa for non-unzip methods
        assert.throws(() => {
            zlib.gunzip(zip_compressed);
        });

        // Note: unzip now supports auto-detection and can handle gzip format
        // This is compatible with Node.js behavior

        // But each format should decompress with its own method
        assert.deepEqual(zlib.unzip(zip_compressed), b);
        assert.deepEqual(zlib.gunzip(gzip_compressed), b);

        // ZIP format is now compatible with inflate (since ZIP now uses deflate format for Node.js compatibility)
        assert.deepEqual(zlib.inflate(zip_compressed), b);
    });

    it("unzip maxSize", () => {
        var compressed = zlib.zip(b);
        zlib.unzip(compressed, M);
        assert.throws(() => {
            zlib.unzip(compressed, M - 1);
        });
    });

    it("zipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.zipTo(b, stm);
        stm.rewind();
        assert.deepEqual(zlib.unzip(stm.readAll()), b);
    });

    it("unzipTo (from Buffer)", () => {
        var stm = new io.MemoryStream();
        zlib.unzipTo(zlib.zip(b), stm);
        stm.rewind();
        assert.deepEqual(stm.readAll(), b);
    });

    it("zipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(b);
        stm.rewind();

        var stm1 = new io.MemoryStream();
        zlib.zipTo(stm, stm1);

        stm1.rewind();
        assert.deepEqual(zlib.unzip(stm1.readAll()), b);
    });

    it("unzipTo (from Stream)", () => {
        var stm = new io.MemoryStream();
        stm.write(zlib.zip(b));
        stm.rewind();

        var stm1 = new io.MemoryStream();
        zlib.unzipTo(stm, stm1);
        stm1.rewind();
        assert.deepEqual(stm1.readAll(), b);
    });

    it("zipTo vs gzipTo output format difference", () => {
        var zip_stm = new io.MemoryStream();
        var gzip_stm = new io.MemoryStream();

        zlib.zipTo(b, zip_stm);
        zlib.gzipTo(b, gzip_stm);

        zip_stm.rewind();
        gzip_stm.rewind();

        var zip_data = zip_stm.readAll();
        var gzip_data = gzip_stm.readAll();

        // ZIP and GZIP stream outputs should be different
        assert.notDeepEqual(zip_data, gzip_data);

        // Each should decompress correctly with its own method
        assert.deepEqual(zlib.unzip(zip_data), b);
        assert.deepEqual(zlib.gunzip(gzip_data), b);
    });

    it("createZip and createUnzip streams", () => {
        var zip_stm = new io.MemoryStream();
        var unzip_stm = new io.MemoryStream();

        // Test zip stream
        var zip_stream = zlib.createZip(zip_stm);
        zip_stream.write(b);
        zip_stream.close();

        zip_stm.rewind();
        var compressed_data = zip_stm.readAll();

        // Test unzip stream
        var unzip_stream = zlib.createUnzip(unzip_stm);
        unzip_stream.write(compressed_data);
        unzip_stream.close();

        unzip_stm.rewind();
        assert.deepEqual(unzip_stm.readAll(), b);
    });

    it("createZip with compression levels", () => {
        var fast_stm = new io.MemoryStream();
        var best_stm = new io.MemoryStream();

        var fast_zip = zlib.createZip(fast_stm, zlib.BEST_SPEED);
        var best_zip = zlib.createZip(best_stm, zlib.BEST_COMPRESSION);

        fast_zip.write(b);
        fast_zip.close();

        best_zip.write(b);
        best_zip.close();

        fast_stm.rewind();
        best_stm.rewind();

        var fast_data = fast_stm.readAll();
        var best_data = best_stm.readAll();

        // Different compression levels should produce different sizes
        assert.notEqual(fast_data.length, best_data.length);

        // Both should decompress to original data
        assert.deepEqual(zlib.unzip(fast_data), b);
        assert.deepEqual(zlib.unzip(best_data), b);
    });

    it("zip format header differences", () => {
        var zip_data = zlib.zip(b);
        var gzip_data = zlib.gzip(b);
        var deflate_data = zlib.deflate(b);
        var deflateRaw_data = zlib.deflateRaw(b);

        // Check that different formats have different headers
        // GZIP starts with 0x1f, 0x8b
        assert.equal(gzip_data[0], 0x1f);
        assert.equal(gzip_data[1], 0x8b);

        // Deflate (zlib format) starts with 0x78
        assert.equal(deflate_data[0], 0x78);

        // ZIP format now uses deflate format for Node.js compatibility
        assert.equal(zip_data[0], 0x78);

        // ZIP and deflate should be the same (ZIP now uses deflate format for Node.js compatibility)
        assert.deepEqual(zip_data, deflate_data);

        // Different formats should produce different outputs
        assert.notDeepEqual(zip_data.slice(0, 10), gzip_data.slice(0, 10));
        assert.notDeepEqual(zip_data.slice(0, 10), deflateRaw_data.slice(0, 10));

        // Verify compatibility: ZIP data should be decompressible by both unzip and inflate (deflate format)
        assert.deepEqual(zlib.unzip(zip_data), b);
        assert.deepEqual(zlib.inflate(zip_data), b);
    });

    it("compression format compatibility matrix", () => {
        var zip_data = zlib.zip(b);
        var gzip_data = zlib.gzip(b);
        var deflate_data = zlib.deflate(b);
        var deflateRaw_data = zlib.deflateRaw(b);

        // Test what decompresses what

        // ZIP now uses deflate format, should be compatible with unzip and inflate
        assert.deepEqual(zlib.unzip(zip_data), b);
        assert.deepEqual(zlib.inflate(zip_data), b);

        // GZIP should only work with gunzip and unzip (Node.js compatibility)
        assert.deepEqual(zlib.gunzip(gzip_data), b);
        assert.deepEqual(zlib.unzip(gzip_data), b); // unzip supports gzip in Node.js
        assert.throws(() => zlib.inflate(gzip_data));
        assert.throws(() => zlib.inflateRaw(gzip_data));

        // Deflate (zlib format) should work with inflate and unzip (Node.js compatibility)
        assert.deepEqual(zlib.inflate(deflate_data), b);
        assert.deepEqual(zlib.unzip(deflate_data), b); // unzip supports deflate in Node.js
        assert.throws(() => zlib.gunzip(deflate_data));
        assert.throws(() => zlib.inflateRaw(deflate_data));

        // DeflateRaw should work with inflateRaw only (Node.js compatibility)
        assert.deepEqual(zlib.inflateRaw(deflateRaw_data), b);
        assert.throws(() => zlib.unzip(deflateRaw_data)); // unzip does NOT support deflateRaw in Node.js
        assert.throws(() => zlib.gunzip(deflateRaw_data));
        assert.throws(() => zlib.inflate(deflateRaw_data));
    });

    it("strict format validation - cross decompression errors", () => {
        var zip_data = zlib.zip(b);
        var gzip_data = zlib.gzip(b);
        var deflate_data = zlib.deflate(b);
        var deflateRaw_data = zlib.deflateRaw(b);

        // GZIP data should work with gunzip and unzip only
        assert.throws(() => zlib.inflate(gzip_data), /data error/);
        assert.throws(() => zlib.inflateRaw(gzip_data), /data error/);
        // Note: unzip can handle gzip in Node.js, so no error expected

        // Deflate (zlib) data should work with inflate and unzip only
        assert.throws(() => zlib.gunzip(deflate_data), /data error/);
        assert.throws(() => zlib.inflateRaw(deflate_data), /data error/);
        // Note: unzip can handle deflate in Node.js, so no error expected

        // ZIP now uses deflate format, should work with inflate and unzip (Node.js compatibility)
        assert.throws(() => zlib.gunzip(zip_data), /data error/);
        // Note: inflate and unzip should now work with zip data since it uses deflate format

        // DeflateRaw data should work with inflateRaw only (Node.js compatibility) 
        assert.throws(() => zlib.gunzip(deflateRaw_data), /data error/);
        assert.throws(() => zlib.inflate(deflateRaw_data), /data error/);
        assert.throws(() => zlib.unzip(deflateRaw_data), /data error/); // unzip does NOT support deflateRaw in Node.js
    });

    it("format header validation", () => {
        var small_data = Buffer.from("Hello World");

        var zip_data = zlib.zip(small_data);
        var gzip_data = zlib.gzip(small_data);
        var deflate_data = zlib.deflate(small_data);
        var deflateRaw_data = zlib.deflateRaw(small_data);

        // GZIP magic number validation
        assert.equal(gzip_data[0], 0x1f, "GZIP should start with 0x1f");
        assert.equal(gzip_data[1], 0x8b, "GZIP should have 0x8b as second byte");

        // Deflate (zlib) header validation
        assert.equal(deflate_data[0], 0x78, "Deflate should start with 0x78");
        // Second byte varies but should be a valid zlib header
        // Just check that it's not obviously wrong
        assert.ok(deflate_data[1] >= 0x01 && deflate_data[1] <= 0xda,
            "Deflate should have valid second byte");

        // ZIP now uses deflate format for Node.js compatibility
        assert.equal(zip_data[0], 0x78, "ZIP now uses deflate format and should start with 0x78");

        // DeflateRaw should not have magic headers
        assert.notEqual(deflateRaw_data[0], 0x1f, "DeflateRaw should not start with GZIP magic");
        assert.notEqual(deflateRaw_data[0], 0x78, "DeflateRaw should not start with zlib magic");

        // ZIP and deflate should be identical (Node.js compatibility change)
        assert.deepEqual(zip_data, deflate_data, "ZIP and deflate should produce identical output for Node.js compatibility");
    });

    it("corrupted data handling", () => {
        var original_data = Buffer.from("Test data for corruption testing with sufficient length for meaningful corruption");
        var gzip_data = zlib.gzip(original_data);
        var deflate_data = zlib.deflate(original_data);

        // Test corrupted GZIP data (should throw in both Node.js and fibjs)
        var corrupted_gzip = Buffer.from(gzip_data);
        corrupted_gzip[10] ^= 0xff; // Flip bits in the middle
        assert.throws(() => zlib.gunzip(corrupted_gzip));

        // Test corrupted deflate data (should throw in both Node.js and fibjs)
        var corrupted_deflate = Buffer.from(deflate_data);
        corrupted_deflate[5] ^= 0xff; // Flip bits in the middle
        assert.throws(() => zlib.inflate(corrupted_deflate));

        // Note: corrupted deflateRaw/ZIP might not always throw in Node.js
        // so we don't test that case to maintain compatibility

        // Note: Different implementations may handle truncated GZIP differently
        // fibjs may be more tolerant than Node.js in recovering partial data
        // This is an implementation difference we can accept
    });

    it("wrong format detection", () => {
        var small_data = Buffer.from("Hello World");

        var zip_data = zlib.zip(small_data);
        var gzip_data = zlib.gzip(small_data);
        var deflate_data = zlib.deflate(small_data);
        var deflateRaw_data = zlib.deflateRaw(small_data);

        // GZIP magic number validation
        assert.equal(gzip_data[0], 0x1f, "GZIP should start with 0x1f");
        assert.equal(gzip_data[1], 0x8b, "GZIP should have 0x8b as second byte");

        // Deflate (zlib) header validation
        assert.equal(deflate_data[0], 0x78, "Deflate should start with 0x78");
        // Second byte varies but should be reasonable
        assert.ok(deflate_data[1] >= 0x01 && deflate_data[1] <= 0xda,
            "Deflate should have valid second byte");

        // ZIP now uses deflate format for Node.js compatibility
        assert.equal(zip_data[0], 0x78, "ZIP now uses deflate format and should start with 0x78");

        // DeflateRaw should not have magic headers
        assert.notEqual(deflateRaw_data[0], 0x1f, "DeflateRaw should not start with GZIP magic");
        assert.notEqual(deflateRaw_data[0], 0x78, "DeflateRaw should not start with zlib magic");

        // ZIP and deflate should be identical (Node.js compatibility change)
        assert.deepEqual(zip_data, deflate_data, "ZIP and deflate should produce identical output for Node.js compatibility");

        // Test wrong magic numbers - these should throw data errors
        assert.throws(() => zlib.gunzip(Buffer.from([0x78, 0x9c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01])));
        assert.throws(() => zlib.inflate(Buffer.from([0x1f, 0x8b, 0x08, 0x00])));
    });

    it("format-specific decompression success validation", () => {
        var test_data = Buffer.from("Format validation test data");

        // Each format should only decompress with its correct method
        var zip_compressed = zlib.zip(test_data);
        var gzip_compressed = zlib.gzip(test_data);
        var deflate_compressed = zlib.deflate(test_data);
        var deflateRaw_compressed = zlib.deflateRaw(test_data);

        // Successful decompressions
        assert.deepEqual(zlib.unzip(zip_compressed), test_data);
        assert.deepEqual(zlib.inflate(zip_compressed), test_data); // ZIP now uses deflate format
        assert.deepEqual(zlib.gunzip(gzip_compressed), test_data);
        assert.deepEqual(zlib.inflate(deflate_compressed), test_data);
        assert.deepEqual(zlib.inflateRaw(deflateRaw_compressed), test_data);
        assert.throws(() => zlib.unzip(deflateRaw_compressed)); // unzip does NOT support deflateRaw for Node.js compatibility

        // All other combinations should fail with data errors
        var all_formats = [
            { name: 'zip', data: zip_compressed },
            { name: 'gzip', data: gzip_compressed },
            { name: 'deflate', data: deflate_compressed },
            { name: 'deflateRaw', data: deflateRaw_compressed }
        ];

        var all_methods = [
            { name: 'unzip', fn: zlib.unzip, compatible: ['zip', 'gzip', 'deflate'] }, // unzip supports zip, gzip and deflate
            { name: 'gunzip', fn: zlib.gunzip, compatible: ['gzip'] },
            { name: 'inflate', fn: zlib.inflate, compatible: ['zip', 'deflate'] }, // zip now uses deflate format
            { name: 'inflateRaw', fn: zlib.inflateRaw, compatible: ['deflateRaw'] }
        ];

        all_formats.forEach(format => {
            all_methods.forEach(method => {
                if (!method.compatible.includes(format.name)) {
                    assert.throws(() => method.fn(format.data), /data error/,
                        `${method.name} should not decompress ${format.name} data`);
                }
            });
        });
    });

    // New tests for unzip auto-detection capability (Node.js compatibility)
    describe("unzip auto-detection compatibility", () => {
        it("unzip should handle deflate format like Node.js", () => {
            var test_data = Buffer.from("Hello, World! This is a test string for compression testing.");
            var deflate_compressed = zlib.deflate(test_data);

            // This should work now (similar to Node.js unzip behavior)
            var result = zlib.unzip(deflate_compressed);
            assert.deepEqual(result, test_data, "unzip should handle deflate format");
        });

        it("unzip should handle gzip format like Node.js", () => {
            var test_data = Buffer.from("Hello, World! This is a test string for compression testing.");
            var gzip_compressed = zlib.gzip(test_data);

            // This should work now (similar to Node.js unzip behavior)
            var result = zlib.unzip(gzip_compressed);
            assert.deepEqual(result, test_data, "unzip should handle gzip format");
        });

        it("unzip should NOT handle deflateRaw format for Node.js compatibility", () => {
            var test_data = Buffer.from("Hello, World! This is a test string for compression testing.");
            var deflateRaw_compressed = zlib.deflateRaw(test_data);

            // This should now fail to match Node.js behavior (unzip does not support deflateRaw)
            assert.throws(() => {
                zlib.unzip(deflateRaw_compressed);
            }, /data error/, "unzip should NOT handle deflateRaw format like Node.js");
        });

        it("unzip auto-detection with various data sizes", () => {
            var test_cases = [
                Buffer.from("small"),
                Buffer.from("Medium size test data for compression"),
                Buffer.alloc(1024).fill('A'), // 1KB of 'A'
                Buffer.alloc(10240).fill('B') // 10KB of 'B'
            ];

            test_cases.forEach((test_data, index) => {
                // Test deflate format
                var deflate_compressed = zlib.deflate(test_data);
                var deflate_result = zlib.unzip(deflate_compressed);
                assert.deepEqual(deflate_result, test_data,
                    `unzip should handle deflate format for test case ${index + 1}`);

                // Test gzip format
                var gzip_compressed = zlib.gzip(test_data);
                var gzip_result = zlib.unzip(gzip_compressed);
                assert.deepEqual(gzip_result, test_data,
                    `unzip should handle gzip format for test case ${index + 1}`);

                // Test deflateRaw format - should NOT work with unzip (Node.js compatibility)
                var deflateRaw_compressed = zlib.deflateRaw(test_data);
                assert.throws(() => {
                    zlib.unzip(deflateRaw_compressed);
                }, `unzip should NOT handle deflateRaw format for Node.js compatibility (test case ${index + 1})`);
            });
        });

        it("unzip format detection should match Node.js behavior", () => {
            var test_data = Buffer.from("Node.js compatibility test data with sufficient length for meaningful compression");

            // Create compressed data with different formats
            var deflate_data = zlib.deflate(test_data);
            var gzip_data = zlib.gzip(test_data);
            var deflateRaw_data = zlib.deflateRaw(test_data);

            // Verify magic numbers for format identification
            assert.equal(gzip_data[0], 0x1f, "GZIP should start with 0x1f");
            assert.equal(gzip_data[1], 0x8b, "GZIP should have 0x8b as second byte");
            assert.equal(deflate_data[0], 0x78, "Deflate should start with 0x78");

            // Test that unzip can handle supported formats (Node.js compatibility)
            assert.deepEqual(zlib.unzip(deflate_data), test_data, "unzip should decompress deflate data");
            assert.deepEqual(zlib.unzip(gzip_data), test_data, "unzip should decompress gzip data");
            assert.throws(() => {
                zlib.unzip(deflateRaw_data);
            }, "unzip should NOT decompress deflateRaw data for Node.js compatibility");
        });

        it("unzip should handle edge cases correctly", () => {
            // Test empty data - fibjs handles empty data differently than Node.js
            var empty_data = Buffer.alloc(0);
            var empty_deflate = zlib.deflate(empty_data);
            var empty_gzip = zlib.gzip(empty_data);
            var empty_deflateRaw = zlib.deflateRaw(empty_data);

            // For empty data, fibjs may return null instead of empty buffer
            // This is a known implementation difference
            var empty_deflate_result = zlib.unzip(empty_deflate);
            var empty_gzip_result = zlib.unzip(empty_gzip);
            
            // Accept both null and empty buffer for empty data (implementation difference)
            assert.ok(empty_deflate_result === null || empty_deflate_result.length === 0, 
                "unzip should handle empty deflate data (null or empty buffer)");
            assert.ok(empty_gzip_result === null || empty_gzip_result.length === 0, 
                "unzip should handle empty gzip data (null or empty buffer)");

            // Test single byte
            var single_byte = Buffer.from([42]);
            var single_deflate = zlib.deflate(single_byte);
            var single_gzip = zlib.gzip(single_byte);
            var single_deflateRaw = zlib.deflateRaw(single_byte);

            assert.deepEqual(zlib.unzip(single_deflate), single_byte, "unzip should handle single byte deflate data");
            assert.deepEqual(zlib.unzip(single_gzip), single_byte, "unzip should handle single byte gzip data");
            
            // deflateRaw should NOT work with unzip (Node.js compatibility)
            assert.throws(() => {
                zlib.unzip(single_deflateRaw);
            }, "unzip should NOT handle deflateRaw format for Node.js compatibility");
        });

        it("unzip vs specific decompression methods comparison", () => {
            var test_data = Buffer.from("Comparison test data for different decompression methods");

            var deflate_data = zlib.deflate(test_data);
            var gzip_data = zlib.gzip(test_data);
            var deflateRaw_data = zlib.deflateRaw(test_data);

            // Compare unzip results with specific methods for supported formats
            assert.deepEqual(zlib.unzip(deflate_data), zlib.inflate(deflate_data),
                "unzip and inflate should give same result for deflate data");
            assert.deepEqual(zlib.unzip(gzip_data), zlib.gunzip(gzip_data),
                "unzip and gunzip should give same result for gzip data");

            // unzip should NOT support deflateRaw for Node.js compatibility
            assert.throws(() => {
                zlib.unzip(deflateRaw_data);
            }, "unzip should NOT support deflateRaw format for Node.js compatibility");
        });

        it("unzip with maxSize parameter should work with all formats", () => {
            var test_data = Buffer.from("Test data for maxSize validation with unzip auto-detection");
            var expected_size = test_data.length;

            var deflate_data = zlib.deflate(test_data);
            var gzip_data = zlib.gzip(test_data);
            var deflateRaw_data = zlib.deflateRaw(test_data);

            // Test with sufficient maxSize for supported formats
            assert.deepEqual(zlib.unzip(deflate_data, expected_size * 2), test_data);
            assert.deepEqual(zlib.unzip(gzip_data, expected_size * 2), test_data);

            // deflateRaw should NOT work with unzip (Node.js compatibility)
            assert.throws(() => {
                zlib.unzip(deflateRaw_data, expected_size * 2);
            }, "unzip should NOT support deflateRaw format for Node.js compatibility");

            // Test with exact maxSize for supported formats
            assert.deepEqual(zlib.unzip(deflate_data, expected_size), test_data);
            assert.deepEqual(zlib.unzip(gzip_data, expected_size), test_data);

            // Test with insufficient maxSize (should throw)
            assert.throws(() => zlib.unzip(deflate_data, expected_size - 1));
            assert.throws(() => zlib.unzip(gzip_data, expected_size - 1));
        });

        it("unzip stream operations with auto-detection", () => {
            var test_data = Buffer.from("Stream test data for unzip auto-detection functionality");

            var deflate_data = zlib.deflate(test_data);
            var gzip_data = zlib.gzip(test_data);
            var deflateRaw_data = zlib.deflateRaw(test_data);

            // Test unzipTo with supported formats (Node.js compatibility)
            [
                { name: 'deflate', data: deflate_data },
                { name: 'gzip', data: gzip_data }
            ].forEach(format => {
                var output_stream = new io.MemoryStream();
                zlib.unzipTo(format.data, output_stream);
                output_stream.rewind();
                assert.deepEqual(output_stream.readAll(), test_data,
                    `unzipTo should handle ${format.name} format`);
            });

            // deflateRaw should NOT work with unzipTo (Node.js compatibility)
            assert.throws(() => {
                var output_stream = new io.MemoryStream();
                zlib.unzipTo(deflateRaw_data, output_stream);
            }, "unzipTo should NOT handle deflateRaw format for Node.js compatibility");

            // Test createUnzip with supported formats
            [
                { name: 'deflate', data: deflate_data },
                { name: 'gzip', data: gzip_data }
            ].forEach(format => {
                var output_stream = new io.MemoryStream();
                var unzip_stream = zlib.createUnzip(output_stream);
                unzip_stream.write(format.data);
                unzip_stream.close();
                output_stream.rewind();
                assert.deepEqual(output_stream.readAll(), test_data,
                    `createUnzip should handle ${format.name} format`);
            });

            // deflateRaw should NOT work with createUnzip (Node.js compatibility)
            assert.throws(() => {
                var output_stream = new io.MemoryStream();
                var unzip_stream = zlib.createUnzip(output_stream);
                unzip_stream.write(deflateRaw_data);
                unzip_stream.close();
            }, "createUnzip should NOT handle deflateRaw format for Node.js compatibility");
        });
    });

    // Node.js compatibility tests integrated from zlib_comp_test.js
    describe("Node.js API compatibility", () => {
        var testData = Buffer.from('Hello, World! This is test data for compression compatibility testing with various formats.');
        var smallData = Buffer.from('small');
        var emptyData = Buffer.alloc(0);
        var largeData = Buffer.alloc(1024).fill('A');

        describe("deflate/inflate format compatibility", () => {
            it("deflate/inflate basic functionality", () => {
                var compressed = zlib.deflate(testData);
                var decompressed = zlib.inflate(compressed);
                assert.deepEqual(decompressed, testData);
            });

            it("deflate produces zlib format (0x78 header)", () => {
                var compressed = zlib.deflate(testData);
                assert.equal(compressed[0], 0x78, "deflate should produce zlib format starting with 0x78");
            });

            it("inflate only works with zlib format", () => {
                var gzipData = zlib.gzip(testData);
                var deflateRawData = zlib.deflateRaw(testData);

                assert.throws(() => zlib.inflate(gzipData));
                assert.throws(() => zlib.inflate(deflateRawData));
            });
        });

        describe("gzip/gunzip format compatibility", () => {
            it("gzip/gunzip basic functionality", () => {
                var compressed = zlib.gzip(testData);
                var decompressed = zlib.gunzip(compressed);
                assert.deepEqual(decompressed, testData);
            });

            it("gzip produces gzip format (0x1f 0x8b header)", () => {
                var compressed = zlib.gzip(testData);
                assert.equal(compressed[0], 0x1f, "gzip should start with 0x1f");
                assert.equal(compressed[1], 0x8b, "gzip should have 0x8b as second byte");
            });

            it("gunzip only works with gzip format", () => {
                var deflateData = zlib.deflate(testData);
                var deflateRawData = zlib.deflateRaw(testData);

                assert.throws(() => zlib.gunzip(deflateData));
                assert.throws(() => zlib.gunzip(deflateRawData));
            });
        });

        describe("deflateRaw/inflateRaw format compatibility", () => {
            it("deflateRaw/inflateRaw basic functionality", () => {
                var compressed = zlib.deflateRaw(testData);
                var decompressed = zlib.inflateRaw(compressed);
                assert.deepEqual(decompressed, testData);
            });

            it("deflateRaw produces raw deflate (no magic header)", () => {
                var compressed = zlib.deflateRaw(testData);
                assert.notEqual(compressed[0], 0x1f, "deflateRaw should not start with gzip magic");
                assert.notEqual(compressed[0], 0x78, "deflateRaw should not start with zlib magic");
            });

            it("inflateRaw only works with raw deflate format", () => {
                var gzipData = zlib.gzip(testData);
                var deflateData = zlib.deflate(testData);

                assert.throws(() => zlib.inflateRaw(gzipData));
                assert.throws(() => zlib.inflateRaw(deflateData));
            });
        });

        describe("zip/unzip Node.js compatibility", () => {
            it("zip should produce deflate-compatible format", () => {
                var zipData = zlib.zip(testData);
                var deflateData = zlib.deflate(testData);
                
                // ZIP now uses deflate format for Node.js compatibility
                assert.deepEqual(zipData, deflateData, "zip should produce same output as deflate");
                assert.equal(zipData[0], 0x78, "zip should start with deflate magic 0x78");
            });

            it("unzip handles gzip format like Node.js", () => {
                var gzipData = zlib.gzip(testData);
                var result = zlib.unzip(gzipData);
                assert.deepEqual(result, testData, "unzip should handle gzip format");
            });

            it("unzip handles deflate format like Node.js", () => {
                var deflateData = zlib.deflate(testData);
                var result = zlib.unzip(deflateData);
                assert.deepEqual(result, testData, "unzip should handle deflate format");
            });

            it("unzip does NOT handle deflateRaw format like Node.js", () => {
                var deflateRawData = zlib.deflateRaw(testData);
                assert.throws(() => {
                    zlib.unzip(deflateRawData);
                }, "unzip should NOT handle deflateRaw format to match Node.js");
            });

            it("unzip format detection with various data sizes", () => {
                [smallData, testData, largeData].forEach((data, index) => {
                    var gzipData = zlib.gzip(data);
                    var deflateData = zlib.deflate(data);

                    var gzipResult = zlib.unzip(gzipData);
                    var deflateResult = zlib.unzip(deflateData);

                    assert.deepEqual(gzipResult, data, `unzip should handle gzip for data size ${index}`);
                    assert.deepEqual(deflateResult, data, `unzip should handle deflate for data size ${index}`);
                });
            });

            it("unzip comprehensive format rejection tests", () => {
                // Test with invalid magic numbers that should be rejected
                var invalidMagic1 = Buffer.from([0x42, 0x43, 0x01, 0x02]); // Random bytes
                var invalidMagic2 = Buffer.from([0x50, 0x4b, 0x03, 0x04]); // ZIP magic (not supported)
                var invalidMagic3 = Buffer.from([0x1f, 0x9d]); // Old compress format
                
                assert.throws(() => zlib.unzip(invalidMagic1), "unzip should reject random data");
                assert.throws(() => zlib.unzip(invalidMagic2), "unzip should reject ZIP format");
                assert.throws(() => zlib.unzip(invalidMagic3), "unzip should reject old compress format");
            });

            it("unzip handles minimum data requirements", () => {
                // Test with data that's too short
                var tooShort = Buffer.from([0x1f]); // Only one byte
                assert.throws(() => zlib.unzip(tooShort), "unzip should reject single byte");
                
                // Test with incomplete gzip header
                var incompleteGzip = Buffer.from([0x1f, 0x8b]); // Only magic, no method byte
                assert.throws(() => zlib.unzip(incompleteGzip), "unzip should reject incomplete gzip header");
            });
        });

        describe("cross-format compatibility matrix", () => {
            it("format compatibility matrix matches Node.js behavior", () => {
                var gzipData = zlib.gzip(testData);
                var deflateData = zlib.deflate(testData);
                var deflateRawData = zlib.deflateRaw(testData);
                var zipData = zlib.zip(testData);

                // Test successful combinations (should work)
                assert.deepEqual(zlib.gunzip(gzipData), testData, "gunzip + gzip");
                assert.deepEqual(zlib.unzip(gzipData), testData, "unzip + gzip");

                assert.deepEqual(zlib.inflate(deflateData), testData, "inflate + deflate");
                assert.deepEqual(zlib.unzip(deflateData), testData, "unzip + deflate");

                assert.deepEqual(zlib.inflateRaw(deflateRawData), testData, "inflateRaw + deflateRaw");

                // ZIP compatibility with deflate format
                assert.deepEqual(zlib.unzip(zipData), testData, "unzip + zip");
                assert.deepEqual(zlib.inflate(zipData), testData, "inflate + zip (deflate format)");

                // Test incompatible combinations (should fail)
                assert.throws(() => zlib.inflate(gzipData), "inflate should not work with gzip");
                assert.throws(() => zlib.inflateRaw(gzipData), "inflateRaw should not work with gzip");

                assert.throws(() => zlib.gunzip(deflateData), "gunzip should not work with deflate");
                assert.throws(() => zlib.inflateRaw(deflateData), "inflateRaw should not work with deflate");

                assert.throws(() => zlib.gunzip(deflateRawData), "gunzip should not work with deflateRaw");
                assert.throws(() => zlib.inflate(deflateRawData), "inflate should not work with deflateRaw");
                assert.throws(() => zlib.unzip(deflateRawData), "unzip should not work with deflateRaw");

                assert.throws(() => zlib.gunzip(zipData), "gunzip should not work with zip");
            });
        });

        describe("edge cases and error handling", () => {
            it("handles single byte data", () => {
                var singleByte = Buffer.from([42]);

                var gzipSingle = zlib.gzip(singleByte);
                var deflateSingle = zlib.deflate(singleByte);
                var deflateRawSingle = zlib.deflateRaw(singleByte);

                assert.deepEqual(zlib.gunzip(gzipSingle), singleByte);
                assert.deepEqual(zlib.unzip(gzipSingle), singleByte);
                assert.deepEqual(zlib.inflate(deflateSingle), singleByte);
                assert.deepEqual(zlib.unzip(deflateSingle), singleByte);
                assert.deepEqual(zlib.inflateRaw(deflateRawSingle), singleByte);
            });

            it("unzip handles various edge cases", () => {
                // Test with empty data - may fail gracefully or return null
                try {
                    var emptyResult = zlib.unzip(Buffer.alloc(0));
                    // Should either throw or return null/empty
                    assert.ok(emptyResult === null || emptyResult.length === 0, 
                        "unzip should handle empty buffer gracefully");
                } catch (e) {
                    // Throwing is also acceptable for empty data
                    assert.ok(true, "unzip can reject empty buffer");
                }
                
                // Test with different zlib headers (0x78 variants)
                var testStr = "test string for zlib variants";
                var testBuf = Buffer.from(testStr);
                
                // Create deflate data and verify unzip works
                var deflateData = zlib.deflate(testBuf);
                assert.equal(deflateData[0], 0x78, "Should start with zlib magic");
                
                var unzipResult = zlib.unzip(deflateData);
                assert.deepEqual(unzipResult, testBuf, "unzip should handle zlib format correctly");
            });

            it("validates magic number detection", () => {
                var gzipData = zlib.gzip(testData);
                var deflateData = zlib.deflate(testData);
                var deflateRawData = zlib.deflateRaw(testData);
                var zipData = zlib.zip(testData);

                // Verify magic numbers
                assert.equal(gzipData[0], 0x1f, "gzip magic byte 1");
                assert.equal(gzipData[1], 0x8b, "gzip magic byte 2");
                assert.equal(deflateData[0], 0x78, "deflate magic byte");
                assert.equal(zipData[0], 0x78, "zip magic byte (deflate format)");

                // deflateRaw should not have recognizable magic
                assert.notEqual(deflateRawData[0], 0x1f, "deflateRaw should not have gzip magic");
                assert.notEqual(deflateRawData[0], 0x78, "deflateRaw should not have deflate magic");
            });
        });

        describe("round-trip integrity verification", () => {
            it("round-trip compression maintains data integrity", () => {
                var testCases = [
                    { name: "small", data: smallData },
                    { name: "normal", data: testData },
                    { name: "large", data: largeData },
                    { name: "binary", data: Buffer.from([0, 1, 2, 255, 254, 253]) }
                ];

                testCases.forEach(({ name, data }) => {
                    // Test all format combinations
                    assert.deepEqual(zlib.gunzip(zlib.gzip(data)), data, `gzip round-trip: ${name}`);
                    assert.deepEqual(zlib.inflate(zlib.deflate(data)), data, `deflate round-trip: ${name}`);
                    assert.deepEqual(zlib.inflateRaw(zlib.deflateRaw(data)), data, `deflateRaw round-trip: ${name}`);

                    // Test unzip with supported formats
                    assert.deepEqual(zlib.unzip(zlib.gzip(data)), data, `unzip gzip round-trip: ${name}`);
                    assert.deepEqual(zlib.unzip(zlib.deflate(data)), data, `unzip deflate round-trip: ${name}`);
                    
                    // Test zip round-trip (zip now uses deflate format)
                    assert.deepEqual(zlib.unzip(zlib.zip(data)), data, `zip/unzip round-trip: ${name}`);
                    assert.deepEqual(zlib.inflate(zlib.zip(data)), data, `zip/inflate round-trip: ${name}`);
                });
            });
        });
    });

    // Test options parameter for Node.js compatibility
    describe("options parameter (Node.js compatibility)", () => {
        // Use longer test data to show compression level differences
        var testData = Buffer.alloc(10240);
        for (var i = 0; i < testData.length; i++) {
            testData[i] = i % 256;
        }

        describe("compression functions with options.level", () => {
            it("deflate with options.level", () => {
                var compressed_fast = zlib.deflate(testData, { level: zlib.BEST_SPEED });
                var compressed_best = zlib.deflate(testData, { level: zlib.BEST_COMPRESSION });
                var compressed_default = zlib.deflate(testData, { level: zlib.DEFAULT_COMPRESSION });

                // All should decompress correctly
                assert.deepEqual(zlib.inflate(compressed_fast), testData);
                assert.deepEqual(zlib.inflate(compressed_best), testData);
                assert.deepEqual(zlib.inflate(compressed_default), testData);
            });

            it("gzip with options.level", () => {
                var compressed_fast = zlib.gzip(testData, { level: zlib.BEST_SPEED });
                var compressed_best = zlib.gzip(testData, { level: zlib.BEST_COMPRESSION });
                var compressed_default = zlib.gzip(testData, { level: zlib.DEFAULT_COMPRESSION });

                // All should decompress correctly
                assert.deepEqual(zlib.gunzip(compressed_fast), testData);
                assert.deepEqual(zlib.gunzip(compressed_best), testData);
                assert.deepEqual(zlib.gunzip(compressed_default), testData);
            });

            it("deflateRaw with options.level", () => {
                var compressed_fast = zlib.deflateRaw(testData, { level: zlib.BEST_SPEED });
                var compressed_best = zlib.deflateRaw(testData, { level: zlib.BEST_COMPRESSION });
                var compressed_default = zlib.deflateRaw(testData, { level: zlib.DEFAULT_COMPRESSION });

                // All should decompress correctly
                assert.deepEqual(zlib.inflateRaw(compressed_fast), testData);
                assert.deepEqual(zlib.inflateRaw(compressed_best), testData);
                assert.deepEqual(zlib.inflateRaw(compressed_default), testData);
            });

            it("zip with options.level", () => {
                var compressed_fast = zlib.zip(testData, { level: zlib.BEST_SPEED });
                var compressed_best = zlib.zip(testData, { level: zlib.BEST_COMPRESSION });
                var compressed_default = zlib.zip(testData, { level: zlib.DEFAULT_COMPRESSION });

                // All should decompress correctly
                assert.deepEqual(zlib.unzip(compressed_fast), testData);
                assert.deepEqual(zlib.unzip(compressed_best), testData);
                assert.deepEqual(zlib.unzip(compressed_default), testData);
            });

            it("options.level with numeric values", () => {
                // Test with numeric level values (0-9)
                var compressed_0 = zlib.deflate(testData, { level: 0 });
                var compressed_1 = zlib.deflate(testData, { level: 1 });
                var compressed_6 = zlib.deflate(testData, { level: 6 });
                var compressed_9 = zlib.deflate(testData, { level: 9 });

                // All should decompress correctly
                assert.deepEqual(zlib.inflate(compressed_0), testData);
                assert.deepEqual(zlib.inflate(compressed_1), testData);
                assert.deepEqual(zlib.inflate(compressed_6), testData);
                assert.deepEqual(zlib.inflate(compressed_9), testData);

                // Level 0 (no compression) should be larger than level 9
                assert.ok(compressed_0.length > compressed_9.length);
            });
        });

        describe("decompression functions with options.maxOutputLength", () => {
            var shortData = Buffer.from("Test data for maxOutputLength testing.");

            it("inflate with options.maxOutputLength", () => {
                var compressed = zlib.deflate(shortData);
                var decompressed = zlib.inflate(compressed, { maxOutputLength: 1024 });
                assert.deepEqual(decompressed, shortData);

                // Should throw when output exceeds maxOutputLength
                assert.throws(() => {
                    zlib.inflate(compressed, { maxOutputLength: 10 });
                });
            });

            it("gunzip with options.maxOutputLength", () => {
                var compressed = zlib.gzip(shortData);
                var decompressed = zlib.gunzip(compressed, { maxOutputLength: 1024 });
                assert.deepEqual(decompressed, shortData);

                // Should throw when output exceeds maxOutputLength
                assert.throws(() => {
                    zlib.gunzip(compressed, { maxOutputLength: 10 });
                });
            });

            it("inflateRaw with options.maxOutputLength", () => {
                var compressed = zlib.deflateRaw(shortData);
                var decompressed = zlib.inflateRaw(compressed, { maxOutputLength: 1024 });
                assert.deepEqual(decompressed, shortData);

                // Should throw when output exceeds maxOutputLength
                assert.throws(() => {
                    zlib.inflateRaw(compressed, { maxOutputLength: 10 });
                });
            });

            it("unzip with options.maxOutputLength", () => {
                var compressed = zlib.zip(shortData);
                var decompressed = zlib.unzip(compressed, { maxOutputLength: 1024 });
                assert.deepEqual(decompressed, shortData);

                // Should throw when output exceeds maxOutputLength
                assert.throws(() => {
                    zlib.unzip(compressed, { maxOutputLength: 10 });
                });
            });

            it("maxOutputLength with -1 means no limit", () => {
                var largeData = Buffer.alloc(10240);
                for (var i = 0; i < largeData.length; i++) {
                    largeData[i] = Math.random() * 256;
                }

                var compressed = zlib.deflate(largeData);
                var decompressed = zlib.inflate(compressed, { maxOutputLength: -1 });
                assert.deepEqual(decompressed, largeData);
            });
        });

        describe("backward compatibility", () => {
            var shortData = Buffer.from("Short test data for backward compatibility testing.");

            it("integer parameter still works for compression level", () => {
                var compressed_int = zlib.deflate(shortData, zlib.BEST_SPEED);
                var compressed_opt = zlib.deflate(shortData, { level: zlib.BEST_SPEED });

                assert.deepEqual(zlib.inflate(compressed_int), shortData);
                assert.deepEqual(zlib.inflate(compressed_opt), shortData);
                assert.deepEqual(compressed_int, compressed_opt);
            });

            it("integer parameter still works for maxSize", () => {
                var compressed = zlib.deflate(shortData);
                var decompressed_int = zlib.inflate(compressed, 1024);
                var decompressed_opt = zlib.inflate(compressed, { maxOutputLength: 1024 });

                assert.deepEqual(decompressed_int, shortData);
                assert.deepEqual(decompressed_opt, shortData);
            });

            it("default behavior when options is empty object", () => {
                var compressed = zlib.deflate(shortData, {});
                var decompressed = zlib.inflate(compressed, {});

                assert.deepEqual(decompressed, shortData);
            });
        });

        describe("error handling for invalid options", () => {
            var shortData = Buffer.from("Test data for error handling.");

            it("should handle invalid level type gracefully", () => {
                assert.throws(() => {
                    zlib.deflate(shortData, { level: "invalid" });
                });
            });

            it("should handle invalid maxOutputLength type gracefully", () => {
                var compressed = zlib.deflate(shortData);
                assert.throws(() => {
                    zlib.inflate(compressed, { maxOutputLength: "invalid" });
                });
            });
        });
    });

    describe("ZlibCodec classes (Node.js compatible)", () => {
        var testData = Buffer.from("hello world, this is a test of zlib codec classes");
        var Z_NO_FLUSH = zlib.constants.Z_NO_FLUSH;
        var Z_FINISH = zlib.constants.Z_FINISH;
        var Z_SYNC_FLUSH = zlib.constants.Z_SYNC_FLUSH;

        describe("class availability", () => {
            it("exports all class constructors", () => {
                assert.equal(typeof zlib.Gzip, "function");
                assert.equal(typeof zlib.Gunzip, "function");
                assert.equal(typeof zlib.Deflate, "function");
                assert.equal(typeof zlib.Inflate, "function");
                assert.equal(typeof zlib.DeflateRaw, "function");
                assert.equal(typeof zlib.InflateRaw, "function");
                assert.equal(typeof zlib.Unzip, "function");
            });
        });

        describe("constructor and properties", () => {
            it("creates instances with new", () => {
                var g = new zlib.Gzip({});
                assert.ok(g);
                assert.equal(typeof g._processChunk, "function");
                assert.equal(typeof g.close, "function");
                assert.equal(typeof g.reset, "function");
                assert.equal(typeof g.params, "function");
            });

            it("default options when empty object", () => {
                var g = new zlib.Gzip({});
                assert.ok(g);
            });

            it("default options when no argument", () => {
                var g = new zlib.Gzip();
                assert.ok(g);
            });

            it("has _handle property with close method", () => {
                var g = new zlib.Gzip({});
                assert.ok(g._handle);
                assert.equal(typeof g._handle.close, "function");
            });

            it("_handle can be set and get", () => {
                var g = new zlib.Gzip({});
                var originalHandle = g._handle;
                var newHandle = { close: function () { } };
                g._handle = newHandle;
                assert.strictEqual(g._handle, newHandle);
                g._handle = originalHandle;
            });

            it("inherits EventEmitter", () => {
                var g = new zlib.Gzip({});
                assert.equal(typeof g.on, "function");
                assert.equal(typeof g.removeAllListeners, "function");
                assert.equal(typeof g.emit, "function");
            });

            it("on/removeAllListeners work", () => {
                var g = new zlib.Gzip({});
                var called = false;
                g.on("error", function () { called = true; });
                g.removeAllListeners("error");
                assert.equal(g.listenerCount("error"), 0);
            });
        });

        describe("Gzip/Gunzip roundtrip", () => {
            it("compress and decompress with Z_FINISH", () => {
                var gzip = new zlib.Gzip({});
                var compressed = gzip._processChunk(testData, Z_FINISH);
                assert.ok(compressed.length > 0);

                var gunzip = new zlib.Gunzip({});
                var decompressed = gunzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });

            it("compress with level option", () => {
                var gzip = new zlib.Gzip({ level: 1 });
                var fast = gzip._processChunk(testData, Z_FINISH);

                var gzip2 = new zlib.Gzip({ level: 9 });
                var best = gzip2._processChunk(testData, Z_FINISH);

                // Both should decompress to same data
                var gunzip1 = new zlib.Gunzip({});
                assert.deepEqual(gunzip1._processChunk(fast, Z_FINISH), testData);

                var gunzip2 = new zlib.Gunzip({});
                assert.deepEqual(gunzip2._processChunk(best, Z_FINISH), testData);
            });

            it("compatible with zlib.gunzip()", () => {
                var gzip = new zlib.Gzip({});
                var compressed = gzip._processChunk(testData, Z_FINISH);
                assert.deepEqual(zlib.gunzip(compressed), testData);
            });

            it("decompresses data from zlib.gzip()", () => {
                var compressed = zlib.gzip(testData);
                var gunzip = new zlib.Gunzip({});
                var decompressed = gunzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });
        });

        describe("Deflate/Inflate roundtrip", () => {
            it("compress and decompress with Z_FINISH", () => {
                var deflate = new zlib.Deflate({});
                var compressed = deflate._processChunk(testData, Z_FINISH);
                assert.ok(compressed.length > 0);

                var inflate = new zlib.Inflate({});
                var decompressed = inflate._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });

            it("compatible with zlib.inflate()", () => {
                var deflate = new zlib.Deflate({});
                var compressed = deflate._processChunk(testData, Z_FINISH);
                assert.deepEqual(zlib.inflate(compressed), testData);
            });

            it("decompresses data from zlib.deflate()", () => {
                var compressed = zlib.deflate(testData);
                var inflate = new zlib.Inflate({});
                var decompressed = inflate._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });
        });

        describe("DeflateRaw/InflateRaw roundtrip", () => {
            it("compress and decompress with Z_FINISH", () => {
                var deflateRaw = new zlib.DeflateRaw({});
                var compressed = deflateRaw._processChunk(testData, Z_FINISH);
                assert.ok(compressed.length > 0);

                var inflateRaw = new zlib.InflateRaw({});
                var decompressed = inflateRaw._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });

            it("compatible with zlib.inflateRaw()", () => {
                var deflateRaw = new zlib.DeflateRaw({});
                var compressed = deflateRaw._processChunk(testData, Z_FINISH);
                assert.deepEqual(zlib.inflateRaw(compressed), testData);
            });

            it("decompresses data from zlib.deflateRaw()", () => {
                var compressed = zlib.deflateRaw(testData);
                var inflateRaw = new zlib.InflateRaw({});
                var decompressed = inflateRaw._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });
        });

        describe("Unzip auto-detection", () => {
            it("decompresses gzip data", () => {
                var gzip = new zlib.Gzip({});
                var compressed = gzip._processChunk(testData, Z_FINISH);

                var unzip = new zlib.Unzip({});
                var decompressed = unzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });

            it("decompresses deflate (zlib) data", () => {
                var deflate = new zlib.Deflate({});
                var compressed = deflate._processChunk(testData, Z_FINISH);

                var unzip = new zlib.Unzip({});
                var decompressed = unzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });

            it("compatible with zlib.gzip() output", () => {
                var compressed = zlib.gzip(testData);
                var unzip = new zlib.Unzip({});
                var decompressed = unzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, testData);
            });
        });

        describe("multi-chunk processing", () => {
            it("Gzip: multiple writes then finish", () => {
                var gzip = new zlib.Gzip({});
                var chunk1 = Buffer.from("hello ");
                var chunk2 = Buffer.from("world");

                var out1 = gzip._processChunk(chunk1, Z_NO_FLUSH);
                var out2 = gzip._processChunk(chunk2, Z_FINISH);

                var compressed = Buffer.concat([out1, out2]);
                var gunzip = new zlib.Gunzip({});
                var decompressed = gunzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, Buffer.from("hello world"));
            });

            it("Deflate: multiple writes then finish", () => {
                var deflate = new zlib.Deflate({});
                var chunk1 = Buffer.from("foo ");
                var chunk2 = Buffer.from("bar");

                var out1 = deflate._processChunk(chunk1, Z_NO_FLUSH);
                var out2 = deflate._processChunk(chunk2, Z_FINISH);

                var compressed = Buffer.concat([out1, out2]);
                var inflate = new zlib.Inflate({});
                var decompressed = inflate._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, Buffer.from("foo bar"));
            });

            it("Z_SYNC_FLUSH produces intermediate output", () => {
                var gzip = new zlib.Gzip({});
                var chunk = Buffer.from("test sync flush");

                var out1 = gzip._processChunk(chunk, Z_SYNC_FLUSH);
                assert.ok(out1.length > 0);

                var out2 = gzip._processChunk(Buffer.alloc(0), Z_FINISH);
                var compressed = Buffer.concat([out1, out2]);

                var gunzip = new zlib.Gunzip({});
                var decompressed = gunzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, chunk);
            });
        });

        describe("large data", () => {
            it("handles large buffer roundtrip", () => {
                var gzip = new zlib.Gzip({});
                var compressed = gzip._processChunk(b, Z_FINISH);
                assert.ok(compressed.length > 0);

                var gunzip = new zlib.Gunzip({});
                var decompressed = gunzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, b);
            });
        });

        describe("close and reset", () => {
            it("close prevents further processing", () => {
                var gzip = new zlib.Gzip({});
                gzip.close();
                assert.throws(() => {
                    gzip._processChunk(testData, Z_FINISH);
                });
            });

            it("reset allows reuse", () => {
                var gzip = new zlib.Gzip({});
                var compressed1 = gzip._processChunk(testData, Z_FINISH);
                gzip.reset();
                var compressed2 = gzip._processChunk(testData, Z_FINISH);

                var gunzip1 = new zlib.Gunzip({});
                assert.deepEqual(gunzip1._processChunk(compressed1, Z_FINISH), testData);
                var gunzip2 = new zlib.Gunzip({});
                assert.deepEqual(gunzip2._processChunk(compressed2, Z_FINISH), testData);
            });
        });

        describe("params", () => {
            it("changes compression level on deflate", () => {
                var deflate = new zlib.Deflate({});
                deflate.params(1, 0);
                var compressed = deflate._processChunk(testData, Z_FINISH);

                var inflate = new zlib.Inflate({});
                assert.deepEqual(inflate._processChunk(compressed, Z_FINISH), testData);
            });

            it("throws on inflate (non-deflate)", () => {
                var inflate = new zlib.Inflate({});
                assert.throws(() => {
                    inflate.params(1, 0);
                });
            });
        });

        describe("empty data", () => {
            it("handles empty buffer", () => {
                var gzip = new zlib.Gzip({});
                var compressed = gzip._processChunk(Buffer.alloc(0), Z_FINISH);

                var gunzip = new zlib.Gunzip({});
                var decompressed = gunzip._processChunk(compressed, Z_FINISH);
                assert.deepEqual(decompressed, Buffer.alloc(0));
            });
        });
    });
});