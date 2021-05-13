/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/**
 * @description url 处理模块 
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

}

