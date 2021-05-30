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
 *  var conn = db.open('rng://user:pass@host:port/dbname');
 *  ```
 *  通过指定数据库引擎，可以建立不同的数据库链接。fibjs 内置两个 sql 引擎：sqlite 和 mysql，同时还支持通过 ODBC/unixODBC 连接更多数据库，基于 ODBC/unixODBC，fibjs 构建了与 mssql 和 PostgreSQL 的驱动。
 *  为了使用 ODBC/unixODBC，需要安装对应的驱动，在 posix 下使用 mssql 需要安装 freetds，使用 PostgreSQL 需要安装 psqlodbc。
 *  正常情况下驱动安装成功即可直接使用，无需进一步配置。
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
     * 
     *      为了建立与 mssql 的连接，在 posix 下必须安装 freetds 的 odbc 驱动，也可以通过指定驱动来使用微软的 mssql 驱动，指定驱动的方式，是在 url 后增加 ?driver=msodbcsql17[.so/.dylib] 的选项。
     *      @param connString 数据库描述，如：mssql://user:pass@host/db
     *      @return 返回数据库连接对象
     *      
     */
    function openMSSQL(connString: string): Class_DbConnection;

    function openMSSQL(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description 打开一个 PostgresSQL 数据库
     * 
     *      为了建立与 PostgresSQL 的连接，必须安装 PostgresSQL 的 odbc 驱动。
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

