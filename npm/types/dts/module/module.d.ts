/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The module module exposes Node.js-compatible helpers about the module system itself
 *
 *  It builds a `require()` bound to a known location, lists the built-in modules and
 *  probes the compile cache; useful when a script loads code from a file it locates
 *  itself, enumerates the built-ins, or answers Node.js compatibility checks.
 *
 *  Main capabilities:
 *
 *  - **Require factory**: `createRequire` returns a `require()` whose relative lookups
 *    start next to a given file, directory or file: URL;
 *  - **Built-in inventory**: `builtinModules` lists every module that can be required
 *    without a path, once as a bare name and once with the `node:` prefix;
 *  - **Compile cache probe**: `enableCompileCache` is the Node.js v22.8+ entry point;
 *    fibjs does not implement the bytecode cache, so the call reports status 2.
 *
 *  Concepts:
 *
 *  - **The module object**: fibjs runs every script as a CommonJS module with its own
 *    `module`, `exports` and `require`; the `module` core module documented here is a
 *    different object that only carries the three helpers above. `require('module')`,
 *    `require('node:module')` and `require('fibjs:module')` return the same object,
 *    while the running script's own module object is the global `module`. A top-level
 *    `const`/`let` binding named `module` collides with that wrapper binding, so the
 *    examples below bind the core module to `mod` instead (Node.js rejects the same
 *    declaration).
 *  - **Module resolution**: `createRequire` loads nothing by itself; it returns a
 *    function that resolves a request the way the global require does: relative ids
 *    against the directory of the base, bare ids through node_modules and the built-in
 *    list. The returned function's `resolve(id)` answers the resulting path without
 *    loading it.
 *  - **Built-in modules**: modules compiled into the binary or embedded as JavaScript
 *    can be required by bare name or with the `node:` prefix; the inventory also covers
 *    sub-path modules such as `fs/promises`, `path/posix` and `timers/promises`.
 *  - **Compile cache**: Node.js can persist V8-compiled bytecode between runs; fibjs
 *    accepts the call for compatibility but keeps no cache, so `enableCompileCache`
 *    never changes startup or the code that runs afterwards.
 *
 *  Import:
 *  ```JavaScript
 *  const mod = require('module');
 *  // require('node:module') and require('fibjs:module') return the same object
 *  ```
 *
 *  Example 1 — build a require() for a known location:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const mod = require('module');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-module-'));
 *  const main = path.join(dir, 'main.js');
 *  fs.writeFileSync(path.join(dir, 'answer.js'), 'module.exports = 42;');
 *
 *  const req = mod.createRequire(main);
 *  console.log(req.resolve('./answer.js')); // <dir>/answer.js
 *  console.log(req('./answer.js')); // 42
 *  console.log(typeof req('fs').readFile); // function
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — inspect the built-in module inventory:
 *  ```JavaScript
 *  const mod = require('module');
 *
 *  console.log(mod.builtinModules.includes('fs')); // true
 *  console.log(mod.builtinModules.includes('node:fs')); // true
 *  console.log(mod.builtinModules.includes('path/posix')); // true
 *
 *  const original = mod.builtinModules;
 *  mod.builtinModules = []; // the property is read-only, the assignment is ignored
 *  console.log(mod.builtinModules === original); // true
 *  ```
 *
 *  Example 3 — the compile cache probe used by compatibility checks:
 *  ```JavaScript
 *  const mod = require('module');
 *
 *  const result = mod.enableCompileCache();
 *  console.log(result.status); // 2, not supported
 *  console.log(result.message); // Compile cache is not supported in fibjs
 *  ```
 *
 *  Notes:
 *
 *  - `builtinModules` holds both forms of every built-in plus the sub-path modules, in module
 *    registration order rather than alphabetically, and without duplicates.
 *  - The `require()` returned by `createRequire` carries only `resolve`; Node.js also exposes
 *    `main`, `extensions` and `cache` on it.
 *  - Node.js requires an absolute path or a file: URL as the base and rejects a relative one with
 *    ERR_INVALID_ARG_VALUE; fibjs accepts any string and resolves a relative base against the
 *    current working directory (plans/compat-differences.md 2.259).
 *  - Node.js implements a real on-disk compile cache (status 0/1/2/3 plus `getCompileCacheDir`
 *    and `flushCompileCache`); fibjs returns status 2 with the fixed message above and has no
 *    cache directory (plans/compat-differences.md 2.260).
 *  - Do not bind the core module to a top-level `const module`: the CommonJS wrapper already
 *    provides `module`, so the declaration is a syntax error and the engine retries the file as
 *    an ES module, where `require` is undefined. Use another name (`mod` in these examples) or
 *    `var module`; Node.js rejects the same declaration.
 *
 */
declare module 'module' {
    /**
     * @description Creates a require function bound to a base path
     *
     *      The returned function resolves a request the way the global require does, with the
     *      directory of the base as the starting point: a relative id is looked up next to the base,
     *      a bare id goes through node_modules and the built-in list. `base` may be an absolute file
     *      path, a directory path, a relative path or a file: URL; the directory part is used, so a
     *      directory base resolves relative ids against its parent. Node.js requires an absolute path
     *      or a file: URL and rejects a relative base with ERR_INVALID_ARG_VALUE; fibjs accepts any
     *      string and resolves a relative base against the current working directory. The returned
     *      function has `resolve(id)`, which returns the path that require would load (a built-in
     *      resolves to its name) without loading it, and reports a missing module with
     *      MODULE_NOT_FOUND [20024]; unlike Node.js it has no `main`, `extensions` or `cache`.
     *
     *      Example — resolve and load a module next to a known file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const mod = require('module');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-creq-'));
     *      const main = path.join(dir, 'main.js');
     *      fs.writeFileSync(path.join(dir, 'answer.js'), 'module.exports = 42;');
     *
     *      const req = mod.createRequire(main);
     *      console.log(req.resolve('./answer.js')); // <dir>/answer.js
     *      console.log(req('./answer.js')); // 42
     *      console.log(req.resolve('fs')); // fs
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param base base path or file: URL; the directory part becomes the resolution root
     *      @return a require function bound to the base, with a resolve method
     *
     */
    function createRequire(base: string): (id: string)=>any;

    /**
     * @description The list of built-in module names, as a read-only property
     *
     *      Contains every module that can be required without a path, each once as a bare name and
     *      once with the `node:` prefix, plus the sub-path modules (`fs/promises`, `path/posix`,
     *      `dns/promises`, `assert/strict`, `util/types`, `timers/promises`) and the JavaScript
     *      modules embedded in the binary (`stream`, `readline`, `stream/web`,
     *      `diagnostics_channel`, `inspector` and their `node:` forms). Entries appear in module
     *      registration order rather than alphabetically and are unique. The property itself is
     *      read-only and non-configurable: assigning to it is silently ignored. The array object is
     *      not frozen, so elements can be added or replaced, but such edits are local and do not
     *      change what require accepts. Node.js exposes only the bare names in this list, makes the
     *      property writable and configurable, and covers the prefix question with
     *      `module.isBuiltin()` (plans/compat-differences.md 2.258).
     *
     *      Example — query the list and observe the read-only property:
     *      ```JavaScript
     *      const mod = require('module');
     *
     *      console.log(mod.builtinModules.includes('fs')); // true
     *      console.log(mod.builtinModules.includes('node:fs')); // true
     *      console.log(mod.builtinModules.includes('timers/promises')); // true
     *
     *      const original = mod.builtinModules;
     *      mod.builtinModules = [];
     *      console.log(mod.builtinModules === original); // true
     *      ```
     *
     */
    const builtinModules: string[];

    /**
     * @description Compatibility entry point for the Node.js compile cache; always a no-op in fibjs
     *
     *      Node.js uses this call to enable a V8 bytecode cache on disk so that later runs start
     *      faster, and returns an object such as `{ status, directory, message }`. fibjs does not
     *      implement the cache: the call performs no work, ignores its argument (including invalid
     *      values and extra arguments), and always returns a new object
     *      `{ "status": 2, "message": "Compile cache is not supported in fibjs" }`, where status 2
     *      means "not available". Nothing about startup or about the code that runs afterwards
     *      changes, and the Node.js companions `module.getCompileCacheDir` and
     *      `module.flushCompileCache` do not exist (plans/compat-differences.md 2.260).
     *
     *      Example — the probe and its fixed result:
     *      ```JavaScript
     *      const mod = require('module');
     *
     *      const result = mod.enableCompileCache();
     *      console.log(result.status); // 2
     *      console.log(result.message); // Compile cache is not supported in fibjs
     *      console.log(mod.enableCompileCache('/tmp/cache').status); // 2, the path is ignored
     *      ```
     *
     *      @param cacheDir cache directory path; accepted for Node.js compatibility and ignored, optional
     *      @return an object with status 2 and the message "Compile cache is not supported in fibjs"
     *
     */
    function enableCompileCache(cacheDir?: string): FIBJS.GeneralObject;

}

