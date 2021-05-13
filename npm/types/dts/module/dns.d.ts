/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description dns 域名查询模块
 * 
 *  基础模块，引用方式：
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
     *      @return 返回查询的 ip 字符串
     *      
     */
    function lookup(name: string): string;

    function lookup(name: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

}

