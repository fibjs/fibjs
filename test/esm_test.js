var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

const coroutine = require('coroutine');
const vm = require('vm');
const url = require('url');
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
            await assert.rejects(async () => {
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
            await assert.rejects(async () => {
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

            await assert.rejects(async () => {
                m = await import('./esm_files/esm4');
            });
        });

        it("throw error when 'default' not exists", async () => {
            var m;
            try {
                m = await import('./esm_files/esm9.mjs');
            } catch (e) { }
        });

        it("concurrent import of same module", async () => {
            // Test that concurrent dynamic imports of the same module
            // return the same module instance (not load twice)
            var m = await import('./esm_files/esm20.mjs');

            // Both A and B should get the same shared module instance
            assert.equal(m.a_instanceId, m.b_instanceId);
            assert.equal(m.a_value, 'SHARED_VALUE');
            assert.equal(m.b_value, 'SHARED_VALUE');
            assert.equal(m.same_instance, true);
        });

        it("cached module default export", async () => {
            // Test that cached modules correctly resolve default exports
            // When module A and B both import the same shared module,
            // the second import should get the correct default export
            var m = await import('./esm_files/esm21.mjs');

            // Both A and B should get the correct default export value
            assert.equal(m.a_result, 42);
            assert.equal(m.b_result, 42);
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
                filename: path.join(__dirname, 'esm_files', 'esm17.mjs'),
                url: url.pathToFileURL(path.join(__dirname, 'esm_files', 'esm17.mjs')).href
            }
        });
    });

    it("import.meta in dependency module should point to correct file", async () => {
        var m = await import('./esm_files/esm22.mjs');

        // Parent module's import.meta should point to esm22.mjs
        assert.equal(m.parentMeta.filename, path.join(__dirname, 'esm_files', 'esm22.mjs'));
        assert.equal(m.parentMeta.dirname, path.join(__dirname, 'esm_files'));

        // Child module's import.meta should point to esm22_dep.mjs, NOT esm22.mjs
        assert.equal(m.childMeta.filename, path.join(__dirname, 'esm_files', 'esm22_dep.mjs'));
        assert.equal(m.childMeta.dirname, path.join(__dirname, 'esm_files'));

        // Verify they are different
        assert.notEqual(m.parentMeta.filename, m.childMeta.filename);

        assert.equal(m.childValue, 22);
    });

    it("module instantiation failure should not crash", async () => {
        // This tests that when a module with import.meta fails to instantiate,
        // it should throw an error but not crash
        await assert.rejects(async () => {
            await import('./esm_files/esm23_error.mjs');
        });
    });

    it("error message should contain the file path that initiated the import", async () => {
        // This tests that when a module cannot be found, the error message
        // should include the path of the file that tried to import it
        try {
            await import('./esm_files/esm24_import_notfound.mjs');
            assert.fail("Should have thrown an error");
        } catch (e) {
            var expectedPath = path.join(__dirname, 'esm_files', 'esm24_import_notfound.mjs');
            assert.ok(e.message.includes('this_module_does_not_exist.mjs'), 
                'Error message should contain the missing module name');
            assert.ok(e.message.includes('esm24_import_notfound.mjs'), 
                'Error message should contain the file path that initiated the import');
        }
    });

    it("error message should contain the correct file path in dependency tree", async () => {
        // This tests that when a dependency module cannot be found,
        // the error message should include the path of the dependency file that tried to import it,
        // not the root module
        try {
            await import('./esm_files/esm25_parent.mjs');
            assert.fail("Should have thrown an error");
        } catch (e) {
            assert.ok(e.message.includes('nonexistent_dep_module.mjs'), 
                'Error message should contain the missing module name');
            assert.ok(e.message.includes('esm25_child.mjs'), 
                'Error message should contain the child file path that initiated the import');
            assert.ok(!e.message.includes('esm25_parent.mjs') || e.message.indexOf('esm25_child.mjs') < e.message.indexOf('esm25_parent.mjs'),
                'Error message should reference the child module, not just the parent');
        }
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

    it("BUGFIX: Crash when async module triggers microtask during Evaluate", async () => {
        var m = await import('./esm_files/crash_import/test.mjs');
        assert.ok(m);
    });

    it("BUGFIX: Sandbox should not be GC'd while module functions are still in use", async () => {
        // Create sandbox with addBuiltinModules to allow node:test import
        let sandbox = new vm.SandBox({});
        sandbox.addBuiltinModules();

        const module1Path = path.join(__dirname, 'esm_files', 'gc_test_module1.mjs');
        let module1 = await sandbox.import(module1Path, __dirname);
        let doImport = module1.doImport;

        // Release sandbox reference and force GC
        sandbox = null;
        if (typeof global.gc === 'function') {
            global.gc();
            global.gc();
            coroutine.sleep(10);
        }

        // Dynamic import should still work because sandbox is kept alive via import.meta
        const result = await doImport();
        assert.equal(result.value, 42);
    });

    describe('require in ES module', () => {
        const child_process = require('child_process');
        const cmd = process.execPath;

        it('should throw error when using require() in ES module', () => {
            // Test that require() throws a clear error in ESM context
            const testFile = path.join(__dirname, 'esm_files', 'require_error', 'test_require.js');
            assert.throws(() => {
                child_process.execFileSync(cmd, [testFile], {
                    encoding: 'utf8',
                    stdio: ['pipe', 'pipe', 'pipe']
                });
            }, (err) => {
                // Check that stderr contains the expected error message
                return err.stderr.includes('require is not defined in ES module scope');
            });
        });

        it('should suggest using import instead', () => {
            const testFile = path.join(__dirname, 'esm_files', 'require_error', 'test_require.js');
            try {
                child_process.execFileSync(cmd, [testFile], {
                    encoding: 'utf8',
                    stdio: ['pipe', 'pipe', 'pipe']
                });
                assert.fail('should have thrown');
            } catch (err) {
                assert.ok(err.stderr.includes('use import instead'), 
                    'Error message should suggest using import instead');
            }
        });

        it('should work with import in the same ESM package', async () => {
            // Verify that import still works in the ESM package
            const m = await import('./esm_files/esm1.mjs');
            assert.deepEqual(m, { test: 4 });
        });
    });
});


