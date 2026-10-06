/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Statement.d.ts" />
/**
 * @description DbConnection is the base class of SQL database connections: it owns one session
 *  with the server (or the database file) and exposes querying, prepared statements,
 *  transactions and schema inspection
 *
 *  DbConnection is never constructed directly: `db.open` returns the subclass chosen by the
 *  protocol prefix of the connection string (SQLite, MySQL, mssql, psql, dm or the generic
 *  odbc object) and the per-engine `db.openXxx` methods return the same objects. Engine
 *  independent code can be written against this class, using `type` to branch where the
 *  engines differ.
 *
 *  Concepts:
 *
 *  - **Call forms**: `execute`, `format`, `prepare`, `iterate`, `getTables`, `getTableInfo`,
 *    `use` and the transaction methods are fiber-synchronous; their `...Async` aliases
 *    return a Promise and `db.promises` connections expose the same API as promises.
 *  - **Parameters**: SQL text uses `?` placeholders. `execute(sql, ...args)` and `format`
 *    interpolate the escaped arguments into the string, while `prepare` returns a Statement
 *    that binds them when it runs (named `:name` and `?NNN` placeholders are bound by
 *    position too). Escaping: strings are quoted with doubled quotes, buffers become binary
 *    literals, Date becomes a SQL timestamp string and null/undefined becomes NULL.
 *  - **Results**: queries return an array of row objects keyed by column name;
 *    INSERT/UPDATE/DELETE results carry `affected` and `insertId` (mssql has no
 *    `insertId`); a string with several statements returns an array of result sets.
 *  - **Transactions**: without an argument, `begin`/`commit`/`rollback` drive the
 *    connection transaction. With a `point` name they use savepoints: begin creates
 *    `SAVEPOINT point`, commit runs `RELEASE SAVEPOINT point` and rollback runs
 *    `ROLLBACK TO point` (which keeps the savepoint on the stack, so release it with
 *    commit(point) or end the transaction with a plain rollback). `trans` wraps the three
 *    calls around a function; nested `trans` calls need a savepoint name.
 *  - **One cursor per connection**: an open Statement cursor rejects `execute`, `prepare`,
 *    `iterate` and transaction control on the same connection with error number 20028 until
 *    the iterator is exhausted, `return()` is called or the for...of loop ends.
 *  - **Dialect differences**: SQLite rejects `use` and supports `columns()` on prepared
 *    statements, MySQL and ODBC do not; mssql omits `insertId`. See the SQLite and MySQL
 *    classes.
 *
 *  Obtained from:
 *  - `db.open(connString)` — the engine is selected by the protocol prefix;
 *  - `db.openSQLite(...)`, `db.openMySQL(...)`, `db.openPSQL(...)`, `db.openMSSQL(...)`,
 *    `db.openDM(...)` and `db.openOdbc(...)` — one engine each;
 *  - `db.promises` or the `...Async` aliases — promise-based forms of the same calls.
 *
 *  Example 1 — a round trip through the shared connection API:
 *  ```JavaScript
 *  const db = require('db');
 *  const conn = db.openSQLite(':memory:');
 *
 *  conn.execute('CREATE TABLE account (id INTEGER PRIMARY KEY, name TEXT, balance REAL)');
 *  const inserted = conn.execute('INSERT INTO account (name, balance) VALUES (?, ?)', 'alice', 100);
 *  console.log(inserted.affected, inserted.insertId); // 1 1
 *
 *  const rows = conn.execute('SELECT name, balance FROM account');
 *  console.log(rows.length, rows[0].name); // 1 alice
 *
 *  conn.close();
 *  ```
 *
 *  Example 2 — a transaction that rolls back when the function throws:
 *  ```JavaScript
 *  const db = require('db');
 *  const conn = db.openSQLite(':memory:');
 *  conn.execute('CREATE TABLE account (name TEXT, balance REAL)');
 *  conn.execute('INSERT INTO account (name, balance) VALUES (?, ?)', 'alice', 100);
 *
 *  function transfer(from, to, amount) {
 *      return conn.trans((c) => {
 *          c.execute('UPDATE account SET balance = balance - ? WHERE name = ?', amount, from);
 *          c.execute('UPDATE account SET balance = balance + ? WHERE name = ?', amount, to);
 *          if (amount > c.execute('SELECT balance FROM account WHERE name = ?', from)[0].balance)
 *              throw new Error('insufficient funds'); // the throw rolls the transaction back
 *          return true;
 *      });
 *  }
 *
 *  try {
 *      transfer('alice', 'bob', 500);
 *  } catch (err) {
 *      console.log(err.message); // insufficient funds
 *  }
 *  console.log(conn.execute('SELECT balance FROM account WHERE name = ?', 'alice')[0].balance); // 100
 *
 *  conn.close();
 *  ```
 *
 */
declare class Class_DbConnection extends Class_object {
    /**
     * @description Queries the type of the current database connection
     *
     *      Returns the engine name as a string: `"SQLite"`, `"mysql"`, `"mssql"`, `"psql"`,
     *      `"dm"` or `"odbc"`. The value is fixed when the connection is created and stays
     *      readable after close, so it is the supported way to branch on engine-specific SQL.
     *
     */
    readonly type: string;

    /**
     * @description Closes the current database connection
     *
     *      Releases the server session or the database file handle. SQLite also closes every
     *      prepared statement and active cursor of the connection; after close every operation
     *      fails with error number 20009 and an open transaction is rolled back. MySQL close is
     *      idempotent, while SQLite close on an already closed connection reports 20009.
     *
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the current database connection
     *
     *      Releases the server session or the database file handle. SQLite also closes every
     *      prepared statement and active cursor of the connection; after close every operation
     *      fails with error number 20009 and an open transaction is rolled back. MySQL close is
     *      idempotent, while SQLite close on an already closed connection reports 20009.
     *
     */
    closeSync(): void;

    /**
     * @description Closes the current database connection
     *
     *      Releases the server session or the database file handle. SQLite also closes every
     *      prepared statement and active cursor of the connection; after close every operation
     *      fails with error number 20009 and an open transaction is rolled back. MySQL close is
     *      idempotent, while SQLite close on an already closed connection reports 20009.
     *
     */
    closeAsync(): Promise<void>;

    /**
     * @description Selects the default database of the current database connection
     *
     *      Sends `USE <dbName>` to the server, with the name escaped as a string literal. It is
     *      useful on MySQL to switch the current schema; SQLite has one database per file and
     *      rejects the statement with error number 20024 (the SQLite message mentions a USE
     *      syntax error).
     *
     *      @param dbName the database name
     *
     */
    use(dbName: string): void;

    use(dbName: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Selects the default database of the current database connection
     *
     *      Sends `USE <dbName>` to the server, with the name escaped as a string literal. It is
     *      useful on MySQL to switch the current schema; SQLite has one database per file and
     *      rejects the statement with error number 20024 (the SQLite message mentions a USE
     *      syntax error).
     *
     *      @param dbName the database name
     *
     */
    useSync(dbName: string): void;

    /**
     * @description Selects the default database of the current database connection
     *
     *      Sends `USE <dbName>` to the server, with the name escaped as a string literal. It is
     *      useful on MySQL to switch the current schema; SQLite has one database per file and
     *      rejects the statement with error number 20024 (the SQLite message mentions a USE
     *      syntax error).
     *
     *      @param dbName the database name
     *
     */
    useAsync(dbName: string): Promise<void>;

    /**
     * @description Gets information about all tables in the current database
     *
     *      Returns one object per table, currently with a single `name` property, ordered by
     *      name. SQLite reads `sqlite_master` and skips the internal `sqlite_%` tables; MySQL
     *      reads `information_schema.tables` of the current database. Use getTableInfo for the
     *      columns of one table.
     *
     *      Example — list the tables and inspect one of them:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY)');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.getTables().map((t) => t.name).join(',')); // log,user
     *      console.log(conn.getTableInfo('user')[0].column_name);      // id
     *
     *      conn.close();
     *      ```
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTables(): any[];

    getTables(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Gets information about all tables in the current database
     *
     *      Returns one object per table, currently with a single `name` property, ordered by
     *      name. SQLite reads `sqlite_master` and skips the internal `sqlite_%` tables; MySQL
     *      reads `information_schema.tables` of the current database. Use getTableInfo for the
     *      columns of one table.
     *
     *      Example — list the tables and inspect one of them:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY)');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.getTables().map((t) => t.name).join(',')); // log,user
     *      console.log(conn.getTableInfo('user')[0].column_name);      // id
     *
     *      conn.close();
     *      ```
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesSync(): any[];

    /**
     * @description Gets information about all tables in the current database
     *
     *      Returns one object per table, currently with a single `name` property, ordered by
     *      name. SQLite reads `sqlite_master` and skips the internal `sqlite_%` tables; MySQL
     *      reads `information_schema.tables` of the current database. Use getTableInfo for the
     *      columns of one table.
     *
     *      Example — list the tables and inspect one of them:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY)');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.getTables().map((t) => t.name).join(',')); // log,user
     *      console.log(conn.getTableInfo('user')[0].column_name);      // id
     *
     *      conn.close();
     *      ```
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesAsync(): Promise<any[]>;

    /**
     * @description Gets detailed information about the given table
     *
     *      Each item is an object with the fixed keys `column_name`, `data_type`,
     *      `character_maximum_length` (null when the engine does not report it), `is_nullable`
     *      ('YES' or 'NO') and `column_default`, ordered by column position. A table that does
     *      not exist yields an empty array. SQLite is implemented through `pragma_table_info`
     *      and MySQL through `information_schema.columns`, so both return the same keys.
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfo(tableName: string): any[];

    getTableInfo(tableName: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Gets detailed information about the given table
     *
     *      Each item is an object with the fixed keys `column_name`, `data_type`,
     *      `character_maximum_length` (null when the engine does not report it), `is_nullable`
     *      ('YES' or 'NO') and `column_default`, ordered by column position. A table that does
     *      not exist yields an empty array. SQLite is implemented through `pragma_table_info`
     *      and MySQL through `information_schema.columns`, so both return the same keys.
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoSync(tableName: string): any[];

    /**
     * @description Gets detailed information about the given table
     *
     *      Each item is an object with the fixed keys `column_name`, `data_type`,
     *      `character_maximum_length` (null when the engine does not report it), `is_nullable`
     *      ('YES' or 'NO') and `column_default`, ordered by column position. A table that does
     *      not exist yields an empty array. SQLite is implemented through `pragma_table_info`
     *      and MySQL through `information_schema.columns`, so both return the same keys.
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoAsync(tableName: string): Promise<any[]>;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      Without an argument the connection transaction is started with `BEGIN` (SQLite uses
     *      `BEGIN IMMEDIATE` so that a later write takes the WAL write lock up front). With
     *      `point` a savepoint of that name is created through `SAVEPOINT point`, which is also
     *      how a nested trans call is expressed. The call fails with error number 20028 while a
     *      Statement cursor is open on the connection.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    begin(point?: string): void;

    begin(point?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      Without an argument the connection transaction is started with `BEGIN` (SQLite uses
     *      `BEGIN IMMEDIATE` so that a later write takes the WAL write lock up front). With
     *      `point` a savepoint of that name is created through `SAVEPOINT point`, which is also
     *      how a nested trans call is expressed. The call fails with error number 20028 while a
     *      Statement cursor is open on the connection.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginSync(point?: string): void;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      Without an argument the connection transaction is started with `BEGIN` (SQLite uses
     *      `BEGIN IMMEDIATE` so that a later write takes the WAL write lock up front). With
     *      `point` a savepoint of that name is created through `SAVEPOINT point`, which is also
     *      how a nested trans call is expressed. The call fails with error number 20028 while a
     *      Statement cursor is open on the connection.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginAsync(point?: string): Promise<void>;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      Without an argument the current transaction is committed with `COMMIT`; with `point`
     *      the named savepoint is released through `RELEASE SAVEPOINT point`. Committing
     *      without an active transaction reports error number 20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commit(point?: string): void;

    commit(point?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      Without an argument the current transaction is committed with `COMMIT`; with `point`
     *      the named savepoint is released through `RELEASE SAVEPOINT point`. Committing
     *      without an active transaction reports error number 20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitSync(point?: string): void;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      Without an argument the current transaction is committed with `COMMIT`; with `point`
     *      the named savepoint is released through `RELEASE SAVEPOINT point`. Committing
     *      without an active transaction reports error number 20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitAsync(point?: string): Promise<void>;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      Without an argument the current transaction is rolled back with `ROLLBACK`; with
     *      `point` the savepoint is rolled back with `ROLLBACK TO point`. Note that
     *      `ROLLBACK TO` keeps the savepoint on the stack and the surrounding transaction open:
     *      release it with commit(point) or finish the transaction with a plain
     *      commit/rollback. Rolling back without an active transaction reports error number
     *      20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollback(point?: string): void;

    rollback(point?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      Without an argument the current transaction is rolled back with `ROLLBACK`; with
     *      `point` the savepoint is rolled back with `ROLLBACK TO point`. Note that
     *      `ROLLBACK TO` keeps the savepoint on the stack and the surrounding transaction open:
     *      release it with commit(point) or finish the transaction with a plain
     *      commit/rollback. Rolling back without an active transaction reports error number
     *      20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackSync(point?: string): void;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      Without an argument the current transaction is rolled back with `ROLLBACK`; with
     *      `point` the savepoint is rolled back with `ROLLBACK TO point`. Note that
     *      `ROLLBACK TO` keeps the savepoint on the stack and the surrounding transaction open:
     *      release it with commit(point) or finish the transaction with a plain
     *      commit/rollback. Rolling back without an active transaction reports error number
     *      20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackAsync(point?: string): Promise<void>;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *
     *      The function is called with the connection as both its argument and its `this`, and
     *      its outcome decides the transaction:
     *      - a return value other than false, including no return at all, commits;
     *      - returning false rolls back and makes trans return false;
     *      - throwing rolls back and rethrows the error to the caller.
     *
     *      Inside an existing transaction call the two-argument form with a savepoint name: a
     *      nested trans without a name issues a second BEGIN and fails on engines that reject
     *      nested transactions.
     *
     *      Example — commit, explicit rollback and rollback on throw:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.trans((c) => {
     *          c.execute('INSERT INTO log (msg) VALUES (?)', 'kept');
     *          return true;
     *      })); // true
     *
     *      console.log(conn.trans((c) => {
     *          c.execute('INSERT INTO log (msg) VALUES (?)', 'discarded');
     *          return false;
     *      })); // false
     *
     *      try {
     *          conn.trans((c) => {
     *              c.execute('INSERT INTO log (msg) VALUES (?)', 'thrown');
     *              throw new Error('stop');
     *          });
     *      } catch (err) {
     *          console.log(err.message); // stop
     *      }
     *      console.log(conn.execute('SELECT * FROM log').length); // 1
     *
     *      conn.close();
     *      ```
     *
     *      @param func the function to execute in a transaction
     *      @return returns whether the transaction was committed: returns true on a normal commit, false on rollback, and throws if the transaction fails
     *
     */
    trans(func: (conn: Class_DbConnection | Class_DbConnectionPromise)=>any): boolean;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *
     *      Same outcome rules as the one-argument form, with `point` naming the savepoint that
     *      wraps the call: begin(point) creates `SAVEPOINT point`, commit(point) releases it and
     *      a false return or a throw rolls back with `ROLLBACK TO point`. Because the rollback
     *      does not release the savepoint, the surrounding transaction stays open; call it from
     *      inside an outer transaction (or trans() with no name) and let the outer call finish,
     *      otherwise the connection remains inside a transaction.
     *
     *      @param point the transaction name
     *      @param func the function to execute in a transaction
     *      @return returns whether the transaction was committed: returns true on a normal commit, false on rollback, and throws if the transaction fails
     *
     */
    trans(point: string, func: (conn: Class_DbConnection | Class_DbConnectionPromise)=>any): boolean;

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      The result of a SELECT is an array of row objects keyed by column name. An
     *      INSERT/UPDATE/DELETE result is an array-like whose `affected` and `insertId`
     *      properties carry the changed row count and the generated key (mssql does not provide
     *      `insertId`). A string containing several statements returns an array with one result
     *      set per statement. This one-argument form does not interpolate values; to pass
     *      parameters use execute(sql, ...args), prepare or format. Engine errors are reported
     *      with number 20024, calls on a closed connection with 20009 and a busy cursor with
     *      20028.
     *
     *      Example — run a query and inspect an update result:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const rows = conn.execute('SELECT * FROM user');
     *      console.log(rows.length); // 0
     *
     *      const inserted = conn.execute("INSERT INTO user (name) VALUES ('alice')");
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      conn.close();
     *      ```
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    execute(sql: string): any[];

    execute(sql: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      The result of a SELECT is an array of row objects keyed by column name. An
     *      INSERT/UPDATE/DELETE result is an array-like whose `affected` and `insertId`
     *      properties carry the changed row count and the generated key (mssql does not provide
     *      `insertId`). A string containing several statements returns an array with one result
     *      set per statement. This one-argument form does not interpolate values; to pass
     *      parameters use execute(sql, ...args), prepare or format. Engine errors are reported
     *      with number 20024, calls on a closed connection with 20009 and a busy cursor with
     *      20028.
     *
     *      Example — run a query and inspect an update result:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const rows = conn.execute('SELECT * FROM user');
     *      console.log(rows.length); // 0
     *
     *      const inserted = conn.execute("INSERT INTO user (name) VALUES ('alice')");
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      conn.close();
     *      ```
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string): any[];

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      The result of a SELECT is an array of row objects keyed by column name. An
     *      INSERT/UPDATE/DELETE result is an array-like whose `affected` and `insertId`
     *      properties carry the changed row count and the generated key (mssql does not provide
     *      `insertId`). A string containing several statements returns an array with one result
     *      set per statement. This one-argument form does not interpolate values; to pass
     *      parameters use execute(sql, ...args), prepare or format. Engine errors are reported
     *      with number 20024, calls on a closed connection with 20009 and a busy cursor with
     *      20028.
     *
     *      Example — run a query and inspect an update result:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const rows = conn.execute('SELECT * FROM user');
     *      console.log(rows.length); // 0
     *
     *      const inserted = conn.execute("INSERT INTO user (name) VALUES ('alice')");
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      conn.close();
     *      ```
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeAsync(sql: string): Promise<any[]>;

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
     *
     *      Each `?` in sql is replaced from left to right by the escaped form of the matching
     *      argument: strings are quoted with doubled quotes, buffers become binary literals
     *      (SQLite `x'hex'`, MySQL `0xhex`), numbers and BigInt are inserted as they are,
     *      booleans as true/false, Date as a SQL timestamp string and null/undefined as NULL;
     *      arrays expand to a parenthesized value list. Missing arguments leave the `?` in
     *      place (SQLite binds an unbound `?` as NULL) and extra arguments are ignored. The
     *      values are escaped client-side, so the statement text is still sent as a whole;
     *      prefer prepare() when the same statement runs repeatedly.
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    execute(sql: string, ...args: any[]): any[];

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
     *
     *      Each `?` in sql is replaced from left to right by the escaped form of the matching
     *      argument: strings are quoted with doubled quotes, buffers become binary literals
     *      (SQLite `x'hex'`, MySQL `0xhex`), numbers and BigInt are inserted as they are,
     *      booleans as true/false, Date as a SQL timestamp string and null/undefined as NULL;
     *      arrays expand to a parenthesized value list. Missing arguments leave the `?` in
     *      place (SQLite binds an unbound `?` as NULL) and extra arguments are ignored. The
     *      values are escaped client-side, so the statement text is still sent as a whole;
     *      prefer prepare() when the same statement runs repeatedly.
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string, ...args: any[]): any[];

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
     *
     *      Each `?` in sql is replaced from left to right by the escaped form of the matching
     *      argument: strings are quoted with doubled quotes, buffers become binary literals
     *      (SQLite `x'hex'`, MySQL `0xhex`), numbers and BigInt are inserted as they are,
     *      booleans as true/false, Date as a SQL timestamp string and null/undefined as NULL;
     *      arrays expand to a parenthesized value list. Missing arguments leave the `?` in
     *      place (SQLite binds an unbound `?` as NULL) and extra arguments are ignored. The
     *      values are escaped client-side, so the statement text is still sent as a whole;
     *      prefer prepare() when the same statement runs repeatedly.
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeAsync(sql: string, ...args: any[]): Promise<any[]>;

    /**
     * @description Formats an sql command and returns the formatted result
     *
     *      Returns the SQL with every `?` replaced by the escaped argument, using the same
     *      rules as execute(sql, ...args) (see there for the per-type escaping). The command is
     *      not executed, and the method does not touch the engine, so it keeps working after
     *      close(); it is the way to build SQL text for logging or for a later execute. Passing
     *      a function as an argument fails with error number 20004.
     *
     *      Example — see the escaped forms of the values:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      console.log(conn.format('SELECT ?, ?, ?', "it's", null, new Date(0)));
     *      // SELECT 'it''s', NULL, '1970-01-01 00:00:00'
     *      conn.close();
     *      ```
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns the formatted sql command
     *
     */
    format(sql: string, ...args: any[]): string;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      Only one statement is accepted: a multi-statement string fails with error number
     *      20004 and an empty string with 20024. SQLite compiles the statement immediately, so
     *      syntax errors and missing tables surface here; MySQL and ODBC defer compilation to
     *      the first execution. The call fails with 20028 while another cursor is open on the
     *      connection and with 20009 when the connection is closed. The returned Statement
     *      belongs to this connection and can be executed repeatedly in get/all/run/iterate
     *      mode.
     *
     *      Example — prepare once and execute with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *      conn.execute("INSERT INTO t VALUES ('b')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v);   // a
     *      console.log(stmt.get('z'));     // undefined
     *      console.log(stmt.all().length); // 2
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepare(sql: string): Class_Statement;

    prepare(sql: string, callback: (err: Error | undefined | null, retVal: Class_Statement)=>any): void;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      Only one statement is accepted: a multi-statement string fails with error number
     *      20004 and an empty string with 20024. SQLite compiles the statement immediately, so
     *      syntax errors and missing tables surface here; MySQL and ODBC defer compilation to
     *      the first execution. The call fails with 20028 while another cursor is open on the
     *      connection and with 20009 when the connection is closed. The returned Statement
     *      belongs to this connection and can be executed repeatedly in get/all/run/iterate
     *      mode.
     *
     *      Example — prepare once and execute with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *      conn.execute("INSERT INTO t VALUES ('b')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v);   // a
     *      console.log(stmt.get('z'));     // undefined
     *      console.log(stmt.all().length); // 2
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareSync(sql: string): Class_Statement;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      Only one statement is accepted: a multi-statement string fails with error number
     *      20004 and an empty string with 20024. SQLite compiles the statement immediately, so
     *      syntax errors and missing tables surface here; MySQL and ODBC defer compilation to
     *      the first execution. The call fails with 20028 while another cursor is open on the
     *      connection and with 20009 when the connection is closed. The returned Statement
     *      belongs to this connection and can be executed repeatedly in get/all/run/iterate
     *      mode.
     *
     *      Example — prepare once and execute with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *      conn.execute("INSERT INTO t VALUES ('b')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v);   // a
     *      console.log(stmt.get('z'));     // undefined
     *      console.log(stmt.all().length); // 2
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareAsync(sql: string): Promise<Class_StatementPromise>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, so the cursor is released automatically and the
     *      connection is immediately reusable. Calling next()/return() manually is dangerous:
     *      the iterator keeps the cursor open until the results are exhausted; if return() is
     *      forgotten on break or exception, the leaked cursor occupies the connection. While a
     *      cursor is open, execute, prepare, iterate and transaction control on the same
     *      connection fail with error number 20028.
     *
     *      Example — stream rows and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      for (const row of conn.iterate('SELECT v FROM t ORDER BY v')) {
     *          if (row.v === 2)
     *              break;            // breaking releases the cursor
     *          console.log(row.v);   // 0, then 1
     *      }
     *      console.log(conn.execute('SELECT COUNT(*) AS n FROM t')[0].n); // 5
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterate(sql: string, ...args: any[]): Iterator<any>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, so the cursor is released automatically and the
     *      connection is immediately reusable. Calling next()/return() manually is dangerous:
     *      the iterator keeps the cursor open until the results are exhausted; if return() is
     *      forgotten on break or exception, the leaked cursor occupies the connection. While a
     *      cursor is open, execute, prepare, iterate and transaction control on the same
     *      connection fail with error number 20028.
     *
     *      Example — stream rows and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      for (const row of conn.iterate('SELECT v FROM t ORDER BY v')) {
     *          if (row.v === 2)
     *              break;            // breaking releases the cursor
     *          console.log(row.v);   // 0, then 1
     *      }
     *      console.log(conn.execute('SELECT COUNT(*) AS n FROM t')[0].n); // 5
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateSync(sql: string, ...args: any[]): Iterator<any>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, so the cursor is released automatically and the
     *      connection is immediately reusable. Calling next()/return() manually is dangerous:
     *      the iterator keeps the cursor open until the results are exhausted; if return() is
     *      forgotten on break or exception, the leaked cursor occupies the connection. While a
     *      cursor is open, execute, prepare, iterate and transaction control on the same
     *      connection fail with error number 20028.
     *
     *      Example — stream rows and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      for (const row of conn.iterate('SELECT v FROM t ORDER BY v')) {
     *          if (row.v === 2)
     *              break;            // breaking releases the cursor
     *          console.log(row.v);   // 0, then 1
     *      }
     *      console.log(conn.execute('SELECT COUNT(*) AS n FROM t')[0].n); // 5
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateAsync(sql: string, ...args: any[]): Promise<Iterator<any>>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Statement.d.ts" />
/**
 * The promise variant of the DbConnection class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_DbConnectionPromise extends Class_object {
    /**
     * @description Queries the type of the current database connection
     *
     *      Returns the engine name as a string: `"SQLite"`, `"mysql"`, `"mssql"`, `"psql"`,
     *      `"dm"` or `"odbc"`. The value is fixed when the connection is created and stays
     *      readable after close, so it is the supported way to branch on engine-specific SQL.
     *
     */
    readonly type: string;

    /**
     * @description Closes the current database connection
     *
     *      Releases the server session or the database file handle. SQLite also closes every
     *      prepared statement and active cursor of the connection; after close every operation
     *      fails with error number 20009 and an open transaction is rolled back. MySQL close is
     *      idempotent, while SQLite close on an already closed connection reports 20009.
     *
     */
    close(): Promise<void>;

    /**
     * @description Closes the current database connection
     *
     *      Releases the server session or the database file handle. SQLite also closes every
     *      prepared statement and active cursor of the connection; after close every operation
     *      fails with error number 20009 and an open transaction is rolled back. MySQL close is
     *      idempotent, while SQLite close on an already closed connection reports 20009.
     *
     */
    closeSync(): void;

    /**
     * @description Closes the current database connection
     *
     *      Releases the server session or the database file handle. SQLite also closes every
     *      prepared statement and active cursor of the connection; after close every operation
     *      fails with error number 20009 and an open transaction is rolled back. MySQL close is
     *      idempotent, while SQLite close on an already closed connection reports 20009.
     *
     */
    closeAsync(): Promise<void>;

    /**
     * @description Selects the default database of the current database connection
     *
     *      Sends `USE <dbName>` to the server, with the name escaped as a string literal. It is
     *      useful on MySQL to switch the current schema; SQLite has one database per file and
     *      rejects the statement with error number 20024 (the SQLite message mentions a USE
     *      syntax error).
     *
     *      @param dbName the database name
     *
     */
    use(dbName: string): Promise<void>;

    /**
     * @description Selects the default database of the current database connection
     *
     *      Sends `USE <dbName>` to the server, with the name escaped as a string literal. It is
     *      useful on MySQL to switch the current schema; SQLite has one database per file and
     *      rejects the statement with error number 20024 (the SQLite message mentions a USE
     *      syntax error).
     *
     *      @param dbName the database name
     *
     */
    useSync(dbName: string): void;

    /**
     * @description Selects the default database of the current database connection
     *
     *      Sends `USE <dbName>` to the server, with the name escaped as a string literal. It is
     *      useful on MySQL to switch the current schema; SQLite has one database per file and
     *      rejects the statement with error number 20024 (the SQLite message mentions a USE
     *      syntax error).
     *
     *      @param dbName the database name
     *
     */
    useAsync(dbName: string): Promise<void>;

    /**
     * @description Gets information about all tables in the current database
     *
     *      Returns one object per table, currently with a single `name` property, ordered by
     *      name. SQLite reads `sqlite_master` and skips the internal `sqlite_%` tables; MySQL
     *      reads `information_schema.tables` of the current database. Use getTableInfo for the
     *      columns of one table.
     *
     *      Example — list the tables and inspect one of them:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY)');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.getTables().map((t) => t.name).join(',')); // log,user
     *      console.log(conn.getTableInfo('user')[0].column_name);      // id
     *
     *      conn.close();
     *      ```
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTables(): Promise<any[]>;

    /**
     * @description Gets information about all tables in the current database
     *
     *      Returns one object per table, currently with a single `name` property, ordered by
     *      name. SQLite reads `sqlite_master` and skips the internal `sqlite_%` tables; MySQL
     *      reads `information_schema.tables` of the current database. Use getTableInfo for the
     *      columns of one table.
     *
     *      Example — list the tables and inspect one of them:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY)');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.getTables().map((t) => t.name).join(',')); // log,user
     *      console.log(conn.getTableInfo('user')[0].column_name);      // id
     *
     *      conn.close();
     *      ```
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesSync(): any[];

    /**
     * @description Gets information about all tables in the current database
     *
     *      Returns one object per table, currently with a single `name` property, ordered by
     *      name. SQLite reads `sqlite_master` and skips the internal `sqlite_%` tables; MySQL
     *      reads `information_schema.tables` of the current database. Use getTableInfo for the
     *      columns of one table.
     *
     *      Example — list the tables and inspect one of them:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY)');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.getTables().map((t) => t.name).join(',')); // log,user
     *      console.log(conn.getTableInfo('user')[0].column_name);      // id
     *
     *      conn.close();
     *      ```
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesAsync(): Promise<any[]>;

    /**
     * @description Gets detailed information about the given table
     *
     *      Each item is an object with the fixed keys `column_name`, `data_type`,
     *      `character_maximum_length` (null when the engine does not report it), `is_nullable`
     *      ('YES' or 'NO') and `column_default`, ordered by column position. A table that does
     *      not exist yields an empty array. SQLite is implemented through `pragma_table_info`
     *      and MySQL through `information_schema.columns`, so both return the same keys.
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfo(tableName: string): Promise<any[]>;

    /**
     * @description Gets detailed information about the given table
     *
     *      Each item is an object with the fixed keys `column_name`, `data_type`,
     *      `character_maximum_length` (null when the engine does not report it), `is_nullable`
     *      ('YES' or 'NO') and `column_default`, ordered by column position. A table that does
     *      not exist yields an empty array. SQLite is implemented through `pragma_table_info`
     *      and MySQL through `information_schema.columns`, so both return the same keys.
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoSync(tableName: string): any[];

    /**
     * @description Gets detailed information about the given table
     *
     *      Each item is an object with the fixed keys `column_name`, `data_type`,
     *      `character_maximum_length` (null when the engine does not report it), `is_nullable`
     *      ('YES' or 'NO') and `column_default`, ordered by column position. A table that does
     *      not exist yields an empty array. SQLite is implemented through `pragma_table_info`
     *      and MySQL through `information_schema.columns`, so both return the same keys.
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoAsync(tableName: string): Promise<any[]>;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      Without an argument the connection transaction is started with `BEGIN` (SQLite uses
     *      `BEGIN IMMEDIATE` so that a later write takes the WAL write lock up front). With
     *      `point` a savepoint of that name is created through `SAVEPOINT point`, which is also
     *      how a nested trans call is expressed. The call fails with error number 20028 while a
     *      Statement cursor is open on the connection.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    begin(point?: string): Promise<void>;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      Without an argument the connection transaction is started with `BEGIN` (SQLite uses
     *      `BEGIN IMMEDIATE` so that a later write takes the WAL write lock up front). With
     *      `point` a savepoint of that name is created through `SAVEPOINT point`, which is also
     *      how a nested trans call is expressed. The call fails with error number 20028 while a
     *      Statement cursor is open on the connection.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginSync(point?: string): void;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      Without an argument the connection transaction is started with `BEGIN` (SQLite uses
     *      `BEGIN IMMEDIATE` so that a later write takes the WAL write lock up front). With
     *      `point` a savepoint of that name is created through `SAVEPOINT point`, which is also
     *      how a nested trans call is expressed. The call fails with error number 20028 while a
     *      Statement cursor is open on the connection.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginAsync(point?: string): Promise<void>;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      Without an argument the current transaction is committed with `COMMIT`; with `point`
     *      the named savepoint is released through `RELEASE SAVEPOINT point`. Committing
     *      without an active transaction reports error number 20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commit(point?: string): Promise<void>;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      Without an argument the current transaction is committed with `COMMIT`; with `point`
     *      the named savepoint is released through `RELEASE SAVEPOINT point`. Committing
     *      without an active transaction reports error number 20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitSync(point?: string): void;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      Without an argument the current transaction is committed with `COMMIT`; with `point`
     *      the named savepoint is released through `RELEASE SAVEPOINT point`. Committing
     *      without an active transaction reports error number 20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitAsync(point?: string): Promise<void>;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      Without an argument the current transaction is rolled back with `ROLLBACK`; with
     *      `point` the savepoint is rolled back with `ROLLBACK TO point`. Note that
     *      `ROLLBACK TO` keeps the savepoint on the stack and the surrounding transaction open:
     *      release it with commit(point) or finish the transaction with a plain
     *      commit/rollback. Rolling back without an active transaction reports error number
     *      20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollback(point?: string): Promise<void>;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      Without an argument the current transaction is rolled back with `ROLLBACK`; with
     *      `point` the savepoint is rolled back with `ROLLBACK TO point`. Note that
     *      `ROLLBACK TO` keeps the savepoint on the stack and the surrounding transaction open:
     *      release it with commit(point) or finish the transaction with a plain
     *      commit/rollback. Rolling back without an active transaction reports error number
     *      20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackSync(point?: string): void;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      Without an argument the current transaction is rolled back with `ROLLBACK`; with
     *      `point` the savepoint is rolled back with `ROLLBACK TO point`. Note that
     *      `ROLLBACK TO` keeps the savepoint on the stack and the surrounding transaction open:
     *      release it with commit(point) or finish the transaction with a plain
     *      commit/rollback. Rolling back without an active transaction reports error number
     *      20024.
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackAsync(point?: string): Promise<void>;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *
     *      The function is called with the connection as both its argument and its `this`, and
     *      its outcome decides the transaction:
     *      - a return value other than false, including no return at all, commits;
     *      - returning false rolls back and makes trans return false;
     *      - throwing rolls back and rethrows the error to the caller.
     *
     *      Inside an existing transaction call the two-argument form with a savepoint name: a
     *      nested trans without a name issues a second BEGIN and fails on engines that reject
     *      nested transactions.
     *
     *      Example — commit, explicit rollback and rollback on throw:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE log (msg TEXT)');
     *
     *      console.log(conn.trans((c) => {
     *          c.execute('INSERT INTO log (msg) VALUES (?)', 'kept');
     *          return true;
     *      })); // true
     *
     *      console.log(conn.trans((c) => {
     *          c.execute('INSERT INTO log (msg) VALUES (?)', 'discarded');
     *          return false;
     *      })); // false
     *
     *      try {
     *          conn.trans((c) => {
     *              c.execute('INSERT INTO log (msg) VALUES (?)', 'thrown');
     *              throw new Error('stop');
     *          });
     *      } catch (err) {
     *          console.log(err.message); // stop
     *      }
     *      console.log(conn.execute('SELECT * FROM log').length); // 1
     *
     *      conn.close();
     *      ```
     *
     *      @param func the function to execute in a transaction
     *      @return returns whether the transaction was committed: returns true on a normal commit, false on rollback, and throws if the transaction fails
     *
     */
    trans(func: (conn: Class_DbConnection | Class_DbConnectionPromise)=>any): boolean;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *
     *      Same outcome rules as the one-argument form, with `point` naming the savepoint that
     *      wraps the call: begin(point) creates `SAVEPOINT point`, commit(point) releases it and
     *      a false return or a throw rolls back with `ROLLBACK TO point`. Because the rollback
     *      does not release the savepoint, the surrounding transaction stays open; call it from
     *      inside an outer transaction (or trans() with no name) and let the outer call finish,
     *      otherwise the connection remains inside a transaction.
     *
     *      @param point the transaction name
     *      @param func the function to execute in a transaction
     *      @return returns whether the transaction was committed: returns true on a normal commit, false on rollback, and throws if the transaction fails
     *
     */
    trans(point: string, func: (conn: Class_DbConnection | Class_DbConnectionPromise)=>any): boolean;

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      The result of a SELECT is an array of row objects keyed by column name. An
     *      INSERT/UPDATE/DELETE result is an array-like whose `affected` and `insertId`
     *      properties carry the changed row count and the generated key (mssql does not provide
     *      `insertId`). A string containing several statements returns an array with one result
     *      set per statement. This one-argument form does not interpolate values; to pass
     *      parameters use execute(sql, ...args), prepare or format. Engine errors are reported
     *      with number 20024, calls on a closed connection with 20009 and a busy cursor with
     *      20028.
     *
     *      Example — run a query and inspect an update result:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const rows = conn.execute('SELECT * FROM user');
     *      console.log(rows.length); // 0
     *
     *      const inserted = conn.execute("INSERT INTO user (name) VALUES ('alice')");
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      conn.close();
     *      ```
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    execute(sql: string): Promise<any[]>;

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      The result of a SELECT is an array of row objects keyed by column name. An
     *      INSERT/UPDATE/DELETE result is an array-like whose `affected` and `insertId`
     *      properties carry the changed row count and the generated key (mssql does not provide
     *      `insertId`). A string containing several statements returns an array with one result
     *      set per statement. This one-argument form does not interpolate values; to pass
     *      parameters use execute(sql, ...args), prepare or format. Engine errors are reported
     *      with number 20024, calls on a closed connection with 20009 and a busy cursor with
     *      20028.
     *
     *      Example — run a query and inspect an update result:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const rows = conn.execute('SELECT * FROM user');
     *      console.log(rows.length); // 0
     *
     *      const inserted = conn.execute("INSERT INTO user (name) VALUES ('alice')");
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      conn.close();
     *      ```
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string): any[];

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      The result of a SELECT is an array of row objects keyed by column name. An
     *      INSERT/UPDATE/DELETE result is an array-like whose `affected` and `insertId`
     *      properties carry the changed row count and the generated key (mssql does not provide
     *      `insertId`). A string containing several statements returns an array with one result
     *      set per statement. This one-argument form does not interpolate values; to pass
     *      parameters use execute(sql, ...args), prepare or format. Engine errors are reported
     *      with number 20024, calls on a closed connection with 20009 and a busy cursor with
     *      20028.
     *
     *      Example — run a query and inspect an update result:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const rows = conn.execute('SELECT * FROM user');
     *      console.log(rows.length); // 0
     *
     *      const inserted = conn.execute("INSERT INTO user (name) VALUES ('alice')");
     *      console.log(inserted.affected, inserted.insertId); // 1 1
     *
     *      conn.close();
     *      ```
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeAsync(sql: string): Promise<any[]>;

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
     *
     *      Each `?` in sql is replaced from left to right by the escaped form of the matching
     *      argument: strings are quoted with doubled quotes, buffers become binary literals
     *      (SQLite `x'hex'`, MySQL `0xhex`), numbers and BigInt are inserted as they are,
     *      booleans as true/false, Date as a SQL timestamp string and null/undefined as NULL;
     *      arrays expand to a parenthesized value list. Missing arguments leave the `?` in
     *      place (SQLite binds an unbound `?` as NULL) and extra arguments are ignored. The
     *      values are escaped client-side, so the statement text is still sent as a whole;
     *      prefer prepare() when the same statement runs repeatedly.
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    execute(sql: string, ...args: any[]): Promise<any[]>;

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
     *
     *      Each `?` in sql is replaced from left to right by the escaped form of the matching
     *      argument: strings are quoted with doubled quotes, buffers become binary literals
     *      (SQLite `x'hex'`, MySQL `0xhex`), numbers and BigInt are inserted as they are,
     *      booleans as true/false, Date as a SQL timestamp string and null/undefined as NULL;
     *      arrays expand to a parenthesized value list. Missing arguments leave the `?` in
     *      place (SQLite binds an unbound `?` as NULL) and extra arguments are ignored. The
     *      values are escaped client-side, so the statement text is still sent as a whole;
     *      prefer prepare() when the same statement runs repeatedly.
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string, ...args: any[]): any[];

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
     *
     *      Each `?` in sql is replaced from left to right by the escaped form of the matching
     *      argument: strings are quoted with doubled quotes, buffers become binary literals
     *      (SQLite `x'hex'`, MySQL `0xhex`), numbers and BigInt are inserted as they are,
     *      booleans as true/false, Date as a SQL timestamp string and null/undefined as NULL;
     *      arrays expand to a parenthesized value list. Missing arguments leave the `?` in
     *      place (SQLite binds an unbound `?` as NULL) and extra arguments are ignored. The
     *      values are escaped client-side, so the statement text is still sent as a whole;
     *      prefer prepare() when the same statement runs repeatedly.
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeAsync(sql: string, ...args: any[]): Promise<any[]>;

    /**
     * @description Formats an sql command and returns the formatted result
     *
     *      Returns the SQL with every `?` replaced by the escaped argument, using the same
     *      rules as execute(sql, ...args) (see there for the per-type escaping). The command is
     *      not executed, and the method does not touch the engine, so it keeps working after
     *      close(); it is the way to build SQL text for logging or for a later execute. Passing
     *      a function as an argument fails with error number 20004.
     *
     *      Example — see the escaped forms of the values:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      console.log(conn.format('SELECT ?, ?, ?', "it's", null, new Date(0)));
     *      // SELECT 'it''s', NULL, '1970-01-01 00:00:00'
     *      conn.close();
     *      ```
     *
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns the formatted sql command
     *
     */
    format(sql: string, ...args: any[]): string;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      Only one statement is accepted: a multi-statement string fails with error number
     *      20004 and an empty string with 20024. SQLite compiles the statement immediately, so
     *      syntax errors and missing tables surface here; MySQL and ODBC defer compilation to
     *      the first execution. The call fails with 20028 while another cursor is open on the
     *      connection and with 20009 when the connection is closed. The returned Statement
     *      belongs to this connection and can be executed repeatedly in get/all/run/iterate
     *      mode.
     *
     *      Example — prepare once and execute with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *      conn.execute("INSERT INTO t VALUES ('b')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v);   // a
     *      console.log(stmt.get('z'));     // undefined
     *      console.log(stmt.all().length); // 2
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepare(sql: string): Promise<Class_StatementPromise>;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      Only one statement is accepted: a multi-statement string fails with error number
     *      20004 and an empty string with 20024. SQLite compiles the statement immediately, so
     *      syntax errors and missing tables surface here; MySQL and ODBC defer compilation to
     *      the first execution. The call fails with 20028 while another cursor is open on the
     *      connection and with 20009 when the connection is closed. The returned Statement
     *      belongs to this connection and can be executed repeatedly in get/all/run/iterate
     *      mode.
     *
     *      Example — prepare once and execute with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *      conn.execute("INSERT INTO t VALUES ('b')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v);   // a
     *      console.log(stmt.get('z'));     // undefined
     *      console.log(stmt.all().length); // 2
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareSync(sql: string): Class_Statement;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      Only one statement is accepted: a multi-statement string fails with error number
     *      20004 and an empty string with 20024. SQLite compiles the statement immediately, so
     *      syntax errors and missing tables surface here; MySQL and ODBC defer compilation to
     *      the first execution. The call fails with 20028 while another cursor is open on the
     *      connection and with 20009 when the connection is closed. The returned Statement
     *      belongs to this connection and can be executed repeatedly in get/all/run/iterate
     *      mode.
     *
     *      Example — prepare once and execute with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *      conn.execute("INSERT INTO t VALUES ('b')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v);   // a
     *      console.log(stmt.get('z'));     // undefined
     *      console.log(stmt.all().length); // 2
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareAsync(sql: string): Promise<Class_StatementPromise>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, so the cursor is released automatically and the
     *      connection is immediately reusable. Calling next()/return() manually is dangerous:
     *      the iterator keeps the cursor open until the results are exhausted; if return() is
     *      forgotten on break or exception, the leaked cursor occupies the connection. While a
     *      cursor is open, execute, prepare, iterate and transaction control on the same
     *      connection fail with error number 20028.
     *
     *      Example — stream rows and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      for (const row of conn.iterate('SELECT v FROM t ORDER BY v')) {
     *          if (row.v === 2)
     *              break;            // breaking releases the cursor
     *          console.log(row.v);   // 0, then 1
     *      }
     *      console.log(conn.execute('SELECT COUNT(*) AS n FROM t')[0].n); // 5
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterate(sql: string, ...args: any[]): Promise<Iterator<any>>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, so the cursor is released automatically and the
     *      connection is immediately reusable. Calling next()/return() manually is dangerous:
     *      the iterator keeps the cursor open until the results are exhausted; if return() is
     *      forgotten on break or exception, the leaked cursor occupies the connection. While a
     *      cursor is open, execute, prepare, iterate and transaction control on the same
     *      connection fail with error number 20028.
     *
     *      Example — stream rows and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      for (const row of conn.iterate('SELECT v FROM t ORDER BY v')) {
     *          if (row.v === 2)
     *              break;            // breaking releases the cursor
     *          console.log(row.v);   // 0, then 1
     *      }
     *      console.log(conn.execute('SELECT COUNT(*) AS n FROM t')[0].n); // 5
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateSync(sql: string, ...args: any[]): Iterator<any>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, so the cursor is released automatically and the
     *      connection is immediately reusable. Calling next()/return() manually is dangerous:
     *      the iterator keeps the cursor open until the results are exhausted; if return() is
     *      forgotten on break or exception, the leaked cursor occupies the connection. While a
     *      cursor is open, execute, prepare, iterate and transaction control on the same
     *      connection fail with error number 20028.
     *
     *      Example — stream rows and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      for (const row of conn.iterate('SELECT v FROM t ORDER BY v')) {
     *          if (row.v === 2)
     *              break;            // breaking releases the cursor
     *          console.log(row.v);   // 0, then 1
     *      }
     *      console.log(conn.execute('SELECT COUNT(*) AS n FROM t')[0].n); // 5
     *      conn.close();
     *      ```
     *
     *      @param sql the query statement to prepare
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateAsync(sql: string, ...args: any[]): Promise<Iterator<any>>;

}


declare namespace Class_DbConnection {
    const promises: FIBJS.GeneralObject;
}
