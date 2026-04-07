/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/os_constants_errno.d.ts" />
/// <reference path="../module/os_constants_signals.d.ts" />
/// <reference path="../module/os_constants_priority.d.ts" />
/// <reference path="../module/os_constants_dlopen.d.ts" />
/**
 * @description os 模块常量定义模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var constants = require('os').constants
 *  ```
 *  
 */
declare module 'os_constants' {
    /**
     * @description UDP 地址重用标志 
     */
    export const UV_UDP_REUSEADDR: 4;

    /**
     * @description errno 错误码常量子对象 
     */
    const errno: typeof import ('os_constants_errno');

    /**
     * @description 信号常量子对象 
     */
    const signals: typeof import ('os_constants_signals');

    /**
     * @description 进程优先级常量子对象 
     */
    const priority: typeof import ('os_constants_priority');

    /**
     * @description 动态链接库加载标志常量子对象 
     */
    const dlopen: typeof import ('os_constants_dlopen');

}

