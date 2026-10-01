/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description AsyncResource is a class used to embed asynchronous context tracking.
 *
 *  When this class is extended, the asynchronous context captured at construction time is preserved and restored when `runInAsyncScope` or `bind` is called.
 *  This is especially useful for callback-based APIs, where the callback must run in the asynchronous context of the resource that initiated the operation.
 *
 *  Example:
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
 *          console.log(als.getStore().requestId); // Output: 123
 *      });
 *      // Later, outside the asynchronous context:
 *      handler.onComplete('ok');
 *  });
 *  ```
 *
 */
declare class Class_AsyncResource extends Class_object {
    /**
     * @description Creates a new AsyncResource instance
     *
     *      @param type the type of the async resource, used for diagnostics
     *      @param triggerAsyncId optional. A numeric triggerAsyncId or an options object containing the following properties:
     *        - triggerAsyncId: the async id that triggered this resource
     *        - requireManualDestroy: if true, the resource is not destroyed automatically
     *
     */
    constructor(type: string, triggerAsyncId?: any);

    /**
     * @description Gets the unique async id assigned to this resource
     *
     *      @return returns the numeric async id
     *
     */
    asyncId(): number;

    /**
     * @description Gets the trigger async id of this resource
     *
     *      @return returns the numeric trigger async id
     *
     */
    triggerAsyncId(): number;

    /**
     * @description Executes a function in the asynchronous context of this resource
     *
     *      The callback function is invoked in the asynchronous context that was active when this AsyncResource was constructed,
     *      allowing AsyncLocalStorage stores to be restored correctly.
     *
     *      @param fn the function to execute
     *      @param thisArg the `this` value of the callback. Default is undefined.
     *      @param args additional parameters passed to the callback
     *      @return returns the return value of the callback function
     *
     */
    runInAsyncScope(fn: (...args: any[])=>any, thisArg?: any, ...args: any[]): any;

    /**
     * @description Marks this resource as destroyed
     *
     *      In fibjs this is a no-op, kept for compatibility with existing API calls.
     *
     *      @return returns a reference to this AsyncResource
     *
     */
    emitDestroy(): Class_AsyncResource;

    /**
     * @description Binds a function to run within the asynchronous scope of this resource
     *
     *      The returned function has an `asyncResource` property referencing this AsyncResource instance.
     *
     *      @param fn the function to bind
     *      @param thisArg optional `this` value of the function
     *      @return returns the bound function
     *
     */
    bind(fn: (...args: any[])=>any, thisArg?: any): (...args: any[])=>any;

    /**
     * @description Static method that binds a function to the current asynchronous context
     *
     *      Creates an internal AsyncResource and binds the function to it.
     *
     *      @param fn the function to bind
     *      @param type optional internal AsyncResource type string. Default is "bound-anonymous-fn".
     *      @param thisArg optional `this` value of the function
     *      @return returns the bound function
     *
     */
    static bind(fn: (...args: any[])=>any, type?: string, thisArg?: any): (...args: any[])=>any;

}

