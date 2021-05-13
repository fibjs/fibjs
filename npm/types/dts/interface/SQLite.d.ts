/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description sqlite 数据库连接对象
 * 
 *  使用 db.open 或 db.openSQLite 创建，创建方式：
 *  ```JavaScript
 *  var slite = db.openSQLite("sqlite:/path/to/db");
 *  ```
 * 
 */
declare class Class_SQLite extends Class_DbConnection {
    /**
     * @description 当前数据库文件名 
     */
    readonly fileName: string;

    /**
     * @description 查询和设置数据库超时时间，以毫秒为单位 
     */
    timeout: number;

    /**
     * @description 备份当前数据库到新文件
     * 	 @param fileName 指定备份的数据库文件名 
     */
    backup(fileName: string): void;

    backup(fileName: string, callback: (err: Error | undefined | null)=>any): void;

}

