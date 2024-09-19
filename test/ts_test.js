var test = require("test");
test.setup();

describe('TypeScript modules', () => {
    it("require cts", () => {
        const t = require('./ts_files/ts1.cts');
        assert.deepEqual(t, { test: "test1" });
    });

    it("require mts", () => {
        const t = require('./ts_files/ts2.mts');
        assert.deepEqual(t, { test: "test2" });
    });

    it("await import cts", async () => {
        const t = await import('./ts_files/ts1.cts');
        assert.deepEqual(t, { test: "test1" });
    });

    it("await import mts", async () => {
        const t = await import('./ts_files/ts2.mts');
        assert.deepEqual(t, { test: "test2" });
    });

    it("import from cts", async () => {
        const t = await import('./ts_files/test3.mts');
        assert.deepEqual(t, {
            "test3": "test3"
        });
    });

    it("import from mts", async () => {
        const t = await import('./ts_files/test4.mts');
        assert.deepEqual(t, {
            "test4": "test4"
        });
    });

    it("import ts from commonjs", async () => {
        const t = await import('./ts_files/test5.mts');
        assert.deepEqual(t, {
            "test5": "test5"
        });
    });

    it("import ts from esmodule", async () => {
        const t = await import('./ts_files/test6.mts');
        assert.deepEqual(t, {
            "test6": "test6"
        });
    });
});

require.main === module && test.run(console.DEBUG);