/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/**
 * @description url 处理模块，url 模块提供了 Url 类，用于处理 URL 相关的操作，包括解析、组装、拼接等
 * 
 * Url 类的常用属性和方法如下：
 * 
 * 属性：
 * 
 * - href：返回完整的 URL 字符串。
 * - protocol：URL 的协议部分。
 * - host：URL 的主机部分。
 * - auth：URL 的认证部分。
 * - hostname：URL 的主机名部分。
 * - port：URL 的端口部分。
 * - pathname：URL 的路径部分。
 * - search：URL 的查询参数字符串。
 * - hash：URL 的 hash 部分。
 * 
 * 方法：
 * 
 * - resolve(from, to)：将解析 to（或 to 与 from 的组合）为一个绝对 URL，并返回解析后的 URL 对象。
 * - parse(urlString)：将 URL 字符串解析为 URL 对象并返回该对象。
 * - format(urlObject)：将 URL 对象格式化为 URL 字符串并返回。
 * 
 * 下面是一个 url 模块的示例：
 * 
 * ```JavaScript
 * const { URL } = require('url');
 * 
 * const url = new URL('http://www.baidu.com/s?ie=UTF-8&wd=fibjs#hash');
 * console.log(url.protocol); // 'http:'
 * console.log(url.host); // 'www.baidu.com'
 * console.log(url.path); // '/s?ie=UTF-8&wd=fibjs'
 * console.log(url.hash); // '#hash'
 * 
 * console.log(url.href); // 'http://www.baidu.com/s?ie=UTF-8&wd=fibjs&query=fibjs#hash'
 * ```
 * 
 * 在这个例子中，我们演示了如何使用 url 模块来解析 url 字符串，再重新组装成新的 url 字符串。
 * 
 */
declare module 'url' {
    /**
     * @description 参数构造 UrlObject 对象
     *      @param args 指定构造参数的字典对象，支持的字段有：protocol, slashes, username, password, hostname, port, pathname, query, hash
     *      @return 返回构造成功的字符串
     *      
     */
    function format(args: FIBJS.GeneralObject): string;

    /**
     * @description 解析一个 url 字符串
     *      @param url 指定需要解析的 url 字符串
     *      @param parseQueryString 指定是否解析 query
     *      @param slashesDenoteHost  默认为false, 如果设置为true，则从字符串'//'之后到下一个'/'之前的字符串会被解析为host，例如'//foo/bar', 结果应该是{host: 'foo', pathname: '/bar'}而不是{pathname: '//foo/bar'}
     *      @return 返回包含解析数据的对象
     *      
     */
    function parse(url: string, parseQueryString?: boolean, slashesDenoteHost?: boolean): Class_UrlObject;

    /**
     * @description 合并相对路径成为一个绝对路径
     * 
     *      @param _from 源路径 
     *      @param to 相对路径 
     *      @return 返回得到的绝对路径
     *      
     */
    function resolve(_from: string, to: string): string;

    /**
     * @description 创建一个 UrlObject 请求对象，参见 UrlObject 
     */
    const URL: typeof Class_UrlObject;

    /**
     * @description 将一个 url 对象转换为跨平台相关的绝对路径
     * 
     *      @param url 指定需要转换的 url 对象
     *      @return 返回转换后的绝对路径
     *     
     */
    function fileURLToPath(url: Class_UrlObject): string;

    /**
     * @description 将一个 url 字符串转换为跨平台相关的绝对路径
     * 
     *      @param url 指定需要转换的 url 字符串
     *      @return 返回转换后的绝对路径
     *     
     */
    function fileURLToPath(url: string): string;

    /**
     * @description 将一个跨平台相关的绝对路径转换为 url 对象
     *     
     *      @param path 指定需要转换的绝对路径
     *      @return 返回转换后的 url 对象
     *     
     */
    function pathToFileURL(path: string): Class_UrlObject;

}

