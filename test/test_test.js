var test = require("test");
test.setup();

describe("test", () => {
    it("skip & only", () => {
        assert.equal(describe.skip, xdescribe);
        assert.equal(describe.only, odescribe);

        assert.equal(it.skip, xit);
        assert.equal(it.only, oit);
    });

    it("it throw when not in describe", () => {
        assert.throws(() => {
            it("x", () => {});
        });
    });

    it("before throw when not in describe", () => {
        assert.throws(() => {
            before(() => {});
        });
    });

    it("after throw when not in describe", () => {
        assert.throws(() => {
            after(() => {});
        });
    });

    it("beforeEach throw when not in describe", () => {
        assert.throws(() => {
            beforeEach(() => {});
        });
    });

    it("afterEach throw when not in describe", () => {
        assert.throws(() => {
            afterEach(() => {});
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
});

repl && test.run(console.DEBUG);