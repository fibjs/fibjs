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
    /**
     * @description 延迟绑定，符号在使用时才解析 
     */
    export const RTLD_LAZY: 1;

    /**
     * @description 立即绑定，加载时解析全部符号 
     */
    export const RTLD_NOW: 2;

    /**
     * @description 符号对后续加载的库全局可见 
     */
    export const RTLD_GLOBAL: 256;

    /**
     * @description 符号仅对当前库可见 
     */
    export const RTLD_LOCAL: 0;

    /**
     * @description 优先使用库自身的符号 
     */
    export const RTLD_DEEPBIND: 8;

}

