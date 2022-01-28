/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description performance 基础性能监控模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var performance = require('perf_hooks').performance;
 *  ```
 *  
 */
declare module 'performance' {
    /**
     * @description 查询当前进程时间 
     *      @return 返回当前进程时间
     */
    function now(): number;

}

