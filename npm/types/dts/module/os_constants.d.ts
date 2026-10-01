/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/os_constants_errno.d.ts" />
/// <reference path="../module/os_constants_signals.d.ts" />
/// <reference path="../module/os_constants_priority.d.ts" />
/// <reference path="../module/os_constants_dlopen.d.ts" />
/**
 * @description Constant definitions for the os module
 *
 *  Usage:
 *  ```JavaScript
 *  var constants = require('os').constants
 *  ```
 *
 */
declare module 'os_constants' {
    /**
     * @description UDP address reuse flag
     */
    export const UV_UDP_REUSEADDR: 4;

    /**
     * @description errno error code constants sub-object
     */
    const errno: typeof import ('os_constants_errno');

    /**
     * @description signal constants sub-object
     */
    const signals: typeof import ('os_constants_signals');

    /**
     * @description process priority constants sub-object
     */
    const priority: typeof import ('os_constants_priority');

    /**
     * @description dynamic library loading flag constants sub-object
     */
    const dlopen: typeof import ('os_constants_dlopen');

}

