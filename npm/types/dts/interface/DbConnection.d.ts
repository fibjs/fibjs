/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 数据库连接对象，用于建立和维护一个数据库连接会话。
 */
declare class Class_DbConnection extends Class_object {
    /**
     * @description 查询当前连接数据库类型 
     */
    readonly type: string;

    /**
     * @description 关闭当前数据库连接 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 选择当前数据库连接的缺省数据库
     * 	 @param dbName 指定数据库名
     * 	 
     */
    use(dbName: string): void;

    use(dbName: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 在当前数据库连接上启动一个事务
     *     
     *      @param point 指定事务的名称，缺省不指定
     *     
     */
    begin(point?: string): void;

    begin(point?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 提交当前数据库连接上的事务
     *     
     *      @param point 指定事务的名称，缺省不指定
     *     
     */
    commit(point?: string): void;

    commit(point?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 回滚当前数据库连接上的事务
     *     
     *      @param point 指定事务的名称，缺省不指定
     *     
     */
    rollback(point?: string): void;

    rollback(point?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 进入事务执行一个函数，并根据函数执行情况提交或者回滚 
     *      func 执行有三种结果：
     *      * 函数正常返回，包括运行结束和主动 return，此时事务将自动提交
     *      * 函数返回 false，此时事务将回滚
     *      * 函数运行错误，事务自动回滚
     * 
     *      @param func 以事务方式执行的函数
     *      @return 返回事务是否提交，正常 commit 时返回 true, rollback 时返回 false，如果事务出错则抛出错误
     *     
     */
    trans(func: (...args: any[])=>any): boolean;

    /**
     * @description 进入事务执行一个函数，并根据函数执行情况提交或者回滚 
     *      func 执行有三种结果：
     *      * 函数正常返回，包括运行结束和主动 return，此时事务将自动提交
     *      * 函数返回 false，此时事务将回滚
     *      * 函数运行错误，事务自动回滚
     * 
     *      @param point 指定事务的名称
     *      @param func 以事务方式执行的函数
     *      @return 返回事务是否提交，正常 commit 时返回 true, rollback 时返回 false，如果事务出错则抛出错误
     *     
     */
    trans(point: string, func: (...args: any[])=>any): boolean;

    /**
     * @description 执行一个 sql 命令，并返回执行结果
     * 
     *      @param sql 字符串
     *      @return 返回包含结果记录的数组，如果请求是 UPDATE 或者 INSERT，返回结果还会包含 affected 和 insertId，mssql 不支持 insertId。
     *      
     */
    execute(sql: string): any[];

    execute(sql: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 执行一个 sql 命令，并返回执行结果，可根据参数格式化字符串
     * 
     *      @param sql 格式化字符串，可选参数用 ? 指定。例如：'SELECT FROM TEST WHERE [id]=?'
     *      @param args 可选参数列表
     *      @return 返回包含结果记录的数组，如果请求是 UPDATE 或者 INSERT，返回结果还会包含 affected 和 insertId，mssql 不支持 insertId。
     *      
     */
    execute(sql: string, ...args: any[]): any[];

    /**
     * @description 创建数据表
     * 
     *      @param opts 参数列表
     *      
     */
    createTable(opts: FIBJS.GeneralObject): void;

    createTable(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 删除数据表
     * 
     *      @param opts 参数列表
     *      
     */
    dropTable(opts: FIBJS.GeneralObject): void;

    dropTable(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 创建数据表索引
     * 
     *      @param opts 参数列表
     *      
     */
    createIndex(opts: FIBJS.GeneralObject): void;

    createIndex(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 删除数据表索引
     * 
     *      @param opts 参数列表
     *      
     */
    dropIndex(opts: FIBJS.GeneralObject): void;

    dropIndex(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 插入新记录
     * 
     *      @param opts 参数列表
     *      @return 返回包含插入的 id，如果引擎不支持则返回 0
     *      
     */
    insert(opts: FIBJS.GeneralObject): number;

    insert(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据指定的条件查询数据
     * 
     *      @param opts 参数列表
     *      @return 返回包含结果记录
     *      
     */
    find(opts: FIBJS.GeneralObject): any[];

    find(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 根据指定的条件统计数据记录数
     * 
     *      @param opts 参数列表
     *      @return 返回包含结果记录数
     *      
     */
    count(opts: FIBJS.GeneralObject): number;

    count(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据指定的条件更新数据
     * 
     *      @param opts 参数列表
     *      @return 返回包含更新的记录数
     *      
     */
    update(opts: FIBJS.GeneralObject): number;

    update(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据指定的条件删除数据
     * 
     *      @param opts 可选参数列表
     *      @return 返回包含更新的记录数
     *      
     */
    remove(opts: FIBJS.GeneralObject): number;

    remove(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 格式化一个 sql 命令，并返回格式化结果
     * 
     *      @param method 指定请求的方法
     *      @param opts 可选参数列表
     *      @return 返回格式化之后的 sql 命令
     *      
     */
    format(method: string, opts: FIBJS.GeneralObject): string;

    /**
     * @description 格式化一个 sql 命令，并返回格式化结果
     * 
     *      @param sql 格式化字符串，可选参数用 ? 指定。例如：'SELECT FROM TEST WHERE [id]=?'
     *      @param args 可选参数列表
     *      @return 返回格式化之后的 sql 命令
     *      
     */
    format(sql: string, ...args: any[]): string;

}

