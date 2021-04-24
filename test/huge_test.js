var test = require("test");
test.setup();

var base32 = require('base32');
var hex = require('hex');
var util = require('util');

describe('huge buffer', () => {
    it("FIX: big Buffer base64 encode", () => {
        var buf = Buffer.alloc(1700601981);
        assert.throws(() => {
            buf.toString('base64');
        });
    });

    it("FIX: big Buffer hex encode", () => {
        var buf = Buffer.alloc(1700601981);
        assert.throws(() => {
            buf.toString('hex');
        });
    });

    it("FIX: big string format", () => {
        util.format("", base32.encode(530543385));
    });

    it("FIX: big Buffer Iterator", () => {
        buf = global.Buffer.alloc(1498850885);
        buf.values();
    });

    it("FIX: huge string transfer between fibjs and v8", () => {
        var s = hex.encode(502045304);
        var s1 = util.format(s);
        var s2 = util.format(s1);
        assert.equal(s2.length, 1004090608);
    })
});

require.main === module && test.run(console.DEBUG);