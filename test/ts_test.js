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

