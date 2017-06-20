var test = require("test");
test.setup();

describe("test", () => {
    it("it throw when not in describe", () => {
        assert.throws(() => {
            it("x", () => { });
        });
    });

    it("before throw when not in describe", () => {
        assert.throws(() => {
            before(() => { });
        });
    });

    it("after throw when not in describe", () => {
        assert.throws(() => {
            after(() => { });
        });
    });

    it("beforeEach throw when not in describe", () => {
        assert.throws(() => {
            beforeEach(() => { });
        });
    });

    it("afterEach throw when not in describe", () => {
        assert.throws(() => {
            afterEach(() => { });
        });
    });
});

argv.length && test.run(console.DEBUG);
