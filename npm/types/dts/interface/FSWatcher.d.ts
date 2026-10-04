/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description File system watcher object
 *
 *  When `fs.watch(target)` is called successfully, an object of this type is returned
 *  ```JavaScript
 *  var fs = require("fs");
 *  var watcher = fs.watch((eventType, filename) => {
 *     if (filename) {
 *       console.log(filename);
 *       // Prints: <Buffer ...>
 *     }
 *  });
 *
 *  watcher.close();
 *
 *  // calling fs.watch with callback and options
 *  fs.watch('./tmp', { encoding: 'buffer' }, (eventType, filename) => {
 *   if (filename) {
 *     console.log(filename);
 *     // Prints: <Buffer ...>
 *   }
 * });
 *  ```
 *
 */
declare class Class_FSWatcher extends Class_EventEmitter {
    /**
     * @description Queries and binds the "file change" event, equivalent to on("change", func);
     *
     *      Triggered when any change to the file occurs (content modification or rename).
     *      Callback signature: (eventType: 'change' | 'rename', filename: string | Buffer) => void
     *
     */
    on(event: "change", listener: ()=>void): this;

    once(event: "change", listener: ()=>void): this;

    off(event: "change", listener: ()=>void): this;

    addListener(event: "change", listener: ()=>void): this;

    removeListener(event: "change", listener: ()=>void): this;

    addEventListener(event: "change", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "change", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "change", listener: ()=>void): this;

    prependOnceListener(event: "change", listener: ()=>void): this;

    /**
     * @description Queries and binds the "file change" event, equivalent to on("change", func);
     *
     *      Triggered when any change to the file occurs (content modification or rename).
     *      Callback signature: (eventType: 'change' | 'rename', filename: string | Buffer) => void
     *
     */
    onchange: (()=>void) | null;

    /**
     * @description Queries and binds the "content change only" event, equivalent to on("changeonly", func);
     *
     *      Triggered only when the file content is modified (excluding renames).
     *      Callback signature: (eventType: 'change', filename: string | Buffer) => void
     *
     */
    on(event: "changeonly", listener: ()=>void): this;

    once(event: "changeonly", listener: ()=>void): this;

    off(event: "changeonly", listener: ()=>void): this;

    addListener(event: "changeonly", listener: ()=>void): this;

    removeListener(event: "changeonly", listener: ()=>void): this;

    addEventListener(event: "changeonly", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "changeonly", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "changeonly", listener: ()=>void): this;

    prependOnceListener(event: "changeonly", listener: ()=>void): this;

    /**
     * @description Queries and binds the "content change only" event, equivalent to on("changeonly", func);
     *
     *      Triggered only when the file content is modified (excluding renames).
     *      Callback signature: (eventType: 'change', filename: string | Buffer) => void
     *
     */
    onchangeonly: (()=>void) | null;

    /**
     * @description Queries and binds the "rename only" event, equivalent to on("renameonly", func);
     *
     *      Triggered only when the file is renamed (excluding content modifications).
     *      Callback signature: (eventType: 'rename', filename: string | Buffer) => void
     *
     */
    on(event: "renameonly", listener: ()=>void): this;

    once(event: "renameonly", listener: ()=>void): this;

    off(event: "renameonly", listener: ()=>void): this;

    addListener(event: "renameonly", listener: ()=>void): this;

    removeListener(event: "renameonly", listener: ()=>void): this;

    addEventListener(event: "renameonly", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "renameonly", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "renameonly", listener: ()=>void): this;

    prependOnceListener(event: "renameonly", listener: ()=>void): this;

    /**
     * @description Queries and binds the "rename only" event, equivalent to on("renameonly", func);
     *
     *      Triggered only when the file is renamed (excluding content modifications).
     *      Callback signature: (eventType: 'rename', filename: string | Buffer) => void
     *
     */
    onrenameonly: (()=>void) | null;

    /**
     * @description Queries and binds the "watcher closed" event, equivalent to on("close", func);
     */
    on(event: "close", listener: ()=>void): this;

    once(event: "close", listener: ()=>void): this;

    off(event: "close", listener: ()=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    /**
     * @description Queries and binds the "watcher closed" event, equivalent to on("close", func);
     */
    onclose: (()=>void) | null;

    /**
     * @description Queries and binds the "error occurred" event, equivalent to on("error", func);
     */
    on(event: "error", listener: ()=>void): this;

    once(event: "error", listener: ()=>void): this;

    off(event: "error", listener: ()=>void): this;

    addListener(event: "error", listener: ()=>void): this;

    removeListener(event: "error", listener: ()=>void): this;

    addEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: ()=>void): this;

    /**
     * @description Queries and binds the "error occurred" event, equivalent to on("error", func);
     */
    onerror: (()=>void) | null;

    /**
     * @description Closes the Watcher; no longer receives the corresponding file change events
     */
    close(): void;

    /**
     * @description Increments the reference count, telling fibjs not to exit the process while the watcher is still in use.
     *
     *      When fs.watch() is called with the persistent option set to true (the default), the FSWatcher automatically refs.
     *
     *      @return returns the FSWatcher itself
     *
     */
    ref(): Class_FSWatcher;

    /**
     * @description Decrements the reference count; allows the process to exit while the watcher is still active.
     *
     *      @return returns the FSWatcher itself
     *
     */
    unref(): Class_FSWatcher;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}

