/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Statement is a prepared statement created by DbConnection.prepare(): it can be
 *  executed repeatedly and can stream a result set row by row
 *
 *  The same Statement supports four execution modes:
 *  - `get(...args)` returns the first row, or undefined when there is no result;
 *  - `all(...args)` materializes the whole result set into an array;
 *  - `run(...args)` discards rows and returns the `{ changes, lastInsertRowid }` counters;
 *  - `iterate(...args)` returns an iterator that keeps only one row in memory.
 *
 *  Preparing lifts the per-execution parsing cost, and binding values instead of quoting
 *  them into the SQL text keeps parameters correct and safe. A Statement belongs to the
 *  connection that created it and stays usable until close() or until the connection is
 *  closed.
 *
 *  Concepts:
 *
 *  - **Lifecycle**: `prepare` compiles or stages the SQL; each get/all/run opens a cursor,
 *    consumes the result and releases it before returning, so the statement is immediately
 *    reusable. `iterate` keeps the cursor open until the iterator ends or return() is
 *    called.
 *  - **One cursor per connection**: while a Statement iterator is open, the owning
 *    connection rejects execute, prepare, iterate and transaction control with error number
 *    20028. Prefer `for (const row of stmt.iterate(...))`: the engine calls the iterator's
 *    return() when the loop ends, breaks or throws, which releases the cursor. Calling
 *    next() manually means calling return() yourself on every early exit.
 *  - **Parameters**: `?` placeholders are bound from left to right; named `:name` and
 *    `?NNN` placeholders are bound by position as well, not by name. Missing arguments bind
 *    NULL, extra arguments are ignored. Buffers bind as BLOBs, Date as a SQL timestamp
 *    string and null as NULL.
 *  - **Lifecycle errors**: using a statement after its close(), or after the connection was
 *    closed, fails with error number 20027.
 *
 *  Obtained from:
 *  - `conn.prepare(sql)` — compiles one statement on the connection and returns it;
 *  - `conn.iterate(sql, ...args)` — convenience that prepares and immediately opens the
 *    row iterator.
 *
 *  Example 1 — prepare once and execute in get/all/run modes:
 *  ```JavaScript
 *  const db = require('db');
 *  const conn = db.openSQLite(':memory:');
 *  conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
 *
 *  const insert = conn.prepare('INSERT INTO user (name) VALUES (?)');
 *  console.log(Number(insert.run('alice').lastInsertRowid)); // 1
 *  insert.run('bob');
 *
 *  const select = conn.prepare('SELECT name FROM user WHERE id = ?');
 *  console.log(select.get(1).name);  // alice
 *  console.log(select.all().length); // 2
 *
 *  select.close();
 *  insert.close();
 *  conn.close();
 *  ```
 *
 *  Example 2 — stream a result set with for...of:
 *  ```JavaScript
 *  const db = require('db');
 *  const conn = db.openSQLite(':memory:');
 *  conn.execute('CREATE TABLE log (v INTEGER)');
 *  for (let i = 0; i < 10; i++)
 *      conn.execute('INSERT INTO log VALUES (?)', i);
 *
 *  let sum = 0;
 *  for (const row of conn.prepare('SELECT v FROM log ORDER BY v').iterate()) {
 *      sum += row.v; // only one row is held in memory at a time
 *  }
 *  console.log(sum); // 45
 *
 *  conn.close();
 *  ```
 *
 */
declare class Class_Statement extends Class_object {
    /**
     * @description Executes the statement and returns the first row, or undefined if there is no result
     *
     *      Opens a cursor, reads at most one row and releases it before returning, so the
     *      statement is immediately reusable and the connection is free for other statements.
     *      A query with no matching row returns undefined, and so does a statement without a
     *      result set such as INSERT; use run or all to obtain the counters of a non-query.
     *      Missing arguments bind NULL and extra arguments are ignored. Engine errors are
     *      reported while the statement opens or fetches (for example number 20024 on SQLite),
     *      and a statement closed by close() or by closing its connection fails with 20027.
     *
     *      Example — reuse one statement with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v); // a
     *      console.log(stmt.get('z'));   // undefined
     *      console.log(stmt.get('a').v); // a, the cursor was released and reuse is safe
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns the first row object, or undefined if there is no result
     *
     */
    get(...args: any[]): any;

    /**
     * @description Executes the statement and returns the first row, or undefined if there is no result
     *
     *      Opens a cursor, reads at most one row and releases it before returning, so the
     *      statement is immediately reusable and the connection is free for other statements.
     *      A query with no matching row returns undefined, and so does a statement without a
     *      result set such as INSERT; use run or all to obtain the counters of a non-query.
     *      Missing arguments bind NULL and extra arguments are ignored. Engine errors are
     *      reported while the statement opens or fetches (for example number 20024 on SQLite),
     *      and a statement closed by close() or by closing its connection fails with 20027.
     *
     *      Example — reuse one statement with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v); // a
     *      console.log(stmt.get('z'));   // undefined
     *      console.log(stmt.get('a').v); // a, the cursor was released and reuse is safe
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns the first row object, or undefined if there is no result
     *
     */
    getSync(...args: any[]): any;

    /**
     * @description Executes the statement and returns the first row, or undefined if there is no result
     *
     *      Opens a cursor, reads at most one row and releases it before returning, so the
     *      statement is immediately reusable and the connection is free for other statements.
     *      A query with no matching row returns undefined, and so does a statement without a
     *      result set such as INSERT; use run or all to obtain the counters of a non-query.
     *      Missing arguments bind NULL and extra arguments are ignored. Engine errors are
     *      reported while the statement opens or fetches (for example number 20024 on SQLite),
     *      and a statement closed by close() or by closing its connection fails with 20027.
     *
     *      Example — reuse one statement with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v); // a
     *      console.log(stmt.get('z'));   // undefined
     *      console.log(stmt.get('a').v); // a, the cursor was released and reuse is safe
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns the first row object, or undefined if there is no result
     *
     */
    getAsync(...args: any[]): Promise<any>;

    /**
     * @description Executes the statement and returns all rows (materialized at once)
     *
     *      Reads the complete result set before returning and releases the cursor, so the
     *      statement can be reused immediately. A query with no matching rows returns an empty
     *      array. A statement without a result set (INSERT/UPDATE/DELETE/DDL) returns an array
     *      with no rows plus the `affected` and `insertId` properties, mirroring the shape of
     *      execute results. Prefer iterate for large result sets: all keeps every row in memory
     *      at once.
     *
     *      @param args the bound parameters
     *      @return returns an array of all row objects
     *
     */
    all(...args: any[]): any[];

    /**
     * @description Executes the statement and returns all rows (materialized at once)
     *
     *      Reads the complete result set before returning and releases the cursor, so the
     *      statement can be reused immediately. A query with no matching rows returns an empty
     *      array. A statement without a result set (INSERT/UPDATE/DELETE/DDL) returns an array
     *      with no rows plus the `affected` and `insertId` properties, mirroring the shape of
     *      execute results. Prefer iterate for large result sets: all keeps every row in memory
     *      at once.
     *
     *      @param args the bound parameters
     *      @return returns an array of all row objects
     *
     */
    allSync(...args: any[]): any[];

    /**
     * @description Executes the statement and returns all rows (materialized at once)
     *
     *      Reads the complete result set before returning and releases the cursor, so the
     *      statement can be reused immediately. A query with no matching rows returns an empty
     *      array. A statement without a result set (INSERT/UPDATE/DELETE/DDL) returns an array
     *      with no rows plus the `affected` and `insertId` properties, mirroring the shape of
     *      execute results. Prefer iterate for large result sets: all keeps every row in memory
     *      at once.
     *
     *      @param args the bound parameters
     *      @return returns an array of all row objects
     *
     */
    allAsync(...args: any[]): Promise<any[]>;

    /**
     * @description Executes a statement that returns no result set
     *
     *      Runs the statement and discards any result rows (a SELECT is drained row by row so
     *      the cursor is released), then returns an object with the `changes` and
     *      `lastInsertRowid` counters. Both are BigInt carrying the raw 64-bit engine values,
     *      so counters beyond 2^53 keep their precision; convert with Number() when a plain
     *      number is enough and remember that JSON.stringify throws on BigInt values. Engines
     *      that do not report a generated key (mssql) leave `lastInsertRowid` at 0.
     *
     *      Example — insert and read the counters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, v TEXT)');
     *
     *      const stmt = conn.prepare('INSERT INTO t (v) VALUES (?)');
     *      const result = stmt.run('first');
     *      console.log(Number(result.changes));           // 1
     *      console.log(Number(result.lastInsertRowid));   // 1
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a { changes, lastInsertRowid } object
     *
     */
    run(...args: any[]): any;

    /**
     * @description Executes a statement that returns no result set
     *
     *      Runs the statement and discards any result rows (a SELECT is drained row by row so
     *      the cursor is released), then returns an object with the `changes` and
     *      `lastInsertRowid` counters. Both are BigInt carrying the raw 64-bit engine values,
     *      so counters beyond 2^53 keep their precision; convert with Number() when a plain
     *      number is enough and remember that JSON.stringify throws on BigInt values. Engines
     *      that do not report a generated key (mssql) leave `lastInsertRowid` at 0.
     *
     *      Example — insert and read the counters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, v TEXT)');
     *
     *      const stmt = conn.prepare('INSERT INTO t (v) VALUES (?)');
     *      const result = stmt.run('first');
     *      console.log(Number(result.changes));           // 1
     *      console.log(Number(result.lastInsertRowid));   // 1
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a { changes, lastInsertRowid } object
     *
     */
    runSync(...args: any[]): any;

    /**
     * @description Executes a statement that returns no result set
     *
     *      Runs the statement and discards any result rows (a SELECT is drained row by row so
     *      the cursor is released), then returns an object with the `changes` and
     *      `lastInsertRowid` counters. Both are BigInt carrying the raw 64-bit engine values,
     *      so counters beyond 2^53 keep their precision; convert with Number() when a plain
     *      number is enough and remember that JSON.stringify throws on BigInt values. Engines
     *      that do not report a generated key (mssql) leave `lastInsertRowid` at 0.
     *
     *      Example — insert and read the counters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, v TEXT)');
     *
     *      const stmt = conn.prepare('INSERT INTO t (v) VALUES (?)');
     *      const result = stmt.run('first');
     *      console.log(Number(result.changes));           // 1
     *      console.log(Number(result.lastInsertRowid));   // 1
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a { changes, lastInsertRowid } object
     *
     */
    runAsync(...args: any[]): Promise<any>;

    /**
     * @description Executes the statement and returns an iterator for row-by-row reads
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, releasing the cursor automatically and making
     *      the connection immediately reusable. While the cursor is open the connection
     *      rejects other statements with error number 20028. Calling next()/return() manually
     *      is dangerous: the cursor stays open until the results are exhausted, and if return()
     *      is forgotten on break or exception it occupies the connection. A statement without a
     *      result set yields an iterator that is already done.
     *
     *      Example — stream a result set and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      const stmt = conn.prepare('SELECT v FROM t ORDER BY v');
     *      for (const row of stmt.iterate()) {
     *          if (row.v === 2)
     *              break;           // breaking calls return() and releases the cursor
     *          console.log(row.v);  // 0, then 1
     *      }
     *      console.log(stmt.all().length); // 5, the connection is reusable again
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterate(...args: any[]): Iterator<any>;

    /**
     * @description Executes the statement and returns an iterator for row-by-row reads
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, releasing the cursor automatically and making
     *      the connection immediately reusable. While the cursor is open the connection
     *      rejects other statements with error number 20028. Calling next()/return() manually
     *      is dangerous: the cursor stays open until the results are exhausted, and if return()
     *      is forgotten on break or exception it occupies the connection. A statement without a
     *      result set yields an iterator that is already done.
     *
     *      Example — stream a result set and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      const stmt = conn.prepare('SELECT v FROM t ORDER BY v');
     *      for (const row of stmt.iterate()) {
     *          if (row.v === 2)
     *              break;           // breaking calls return() and releases the cursor
     *          console.log(row.v);  // 0, then 1
     *      }
     *      console.log(stmt.all().length); // 5, the connection is reusable again
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateSync(...args: any[]): Iterator<any>;

    /**
     * @description Executes the statement and returns an iterator for row-by-row reads
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, releasing the cursor automatically and making
     *      the connection immediately reusable. While the cursor is open the connection
     *      rejects other statements with error number 20028. Calling next()/return() manually
     *      is dangerous: the cursor stays open until the results are exhausted, and if return()
     *      is forgotten on break or exception it occupies the connection. A statement without a
     *      result set yields an iterator that is already done.
     *
     *      Example — stream a result set and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      const stmt = conn.prepare('SELECT v FROM t ORDER BY v');
     *      for (const row of stmt.iterate()) {
     *          if (row.v === 2)
     *              break;           // breaking calls return() and releases the cursor
     *          console.log(row.v);  // 0, then 1
     *      }
     *      console.log(stmt.all().length); // 5, the connection is reusable again
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateAsync(...args: any[]): Promise<Iterator<any>>;

    /**
     * @description Returns the result column metadata
     *
     *      Returns one object per result column with the properties `name` and `type`; on
     *      SQLite the type is the declared type of the column and an empty string for computed
     *      columns such as aggregates. On MySQL and ODBC the method is not implemented and
     *      fails with error number 20009 ("requires server-side prepared statements"), because
     *      those engines do not send result metadata at prepare time. Reading the metadata does
     *      not execute the statement; after close the call fails with 20027.
     *
     *      Example — inspect the column metadata of a query:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const stmt = conn.prepare('SELECT id, name, COUNT(*) AS n FROM user');
     *      stmt.columns().forEach((col) => console.log(col.name, col.type));
     *      // id INTEGER / name TEXT / n (computed columns have an empty type)
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columns(): any[];

    columns(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Returns the result column metadata
     *
     *      Returns one object per result column with the properties `name` and `type`; on
     *      SQLite the type is the declared type of the column and an empty string for computed
     *      columns such as aggregates. On MySQL and ODBC the method is not implemented and
     *      fails with error number 20009 ("requires server-side prepared statements"), because
     *      those engines do not send result metadata at prepare time. Reading the metadata does
     *      not execute the statement; after close the call fails with 20027.
     *
     *      Example — inspect the column metadata of a query:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const stmt = conn.prepare('SELECT id, name, COUNT(*) AS n FROM user');
     *      stmt.columns().forEach((col) => console.log(col.name, col.type));
     *      // id INTEGER / name TEXT / n (computed columns have an empty type)
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columnsSync(): any[];

    /**
     * @description Returns the result column metadata
     *
     *      Returns one object per result column with the properties `name` and `type`; on
     *      SQLite the type is the declared type of the column and an empty string for computed
     *      columns such as aggregates. On MySQL and ODBC the method is not implemented and
     *      fails with error number 20009 ("requires server-side prepared statements"), because
     *      those engines do not send result metadata at prepare time. Reading the metadata does
     *      not execute the statement; after close the call fails with 20027.
     *
     *      Example — inspect the column metadata of a query:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const stmt = conn.prepare('SELECT id, name, COUNT(*) AS n FROM user');
     *      stmt.columns().forEach((col) => console.log(col.name, col.type));
     *      // id INTEGER / name TEXT / n (computed columns have an empty type)
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columnsAsync(): Promise<any[]>;

    /**
     * @description The original SQL of the current statement
     *
     *      Returns exactly the SQL string that was passed to prepare, without the bound
     *      parameters, and keeps working after close(). It is useful to log or compare
     *      statements; the placeholders are still `?` in the returned text.
     *
     */
    readonly sourceSQL: string;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     *
     *      Finalizes the engine-side statement resources (for SQLite the compiled statement)
     *      and detaches it from the connection. Other members of the closed statement either
     *      fail with error number 20027 or, for iterators created before the close, report
     *      "done". Closing the connection closes all of its statements as well; close() itself
     *      is idempotent.
     *
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     *
     *      Finalizes the engine-side statement resources (for SQLite the compiled statement)
     *      and detaches it from the connection. Other members of the closed statement either
     *      fail with error number 20027 or, for iterators created before the close, report
     *      "done". Closing the connection closes all of its statements as well; close() itself
     *      is idempotent.
     *
     */
    closeSync(): void;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     *
     *      Finalizes the engine-side statement resources (for SQLite the compiled statement)
     *      and detaches it from the connection. Other members of the closed statement either
     *      fail with error number 20027 or, for iterators created before the close, report
     *      "done". Closing the connection closes all of its statements as well; close() itself
     *      is idempotent.
     *
     */
    closeAsync(): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * The promise variant of the Statement class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_StatementPromise extends Class_object {
    /**
     * @description Executes the statement and returns the first row, or undefined if there is no result
     *
     *      Opens a cursor, reads at most one row and releases it before returning, so the
     *      statement is immediately reusable and the connection is free for other statements.
     *      A query with no matching row returns undefined, and so does a statement without a
     *      result set such as INSERT; use run or all to obtain the counters of a non-query.
     *      Missing arguments bind NULL and extra arguments are ignored. Engine errors are
     *      reported while the statement opens or fetches (for example number 20024 on SQLite),
     *      and a statement closed by close() or by closing its connection fails with 20027.
     *
     *      Example — reuse one statement with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v); // a
     *      console.log(stmt.get('z'));   // undefined
     *      console.log(stmt.get('a').v); // a, the cursor was released and reuse is safe
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns the first row object, or undefined if there is no result
     *
     */
    get(...args: any[]): Promise<any>;

    /**
     * @description Executes the statement and returns the first row, or undefined if there is no result
     *
     *      Opens a cursor, reads at most one row and releases it before returning, so the
     *      statement is immediately reusable and the connection is free for other statements.
     *      A query with no matching row returns undefined, and so does a statement without a
     *      result set such as INSERT; use run or all to obtain the counters of a non-query.
     *      Missing arguments bind NULL and extra arguments are ignored. Engine errors are
     *      reported while the statement opens or fetches (for example number 20024 on SQLite),
     *      and a statement closed by close() or by closing its connection fails with 20027.
     *
     *      Example — reuse one statement with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v); // a
     *      console.log(stmt.get('z'));   // undefined
     *      console.log(stmt.get('a').v); // a, the cursor was released and reuse is safe
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns the first row object, or undefined if there is no result
     *
     */
    getSync(...args: any[]): any;

    /**
     * @description Executes the statement and returns the first row, or undefined if there is no result
     *
     *      Opens a cursor, reads at most one row and releases it before returning, so the
     *      statement is immediately reusable and the connection is free for other statements.
     *      A query with no matching row returns undefined, and so does a statement without a
     *      result set such as INSERT; use run or all to obtain the counters of a non-query.
     *      Missing arguments bind NULL and extra arguments are ignored. Engine errors are
     *      reported while the statement opens or fetches (for example number 20024 on SQLite),
     *      and a statement closed by close() or by closing its connection fails with 20027.
     *
     *      Example — reuse one statement with different parameters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v TEXT)');
     *      conn.execute("INSERT INTO t VALUES ('a')");
     *
     *      const stmt = conn.prepare('SELECT * FROM t WHERE v = ?');
     *      console.log(stmt.get('a').v); // a
     *      console.log(stmt.get('z'));   // undefined
     *      console.log(stmt.get('a').v); // a, the cursor was released and reuse is safe
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns the first row object, or undefined if there is no result
     *
     */
    getAsync(...args: any[]): Promise<any>;

    /**
     * @description Executes the statement and returns all rows (materialized at once)
     *
     *      Reads the complete result set before returning and releases the cursor, so the
     *      statement can be reused immediately. A query with no matching rows returns an empty
     *      array. A statement without a result set (INSERT/UPDATE/DELETE/DDL) returns an array
     *      with no rows plus the `affected` and `insertId` properties, mirroring the shape of
     *      execute results. Prefer iterate for large result sets: all keeps every row in memory
     *      at once.
     *
     *      @param args the bound parameters
     *      @return returns an array of all row objects
     *
     */
    all(...args: any[]): Promise<any[]>;

    /**
     * @description Executes the statement and returns all rows (materialized at once)
     *
     *      Reads the complete result set before returning and releases the cursor, so the
     *      statement can be reused immediately. A query with no matching rows returns an empty
     *      array. A statement without a result set (INSERT/UPDATE/DELETE/DDL) returns an array
     *      with no rows plus the `affected` and `insertId` properties, mirroring the shape of
     *      execute results. Prefer iterate for large result sets: all keeps every row in memory
     *      at once.
     *
     *      @param args the bound parameters
     *      @return returns an array of all row objects
     *
     */
    allSync(...args: any[]): any[];

    /**
     * @description Executes the statement and returns all rows (materialized at once)
     *
     *      Reads the complete result set before returning and releases the cursor, so the
     *      statement can be reused immediately. A query with no matching rows returns an empty
     *      array. A statement without a result set (INSERT/UPDATE/DELETE/DDL) returns an array
     *      with no rows plus the `affected` and `insertId` properties, mirroring the shape of
     *      execute results. Prefer iterate for large result sets: all keeps every row in memory
     *      at once.
     *
     *      @param args the bound parameters
     *      @return returns an array of all row objects
     *
     */
    allAsync(...args: any[]): Promise<any[]>;

    /**
     * @description Executes a statement that returns no result set
     *
     *      Runs the statement and discards any result rows (a SELECT is drained row by row so
     *      the cursor is released), then returns an object with the `changes` and
     *      `lastInsertRowid` counters. Both are BigInt carrying the raw 64-bit engine values,
     *      so counters beyond 2^53 keep their precision; convert with Number() when a plain
     *      number is enough and remember that JSON.stringify throws on BigInt values. Engines
     *      that do not report a generated key (mssql) leave `lastInsertRowid` at 0.
     *
     *      Example — insert and read the counters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, v TEXT)');
     *
     *      const stmt = conn.prepare('INSERT INTO t (v) VALUES (?)');
     *      const result = stmt.run('first');
     *      console.log(Number(result.changes));           // 1
     *      console.log(Number(result.lastInsertRowid));   // 1
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a { changes, lastInsertRowid } object
     *
     */
    run(...args: any[]): Promise<any>;

    /**
     * @description Executes a statement that returns no result set
     *
     *      Runs the statement and discards any result rows (a SELECT is drained row by row so
     *      the cursor is released), then returns an object with the `changes` and
     *      `lastInsertRowid` counters. Both are BigInt carrying the raw 64-bit engine values,
     *      so counters beyond 2^53 keep their precision; convert with Number() when a plain
     *      number is enough and remember that JSON.stringify throws on BigInt values. Engines
     *      that do not report a generated key (mssql) leave `lastInsertRowid` at 0.
     *
     *      Example — insert and read the counters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, v TEXT)');
     *
     *      const stmt = conn.prepare('INSERT INTO t (v) VALUES (?)');
     *      const result = stmt.run('first');
     *      console.log(Number(result.changes));           // 1
     *      console.log(Number(result.lastInsertRowid));   // 1
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a { changes, lastInsertRowid } object
     *
     */
    runSync(...args: any[]): any;

    /**
     * @description Executes a statement that returns no result set
     *
     *      Runs the statement and discards any result rows (a SELECT is drained row by row so
     *      the cursor is released), then returns an object with the `changes` and
     *      `lastInsertRowid` counters. Both are BigInt carrying the raw 64-bit engine values,
     *      so counters beyond 2^53 keep their precision; convert with Number() when a plain
     *      number is enough and remember that JSON.stringify throws on BigInt values. Engines
     *      that do not report a generated key (mssql) leave `lastInsertRowid` at 0.
     *
     *      Example — insert and read the counters:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (id INTEGER PRIMARY KEY AUTOINCREMENT, v TEXT)');
     *
     *      const stmt = conn.prepare('INSERT INTO t (v) VALUES (?)');
     *      const result = stmt.run('first');
     *      console.log(Number(result.changes));           // 1
     *      console.log(Number(result.lastInsertRowid));   // 1
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a { changes, lastInsertRowid } object
     *
     */
    runAsync(...args: any[]): Promise<any>;

    /**
     * @description Executes the statement and returns an iterator for row-by-row reads
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, releasing the cursor automatically and making
     *      the connection immediately reusable. While the cursor is open the connection
     *      rejects other statements with error number 20028. Calling next()/return() manually
     *      is dangerous: the cursor stays open until the results are exhausted, and if return()
     *      is forgotten on break or exception it occupies the connection. A statement without a
     *      result set yields an iterator that is already done.
     *
     *      Example — stream a result set and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      const stmt = conn.prepare('SELECT v FROM t ORDER BY v');
     *      for (const row of stmt.iterate()) {
     *          if (row.v === 2)
     *              break;           // breaking calls return() and releases the cursor
     *          console.log(row.v);  // 0, then 1
     *      }
     *      console.log(stmt.all().length); // 5, the connection is reusable again
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterate(...args: any[]): Promise<Iterator<any>>;

    /**
     * @description Executes the statement and returns an iterator for row-by-row reads
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, releasing the cursor automatically and making
     *      the connection immediately reusable. While the cursor is open the connection
     *      rejects other statements with error number 20028. Calling next()/return() manually
     *      is dangerous: the cursor stays open until the results are exhausted, and if return()
     *      is forgotten on break or exception it occupies the connection. A statement without a
     *      result set yields an iterator that is already done.
     *
     *      Example — stream a result set and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      const stmt = conn.prepare('SELECT v FROM t ORDER BY v');
     *      for (const row of stmt.iterate()) {
     *          if (row.v === 2)
     *              break;           // breaking calls return() and releases the cursor
     *          console.log(row.v);  // 0, then 1
     *      }
     *      console.log(stmt.all().length); // 5, the connection is reusable again
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateSync(...args: any[]): Iterator<any>;

    /**
     * @description Executes the statement and returns an iterator for row-by-row reads
     *
     *      Traversing with for...of is recommended: the engine calls the iterator's return()
     *      when the loop ends, breaks or throws, releasing the cursor automatically and making
     *      the connection immediately reusable. While the cursor is open the connection
     *      rejects other statements with error number 20028. Calling next()/return() manually
     *      is dangerous: the cursor stays open until the results are exhausted, and if return()
     *      is forgotten on break or exception it occupies the connection. A statement without a
     *      result set yields an iterator that is already done.
     *
     *      Example — stream a result set and stop early:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE t (v INTEGER)');
     *      for (let i = 0; i < 5; i++)
     *          conn.execute('INSERT INTO t VALUES (?)', i);
     *
     *      const stmt = conn.prepare('SELECT v FROM t ORDER BY v');
     *      for (const row of stmt.iterate()) {
     *          if (row.v === 2)
     *              break;           // breaking calls return() and releases the cursor
     *          console.log(row.v);  // 0, then 1
     *      }
     *      console.log(stmt.all().length); // 5, the connection is reusable again
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @param args the bound parameters
     *      @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterateAsync(...args: any[]): Promise<Iterator<any>>;

    /**
     * @description Returns the result column metadata
     *
     *      Returns one object per result column with the properties `name` and `type`; on
     *      SQLite the type is the declared type of the column and an empty string for computed
     *      columns such as aggregates. On MySQL and ODBC the method is not implemented and
     *      fails with error number 20009 ("requires server-side prepared statements"), because
     *      those engines do not send result metadata at prepare time. Reading the metadata does
     *      not execute the statement; after close the call fails with 20027.
     *
     *      Example — inspect the column metadata of a query:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const stmt = conn.prepare('SELECT id, name, COUNT(*) AS n FROM user');
     *      stmt.columns().forEach((col) => console.log(col.name, col.type));
     *      // id INTEGER / name TEXT / n (computed columns have an empty type)
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columns(): Promise<any[]>;

    /**
     * @description Returns the result column metadata
     *
     *      Returns one object per result column with the properties `name` and `type`; on
     *      SQLite the type is the declared type of the column and an empty string for computed
     *      columns such as aggregates. On MySQL and ODBC the method is not implemented and
     *      fails with error number 20009 ("requires server-side prepared statements"), because
     *      those engines do not send result metadata at prepare time. Reading the metadata does
     *      not execute the statement; after close the call fails with 20027.
     *
     *      Example — inspect the column metadata of a query:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const stmt = conn.prepare('SELECT id, name, COUNT(*) AS n FROM user');
     *      stmt.columns().forEach((col) => console.log(col.name, col.type));
     *      // id INTEGER / name TEXT / n (computed columns have an empty type)
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columnsSync(): any[];

    /**
     * @description Returns the result column metadata
     *
     *      Returns one object per result column with the properties `name` and `type`; on
     *      SQLite the type is the declared type of the column and an empty string for computed
     *      columns such as aggregates. On MySQL and ODBC the method is not implemented and
     *      fails with error number 20009 ("requires server-side prepared statements"), because
     *      those engines do not send result metadata at prepare time. Reading the metadata does
     *      not execute the statement; after close the call fails with 20027.
     *
     *      Example — inspect the column metadata of a query:
     *      ```JavaScript
     *      const db = require('db');
     *      const conn = db.openSQLite(':memory:');
     *      conn.execute('CREATE TABLE user (id INTEGER PRIMARY KEY, name TEXT)');
     *
     *      const stmt = conn.prepare('SELECT id, name, COUNT(*) AS n FROM user');
     *      stmt.columns().forEach((col) => console.log(col.name, col.type));
     *      // id INTEGER / name TEXT / n (computed columns have an empty type)
     *
     *      stmt.close();
     *      conn.close();
     *      ```
     *
     *      @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columnsAsync(): Promise<any[]>;

    /**
     * @description The original SQL of the current statement
     *
     *      Returns exactly the SQL string that was passed to prepare, without the bound
     *      parameters, and keeps working after close(). It is useful to log or compare
     *      statements; the placeholders are still `?` in the returned text.
     *
     */
    readonly sourceSQL: string;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     *
     *      Finalizes the engine-side statement resources (for SQLite the compiled statement)
     *      and detaches it from the connection. Other members of the closed statement either
     *      fail with error number 20027 or, for iterators created before the close, report
     *      "done". Closing the connection closes all of its statements as well; close() itself
     *      is idempotent.
     *
     */
    close(): Promise<void>;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     *
     *      Finalizes the engine-side statement resources (for SQLite the compiled statement)
     *      and detaches it from the connection. Other members of the closed statement either
     *      fail with error number 20027 or, for iterators created before the close, report
     *      "done". Closing the connection closes all of its statements as well; close() itself
     *      is idempotent.
     *
     */
    closeSync(): void;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     *
     *      Finalizes the engine-side statement resources (for SQLite the compiled statement)
     *      and detaches it from the connection. Other members of the closed statement either
     *      fail with error number 20027 or, for iterators created before the close, report
     *      "done". Closing the connection closes all of its statements as well; close() itself
     *      is idempotent.
     *
     */
    closeAsync(): Promise<void>;

}


declare namespace Class_Statement {
    const promises: FIBJS.GeneralObject;
}
