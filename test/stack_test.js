var test = require("test");
test.setup();

var coroutine = require('coroutine');
var util = require('util');
var fs = require('fs');

function test_fiber() {
    coroutine.sleep(100);
}

function test_callback(cb) {
    setTimeout(function () {
        cb();
    }, 100);
}

function level3_callback(callback) {
    fs.readFile('/path/to/nonexistent/file.txt', (err, data) => {
        callback(err); // 将错误传递给回调
    });
}

function level2_callback(callback) {
    level3_callback(callback);
}

function level1_callback(callback) {
    level2_callback(callback);
}

async function level3_promise() {
    return await fs.readFileAsync('/path/to/nonexistent/file.txt');
}

async function level2_promise() {
    return await level3_promise();
}

async function level1_promise() {
    return await level2_promise();
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
        assert.equal(`    at util.sync.stub (native code)`, fb.stack.split("\n")[0]);
    });

    describe("async error stack trace", () => {
        it("should preserve stack trace in callback errors", done => {
            level1_callback((err) => {
                if (err) {
                    try {
                        assert.ok(err.stack.includes('level1_callback'), "Stack should include level1_callback");
                        assert.ok(err.stack.includes('level2_callback'), "Stack should include level2_callback");
                        assert.ok(err.stack.includes('level3_callback'), "Stack should include level3_callback");
                        assert.equal(err.code, 'ENOENT', "Error should be file not found");
                        done();
                    } catch (assertErr) {
                        done(assertErr);
                    }
                } else {
                    done(new Error("Should have received an error"));
                }
            });
        });

        it("should preserve stack trace in promise errors", async () => {
            var error_caught = null;

            try {
                await level1_promise();
            } catch (e) {
                error_caught = e;
            }

            assert.ok(error_caught !== null, "Should catch an error");
            assert.ok(error_caught.stack.includes('level1_promise'), "Stack should include level1_promise");
            assert.ok(error_caught.stack.includes('level2_promise'), "Stack should include level2_promise");
            assert.ok(error_caught.stack.includes('level3_promise'), "Stack should include level3_promise");
            assert.equal(error_caught.code, 'ENOENT', "Error should be file not found");
        });

        it("should include correct file and line numbers in stack", async () => {
            try {
                await fs.readFileAsync('/path/to/nonexistent/file.txt');
                assert.fail("Should have thrown an error");
            } catch (e) {
                assert.ok(e.stack.includes('stack_test.js'), "Stack should include current test file");
                assert.ok(e.stack.match(/:\d+:\d+/), "Stack should include line and column numbers");
            }
        });
    });
});
