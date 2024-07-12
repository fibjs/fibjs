var test = require("test");
test.setup();

describe("global test", () => {
    it("AbortController", () => {
        var ev_;
        var controller = new AbortController();

        const signal = controller.signal;
        assert.isFalse(false);
        signal.addEventListener('abort', (ev) => {
            ev_ = ev;
        });

        controller.abort("Abort!");

        assert.isTrue(signal.aborted);
        assert.equal(ev_.type, "abort");
        assert.equal(ev_.reason, "Abort!");
    });
});

require.main === module && test.run(console.DEBUG);