var { describe, suite, it, todo, odescribe, oit, xdescribe, xit, skip, only, before, after, beforeEach, afterEach } = require('node:test');
var test = require('node:test');
var assert = require('assert');

var vm = require("vm");
var util = require("util");
var coroutine = require("coroutine");

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

assert.throws(() => {
    test.mustCall(function (a, b) {
        return a + b;
    });
})

describe("test", () => {
    assert.throws(() => {
        test.mustCall(function (a, b) {
            return a + b;
        });
    })

    it("alias of test", () => {
        assert.equal(test, test.it);
        assert.equal(test, test.test);
    });

    it("alias of suite", () => {
        assert.equal(test.describe, test.suite);
    });

    it("global alias", () => {
        assert.equal(describe, suite);
        assert.equal(describe, test.describe);
        assert.equal(xdescribe, test.xdescribe);
        assert.equal(odescribe, test.odescribe);
        assert.equal(assert, test.assert);
        assert.equal(it.it, it);
        assert.equal(it.xit, xit);
        assert.equal(it.skip, skip);
        assert.equal(it.oit, oit);
        assert.equal(it.only, only);
    });

    it("support it in it", () => {
        it("it in it", () => { });
    });

    it("support before in it", () => {
        before(() => { });
    });

    it("support after in it", () => {
        after(() => { });
    });

    it("support beforeEach in it", () => {
        beforeEach(() => { });
    });

    it("support afterEach in it", () => {
        afterEach(() => { });
    });

    describe('describe.skip', () => {
        var steps = [];

        describe('suite', () => {
            describe('step1', () => {
                before(() => {
                    steps.push('step1');
                });
            });

            describe.skip('step2', () => {
                before(() => {
                    steps.push('step2');
                });
            });

            describe('step3', () => {
                before(() => {
                    steps.push('step3');
                });
            });
        });

        it('check', () => {
            assert.deepEqual(steps, [
                'step1',
                'step3'
            ])
        });
    });


    describe('describe.only', () => {
        var steps = [];

        describe('suite', () => {
            describe('step1', () => {
                before(() => {
                    steps.push('step1');
                });
            });

            describe.only('step2', () => {
                before(() => {
                    steps.push('step2');
                });
            });

            describe('step3', () => {
                before(() => {
                    steps.push('step3');
                });
            });

            describe.only('step4', () => {
                before(() => {
                    steps.push('step4');
                });
            });
        });

        it('check', () => {
            assert.deepEqual(steps, [
                'step2',
                'step4'
            ])
        });
    });

    describe('it.skip', () => {
        var steps = [];

        describe('suite', () => {
            it('step1', () => {
                steps.push('step1');
            });

            it.skip('step2', () => {
                steps.push('step2');
            });

            it('step3', () => {
                steps.push('step3');
            });
        });

        it('check', () => {
            assert.deepEqual(steps, [
                'step1',
                'step3'
            ])
        });
    });

    describe('it.only', () => {
        var steps = [];

        describe('suite', () => {
            it('step1', () => {
                steps.push('step1');
            });

            it.only('step2', () => {
                steps.push('step2');
            });

            it('step3', () => {
                steps.push('step3');
            });

            it.only('step4', () => {
                steps.push('step4');
            });
        });

        it('check', () => {
            assert.deepEqual(steps, [
                'step2',
                'step4'
            ])
        });
    });

    describe("todo case", () => {
        var t = false;

        todo("todo", () => {
            t = true;
        });

        it("check", () => {
            assert.equal(t, false);
        });
    });

    describe('hook function', () => {
        var steps = [];

        describe('before/after', () => {
            before(() => {
                steps.push('before');
            });

            after(() => {
                steps.push('after');
            });

            beforeEach(() => {
                steps.push('before each 1');
            });

            beforeEach(() => {
                steps.push('before each 2');
            });

            afterEach(() => {
                steps.push('after each 1');
            });

            afterEach(() => {
                steps.push('after each 2');
            });

            xit('skip', () => {
                steps.push('skip');
            });

            todo('todo', () => {
                steps.push('todo');
            });

            it('test', () => {
                steps.push('test');
            });

            describe('sub describe', () => {
                beforeEach(() => {
                    steps.push('sub before each 2');
                });

                afterEach(() => {
                    steps.push('sub after each 1');
                });

                it('test', () => {
                    steps.push('sub test');
                });
            });
        });

        it("check", () => {
            assert.deepEqual(steps, [
                "before",
                "before each 1",
                "before each 2",
                "test",
                "after each 2",
                "after each 1",
                "before each 1",
                "before each 2",
                "sub before each 2",
                "sub test",
                "sub after each 1",
                "after each 2",
                "after each 1",
                "after"
            ]);
        });
    });

    describe("async case", () => {
        var t = false;
        it("case", async () => {
            await sleep(100);
            t = true;
        });

        it("check", () => {
            assert.equal(t, true);
        });
    });

    it('mustCall', () => {
        setImmediate(test.mustCall(function (a, b) {
            return a + b;
        }));
    });

    it('result of mustCall', () => {
        assert.equal(test.mustCall(function (a, b) {
            return a + b;
        })(1, 2), 3);
    });

    xit('mustNotCall', () => {
        assert.throws(() => {
            test.mustNotCall(function (a, b) {
                return a + b;
            })();
        });
    });

    xit('asyn mustNotCall', () => {
        var not_call = test.mustNotCall(function (a, b) {
            return a + b;
        });
        setImmediate(() => {
            not_call(1, 2);
        });

        coroutine.sleep(1);
    });

    describe("callback case", () => {
        var t;

        it("function(done)", function (done) {
            t = false;
            setTimeout(() => {
                t = true;
                done();
            }, 100);
        });

        it("function(done) check", () => {
            assert.equal(t, true);
        });

        it("function name(done)", function name(done) {
            t = false;
            setTimeout(() => {
                t = true;
                done();
            }, 100);
        });

        it("function name(done) check", () => {
            assert.equal(t, true);
        });

        it("done =>", done => {
            t = false;
            setTimeout(() => {
                t = true;
                done();
            }, 100);
        });

        it("done => check", () => {
            assert.equal(t, true);
        });


        it("(done) =>", (done) => {
            t = false;
            setTimeout(() => {
                t = true;
                done();
            }, 100);
        });

        it("(done) => check", () => {
            assert.equal(t, true);
        });

        it("done(cb)", done => {
            t = false;
            done(() => {
                t = true;
            });
        });

        it("done(cb) => check", () => {
            assert.equal(t, true);
        });

        xdescribe("callback in jsc", () => {
            var sbox = new vm.SandBox({});
            var bin = util.compile("tc1.js", "before(() => {global.jsc_before = true});");
            var b = sbox.addScript("t1.jsc", bin);

            it('check jsc result', () => {
                assert.isTrue(global.jsc_before);
            });
        });

        xdescribe("async callback in jsc", () => {
            var sbox = new vm.SandBox({});
            var bin = util.compile("tc1.js", "before((done) => {global.jsc_before1 = true;done();});");
            var b = sbox.addScript("t1.jsc", bin);

            it('check jsc result', () => {
                assert.isTrue(global.jsc_before1);
            });
        });
    });

    describe("async describe with dynamic import", () => {
        var mod;
        var executionOrder = [];

        describe("async import test", async () => {
            executionOrder.push("async describe start");

            // 延迟执行避免 ESM 加载期间的 import 重入死锁
            mod = await import('./async_describe_module.mjs');

            executionOrder.push("async describe end");

            it("should have correct value from imported module", () => {
                executionOrder.push("test 1");
                assert.equal(mod.value, 42);
            });

            describe("nested async describe", async () => {
                executionOrder.push("nested async describe");

                it("nested test", () => {
                    executionOrder.push("nested test");
                    assert.ok(true);
                });
            });
        });

        describe("sync describe after async", () => {
            executionOrder.push("sync describe");

            it("sync test", () => {
                executionOrder.push("sync test");
                assert.ok(true);
            });
        });

        it("check execution order", () => {
            // async describe 应该正确等待完成，保证顺序
            assert.deepEqual(executionOrder, [
                "async describe start",
                "async describe end",
                "test 1",
                "nested async describe",
                "nested test",
                "sync describe",
                "sync test"
            ]);
        });
    });
});

