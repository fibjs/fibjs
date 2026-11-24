var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var mime = require('mime');

describe('mime', function () {
    it("getType", function () {
        assert.equal(mime.getType('a.html'), 'text/html');
        assert.equal(mime.getType('a.js'), 'application/javascript');
        assert.equal(mime.getType('a.css'), 'text/css');
        assert.equal(mime.getType('a.jpg'), 'image/jpeg');
        assert.equal(mime.getType('a.png'), 'image/png');
        assert.equal(mime.getType('a.gif'), 'image/gif');
        assert.equal(mime.getType('a.bmp'), 'image/bmp');
        assert.equal(mime.getType('a.ico'), 'image/x-icon');
        assert.equal(mime.getType('a.json'), 'application/json');
        assert.equal(mime.getType('a.xml'), 'application/xml');
        assert.equal(mime.getType('a.txt'), 'text/plain');
        assert.equal(mime.getType('a.a'), 'application/octet-stream');
    });

    it("getType with ext", function () {
        assert.equal(mime.getType('html'), 'text/html');
        assert.equal(mime.getType('js'), 'application/javascript');
        assert.equal(mime.getType('css'), 'text/css');
        assert.equal(mime.getType('jpg'), 'image/jpeg');
        assert.equal(mime.getType('png'), 'image/png');
        assert.equal(mime.getType('gif'), 'image/gif');
        assert.equal(mime.getType('bmp'), 'image/bmp');
        assert.equal(mime.getType('ico'), 'image/x-icon');
        assert.equal(mime.getType('json'), 'application/json');
        assert.equal(mime.getType('xml'), 'application/xml');
        assert.equal(mime.getType('txt'), 'text/plain');
        assert.equal(mime.getType('a'), 'application/octet-stream');
    });

    it("addType", function () {
        mime.addType('a', 'application/a');
        assert.equal(mime.getType('a.a'), 'application/a');
        assert.equal(mime.getType('a'), 'application/a');
    });

    it("addType is case-insensitive", function () {
        mime.addType('MyExt', 'application/x-myext');
        assert.equal(mime.getType('a.myext'), 'application/x-myext');
        assert.equal(mime.getType('a.MYEXT'), 'application/x-myext');
        assert.equal(mime.getType('MYEXT'), 'application/x-myext');
    });

    it("looks up every entry of the sorted table", function () {
        assert.equal(mime.getType('a.vbox'), 'application/x-virtualbox-vbox');
        assert.equal(mime.getType('a.vbox-extpack'), 'application/x-virtualbox-vbox-extpack');
        assert.equal(mime.getType('a.x32'), 'application/x-authorware-bin');
        assert.equal(mime.getType('a.x_t'), 'model/vnd.parasolid.transmit.text');
        assert.equal(mime.getType('a.br'), 'application/x-brotli');
        assert.equal(mime.getType('a.zst'), 'application/zstd');
    });

    it("override type", function () {
        mime.addType('a', 'application/a1');
        assert.equal(mime.getType('a.a'), 'application/a1');
        assert.equal(mime.getType('a'), 'application/a1');

        mime.addType('html', 'application/html1');
        assert.equal(mime.getType('html'), 'application/html1');
        assert.equal(mime.getType('a.html'), 'application/html1');
        mime.addType('html', 'text/html');
    });
});


