/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Script 脚本编译和运行对象
 * 
 *  创建方法
 *  ```JavaScript
 *  var Script = new vm.Script('console.log(100)');
 *  ```
 *  
 */
declare class Class_Script extends Class_object {
    /**
     * @description Script 对象构造函数
     *      @param code 指定要编译和运行的脚本代码
     *      @param opts 指定编译和运行选项
     *     
     */
    constructor(code: string, opts?: FIBJS.GeneralObject);

    /**
     * @description 在给定 contextifiedObject 内运行 vm.Script 对象包含的已编译代码并返回结果
     *      @param contextifiedObject 指定运行时的上下文对象
     *      @param opts 指定运行选项
     *      @return 返回运行结果
     *     
     */
    runInContext(contextifiedObject: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): any;

    /**
     * @description 使用给定的 contextObject 在创建的上下文中, 在其中运行 vm.Script 对象包含的已编译代码并返回结果
     *      @param contextObject 指定将被上下文化的对象
     *      @param opts 指定运行选项
     *      @return 返回运行结果
     *     
     */
    runInNewContext(contextObject?: FIBJS.GeneralObject, opts?: FIBJS.GeneralObject): any;

    /**
     * @description 在当前上下文内内运行 vm.Script 对象包含的已编译代码并返回结果
     *      @param opts 指定运行选项
     *      @return 返回运行结果
     *     
     */
    runInThisContext(opts?: FIBJS.GeneralObject): any;

    /**
     * @description 根据当前 Script 对象创建代码缓存
     *      @return 返回代码缓存数据
     *     
     */
    createCachedData(): Class_Buffer;

}

