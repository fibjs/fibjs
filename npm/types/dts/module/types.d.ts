/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description types 模块提供了对数据类型判断的工具函数
 * 
 * 下面是具体的介绍和示例：
 * 
 * ```JavaScript
 * var util = require('util');
 * console.log(util.types.isDate(new Date()));
 * console.log(util.types.isRegExp(/some regexp/));
 * ```
 * 
 */
declare module 'types' {
    /**
     * @description 检测给定的变量是否不包含任何值(没有可枚举的属性)
     *      @param v 给定需要检测的变量
     *      @return 如果为空则返回 True
     *      
     */
    function isEmpty(v: any): boolean;

    /**
     * @description 检测给定的变量是否是数组
     *      @param v 给定需要检测的变量
     *      @return 如果是数组则返回 True
     *      
     */
    function isArray(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Boolean
     *      @param v 给定需要检测的变量
     *      @return 如果是 Boolean 则返回 True
     *      
     */
    function isBoolean(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Null
     *      @param v 给定需要检测的变量
     *      @return 如果是 Null 则返回 True
     *      
     */
    function isNull(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Null 或者 Undefined
     *      @param v 给定需要检测的变量
     *      @return 如果是 Null 或者 Undefined 则返回 True
     *      
     */
    function isNullOrUndefined(v: any): boolean;

    /**
     * @description 检测给定的变量是否是数字
     *      @param v 给定需要检测的变量
     *      @return 如果是数字则返回 True
     *      
     */
    function isNumber(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 BigInt
     *      @param v 给定需要检测的变量
     *      @return 如果是数字则返回 True
     *      
     */
    function isBigInt(v: any): boolean;

    /**
     * @description 检测给定的变量是否是字符串
     *      @param v 给定需要检测的变量
     *      @return 如果是字符串则返回 True
     *      
     */
    function isString(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Undefined
     *      @param v 给定需要检测的变量
     *      @return 如果是 Undefined 则返回 True
     *      
     */
    function isUndefined(v: any): boolean;

    /**
     * @description 检测给定的变量是否是正则对象
     *      @param v 给定需要检测的变量
     *      @return 如果是正则对象则返回 True
     *      
     */
    function isRegExp(v: any): boolean;

    /**
     * @description 检测给定的变量是否是对象
     *      @param v 给定需要检测的变量
     *      @return 如果是对象则返回 True
     *      
     */
    function isObject(v: any): boolean;

    /**
     * @description 检测给定的变量是否是日期对象
     *      @param v 给定需要检测的变量
     *      @return 如果是日期对象则返回 True
     *      
     */
    function isDate(v: any): boolean;

    /**
     * @description 检测给定的变量是否是错误对象
     *      @param v 给定需要检测的变量
     *      @return 如果是错误对象则返回 True
     *      
     */
    function isNativeError(v: any): boolean;

    /**
     * @description 检测给定的变量是否是原始类型
     *      @param v 给定需要检测的变量
     *      @return 如果是原始类型则返回 True
     *      
     */
    function isPrimitive(v: any): boolean;

    /**
     * @description 检测给定的变量是否是Symbol类型
     *      @param v 给定需要检测的变量
     *      @return 如果是Symbol类型则返回 True
     *      
     */
    function isSymbol(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 DataView 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 DataView 类型则返回 True
     *      
     */
    function isDataView(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 External 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 External 类型则返回 True
     *      
     */
    function isExternal(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Map 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Map 类型则返回 True
     *      
     */
    function isMap(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 MapIterator 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 MapIterator 类型则返回 True
     *      
     */
    function isMapIterator(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Promise 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Promise 类型则返回 True
     *      
     */
    function isPromise(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 AsyncFunction 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 AsyncFunction 类型则返回 True
     *      
     */
    function isAsyncFunction(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Set 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Set 类型则返回 True
     *      
     */
    function isSet(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 SetIterator 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 SetIterator 类型则返回 True
     *      
     */
    function isSetIterator(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 TypedArray 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 TypedArray 类型则返回 True
     *      
     */
    function isTypedArray(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Float32Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Float32Array 类型则返回 True
     *      
     */
    function isFloat32Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Float64Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Float64Array 类型则返回 True
     *      
     */
    function isFloat64Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Int8Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Int8Array 类型则返回 True
     *      
     */
    function isInt8Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Int16Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Int16Array 类型则返回 True
     *      
     */
    function isInt16Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Int32Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Int32Array 类型则返回 True
     *      
     */
    function isInt32Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Uint8Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Uint8Array 类型则返回 True
     *      
     */
    function isUint8Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Uint8ClampedArray 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Uint8ClampedArray 类型则返回 True
     *      
     */
    function isUint8ClampedArray(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Uint16Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Uint16Array 类型则返回 True
     *      
     */
    function isUint16Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是 Uint32Array 类型
     *      @param v 给定需要检测的变量
     *      @return 如果是 Uint32Array 类型则返回 True
     *      
     */
    function isUint32Array(v: any): boolean;

    /**
     * @description 检测给定的变量是否是函数对象
     *      @param v 给定需要检测的变量
     *      @return 如果是函数对象则返回 True
     *      
     */
    function isFunction(v: any): boolean;

    /**
     * @description 检测给定的变量是否是函数 Buffer 对象
     *      @param v 给定需要检测的变量
     *      @return 如果是函数 Buffer 对象则返回 True
     *      
     */
    function isBuffer(v: any): boolean;

}

