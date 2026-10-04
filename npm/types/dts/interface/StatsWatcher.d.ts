/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description File Stats watcher object
 *
 *  When `fs.watchFile(target, onchange)` is called successfully, an object of this type is returned
 *  ```JavaScript
 *  var fs = require("fs");
 *  var statsWatcher = fs.watchFile(target, (curStat, prevStat) => {
 *     // process
 *     // ...
 *
 *     statsWatcher.unref();
 *  });
 *  ```
 *  **Notes** The onchange callback is triggered if and **only if** the mtime property of the watched target file changes
 *
 *  Merely accessing the target file does not trigger the onchange callback.
 *
 *  If, when `fs.watchFile(target)` is called, the file or directory represented by target does not exist yet, the onchange callback will **not** be called until the target is created, after which the callback starts being called.
 *  If the target file is deleted while the watcher is working, no further callbacks will be generated
 *
 */
declare class Class_StatsWatcher extends Class_EventEmitter {
    /**
     * @description Queries and binds the "file change" event, equivalent to on("change", func);
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
     */
    onchange: (()=>void) | null;

    /**
     * @description Stops watching the target file path and clears the reference count (no longer holds the process)
     *
     */
    close(): void;

    /**
     * @description Stops watching the target file path and clears the reference count (no longer holds the process); equivalent to close()
     *
     */
    stop(): void;

    /**
     * @description Increments the reference count, telling fibjs not to exit the process while the watcher is still in use,
     *   A StatsWatcher obtained via `fs.watchFile()` has already called this method by default, so it holds the process by default.
     *
     *   @return returns the StatsWatcher itself
     *
     */
    ref(): Class_StatsWatcher;

    /**
     * @description Decrements the reference count
     *
     *   @return returns the StatsWatcher itself
     *
     */
    unref(): Class_StatsWatcher;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}

