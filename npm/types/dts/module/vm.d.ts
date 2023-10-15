/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SandBox.d.ts" />
/// <reference path="../interface/Script.d.ts" />
/**
 * @description 沙箱模块，用于隔离不同安全等级的运行环境
 * 
 *  通过建立隔离沙箱，可以限制脚本运行时可以接触的资源，隔离不同脚本执行环境，并可以为不同的环境定制基础模块，以保障整体运行环境的安全。
 * 
 *  下面的示例创建一个沙箱，限制只允许访问全局基础模块中的 assert 模块，并添加 a 和 b 两个定制模块：
 *  ```JavaScript
 *  var vm = require('vm');
 *  var sbox = new vm.SandBox({
 *    a: 100,
 *    b: 200,
 *    assert: require('assert')
 *  });
 * 
 *  var mod_in_sbox = sbox.require('./path/to/mod');
 *  ```
 *  
 */
declare module 'vm' {
    /**
     * @description 创建一个 SandBox 对象，参见 SandBox 
     */
    const SandBox: typeof Class_SandBox;

    /**
     * @description 创建一个 Script 对象，参见 Script 
     */
    const Script: typeof Class_Script;

    /**
     * @description 创建一个上下文对象
     *      @param contextObject 指定将被上下文化的对象
     *      @param opts 指定上下文选项
     *      @return 返回上下文对象
     *     
     */
    function createContext(contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description 如果给定的 object 对象已使用 vm.createContext() 进行上下文化，则返回 true
     *      @param contextObject 指定要检查的对象
     *      @return 如果给定的 object 对象已使用 vm.createContext() 进行上下文化，则返回 true
     *     
     */
    function isContext(contextObject: FIBJS.GeneralObject): boolean;

    /**
     * @description 在给定 contextifiedObject 内运行 code 指定的代码并返回结果
     *      @param code 指定要编译和运行的脚本代码
     *      @param contextifiedObject 指定运行时的上下文对象
     *      @param opts 指定运行选项
     *      @return 返回运行结果
     *     
     */
    function runInContext(code: string, contextifiedObject: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): any;

    /**
     * @description 在给定 contextifiedObject 内运行 code 指定的代码并返回结果
     *      @param code 指定要编译和运行的脚本代码
     *      @param contextifiedObject 指定运行时的上下文对象
     *      @param filename 指定脚本文件名
     *      @return 返回运行结果
     *     
     */
    function runInContext(code: string, contextifiedObject: FIBJS.GeneralObject, filename: string): any;

    /**
     * @description 使用给定的 contextObject 在创建的上下文中, 在其中运行 code 指定的代码并返回结果
     *      @param code 指定要编译和运行的脚本代码
     *      @param contextObject 指定将被上下文化的对象
     *      @param opts 指定运行选项
     *      @return 返回运行结果
     *     
     */
    function runInNewContext(code: string, contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): any;

    /**
     * @description 使用给定的 contextObject 在创建的上下文中, 在其中运行 code 指定的代码并返回结果
     *      @param code 指定要编译和运行的脚本代码
     *      @param contextObject 指定将被上下文化的对象
     *      @param filename 指定脚本文件名
     *      @return 返回运行结果
     *     
     */
    function runInNewContext(code: string, contextObject?: FIBJS.GeneralObject, filename?: string): any;

    /**
     * @description 在当前上下文内内运行 code 指定的代码并返回结果
     *      @param code 指定要编译和运行的脚本代码
     *      @param opts 指定运行选项
     *      @return 返回运行结果
     *     
     */
    function runInThisContext(code: string, opts?: FIBJS.GeneralObject): any;

    /**
     * @description 在当前上下文内内运行 code 指定的代码并返回结果
     *      @param code 指定要编译和运行的脚本代码
     *      @param filename 指定脚本文件名
     *      @return 返回运行结果
     *     
     */
    function runInThisContext(code: string, filename: string): any;

}

