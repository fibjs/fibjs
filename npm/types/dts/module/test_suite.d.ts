/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The test_suite module is a test suite module that can be called as a function
 */
declare module 'test_suite' {
    /**
     * @description Defines a test suite, can be nested
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function Function(name: string, block: (...args: any[])=>any): void;

    /**
     * @description Defines a test suite (with options), can be nested
     *      @param name defines the item name
     *      @param options the test options, supporting: { skip, todo, only }
     *      @param block the test content
     *
     */
    function Function(name: string, options: FIBJS.GeneralObject, block: (...args: any[])=>any): void;

    /**
     * @description Paused test suite item definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function skip(name: string, block: (...args: any[])=>any): void;

    /**
     * @description Independent test suite item definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function only(name: string, block: (...args: any[])=>any): void;

    /**
     * @description Planned test suite definition
     *      @param name defines the item name
     *      @param block the test content
     *
     */
    function todo(name: string, block: (...args: any[])=>any): void;

    /**
     * @description Planned test suite definition (with options)
     *      @param name defines the item name
     *      @param options the test options, supporting: { skip, todo, only }
     *      @param block the test content
     *
     */
    function todo(name: string, options: FIBJS.GeneralObject, block: (...args: any[])=>any): void;

}

