/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description os_constants priority submodule, containing process priority constants
 *
 *  Usage:
 *  ```JavaScript
 *  var priority = require('os').constants.priority
 *  ```
 *
 */
declare module 'os_constants_priority' {
    /**
     * @description Low priority
     */
    export const PRIORITY_LOW: 19;

    /**
     * @description Below-normal priority
     */
    export const PRIORITY_BELOW_NORMAL: 10;

    /**
     * @description Normal priority
     */
    export const PRIORITY_NORMAL: 0;

    /**
     * @description Above-normal priority
     */
    export const PRIORITY_ABOVE_NORMAL: -7;

    /**
     * @description High priority
     */
    export const PRIORITY_HIGH: -14;

    /**
     * @description Highest priority
     */
    export const PRIORITY_HIGHEST: -20;

}

