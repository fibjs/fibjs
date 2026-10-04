// String-argument compatibility for Buffer parameters.
//
// Every Buffer parameter used to accept a string through the implicit
// conversion in GetArgumentValue (utf8, or the API's own encoding). That
// implicit path is gone: every call below is part of the compatibility
// surface and must keep working through the explicit String overloads
// (worklist: plans/buffer-param-audit-2026-10-01.md).
//
// A case belongs here only when the old binary accepted it: run this suite
// once against a binary with the implicit conversion restored (see the plan)
// to confirm the surface, then it must stay green on the current binary.
// More modules are added tranche by tranche.

var { describe, it } = require('node:test');
var assert = require('assert');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

describe('Buffer string arguments (compat surface)', { skip: !isFibjs }, () => {
    const text = 'hello, world';

    it('zlib: sync forms accept string data', () => {
        var zlib = require('zlib');

        assert.equal(zlib.gunzipSync(zlib.gzipSync(text)).toString(), text);
        assert.equal(zlib.inflateSync(zlib.deflateSync(text)).toString(), text);
        assert.equal(zlib.inflateRawSync(zlib.deflateRawSync(text)).toString(), text);
        assert.equal(zlib.gunzipSync(zlib.gzipSync(text, { level: 1 })).toString(), text);
        assert.equal(zlib.inflateSync(zlib.deflateSync(text, { level: 1 })).toString(), text);
        assert.equal(zlib.inflateRawSync(zlib.deflateRawSync(text, { level: 1 })).toString(), text);
        assert.equal(zlib.unzipSync(zlib.zipSync(text)).toString(), text);
        assert.equal(zlib.unzipSync(zlib.zipSync(text, { level: 1 })).toString(), text);
    });

    it('zlib: async forms accept string data (sync invocation and callback)', () => {
        var zlib = require('zlib');
        var coroutine = require('coroutine');

        assert.equal(zlib.gunzip(zlib.gzip(text)).toString(), text);
        assert.equal(zlib.inflate(zlib.deflate(text)).toString(), text);
        assert.equal(zlib.inflateRaw(zlib.deflateRaw(text)).toString(), text);
        assert.equal(zlib.unzip(zlib.zip(text)).toString(), text);

        var cbData = null;
        zlib.gzip(text, (err, buf) => {
            cbData = buf;
        });

        for (var i = 0; i < 200 && !cbData; i++)
            coroutine.sleep(10);

        assert.ok(cbData, 'the callback form must deliver the compressed data');
        assert.equal(zlib.gunzipSync(cbData).toString(), text);
    });

    it('zlib: *To forms accept string data', () => {
        var zlib = require('zlib');
        var io = require('io');
        var stm;

        stm = new io.MemoryStream();
        zlib.deflateTo(text, stm);
        stm.rewind();
        assert.equal(zlib.inflateSync(stm.readAll()).toString(), text);

        stm = new io.MemoryStream();
        zlib.gzipTo(text, stm);
        stm.rewind();
        assert.equal(zlib.gunzipSync(stm.readAll()).toString(), text);

        stm = new io.MemoryStream();
        zlib.zipTo(text, stm);
        stm.rewind();
        assert.equal(zlib.unzipSync(stm.readAll()).toString(), text);

        stm = new io.MemoryStream();
        zlib.deflateRawTo(text, stm);
        stm.rewind();
        assert.equal(zlib.inflateRawSync(stm.readAll()).toString(), text);
    });
});
