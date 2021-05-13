/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Windows 注册表访问模块
 *  引用方式：
 *  ```JavaScript
 *  var registry = require('registry');
 *  var value = registry.get(registry.CLASSES_ROOT, "\node1\node2\value");
 *  ```
 *  
 */
declare module 'registry' {
    /**
     * @description 注册表根，存储Windows可识别的文件类型的详细列表，以及相关联的程序 
     */
    export const CLASSES_ROOT: 0;

    /**
     * @description 注册表根，存储当前用户设置的信息 
     */
    export const CURRENT_USER: 1;

    /**
     * @description 注册表根，包括安装在计算机上的硬件和软件的信息 
     */
    export const LOCAL_MACHINE: 2;

    /**
     * @description 注册表根，包含使用计算机的用户的信息 
     */
    export const USERS: 3;

    /**
     * @description 注册表根，这个分支包含计算机当前的硬件配置信息 
     */
    export const CURRENT_CONFIG: 5;

    /**
     * @description 注册表数据类型，字符串 
     */
    export const SZ: 1;

    /**
     * @description 注册表数据类型，扩展字符串 
     */
    export const EXPAND_SZ: 2;

    /**
     * @description 注册表数据类型，32 位数值 
     */
    export const DWORD: 4;

    /**
     * @description 注册表数据类型，64 位数值 
     */
    export const QWORD: 11;

    /**
     * @description 返回指定键值下的所有子健
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @return 返回该键值下所有子健
     *      
     */
    function listSubKey(root: number, key: string): any[];

    /**
     * @description 返回指定键值下的所有数据的健
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @return 返回该键值下所有数据的健
     *      
     */
    function listValue(root: number, key: string): any[];

    /**
     * @description 查询指定键值的数值
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @return 返回指定键值的数值
     *      
     */
    function get(root: number, key: string): any;

    /**
     * @description 设置指定键值为数字
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param value 指定数字
     *      @param type 指定类型，允许的类型为 DWORD 和 QWORD，缺省为 DWORD
     *      
     */
    function set(root: number, key: string, value: number, type: number): void;

    /**
     * @description 设置指定键值为字符串
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param value 指定字符串
     *      @param type 指定类型，允许的类型为 SZ 和 EXPAND_SZ，缺省为 SZ
     *      
     */
    function set(root: number, key: string, value: string, type: number): void;

    /**
     * @description 设置指定键值为多字符串
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param value 指定多字符串数组
     *      
     */
    function set(root: number, key: string, value: any[]): void;

    /**
     * @description 设置指定键值为二进制
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param value 指定二进制数据
     *      
     */
    function set(root: number, key: string, value: Class_Buffer): void;

    /**
     * @description 删除指定键值的数值
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      
     */
    function del(root: number, key: string): void;

}

