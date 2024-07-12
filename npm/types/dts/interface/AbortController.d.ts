/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/AbortSignal.d.ts" />
/**
 * @description 控制器对象，用于在需要时中止一个或多个 Web 请求
 * 
 * AbortController 对象为全局基础类，在任何时候都可以直接以 new AbortController() 创建：
 *  ```JavaScript
 *  var buf = new AbortController();
 *  ```
 *  
 */
declare class Class_AbortController extends Class_object {
    /**
     * @description 用于中止一个或多个 Web 请求的 AbortSignal 对象 
     */
    constructor();

    /**
     * @description 用于中止一个或多个 Web 请求的 AbortSignal 对象 
     */
    readonly signal: Class_AbortSignal;

    /**
     * @description 用于中止一个或多个 Web 请求
     *      @param reason 一个可选的字符串，用于描述中止请求的原因
     *     
     */
    abort(reason?: string): void;

}

