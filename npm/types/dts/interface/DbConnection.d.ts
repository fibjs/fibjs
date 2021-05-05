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

    /**
     * @description 在当前数据库连接上启动一个事务
     *     
     *      @param point 指定事务的名称，缺省不指定
     *     
     */
    begin(point: string): void;

    /**
     * @description 提交当前数据库连接上的事务
     *     
     *      @param point 指定事务的名称，缺省不指定
     *     
     */
    commit(point: string): void;

    /**
     * @description 回滚当前数据库连接上的事务
     *     
     *      @param point 指定事务的名称，缺省不指定
     *     
     */
    rollback(point: string): void;

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
    trans(func: ()=>any): boolean;

    /**
     * @description 执行一个 sql 命令，并返回执行结果，可根据参数格式化字符串
     * 
     *      @param sql 格式化字符串，可选参数用 ? 指定。例如：'SELECT FROM TEST WHERE [id]=?'
     *      @param args 可选参数列表
     *      @return 返回包含结果记录的数组，如果请求是 UPDATE 或者 INSERT，返回结果还会包含 affected 和 insertId，mssql 不支持 insertId。
     *      
     */
    execute(sql: string, args: any[]): any[];

    /**
     * @description 创建数据表
     * 
     *      @param opts 参数列表
     *      
     */
    createTable(opts: object): void;

    /**
     * @description 删除数据表
     * 
     *      @param opts 参数列表
     *      
     */
    dropTable(opts: object): void;

    /**
     * @description 创建数据表索引
     * 
     *      @param opts 参数列表
     *      
     */
    createIndex(opts: object): void;

    /**
     * @description 删除数据表索引
     * 
     *      @param opts 参数列表
     *      
     */
    dropIndex(opts: object): void;

    /**
     * @description 插入新记录
     * 
     *      @param opts 参数列表
     *      @return 返回包含插入的 id，如果引擎不支持则返回 0
     *      
     */
    insert(opts: object): number;

    /**
     * @description 根据指定的条件查询数据
     * 
     *      @param opts 参数列表
     *      @return 返回包含结果记录
     *      
     */
    find(opts: object): any[];

    /**
     * @description 根据指定的条件统计数据记录数
     * 
     *      @param opts 参数列表
     *      @return 返回包含结果记录数
     *      
     */
    count(opts: object): number;

    /**
     * @description 根据指定的条件更新数据
     * 
     *      @param opts 参数列表
     *      @return 返回包含更新的记录数
     *      
     */
    update(opts: object): number;

    /**
     * @description 根据指定的条件删除数据
     * 
     *      @param opts 可选参数列表
     *      @return 返回包含更新的记录数
     *      
     */
    remove(opts: object): number;

    /**
     * @description 格式化一个 sql 命令，并返回格式化结果
     * 
     *      @param method 指定请求的方法
     *      @param opts 可选参数列表
     *      @return 返回格式化之后的 sql 命令
     *      
     */
    format(method: string, opts: object): string;

}

