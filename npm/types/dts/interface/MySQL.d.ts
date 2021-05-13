/// <reference path="../_import/_fibjs.d.ts" />
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
     * @description 数据库连接接收缓存尺寸 
     */
    rxBufferSize: number;

    /**
     * @description 数据库连接发送缓存尺寸 
     */
    txBufferSize: number;

}

