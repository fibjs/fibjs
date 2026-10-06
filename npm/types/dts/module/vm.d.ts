/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SandBox.d.ts" />
/// <reference path="../interface/Script.d.ts" />
/**
 * @description The vm module runs JavaScript source text in the current context or in a fresh, isolated context and exposes the SandBox module registry; use it to evaluate generated code, reuse a compiled script and keep evaluated code away from host globals
 *
 *  Main capabilities:
 *
 *  - **Evaluate text**: `runInThisContext` (host context), `runInNewContext` (fresh context) and
 *    `runInContext` (a context prepared by createContext);
 *  - **Contexts**: `createContext` prepares an object as a context, `isContext` detects one;
 *  - **Compiled scripts**: the `Script` class — constructor, `runInThisContext`, `runInContext`,
 *    `runInNewContext` and `createCachedData`;
 *  - **Module registry**: the `SandBox` class, an isolated module table with its own require (see
 *    the SandBox class; it is a module loader, not a script evaluation API).
 *
 *  Concepts:
 *
 *  - **Compile vs run**: every runIn* function compiles the source text and runs it in one step;
 *    Script splits the two so one compiled script can be run any number of times in different
 *    contexts. Use createCachedData to persist the V8 code cache between processes.
 *  - **Contexts**: a context is a global object plus its own set of built-ins. createContext
 *    prepares an object and returns it marked as a context; runInNewContext prepares the object it
 *    is given. The prepared object is the visible global: its own properties are readable and
 *    writable by the script, and globals the script creates appear as its properties. Contexts do
 *    not share their globals, so the same compiled script keeps separate state in each context.
 *  - **Isolation**: a fresh context isolates the global scope but is not a security boundary; the
 *    script can still reach the host through the objects it is given, including functions such as
 *    Buffer. A fresh context provides the standard JavaScript built-ins and Buffer; it has no
 *    console, timers, require, process, module, URL or fetch. runInThisContext runs in the normal
 *    fibjs global, where all of these exist. See the SandBox class for an isolated module registry.
 *  - **SandBox and vm**: the module exposes two complementary tools. A standalone SandBox global
 *    is also a vm context (`vm.isContext(sandbox.global)` is true), so
 *    `vm.runInContext(code, sandbox.global)` evaluates code against the sandbox global, while
 *    `sandbox.require` loads modules through the sandbox registry.
 *  - **Results and errors**: the completion value of the last statement is returned; an empty
 *    script returns undefined. A syntax error is thrown during compilation as a SyntaxError with
 *    the V8 message and no error number. A runtime error is an ordinary JavaScript error whose
 *    stack names the script file; the default file name is `<anonymous>`, and filename, lineOffset
 *    and columnOffset change the reported position. Running in an object that is not a context
 *    throws Error 20003. An opts argument that is a number, boolean or array throws Error 20005;
 *    null is accepted as "no options".
 *  - **Timeout**: the timeout option (milliseconds) limits one run. A run that exceeds it fails
 *    with Error 20021 ("The maximum amount of time for a script to execute was exceeded.");
 *    Node.js reports the same condition as an ERR_SCRIPT_EXECUTION_TIMEOUT error.
 *  - **Dynamic import**: vm installs no import callback for compiled scripts, so code that calls
 *    `import()` aborts the process in this release; load modules with require instead.
 *  - **Node.js comparison**: the core surface matches Node's vm — createContext, isContext,
 *    runInContext, runInNewContext, runInThisContext, and Script with the three run methods and
 *    createCachedData. The per-member notes list the differences; the main ones are the default
 *    file name `<anonymous>` (Node.js uses `evalmachine.<anonymous>`), the `type: 'module'`
 *    compile option as a fibjs extension, and the missing Node.js members compileFunction,
 *    measureMemory, constants, SourceTextModule and SyntheticModule.
 *
 *  Import:
 *  ```JavaScript
 *  const vm = require('vm');
 *  ```
 *
 *  Example 1 — evaluate expressions and share state in the current context:
 *  ```JavaScript
 *  const vm = require('vm');
 *
 *  console.log(vm.runInThisContext('6 * 7')); // 42
 *
 *  vm.runInThisContext('globalThis.__vm_runs = (globalThis.__vm_runs || 0) + 1;');
 *  console.log(globalThis.__vm_runs); // 1
 *  delete globalThis.__vm_runs;
 *  ```
 *
 *  Example 2 — isolate code in a new context and reuse an existing one:
 *  ```JavaScript
 *  const vm = require('vm');
 *
 *  const sandbox = { count: 1 };
 *  console.log(vm.runInNewContext('count += 41; count;', sandbox)); // 42
 *  console.log(sandbox.count); // 42
 *
 *  const context = vm.createContext({ name: 'fibjs' });
 *  console.log(vm.isContext(context)); // true
 *  console.log(vm.runInContext('name + "!"', context)); // fibjs!
 *  ```
 *
 *  Example 3 — compile once, reuse the script, reuse its code cache, and bound the run time:
 *  ```JavaScript
 *  const vm = require('vm');
 *
 *  const source = 'counter = (typeof counter === "undefined" ? 0 : counter) + 1; counter;';
 *  const script = new vm.Script(source);
 *  console.log(script.runInNewContext({})); // 1
 *  console.log(script.runInNewContext({ counter: 41 })); // 42
 *
 *  console.log(new vm.Script(source, { cachedData: script.createCachedData() })
 *      .runInNewContext({ counter: 41 })); // 42
 *
 *  try {
 *      new vm.Script('while (true);').runInThisContext({ timeout: 100 });
 *  } catch (e) {
 *      console.log(e.number); // 20021
 *  }
 *  ```
 *
 */
declare module 'vm' {
    /**
     * @description The SandBox class object, see the SandBox class
     *
     *      A SandBox is an isolated module registry: it owns a module table and loads code through
     *      `sandbox.require`, which is a different concern from running script text in a context. The
     *      two meet again because a standalone sandbox global is a vm context, so it can be passed to
     *      runInContext.
     *
     */
    const SandBox: typeof Class_SandBox;

    /**
     * @description The Script class object, see the Script class
     *
     *      A Script compiles source text once and runs it in any context. The runIn* functions of this
     *      module are one-shot equivalents that compile and run in a single call.
     *
     */
    const Script: typeof Class_Script;

    /**
     * @description Prepares an object as a context and returns it
     *
     *      The object is marked as a context and becomes the global object of the new context: its own
     *      properties are readable and writable by scripts, and globals the scripts create appear as
     *      its properties. The mark is permanent; calling createContext again with a prepared object
     *      returns it unchanged, and isContext then reports true.
     *
     *      opts is accepted for Node.js compatibility and is currently ignored. Preparing a context is
     *      not a security boundary, see the module Concepts.
     *
     *      Example:
     *      ```JavaScript
     *      const vm = require('vm');
     *
     *      const context = vm.createContext({ count: 0 });
     *      vm.runInContext('count += 1;', context);
     *      console.log(context.count, vm.isContext(context)); // 1 true
     *      ```
     *
     *      @param contextObject the object to prepare as a context; a new empty object when omitted
     *      @param opts reserved options, ignored in this release
     *      @return the prepared context object
     *
     */
    function createContext(contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Returns true when the object is a prepared context
     *
     *      A true result means the object carries the context mark set by createContext or
     *      runInNewContext, or that it is the standalone global of a SandBox. A plain object returns
     *      false.
     *
     *      Example:
     *      ```JavaScript
     *      const vm = require('vm');
     *
     *      console.log(vm.isContext({})); // false
     *      console.log(vm.isContext(vm.createContext())); // true
     *      ```
     *
     *      @param contextObject the object to check
     *      @return true when the object is a prepared context
     *
     */
    function isContext(contextObject: FIBJS.GeneralObject): boolean;

    /**
     * @description Compiles the code and runs it in a prepared context, returning the completion value
     *
     *      contextifiedObject must be a context, otherwise Error 20003 is thrown. The code runs in that
     *      context's global scope, reading and writing its properties; globals it creates remain in the
     *      context for later runs.
     *
     *      opts may be an options object or a file name string equivalent to an object with filename.
     *      The compile and run options are accepted in the same object:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "filename": "<anonymous>", // file name used in error stacks
     *          "lineOffset": 0,           // number of lines added to reported positions
     *          "columnOffset": 0,         // number of columns added to reported positions
     *          "timeout": 0               // run limit in milliseconds; 0 disables the limit
     *      })
     *      ```
     *
     *      A syntax error is thrown as a SyntaxError at compile time. A run that exceeds timeout fails
     *      with Error 20021 ("The maximum amount of time for a script to execute was exceeded.");
     *      Node.js reports the same condition as ERR_SCRIPT_EXECUTION_TIMEOUT.
     *
     *      Example:
     *      ```JavaScript
     *      const vm = require('vm');
     *
     *      const context = vm.createContext({ width: 6, height: 7 });
     *      console.log(vm.runInContext('width * height', context)); // 42
     *      ```
     *
     *      @param code the source text to compile and run
     *      @param contextifiedObject the prepared context to run in; Error 20003 when it is not a context
     *      @param opts the run options object, or the script file name
     *      @return the completion value of the last statement
     *
     */
    function runInContext(code: string, contextifiedObject: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject | string): any;

    /**
     * @description Prepares an object as a context, compiles the code and runs it in that context
     *
     *      Equivalent to createContext(contextObject) followed by runInContext(code, contextObject,
     *      opts); the object remains a context afterwards, so it can be reused with runInContext. When
     *      contextObject is omitted, a new empty object is used and discarded.
     *
     *      opts may be an options object or a file name string; the options are the same as
     *      runInContext, including timeout.
     *
     *      Example:
     *      ```JavaScript
     *      const vm = require('vm');
     *
     *      const sandbox = { name: 'fibjs' };
     *      console.log(vm.runInNewContext('name.toUpperCase()', sandbox)); // FIBJS
     *      console.log(sandbox.name); // fibjs (the script did not modify it)
     *      ```
     *
     *      @param code the source text to compile and run
     *      @param contextObject the object to prepare as a context; a new empty object when omitted
     *      @param opts the run options object, or the script file name
     *      @return the completion value of the last statement
     *
     */
    function runInNewContext(code: string, contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject | string): any;

    /**
     * @description Compiles the code and runs it in the current context, returning the completion value
     *
     *      The script shares the caller global object: it can read and write global variables and use
     *      the host require, process and timers. opts may be an options object or a file name string;
     *      the options are the same as runInContext, including timeout.
     *
     *      @param code the source text to compile and run
     *      @param opts the run options object, or the script file name
     *      @return the completion value of the last statement
     *
     */
    function runInThisContext(code: string, opts?: FIBJS.GeneralObject | string): any;

}

