/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description HttpCollection 是一个通用容器，用于处理 http 消息中的 headers, query, form, cookie 数据
 * 
 * 我们以 headers 为例，说明 HttpCollection 的使用方法。
 * 
 * HttpCollection 支持三种形式添加数据：
 * 
 * 1. 添加一个键值数据，添加数据并不修改已存在的键值的数据。`add`
 * 
 * ```JavaScript
 * headers.add({
 *     'Content-Type': 'text/plain',
 *     'User-Agent': 'fibjs'
 * });
 * ```
 * 
 * 2. 添加一个键值的一组数据，添加数据并不修改已存在的键值的数据。`add`
 * 
 * ```JavaScript
 * headers.add('Set-Cookie', [
 *     'a=10',
 *     'b=20'
 * ]);
 * ```
 * 
 * 3. 添加一个键值数据，添加数据并不修改已存在的键值的数据。`add`
 * 
 * ```JavaScript
 * headers.add('Accept-Encoding', 'gzip');
 * ```
 * 
 * HttpCollection 设置数据的形式与添加相同，使用的方法是 `set`。
 * 
 * 我们可以使用 `has` 检查容器内是否存在指定键值的数据
 * 
 * ```JavaScript
 * const contentTypeExists = headers.has('Content-Type');
 * ```
 * 
 * 使用 `first` 获取容器内某个键对应的第一个值：
 * 
 * ```JavaScript
 * const contentType = headers.first('Content-Type');
 * ```
 * 
 * 使用 `all` 查询指定键值的全部值，返回一个数组。如果传递一个空字符串的参数，则返回全部值
 * 
 * ```JavaScript
 * const cookieArray = headers.all('Set-Cookie');
 * const alls = headers.all();
 * ```
 * 
 * 使用 `delete` 方法删除指定键值的所有数据，返回 `true` 表示 
 */
declare class Class_HttpCollection extends Class_object {
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
     * @description 查询指定键值的第一个值
     *      @param name 指定要查询的键值
     *      @return 返回键值所对应的值，若不存在，则返回 undefined
     *      
     */
    first(name: string): any;

    /**
     * @description 查询指定键值的第一个值，等同于 first
     *      @param name 指定要查询的键值
     *      @return 返回键值所对应的值，若不存在，则返回 undefined
     *      
     */
    get(name: string): any;

    /**
     * @description 查询指定键值的全部值
     *      @param name 指定要查询的键值，传递空字符串返回全部键值的结果
     *      @return 返回键值所对应全部值的数组，若数据不存在，则返回 null
     *      
     */
    all(name?: string): FIBJS.GeneralObject;

    /**
     * @description 添加一个键值数据，添加数据并不修改已存在的键值的数据
     *      @param map 指定要添加的键值数据字典
     *      
     */
    append(map: FIBJS.GeneralObject): void;

    /**
     * @description 添加一个键值的一组数据，添加数据并不修改已存在的键值的数据
     *      @param name 指定要添加的键值
     *      @param values 指定要添加的一组数据
     *      
     */
    append(name: string, values: any[]): void;

    /**
     * @description 添加一组数据，添加数据并不修改已存在的键值的数据
     *      @param entries 指定要添加的一组数据，格式为 [[<key>, <value>]]
     *      
     */
    append(entries: any[]): void;

    /**
     * @description 添加一个键值数据，添加数据并不修改已存在的键值的数据
     *      @param name 指定要添加的键值
     *      @param value 指定要添加的数据
     *      
     */
    append(name: string, value: any): void;

    /**
     * @description 设定一个键值数据，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余数据
     *      @param map 指定要设定的键值数据字典
     *      
     */
    set(map: FIBJS.GeneralObject): void;

    /**
     * @description 设定一个键值的一组数据，设定数据将修改键值所对应的数值，并清除相同键值的其余数据
     *      @param name 指定要设定的键值
     *      @param values 指定要设定的一组数据
     *      
     */
    set(name: string, values: any[]): void;

    /**
     * @description 设定一个键值数据，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余数据
     *      @param name 指定要设定的键值
     *      @param value 指定要设定的数据
     *      
     */
    set(name: string, value: any): void;

    /**
     * @description 删除指定键值的全部值
     *      @param name 指定要删除的键值
     *      
     */
    remove(name: string): void;

    /**
     * @description 删除指定键值的全部值
     *      @param name 指定要删除的键值
     *      
     */
    delete(name: string): void;

    /**
     * @description 按照键值排序容器内的内容
     *      
     */
    sort(): void;

    /**
     * @description 遍历容器内的内容
     *      @param callback 指定遍历时调用的函数，函数参数为 (value, key, object)
     *      
     */
    forEach(callback: (...args: any[])=>any): void;

    /**
     * @description 遍历容器内的内容
     *      @param callback 指定遍历时调用的函数，函数参数为 (value, key, object)
     *      @param thisArg 指定回调函数的 this 对象
     *      
     */
    forEach(callback: (...args: any[])=>any, thisArg: any): void;

    /**
     * @description 查询容器内的键值
     *      @return 返回包含所有键值的迭代器
     *      
     */
    keys(): Iterator<any>;

    /**
     * @description 查询容器内的数值
     *      @return 返回包含所有数值的迭代器
     *      
     */
    values(): Iterator<any>;

    /**
     * @description 查询容器内的键值和数值
     *      @return 返回包含所有键值和数值的迭代器
     *      
     */
    entries(): Iterator<any>;


    "[Symbol.iterator]"(): Iterator<any>;

}

