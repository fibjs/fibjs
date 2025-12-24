/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description Headers 是一个专门用于处理 HTTP 头部信息的容器类，继承自 HttpCollection
 * 
 * Headers 实现了标准的 HTTP Headers API，同时作为全局 Headers 对象和 http.Headers 的实现类。它提供了完整的 HTTP 头部管理功能，支持标准的 HTTP 头部字段操作，继承了 HttpCollection 的所有功能，包括添加、设置、查询和删除头部字段。
 * 
 * Headers 支持以下几种使用方式：
 * 
 * 1. 作为全局 Headers API 使用（Web 标准）：
 * 
 * ```JavaScript
 * // Create empty Headers object
 * const headers = new Headers();
 * 
 * // Initialize with object
 * const headers = new Headers({
 *     'Content-Type': 'application/json',
 *     'Accept': 'application/json'
 * });
 * 
 * // Initialize with array
 * const headers = new Headers([
 *     ['Content-Type', 'application/json'],
 *     ['Accept', 'application/json']
 * ]);
 * 
 * // Copy from another Headers object
 * const copy = new Headers(headers);
 * ```
 * 
 * 2. 作为 http.Headers 使用（fibjs 扩展）：
 * 
 * ```JavaScript
 * const headers = new http.Headers({
 *     'User-Agent': 'fibjs/1.0',
 *     'Accept': 'text/html'
 * });
 * ```
 * 
 * Headers API 标准方法示例：
 * 
 * ```JavaScript
 * // Standard Headers API methods
 * headers.set('Content-Type', 'text/html; charset=utf-8');
 * headers.append('Accept', 'application/json');
 * headers.get('Content-Type');  // 'text/html; charset=utf-8'
 * headers.has('Accept');        // true
 * headers.delete('User-Agent');
 * 
 * // Iterator support
 * for (const [name, value] of headers) {
 *     console.log(`${name}: ${value}`);
 * }
 * 
 * // Iterate over keys
 * for (const name of headers.keys()) {
 *     console.log(name);
 * }
 * 
 * // Iterate over values
 * for (const value of headers.values()) {
 *     console.log(value);
 * }
 * 
 * // forEach method
 * headers.forEach((value, name) => {
 *     console.log(`${name}: ${value}`);
 * });
 * ```
 * 
 * fibjs 扩展方法示例（继承自 HttpCollection）：
 * 
 * ```JavaScript
 * // Add multiple values (without overwriting existing)
 * headers.add('Cache-Control', 'no-cache');
 * 
 * // Get first value
 * const auth = headers.first('Authorization');
 * 
 * // Get all values
 * const cookies = headers.all('Set-Cookie');
 * 
 * // Set multiple cookies
 * headers.set('Set-Cookie', [
 *     'sessionId=abc123; Path=/',
 *     'userId=456; Path=/; HttpOnly'
 * ]);
 * ```
 * 
 * Headers 自动处理头部字段名的大小写不敏感特性，完全遵循 HTTP 协议规范和 Web 标准 Headers API。
 *  
 */
declare class Class_Headers extends Class_HttpCollection {
    /**
     * @description Headers 构造函数，创建一个新的空 HTTP 头部容器 
     */
    constructor();

    /**
     * @description Headers 构造函数，使用给定的对象初始化 HTTP 头部容器
     *      @param init 初始化用的头部字段对象，键为头部字段名，值为头部字段值
     *      
     */
    constructor(init: FIBJS.GeneralObject);

    /**
     * @description Headers 构造函数，使用给定的数组初始化 HTTP 头部容器
     *      @param init 初始化用的头部字段数组，每个元素为一个包含头部字段名和头部字段值的数组
     *      
     */
    constructor(init: any[]);

    /**
     * @description Headers 构造函数，使用给定的 HTTP 头部容器初始化 HTTP 头部容器
     *      @param init 初始化用的 HTTP 头部容器
     *      
     */
    constructor(init: Class_Headers);

}

