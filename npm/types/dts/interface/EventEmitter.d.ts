/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 事件触发对象，可用于建立观察者模式，支持事件触发的对象均继承于此，同一事件的同一函数只会产生一次回调
 * 
 *  Event 对象可独立创建，以用于组建自定义的事件系统：
 *  ```JavaScript
 *  var EventEmitter = require('events');
 * 
 *  var e = new EventEmitter();
 *  ```
 *  
 */
declare class Class_EventEmitter extends Class_object {
    /**
     * @description 构造函数 
     */
    constructor();

    /**
     * @description 事件触发对象 
     */
    static EventEmitter: Class_EventEmitter;

    /**
     * @description 默认全局最大监听器数 
     */
    static defaultMaxListeners: number;

    /**
     * @description 绑定一个事件处理函数到对象
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    on(ev: string, func: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 绑定一个事件处理函数到对象
     *     @param map 指定事件映射关系，对象属性名称将作为事件名称，属性的值将作为事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 绑定一个事件处理函数到对象
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    addListener(ev: string, func: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 绑定一个事件处理函数到对象
     *     @param map 指定事件映射关系，对象属性名称将作为事件名称，属性的值将作为事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 绑定一个事件处理函数到对象起始
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    prependListener(ev: string, func: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 绑定一个事件处理函数到对象起始
     *     @param map 指定事件映射关系，对象属性名称将作为事件名称，属性的值将作为事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 绑定一个一次性事件处理函数到对象，一次性处理函数只会触发一次
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    once(ev: string, func: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 绑定一个一次性事件处理函数到对象，一次性处理函数只会触发一次
     *     @param map 指定事件映射关系，对象属性名称将作为事件名称，属性的值将作为事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 绑定一个事件处理函数到对象起始
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    prependOnceListener(ev: string, func: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 绑定一个事件处理函数到对象起始
     *     @param map 指定事件映射关系，对象属性名称将作为事件名称，属性的值将作为事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 从对象处理队列中取消指定函数
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    off(ev: string, func: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 取消对象处理队列中的全部函数
     *     @param ev 指定事件的名称
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    off(ev: string): FIBJS.GeneralObject;

    /**
     * @description 从对象处理队列中取消指定函数
     *     @param map 指定事件映射关系，对象属性名称作为事件名称，属性的值作为事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 从对象处理队列中取消指定函数
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    removeListener(ev: string, func: (...args: any[])=>any): FIBJS.GeneralObject;

    /**
     * @description 取消对象处理队列中的全部函数
     *     @param ev 指定事件的名称
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    removeListener(ev: string): FIBJS.GeneralObject;

    /**
     * @description 从对象处理队列中取消指定函数
     *     @param map 指定事件映射关系，对象属性名称作为事件名称，属性的值作为事件处理函数
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 从对象处理队列中取消所有事件的所有监听器， 如果指定事件，则移除指定事件的所有监听器。
     *     @param ev 指定事件的名称
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    removeAllListeners(ev: string): FIBJS.GeneralObject;

    /**
     * @description 从对象处理队列中取消所有事件的所有监听器， 如果指定事件，则移除指定事件的所有监听器。
     *     @param evs 指定事件的名称
     *     @return 返回事件对象本身，便于链式调用
     *    
     */
    removeAllListeners(evs?: any[]): FIBJS.GeneralObject;

    /**
     *  监听器的默认限制的数量，仅用于兼容
     *     @param n 指定事件的数量
     *    
     */
    setMaxListeners(n: number): void;

    /**
     *  获取监听器的默认限制的数量，仅用于兼容
     *     @return 返回默认限制数量
     *    
     */
    getMaxListeners(): number;

    /**
     * @description 查询对象指定事件的监听器数组
     *     @param ev 指定事件的名称
     *     @return 返回指定事件的监听器数组
     *    
     */
    listeners(ev: string): any[];

    /**
     * @description 查询对象指定事件的监听器数量
     *     @param ev 指定事件的名称
     *     @return 返回指定事件的监听器数量
     *    
     */
    listenerCount(ev: string): number;

    /**
     * @description 查询监听器事件名称
     *     @return 返回事件名称数组
     *    
     */
    eventNames(): any[];

    /**
     * @description 主动触发一个事件
     *     @param ev 事件名称
     *     @param args 事件参数，将会传递给事件处理函数
     *     @return 返回事件触发状态，有响应事件返回 true，否则返回 false
     *    
     */
    emit(ev: string, ...args: any[]): boolean;

}

