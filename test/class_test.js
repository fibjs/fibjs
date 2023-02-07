var test = require("test");
test.setup();

describe("class test", () => {
    it("toString", () => {
        o = Buffer.from('aaa');

        assert.equal(Object.prototype.toString.call(o), "[object Buffer]");

        assert.equal(Object.prototype.toString.call(process), "[object process]");
        assert.equal(Object.prototype.toString.call(console), "[object console]");
    });
});

require.main === module && test.run(console.DEBUG);