/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/**
 * @description worker 基础模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var worker_threads = require('worker_threads');
 *  ```
 *  
 */
declare module 'worker_threads' {
    /**
     * @description 独立线程工作对象，参见 Worker 
     */
    const Worker: typeof Class_Worker;

    /**
     * @description 查询当前 Worker 是不是主线程 
     */
    const isMainThread: boolean;

    /**
     * @description 查询当前 Worker 的父线程 
     */
    const parentPort: Class_Worker;

}

