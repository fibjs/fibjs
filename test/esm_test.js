var test = require("test");
test.setup();

const coroutine = require('coroutine');

describe('ECMAScript modules', () => {
    describe('await import', () => {
        it("simple import", async () => {
            var m = await import('./esm_files/esm1.mjs');
            assert.deepEqual(m, { test: 4 });

            var m1 = await import('./esm_files/esm1.mjs');
            assert.deepEqual(m1, { test: 4 });

            assert.equal(m, m1);
        });

        it("require(mjs)", async () => {
            var m2 = require('./esm_files/esm1.1.mjs');
            assert.deepEqual(m2, { test: 4 });

            var m = await import('./esm_files/esm1.1.mjs');
            assert.deepEqual(m, { test: 4 });

            assert.equal(m, m2);
        });

        it("throw when file not exists", async () => {
            assert.throws(async () => {
                await import('./esm_files/not_exists.mjs');
            });
        });

        it("import js", async () => {
            var m = await import('./esm_files/esm2');
            assert.deepEqual(m, { test: 200 });
        });

        it("import json", async () => {
            var m = await import('./esm_files/esm3');
            assert.deepEqual(m, { test: 500 });
        });

        it("import builtin module", async () => {
            var m = await import('buffer');
            var m1 = require('buffer');
            assert.equal(m, m1);
        });

        it("remove module when SyntaxError", async () => {
            var m;
            try {
                m = await import('./esm_files/esm4');
            } catch (e) { }

            assert.throws(async () => {
                m = await import('./esm_files/esm4');
            });
        });

        it("throw error when 'default' not exists", async () => {
            var m;
            try {
                m = await import('./esm_files/esm9.mjs');
            } catch (e) { }
        });
    });

    describe('import from mjs', () => {
        it("simple import", async () => {
            var m = await import('./esm_files/esm5.mjs');
            assert.equal(m.test2.test, m.test2.test1);
            assert.deepEqual(m.test2.test, {
                test: 5
            });
        });

        describe('import js', () => {
            it("simple import", async () => {
                var m = await import('./esm_files/esm6.mjs');
                assert.equal(m.test2.test, m.test2.test1);
                assert.deepEqual(m.test2.test, {
                    test: 6
                });
            });

            it("default export", async () => {
                var m = await import('./esm_files/esm7.mjs');
                assert.deepEqual(m.test2.test, {
                    test: {
                        test: 7
                    }
                });
            });

            it("default export array", async () => {
                var m = await import('./esm_files/esm8.mjs');
                assert.deepEqual(m.test2.test, {
                    test: [
                        1,
                        2,
                        3,
                        4,
                        5
                    ]
                });
            });
        });

        describe('module tree', () => {
            it("dependency module", async () => {
                var mod1 = await import('./esm_files/esm12.mjs');
                var mod2 = await import('./esm_files/esm12.1.mjs');

                assert.equal(mod1.test2.test, mod2.test);
            });

            it("dependency module when top import failed", async () => {
                try {
                    var mod1 = await import('./esm_files/esm13.mjs');
                } catch (e) { }

                var mod2 = await import('./esm_files/esm13.1.mjs');

                assert.deepEqual(mod2.test, {
                    a: 13,
                    b: 13
                });
            });
        });
    });

    describe('parallel import', () => {
        const ev = require('./esm_files/sync');

        it("require a pendding module", async () => {
            var m1, m2;
            const ev1 = new coroutine.Event();

            ev.clear();

            setImmediate(() => {
                m1 = require('./esm_files/esm10.mjs');
                ev1.set();
            });

            coroutine.sleep();

            ev.set();

            m2 = require('./esm_files/esm10.mjs');

            ev1.wait();

            assert.equal(m1, m2);
        });

        it("import a pendding module", async () => {
            var m1, m2;
            const ev1 = new coroutine.Event();

            ev.clear();

            setImmediate(async () => {
                m1 = await import('./esm_files/esm11.mjs');
                ev1.set();
            });

            coroutine.sleep();

            ev.set();

            m2 = await import('./esm_files/esm11.mjs');

            ev1.wait();

            assert.equal(m1, m2);
        });
    });
});

require.main === module && test.run(console.DEBUG);
