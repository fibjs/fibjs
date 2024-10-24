var test = require("test");
test.setup();

const coroutine = require('coroutine');
const vm = require('vm');
const path = require('path');

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

        it("import json with 'assert'", async () => {
            assert.throws(async () => {
                var m = await import('./esm_files/esm18.mjs');
            });
        });

        it("import json with 'with'", async () => {
            var m = await import('./esm_files/esm19');
            assert.deepEqual(m, { test: 10000 });
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

    describe('sandbox', () => {
        it('import esm in sandbox', async () => {
            var sbox = new vm.SandBox();
            var m = await sbox.import('./esm_files/esm1.mjs', __dirname);
            assert.deepEqual(m, { test: 4 });
        });

        it('cannot require esm in sandbox', () => {
            var sbox = new vm.SandBox();
            assert.throws(() => {
                sbox.require('./esm_files/esm1.mjs', __dirname);
            });
        });
    });

    describe('parallel import', () => {
        const ev = require('./esm_files/async');

        it("require a pendding module", async () => {
            var m1, m2;
            var s = 0;

            ev.clear();

            setImmediate(() => {
                s = 1;
                m1 = require('./esm_files/esm10.mjs');
                ev.set();
            });

            while (!s)
                coroutine.sleep();

            ev.set();
            ev.clear();

            m2 = require('./esm_files/esm10.mjs');

            await ev.wait();

            assert.equal(m1, m2);
        });

        it("import a pendding module", async () => {
            var m1, m2;
            var s = 0;

            ev.clear();

            setImmediate(async () => {
                s = 1;
                m1 = await import('./esm_files/esm11.mjs');
                ev.set();
            });

            while (!s)
                coroutine.sleep();

            ev.set();
            ev.clear();

            m2 = await import('./esm_files/esm11.mjs');

            await ev.wait();

            assert.equal(m1, m2);
        });

        it("import a pendding dependency module", async () => {
            var m1, m2;
            var s = 0;

            ev.clear();

            setImmediate(async () => {
                s = 1;
                m1 = await import('./esm_files/esm14.mjs');
                ev.set();
            });

            while (!s)
                coroutine.sleep();

            ev.set();
            ev.clear();

            m2 = await import('./esm_files/esm14.1.mjs');

            await ev.wait();

            assert.equal(m1.test2.test, m2.test);
        });
    });

    it("support import.meta", async () => {
        var m = await import('./esm_files/esm17.mjs');
        assert.deepEqual(m, {
            test: {
                dirname: path.join(__dirname, 'esm_files'),
                filename: path.join(__dirname, 'esm_files', 'esm17.mjs')
            }
        });
    });

    it("BUGFIX: crash when cjs export 'default'", async () => {
        var m = await import('./esm_files/esm15.mjs');
        assert.deepEqual(m, {
            "test": {
                "test1": {
                    "default": 15
                }
            }
        });
    });

    it("BUGFIX: Initiating a top-level import in MIS will cause a crash", async () => {
        var m = await import('./esm_files/esm16.mjs');
        assert.deepEqual(m, {
            "test": {
                "test1": {
                    "test": 16
                }
            }
        });
    });
});

require.main === module && test.run(console.DEBUG);
