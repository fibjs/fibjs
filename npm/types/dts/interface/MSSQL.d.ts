/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description SQL Server 数据库连接对象
 * 
 *  使用 db.open 或 db.openMySQL 创建，创建方式：
 *  ```JavaScript
 *  var sql = db.openMSSQL("mssql://user:pass@host/db");
 *  ```
 * 
 */
declare class Class_MSSQL extends Class_DbConnection {
    /**
     * @description 选择当前数据库连接的缺省数据库
     * 	 @param dbName 指定数据库名
     * 	 
     */
    use(dbName: string): void;

    use(dbName: string, callback: (err: Error | undefined | null)=>any): void;

}

