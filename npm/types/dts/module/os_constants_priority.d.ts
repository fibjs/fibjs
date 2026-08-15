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
    /**
     * @description 低优先级 
     */
    export const PRIORITY_LOW: 19;

    /**
     * @description 低于正常优先级 
     */
    export const PRIORITY_BELOW_NORMAL: 10;

    /**
     * @description 正常优先级 
     */
    export const PRIORITY_NORMAL: 0;

    /**
     * @description 高于正常优先级 
     */
    export const PRIORITY_ABOVE_NORMAL: -7;

    /**
     * @description 高优先级 
     */
    export const PRIORITY_HIGH: -14;

    /**
     * @description 最高优先级 
     */
    export const PRIORITY_HIGHEST: -20;

}

