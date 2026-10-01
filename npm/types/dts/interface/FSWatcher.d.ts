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

    /**
     * @description Queries and binds the "content change only" event, equivalent to on("changeonly", func);
     *
     *      Triggered only when the file content is modified (excluding renames).
     *      Callback signature: (eventType: 'change', filename: string | Buffer) => void
     *
     */
    on(event: "changeonly", listener: ()=>void): this;

    /**
     * @description Queries and binds the "rename only" event, equivalent to on("renameonly", func);
     *
     *      Triggered only when the file is renamed (excluding content modifications).
     *      Callback signature: (eventType: 'rename', filename: string | Buffer) => void
     *
     */
    on(event: "renameonly", listener: ()=>void): this;

    /**
     * @description Queries and binds the "watcher closed" event, equivalent to on("close", func);
     */
    on(event: "close", listener: ()=>void): this;

    /**
     * @description Queries and binds the "error occurred" event, equivalent to on("error", func);
     */
    on(event: "error", listener: ()=>void): this;

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

}

