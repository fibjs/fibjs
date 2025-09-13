var test = require("test");
test.setup();

describe('ECMAScript modules', () => {
    it("BUGFIX: Crash when importing coroutine module after top-level await module", async () => {
        var m = await import('./test.mjs');
    });
});


