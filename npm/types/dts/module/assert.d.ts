/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description 断言测试模块，如果测试值为假，则报错，报错行为可设定继续运行或者错误抛出
 * 
 *  引用方法：
 *  ```JavaScript
 *  var assert = require('assert');
 *  ```
 *  或者通过 test 模块引用：
 *  ```JavaScript
 *  var test = require('test');
 *  var assert = test.assert;
 *  ```
 *  或者通过 test.setup 配置：
 *  ```JavaScript
 *  require("test").setup();
 *  ```
 *  
 */
declare module 'assert' {
    /**
     * @description 测试数值为真，为假则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function Function(actual?: any, msg?: string): void;

    /**
     * @description 测试数值为真，为假则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function ok(actual?: any, msg?: string): void;

    /**
     * @description 测试数值为假，为真则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notOk(actual?: any, msg?: string): void;

    /**
     * @description 测试数值等于预期值，不相等则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function equal(actual?: any, expected?: any, msg?: string): void;

    /**
     * @description 测试数值不等于预期值，相等则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notEqual(actual?: any, expected?: any, msg?: string): void;

    /**
     * @description 测试数值严格等于预期值，不相等则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function strictEqual(actual?: any, expected?: any, msg?: string): void;

    /**
     * @description 测试数值不严格等于预期值，相等则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notStrictEqual(actual?: any, expected?: any, msg?: string): void;

    /**
     * @description 测试数值深度等于预期值，不相等则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function deepEqual(actual?: any, expected?: any, msg?: string): void;

    /**
     * @description 测试数值不深度等于预期值，相等则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notDeepEqual(actual?: any, expected?: any, msg?: string): void;

    /**
     * @description 测试数值近似等于预期值，否则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param delta 近似的小数精度
     *      @param msg 断言失败时的提示信息
     *      
     */
    function closeTo(actual: any, expected: any, delta: any, msg?: string): void;

    /**
     * @description 测试数值不近似等于预期值，否则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param delta 近似的小数精度
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notCloseTo(actual: any, expected: any, delta: any, msg?: string): void;

    /**
     * @description 测试数值小于预期值，大于或等于则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function lessThan(actual: any, expected: any, msg?: string): void;

    /**
     * @description 测试数值不小于预期值，小于则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notLessThan(actual: any, expected: any, msg?: string): void;

    /**
     * @description 测试数值大于预期值，小于或等于则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function greaterThan(actual: any, expected: any, msg?: string): void;

    /**
     * @description 测试数值不大于预期值，大于则断言失败
     *      @param actual 要测试的数值
     *      @param expected 预期的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notGreaterThan(actual: any, expected: any, msg?: string): void;

    /**
     * @description 测试变量存在，为假则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function exist(actual: any, msg?: string): void;

    /**
     * @description 测试变量不存在，为真则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notExist(actual: any, msg?: string): void;

    /**
     * @description 测试数值为布尔值真，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isTrue(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为布尔值真，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotTrue(actual: any, msg?: string): void;

    /**
     * @description 测试数值为布尔值假，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isFalse(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为布尔值假，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotFalse(actual: any, msg?: string): void;

    /**
     * @description 测试数值为 Null，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNull(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为 Null，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotNull(actual: any, msg?: string): void;

    /**
     * @description 测试数值为 undefined，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isUndefined(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为 undefined，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isDefined(actual: any, msg?: string): void;

    /**
     * @description 测试数值为函数，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isFunction(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为函数，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotFunction(actual: any, msg?: string): void;

    /**
     * @description 测试数值为对象，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isObject(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为对象，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotObject(actual: any, msg?: string): void;

    /**
     * @description 测试数值为数组，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isArray(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为数组，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotArray(actual: any, msg?: string): void;

    /**
     * @description 测试数值为字符串，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isString(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为字符串，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotString(actual: any, msg?: string): void;

    /**
     * @description 测试数值为数字，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNumber(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为数字，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotNumber(actual: any, msg?: string): void;

    /**
     * @description 测试数值为布尔，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isBoolean(actual: any, msg?: string): void;

    /**
     * @description 测试数值不为布尔，否则断言失败
     *      @param actual 要测试的数值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function isNotBoolean(actual: any, msg?: string): void;

    /**
     * @description 测试数值为给定类型，否则断言失败
     *      @param actual 要测试的数值
     *      @param type 指定的类型
     *      @param msg 断言失败时的提示信息
     *      
     */
    function typeOf(actual: any, type: string, msg?: string): void;

    /**
     * @description 测试数值不为给定类型，否则断言失败
     *      @param actual 要测试的数值
     *      @param type 指定的类型
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notTypeOf(actual: any, type: string, msg?: string): void;

    /**
     * @description 测试对象中包含指定属性，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性
     *      @param msg 断言失败时的提示信息
     *      
     */
    function property(object: any, prop: any, msg?: string): void;

    /**
     * @description 测试对象中不包含指定属性，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notProperty(object: any, prop: any, msg?: string): void;

    /**
     * @description 深度测试对象中包含指定属性，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性，以“.”分割
     *      @param msg 断言失败时的提示信息
     *      
     */
    function deepProperty(object: any, prop: any, msg?: string): void;

    /**
     * @description 深度测试对象中不包含指定属性，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性，以“.”分割
     *      @param msg 断言失败时的提示信息
     *      
     */
    function notDeepProperty(object: any, prop: any, msg?: string): void;

    /**
     * @description 测试对象中指定属性的值为给定值，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性
     *      @param value 给定的值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function propertyVal(object: any, prop: any, value: any, msg?: string): void;

    /**
     * @description 测试对象中指定属性的值不为给定值，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性
     *      @param value 给定的值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function propertyNotVal(object: any, prop: any, value: any, msg?: string): void;

    /**
     * @description 深度测试对象中指定属性的值为给定值，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性，以“.”分割
     *      @param value 给定的值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function deepPropertyVal(object: any, prop: any, value: any, msg?: string): void;

    /**
     * @description 深度测试对象中指定属性的值不为给定值，否则断言失败
     *      @param object 要测试的对象
     *      @param prop 要测试的属性，以“.”分割
     *      @param value 给定的值
     *      @param msg 断言失败时的提示信息
     *      
     */
    function deepPropertyNotVal(object: any, prop: any, value: any, msg?: string): void;

    /**
     * @description 测试给定的代码会抛出错误，未抛出则断言失败
     *      @param block 指定测试的代码，以函数形式给出
     *      @param msg 断言失败时的提示信息
     *      
     */
    /* Illegal function name 'throws' can't be used here
    function throws(block: (...args: any[])=>any, msg?: string): void;
    */

    /**
     * @description 测试给定的代码不会抛出错误，抛出则断言失败
     *      @param block 指定测试的代码，以函数形式给出
     *      @param msg 断言失败时的提示信息
     *      
     */
    function doesNotThrow(block: (...args: any[])=>any, msg?: string): void;

    /**
     * @description 如果参数为真，则抛出
     *      @param object 参数
     *      
     */
    function ifError(object?: any): void;

}

