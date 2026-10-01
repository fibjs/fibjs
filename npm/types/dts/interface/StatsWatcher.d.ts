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

}

