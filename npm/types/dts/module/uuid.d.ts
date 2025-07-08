/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description uuid 模块提供唯一 id 的创建于操作。它可以用于生成符合各种不同要求的 UUID(Universally Unique Identifier)
 * 
 * `uuid` 模块提供了多个静态函数，可以用于配置和生成不同种类的 UUID。
 * 以下是使用 md5 创建 uuid 的例子:
 * ```JavaScript
 * const uuid = require('uuid');
 * const ns = uuid.DNS;
 * const name = 'example.com';
 * console.log(uuid.md5(ns, name));
 * ```
 * 在以上例子中，首先引入了 uuid 模块，然后指定了名字空间和名称，并通过 md5 算法生成了符合要求的 UUID，并输出到控制台。
 * 同样，我们还可以使用 snowflake 算法生成 uuid，以下是使用 snowflake 算法创建 uuid 的例子：
 * ```JavaScript
 * const uuid = require('uuid');
 * const s = uuid.snowflake();
 * console.log(s);
 * ```
 * 在以上例子中，snowflake() 方法会返回一个 Buffer 对象，可以将其转换为字符串后输出到控制台，以获取生成的 uuid。
 *  
 */
declare module 'uuid' {
    /**
     * @description md5 与 sha1 创建 uuid 时指定 name 命名为域名 
     */
    export const DNS: 0;

    /**
     * @description md5 与 sha1 创建 uuid 时指定 name 命名为 url 地址 
     */
    export const URL: 1;

    /**
     * @description md5 与 sha1 创建 uuid 时指定 name 命名为 ISO OID 
     */
    export const OID: 2;

    /**
     * @description md5 与 sha1 创建 uuid 时指定 name 命名为 X.500 DN 
     */
    export const X509: 3;

    /**
     * @description 返回一个空的 uuid 
     */
    const NIL: string;

    /**
     * @description 返回一个最大 UUID 字符串 
     */
    const MAX: string;

    /**
     * @description 解析 uuid 字符串
     *      @param uuid 要解析的 uuid 字符串
     *      @return 返回解析后的二进制 id
     *      
     */
    function parse(uuid: string): Class_Buffer;

    /**
     * @description 将二进制 id 转换为 uuid 字符串
     *      @param id 要转换的二进制 id
     *      @return 返回转换后的 uuid 字符串
     *      
     */
    function stringify(arr: any[], offset?: number): string;

    /**
     * @description 使用时间戳创建 uuid
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v1(): string;

    /**
     * @description 使用随机数创建 uuid
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v4(): string;

    /**
     * @description 验证 uuid 字符串是否符合规范
     *      @param uuid 要验证的 uuid 字符串
     *      @return 返回 true 表示符合规范，false 表示不符合规范
     *      
     */
    function validate(uuid: string): boolean;

    /**
     * @description 使用时间和主机名创建 uuid
     *      @return 返回一个生成的二进制 id
     *      
     */
    function node(): Class_Buffer;

    /**
     * @description 使用特定命名的 md5 创建 uuid
     *      @param ns 指定命名空间，可以为 uuid.DNS, uuid.URL, uuid.OID, uuid.X509
     *      @param name 指定名称
     *      @return 返回一个生成的二进制 id
     *      
     */
    function md5(ns: number, name: string): Class_Buffer;

    /**
     * @description 使用随机数创建 uuid
     *      @return 返回一个生成的二进制 id
     *      
     */
    function random(): Class_Buffer;

    /**
     * @description 使用特定命名的 sha1 创建 uuid
     *      @param ns 指定命名空间，可以为 uuid.DNS, uuid.URL, uuid.OID, uuid.X509
     *      @param name 指定名称
     *      @return 返回一个生成的二进制 id
     *      
     */
    function sha1(ns: number, name: string): Class_Buffer;

    /**
     * @description 使用 Snowflake 算法创建 uuid
     *      @return 返回一个生成的二进制 id
     *      
     */
    function snowflake(): Class_Buffer;

    /**
     * @description 查询和修改 Snowflake 算法的主机 id 
     */
    var hostID: number;

}

