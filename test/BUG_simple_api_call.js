var test = require("test");
test.setup();

var coroutine = require('coroutine');

describe("BUG:simple_api_call", () => {
    it("not hungup", () => {
        for (var i = 0; i < 100; i++)
            coroutine.sleep(1);
    });
});

repl && test.run(console.DEBUG);