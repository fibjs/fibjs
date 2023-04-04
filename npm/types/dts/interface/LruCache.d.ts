/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description LruCache(Least Recently Used Cache) 是用来维护LRU缓存的类。它可以存储一定数量的值，并且保持了容器的尺寸。当新数据进来时，如果容器还没有到达尺寸限制，新数据可以直接加入到容器中。如果容器已满，则容器将淘汰最近最少使用的数据
 * 
 * 我们可以通过如下方式来使用：
 * 
 * ```JavaScript
 * const util = require('util')
 * const c = new util.LruCache(10, 100) // 缓存最大尺寸为10，元素失效时间为100ms
 * ```
 * 
 * 其中，set() 是设置键值对的接口：
 * 
 * ```JavaScript
 * LruCache.set(String name, Value value);
 * ```
 * 
 * name 参数指定要设定的键值，value 参数指定要设定的值。
 * 
 * `LruCache` 的 `get` 方法可以用回调函数的方式更新缓存数据：
 * ```JavaScript
 * var c = new util.LruCache(10, 1000); // 最多缓存 10 个值，1 秒之后数据过期
 * 
 * function get_data(name) {
 *     // 从其他数据源查询数据，如数据库、缓存、文件等
 *     // ...
 *     var data = {'name': name, 'value': Math.random()};
 *     console.log('update data: ' + JSON.stringify(data)); // 记录缓存更新的信息
 *     return data;
 * }
 * 
 * console.log(c.get('a', get_data));
 * console.log(c.get('a', get_data));
 * ```
 * 执行结果：
 * ```sh
 * update data: {"name":"a","value":0.4019124971556616}
 * {"name":"a","value":0.4019124971556616} // 第一次查询需要缓存数据，会调用 updater 函数
 * {"name":"a","value":0.4019124971556616} // 第二次查询直接从缓存中获取数据，不需要再调用 updater 函数
 * ```
 * 
 * 具体使用 LruCache 时，建议开发人员遵循以下的最佳实践：
 * 
 * - 可以在后端数据处理过程中，以某个特定时间点为新鲜点，设定失效时间，来使用 LruCache 优化应用性能。
 * - 在对应的场景下分析业务决策。例如更频发的数据更新，需要配置更短的失效时间，而更新频率较低的缓存数据则不需要设定太短的失效时间。
 * - 在不同使用场景中，考虑不同数据访问模式，使用合适的 LruCache 类实例。
 *  
 */
declare class Class_LruCache extends Class_EventEmitter {
    /**
     * @description LruCache 对象构造函数
     *      @param size 缓存最大尺寸
     *      @param timeout 元素失效时间，单位是 ms，小于等于 0 不失效，缺省为 0
     *      
     */
    constructor(size: number, timeout?: number);

    /**
     * @description 查询容器内数值个数 
     */
    readonly size: number;

    /**
     * @description 查询和设置容器内元素失效时间，单位是 ms，小于等于 0 不失效 
     */
    timeout: number;

    /**
     * @description 清除容器数据 
     */
    clear(): void;

    /**
     * @description 检查容器内是否存在指定键值的数据
     *      @param name 指定要检查的键值
     *      @return 返回键值是否存在
     *      
     */
    has(name: string): boolean;

    /**
     * @description 查询指定键值的值
     *      @param name 指定要查询的键值
     *      @return 返回键值所对应的值，若不存在，则返回 undefined
     *      
     */
    get(name: string): any;

    /**
     * @description 查询指定键值的值，若不存在或过期，则调用回调函数更新数据
     *      @param name 指定要查询的键值
     *      @param updater 指定更新函数
     *      @return 返回键值所对应的值
     *      
     */
    get(name: string, updater: (...args: any[])=>any): any;

    /**
     * @description 设定一个键值数据，键值不存在则插入一条新数据
     *      @param name 指定要设定的键值
     *      @param value 指定要设定的数据
     *      
     */
    set(name: string, value: any): void;

    /**
     * @description 设定一个键值数据，键值不存在则插入新数据
     *      @param map 指定要设定的键值数据字典
     *      
     */
    set(map: FIBJS.GeneralObject): void;

    /**
     * @description 删除指定键值的全部值
     *      @param name 指定要删除的键值
     *      
     */
    remove(name: string): void;

    /**
     * @description 检查容器是否为空
     *      @return 容器内无数值则返回 true
     *      
     */
    isEmpty(): boolean;

    /**
     * @description 查询和绑定数据超时事件，相当于 on("expire", func); 
     */
    onexpire: (...args: any[])=>any;

}

