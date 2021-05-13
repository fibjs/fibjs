/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description uuid 唯一 id 模块
 * 
 *  基础模块。提供唯一 id 的创建于操作
 *  ```JavaScript
 *  var uuid = require('uuid');
 *  ```
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

