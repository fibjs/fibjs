/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description test_suite 模块是一个测试套件模块，可作为函数调用 
 */
declare module 'test_suite' {
    /**
     * @description 定义一个测试套件，可嵌套定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function Function(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 暂停测试套件的项目定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function skip(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 独立测试套件的项目定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function only(name: string, block: (...args: any[])=>any): void;

}

