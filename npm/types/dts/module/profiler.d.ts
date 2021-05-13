/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HeapSnapshot.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description 内存 profiler 模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var profiler = require('profiler');
 *  ```
 *  
 */
declare module 'profiler' {
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

}

