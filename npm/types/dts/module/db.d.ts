/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/MySQL.d.ts" />
/// <reference path="../interface/SQLite.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/// <reference path="../interface/MongoDB.d.ts" />
/// <reference path="../interface/LevelDB.d.ts" />
/// <reference path="../interface/Redis.d.ts" />
/**
 * @description 数据库访问模块
 * 
 *  基础模块。可用于创建和操作数据库资源，引用方式：
 *  ```JavaScript
 *  var db = require('db');
 *  ```
 *  
 */
declare module 'db' {
    /**
     * @description 打开一个数据库，此方法为通用入口，根据提供的 connString 不同调用不同的引擎
     *      @param connString 数据库描述，如：mysql://user:pass@host/db
     *      @return 返回数据库连接对象
     *      
     */
    function open(connString: string): Class_object;

    function open(connString: string, callback: (err: Error | undefined | null, retVal: Class_object)=>any): void;

    /**
     * @description 打开一个 mysql 数据库
     *      @param connString 数据库描述，如：mysql://user:pass@host/db
     *      @return 返回数据库连接对象
     *      
     */
    function openMySQL(connString: string): Class_MySQL;

    function openMySQL(connString: string, callback: (err: Error | undefined | null, retVal: Class_MySQL)=>any): void;

    /**
     * @description 打开一个 sqlite 数据库
     *      @param connString 数据库描述，如：sqlite:test.db 或者 test.db
     *      @return 返回数据库连接对象
     *      
     */
    function openSQLite(connString: string): Class_SQLite;

    function openSQLite(connString: string, callback: (err: Error | undefined | null, retVal: Class_SQLite)=>any): void;

    /**
     * @description 打开一个 sqlite 数据库
     *      @param connString 数据库描述，如：odbc://user:pass@host/db?driver=PostgreSQL%20ANSI
     *      @return 返回数据库连接对象
     *      
     */
    function openOdbc(connString: string): Class_DbConnection;

    function openOdbc(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description 打开一个 mssql 数据库
     *      @param connString 数据库描述，如：mssql://user:pass@host/db
     *      @return 返回数据库连接对象
     *      
     */
    function openMSSQL(connString: string): Class_DbConnection;

    function openMSSQL(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description 打开一个 postgres 数据库
     *      @param connString 数据库描述，如：psql://user:pass@host/db
     *      @return 返回数据库连接对象
     *      
     */
    function openPSQL(connString: string): Class_DbConnection;

    function openPSQL(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description 打开一个 mongodb 数据库
     *      @param connString 数据库描述
     *      @return 返回数据库连接对象
     *      
     */
    function openMongoDB(connString: string): Class_MongoDB;

    function openMongoDB(connString: string, callback: (err: Error | undefined | null, retVal: Class_MongoDB)=>any): void;

    /**
     * @description 打开一个 leveldb 数据库
     *      @param connString 数据库描述，如：level:test.db 或者 test.db
     *      @return 返回数据库对象
     *      
     */
    function openLevelDB(connString: string): Class_LevelDB;

    function openLevelDB(connString: string, callback: (err: Error | undefined | null, retVal: Class_LevelDB)=>any): void;

    /**
     * @description 打开一个 Redis 数据库
     *      @param connString 数据库描述，如：redis://server:port 或者 "server"
     *      @return 返回数据库连接对象
     *      
     */
    function openRedis(connString: string): Class_Redis;

    function openRedis(connString: string, callback: (err: Error | undefined | null, retVal: Class_Redis)=>any): void;

}

