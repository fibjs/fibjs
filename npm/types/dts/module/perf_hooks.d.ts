/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/PerformanceObserver.d.ts" />
/// <reference path="../module/performance.d.ts" />
/**
 * @description perf_hooks basic module
 *
 *  Reference method:
 *  ```JavaScript
 *  var perf_hooks = require('perf_hooks');
 *  ```
 *
 */
declare module 'perf_hooks' {
    /**
     * @description The PerformanceEntry interface is an interface representing performance entries
     */
    const PerformanceObserver: typeof Class_PerformanceObserver;

    /**
     * @description performance basic performance monitoring module
     */
    const performance: typeof import ('performance');

}

