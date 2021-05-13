/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description Url 处理对象
 * 
 *  基础模块。提供 url 的格式化，解析与拼装
 *  ```JavaScript
 *  var url = new net.Url('http://www.xici.net/');
 *  var url = new net.Url({protocol: 'http:', hostname:'www.xici.net', pathname:'/'});
 *  ```
 *  
 */
declare class Class_UrlObject extends Class_object {
    /**
     * @description UrlObject 对象构造函数，使用参数构造
     *      @param args 指定构造参数的字典对象，支持的字段有：protocol, slashes, username, password, hostname, port, pathname, query, hash
     *      
     */
    constructor(args: FIBJS.GeneralObject);

    /**
     * @description UrlObject 对象构造函数，使用 url 字符串构造
     *      @param url 指定构造 url 字符串
     *      @param parseQueryString 指定是否解析 query
     *      @param slashesDenoteHost  默认为false, 如果设置为true，则从字符串'//'之后到下一个'/'之前的字符串会被解析为host，例如'//foo/bar', 结果应该是{host: 'foo', pathname: '/bar'}而不是{pathname: '//foo/bar'}
     *      
     */
    constructor(url?: string, parseQueryString?: boolean, slashesDenoteHost?: boolean);

    /**
     * @description 解析一个 url 字符串
     *      @param url 指定需要解析的 url 字符串
     *      @param parseQueryString 指定是否解析 query
     *      @param slashesDenoteHost  默认为false, 如果设置为true，则从字符串'//'之后到下一个'/'之前的字符串会被解析为host，例如'//foo/bar', 结果应该是{host: 'foo', pathname: '/bar'}而不是{pathname: '//foo/bar'}
     *      
     */
    parse(url: string, parseQueryString?: boolean, slashesDenoteHost?: boolean): void;

    /**
     * @description 使用指定的参数构造 UrlObject
     *      @param args 指定构造参数的字典对象，支持的字段有：protocol, slashes, username, password, hostname, port, pathname, query, hash
     *      
     */
    format(args: FIBJS.GeneralObject): void;

    /**
     * @description 重定位 url 路径，自动识别新路径为相对路径还是绝对路径
     *      @param url 指定新的路径
     *      @return 返回包含重定位数据的对象
     *      
     */
    resolve(url: string): Class_UrlObject;

    /**
     * @description 标准化路径
     *      
     */
    normalize(): void;

    /**
     * @description 查询和设置当前 UrlObject 对象中的完整 url 地址描述，此描述由其他所有属性组装而成
     *      
     */
    href: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的协议名称
     *      
     */
    protocol: string;

    /**
     * @description 查询和设置当前 UrlObject 对象是否包含双斜杠
     *      
     */
    slashes: boolean;

    /**
     * @description 查询和设置当前 UrlObject 对象中的完整验证字符串，由 username 和 password 属性组装而成
     *      
     */
    auth: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的验证用户
     *      
     */
    username: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的验证密码
     *      
     */
    password: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的完整主机描述，由 hastname 和 port 组装而成
     *      
     */
    host: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的主机名
     *      
     */
    hostname: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的端口号
     *      
     */
    port: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的请求完整路径（含请求），由 pathname 和 query 组装而成
     *      
     */
    path: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的路径
     *      
     */
    pathname: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的请求字符串（含“?”），等效于“?”+query
     *      
     */
    search: string;

    /**
     * @description 查询和设置当前 UrlObject 对象中的请求字符串（ 不含“?”）
     *      
     */
    query: any;

    /**
     * @description 查询和设置当前 UrlObject 对象中的请求锚点（含“\#”）
     *      
     */
    hash: string;

    /**
     * @description 查询当前 UrlObject 对象中的请求字符串（ 不含“?”）
     *      
     */
    readonly searchParams: Class_HttpCollection;

}

