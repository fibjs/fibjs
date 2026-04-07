/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description os_constants dlopen 子模块，包含动态链接库加载标志常量
 * 
 *  引用方法：
 *  ```JavaScript
 *  var dlopen = require('os').constants.dlopen
 *  ```
 *  
 */
declare module 'os_constants_dlopen' {
    export const RTLD_LAZY: 1;

    export const RTLD_NOW: 2;

    export const RTLD_GLOBAL: 256;

    export const RTLD_LOCAL: 0;

    export const RTLD_DEEPBIND: 8;

}

