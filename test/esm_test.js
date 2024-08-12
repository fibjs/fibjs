var test = require("test");
test.setup();

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

    });

});

require.main === module && test.run(console.DEBUG);
