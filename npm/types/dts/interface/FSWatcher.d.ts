/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Watches a file or directory with the platform notification service
 *
 *  Returned by fs.watch. It reports which file changed and whether its content or its name
 *  changed, so it is the tool for reacting to changes without polling; fs.watchFile polls the
 *  status through a StatsWatcher instead and hands the before/after Stat objects to one
 *  callback.
 *
 *  Events:
 *
 *  - `'change'` (eventType, filename) — any change of the target: eventType is 'change' for a
 *    content modification and 'rename' for a create, delete or rename. The callback passed to
 *    fs.watch is bound to this event only; `on('change', ...)` yields the same stream.
 *  - `'changeonly'` (eventType, filename) — emitted together with 'change' when only the
 *    content changed; eventType is always 'change'. fibjs extension, Node.js has no such event.
 *  - `'renameonly'` (eventType, filename) — emitted together with 'change' when only the name
 *    changed; eventType is always 'rename'. fibjs extension.
 *  - `'close'` — emitted once when close() releases the watcher.
 *  - `'error'` — emitted when the platform watch cannot start or fails later. When fs.watch
 *    cannot watch the target at all (for example a missing path) the event is emitted before
 *    fs.watch returns and the call itself also throws, so wrap fs.watch in try/catch.
 *
 *  Concepts:
 *
 *  - **Event noise is normal**: the exact stream is platform dependent; one write can be
 *    reported as several 'change' events, and a delete can report a change followed by a
 *    rename. Handlers should be idempotent and may guard the first event with a flag.
 *  - **filename**: the affected name relative to the watched directory, or the base name when
 *    a file is watched; a Buffer when the watcher was created with `encoding: 'buffer'`, and an
 *    empty string when the platform does not report a name. A recursive watch reports the path
 *    relative to the watched root.
 *  - **Recursive watching**: `fs.watch(dir, { recursive: true })` is native on win32/darwin
 *    and emulated by fibjs on Linux; the emulation may coalesce or repeat events, so treat them
 *    as notifications, not as a complete transaction log.
 *  - **Keep-alive**: with the default `persistent: true` the watcher refs the isolate and keeps
 *    the process alive; `unref()` or `persistent: false` releases it and `ref()` takes it back.
 *    close() releases the platform handle and the reference; a closed watcher cannot restart.
 *
 *  Obtained from:
 *  - `fs.watch(target[, options][, callback])` — the only entry point. fibjs exposes neither a
 *    global FSWatcher constructor nor fs.FSWatcher (plans/compat-differences.md 2.18).
 *
 *  Example 1 — watch a directory and print what changes:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const coroutine = require('coroutine');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-watch-'));
 *  const watcher = fs.watch(dir, (eventType, filename) => {
 *      console.log(eventType, String(filename));
 *  });
 *  watcher.on('changeonly', () => console.log('content changed'));
 *
 *  coroutine.sleep(100); // let the platform watch start before the first change
 *  fs.writeFile(path.join(dir, 'a.txt'), 'hello');
 *  coroutine.sleep(300); // wait for the notification
 *
 *  watcher.close();
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — watch one file with a one-shot guard:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const coroutine = require('coroutine');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-watch-'));
 *  const file = path.join(dir, 'data.txt');
 *  fs.writeFile(file, 'first');
 *
 *  let seen = false;
 *  const watcher = fs.watch(file, (eventType, filename) => {
 *      if (seen)
 *          return; // a single change can be reported more than once
 *      seen = true;
 *      console.log(eventType, String(filename));
 *  });
 *
 *  coroutine.sleep(100);
 *  fs.writeFile(file, 'second');
 *  coroutine.sleep(300);
 *
 *  watcher.close();
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  A watcher must be closed to release the platform handle; with the default persistent option
 *  it also keeps the process alive until then.
 *
 */
declare class Class_FSWatcher extends Class_EventEmitter {
    /**
     * @description Binds the "change" event, equivalent to on("change", func)
     *
     *      Emitted for any change of the target: eventType is 'change' for a content modification
     *      and 'rename' for a create, delete or rename. The callback passed to fs.watch is bound to
     *      this event only. filename is '' when the platform does not report a name.
     *      @param eventType the event type, either 'change' or 'rename'
     *      @param filename the changed file name; a Buffer when the 'buffer' encoding was set
     *
     */
    on(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    once(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    off(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    addListener(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    removeListener(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    addEventListener(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    prependOnceListener(event: "change", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    /**
     * @description Binds the "change" event, equivalent to on("change", func)
     *
     *      Emitted for any change of the target: eventType is 'change' for a content modification
     *      and 'rename' for a create, delete or rename. The callback passed to fs.watch is bound to
     *      this event only. filename is '' when the platform does not report a name.
     *      @param eventType the event type, either 'change' or 'rename'
     *      @param filename the changed file name; a Buffer when the 'buffer' encoding was set
     *
     */
    onchange: ((eventType: string, filename: string | Class_Buffer)=>void) | null;

    /**
     * @description Binds the "changeonly" event, equivalent to on("changeonly", func)
     *
     *      Emitted in addition to 'change' when the content was modified and the name did not
     *      change; eventType is always 'change'. fibjs extension, Node.js has no such event.
     *      @param eventType the event type, always 'change'
     *      @param filename the changed file name; a Buffer when the 'buffer' encoding was set
     *
     */
    on(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    once(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    off(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    addListener(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    removeListener(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    addEventListener(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    prependOnceListener(event: "changeonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    /**
     * @description Binds the "changeonly" event, equivalent to on("changeonly", func)
     *
     *      Emitted in addition to 'change' when the content was modified and the name did not
     *      change; eventType is always 'change'. fibjs extension, Node.js has no such event.
     *      @param eventType the event type, always 'change'
     *      @param filename the changed file name; a Buffer when the 'buffer' encoding was set
     *
     */
    onchangeonly: ((eventType: string, filename: string | Class_Buffer)=>void) | null;

    /**
     * @description Binds the "renameonly" event, equivalent to on("renameonly", func)
     *
     *      Emitted in addition to 'change' when the name changed and the content did not;
     *      eventType is always 'rename'. fibjs extension, Node.js has no such event.
     *      @param eventType the event type, always 'rename'
     *      @param filename the renamed file name; a Buffer when the 'buffer' encoding was set
     *
     */
    on(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    once(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    off(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    addListener(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    removeListener(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    addEventListener(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    prependOnceListener(event: "renameonly", listener: (eventType: string, filename: string | Class_Buffer)=>void): this;

    /**
     * @description Binds the "renameonly" event, equivalent to on("renameonly", func)
     *
     *      Emitted in addition to 'change' when the name changed and the content did not;
     *      eventType is always 'rename'. fibjs extension, Node.js has no such event.
     *      @param eventType the event type, always 'rename'
     *      @param filename the renamed file name; a Buffer when the 'buffer' encoding was set
     *
     */
    onrenameonly: ((eventType: string, filename: string | Class_Buffer)=>void) | null;

    /**
     * @description Binds the "close" event, equivalent to on("close", func)
     *
     *      Emitted once when close() releases the watcher; a second close() is a no-op and does not
     *      emit again.
     *
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
     * @description Binds the "close" event, equivalent to on("close", func)
     *
     *      Emitted once when close() releases the watcher; a second close() is a no-op and does not
     *      emit again.
     *
     */
    onclose: (()=>void) | null;

    /**
     * @description Binds the "error" event, equivalent to on("error", func)
     *
     *      Carries the failure reported by the platform watch. A watcher that cannot start also
     *      makes fs.watch throw and emits this event before the call returns, so a listener
     *      attached afterwards may miss the first failure; guard fs.watch with try/catch.
     *
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
     * @description Binds the "error" event, equivalent to on("error", func)
     *
     *      Carries the failure reported by the platform watch. A watcher that cannot start also
     *      makes fs.watch throw and emits this event before the call returns, so a listener
     *      attached afterwards may miss the first failure; guard fs.watch with try/catch.
     *
     */
    onerror: (()=>void) | null;

    /**
     * @description Closes the watcher and releases the platform handle
     *
     *      Stops the file change events and emits a single 'close' event; the watcher cannot be
     *      started again. Calling close() twice is a no-op (Node.js behaves the same). A watcher
     *      created with the default persistent option also keeps the process alive until close().
     *
     *      Example — close once and observe a single 'close' event:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-watch-'));
     *      const watcher = fs.watch(dir, () => {});
     *      let closed = 0;
     *      watcher.on('close', () => closed++);
     *      watcher.close();
     *      watcher.close();     // no second 'close' event
     *      console.log(closed); // 1
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    close(): void;

    /**
     * @description Increments the reference count so the process stays alive
     *
     *      A watcher created with the default persistent option refs itself; ref() adds another
     *      reference and returns the watcher so the call can be chained. Node.js behaves the same
     *      way.
     *      @return returns the FSWatcher itself
     *
     */
    ref(): Class_FSWatcher;

    /**
     * @description Decrements the reference count so the process may exit
     *
     *      The watcher keeps receiving events while the process is running, but it no longer keeps
     *      the process alive on its own; persistent: false does the same at creation. Returns the
     *      watcher so the call can be chained.
     *
     *      Example — a watcher that does not hold the process open:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-watch-'));
     *      const watcher = fs.watch(dir, () => {}).unref();
     *      watcher.ref(); // take the reference back; also returns the watcher
     *      watcher.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @return returns the FSWatcher itself
     *
     */
    unref(): Class_FSWatcher;

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

