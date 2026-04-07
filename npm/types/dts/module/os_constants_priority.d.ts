/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description os_constants priority 子模块，包含进程优先级常量
 * 
 *  引用方法：
 *  ```JavaScript
 *  var priority = require('os').constants.priority
 *  ```
 *  
 */
declare module 'os_constants_priority' {
    export const PRIORITY_LOW: 19;

    export const PRIORITY_BELOW_NORMAL: 10;

    export const PRIORITY_NORMAL: 0;

    export const PRIORITY_ABOVE_NORMAL: -7;

    export const PRIORITY_HIGH: -14;

    export const PRIORITY_HIGHEST: -20;

}

