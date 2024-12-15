/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/PerformanceObserver.d.ts" />
/// <reference path="../module/performance.d.ts" />
/**
 * @description perf_hooks 基础模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var perf_hooks = require('perf_hooks');
 *  ```
 *  
 */
declare module 'perf_hooks' {
    /**
     * @description PerformanceEntry 接口用于表示性能记录的接口 
     */
    const PerformanceObserver: typeof Class_PerformanceObserver;

    /**
     * @description performance 基础性能监控模块 
     */
    const performance: typeof import ('performance');

}

