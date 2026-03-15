/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description AsyncResource 是用于嵌入异步上下文跟踪的类。
 * 
 *  扩展此类时，在构造时捕获的异步上下文会被保留，并在调用 `runInAsyncScope` 或 `bind` 时恢复。
 *  这对于基于回调的 API 特别有用，其中回调必须在发起操作的资源的异步上下文中运行。
 * 
 *  示例：
 *  ```javascript
 *  const { AsyncResource, AsyncLocalStorage } = require('async_hooks');
 *  const als = new AsyncLocalStorage();
 * 
 *  class RequestHandler extends AsyncResource {
 *      constructor(callback) {
 *          super('RequestHandler');
 *          this.callback = callback;
 *      }
 * 
 *      onComplete(result) {
 *          this.runInAsyncScope(this.callback, null, null, result);
 *      }
 *  }
 * 
 *  als.run({ requestId: '123' }, () => {
 *      const handler = new RequestHandler((err, data) => {
 *          console.log(als.getStore().requestId); // 输出: 123
 *      });
 *      // 稍后，在异步上下文之外：
 *      handler.onComplete('ok');
 *  });
 *  ```
 * 
 */
declare class Class_AsyncResource extends Class_object {
    /**
     * @description 创建一个新的 AsyncResource 实例
     * 
     *      @param type 异步资源的类型，用于诊断信息
     *      @param triggerAsyncId 可选。数字类型的 triggerAsyncId 或包含以下属性的选项对象：
     *        - triggerAsyncId: 触发此资源的异步 ID
     *        - requireManualDestroy: 如果为 true，资源不会被自动销毁
     *     
     */
    constructor(type: string, triggerAsyncId?: any);

    /**
     * @description 获取分配给此资源的唯一异步 ID
     * 
     *      @return 返回数字类型的异步 ID
     *     
     */
    asyncId(): number;

    /**
     * @description 获取此资源的触发异步 ID
     * 
     *      @return 返回数字类型的触发异步 ID
     *     
     */
    triggerAsyncId(): number;

    /**
     * @description 在此资源的异步上下文中执行函数
     * 
     *      回调函数在构造此 AsyncResource 时激活的异步上下文中调用，
     *      允许 AsyncLocalStorage 存储被正确恢复。
     * 
     *      @param fn 要执行的函数
     *      @param thisArg 回调的 `this` 值。默认为 undefined。
     *      @param args 传递给回调的额外参数
     *      @return 返回回调函数的返回值
     *     
     */
    runInAsyncScope(fn: (...args: any[])=>any, thisArg?: any, ...args: any[]): any;

    /**
     * @description 将此资源标记为已销毁
     * 
     *      在 fibjs 中这是一个空操作，但为了与 Node.js API 兼容而保留。
     * 
     *      @return 返回此 AsyncResource 的引用
     *     
     */
    emitDestroy(): Class_AsyncResource;

    /**
     * @description 绑定一个函数使其在此资源的异步作用域内运行
     * 
     *      返回的函数将有一个 `asyncResource` 属性引用此 AsyncResource 实例。
     * 
     *      @param fn 要绑定的函数
     *      @param thisArg 可选的函数 `this` 值
     *      @return 返回绑定后的函数
     *     
     */
    bind(fn: (...args: any[])=>any, thisArg?: any): (...args: any[])=>any;

    /**
     * @description 静态方法，将函数绑定到当前异步上下文
     * 
     *      创建一个内部 AsyncResource 并将函数绑定到它。
     * 
     *      @param fn 要绑定的函数
     *      @param type 可选的内部 AsyncResource 类型字符串。默认为 "bound-anonymous-fn"。
     *      @param thisArg 可选的函数 `this` 值
     *      @return 返回绑定后的函数
     *     
     */
    static bind(fn: (...args: any[])=>any, type?: string, thisArg?: any): (...args: any[])=>any;

}

