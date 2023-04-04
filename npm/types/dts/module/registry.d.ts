/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description registry 模块是一个操作 Windows 注册表（Registry）的模块。它提供了访问注册表的方法和常量，可以读取、修改、删除、添加等操作。registry 模块提供的操作方式和 Windows 应用程序使用的方式类似，但却是在 FibJS 中提供了能力。常量有常见的 Root、 数据类型等常量，还有一些用于不同操作的返回值的常量
 * 
 * 常用的函数有：
 * 1. get(root, key[, flags])：获取指定注册表项的值。
 * 2. set(root, key, value[, type])：设置指定注册表项的值。
 * 3. del(root, key)：删除指定注册表项。
 * 
 * 接下来，我们验证某个注册表项在指定注册表分支上的键值是否存在，然后将该键值读取。如果不存在，就将该键值写入到指定注册表项。代码如下所示：
 * ```JavaScript
 * var registry = require('registry');
 * 
 * // 验证指定键值是否存在，如果不存在，写入一个数据
 * var key = "Software\\Fibjs\\Test\\KeyName";
 * if(!registry.get(registry.CLASSES_ROOT, key)) {
 *     registry.set(registry.CLASSES_ROOT, key, "test_value");
 * }
 * // 读取指定键值数据
 * var value = registry.get(registry.CLASSES_ROOT, key);
 * console.log(value);
 * ```
 * 该程序首先验证注册表项是否存在，如果不存在，则写入注册表，键名为 Software\Fibjs\Test\KeyName，并将值设置为 test_value。最后，读取该注册表的键值并在控制台输出。
 * 上面的代码展示了 registry 模块的基本用法，可以通过这个模块很方便地读取、修改、添加、删除注册表中的信息。
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

