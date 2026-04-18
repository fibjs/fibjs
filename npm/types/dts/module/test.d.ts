/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/test_suite.d.ts" />
/// <reference path="../module/assert.d.ts" />
/**
 * @description test 模块是一个测试框架，结合断言模块 `assert` 可以方便地编写各种测试用例，可作为函数调用
 * 
 * 在编写测试用例前通常需要先定义一个测试模块用来描述测试内容。
 * 
 * - describe
 * 
 * describe 所有的测试分组的容器，类似于测试套件的概念，是用于将 `it` 测试挂靠在特定的分类下，describe 可以包含多个 it 用例或者其它嵌套的 describe 表示的子分类。
 * 
 * ```
 * describe(String name, Function block)
 * ```
 * 
 * 调用参数:
 * name: String, 定义模块名称
 * block: Function, 模块初始化代码
 * 
 * - it
 * 
 * 表示单个测试用例，每个描述都应该只测单一的一种情况，以保证测试结果的可靠性。
 * 
 * ```
 * it(String name, Function block)
 * ```
 * 
 * 调用参数:
 * name: String, 定义项目名称
 * block: Function, 测试内容
 * 
 * - xit & it.skip
 * 
 * 表示被跳过的测试用例。
 * 
 * ```
 * xit(String name, Function block)
 * ```
 * 
 * 调用参数:
 * name: String, 定义项目名称
 * block: Function, 测试内容
 * 
 * - oit & it.only
 * 
 * 表示仅运行当前测试用例，忽略其他测试用例，以便单独调试当前用例，非常实用。
 * 
 * ```
 * oit(String name, Function block)
 * it.only(String name, Function block)
 * ```
 * 
 * 调用参数:
 * name: String, 定义项目名称
 * block: Function, 测试内容
 * 
 * - todo
 * 
 * 表示需进一步完善测试用例的计划。
 * 
 * ```
 * todo(String name, Function block)
 * ```
 * 
 * 调用参数:
 * name: String, 定义项目名称
 * block: Function, 测试内容
 * 
 * 在编写测试用例时，通常使用 assert 断言模块，对函数的返回进行检查。使用方法如下：
 * 
 * ```
 * assert(condition, String message);
 * ```
 * 
 * 其中，第一个参数是需要断言的条件，第二个参数是错误消息。
 *  
 */
declare module 'test' {
    /**
     * @description 定义一个测试项目
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function Function(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 测试框架模块，指向本模块，可作为函数调用 
     */
    const test: typeof import ('test');

    /**
     * @description 测试框架模块，指向本模块，可作为函数调用 
     */
    const it: typeof import ('test');

    /**
     * @description 测试套件模块，可作为函数调用，参见 test_suite 
     */
    const suite: typeof import ('test_suite');

    /**
     * @description 测试套件模块，可作为函数调用，参见 test_suite 
     */
    const describe: typeof import ('test_suite');

    /**
     * @description 断言测试模块，可作为函数调用，如果测试值为假，则报错，报错行为可设定继续运行或者错误抛出 
     */
    const assert: typeof import ('assert');

    /**
     * @description 暂停测试套件定义
     *      @param name 定义模块名称
     *      @param block 模块初始化代码
     *      
     */
    function xdescribe(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 独立测试套件定义
     *      @param name 定义模块名称
     *      @param block 模块初始化代码
     *      
     */
    function odescribe(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 暂停测试的项目定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function xit(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 暂停测试的项目定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function skip(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 独立测试的项目定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function oit(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 独立测试的项目定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function only(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 计划项目定义
     *      @param name 定义项目名称
     *      @param block 测试内容
     *      
     */
    function todo(name: string, block: (...args: any[])=>any): void;

    /**
     * @description 计划项目定义
     *      @param name 定义项目名称
     *      
     */
    function todo(name: string): void;

    /**
     * @description 定义当前测试模块进入事件
     *      @param func 事件函数
     *      
     */
    function before(func: (...args: any[])=>any): void;

    /**
     * @description 定义当前测试模块退出事件
     *      @param func 事件函数
     *      
     */
    function after(func: (...args: any[])=>any): void;

    /**
     * @description 定义当前测试模块测试项目进入事件
     *      @param func 事件函数
     *      
     */
    function beforeEach(func: (...args: any[])=>any): void;

    /**
     * @description 定义当前测试模块测试项目退出事件
     *      @param func 事件函数
     *      
     */
    function afterEach(func: (...args: any[])=>any): void;

    /**
     * @description 测试一个函数必须被调用指定次数
     *      @param func 被测试的函数
     *      @return 返回被包裹的函数
     *      
     */
    function mustCall(func: (...args: any[])=>any): (...args: any[])=>any;

    /**
     * @description 测试一个函数必须不被调用
     *      @param func 被测试的函数
     *      @return 返回被包裹的函数
     *      
     */
    function mustNotCall(func: (...args: any[])=>any): (...args: any[])=>any;

    /**
     * @description 测试一个函数必须不被调用
     *      @return 返回被包裹的函数
     *     
     */
    function mustNotCall(): (...args: any[])=>any;

    /**
     * @description 设置和查询慢速测试警告阀值，以 ms 为单位，缺省为 75
     *      
     */
    var slow: number;

}

