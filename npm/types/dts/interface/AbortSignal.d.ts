/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 *  brief 该对象允许您与异步操作（例如提取请求）进行通信，并在需要时通过 AbortController 对象中止它 
 */
declare class Class_AbortSignal extends Class_EventEmitter {
    /**
     * @description 用于中止一个或多个 Web 请求
     *      @param reason 一个可选的字符串，用于描述中止请求的原因
     *      @return 返回一个 AbortSignal 对象
     *     
     */
    static abort(reason?: string): Class_AbortSignal;

    /**
     * @description 用于中止一个或多个 Web 请求
     *      @param reason 一个任意类型的值，用于描述中止请求的原因
     *      @return 返回一个 AbortSignal 对象
     *     
     */
    static abort(reason: any): Class_AbortSignal;

    /**
     * @description 创建一个会在超时后自动中止的 AbortSignal
     *      @param ms 超时时间（毫秒）
     *      @return 返回一个将在 ms 毫秒后中止的 AbortSignal 对象
     *     
     */
    static timeout(ms: number): Class_AbortSignal;

    /**
     * @description 如果请求已中止，则抛出异常 
     */
    throwIfAborted(): void;

    /**
     * @description 用于检查是否已请求中止 
     */
    readonly aborted: boolean;

    /**
     * @description 获取中止请求的原因 
     */
    readonly reason: any;

    /**
     * @description 事件处理程序，当中止请求时触发 
     */
    on(event: "abort", listener: ()=>void): this;

}

