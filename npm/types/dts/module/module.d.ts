/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Basic module management
 */
declare module 'module' {
    /**
     * !@description Creates a module require function
     *      @param base the base path of the module
     *      @return returns a module require function
     *
     */
    function createRequire(base: string): (...args: any[])=>any;

    /**
     * !@description Built-in module name list
     *      The built-in module name list. Contains all fibjs built-in module names, and the versions with the node: prefix.
     *
     */
    const builtinModules: any[];

    /**
     * !@description Enables module compile cache
     *      In fibjs it is a no-op; used to cache V8 compiled bytecode to disk to speed up subsequent startup.
     *      @param cacheDir the cache directory path, optional
     *      @return an object containing status and message; status 2 means not supported
     *
     */
    function enableCompileCache(cacheDir?: string): FIBJS.GeneralObject;

}

