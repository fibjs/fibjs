/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Script compilation and execution object
 *
 *  Creation method
 *  ```JavaScript
 *  var Script = new vm.Script('console.log(100)');
 *  ```
 *
 */
declare class Class_Script extends Class_object {
    /**
     * @description Script object constructor
     *      @param code the script code to compile and run
     *      @param opts compile and run options
     *
     */
    constructor(code: string, opts?: FIBJS.GeneralObject);

    /**
     * @description Runs the compiled code contained in the vm.Script object within the given contextifiedObject and returns the result
     *      @param contextifiedObject the context object to run in
     *      @param opts run options
     *      @return returns the run result
     *
     */
    runInContext(contextifiedObject: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): any;

    /**
     * @description Creates a context using the given contextObject, runs the compiled code contained in the vm.Script object in it, and returns the result
     *      @param contextObject the object to be contextified
     *      @param opts run options
     *      @return returns the run result
     *
     */
    runInNewContext(contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): any;

    /**
     * @description Runs the compiled code contained in the vm.Script object in the current context and returns the result
     *      @param opts run options
     *      @return returns the run result
     *
     */
    runInThisContext(opts?: FIBJS.GeneralObject): any;

    /**
     * @description Creates code cache from the current Script object
     *      @return returns the code cache data
     *
     */
    createCachedData(): Class_Buffer;

}

