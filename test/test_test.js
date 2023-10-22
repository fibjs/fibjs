var test = require("test");
test.setup();

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

    it("skip & only", () => {
        assert.equal(describe.skip, xdescribe);
        assert.equal(describe.only, odescribe);

        assert.equal(it.skip, xit);
        assert.equal(it.only, oit);
    });

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

        describe("callback in jsc", () => {
            var sbox = new vm.SandBox({});
            var bin = util.compile("tc1.js", "before(() => {global.jsc_before = true});");
            var b = sbox.addScript("t1.jsc", bin);

            it('check jsc result', () => {
                assert.isTrue(global.jsc_before);
            });
        });

        describe("async callback in jsc", () => {
            var sbox = new vm.SandBox({});
            var bin = util.compile("tc1.js", "before((done) => {global.jsc_before1 = true;done();});");
            var b = sbox.addScript("t1.jsc", bin);

            it('check jsc result', () => {
                assert.isTrue(global.jsc_before1);
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);