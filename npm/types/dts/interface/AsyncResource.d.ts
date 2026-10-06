/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description AsyncResource captures the asynchronous context at construction time so it can be restored later; it is the building block for wrapping callback-based APIs whose callbacks must run in the context of the operation that started them
 *
 *  Extend this class (or use the static `bind`) when an operation outlives the context that created
 *  it: store the context at construction, then run the completion callback through
 *  `runInAsyncScope`. This is how a database query, a worker task or an HTTP handler can restore the
 *  AsyncLocalStorage store of the request that scheduled it, even when the callback runs in a
 *  different fiber or after an await.
 *
 *  The class records a diagnostic `(asyncId, triggerAsyncId)` pair and the captured context; fibjs
 *  does not expose the Node.js async_hooks hook callbacks, so the ids are informational and
 *  `emitDestroy` is a no-op.
 *
 *  Concepts:
 *
 *  - **Resource context**: the constructor captures the async context of the creating fiber; the
 *    capture is restored around `runInAsyncScope` and `bind` calls. An instance created outside any
 *    AsyncLocalStorage context keeps an empty context and clears the store while it runs.
 *  - **Propagation into user-created resources**: a manually created resource is the way to give
 *    context to code that fibjs cannot instrument by itself (a third-party event source, a poll
 *    loop, a queue); bind the completion path once, and all AsyncLocalStorage instances propagate
 *    with it.
 *  - **Node.js comparison**: `new AsyncResource(type, options)`, `runInAsyncScope`, `bind`, the
 *    static `bind`, `asyncId` and `triggerAsyncId` match Node.js; `requireManualDestroy` is accepted
 *    but ignored because there are no destroy hooks, and without an explicit triggerAsyncId fibjs
 *    records 0 while Node.js records the current executionAsyncId.
 *
 *  Import:
 *  ```JavaScript
 *  const { AsyncResource } = require('async_hooks');
 *  ```
 *
 *  Obtained from:
 *  - `new AsyncResource(type[, options])` — create a resource around your own operation;
 *  - `AsyncResource.bind(fn[, type[, thisArg]])` — the same capture without an explicit instance.
 *
 *  Example 1 — a callback-based API that must restore the request context:
 *  ```JavaScript
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
 *  const handler = als.run({ requestId: '123' }, () => {
 *      return new RequestHandler((err, data) => console.log(als.getStore().requestId, data));
 *  });
 *
 *  // the callback runs outside the run() scope but still sees the request context
 *  handler.onComplete('ok'); // 123 ok
 *  ```
 *
 *  Example 2 — bind an event callback to the context that subscribed it:
 *  ```JavaScript
 *  const { AsyncResource, AsyncLocalStorage } = require('async_hooks');
 *  const als = new AsyncLocalStorage();
 *
 *  const subscription = als.run({ topic: 'news' }, () => {
 *      return {
 *          onEvent: AsyncResource.bind((value) => {
 *              console.log(als.getStore().topic, value); // news update
 *          })
 *      };
 *  });
 *
 *  subscription.onEvent('update');
 *  ```
 *
 *  Example 3 — a resource restores its context in a later immediate:
 *  ```JavaScript
 *  const { AsyncResource, AsyncLocalStorage } = require('async_hooks');
 *  const als = new AsyncLocalStorage();
 *
 *  als.run('captured', () => {
 *      const resource = new AsyncResource('Job');
 *      setImmediate(() => resource.runInAsyncScope(() => console.log(als.getStore())));
 *  });
 *  ```
 *
 */
declare class Class_AsyncResource extends Class_object {
    /**
     * @description Creates a new AsyncResource instance
     *
     *      The constructor captures the asynchronous context of the calling fiber, so any callback run
     *      through `runInAsyncScope` or through the bound function sees the AsyncLocalStorage stores of
     *      the creation point.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "triggerAsyncId": 0,          // id of the triggering resource, informational
     *          "requireManualDestroy": false // accepted for Node.js compatibility, ignored by fibjs
     *      })
     *      ```
     *
     *      A number may be passed directly as the second argument instead of the options object. type is
     *      required and must be a string; a missing or non-string type throws a TypeError.
     *
     *      @param type the type of the async resource, used for diagnostics
     *      @param triggerAsyncId optional. A numeric triggerAsyncId or an options object containing the
     *        following properties:
     *        - triggerAsyncId: the async id that triggered this resource
     *        - requireManualDestroy: if true, the resource is not destroyed automatically
     *
     */
    constructor(type: string, triggerAsyncId?: any);

    /**
     * @description Gets the unique async id assigned to this resource
     *
     *      Ids come from a process-wide counter and are unique within the process; the first resource
     *      created gets 1. In Node.js the id is the async id allocated by the async hooks subsystem, so
     *      the values differ, but they are used in the same informational way.
     *
     *      @return returns the numeric async id
     *
     */
    asyncId(): number;

    /**
     * @description Gets the trigger async id of this resource
     *
     *      The value is the second constructor argument, or the triggerAsyncId option inside it. It
     *      records what created the resource for diagnostics; fibjs defaults to 0 when the argument is
     *      omitted, while Node.js defaults to the current executionAsyncId().
     *
     *      @return returns the numeric trigger async id
     *
     */
    triggerAsyncId(): number;

    /**
     * @description Executes a function in the asynchronous context of this resource
     *
     *      Saves the current context, restores the context captured when this AsyncResource was
     *      constructed, calls fn with thisArg as `this` and the extra args, then restores the previous
     *      context; the return value of fn is returned. When thisArg is undefined, the function is called
     *      with the global object as `this`, matching the fibjs call of a plain function.
     *
     *      If fn throws, the error propagates after the context is restored. AsyncLocalStorage stores of
     *      the captured context are visible inside fn and in the asynchronous operations it creates.
     *
     *      Example — call a stored callback later while restoring its original context:
     *      ```JavaScript
     *      const { AsyncResource, AsyncLocalStorage } = require('async_hooks');
     *      const als = new AsyncLocalStorage();
     *
     *      const resource = als.run({ id: 7 }, () => new AsyncResource('Later'));
     *      console.log(als.getStore()); // undefined
     *      console.log(resource.runInAsyncScope(() => als.getStore().id)); // 7
     *      ```
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
     *      In fibjs this is a no-op kept for Node.js compatibility: fibjs does not run async_hooks
     *      destroy hooks, and calling it more than once does not raise an error. It returns the resource
     *      itself so calls can be chained.
     *
     *      @return returns a reference to this AsyncResource
     *
     */
    emitDestroy(): Class_AsyncResource;

    /**
     * @description Binds a function to run within the asynchronous scope of this resource
     *
     *      Returns a new function that restores the context of this resource (captured at construction),
     *      calls fn (with thisArg as `this` when given, otherwise the `this` of the bound call) and
     *      returns its result. The bound function carries an `asyncResource` property referencing this
     *      instance, which is a fibjs extension.
     *
     *      Example — bind a completion callback and call it outside the context:
     *      ```JavaScript
     *      const { AsyncResource } = require('async_hooks');
     *
     *      const resource = new AsyncResource('Task');
     *      const done = resource.bind((value) => value * 2, null);
     *      console.log(done(21)); // 42
     *      console.log(done.asyncResource === resource); // true
     *      ```
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
     *      Creates an internal AsyncResource with the given type, then binds fn to the context captured at
     *      this call. It is equivalent to creating a resource and calling its `bind` method, without
     *      keeping the instance; the returned function also exposes the internal resource through the
     *      `asyncResource` property.
     *
     *      Example — capture the current context and reuse it later:
     *      ```JavaScript
     *      const { AsyncResource, AsyncLocalStorage } = require('async_hooks');
     *      const als = new AsyncLocalStorage();
     *
     *      const read = als.run('request-1', () => AsyncResource.bind(() => als.getStore()));
     *      als.run('request-2', () => console.log(read())); // request-1
     *      ```
     *
     *      @param fn the function to bind
     *      @param type optional internal AsyncResource type string. Default is "bound-anonymous-fn".
     *      @param thisArg optional `this` value of the function
     *      @return returns the bound function
     *
     */
    static bind(fn: (...args: any[])=>any, type?: string, thisArg?: any): (...args: any[])=>any;

}

