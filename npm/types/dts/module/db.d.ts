/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/MySQL.d.ts" />
/// <reference path="../interface/SQLite.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/// <reference path="../interface/LevelDB.d.ts" />
/// <reference path="../interface/Redis.d.ts" />
/**
 * @description Database access module
 *
 *  Basic module. Can be used to create and operate on database resources. To require it:
 *  ```JavaScript
 *  var db = require('db');
 *  var conn = db.open('rng://user:pass@host:port/dbname');
 *  ```
 *  Different database connections can be established by specifying a database engine. fibjs has two built-in sql engines: sqlite and mysql, and also supports connecting to more databases through ODBC/unixODBC; based on ODBC/unixODBC, fibjs provides drivers for mssql and PostgreSQL.
 *  To use ODBC/unixODBC, the corresponding drivers must be installed; on posix, using mssql requires installing freetds, and using PostgreSQL requires installing psqlodbc.
 *  Normally, once the drivers are installed successfully they can be used directly without further configuration.
 *
 */
declare module 'db' {
    /**
     * @description Opens a database; this method is the generic entry point and calls different engines depending on the given connString
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function open(connString: string): Class_object;

    function open(connString: string, callback: (err: Error | undefined | null, retVal: Class_object)=>any): void;

    /**
     * @description Opens a database; this method is the generic entry point and calls different engines depending on the given connString
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openSync(connString: string): Class_object;

    /**
     * @description Opens a database; this method is the generic entry point and calls different engines depending on the given connString
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openAsync(connString: string): Promise<Class_object>;

    /**
     * @description Opens a mysql database
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMySQL(connString: string): Class_MySQL;

    function openMySQL(connString: string, callback: (err: Error | undefined | null, retVal: Class_MySQL)=>any): void;

    /**
     * @description Opens a mysql database
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMySQLSync(connString: string): Class_MySQL;

    /**
     * @description Opens a mysql database
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMySQLAsync(connString: string): Promise<Class_MySQL>;

    /**
     * @description Opens a sqlite database
     *      @param connString the database description, such as: sqlite:test.db or test.db
     *      @return returns the database connection object
     *
     */
    function openSQLite(connString: string): Class_SQLite;

    function openSQLite(connString: string, callback: (err: Error | undefined | null, retVal: Class_SQLite)=>any): void;

    /**
     * @description Opens a sqlite database
     *      @param connString the database description, such as: sqlite:test.db or test.db
     *      @return returns the database connection object
     *
     */
    function openSQLiteSync(connString: string): Class_SQLite;

    /**
     * @description Opens a sqlite database
     *      @param connString the database description, such as: sqlite:test.db or test.db
     *      @return returns the database connection object
     *
     */
    function openSQLiteAsync(connString: string): Promise<Class_SQLite>;

    /**
     * @description Opens a sqlite database
     *      @param connString the database description, such as: odbc://user:pass@host/db?driver=PostgreSQL%20ANSI
     *      @return returns the database connection object
     *
     */
    function openOdbc(connString: string): Class_DbConnection;

    function openOdbc(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description Opens a sqlite database
     *      @param connString the database description, such as: odbc://user:pass@host/db?driver=PostgreSQL%20ANSI
     *      @return returns the database connection object
     *
     */
    function openOdbcSync(connString: string): Class_DbConnection;

    /**
     * @description Opens a sqlite database
     *      @param connString the database description, such as: odbc://user:pass@host/db?driver=PostgreSQL%20ANSI
     *      @return returns the database connection object
     *
     */
    function openOdbcAsync(connString: string): Promise<Class_DbConnection>;

    /**
     * @description Opens an mssql database
     *
     *      To connect to mssql, the freetds odbc driver must be installed on posix; the Microsoft mssql driver can also be used by specifying it; to specify a driver, append the ?driver=msodbcsql17[.so/.dylib] option to the url.
     *      @param connString the database description, such as: mssql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMSSQL(connString: string): Class_DbConnection;

    function openMSSQL(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description Opens an mssql database
     *
     *      To connect to mssql, the freetds odbc driver must be installed on posix; the Microsoft mssql driver can also be used by specifying it; to specify a driver, append the ?driver=msodbcsql17[.so/.dylib] option to the url.
     *      @param connString the database description, such as: mssql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMSSQLSync(connString: string): Class_DbConnection;

    /**
     * @description Opens an mssql database
     *
     *      To connect to mssql, the freetds odbc driver must be installed on posix; the Microsoft mssql driver can also be used by specifying it; to specify a driver, append the ?driver=msodbcsql17[.so/.dylib] option to the url.
     *      @param connString the database description, such as: mssql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMSSQLAsync(connString: string): Promise<Class_DbConnection>;

    /**
     * @description Opens a DaMeng database
     *
     *      To connect to a DaMeng database, the odbc driver of DaMeng must be installed.
     *      On Linux, obtain the ODBC driver file from the DaMeng installation directory, copy it into the system library path and configure unixODBC.
     *      @param connString the database description, such as: dm://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openDM(connString: string): Class_DbConnection;

    function openDM(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description Opens a DaMeng database
     *
     *      To connect to a DaMeng database, the odbc driver of DaMeng must be installed.
     *      On Linux, obtain the ODBC driver file from the DaMeng installation directory, copy it into the system library path and configure unixODBC.
     *      @param connString the database description, such as: dm://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openDMSync(connString: string): Class_DbConnection;

    /**
     * @description Opens a DaMeng database
     *
     *      To connect to a DaMeng database, the odbc driver of DaMeng must be installed.
     *      On Linux, obtain the ODBC driver file from the DaMeng installation directory, copy it into the system library path and configure unixODBC.
     *      @param connString the database description, such as: dm://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openDMAsync(connString: string): Promise<Class_DbConnection>;

    /**
     * @description Opens a PostgresSQL database
     *
     *      To connect to PostgresSQL, the odbc driver of PostgresSQL must be installed.
     *      On ubuntu, install the PostgresSQL odbc driver with the following command:
     *      ```bash
     *      apt install unixodbc unixodbc-dev odbc-postgresql
     *      ```
     *      On mac, install the PostgresSQL odbc driver with the following command:
     *      ```bash
     *      brew install unixodbc psqlodbc
     *      ```
     *      You also need to add the path of the brew-installed odbc driver to the environment variables; it is usually /usr/local/lib or /opt/homebrew/lib. You can use find to locate the path of libodbc.dylib:
     *     ```bash
     *     find /usr/local/ -name libodbc.dylib
     *     find /opt/homebrew/ -name libodbc.dylib
     *     ```
     *      Edit ~/.zshrc and add the following content:
     *      ```bash
     *      export DYLD_LIBRARY_PATH=/opt/homebrew/lib:$DYLD_LIBRARY_PATH
     *      ```
     *
     *      @param connString the database description, such as: psql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openPSQL(connString: string): Class_DbConnection;

    function openPSQL(connString: string, callback: (err: Error | undefined | null, retVal: Class_DbConnection)=>any): void;

    /**
     * @description Opens a PostgresSQL database
     *
     *      To connect to PostgresSQL, the odbc driver of PostgresSQL must be installed.
     *      On ubuntu, install the PostgresSQL odbc driver with the following command:
     *      ```bash
     *      apt install unixodbc unixodbc-dev odbc-postgresql
     *      ```
     *      On mac, install the PostgresSQL odbc driver with the following command:
     *      ```bash
     *      brew install unixodbc psqlodbc
     *      ```
     *      You also need to add the path of the brew-installed odbc driver to the environment variables; it is usually /usr/local/lib or /opt/homebrew/lib. You can use find to locate the path of libodbc.dylib:
     *     ```bash
     *     find /usr/local/ -name libodbc.dylib
     *     find /opt/homebrew/ -name libodbc.dylib
     *     ```
     *      Edit ~/.zshrc and add the following content:
     *      ```bash
     *      export DYLD_LIBRARY_PATH=/opt/homebrew/lib:$DYLD_LIBRARY_PATH
     *      ```
     *
     *      @param connString the database description, such as: psql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openPSQLSync(connString: string): Class_DbConnection;

    /**
     * @description Opens a PostgresSQL database
     *
     *      To connect to PostgresSQL, the odbc driver of PostgresSQL must be installed.
     *      On ubuntu, install the PostgresSQL odbc driver with the following command:
     *      ```bash
     *      apt install unixodbc unixodbc-dev odbc-postgresql
     *      ```
     *      On mac, install the PostgresSQL odbc driver with the following command:
     *      ```bash
     *      brew install unixodbc psqlodbc
     *      ```
     *      You also need to add the path of the brew-installed odbc driver to the environment variables; it is usually /usr/local/lib or /opt/homebrew/lib. You can use find to locate the path of libodbc.dylib:
     *     ```bash
     *     find /usr/local/ -name libodbc.dylib
     *     find /opt/homebrew/ -name libodbc.dylib
     *     ```
     *      Edit ~/.zshrc and add the following content:
     *      ```bash
     *      export DYLD_LIBRARY_PATH=/opt/homebrew/lib:$DYLD_LIBRARY_PATH
     *      ```
     *
     *      @param connString the database description, such as: psql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openPSQLAsync(connString: string): Promise<Class_DbConnection>;

    /**
     * @description Opens a leveldb database
     *      @param connString the database description, such as: level:test.db or test.db
     *      @return returns the database object
     *
     */
    function openLevelDB(connString: string): Class_LevelDB;

    function openLevelDB(connString: string, callback: (err: Error | undefined | null, retVal: Class_LevelDB)=>any): void;

    /**
     * @description Opens a leveldb database
     *      @param connString the database description, such as: level:test.db or test.db
     *      @return returns the database object
     *
     */
    function openLevelDBSync(connString: string): Class_LevelDB;

    /**
     * @description Opens a leveldb database
     *      @param connString the database description, such as: level:test.db or test.db
     *      @return returns the database object
     *
     */
    function openLevelDBAsync(connString: string): Promise<Class_LevelDB>;

    /**
     * @description Opens a Redis database
     *      @param connString the database description, such as: redis://server:port or "server"
     *      @return returns the database connection object
     *
     */
    function openRedis(connString: string): Class_Redis;

    function openRedis(connString: string, callback: (err: Error | undefined | null, retVal: Class_Redis)=>any): void;

    /**
     * @description Opens a Redis database
     *      @param connString the database description, such as: redis://server:port or "server"
     *      @return returns the database connection object
     *
     */
    function openRedisSync(connString: string): Class_Redis;

    /**
     * @description Opens a Redis database
     *      @param connString the database description, such as: redis://server:port or "server"
     *      @return returns the database connection object
     *
     */
    function openRedisAsync(connString: string): Promise<Class_Redis>;

}

