/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description dns 域名查询模块
 * 
 *  基础模块，提供主机名的地址查询能力：
 * 
 *  - `resolve`：查询主机名的全部地址，返回 ip 字符串数组；
 *  - `lookup`：查询主机名的地址，支持指定地址族与返回全部结果。
 * 
 *  引用方式：
 *  ```JavaScript
 *  var dns = require('dns');
 *  ```
 *  
 */
declare module 'dns' {
    /**
     * @description 查询给定的主机名的地址
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串数组
     *      
     */
    function resolve(name: string): any[];

    function resolve(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 查询给定的主机名的地址
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串数组
     *      
     */
    function resolveSync(name: string): any[];

    /**
     * @description 查询给定的主机名的地址
     *      @param name 指定主机名
     *      @return 返回查询的 ip 字符串数组
     *      
     */
    function resolveAsync(name: string): Promise<any[]>;

    /**
     * @description 查询给定的主机名的地址
     * 
     *      options 支持的选项如下：
     *      ```JavaScript
     *      {
     *          "family": 0, // 指定地址族：0 为任意，4 为 IPv4，6 为 IPv6，也可使用 "IPv4"/"IPv6"。默认: 0
     *          "all": false // 为 true 时返回全部地址的对象数组，否则返回第一个地址的字符串。默认: false
     *      }
     *      ```
     * 
     *      all 为 true 时返回的数组元素包含 `address`（ip 字符串）与 `family`（地址族编号）字段。
     *      @param name 指定主机名
     *      @param options 查询选项
     *      @return 返回查询的 ip 字符串
     *      
     */
    function lookup(name: string, options?: FIBJS.GeneralObject): any;

    function lookup(name: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 查询给定的主机名的地址
     * 
     *      options 支持的选项如下：
     *      ```JavaScript
     *      {
     *          "family": 0, // 指定地址族：0 为任意，4 为 IPv4，6 为 IPv6，也可使用 "IPv4"/"IPv6"。默认: 0
     *          "all": false // 为 true 时返回全部地址的对象数组，否则返回第一个地址的字符串。默认: false
     *      }
     *      ```
     * 
     *      all 为 true 时返回的数组元素包含 `address`（ip 字符串）与 `family`（地址族编号）字段。
     *      @param name 指定主机名
     *      @param options 查询选项
     *      @return 返回查询的 ip 字符串
     *      
     */
    function lookupSync(name: string, options?: FIBJS.GeneralObject): any;

    /**
     * @description 查询给定的主机名的地址
     * 
     *      options 支持的选项如下：
     *      ```JavaScript
     *      {
     *          "family": 0, // 指定地址族：0 为任意，4 为 IPv4，6 为 IPv6，也可使用 "IPv4"/"IPv6"。默认: 0
     *          "all": false // 为 true 时返回全部地址的对象数组，否则返回第一个地址的字符串。默认: false
     *      }
     *      ```
     * 
     *      all 为 true 时返回的数组元素包含 `address`（ip 字符串）与 `family`（地址族编号）字段。
     *      @param name 指定主机名
     *      @param options 查询选项
     *      @return 返回查询的 ip 字符串
     *      
     */
    function lookupAsync(name: string, options?: FIBJS.GeneralObject): Promise<any>;

}

