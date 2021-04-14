var test = require("test");
test.setup();

var base32 = require('base32');
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
});

require.main === module && test.run(console.DEBUG);