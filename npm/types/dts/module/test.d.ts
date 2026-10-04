/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/test_suite.d.ts" />
/// <reference path="../module/assert.d.ts" />
/**
 * @description The test module is a testing framework; together with the assertion module `assert` it makes it easy to write various test cases; it can be called as a function
 *
 * Before writing test cases, it is usually necessary to first define a test module to describe the test content.
 *
 * - describe
 *
 * describe is the container of all test groups, similar to the concept of a test suite; it is used to attach `it` tests under a specific category, and describe can contain multiple it cases or other sub-categories represented by nested describe.
 *
 * ```
 * describe(String name, Function block)
 * ```
 *
 * Call parameters:
 * name: String, defines the module name
 * block: Function, the module initialization code
 *
 * - it
 *
 * Represents a single test case; each description should test only a single situation to ensure the reliability of the test result.
 *
 * ```
 * it(String name, Function block)
 * ```
 *
 * Call parameters:
 * name: String, defines the item name
 * block: Function, the test content
 *
 * - xit & it.skip
 *
 * Represents a skipped test case.
 *
 * ```
 * xit(String name, Function block)
 * ```
 *
 * Call parameters:
 * name: String, defines the item name
 * block: Function, the test content
 *
 * - oit & it.only
 *
 * Represents running only the current test case and ignoring other test cases, so as to debug the current case separately; very practical.
 *
 * ```
 * oit(String name, Function block)
 * it.only(String name, Function block)
 * ```
 *
 * Call parameters:
 * name: String, defines the item name
 * block: Function, the test content
 *
 * - todo
 *
 * Represents the plan of test cases that need further improvement.
 *
 * ```
 * todo(String name, Function block)
 * ```
 *
 * Call parameters:
 * name: String, defines the item name
 * block: Function, the test content
 *
 * When writing test cases, the assert assertion module is usually used to check the return of functions. The usage is as follows:
 *
 * ```
 * assert(condition, String message);
 * ```
 *
 * The first parameter is the condition to assert, and the second parameter is the error message.
 *
 */
declare module 'test' {
    /**
     * @description Defines a test item
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function Function(name: string, block: ()=>void): void;

    /**
     * @description Defines a test item (with options)
     *      @param name defines the item name
     *      @param options the test options, supporting: { skip, todo, only }
     *      @param block the test content
     *
     */
    function Function(name: string, options: FIBJS.GeneralObject, block: ()=>void): void;

    /**
     * @description Test framework module; points to this module and can be called as a function
     */
    const test: typeof import ('test');

    /**
     * @description Test framework module; points to this module and can be called as a function
     */
    const it: typeof import ('test');

    /**
     * @description Test suite module; can be called as a function, see test_suite
     */
    const suite: typeof import ('test_suite');

    /**
     * @description Test suite module; can be called as a function, see test_suite
     */
    const describe: typeof import ('test_suite');

    /**
     * @description Assertion test module; can be called as a function; if the tested value is false, an error is reported, and the error behavior can be configured to continue running or to throw the error
     */
    const assert: typeof import ('assert');

    /**
     * @description Paused test suite definition
     *      @param name defines the module name
     *      @param block the module initialization code
     *
     */
    function xdescribe(name: string, block: ()=>void): void;

    /**
     * @description Independent test suite definition
     *      @param name defines the module name
     *      @param block the module initialization code
     *
     */
    function odescribe(name: string, block: ()=>void): void;

    /**
     * @description Paused test item definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function xit(name: string, block: ()=>void): void;

    /**
     * @description Paused test item definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function skip(name: string, block: ()=>void): void;

    /**
     * @description Independent test item definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function oit(name: string, block: ()=>void): void;

    /**
     * @description Independent test item definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function only(name: string, block: ()=>void): void;

    /**
     * @description Planned test item definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function todo(name: string, block: ()=>void): void;

    /**
     * @description Planned test item definition (with options)
     *      @param name defines the item name
     *      @param options the test options, supporting: { skip, todo, only }
     *      @param block the test content
     *
     */
    function todo(name: string, options: FIBJS.GeneralObject, block: ()=>void): void;

    /**
     * @description Planned test item definition
     *      @param name defines the item name
     *
     */
    function todo(name: string): void;

    /**
     * @description Defines the enter event of the current test module
     *      @param func the event function
     *
     */
    function before(func: ()=>void): void;

    /**
     * @description Defines the exit event of the current test module
     *      @param func the event function
     *
     */
    function after(func: ()=>void): void;

    /**
     * @description Defines the test item enter event of the current test module
     *      @param func the event function
     *
     */
    function beforeEach(func: ()=>void): void;

    /**
     * @description Defines the test item exit event of the current test module
     *      @param func the event function
     *
     */
    function afterEach(func: ()=>void): void;

    /**
     * @description Tests that a function must be called a specified number of times
     *      @param func the function to test
     *      @return returns the wrapped function
     *
     */
    function mustCall(func: (...args: any[])=>any): (...args: any[])=>any;

    /**
     * @description Tests that a function must not be called
     *      @param func the function to test
     *      @return returns the wrapped function
     *
     */
    function mustNotCall(func: (...args: any[])=>any): (...args: any[])=>any;

    /**
     * @description Tests that a function must not be called
     *      @return returns the wrapped function
     *
     */
    function mustNotCall(): (...args: any[])=>any;

    /**
     * @description Sets and queries the slow test warning threshold, in ms, default 75
     *
     */
    var slow: number;

}

