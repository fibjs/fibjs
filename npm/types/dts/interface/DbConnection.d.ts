/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Statement.d.ts" />
/**
 * @description DBConnection is the base class of database connections, used to establish and maintain a database connection session. It implements the basic connection operations and serves as the base for derived classes. It also supports starting, committing and rolling back transactions.
 *
 * Subclasses of DBConnection include Odbc, MySQL and SQLite; by instantiating each subclass we can conveniently access different kinds of databases.
 *
 * DBConnection cannot be created directly; it can only be created with methods such as db.open, for example:
 *
 * ```js
 * var db = require("db");
 * var conn = db.open("mysql://root:123456@localhost:3306/test");
 * ```
 *
 */
declare class Class_DbConnection extends Class_object {
    /**
     * @description Queries the type of the current database connection
     */
    readonly type: string;

    /**
     * @description Closes the current database connection
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the current database connection
     */
    closeSync(): void;

    /**
     * @description Closes the current database connection
     */
    closeAsync(): Promise<void>;

    /**
     * @description Selects the default database of the current database connection
     * 	 @param dbName the database name
     *
     */
    use(dbName: string): void;

    use(dbName: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Selects the default database of the current database connection
     * 	 @param dbName the database name
     *
     */
    useSync(dbName: string): void;

    /**
     * @description Selects the default database of the current database connection
     * 	 @param dbName the database name
     *
     */
    useAsync(dbName: string): Promise<void>;

    /**
     * @description Gets information about all tables in the current database
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTables(): any[];

    getTables(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Gets information about all tables in the current database
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesSync(): any[];

    /**
     * @description Gets information about all tables in the current database
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesAsync(): Promise<any[]>;

    /**
     * @description Gets detailed information about the given table
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
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoSync(tableName: string): any[];

    /**
     * @description Gets detailed information about the given table
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoAsync(tableName: string): Promise<any[]>;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    begin(point?: string): void;

    begin(point?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginSync(point?: string): void;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginAsync(point?: string): Promise<void>;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commit(point?: string): void;

    commit(point?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitSync(point?: string): void;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitAsync(point?: string): Promise<void>;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollback(point?: string): void;

    rollback(point?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackSync(point?: string): void;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackAsync(point?: string): Promise<void>;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *      The execution of func has three outcomes:
     *      * the function returns normally, including finishing or an explicit return; the transaction is committed automatically
     *      * the function returns false; the transaction is rolled back
     *      * the function throws an error; the transaction is rolled back automatically
     *
     *      @param func the function to execute in a transaction
     *      @return returns whether the transaction was committed: returns true on a normal commit, false on rollback, and throws if the transaction fails
     *
     */
    trans(func: (conn: Class_DbConnection | Class_DbConnectionPromise)=>any): boolean;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *      The execution of func has three outcomes:
     *      * the function returns normally, including finishing or an explicit return; the transaction is committed automatically
     *      * the function returns false; the transaction is rolled back
     *      * the function throws an error; the transaction is rolled back automatically
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
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    execute(sql: string): any[];

    execute(sql: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string): any[];

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeAsync(sql: string): Promise<any[]>;

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
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
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string, ...args: any[]): any[];

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
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
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns the formatted sql command
     *
     */
    format(sql: string, ...args: any[]): string;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
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
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareSync(sql: string): Class_Statement;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareAsync(sql: string): Promise<Class_StatementPromise>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended (break/exception releases the cursor automatically):
     *
     *      ```js
     *      for (var row of conn.iterate('SELECT * FROM log WHERE ts > ?', ts)) {
     *          process(row);    // only one row resides in memory at a time
     *      }
     *      // break/exception/normal completion all release the cursor automatically; the connection is immediately reusable
     *      ```
     *
     *      Calling next()/return() manually is dangerous; you must ensure that return() is called on exception and early exit
     *      return() releases the cursor; otherwise the leaked cursor occupies the connection.
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
     *      Traversing with for...of is recommended (break/exception releases the cursor automatically):
     *
     *      ```js
     *      for (var row of conn.iterate('SELECT * FROM log WHERE ts > ?', ts)) {
     *          process(row);    // only one row resides in memory at a time
     *      }
     *      // break/exception/normal completion all release the cursor automatically; the connection is immediately reusable
     *      ```
     *
     *      Calling next()/return() manually is dangerous; you must ensure that return() is called on exception and early exit
     *      return() releases the cursor; otherwise the leaked cursor occupies the connection.
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
     *      Traversing with for...of is recommended (break/exception releases the cursor automatically):
     *
     *      ```js
     *      for (var row of conn.iterate('SELECT * FROM log WHERE ts > ?', ts)) {
     *          process(row);    // only one row resides in memory at a time
     *      }
     *      // break/exception/normal completion all release the cursor automatically; the connection is immediately reusable
     *      ```
     *
     *      Calling next()/return() manually is dangerous; you must ensure that return() is called on exception and early exit
     *      return() releases the cursor; otherwise the leaked cursor occupies the connection.
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
     */
    readonly type: string;

    /**
     * @description Closes the current database connection
     */
    close(): Promise<void>;

    /**
     * @description Closes the current database connection
     */
    closeSync(): void;

    /**
     * @description Closes the current database connection
     */
    closeAsync(): Promise<void>;

    /**
     * @description Selects the default database of the current database connection
     * 	 @param dbName the database name
     *
     */
    use(dbName: string): Promise<void>;

    /**
     * @description Selects the default database of the current database connection
     * 	 @param dbName the database name
     *
     */
    useSync(dbName: string): void;

    /**
     * @description Selects the default database of the current database connection
     * 	 @param dbName the database name
     *
     */
    useAsync(dbName: string): Promise<void>;

    /**
     * @description Gets information about all tables in the current database
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTables(): Promise<any[]>;

    /**
     * @description Gets information about all tables in the current database
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesSync(): any[];

    /**
     * @description Gets information about all tables in the current database
     *
     *      @return returns an array containing table information; each element contains the table name and related properties
     *
     */
    getTablesAsync(): Promise<any[]>;

    /**
     * @description Gets detailed information about the given table
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfo(tableName: string): Promise<any[]>;

    /**
     * @description Gets detailed information about the given table
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoSync(tableName: string): any[];

    /**
     * @description Gets detailed information about the given table
     *
     *      @param tableName the table name to query
     *      @return returns an array containing detailed table information; each element contains the field name, type, length, whether NULL is allowed and other properties
     *
     */
    getTableInfoAsync(tableName: string): Promise<any[]>;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    begin(point?: string): Promise<void>;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginSync(point?: string): void;

    /**
     * @description Starts a transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    beginAsync(point?: string): Promise<void>;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commit(point?: string): Promise<void>;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitSync(point?: string): void;

    /**
     * @description Commits the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    commitAsync(point?: string): Promise<void>;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollback(point?: string): Promise<void>;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackSync(point?: string): void;

    /**
     * @description Rolls back the transaction on the current database connection
     *
     *      @param point the transaction name, not specified by default
     *
     */
    rollbackAsync(point?: string): Promise<void>;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *      The execution of func has three outcomes:
     *      * the function returns normally, including finishing or an explicit return; the transaction is committed automatically
     *      * the function returns false; the transaction is rolled back
     *      * the function throws an error; the transaction is rolled back automatically
     *
     *      @param func the function to execute in a transaction
     *      @return returns whether the transaction was committed: returns true on a normal commit, false on rollback, and throws if the transaction fails
     *
     */
    trans(func: (conn: Class_DbConnection | Class_DbConnectionPromise)=>any): boolean;

    /**
     * @description Enters a transaction to execute a function, and commits or rolls back depending on the function result
     *      The execution of func has three outcomes:
     *      * the function returns normally, including finishing or an explicit return; the transaction is committed automatically
     *      * the function returns false; the transaction is rolled back
     *      * the function throws an error; the transaction is rolled back automatically
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
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    execute(sql: string): Promise<any[]>;

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string): any[];

    /**
     * @description Executes an sql command and returns the execution result
     *
     *      @param sql the sql string
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeAsync(sql: string): Promise<any[]>;

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
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
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns an array containing the result records; if the request is UPDATE or INSERT, the result also contains affected and insertId; mssql does not support insertId.
     *
     */
    executeSync(sql: string, ...args: any[]): any[];

    /**
     * @description Executes an sql command and returns the execution result; the string can be formatted with the given parameters
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
     *      @param sql the format string; optional parameters are specified with ?. For example: 'SELECT FROM TEST WHERE [id]=?'
     *      @param args the optional parameter list
     *      @return returns the formatted sql command
     *
     */
    format(sql: string, ...args: any[]): string;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepare(sql: string): Promise<Class_StatementPromise>;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareSync(sql: string): Class_Statement;

    /**
     * @description Compiles an SQL statement into a prepared statement (single statement) supporting row-by-row reads
     *
     *      @param sql the query statement to prepare
     *      @return returns the prepared statement object
     *
     */
    prepareAsync(sql: string): Promise<Class_StatementPromise>;

    /**
     * @description Executes and returns an iterator over the rows (equivalent to stmt.iterate(...args))
     *
     *      Traversing with for...of is recommended (break/exception releases the cursor automatically):
     *
     *      ```js
     *      for (var row of conn.iterate('SELECT * FROM log WHERE ts > ?', ts)) {
     *          process(row);    // only one row resides in memory at a time
     *      }
     *      // break/exception/normal completion all release the cursor automatically; the connection is immediately reusable
     *      ```
     *
     *      Calling next()/return() manually is dangerous; you must ensure that return() is called on exception and early exit
     *      return() releases the cursor; otherwise the leaked cursor occupies the connection.
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
     *      Traversing with for...of is recommended (break/exception releases the cursor automatically):
     *
     *      ```js
     *      for (var row of conn.iterate('SELECT * FROM log WHERE ts > ?', ts)) {
     *          process(row);    // only one row resides in memory at a time
     *      }
     *      // break/exception/normal completion all release the cursor automatically; the connection is immediately reusable
     *      ```
     *
     *      Calling next()/return() manually is dangerous; you must ensure that return() is called on exception and early exit
     *      return() releases the cursor; otherwise the leaked cursor occupies the connection.
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
     *      Traversing with for...of is recommended (break/exception releases the cursor automatically):
     *
     *      ```js
     *      for (var row of conn.iterate('SELECT * FROM log WHERE ts > ?', ts)) {
     *          process(row);    // only one row resides in memory at a time
     *      }
     *      // break/exception/normal completion all release the cursor automatically; the connection is immediately reusable
     *      ```
     *
     *      Calling next()/return() manually is dangerous; you must ensure that return() is called on exception and early exit
     *      return() releases the cursor; otherwise the leaked cursor occupies the connection.
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
