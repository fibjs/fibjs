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

        it("require(esm) - .js file with ESM syntax", () => {
            // .js file with import/export but no "type": "module" in package.json
            // Node.js v22.12+ supports require(esm); fibjs should too
            var generate = require('./esm_files/require_esm_js/generate.js');
            assert.equal(generate.default('red'), 'generated-red');
        });

        it("require(esm) - named exports without default", () => {
            var mod = require('./esm_files/require_esm_js/named.js');
            assert.equal(typeof mod.add, 'function');
            assert.equal(typeof mod.multiply, 'function');
            assert.equal(mod.add(2, 3), 5);
            assert.equal(mod.multiply(2, 3), 6);
        });

        it("require(esm) - default and named exports interop", () => {
            var generate = require('./esm_files/require_esm_js/generate.js');
            assert.equal(typeof generate.default, 'function');
            assert.equal(generate.default('blue'), 'generated-blue');
        });

        it("require(esm) - _interopRequireDefault pattern (__esModule check)", () => {
            // Simulates TypeScript/Babel transpiled CJS that uses
            // _interopRequireDefault to unwrap ESM default exports.
            // Without __esModule on the namespace, this helper
            // double-wraps: { default: { default: fn } }
            function _interopRequireDefault(obj) {
                return obj && obj.__esModule ? obj : { default: obj };
            }
            var _mod = _interopRequireDefault(require('./esm_files/require_esm_js/generate.js'));
            assert.equal(typeof _mod, 'object');
            assert.equal(typeof _mod.default, 'function',
                '_interopRequireDefault should not double-wrap');
            assert.equal(_mod.default('green'), 'generated-green');
        });

        it("require(esm) - _interopRequireDefault after import (reverse order)", async () => {
            // ESM import caches the original namespace in mod.exports first.
            // Subsequent CJS require() must still get the facade with __esModule
            // so _interopRequireDefault does not double-wrap.
            // Use a fresh SandBox to avoid cache interference from other tests.
            var sbox = new vm.SandBox();

            // First: ESM import (caches the module)
            var ns = await sbox.import('./esm_files/require_esm_js/generate.js', __dirname);
            assert.equal(typeof ns.default, 'function');

            // Then: CJS require (must go through run_module → wait_module,
            // which reads the cached mod.exports — if no wrapper was stored,
            // _interopRequireDefault will double-wrap)
            function _interopRequireDefault(obj) {
                return obj && obj.__esModule ? obj : { default: obj };
            }
            var _mod = _interopRequireDefault(sbox.require('./esm_files/require_esm_js/generate.js', __dirname));
            assert.equal(typeof _mod.default, 'function',
                '_interopRequireDefault after import should not double-wrap');
            assert.equal(_mod.default('yellow'), 'generated-yellow');
        });

        it("require(esm) - SandBox.require after import (sbox cached path)", async () => {
            // sbox.require uses in_cjs=false and cannot load ESM .js standalone.
            // But it CAN load them from cache (via wait_module) after an import.
            // This tests the SandBox::require → run_module → wait_module path
            // for cached modules.
            var sbox = new vm.SandBox();

            // First load via import (caches the module)
            await sbox.import('./esm_files/require_esm_js/generate.js', __dirname);

            // Then require from SandBox (uses SandBox::require, not global require)
            var _mod = sbox.require('./esm_files/require_esm_js/generate.js', __dirname);
            assert.equal(typeof _mod.default, 'function');
            assert.equal(_mod.__esModule, true,
                'sbox.require of cached ESM should have __esModule');
            assert.equal(_mod.default('orange'), 'generated-orange');
        });

        it("require(esm) - dependency loaded via ESM then required from CJS", async () => {
            // dep.mjs is loaded as a dependency (non-root) by main.mjs.
            // saveModule() caches it separately from the root. Verify that
            // a subsequent CJS require of the dependency still gets __esModule.
            var sbox = new vm.SandBox();

            // Load main.mjs which imports dep.mjs as a dependency
            var ns = await sbox.import('./esm_files/interop_dep/main.mjs', __dirname);
            assert.equal(typeof ns.default, 'function');

            // Now require the dependency directly from CJS
            var dep = sbox.require('./esm_files/interop_dep/dep.mjs', __dirname);
            assert.equal(dep.__esModule, true,
                'dependency cached by saveModule should have __esModule');
            assert.equal(typeof dep.default, 'function');
            assert.equal(dep.version, '2.0.0');
        });

        it("require(esm) - secondary pattern: redeclare 'require' identifier", () => {
            // .js file with `const require = 100;` which is a CJS compile error
            // (require is a wrapper parameter) but valid in ESM.
            // Node.js v22.12+ detects this and retries as ESM.
            var mod = require('./esm_files/require_esm_js/require_bug.js');
            assert.equal(mod.value, 42);
        });

        it("require(esm) - secondary pattern: redeclare 'module' identifier", () => {
            // .js file with `const module = { id: 'esm' };` — CJS compile error
            // but valid in ESM.
            var mod = require('./esm_files/require_esm_js/redeclare_module.js');
            assert.equal(mod.moduleId, 'esm');
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

    it("BUGFIX: async handler in ESM evaluate should not hang", async () => {
        var m = await import('./esm_files/esm27_async_handler_no_hang.mjs');
        assert.equal(m.default, true);
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

    it("BUGFIX: circular dynamic+static import should not deadlock", () => {
        const child_process = require('child_process');
        const testFile = path.join(__dirname, 'esm_files', 'esm26_deadlock_a.mjs');
        // Run in subprocess with timeout to detect deadlock
        child_process.execFileSync(process.execPath, [testFile], {
            timeout: 5000,
            encoding: 'utf8',
            stdio: ['pipe', 'pipe', 'pipe']
        });
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

    describe('BUGFIX: import CJS with runtime error should propagate real error', () => {
        // When a CJS package (no "type" field in package.json) throws a
        // runtime error during ESM import, the real error must propagate.
        // Previously, GetConfigValue(o,"type",type) in resolveModuleType
        // polluted Runtime::m_error with the string "type" (the field name)
        // when the "type" field was missing. Later, resove_module() read
        // this stale m_error when installScript returned CALL_E_JAVASCRIPT,
        // producing a misleading "Error: type" that masked the real error.

        it("should propagate real error when CJS (no type field) throws at runtime", async () => {
            try {
                await import('./esm_files/cjs_runtime_error_no_type/index.js');
                assert.fail("Should have thrown an error");
            } catch (e) {
                // The error message must contain the REAL error text, not "type"
                assert.ok(e.message.includes("the REAL runtime error"),
                    'Error should contain the real runtime error message, got: ' + e.message);
                // Must NOT be the stale "type" message from GetConfigValue
                assert.notEqual(e.message, 'type',
                    'Error must not be the stale "type" message from m_error pollution');
            }
        });

        it("should propagate real error when CJS (with type:commonjs) throws at runtime", async () => {
            try {
                await import('./esm_files/cjs_runtime_error_with_type/index.js');
                assert.fail("Should have thrown an error");
            } catch (e) {
                assert.ok(e.message.includes("the REAL runtime error"),
                    'Error should contain the real runtime error message, got: ' + e.message);
            }
        });

        it("should load CJS (no type field) successfully when no error", async () => {
            var m = await import('./esm_files/cjs_ok_no_type/index.js');
            assert.equal(m.value, 42);
            assert.equal(m.message, "loaded ok without type field");
        });
    });
});


