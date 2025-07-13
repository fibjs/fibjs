/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description registry 模块是一个操作 Windows 注册表（Registry）的模块。它提供了访问注册表的方法和常量，可以读取、修改、删除、添加等操作。registry 模块提供的操作方式和 Windows 应用程序使用的方式类似，但却是在 FibJS 中提供了能力。常量有常见的 Root、 数据类型等常量，还有一些用于不同操作的返回值的常量
 * 
 * `registry` 模块是一个用于操作 Windows 注册表（Registry）的模块。注册表是一个分层数据库，用于存储系统和应用程序的配置信息。Windows 操作系统和许多应用程序都依赖注册表来存储和检索配置信息。
 * 
 * Windows 注册表包含多个根键（Root Key），每个根键下包含多个子键（Sub Key）和键值（Value）。常见的根键包括：
 * 
 * - `HKEY_CLASSES_ROOT`：存储文件类型和关联的应用程序信息。
 * - `HKEY_CURRENT_USER`：存储当前用户的配置信息。
 * - `HKEY_LOCAL_MACHINE`：存储计算机上所有用户的配置信息。
 * - `HKEY_USERS`：存储所有用户的配置信息。
 * - `HKEY_CURRENT_CONFIG`：存储当前硬件配置的信息。
 * 
 * 注册表中的数据类型包括字符串（SZ）、扩展字符串（EXPAND_SZ）、32 位数值（DWORD）、64 位数值（QWORD）等。
 * 
 * `registry` 模块提供了一系列函数，用于读取、修改、删除和添加注册表项。常用的函数包括：
 * 
 * - `get(root, key[, flags])`：获取指定注册表项的值。
 * - `set(root, key, value[, type])`：设置指定注册表项的值。
 * - `del(root, key)`：删除指定注册表项。
 * 
 * 以下是一个使用 `registry` 模块的示例代码，展示了如何验证某个注册表项是否存在，如果不存在则写入该项，并读取其值：
 * 
 * ```JavaScript
 * var registry = require('registry');
 * 
 * // 指定键名
 * var key = "Software\\Fibjs\\Test\\KeyName";
 * 
 * // 检查注册表项是否存在
 * if (!registry.get(registry.CLASSES_ROOT, key)) {
 *     // 如果不存在，则写入注册表
 *     registry.set(registry.CLASSES_ROOT, key, "test_value");
 * }
 * 
 * // 读取注册表项的值
 * var value = registry.get(registry.CLASSES_ROOT, key);
 * console.log(value);
 * ```
 * 
 * 该程序首先检查注册表项 `Software\Fibjs\Test\KeyName` 是否存在，如果不存在，则将其值设置为 `test_value`。最后，读取该注册表项的值并输出到控制台。
 * 
 * `registry` 模块提供了一个方便的接口，用于在 FibJS 中操作 Windows 注册表。通过该模块，可以轻松地读取、修改、添加和删除注册表中的信息，从而实现对系统和应用程序配置的管理。
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
     * @description 查询指定键值的数值
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param name 指定数值名称
     *      @return 返回指定键值的数值
     *      
     */
    function get(root: number, key: string, name: string): any;

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
     *      @param values 指定多字符串数组
     *      
     */
    function set(root: number, key: string, values: string[]): void;

    /**
     * @description 设置指定键值为二进制
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param value 指定二进制数据
     *      
     */
    function set(root: number, key: string, value: Class_Buffer): void;

    /**
     * @description 设置指定键值为数字
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param name 指定数值名称
     *      @param value 指定数字
     *      @param type 指定类型，允许的类型为 DWORD 和 QWORD，缺省为 DWORD
     *      
     */
    function set(root: number, key: string, name: string, value: number, type: number): void;

    /**
     * @description 设置指定键值为字符串
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param name 指定数值名称
     *      @param value 指定字符串
     *      @param type 指定类型，允许的类型为 SZ 和 EXPAND_SZ，缺省为 SZ
     *      
     */
    function set(root: number, key: string, name: string, value: string, type: number): void;

    /**
     * @description 设置指定键值为多字符串
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param name 指定数值名称
     *      @param values 指定多字符串数组
     *      
     */
    function set(root: number, key: string, name: string, values: string[]): void;

    /**
     * @description 设置指定键值为二进制
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param name 指定数值名称
     *      @param value 指定二进制数据
     *      
     */
    function set(root: number, key: string, name: string, value: Class_Buffer): void;

    /**
     * @description 检查指定键值是否存在
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @return 返回键值是否存在
     *      
     */
    function has(root: number, key: string): boolean;

    /**
     * @description 检查指定键值是否存在
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param name 指定数值名称
     *      @return 返回键值是否存在
     *      
     */
    function has(root: number, key: string, name: string): boolean;

    /**
     * @description 删除指定键值的数值
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      
     */
    function del(root: number, key: string): void;

    /**
     * @description 删除指定键值的数值
     *      @param root 指定注册表根
     *      @param key 指定键值
     *      @param name 指定数值名称
     *      
     */
    function del(root: number, key: string, name: string): void;

}

