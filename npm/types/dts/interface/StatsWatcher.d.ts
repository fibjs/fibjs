/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description 文件 Stats 观察对象
 * 
 *  当调用 `fs.watchFile(target, onchange)` 成功时, 返回该类型对象
 *  ```JavaScript
 *  var fs = require("fs");
 *  var statsWatcher = fs.watchFile(target, (curStat, prevStat) => {
 *     // process
 *     // ...
 * 
 *     statsWatcher.unref();
 *  });
 *  ```
 *  **注意** 当且**仅当**被观察的目标文件 target 的 mtime 属性发生变化时才会触发 onchange 回调
 *  
 *  单纯地访问(access)目标文件 target 不会触发 onchange 回调.
 * 
 *  如果调用 `fs.watchFile(target)` 时, target 表示的文件或目录还不存在, 则onchange 回调**不会**被调用, 一直到该 target 被创建, 该回调才会开始被调用.
 *  如果在 watcher 工作途中, )目标文件被删除, 则后续不会再有回调产生
 * 
 */
declare class Class_StatsWatcher extends Class_EventEmitter {
    /**
     * @description 查询和绑定"文件改变"事件，相当于 on("change", func); 
     */
    onchange: (...args: any[])=>any;

    /**
     * @description 停止对目标文件路径的观察, 清除引用计数(不再 hold 进程)
     *   
     */
    close(): void;

    /**
     * @description 增加引用计数, 告知 fibjs 只要该 watcher 还在使用就不要退出进程,
     *   经由 `fs.watchFile()` 得到的 StatsWatcher 默认已调用了该方法, 即默认就会 hold 进程.
     * 
     *   @return 返回 StatsWatcher 本身
     *   
     */
    ref(): Class_StatsWatcher;

    /**
     * @description 减少引用计数
     * 
     *   @return 返回 StatsWatcher 本身
     *   
     */
    unref(): Class_StatsWatcher;

}

