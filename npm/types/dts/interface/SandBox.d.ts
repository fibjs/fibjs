/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description An isolated module registry that runs code with an optional standalone global object; use it to load untrusted or host-reloaded code without touching the host module table
 *
 *  A SandBox owns an independent module registry: code loaded through the sandbox sees only the
 *  modules that were added to it (plus the module and buffer built-ins), so sandboxes and the host
 *  do not share module state. A SandBox is not a security sandbox: untrusted code can still reach
 *  the host through the objects it is given, and a sandbox created without an explicit global
 *  object shares the host global object.
 *
 *  Main capabilities:
 *
 *  - **Module registry**: `add` (single and dictionary), `addScript`, `remove`, `has`, `modules`;
 *  - **Loading**: `require`, `resolve`, `import`, `run`;
 *  - **Environment**: `addBuiltinModules`, `setModuleCompiler`, the constructor module dictionary;
 *  - **Global object**: an optional standalone `global` object, queried through `global`.
 *
 *  Concepts:
 *
 *  - **Module registry**: a new sandbox contains only the `module` and `buffer` modules.
 *    addBuiltinModules installs the full built-in module set together with the `node:` and `fibjs:`
 *    aliases, `assert/strict` and the `/promises` submodules. Custom modules are registered with
 *    add or addScript; module names and absolute paths are accepted, relative ids throw Error 20024.
 *  - **Copies on add**: a dictionary added with add(Object mods) is copied entry by entry with
 *    util.clone, so a plain object becomes a new object with shared nested references while native
 *    values such as Buffer keep their identity. Replacing an id replaces the registered module.
 *  - **require resolution**: `sandbox.require(id, base)` is the sandbox entry point; `base` is
 *    mandatory (Error 20002 when omitted) and a relative id is resolved against it. Code running
 *    inside the sandbox gets its own one-argument `require(id)`, which resolves against the module's
 *    own location. Modules are cached per sandbox, so each file is evaluated once. require loads
 *    CommonJS and JSON only; import also loads ECMAScript modules.
 *  - **Global object**: a sandbox created without the constructor global argument executes code in
 *    the host global context: scripts read and write the same global object as the host. Pass an
 *    explicit global object to get a standalone global; that object becomes `sandbox.global`
 *    (`sandbox.global === global`) and is isolated from the host. The `global` property and
 *    `freeze` are only available in this mode and otherwise throw Error 20009 (Invalid procedure
 *    call).
 *  - **freeze behavior**: freeze is intended to make the sandbox global read-only. In the current
 *    release it freezes only the internal context global, so writes made through the standalone
 *    global object are still accepted and scripts observe `Object.isFrozen(globalThis) === false`.
 *    Do not use freeze as an immutability or security boundary.
 *  - **Comparison with Node.js vm**: Node's vm creates a context and evaluates code with vm.Script
 *    or vm.runInContext; it has no module registry. SandBox is a module loader with an optional
 *    standalone global and no compile/eval API. Both are explicit about not being security
 *    boundaries; unlike a Node vm context, a default SandBox shares the host global object.
 *
 *  Obtained from:
 *  - `new vm.SandBox(mods)`;
 *  - `new vm.SandBox(mods, require)`;
 *  - `new vm.SandBox(mods, global)`;
 *  - `new vm.SandBox(mods, require, global)` — see the vm module.
 *
 *  Example 1 — register modules and load a module file with require:
 *  ```JavaScript
 *  const vm = require('vm');
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-sandbox-'));
 *  fs.writeFile(path.join(dir, 'answer.js'), 'module.exports = require("base") + 2;');
 *
 *  const box = new vm.SandBox({ base: 40 });
 *  console.log(box.require('./answer.js', dir)); // 42
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — add the built-in modules and use a standalone global:
 *  ```JavaScript
 *  const vm = require('vm');
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-sandbox-'));
 *  const main = 'module.exports = require("path").basename(require("os").tmpdir());';
 *  fs.writeFile(path.join(dir, 'main.js'), main);
 *
 *  const global_obj = {};
 *  const box = new vm.SandBox({}, global_obj);
 *  box.addBuiltinModules();
 *  console.log(typeof box.require('./main.js', dir) === 'string'); // true
 *  console.log(box.global === global_obj); // true
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 3 — custom require function and addScript:
 *  ```JavaScript
 *  const vm = require('vm');
 *
 *  const box = new vm.SandBox({ greeting: 'hello' }, (id) => {
 *      if (id === 'version')
 *          return '1.0';
 *  });
 *
 *  const mod = box.addScript('meta.js', 'module.exports = require("greeting")' +
 *      ' + " " + require("version");');
 *  console.log(mod); // hello 1.0
 *  console.log(box.require('meta', __dirname)); // hello 1.0
 *  ```
 *
 */
declare class Class_SandBox extends Class_object {
    /**
     * @description Constructs a sandbox and registers the modules of the given dictionary
     *
     *      The dictionary is added with add(Object mods). The new sandbox contains only the module and
     *      buffer built-ins and shares the host global object; call addBuiltinModules to make the other
     *      built-in modules available, and pass a global object to isolate the global scope.
     *
     *      @param mods the module object dictionary to add
     *
     */
    constructor(mods?: FIBJS.GeneralObject);

    /**
     * @description Constructs a sandbox with a custom require function
     *
     *      The function is called whenever an id is not found in the module registry. When it returns a
     *      value the value is used as the module; when it returns undefined the id is resolved from the
     *      file system (and throws MODULE_NOT_FOUND when no file matches). The custom function only
     *      handles the sandbox fallback lookup; module code still receives its own one-argument require.
     *
     *      @param mods the module object dictionary to add
     *      @param require a custom require function; when a module does not exist, the custom function is called first, and if it returns nothing the module is loaded from files
     *
     */
    constructor(mods: FIBJS.GeneralObject, require: (id: string)=>any);

    /**
     * @description Constructs a sandbox with a custom require function and an independent global object
     *
     *      Combines the two behaviors: the dictionary is registered, unknown ids are passed to the
     *      custom require function before file resolution, and the given object becomes the standalone
     *      global of the sandbox. See the other constructors for the details of each part.
     *
     *      @param mods the module object dictionary to add
     *      @param require a custom require function; when a module does not exist, the custom function is called first, and if it returns nothing the module is loaded from files
     *      @param global the initial Global properties to set
     *
     */
    constructor(mods: FIBJS.GeneralObject, require: (id: string)=>any, global: FIBJS.GeneralObject);

    /**
     * @description Constructs a sandbox with an independent global object
     *
     *      The given object becomes the standalone global of the sandbox: its properties are populated
     *      into the sandbox context, scripts read and write it as `global`, and the host globals are not
     *      visible. It is returned by the global property (`sandbox.global === global`). This is the
     *      only mode in which global and freeze are available.
     *
     *      @param mods the module object dictionary to add
     *      @param global the initial Global properties to set
     *
     */
    constructor(mods: FIBJS.GeneralObject, global: FIBJS.GeneralObject);

    /**
     * @description Adds all built-in modules to the sandbox
     *
     *      Installs every built-in module with its `node:` and `fibjs:` aliases, the `assert/strict`
     *      module and the `/promises` submodules; afterwards the sandbox can require fs, path, crypto
     *      and the other built-ins. A new sandbox contains only the module and buffer modules.
     *
     *      Example:
     *      ```JavaScript
     *      const vm = require('vm');
     *      const box = new vm.SandBox();
     *
     *      box.addBuiltinModules();
     *      console.log(box.require('path', __dirname).basename('/a/b.txt')); // b.txt
     *      ```
     *
     */
    addBuiltinModules(): void;

    /**
     * @description Adds a module to the sandbox under the given name
     *
     *      The id must be a module name or an absolute path; a relative id throws Error 20024 ("does
     *      not accept relative path"). The value is copied with util.clone: a plain object becomes a new
     *      object whose nested references are shared with the original, while native values such as
     *      Buffer keep their identity. Registering an existing id replaces the module.
     *
     *      @param id the name of the module to add; this path is unrelated to the running script and must be an absolute path or a module name
     *      @param mod the module object to add
     *
     */
    add(id: string, mod: any): void;

    /**
     * @description Adds a dictionary of modules to the sandbox
     *
     *      Every entry is added as with add(String id, Value mod): the keys must not be relative and
     *      each value is copied individually, so a modification made by sandbox code to a copied plain
     *      object does not affect the original object, while nested references stay shared.
     *
     *      @param mods the module object dictionary to add; added javascript modules are copied so that modifications made by the sandbox do not interfere with each other
     *
     */
    add(mods: FIBJS.GeneralObject): void;

    /**
     * @description Adds a script module to the sandbox and returns its exports
     *
     *      The script is compiled and registered under srcname; the extension selects the loader
     *      (`.js`, `.json`, `.jsc`), and a string is encoded as UTF-8. The code receives the sandbox
     *      one-argument require, so it can load the other modules registered in the sandbox by name.
     *      This is the way to add code that does not exist as a file.
     *
     *      @param srcname the script name to add; srcname must include an extension, such as json, js or jsc
     *      script may be a Buffer, or a string encoded as utf8.
     *      @param script the binary code to add
     *      @return returns the loaded module object
     *
     */
    addScript(srcname: string, script: Class_Buffer | string): any;

    /**
     * @description Removes a module from the sandbox registry
     *
     *      Only the registry entry is removed: module objects already returned by require keep working,
     *      and requiring the id again loads it anew (or fails). Removing an unknown id is a no-op.
     *      Built-in aliases are separate entries and must be removed one by one.
     *
     *      @param id the name of the module to remove; this path is unrelated to the running script and must be an absolute path or a module name
     *
     */
    remove(id: string): void;

    /**
     * @description Checks whether a module id is registered in the sandbox
     *
     *      Returns true for module names and absolute paths registered with add or addScript, and for
     *      the modules installed by addBuiltinModules including their `node:` and `fibjs:` aliases. The
     *      check only inspects the registry: it does not touch the file system and does not call the
     *      custom require function.
     *
     *      @param id the name of the module to check; this path is unrelated to the running script and must be an absolute path or a module name
     *      @return whether it exists
     *
     */
    has(id: string): boolean;

    /**
     * @description Clones the sandbox module registry into a new sandbox
     *
     *      The new sandbox gets a copy of the module table, so later add or remove calls on either
     *      sandbox do not affect the other. The custom require function and the standalone global are
     *      not copied: the clone resolves unknown ids from files and shares the host global object
     *      (verified).
     *
     *      @return the new cloned sandbox
     *
     */
    clone(): Class_SandBox;

    /**
     * @description Freezes the sandbox context global
     *
     *      Only available when the sandbox was created with a standalone global object; otherwise it
     *      throws Error 20009 (Invalid procedure call). Note that in the current release freeze does
     *      not block writes made through the standalone global object, and scripts still observe
     *      `Object.isFrozen(globalThis) === false`; do not rely on it as an immutability or security
     *      boundary.
     *
     */
    freeze(): void;

    /**
     * @description Runs a script file in the sandbox
     *
     *      The file is loaded through the sandbox module system and executed as the main module, so it
     *      uses the sandbox require instead of the host require and can only see the sandbox modules.
     *      Errors propagate to the caller. The path must be absolute; run returns nothing, use require
     *      when the module value is needed.
     *
     *      @param fname the path of the script to run; this path is unrelated to the running script and must be an absolute path
     *
     */
    run(fname: string): void;

    /**
     * @description Resolves a module id and returns the resolved name
     *
     *      A registered module name is returned unchanged; a relative id is resolved against base to an
     *      absolute file name, which must exist on disk. Both arguments are required. This performs the
     *      same resolution as require without loading the module.
     *
     *      @param id the name of the module to load
     *      @param base the lookup path
     *      @return returns the full file name of the loaded module
     *
     */
    resolve(id: string, base: string): string;

    /**
     * @description Loads a CommonJS module and returns its exports
     *
     *      The base argument is mandatory: omitting it throws Error 20002 (Parameter not optional). A
     *      relative id is resolved against base, a module name is looked up in the sandbox registry,
     *      and unknown ids are passed to the custom require function before file resolution. Results
     *      are cached per sandbox, so a module is evaluated once. ECMAScript modules are not supported;
     *      use import instead. A missing module throws MODULE_NOT_FOUND.
     *
     *      @param id the name of the module to load
     *      @param base the lookup path
     *      @return returns the loaded module object
     *
     */
    require(id: string, base: string): any;

    /**
     * @description Asynchronously loads a module, supporting ECMAScript modules
     *
     *      Unlike require, import returns a Promise and can load `.mjs`/ESM files as well as CommonJS
     *      modules, for example `import('./mod.mjs', dir).then((mod) => mod.default)`. The base argument
     *      is required like require. The promise resolves with the module namespace, so a default
     *      export is read from its `default` property.
     *
     *      @param id the name of the module to load
     *      @param base the lookup path
     *      @return returns the loaded module object
     *
     */
    import(id: string, base: string): Promise;

    /**
     * @description Registers a compiler for a custom file extension
     *
     *      When a file with this extension is required, the compiler is called with the file Buffer and
     *      an object `{ filename }` and must return JavaScript source; the compiled module is cached,
     *      so the compiler runs once per file. Re-registering an extension replaces the compiler. The
     *      extname must start with '.' and must not be a built-in extension ('.js', '.json', '.jsc',
     *      '.wasm'); a malformed name throws ReferenceError, a reserved name throws Error 20024.
     *
     *      Example:
     *      ```JavaScript
     *      const vm = require('vm');
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-compiler-'));
     *      fs.writeFile(path.join(dir, 'x.up'), 'value');
     *
     *      const box = new vm.SandBox({});
     *      box.setModuleCompiler('.up', (buf) => 'module.exports = "' + buf.toString().trim() + '";');
     *      console.log(box.require('./x.up', dir)); // value
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param extname the extname, which must start with '.' and must not be a system built-in extension
     *      @param compiler the compile callback; files with this extname are required only once. The callback format is `compiler(buf, requireInfo)`, where buf is the read file Buffer and requireInfo has the structure `{filename: string}`.
     *
     */
    setModuleCompiler(extname: string, compiler: (buf: Class_Buffer, requireInfo: FIBJS.GeneralObject)=>any): void;

    /**
     * @description Queries the standalone global object of the sandbox
     *
     *      Returns the object passed as the constructor global argument, so `sandbox.global === global`.
     *      A sandbox created without that argument shares the host global object and has no standalone
     *      global, so reading this property throws Error 20009 (Invalid procedure call).
     *
     */
    readonly global: FIBJS.GeneralObject;

    /**
     * @description Queries a dictionary copy of all modules currently in the sandbox
     *
     *      The keys are the registered ids, including the `node:` and `fibjs:` aliases of built-in
     *      modules, and the values are the module objects. The returned object is a copy: deleting a key
     *      from it does not remove the module from the sandbox. Use has and remove to change the
     *      registry.
     *
     */
    readonly modules: FIBJS.GeneralObject;

}

