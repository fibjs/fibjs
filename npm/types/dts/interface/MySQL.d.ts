/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description mysql 数据库连接对象
 * 
 *  使用 db.open 或 db.openMySQL 创建，创建方式：
 *  ```JavaScript
 *  var mysql = db.openMySQL("mysql://user:pass@host/db");
 *  ```
 * 
 */
declare class Class_MySQL extends Class_DbConnection {
    /**
     * @description 选择当前数据库连接的缺省数据库
     * 	 @param dbName 指定数据库名
     * 	 
     */
    use(): void;

    /**
     * @description 数据库连接接收缓存尺寸 
     */
    rxBufferSize: number;

    /**
     * @description 数据库连接发送缓存尺寸 
     */
    txBufferSize: number;

}

