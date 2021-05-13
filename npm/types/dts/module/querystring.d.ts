/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description http query 处理模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var querystring = require('querystring');
 *  ```
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

