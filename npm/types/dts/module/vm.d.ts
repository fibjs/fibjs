/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SandBox.d.ts" />
/// <reference path="../interface/Script.d.ts" />
/**
 * @description Sandbox module, used to isolate runtime environments of different security levels
 *
 *  By creating isolated sandboxes, you can limit the resources accessible to scripts at runtime, isolate the execution environments of different scripts, and customize base modules for different environments to ensure the security of the overall runtime environment.
 *
 *  The following example creates a sandbox that only allows access to the assert module in the global base modules, and adds two custom modules a and b:
 *  ```JavaScript
 *  var vm = require('vm');
 *  var sbox = new vm.SandBox({
 *    a: 100,
 *    b: 200,
 *    assert: require('assert')
 *  });
 *
 *  var mod_in_sbox = sbox.require('./path/to/mod');
 *  ```
 *
 */
declare module 'vm' {
    /**
     * @description Creates a SandBox object, see SandBox
     */
    const SandBox: typeof Class_SandBox;

    /**
     * @description Creates a Script object, see Script
     */
    const Script: typeof Class_Script;

    /**
     * @description Creates a context object
     *      @param contextObject specifies the object to be contextified
     *      @param opts specifies the context options
     *      @return returns the context object
     *
     */
    function createContext(contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Returns true if the given object has been contextified using vm.createContext()
     *      @param contextObject specifies the object to check
     *      @return returns true if the given object has been contextified using vm.createContext()
     *
     */
    function isContext(contextObject: FIBJS.GeneralObject): boolean;

    /**
     * @description Runs the code specified by code within the given contextifiedObject and returns the result
     *      @param code specifies the script code to compile and run
     *      @param contextifiedObject specifies the context object at runtime
     *      opts may be the running options object, or the script file name.
     *      @param opts the running options or the script file name
     *      @return returns the running result
     *
     */
    function runInContext(code: string, contextifiedObject: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject | string): any;

    /**
     * @description Uses the given contextObject to create a context, runs the code specified by code in it and returns the result
     *      @param code specifies the script code to compile and run
     *      @param contextObject specifies the object to be contextified
     *      opts may be the running options object, or the script file name.
     *      @param opts the running options or the script file name
     *      @return returns the running result
     *
     */
    function runInNewContext(code: string, contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject | string): any;

    /**
     * @description Runs the code specified by code in the current context and returns the result
     *      @param code specifies the script code to compile and run
     *      opts may be the running options object, or the script file name.
     *      @param opts the running options or the script file name
     *      @return returns the running result
     *
     */
    function runInThisContext(code: string, opts?: FIBJS.GeneralObject | string): any;

}

