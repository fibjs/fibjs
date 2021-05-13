/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description 文件系统观察对象
 * 
 *  当调用 `fs.watch(target)` 成功时, 返回该类型对象
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
 *  // 带回调地调用
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
     * @description 查询和绑定"文件改变"事件，相当于 on("change", func); 
     */
    onchange: (...args: any[])=>any;

    /**
     * @description 查询和绑定"watcher 关闭"的事件，相当于 on("close", func); 
     */
    onclose: (...args: any[])=>any;

    /**
     * @description 查询和绑定"错误发生"的事件，相当于 on("error", func); 
     */
    onerror: (...args: any[])=>any;

    /**
     * @description 关闭该 Watcher, 不再接收对应的文件变化处理事件 
     */
    close(): void;

}

