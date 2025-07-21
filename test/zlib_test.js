var test = require("test");
test.setup();

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
        assert.throws(() => {
            zlib.gunzip(zip_compressed);
        });

        assert.throws(() => {
            zlib.unzip(gzip_compressed);
        });

        // But each format should decompress with its own method
        assert.deepEqual(zlib.unzip(zip_compressed), b);
        assert.deepEqual(zlib.gunzip(gzip_compressed), b);

        // ZIP format should also be compatible with inflateRaw (since ZIP uses deflateRaw internally)
        assert.deepEqual(zlib.inflateRaw(zip_compressed), b);
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

        // ZIP format should be different from GZIP and Deflate
        assert.notEqual(zip_data[0], 0x1f);
        assert.notEqual(zip_data[1], 0x8b);
        assert.notEqual(zip_data[0], 0x78);

        // ZIP and deflateRaw should be the same (ZIP uses deflateRaw internally)
        assert.deepEqual(zip_data, deflateRaw_data);

        // Different formats should produce different outputs
        assert.notDeepEqual(zip_data.slice(0, 10), gzip_data.slice(0, 10));
        assert.notDeepEqual(zip_data.slice(0, 10), deflate_data.slice(0, 10));

        // Verify compatibility: ZIP data should be decompressible by both unzip and inflateRaw
        assert.deepEqual(zlib.unzip(zip_data), b);
        assert.deepEqual(zlib.inflateRaw(zip_data), b);
    });

    it("compression format compatibility matrix", () => {
        var zip_data = zlib.zip(b);
        var gzip_data = zlib.gzip(b);
        var deflate_data = zlib.deflate(b);
        var deflateRaw_data = zlib.deflateRaw(b);

        // Test what decompresses what

        // ZIP should be compatible with unzip and inflateRaw
        assert.deepEqual(zlib.unzip(zip_data), b);
        assert.deepEqual(zlib.inflateRaw(zip_data), b);

        // GZIP should only work with gunzip
        assert.deepEqual(zlib.gunzip(gzip_data), b);
        assert.throws(() => zlib.inflate(gzip_data));
        assert.throws(() => zlib.inflateRaw(gzip_data));
        assert.throws(() => zlib.unzip(gzip_data));

        // Deflate (zlib format) should only work with inflate
        assert.deepEqual(zlib.inflate(deflate_data), b);
        assert.throws(() => zlib.gunzip(deflate_data));
        assert.throws(() => zlib.inflateRaw(deflate_data));
        assert.throws(() => zlib.unzip(deflate_data));

        // DeflateRaw should work with both inflateRaw and unzip
        assert.deepEqual(zlib.inflateRaw(deflateRaw_data), b);
        assert.deepEqual(zlib.unzip(deflateRaw_data), b);
        assert.throws(() => zlib.gunzip(deflateRaw_data));
        assert.throws(() => zlib.inflate(deflateRaw_data));
    });

    it("strict format validation - cross decompression errors", () => {
        var zip_data = zlib.zip(b);
        var gzip_data = zlib.gzip(b);
        var deflate_data = zlib.deflate(b);
        var deflateRaw_data = zlib.deflateRaw(b);

        // Test all invalid cross-decompression combinations
        // Each should throw a "data error" exception

        // GZIP data should only work with gunzip
        assert.throws(() => zlib.inflate(gzip_data), /data error/);
        assert.throws(() => zlib.inflateRaw(gzip_data), /data error/);
        assert.throws(() => zlib.unzip(gzip_data), /data error/);

        // Deflate (zlib) data should only work with inflate
        assert.throws(() => zlib.gunzip(deflate_data), /data error/);
        assert.throws(() => zlib.inflateRaw(deflate_data), /data error/);
        assert.throws(() => zlib.unzip(deflate_data), /data error/);

        // ZIP data should only work with unzip and inflateRaw
        assert.throws(() => zlib.gunzip(zip_data), /data error/);
        assert.throws(() => zlib.inflate(zip_data), /data error/);

        // DeflateRaw data should only work with inflateRaw and unzip
        assert.throws(() => zlib.gunzip(deflateRaw_data), /data error/);
        assert.throws(() => zlib.inflate(deflateRaw_data), /data error/);
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

        // ZIP and deflateRaw should not have magic headers
        assert.notEqual(zip_data[0], 0x1f, "ZIP should not start with GZIP magic");
        assert.notEqual(zip_data[0], 0x78, "ZIP should not start with zlib magic");
        assert.notEqual(deflateRaw_data[0], 0x1f, "DeflateRaw should not start with GZIP magic");
        assert.notEqual(deflateRaw_data[0], 0x78, "DeflateRaw should not start with zlib magic");

        // ZIP and deflateRaw should be identical
        assert.deepEqual(zip_data, deflateRaw_data, "ZIP and deflateRaw should produce identical output");
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

        // ZIP and deflateRaw should not have magic headers
        assert.notEqual(zip_data[0], 0x1f, "ZIP should not start with GZIP magic");
        assert.notEqual(zip_data[0], 0x78, "ZIP should not start with zlib magic");
        assert.notEqual(deflateRaw_data[0], 0x1f, "DeflateRaw should not start with GZIP magic");
        assert.notEqual(deflateRaw_data[0], 0x78, "DeflateRaw should not start with zlib magic");

        // ZIP and deflateRaw should be identical
        assert.deepEqual(zip_data, deflateRaw_data, "ZIP and deflateRaw should produce identical output");

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
        assert.deepEqual(zlib.inflateRaw(zip_compressed), test_data); // ZIP = deflateRaw
        assert.deepEqual(zlib.gunzip(gzip_compressed), test_data);
        assert.deepEqual(zlib.inflate(deflate_compressed), test_data);
        assert.deepEqual(zlib.inflateRaw(deflateRaw_compressed), test_data);
        assert.deepEqual(zlib.unzip(deflateRaw_compressed), test_data); // unzip = inflateRaw

        // All other combinations should fail with data errors
        var all_formats = [
            { name: 'zip', data: zip_compressed },
            { name: 'gzip', data: gzip_compressed },
            { name: 'deflate', data: deflate_compressed },
            { name: 'deflateRaw', data: deflateRaw_compressed }
        ];

        var all_methods = [
            { name: 'unzip', fn: zlib.unzip, compatible: ['zip', 'deflateRaw'] },
            { name: 'gunzip', fn: zlib.gunzip, compatible: ['gzip'] },
            { name: 'inflate', fn: zlib.inflate, compatible: ['deflate'] },
            { name: 'inflateRaw', fn: zlib.inflateRaw, compatible: ['zip', 'deflateRaw'] }
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
});

