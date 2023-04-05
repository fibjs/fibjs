/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description querystring 模块提供了一些用于解析和序列化 URL 查询参数的实用函数，使用 querystring 模块可以方便地将 URL 查询参数解析为对象或字符串，也可以将对象序列化为 URL 查询参数字符串
 * 
 * 以下是 `querystring` 模块的常用函数：
 * 
 * - `querystring.parse(str[, sep[, eq[, options]]])`: 将 URL 查询参数解析为对象
 * - `querystring.stringify(obj[, sep[, eq[, options]]])`: 将对象序列化为 URL 查询参数字符串
 * 
 * 其中，`str` 是要解析的 URL 查询参数字符串，`obj` 是要序列化的对象。
 * 
 * 以下示例说明如何使用 `querystring` 模块从 URL 中解析查询参数为对象：
 * 
 * ```JavaScript
 * const querystring = require('querystring');
 * 
 * const url = 'https://www.example.com/path/to/page?foo=bar&baz=qux';
 * 
 * const search = new URL(url).search; // return '?foo=bar&baz=qux'
 * const query = querystring.parse(search.slice(1)); // parse query string
 * 
 * console.log(query); // output { foo: 'bar', baz: 'qux' }
 * ```
 * 
 * 以上代码先获取了一个 URL，然后从中提取出查询参数部分，并使用 `querystring.parse()` 函数将其解析为对象，最后将对象打印出来。
 * 
 * 接下来，示例说明如何使用 `querystring` 模块将对象序列化为 URL 查询参数字符串：
 * 
 * ```JavaScript
 * const querystring = require('querystring');
 * 
 * const obj = {
 *   foo: 'bar',
 *   baz: 'qux'
 * };
 * 
 * const query = querystring.stringify(obj);
 * 
 * console.log(query); // output "foo=bar&baz=qux"
 * ```
 * 
 * 以上代码中，首先定义了一个对象，然后使用 `querystring.stringify()` 函数将其序列化为 URL 查询参数字符串，最后将字符串打印出来。
 * 
 * 可以发现，使用 `querystring` 模块可以方便地对 URL 查询参数进行解析和序列化，减少了对字符串的繁琐处理，提高了代码的可读性和可维护性。
 *  
 */
declare module 'querystring' {
    /**
     * @description url 部件字符串安全编码
     *      @param str 要编码的 url
     *      @return 返回编码的字符串
     *      
     */
    function escape(str: string): string;

    /**
     * @description url 安全字符串解码
     *      @param str 要解码的 url
     *      @return 返回解码的字符串
     *      
     */
    function unescape(str: string): string;

    /**
     * @description 解析 query 字符串
     *      @param str 要解析的字符串
     *      @param sep 解析时使用的分割字符串，缺省为 &
     *      @param eq 解析时使用的赋值字符串，缺省为 =
     *      @param opt 解析参数，暂未支持
     *      @return 返回解码的对象
     *      
     */
    function parse(str: string, sep?: string, eq?: string, opt?: FIBJS.GeneralObject): Class_HttpCollection;

    /**
     * @description 序列化一个对象为 query 字符串
     *      @param obj 要序列化的对象
     *      @param sep 序列化时使用的分割字符串，缺省为 &
     *      @param eq 序列化时使用的赋值字符串，缺省为 =
     *      @param opt 解析参数，暂未支持
     *      @return 返回序列化后的字符串
     *      
     */
    function stringify(obj: FIBJS.GeneralObject, sep?: string, eq?: string, opt?: FIBJS.GeneralObject): string;

}

