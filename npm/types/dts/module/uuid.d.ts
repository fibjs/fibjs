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
     * @description v3 和 v5 的 DNS 命名空间 UUID 
     */
    const DNS_NAMESPACE: string;

    /**
     * @description v3 和 v5 的 URL 命名空间 UUID 
     */
    const URL_NAMESPACE: string;

    /**
     * @description 解析 uuid 字符串
     *      @param uuid 要解析的 uuid 字符串
     *      @return 返回解析后的二进制 id
     *      
     */
    function parse(uuid: string): Class_Buffer;

    /**
     * @description 将二进制数组转换为 uuid 字符串
     *      @param arr 包含 uuid 二进制数据的数组或 Buffer，长度需不少于 16 字节
     *      @param offset 可选，指定 uuid 数据在数组中的起始偏移，默认为 0
     *      @return 返回转换后的 uuid 字符串
     *      
     */
    function stringify(arr: Class_Buffer, offset?: number): string;

    /**
     * @description 使用时间戳创建 uuid
     *      @param options 可选参数对象，支持以下属性：node（Buffer，节点 ID）、clockseq（Integer，时钟序列）、msecs（Integer，毫秒时间戳）、nsecs（Integer，纳秒时间戳）
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v1(options?: FIBJS.GeneralObject): string;

    /**
     * @description 使用 MD5 命名空间创建 uuid（字符串格式）
     *      @param name 指定名称
     *      @param ns 命名空间 UUID 字符串，或使用预定义命名空间
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v3(name: string, ns: string): string;

    /**
     * @description 使用随机数创建 uuid
     *      @param options 可选参数对象，支持以下属性：random（Buffer，随机数）、rng（Function，随机数生成器）
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v4(options?: FIBJS.GeneralObject): string;

    /**
     * @description 使用 SHA1 命名空间创建 uuid（字符串格式）
     *      @param name 指定名称
     *      @param ns 命名空间 UUID 字符串，或使用预定义命名空间
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v5(name: string, ns: string): string;

    /**
     * @description 获取 uuid 的版本号
     *      @param uuid 要检查的 uuid 字符串
     *      @return 返回 uuid 的版本号（0-7），如果格式无效则返回 undefined
     *      
     */
    function version(uuid: string): number;

    /**
     * @description 使用重排序时间戳创建 uuid v6
     *      @param options 可选参数对象，支持以下属性：node（Buffer，节点 ID）、clockseq（Integer，时钟序列）、msecs（Integer，毫秒时间戳）、nsecs（Integer，纳秒时间戳）
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v6(options?: FIBJS.GeneralObject): string;

    /**
     * @description 使用 Unix Epoch 时间戳创建 uuid v7
     *      @param options 可选参数对象，支持以下属性：msecs（Integer，毫秒时间戳）
     *      @return 返回一个生成的 uuid 字符串
     *      
     */
    function v7(options?: FIBJS.GeneralObject): string;

    /**
     * @description 将 uuid v1 转换为 v6
     *      @param uuid v1 格式的 uuid 字符串
     *      @return 返回转换后的 v6 uuid 字符串
     *      
     */
    function v1ToV6(uuid: string): string;

    /**
     * @description 将 uuid v6 转换为 v1
     *      @param uuid v6 格式的 uuid 字符串
     *      @return 返回转换后的 v1 uuid 字符串
     *      
     */
    function v6ToV1(uuid: string): string;

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

