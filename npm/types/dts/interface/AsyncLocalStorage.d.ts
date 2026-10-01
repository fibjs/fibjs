/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description This object allows you to store and retrieve data across asynchronous operations
 *
 *  AsyncLocalStorage can be used to pass data along an asynchronous call chain, similar to thread-local storage. Each asynchronous operation can access the store data from when it was created, without being mixed up with the data of other asynchronous operations.
 *
 *  The following is a simple example:
 *  ```javascript
 *  const { AsyncLocalStorage } = require('async_hooks');
 *  const als = new AsyncLocalStorage();
 *
 *  als.run({ requestId: 'req-123' }, () => {
 *      setTimeout(() => {
 *          const store = als.getStore();
 *          console.log(store.requestId);  // Output: req-123
 *      }, 100);
 *  });
 *  ```
 *
 */
declare class Class_AsyncLocalStorage extends Class_object {
    /**
     * @description Creates a new AsyncLocalStorage instance
     *
     *      options supports the following options:
     *       - defaultValue: the default value returned when there is no stored value
     *       - name: a name for the AsyncLocalStorage instance, for debugging
     *
     *      @param options an optional object used to configure the AsyncLocalStorage instance
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description Gets the name of the AsyncLocalStorage instance
     *
     *      The name is set via options.name when the instance is created, for debugging purposes. If not set, an empty string is returned.
     *
     */
    readonly name: string;

    /**
     * @description Creates a snapshot function that captures the current asynchronous context
     *
     *      The returned function can be called at any time and executes the passed callback in the context captured at snapshot time.
     *
     *      Example:
     *      ```javascript
     *      const runInContext = als.run({ id: 1 }, () => AsyncLocalStorage.snapshot());
     *      // Later in a different context
     *      als.run({ id: 2 }, () => {
     *          runInContext(() => {
     *              console.log(als.getStore().id);  // Output: 1
     *          });
     *      });
     *      ```
     *
     *      @return returns a function that takes a callback and executes it in the captured context
     *
     */
    static snapshot(): (...args: any[])=>any;

    /**
     * @description Binds a function to the current asynchronous context
     *
     *      Returns a new function that executes the original function in the asynchronous context captured at bind time when called.
     *      This is useful for ensuring that callbacks execute in the correct context.
     *
     *      Example:
     *      ```javascript
     *      const bound = als.run({ id: 1 }, () => {
     *          return AsyncLocalStorage.bind(() => als.getStore());
     *      });
     *      als.run({ id: 2 }, () => {
     *          console.log(bound().id);  // Output: 1
     *      });
     *      ```
     *
     *      @param fn the function to bind
     *      @return returns a new function bound to the current context
     *
     */
    static bind(fn: (...args: any[])=>any): (...args: any[])=>any;

    /**
     * @description Disables the current AsyncLocalStorage instance
     *
     *      After calling this method, getStore() returns undefined (unless defaultValue is set), and store data is no longer propagated to subsequent asynchronous operations.
     *
     */
    disable(): void;

    /**
     * @description Gets the store data of the current asynchronous context
     *
     *      If called within a context set by run() or enterWith(), returns the corresponding store data.
     *      If not within any context, returns undefined or the defaultValue specified when the instance was created.
     *
     *      @return returns the store data of the current context
     *
     */
    getStore(): any;

    /**
     * @description Enters a new asynchronous context and sets the store data
     *
     *      Unlike run(), enterWith() does not require a callback function; it sets the store data in the current execution context,
     *      and the data is propagated to all subsequent asynchronous operations until the current asynchronous context ends.
     *
     *      Example:
     *      ```javascript
     *      setImmediate(() => {
     *          als.enterWith({ id: 1 });
     *          setTimeout(() => {
     *              console.log(als.getStore().id);  // Output: 1
     *          }, 100);
     *      });
     *      ```
     *
     *      @param store the data to store
     *
     */
    enterWith(store: any): void;

    /**
     * @description Runs a callback function in a new asynchronous context
     *
     *      Creates a new asynchronous context, sets the store data in that context, and then executes the callback function.
     *      The callback function and all asynchronous operations it triggers can obtain the store data through getStore().
     *      After the callback finishes executing, the context automatically restores to the state before run() was called.
     *
     *      Example:
     *      ```javascript
     *      const result = als.run({ userId: 'user-1' }, (a, b) => {
     *          console.log(als.getStore().userId);  // Output: user-1
     *          return a + b;
     *      }, 10, 20);
     *      console.log(result);  // Output: 30
     *      ```
     *
     *      @param store the data to store
     *      @param callback the callback function to execute
     *      @param args the parameters passed to the callback function
     *      @return returns the return value of the callback function
     *
     */
    run(store: any, callback: (...args: any[])=>any, ...args: any[]): any;

    /**
     * @description Temporarily exits the current asynchronous context to execute a callback function
     *
     *      During the callback execution, getStore() returns undefined (or defaultValue).
     *      After the callback finishes executing, the original context is restored.
     *
     *      Example:
     *      ```javascript
     *      als.run({ id: 1 }, () => {
     *          console.log(als.getStore().id);  // Output: 1
     *          als.exit(() => {
     *              console.log(als.getStore());  // Output: undefined
     *          });
     *          console.log(als.getStore().id);  // Output: 1
     *      });
     *      ```
     *
     *      @param callback the callback function to execute
     *      @param args the parameters passed to the callback function
     *      @return returns the return value of the callback function
     *
     */
    exit(callback: (...args: any[])=>any, ...args: any[]): any;

}

