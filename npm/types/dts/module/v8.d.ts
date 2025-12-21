/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HeapSnapshot.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 内存 v8 模块
 * 
 * `v8` 模块是一个用于分析和监控 JavaScript 应用程序内存使用情况的工具。它提供了一系列方法，可以帮助开发者深入了解应用程序的内存分配和垃圾回收行为，从而优化性能和内存使用。
 * 
 * 在 JavaScript 中，内存管理是由垃圾回收机制自动处理的。然而，对于复杂的应用程序，特别是那些处理大量数据或长时间运行的应用程序，内存泄漏和不必要的内存占用可能会成为性能瓶颈。通过使用 `v8` 模块，开发者可以获取详细的内存使用情况，并识别和解决潜在的内存问题。
 * 
 * `v8` 模块提供了以下主要功能：
 * 
 * 1. **获取堆内存统计信息**：包括堆内存的总体使用情况、各个堆空间的详细使用情况以及堆中代码的统计信息。
 * 2. **堆快照**：可以保存和加载堆快照，记录特定时间点的堆内存状态。
 * 3. **堆快照对比**：执行给定的函数，并对比执行前后堆内存的变化，帮助识别内存分配和回收的差异。
 * 4. **运行状态采样**：启动一次运行状态采样日志，记录指定时间段内的内存使用情况。
 * 
 * 以下是一些使用 `v8` 模块的示例代码，展示了如何获取堆内存统计信息、保存和加载堆快照以及启动运行状态采样。
 * 
 * ```javascript
 * // 引入 v8 模块
 * var v8 = require('v8');
 * 
 * // 获取堆内存统计信息
 * var heapStats = v8.getHeapStatistics();
 * console.log('Heap Statistics:', heapStats);
 * 
 * // 获取堆空间统计信息
 * var heapSpaceStats = v8.getHeapSpaceStatistics();
 * console.log('Heap Space Statistics:', heapSpaceStats);
 * 
 * // 保存堆快照
 * v8.saveSnapshot('snapshot1');
 * 
 * // 加载堆快照
 * var snapshot = v8.loadSnapshot('snapshot1');
 * console.log('Loaded Snapshot:', snapshot);
 * 
 * // 获取当前时间节点的堆快照
 * var currentSnapshot = v8.takeSnapshot();
 * console.log('Current Snapshot:', currentSnapshot);
 * 
 * // 对比执行前后的堆内存变化
 * function testFunction() {
 *     // 模拟一些内存分配操作
 *     var arr = [];
 *     for (var i = 0; i < 10000; i++) {
 *         arr.push({index: i});
 *     }
 * }
 * var diffResult = v8.diff(testFunction);
 * console.log('Heap Diff Result:', diffResult);
 * 
 * // 启动运行状态采样日志
 * var timer = v8.start('samplingLog', 60000, 100);
 * console.log('Sampling started, timer:', timer);
 * 
 * // 停止采样
 * // timer.clear();
 * ```
 * 
 * 通过这些功能和方法，开发者可以更好地监控和优化 JavaScript 应用程序的内存使用情况，提高应用程序的性能和稳定性。
 *  
 */
declare module 'v8' {
    /**
     * @description 隐藏节点，当显示给用户时可以被过滤掉 
     */
    export const Node_Hidden: 0;

    /**
     * @description 数组 
     */
    export const Node_Array: 1;

    /**
     * @description 字符串 
     */
    export const Node_String: 2;

    /**
     * @description JS对象（字符串和数组除外） 
     */
    export const Node_Object: 3;

    /**
     * @description 编译后的代码 
     */
    export const Node_Code: 4;

    /**
     * @description 函数闭包 
     */
    export const Node_Closure: 5;

    /**
     * @description 正则表达式 
     */
    export const Node_RegExp: 6;

    /**
     * @description 堆中排好序的数字 
     */
    export const Node_HeapNumber: 7;

    /**
     * @description Native对象（非v8堆上的） 
     */
    export const Node_Native: 8;

    /**
     * @description Synthetic对象 
     */
    export const Node_Synthetic: 9;

    /**
     * @description 拼接的字符串 
     */
    export const Node_ConsString: 10;

    /**
     * @description 分割的字符串 
     */
    export const Node_SlicedString: 11;

    /**
     * @description 符号（ES6） 
     */
    export const Node_Symbol: 12;

    /**
     * @description 堆中排好序的SIMD值(ES7) 
     */
    export const Node_SimdValue: 13;

    /**
     * @description 函数中的变量 
     */
    export const Edge_ContextVariable: 0;

    /**
     * @description 数组中的元素 
     */
    export const Edge_Element: 1;

    /**
     * @description 有名对象的属性 
     */
    export const Edge_Property: 2;

    /**
     * @description JS无法进入的链接 
     */
    export const Edge_Internal: 3;

    /**
     * @description 指向需要事先计算出空间大小的节点 
     */
    export const Edge_Hidden: 4;

    /**
     * @description 指向无法事先计算出空间大小的节点 
     */
    export const Edge_Shortcut: 5;

    /**
     * @description 一个弱引用（被GC忽视） 
     */
    export const Edge_Weak: 6;

    /**
     * @description 获取= v8 堆中代码及其元数据的统计信息
     *      @return 返回元数据的统计信息
     *      
     */
    function getHeapCodeStatistics(): FIBJS.GeneralObject;

    /**
     * @description 获取 v8 堆内存的详细使用情况
     *      @return 返回堆内存的详细使用情况
     *      
     */
    function getHeapSpaceStatistics(): any[];

    /**
     * @description 获取 v8 堆内存使用情况的统计信息
     *      @return 返回堆内存使用情况的统计信息
     *      
     */
    function getHeapStatistics(): FIBJS.GeneralObject;

    /**
     * @description 根据指定名称保存一个堆快照
     * 	 @param fname 堆快照名称
     * 	 
     */
    function saveSnapshot(fname: string): void;

    /**
     * @description 根据指定名称读取一个堆快照
     * 	 @param fname 堆快照名称
     * 	 @return 返回读取到的堆快照
     * 	 
     */
    function loadSnapshot(fname: string): Class_HeapSnapshot;

    /**
     * @description 获取当前时间节点的堆快照，堆快照记录了当前时刻JS堆的状态
     * 	 @return 返回获取到的堆信息快照
     * 	 
     */
    function takeSnapshot(): Class_HeapSnapshot;

    /**
     * @description 执行给定的函数，并对比执行前后 v8 堆的变化
     * 	 @param test 给定要测试的函数
     * 	 @return 返回对比的结果
     * 	 
     */
    function diff(test: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 启动一次运行状态采样日志
     * 	 @param fname 给定日志存储文件名
     * 	 @param time 指定采样时间，缺省 1 分钟
     * 	 @param interval 指定间隔时间，缺省 100 毫秒
     *      @return 返回采样定时器，可以通过 clear 方法提前停止采样
     * 	 
     */
    function start(fname: string, time?: number, interval?: number): Class_Timer;

    /**
     * @description 将值序列化为 Buffer
     *      
     *      使用 V8 的序列化格式将任意 JavaScript 值转换为二进制数据。支持循环引用、TypedArray、Map、Set、Date、RegExp、Error 等类型。
     *      不支持函数、Symbol、WeakMap、WeakSet 等类型。
     *      @param value 要序列化的值
     *      @return 返回序列化后的 Buffer
     *      
     */
    function serialize(value: any): Class_Buffer;

    /**
     * @description 将 Buffer 反序列化为值
     *      
     *      将之前通过 serialize 序列化的二进制数据还原为 JavaScript 值。
     *      @param data 要反序列化的 Buffer
     *      @return 返回反序列化后的值
     *      
     */
    function deserialize(data: Class_Buffer): any;

}

