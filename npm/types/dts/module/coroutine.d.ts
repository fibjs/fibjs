/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/// <reference path="../interface/Semaphore.d.ts" />
/// <reference path="../interface/Condition.d.ts" />
/// <reference path="../interface/Event.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/Fiber.d.ts" />
/**
 * @description 并发控制模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var coroutine = require('coroutine');
 *  ```
 *  
 */
declare module 'coroutine' {
    /**
     * @description 锁对象，参见 Lock 
     */
    const Lock: typeof Class_Lock;

    /**
     * @description 信号量对象，参见 Semaphore 
     */
    const Semaphore: typeof Class_Semaphore;

    /**
     * @description 条件变量对象，参见 Condition 
     */
    const Condition: typeof Class_Condition;

    /**
     * @description 事件对象，参见 Event 
     */
    const Event: typeof Class_Event;

    /**
     * @description 独立线程工作对象，参见 Worker 
     */
    const Worker: typeof Class_Worker;

    /**
     * @description 启动一个纤程并返回纤程对象
     *      @param func 制定纤程执行的函数
     *      @param args 可变参数序列，此序列会在纤程内传递给函数
     *      @return 返回纤程对象
     *      
     */
    function start(func: (...args: any[])=>any, ...args: any[]): Class_Fiber;

    /**
     * @description 并行执行一组函数，并等待返回
     *      @param funcs 并行执行的函数数组
     *      @param fibers 限制并发 fiber 数量，缺省为 -1，启用与 funcs 数量相同 fiber
     *      @return 返回函数执行结果的数组
     *      
     */
    function parallel(funcs: any[], fibers?: number): any[];

    /**
     * @description 并行执行一个函数处理一组数据，并等待返回
     *      @param datas 并行执行的数据数组
     *      @param func 并行执行的函数
     *      @param fibers 限制并发 fiber 数量，缺省为 -1，启用与 datas 数量相同 fiber
     *      @return 返回函数执行结果的数组
     *      
     */
    function parallel(datas: any[], func: (...args: any[])=>any, fibers?: number): any[];

    /**
     * @description 并行执行一个函数多次，并等待返回
     *      @param func 并行执行的函数数
     *      @param num 重复任务数量
     *      @param fibers 限制并发 fiber 数量，缺省为 -1，启用与 funcs 数量相同 fiber
     *      @return 返回函数执行结果的数组
     *      
     */
    function parallel(func: (...args: any[])=>any, num: number, fibers?: number): any[];

    /**
     * @description 并行执行一组函数，并等待返回
     *      @param funcs 一组并行执行的函数
     *      @return 返回函数执行结果的数组
     *      
     */
    function parallel(...funcs: any[]): any[];

    /**
     * @description 返回当前纤程
     *      @return 当前纤程对象
     *      
     */
    function current(): Class_Fiber;

    /**
     * @description 暂停当前纤程指定的时间
     *      @param ms 指定要暂停的时间，以毫秒为单位，缺省为 0，即有空闲立即回恢复运行
     *      
     */
    function sleep(ms?: number): void;

    function sleep(ms?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 返回当前正在运行的全部 fiber 数组 
     */
    const fibers: any[];

    /**
     * @description 查询和设置空闲 Fiber 数量，服务器抖动较大时可适度增加空闲 Fiber 数量。缺省为 256 
     */
    var spareFibers: number;

    /**
     * @description 查询当前 vm 编号 
     */
    const vmid: number;

    /**
     * @description 修改和查询本 vm 的输出级别，用以过滤输出信息，缺省为 console.NOTSET，全部输出
     *      
     */
    var loglevel: number;

}

