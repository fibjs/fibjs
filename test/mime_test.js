var test = require("test");
test.setup();

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
        assert.equal(mime.getType('a.xml'), 'text/xml');
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
        assert.equal(mime.getType('xml'), 'text/xml');
        assert.equal(mime.getType('txt'), 'text/plain');
        assert.equal(mime.getType('a'), 'application/octet-stream');
    });

    it("addType", function () {
        mime.addType('a', 'application/a');
        assert.equal(mime.getType('a.a'), 'application/a');
        assert.equal(mime.getType('a'), 'application/a');
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

require.main === module && test.run(console.DEBUG);
