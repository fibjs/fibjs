/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/// <reference path="../interface/MySQL.d.ts" />
/// <reference path="../interface/SQLite.d.ts" />
/// <reference path="../interface/LevelDB.d.ts" />
/// <reference path="../interface/Redis.d.ts" />
/**
 * The promise variant of the db module: async members return a Promise as their primary form.
 */
declare module 'db/promises' {
    /**
     * @description Opens an SQL database; dispatches to the engine selected by the protocol
     *      prefix of connString
     *
     *      Accepted prefixes: `sqlite:`, `mysql:`, `odbc:`, `mssql:`, `psql:` and `dm:`. The
     *      returned object is the engine-specific subclass of DbConnection (SQLite, MySQL or
     *      the ODBC-based mssql/psql/dm/odbc object); only the shared DbConnection API is
     *      guaranteed, use `type` to branch on the engine. `redis://` and `leveldb:` strings
     *      are rejected here: call openRedis / openLevelDB instead. A string without a known
     *      prefix fails with error number 20004; a failed connection reports the driver error
     *      with number 20024.
     *
     *      Example — select the engine with the protocol prefix:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.open('sqlite::memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute('INSERT INTO t VALUES (?)', 'x');
     *      console.log(conn.type);                            // SQLite
     *      console.log(conn.execute('SELECT * FROM t')[0].v); // x
     *      conn.close();
     *      ```
     *
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function open(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens an SQL database; dispatches to the engine selected by the protocol
     *      prefix of connString
     *
     *      Accepted prefixes: `sqlite:`, `mysql:`, `odbc:`, `mssql:`, `psql:` and `dm:`. The
     *      returned object is the engine-specific subclass of DbConnection (SQLite, MySQL or
     *      the ODBC-based mssql/psql/dm/odbc object); only the shared DbConnection API is
     *      guaranteed, use `type` to branch on the engine. `redis://` and `leveldb:` strings
     *      are rejected here: call openRedis / openLevelDB instead. A string without a known
     *      prefix fails with error number 20004; a failed connection reports the driver error
     *      with number 20024.
     *
     *      Example — select the engine with the protocol prefix:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.open('sqlite::memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute('INSERT INTO t VALUES (?)', 'x');
     *      console.log(conn.type);                            // SQLite
     *      console.log(conn.execute('SELECT * FROM t')[0].v); // x
     *      conn.close();
     *      ```
     *
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openSync(connString: string): Class_DbConnection;

    /**
     * @description Opens an SQL database; dispatches to the engine selected by the protocol
     *      prefix of connString
     *
     *      Accepted prefixes: `sqlite:`, `mysql:`, `odbc:`, `mssql:`, `psql:` and `dm:`. The
     *      returned object is the engine-specific subclass of DbConnection (SQLite, MySQL or
     *      the ODBC-based mssql/psql/dm/odbc object); only the shared DbConnection API is
     *      guaranteed, use `type` to branch on the engine. `redis://` and `leveldb:` strings
     *      are rejected here: call openRedis / openLevelDB instead. A string without a known
     *      prefix fails with error number 20004; a failed connection reports the driver error
     *      with number 20024.
     *
     *      Example — select the engine with the protocol prefix:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.open('sqlite::memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute('INSERT INTO t VALUES (?)', 'x');
     *      console.log(conn.type);                            // SQLite
     *      console.log(conn.execute('SELECT * FROM t')[0].v); // x
     *      conn.close();
     *      ```
     *
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openAsync(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens a mysql database
     *
     *      Only `mysql://user:password@host:port/database` strings are accepted; a different
     *      prefix fails with error number 20004. The user and password are URI-decoded, the
     *      database name is the URL path (may be empty) and the port defaults to 3306. The
     *      character set is fixed to utf8mb4 and query-string options are not read, so
     *      credentials must be part of the URL. A server that cannot be reached reports error
     *      number 20024 ("Failed to connect to server"). Use close() to end the session.
     *
     *      Example — connect and run a statement (needs a MySQL server):
     *      ```JavaScript
     *      // requires: mysql
     *      const db = require('db');
     *      const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
     *
     *      conn.execute('CREATE TABLE IF NOT EXISTS user ('
     *          + 'id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(64))');
     *      const inserted = conn.execute('INSERT INTO user (name) VALUES (?)', 'alice');
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      const rows = conn.execute('SELECT name FROM user WHERE id = ?', inserted.insertId);
     *      console.log(rows[0].name); // alice
     *
     *      conn.execute('DROP TABLE user');
     *      conn.close();
     *      ```
     *
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMySQL(connString: string): Promise<Class_MySQLPromise>;

    /**
     * @description Opens a mysql database
     *
     *      Only `mysql://user:password@host:port/database` strings are accepted; a different
     *      prefix fails with error number 20004. The user and password are URI-decoded, the
     *      database name is the URL path (may be empty) and the port defaults to 3306. The
     *      character set is fixed to utf8mb4 and query-string options are not read, so
     *      credentials must be part of the URL. A server that cannot be reached reports error
     *      number 20024 ("Failed to connect to server"). Use close() to end the session.
     *
     *      Example — connect and run a statement (needs a MySQL server):
     *      ```JavaScript
     *      // requires: mysql
     *      const db = require('db');
     *      const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
     *
     *      conn.execute('CREATE TABLE IF NOT EXISTS user ('
     *          + 'id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(64))');
     *      const inserted = conn.execute('INSERT INTO user (name) VALUES (?)', 'alice');
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      const rows = conn.execute('SELECT name FROM user WHERE id = ?', inserted.insertId);
     *      console.log(rows[0].name); // alice
     *
     *      conn.execute('DROP TABLE user');
     *      conn.close();
     *      ```
     *
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMySQLSync(connString: string): Class_MySQL;

    /**
     * @description Opens a mysql database
     *
     *      Only `mysql://user:password@host:port/database` strings are accepted; a different
     *      prefix fails with error number 20004. The user and password are URI-decoded, the
     *      database name is the URL path (may be empty) and the port defaults to 3306. The
     *      character set is fixed to utf8mb4 and query-string options are not read, so
     *      credentials must be part of the URL. A server that cannot be reached reports error
     *      number 20024 ("Failed to connect to server"). Use close() to end the session.
     *
     *      Example — connect and run a statement (needs a MySQL server):
     *      ```JavaScript
     *      // requires: mysql
     *      const db = require('db');
     *      const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
     *
     *      conn.execute('CREATE TABLE IF NOT EXISTS user ('
     *          + 'id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(64))');
     *      const inserted = conn.execute('INSERT INTO user (name) VALUES (?)', 'alice');
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      const rows = conn.execute('SELECT name FROM user WHERE id = ?', inserted.insertId);
     *      console.log(rows[0].name); // alice
     *
     *      conn.execute('DROP TABLE user');
     *      conn.close();
     *      ```
     *
     *      @param connString the database description, such as: mysql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMySQLAsync(connString: string): Promise<Class_MySQLPromise>;

    /**
     * @description Opens a sqlite database
     *
     *      Accepts `sqlite:path`, `sqlite://path` and a bare file path; the file is opened
     *      READWRITE|CREATE, so it is created when missing. `:memory:` opens a private
     *      in-memory database and an empty string opens a temporary on-disk database; both
     *      lose their data when the connection is closed. Every connection enables WAL
     *      journaling, `synchronous=normal` and `temp_store=memory`, and waits up to 5000 ms
     *      for a busy file (see SQLite.timeout). Failures such as a missing parent directory
     *      report error number 20024 with the SQLite message.
     *
     *      Example — create a database file in a temporary directory:
     *      ```JavaScript
     *      const db = require('db');
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-sqlite-'));
     *      const conn = db.openSQLite(path.join(dir, 'notes.db'));
     *
     *      conn.execute('CREATE TABLE note (body TEXT)');
     *      conn.execute('INSERT INTO note (body) VALUES (?)', 'hello');
     *      console.log(conn.fileName.endsWith('notes.db')); // true
     *
     *      conn.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param connString the database description, such as: sqlite:test.db or test.db
     *      @return returns the database connection object
     *
     */
    function openSQLite(connString: string): Promise<Class_SQLitePromise>;

    /**
     * @description Opens a sqlite database
     *
     *      Accepts `sqlite:path`, `sqlite://path` and a bare file path; the file is opened
     *      READWRITE|CREATE, so it is created when missing. `:memory:` opens a private
     *      in-memory database and an empty string opens a temporary on-disk database; both
     *      lose their data when the connection is closed. Every connection enables WAL
     *      journaling, `synchronous=normal` and `temp_store=memory`, and waits up to 5000 ms
     *      for a busy file (see SQLite.timeout). Failures such as a missing parent directory
     *      report error number 20024 with the SQLite message.
     *
     *      Example — create a database file in a temporary directory:
     *      ```JavaScript
     *      const db = require('db');
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-sqlite-'));
     *      const conn = db.openSQLite(path.join(dir, 'notes.db'));
     *
     *      conn.execute('CREATE TABLE note (body TEXT)');
     *      conn.execute('INSERT INTO note (body) VALUES (?)', 'hello');
     *      console.log(conn.fileName.endsWith('notes.db')); // true
     *
     *      conn.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param connString the database description, such as: sqlite:test.db or test.db
     *      @return returns the database connection object
     *
     */
    function openSQLiteSync(connString: string): Class_SQLite;

    /**
     * @description Opens a sqlite database
     *
     *      Accepts `sqlite:path`, `sqlite://path` and a bare file path; the file is opened
     *      READWRITE|CREATE, so it is created when missing. `:memory:` opens a private
     *      in-memory database and an empty string opens a temporary on-disk database; both
     *      lose their data when the connection is closed. Every connection enables WAL
     *      journaling, `synchronous=normal` and `temp_store=memory`, and waits up to 5000 ms
     *      for a busy file (see SQLite.timeout). Failures such as a missing parent directory
     *      report error number 20024 with the SQLite message.
     *
     *      Example — create a database file in a temporary directory:
     *      ```JavaScript
     *      const db = require('db');
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-sqlite-'));
     *      const conn = db.openSQLite(path.join(dir, 'notes.db'));
     *
     *      conn.execute('CREATE TABLE note (body TEXT)');
     *      conn.execute('INSERT INTO note (body) VALUES (?)', 'hello');
     *      console.log(conn.fileName.endsWith('notes.db')); // true
     *
     *      conn.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param connString the database description, such as: sqlite:test.db or test.db
     *      @return returns the database connection object
     *
     */
    function openSQLiteAsync(connString: string): Promise<Class_SQLitePromise>;

    /**
     * @description Opens a generic ODBC database
     *
     *      Only `odbc://` strings are accepted; the driver name is required and is read from
     *      the `Driver` (or `driver`) query-string parameter: `odbc://user:pass@host/db?driver=MyDriver`.
     *      Without it the call fails with error number 20024 ("odbc: no driver specified.").
     *      The URL is translated into an ODBC connection string with the Driver, Server,
     *      optional Port and Database, Uid and Pwd attributes; other ODBC attributes cannot be
     *      set from the URL. The returned object reports type `"odbc"`; install the driver
     *      through the system ODBC configuration (unixODBC on posix).
     *
     *      @param connString the database description, such as: odbc://user:pass@host/db?driver=PostgreSQL%20ANSI
     *      @return returns the database connection object
     *
     */
    function openOdbc(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens a generic ODBC database
     *
     *      Only `odbc://` strings are accepted; the driver name is required and is read from
     *      the `Driver` (or `driver`) query-string parameter: `odbc://user:pass@host/db?driver=MyDriver`.
     *      Without it the call fails with error number 20024 ("odbc: no driver specified.").
     *      The URL is translated into an ODBC connection string with the Driver, Server,
     *      optional Port and Database, Uid and Pwd attributes; other ODBC attributes cannot be
     *      set from the URL. The returned object reports type `"odbc"`; install the driver
     *      through the system ODBC configuration (unixODBC on posix).
     *
     *      @param connString the database description, such as: odbc://user:pass@host/db?driver=PostgreSQL%20ANSI
     *      @return returns the database connection object
     *
     */
    function openOdbcSync(connString: string): Class_DbConnection;

    /**
     * @description Opens a generic ODBC database
     *
     *      Only `odbc://` strings are accepted; the driver name is required and is read from
     *      the `Driver` (or `driver`) query-string parameter: `odbc://user:pass@host/db?driver=MyDriver`.
     *      Without it the call fails with error number 20024 ("odbc: no driver specified.").
     *      The URL is translated into an ODBC connection string with the Driver, Server,
     *      optional Port and Database, Uid and Pwd attributes; other ODBC attributes cannot be
     *      set from the URL. The returned object reports type `"odbc"`; install the driver
     *      through the system ODBC configuration (unixODBC on posix).
     *
     *      @param connString the database description, such as: odbc://user:pass@host/db?driver=PostgreSQL%20ANSI
     *      @return returns the database connection object
     *
     */
    function openOdbcAsync(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens an mssql database
     *
     *      Only `mssql://` strings are accepted; the port defaults to 1433 and the driver to
     *      `libtdsodbc.so` on posix (freetds must be installed) or "SQL Server" on Windows.
     *      Append `?driver=msodbcsql17` (or another name/path) to use a different driver. The
     *      returned object reports type `"mssql"`; note that mssql does not provide `insertId`
     *      in execution results. Connection failures report error number 20024 with the
     *      driver diagnostics.
     *
     *      @param connString the database description, such as: mssql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMSSQL(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens an mssql database
     *
     *      Only `mssql://` strings are accepted; the port defaults to 1433 and the driver to
     *      `libtdsodbc.so` on posix (freetds must be installed) or "SQL Server" on Windows.
     *      Append `?driver=msodbcsql17` (or another name/path) to use a different driver. The
     *      returned object reports type `"mssql"`; note that mssql does not provide `insertId`
     *      in execution results. Connection failures report error number 20024 with the
     *      driver diagnostics.
     *
     *      @param connString the database description, such as: mssql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMSSQLSync(connString: string): Class_DbConnection;

    /**
     * @description Opens an mssql database
     *
     *      Only `mssql://` strings are accepted; the port defaults to 1433 and the driver to
     *      `libtdsodbc.so` on posix (freetds must be installed) or "SQL Server" on Windows.
     *      Append `?driver=msodbcsql17` (or another name/path) to use a different driver. The
     *      returned object reports type `"mssql"`; note that mssql does not provide `insertId`
     *      in execution results. Connection failures report error number 20024 with the
     *      driver diagnostics.
     *
     *      @param connString the database description, such as: mssql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openMSSQLAsync(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens a DaMeng database
     *
     *      Only `dm://` strings are accepted; the port defaults to 5236 and the driver to
     *      `libdodbc.so` on posix or "DM8 ODBC DRIVER" on Windows. On Linux the ODBC driver
     *      file must be copied from the DaMeng installation into the system library path and
     *      configured in unixODBC. The connection is established with
     *      `Server=host:port` (falling back to separate Server/Port attributes) and
     *      TrustServerCertificate enabled. The returned object reports type `"dm"`.
     *
     *      @param connString the database description, such as: dm://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openDM(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens a DaMeng database
     *
     *      Only `dm://` strings are accepted; the port defaults to 5236 and the driver to
     *      `libdodbc.so` on posix or "DM8 ODBC DRIVER" on Windows. On Linux the ODBC driver
     *      file must be copied from the DaMeng installation into the system library path and
     *      configured in unixODBC. The connection is established with
     *      `Server=host:port` (falling back to separate Server/Port attributes) and
     *      TrustServerCertificate enabled. The returned object reports type `"dm"`.
     *
     *      @param connString the database description, such as: dm://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openDMSync(connString: string): Class_DbConnection;

    /**
     * @description Opens a DaMeng database
     *
     *      Only `dm://` strings are accepted; the port defaults to 5236 and the driver to
     *      `libdodbc.so` on posix or "DM8 ODBC DRIVER" on Windows. On Linux the ODBC driver
     *      file must be copied from the DaMeng installation into the system library path and
     *      configured in unixODBC. The connection is established with
     *      `Server=host:port` (falling back to separate Server/Port attributes) and
     *      TrustServerCertificate enabled. The returned object reports type `"dm"`.
     *
     *      @param connString the database description, such as: dm://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openDMAsync(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens a PostgresSQL database
     *
     *      Only `psql://` strings are accepted; the port defaults to 5432 and the driver to
     *      `psqlodbcw.so` (on macOS the Homebrew psqlodbc path is resolved automatically when
     *      no driver is given). Append `?Driver=<name>` to use another driver, for example a
     *      Unicode build. The ODBC driver of PostgreSQL must be installed: on ubuntu with
     *      `apt install unixodbc unixodbc-dev odbc-postgresql`, on macOS with
     *      `brew install unixodbc psqlodbc`; the returned object reports type `"psql"`.
     *
     *      @param connString the database description, such as: psql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openPSQL(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens a PostgresSQL database
     *
     *      Only `psql://` strings are accepted; the port defaults to 5432 and the driver to
     *      `psqlodbcw.so` (on macOS the Homebrew psqlodbc path is resolved automatically when
     *      no driver is given). Append `?Driver=<name>` to use another driver, for example a
     *      Unicode build. The ODBC driver of PostgreSQL must be installed: on ubuntu with
     *      `apt install unixodbc unixodbc-dev odbc-postgresql`, on macOS with
     *      `brew install unixodbc psqlodbc`; the returned object reports type `"psql"`.
     *
     *      @param connString the database description, such as: psql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openPSQLSync(connString: string): Class_DbConnection;

    /**
     * @description Opens a PostgresSQL database
     *
     *      Only `psql://` strings are accepted; the port defaults to 5432 and the driver to
     *      `psqlodbcw.so` (on macOS the Homebrew psqlodbc path is resolved automatically when
     *      no driver is given). Append `?Driver=<name>` to use another driver, for example a
     *      Unicode build. The ODBC driver of PostgreSQL must be installed: on ubuntu with
     *      `apt install unixodbc unixodbc-dev odbc-postgresql`, on macOS with
     *      `brew install unixodbc psqlodbc`; the returned object reports type `"psql"`.
     *
     *      @param connString the database description, such as: psql://user:pass@host/db
     *      @return returns the database connection object
     *
     */
    function openPSQLAsync(connString: string): Promise<Class_DbConnectionPromise>;

    /**
     * @description Opens a leveldb database
     *
     *      Accepts `leveldb:path` and a bare directory path; the directory is created when
     *      missing. The returned LevelDB object is not a DbConnection: it exposes the key-value
     *      API (get/set/remove/between/...) instead of SQL, and `open()` does not accept
     *      `leveldb:` strings. Only one process may open a directory at a time.
     *
     *      @param connString the database description, such as: level:test.db or test.db
     *      @return returns the database object
     *
     */
    function openLevelDB(connString: string): Promise<Class_LevelDBPromise>;

    /**
     * @description Opens a leveldb database
     *
     *      Accepts `leveldb:path` and a bare directory path; the directory is created when
     *      missing. The returned LevelDB object is not a DbConnection: it exposes the key-value
     *      API (get/set/remove/between/...) instead of SQL, and `open()` does not accept
     *      `leveldb:` strings. Only one process may open a directory at a time.
     *
     *      @param connString the database description, such as: level:test.db or test.db
     *      @return returns the database object
     *
     */
    function openLevelDBSync(connString: string): Class_LevelDB;

    /**
     * @description Opens a leveldb database
     *
     *      Accepts `leveldb:path` and a bare directory path; the directory is created when
     *      missing. The returned LevelDB object is not a DbConnection: it exposes the key-value
     *      API (get/set/remove/between/...) instead of SQL, and `open()` does not accept
     *      `leveldb:` strings. Only one process may open a directory at a time.
     *
     *      @param connString the database description, such as: level:test.db or test.db
     *      @return returns the database object
     *
     */
    function openLevelDBAsync(connString: string): Promise<Class_LevelDBPromise>;

    /**
     * @description Opens a Redis database
     *
     *      Accepts `redis://host:port` or a bare host name; the port defaults to 6379. The
     *      returned Redis client is not a DbConnection: it exposes the Redis commands
     *      (`command`, `get`/`set`, lists, sets, hashes, ...). A refused or unreachable server
     *      reports the socket error (for example error number 111, ECONNREFUSED) instead of a
     *      database error number.
     *
     *      @param connString the database description, such as: redis://server:port or "server"
     *      @return returns the database connection object
     *
     */
    function openRedis(connString: string): Promise<Class_Redis>;

    /**
     * @description Opens a Redis database
     *
     *      Accepts `redis://host:port` or a bare host name; the port defaults to 6379. The
     *      returned Redis client is not a DbConnection: it exposes the Redis commands
     *      (`command`, `get`/`set`, lists, sets, hashes, ...). A refused or unreachable server
     *      reports the socket error (for example error number 111, ECONNREFUSED) instead of a
     *      database error number.
     *
     *      @param connString the database description, such as: redis://server:port or "server"
     *      @return returns the database connection object
     *
     */
    function openRedisSync(connString: string): Class_Redis;

    /**
     * @description Opens a Redis database
     *
     *      Accepts `redis://host:port` or a bare host name; the port defaults to 6379. The
     *      returned Redis client is not a DbConnection: it exposes the Redis commands
     *      (`command`, `get`/`set`, lists, sets, hashes, ...). A refused or unreachable server
     *      reports the socket error (for example error number 111, ECONNREFUSED) instead of a
     *      database error number.
     *
     *      @param connString the database description, such as: redis://server:port or "server"
     *      @return returns the database connection object
     *
     */
    function openRedisAsync(connString: string): Promise<Class_Redis>;

}


declare module "db" {
    const promises: typeof import("db/promises");
}
