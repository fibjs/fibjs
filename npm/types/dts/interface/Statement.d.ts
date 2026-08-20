/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 预编译语句对象，可反复执行，支持按条读取
 * 
 * Statement 由 DbConnection.prepare() 创建。与 execute 的"一次性物化全部结果"
 * 不同，Statement 支持 get/all/run/iterate 四种执行方式，其中 iterate 按条
 * 产生行，任意时刻只驻留一行，内存有界。
 * 
 * iterate 返回的迭代器推荐用 for...of 遍历：循环 break 或抛出异常时，引擎
 * 会自动调用迭代器的 return() 释放游标（IteratorClose 语义），无需手工干预：
 * 
 * ```js
 * var stmt = conn.prepare('SELECT * FROM big_table WHERE region = ?');
 * for (var row of stmt.iterate('east')) {
 *     process(row);        // 同一时刻只驻留一行
 * }
 * // break/异常/跑完均自动释放游标，连接立即可复用
 * ```
 * 
 * 手动调用迭代器的 next()/return() 是危险操作：迭代器在结果耗尽前一直保持
 * 游标打开，若在 break 或异常时忘记调用 return()，游标将泄漏并占用连接
 * （同连接后续语句报 BUSY，SQLite 下还可能阻塞其他连接的表结构变更）。
 * 选择手动方式即需自行承担资源释放责任。
 * 
 */
declare class Class_Statement extends Class_object {
    /**
     * @description 执行语句并返回第一行，无结果时返回 undefined
     *          @param args 绑定参数
     *          @return 返回第一行对象，无结果时返回 undefined
     *          
     */
    get(...args: any[]): any;

    /**
     * @description 执行语句并返回全部行（一次性物化）
     *          @param args 绑定参数
     *          @return 返回全部行对象组成的数组
     *          
     */
    all(...args: any[]): any[];

    /**
     * @description 执行不返回结果集的语句
     *          @param args 绑定参数
     *          @return 返回 { changes, lastInsertRowid } 对象
     *          
     */
    run(...args: any[]): any;

    /**
     * @description 执行语句并返回按条读取的迭代器
     * 
     *          推荐使用 for...of 遍历（break/异常自动释放游标）；手动调用
     *          next()/return() 是危险操作，必须自行保证异常与提前结束时调用
     *          return() 释放游标，否则游标泄漏会占用连接。
     * 
     *          @param args 绑定参数
     *          @return 返回行迭代器，逐行产生行对象，内存有界
     *          
     */
    iterate(...args: any[]): Iterator<any>;

    /**
     * @description 返回结果列元数据
     *          @return 返回列元数据数组，每项包含 name/type 等属性
     *          
     */
    columns(): any[];

    columns(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 返回结果列元数据
     *          @return 返回列元数据数组，每项包含 name/type 等属性
     *          
     */
    columnsSync(): any[];

    /**
     * @description 返回结果列元数据
     *          @return 返回列元数据数组，每项包含 name/type 等属性
     *          
     */
    columnsAsync(): Promise<any[]>;

    /**
     * @description 当前语句原始 SQL 
     */
    readonly sourceSQL: string;

    /**
     * @description 主动关闭并释放底层句柄；迭代结束后自动释放，可重复调用 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 主动关闭并释放底层句柄；迭代结束后自动释放，可重复调用 
     */
    closeSync(): void;

    /**
     * @description 主动关闭并释放底层句柄；迭代结束后自动释放，可重复调用 
     */
    closeAsync(): Promise<void>;

}

