/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description EventEmitter 是事件触发对象，它可以被用于建立观察者模式，支持事件触发的对象均继承于此
 * 
 * 当一个事件被触发时，所有与该事件相关联的监听器会以异步方式被调用。它还允许我们创建具有高度可定制性和灵活性的代码。
 * 
 * 常用函数包括：addListener/on、once、removeListener/off、removeAllListeners 和 emit。
 * 
 * 下面是一个示例代码：
 * 
 * ```javascript
 * var fs = require('fs');
 * var EventEmitter = require('events');
 * var event = new EventEmitter();
 * 
 * event.on('read_file', function(filename) {
 *   fs.readFile(filename, 'utf8', function(err, data) {
 *     if (err) {
 *       event.emit('error', err);
 *       return;
 *     }
 *     event.emit('show_content', data);
 *   });
 * });
 * 
 * event.on('error', function(err) {
 *   console.log(`Error ${err}`);
 * });
 * 
 * event.on('show_content', function(content) {
 *   console.log(content);
 * });
 * 
 * event.emit('read_file', 'test.txt');
 * ```
 * 
 * 上述示例代码，当运行时，事件emitter实例event首先监听'read_file'事件，然后在事件触发时(`event.emit('read_file', 'test.txt')`)触发读取文件的操作。当读取成功后，会触发'show_content'事件，此时监听了'show_content'事件的函数就会被执行并显示文件内容。如果在读取文件过程中发生错误，则会触发'error'事件，此时操作失败的情况就得到了应对。
 * 
 * 这种模式在应对异步操作的业务场景中具有很好的优越性。
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
     * @description 绑定一个事件处理函数到对象
     * 
     *     options 参数是一个对象，它可以包含以下属性：
     *     - once: 如果为 true，则事件处理函数只会触发一次，触发后会被移除
     * 
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @param options 指定事件处理函数的选项
     *     @return 返回事件对象本身，便于链式调用
     *     
     */
    addEventListener(ev: string, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

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
     * @description 从对象处理队列中取消指定函数
     *     @param ev 指定事件的名称
     *     @param func 指定事件处理函数
     *     @param options 指定事件处理函数的选项
     *     @return 返回事件对象本身，便于链式调用
     *     
     */
    removeEventListener(ev: string, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

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
     * @description 查询对象指定事件的监听器数量
     *     @param o 指定查询的对象
     *     @param ev 指定事件的名称
     *     @return 返回指定事件的监听器数量
     *    
     */
    listenerCount(o: any, ev: string): number;

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

