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
     * @description 查询和绑定"文件改变"事件，相当于 on("change", func);
     *      
     *      当文件发生任何变化（内容修改或重命名）时触发。
     *      回调函数签名: (eventType: 'change' | 'rename', filename: string | Buffer) => void
     *      
     */
    on(event: "change", listener: ()=>void): this;

    /**
     * @description 查询和绑定"仅内容改变"事件，相当于 on("changeonly", func);
     *      
     *      仅当文件内容修改时触发（不包括重命名）。
     *      回调函数签名: (eventType: 'change', filename: string | Buffer) => void
     *      
     */
    on(event: "changeonly", listener: ()=>void): this;

    /**
     * @description 查询和绑定"仅重命名"事件，相当于 on("renameonly", func);
     *      
     *      仅当文件重命名时触发（不包括内容修改）。
     *      回调函数签名: (eventType: 'rename', filename: string | Buffer) => void
     *      
     */
    on(event: "renameonly", listener: ()=>void): this;

    /**
     * @description 查询和绑定"watcher 关闭"的事件，相当于 on("close", func); 
     */
    on(event: "close", listener: ()=>void): this;

    /**
     * @description 查询和绑定"错误发生"的事件，相当于 on("error", func); 
     */
    on(event: "error", listener: ()=>void): this;

    /**
     * @description 关闭该 Watcher, 不再接收对应的文件变化处理事件 
     */
    close(): void;

    /**
     * @description 增加引用计数, 告知 fibjs 只要该 watcher 还在使用就不要退出进程。
     *      
     *      当调用 fs.watch() 且 persistent 选项为 true（默认）时，FSWatcher 会自动 ref。
     *      
     *      @return 返回 FSWatcher 本身
     *      
     */
    ref(): Class_FSWatcher;

    /**
     * @description 减少引用计数，允许进程在该 watcher 仍然活跃时退出。
     *      
     *      @return 返回 FSWatcher 本身
     *      
     */
    unref(): Class_FSWatcher;

}

