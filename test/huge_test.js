var test = require("test");
test.setup();

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
});

require.main === module && test.run(console.DEBUG);