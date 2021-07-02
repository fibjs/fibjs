/// <reference path="../_import/_fibjs.d.ts" />
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
     * @description performance 基础性能监控模块 
     */
    const performance: typeof import ('performance');

}

