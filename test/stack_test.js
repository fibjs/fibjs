var test = require("test");
test.setup();

var coroutine = require('coroutine');
var util = require('util');

function test_fiber() {
    coroutine.sleep(100);
}

function test_callback(cb) {
    setTimeout(function () {
        cb();
    }, 100);
}

describe("stack", () => {
    it("async function", () => {
        var fb = coroutine.start(test_fiber);
        coroutine.sleep(10);
        assert.equal(`    at coroutine.sleep (native code)`, fb.stack.split("\n")[0]);
    });

    it("callback function", () => {
        var fb = coroutine.start(function () {
            util.sync(test_callback)();
        });
        coroutine.sleep(10);
        console.log(fb.stack);
        assert.equal(`    at util.sync.stub (native code)`, fb.stack.split("\n")[0]);
    });
});
