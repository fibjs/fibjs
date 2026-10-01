/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description os_constants dlopen submodule, containing dynamic library load flag constants
 *
 *  Usage:
 *  ```JavaScript
 *  var dlopen = require('os').constants.dlopen
 *  ```
 *
 */
declare module 'os_constants_dlopen' {
    /**
     * @description Lazy binding, symbols are resolved when used
     */
    export const RTLD_LAZY: 1;

    /**
     * @description Immediate binding, all symbols are resolved at load time
     */
    export const RTLD_NOW: 2;

    /**
     * @description Symbols are globally visible to subsequently loaded libraries
     */
    export const RTLD_GLOBAL: 256;

    /**
     * @description Symbols are visible only to the current library
     */
    export const RTLD_LOCAL: 0;

    /**
     * @description Prefer the library's own symbols
     */
    export const RTLD_DEEPBIND: 8;

}

