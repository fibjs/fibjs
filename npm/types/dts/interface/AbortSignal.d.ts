/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 *  brief 该对象允许您与异步操作（例如提取请求）进行通信，并在需要时通过 AbortController 对象中止它 
 */
declare class Class_AbortSignal extends Class_EventEmitter {
    /**
     * @description 用于中止一个或多个 Web 请求
     *      @param reason 一个可选的字符串，用于描述中止请求的原因
     *     
     */
    abort(reason?: string): void;

    /**
     * @description 用于检查是否已请求中止 
     */
    readonly aborted: boolean;

    /**
     * @description 事件处理程序，当中止请求时触发 
     */
    readonly onabort: (...args: any[])=>any;

}

