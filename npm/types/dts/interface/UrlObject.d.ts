/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/**
 * @description URL 对象，实现 WHATWG URL 标准，用于解析、构造和操作 URL
 * 
 * UrlObject 提供了完整的 URL 处理功能，兼容现代 Web 标准和 Node.js URL API。它支持 URL 的解析、构造、修改和格式化，并提供了丰富的属性和方法来操作 URL 的各个组成部分。
 * 
 * ## 主要特性
 * 
 * - **标准兼容**: 实现 WHATWG URL 标准和 Node.js URL API
 * - **Unicode 支持**: 完整支持国际化域名 (IDN) 和 Unicode 字符
 * - **查询参数**: 集成 URLSearchParams 提供强大的查询参数操作
 * - **路径处理**: 自动处理路径规范化和相对路径解析
 * 
 * ## 创建 URL 对象
 * 
 * ### 1. 使用字符串创建
 * 
 * ```JavaScript
 * const url = require('url');
 * 
 * // 使用完整 URL 字符串
 * const myURL = new URL('https://user:pass@example.com:8080/path?query=value#hash');
 * 
 * // 使用相对 URL 和基础 URL
 * const relativeURL = new URL('/api/users', 'https://example.com');
 * console.log(relativeURL.href); // 'https://example.com/api/users'
 * ```
 * 
 * ### 2. 使用对象构造
 * 
 * ```JavaScript
 * const myURL = new URL({
 *   protocol: 'https:',
 *   hostname: 'example.com',
 *   port: '8080',
 *   pathname: '/api/data',
 *   search: '?format=json'
 * });
 * ```
 * 
 * ## URL 组成部分
 * 
 * 一个完整的 URL 包含以下部分：
 * ```
 * https://user:pass@example.com:8080/path/to/resource?query=value#fragment
 *  \___/   \______/ \_________/ \__/\________________/\___________/ \______/
 *    |        |         |        |          |             |          |
 * protocol   auth      host     port     pathname        search      hash
 *           \___________________/
 *                    origin
 * ```
 * 
 * ## 常用方法
 * 
 * ```JavaScript
 * const myURL = new URL('https://example.com/old-path');
 * 
 * // 解析 URL 字符串
 * const parsed = URL.parse('https://example.com/path');
 * 
 * // 检查 URL 是否有效
 * const isValid = URL.canParse('https://example.com');
 * 
 * // 重定位到新路径
 * const newURL = myURL.resolve('../new-path');
 * ```
 *  
 */
declare class Class_UrlObject extends Class_object {
    /**
     * @description 使用参数对象构造 URL 对象
     *      @param args 构造参数对象，支持的字段有：protocol, slashes, username, password, hostname, port, pathname, query, hash
     *      
     */
    constructor(args?: FIBJS.GeneralObject);

    /**
     * @description 使用 URL 字符串构造 URL 对象
     *      @param url 要解析的 URL 字符串，可以是绝对 URL 或相对 URL
     *      @param base 基础 URL 字符串，当 url 参数是相对 URL 时使用
     *      
     */
    constructor(url: string, base?: string);

    /**
     * @description 解析 URL 字符串并返回 URL 对象，解析失败时返回 null
     *      @param url 要解析的 URL 字符串
     *      @param base 基础 URL 字符串，当 url 是相对 URL 时使用
     *      @return 成功时返回 UrlObject 对象，解析失败时返回 null
     *      
     */
    static parse(url: string, base?: string): Class_UrlObject;

    /**
     * @description 检查 URL 字符串是否可以成功解析
     *      @param url 要检查的 URL 字符串
     *      @param base 基础 URL 字符串，当 url 是相对 URL 时使用
     *      @return 可以解析返回 true，否则返回 false
     *      
     */
    static canParse(url: string, base?: string): boolean;

    /**
     * @description 解析相对 URL 并返回新的绝对 URL 对象
     *      @param url 要解析的相对或绝对 URL 字符串
     *      @return 返回解析后的新 UrlObject 对象
     *      
     */
    resolve(url: string): Class_UrlObject;

    /**
     * @description 完整的 URL 字符串
     *      
     *      获取或设置完整的 URL 字符串。设置此属性时会自动解析并更新其他属性。
     *      
     */
    href: string;

    /**
     * @description URL 协议部分（包含冒号）
     *      
     *      例如：'http:', 'https:', 'ftp:', 'file:' 等
     *      
     */
    protocol: string;

    /**
     * @description 是否包含双斜杠
     *      
     *      指示 URL 是否使用双斜杠格式（如 http://）
     *      
     */
    slashes: boolean;

    /**
     * @description URL 的来源（协议 + 主机 + 端口）
     *      
     *      只读属性，返回格式如：'https://example.com:8080'
     *      对于非网络协议（如 file:）返回 'null'
     *      
     */
    readonly origin: string;

    /**
     * @description 认证信息（用户名:密码）
     *      
     *      只读属性，返回格式如：'username:password'
     *      
     */
    readonly auth: string;

    /**
     * @description 用户名部分
     *      
     *      URL 中的用户名，用于 HTTP 基础认证
     *      
     */
    username: string;

    /**
     * @description 密码部分
     *      
     *      URL 中的密码，用于 HTTP 基础认证
     *      
     */
    password: string;

    /**
     * @description 主机部分（主机名 + 端口）
     *      
     *      包含主机名和端口号，格式如：'example.com:8080'
     *      
     */
    host: string;

    /**
     * @description 主机名部分
     *      
     *      不包含端口号的主机名，支持 IPv4、IPv6 和域名
     *      
     */
    hostname: string;

    /**
     * @description 端口号
     *      
     *      字符串形式的端口号，空字符串表示使用默认端口
     *      
     */
    port: string;

    /**
     * @description 完整路径（路径 + 查询字符串）
     *      
     *      只读属性，包含 pathname 和 search，格式如：'/path?query=value'
     *      
     */
    readonly path: string;

    /**
     * @description URL 路径部分
     *      
     *      URL 中的路径部分，总是以 '/' 开头
     *      
     */
    pathname: string;

    /**
     * @description 查询字符串（包含问号）
     *      
     *      格式如：'?key1=value1&key2=value2'，空查询时为空字符串
     *      
     */
    search: string;

    /**
     * @description 查询参数值
     *      
     *      可以是字符串或对象，设置对象时会自动序列化为查询字符串
     *      
     */
    query: any;

    /**
     * @description URL 片段标识符（包含井号）
     *      
     *      格式如：'#section'，没有片段时为空字符串
     *      
     */
    hash: string;

    /**
     * @description URL 查询参数对象
     *      
     *      只读属性，返回 URLSearchParams 对象用于操作查询参数
     *      与 URL 对象双向绑定，修改会自动更新 search 和 query 属性
     *      
     */
    readonly searchParams: Class_URLSearchParams;

}

