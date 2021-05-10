/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description odbc 数据库连接对象
 * 
 *  使用 db.open 或 db.openOdbc 创建，创建方式：
 *  ```JavaScript
 *  var slite = db.openOdbc("odbc://user:pass@host/db?driver=PostgreSQL%20ANSI");
 *  ```
 * 
 */
declare class Class_Odbc extends Class_DbConnection {
    /**
     * @description 选择当前数据库连接的缺省数据库
     * 	 @param dbName 指定数据库名
     * 	 
     */
    use(dbName: string): void;

    use(dbName: string, callback: (err: Error | undefined | null)=>any): void;

}

