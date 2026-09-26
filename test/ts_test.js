var { describe, xdescribe, it } = require('node:test');
var assert = require('assert');
var child_process = require('child_process');
var path = require('path');

describe('TypeScript modules', () => {
    it("require cts", () => {
        const t = require('./ts_files/ts1.cts');
        assert.deepEqual(t, { test: "test1" });
    });

    it("require mts", () => {
        const t = require('./ts_files/ts2.mts');
        assert.deepEqual(t, { test: "test2" });
    });

    it("await import cts", async () => {
        const t = await import('./ts_files/ts1.cts');
        assert.deepEqual(t, { test: "test1" });
    });

    it("await import mts", async () => {
        const t = await import('./ts_files/ts2.mts');
        assert.deepEqual(t, { test: "test2" });
    });

    it("import from cts", async () => {
        const t = await import('./ts_files/test3.mts');
        assert.deepEqual(t, {
            "test3": "test3"
        });
    });

    it("import from mts", async () => {
        const t = await import('./ts_files/test4.mts');
        assert.deepEqual(t, {
            "test4": "test4"
        });
    });

    it("import ts from commonjs", async () => {
        const t = await import('./ts_files/test5.mts');
        assert.deepEqual(t, {
            "test5": "test5"
        });
    });

    it("import ts from esmodule", async () => {
        const t = await import('./ts_files/test6.mts');
        assert.deepEqual(t, {
            "test6": "test6"
        });
    });

    it("lower a parameter property on every load", () => {
        // The isolate caches file contents, so every sandbox in this process gets the
        // same buffer for a file. Stripping it in place must not leave the erased
        // TypeScript behind for the next load: that load would find no parameter
        // property to lower and silently build a constructor that assigns nothing.
        const vm = require('vm');
        const file = path.join(__dirname, 'ts_files/ts7.cts');

        for (let i = 0; i < 3; i++) {
            const box = new vm.SandBox({});
            assert.deepEqual(box.require(file, __dirname), { sum: 5 });
        }
    });

    // A `.ts` file is ambiguous in exactly the same way a `.js` file is: the
    // extension is the plain one and nothing in package.json says which module
    // system it wants. Both loaders have to recognise ES module syntax and
    // retry as ESM, otherwise every TypeScript file written with import/export
    // fails to load unless the nearest package.json happens to say
    // "type": "module". Node.js reaches the same conclusion by itself.
    describe('module system detection for .ts', () => {
        const dir = path.join(__dirname, 'ts_files/esm');
        const runFile = (file) => {
            const r = child_process.spawnSync(process.execPath, [path.join(dir, file)]);
            return { status: r.status, stdout: String(r.stdout), stderr: String(r.stderr) };
        };

        it("await import a .ts that uses export", async () => {
            const m = await import('./ts_files/esm/mod.ts');
            assert.strictEqual(m.v, 1);
            assert.strictEqual(m.add(1, 2), 3);
        });

        it("require a .ts that uses export", () => {
            const m = require('./ts_files/esm/mod.ts');
            assert.strictEqual(m.v, 1);
        });

        it("a .ts imports another .ts", async () => {
            const m = await import('./ts_files/esm/uses-import.ts');
            assert.strictEqual(m.sum, 3);
        });

        it("a .mjs imports a .ts", async () => {
            const m = await import('./ts_files/esm/from-mjs.mjs');
            assert.strictEqual(m.fromMjs, 1);
        });

        it("runs a .ts entry point that uses export", () => {
            const r = runFile('entry.ts');
            assert.strictEqual(r.status, 0, r.stderr);
            assert.match(r.stdout, /entry ok 3/);
        });

        it("runs a .ts entry point with top-level await", () => {
            // Top-level await does not name the module system the way `import`
            // does: the error it produces is only *possibly* an ESM problem, so
            // the source has to be probed as a module - which means stripping it
            // first, since V8 cannot compile TypeScript.
            const r = runFile('top-level-await.ts');
            assert.strictEqual(r.status, 0, r.stderr);
            assert.match(r.stdout, /tla ok 7/);
        });

        it("still rejects a .cts that uses export", () => {
            // `.cts` pins the module system down, so there is nothing to detect.
            const r = runFile('pinned.cts');
            assert.notStrictEqual(r.status, 0);
            assert.match(r.stderr, /Unexpected token 'export'|Cannot use import statement/);
        });

        it("reports the strip error when the syntax cannot be erased", () => {
            // The retry must not swallow the error the author needs to see.
            const r = runFile('unsupported.ts');
            assert.notStrictEqual(r.status, 0);
            assert.match(r.stderr, /not supported in strip-only mode/);
        });

        it("does not detect syntax when package.json says type: commonjs", () => {
            // Detection is for ambiguous files only. Here package.json has
            // named the module system, so `export` is an error rather than a
            // module to discover - Node.js draws the same line.
            const pinned = path.join(__dirname, 'ts_files/pinned');
            for (const file of ['typed.ts', 'plain.js']) {
                const r = child_process.spawnSync(process.execPath, [path.join(pinned, file)]);
                const stderr = String(r.stderr);
                assert.notStrictEqual(r.status, 0, file + ' should not load');
                assert.match(stderr, /Unexpected token 'export'/, file);
            }
        });
    });

    xdescribe('TypeScript error source display', () => {
        it("should show original TS source in CTS error", () => {
            const result = child_process.spawnSync(process.execPath, [
                path.join(__dirname, 'ts_files/error_test.cts')
            ]);
            const stderr = result.stderr.toString();
            // Error output should contain the original TypeScript source line with type annotations
            assert.ok(stderr.includes('name: string'), "Error source line should show TypeScript type annotation 'name: string'");
        });

        it("should show original TS source in MTS error", () => {
            const result = child_process.spawnSync(process.execPath, [
                path.join(__dirname, 'ts_files/error_test.mts')
            ]);
            const stderr = result.stderr.toString();
            // Error output should contain the original TypeScript source line with type annotations
            assert.ok(stderr.includes('name: string'), "Error source line should show TypeScript type annotation 'name: string'");
        });
    });
});

