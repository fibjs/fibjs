/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Prepared statement object, can be executed repeatedly, supports row-by-row reads
 *
 * Statement is created by DbConnection.prepare(). Unlike the "materialize all results at once" behavior of execute,
 * Statement supports four execution modes: get/all/run/iterate; iterate produces rows
 * one by one, keeping only one row in memory at any time, with bounded memory.
 *
 * The iterator returned by iterate is best traversed with for...of: when the loop breaks or throws, the engine
 * automatically calls the iterator's return() to release the cursor (IteratorClose semantics), with no manual intervention:
 *
 * ```js
 * var stmt = conn.prepare('SELECT * FROM big_table WHERE region = ?');
 * for (var row of stmt.iterate('east')) {
 *     process(row);        // only one row resides in memory at a time
 * }
 * // break/exception/normal completion all release the cursor automatically; the connection is immediately reusable
 * ```
 *
 * Calling the iterator's next()/return() manually is dangerous: the iterator keeps
 * the cursor open until the results are exhausted; if return() is forgotten on break or exception, the cursor leaks and occupies the connection
 * (subsequent statements on the same connection report BUSY, and under SQLite it may also block table schema changes of other connections).
 * Choosing the manual approach means you take responsibility for releasing resources yourself.
 *
 */
declare class Class_Statement extends Class_object {
    /**
     * @description Executes the statement and returns the first row, or undefined if there is no result
     *          @param args the bound parameters
     *          @return returns the first row object, or undefined if there is no result
     *
     */
    get(...args: any[]): any;

    /**
     * @description Executes the statement and returns all rows (materialized at once)
     *          @param args the bound parameters
     *          @return returns an array of all row objects
     *
     */
    all(...args: any[]): any[];

    /**
     * @description Executes a statement that returns no result set
     *          @param args the bound parameters
     *          @return returns a { changes, lastInsertRowid } object
     *
     */
    run(...args: any[]): any;

    /**
     * @description Executes the statement and returns an iterator for row-by-row reads
     *
     *          Traversing with for...of is recommended (break/exception releases the cursor automatically); calling
     *          next()/return() manually is dangerous; you must ensure that return() is called on exception and early exit
     *          return() to release the cursor, otherwise the leaked cursor occupies the connection.
     *
     *          @param args the bound parameters
     *          @return returns a row iterator that produces row objects one by one with bounded memory
     *
     */
    iterate(...args: any[]): Iterator<any>;

    /**
     * @description Returns the result column metadata
     *          @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columns(): any[];

    columns(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Returns the result column metadata
     *          @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columnsSync(): any[];

    /**
     * @description Returns the result column metadata
     *          @return returns an array of column metadata; each item contains name/type and other properties
     *
     */
    columnsAsync(): Promise<any[]>;

    /**
     * @description The original SQL of the current statement
     */
    readonly sourceSQL: string;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     */
    closeSync(): void;

    /**
     * @description Actively closes and releases the underlying handle; it is released automatically after the iteration ends and can be called repeatedly
     */
    closeAsync(): Promise<void>;

}

