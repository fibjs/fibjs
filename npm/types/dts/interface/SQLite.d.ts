/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description The SQLite object is a member of the built-in db module and is mainly responsible for connecting to and operating on SQLite databases; it can be used to create, query, insert into and update SQLite databases. The SQLite object also provides some advanced operations such as backup and SQL formatting. SQLite connection objects also support transactions
 *
 * In practice, we usually create SQLite tables according to business needs and then perform data operations such as insert, delete, update and query, for example:
 *
 * ```JavaScript
 * var db = require('db')
 *
 * // open a SQLite database
 * var sqlite = db.openSQLite('test.db')
 *
 * // use execute method to create a table
 * sqlite.execute('CREATE TABLE test (id INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, age INT NOT NULL)')
 *
 * // use execute method to insert data
 * sqlite.execute('INSERT INTO test (id, name, age) VALUES (?, ?, ?)', 1, 'Alice', 18)
 * sqlite.execute('INSERT INTO test (id, name, age) VALUES (?, ?, ?)', 2, 'Bob', 20)
 * sqlite.execute('INSERT INTO test (id, name, age) VALUES (?, ?, ?)', 3, 'Charlie', 22)
 *
 * // use execute method to query data
 * var rs = sqlite.execute('SELECT * FROM test')
 * console.log(rs)
 *
 * // use execute method to update data
 * sqlite.execute('UPDATE test SET name=?, age=? WHERE id=?', 'Marry', 19, 1)
 *
 * // use execute method to delete data
 * sqlite.execute('DELETE FROM test WHERE id=?', 2)
 * ```
 *
 * SQLite also has a built-in vec_index module: indexes on vector fields can be created in a SQLite database and searches can be performed based on vector fields to obtain the set of vectors closest to the target vector. Vectors can be represented by numeric arrays, such as: [1, 2, 3], and vector dimensions are supported. In addition, vec_index supports batch operations inside transactions.
 *
 * Here is a simple example:
 *
 * ``` JavaScript
 * var db = require('db');
 * var path = require('path');
 *
 * var conn = db.openSQLite(path.join(__dirname, 'vec_test.db'));
 *
 * conn.execute('create virtual table vindex using vec_index(title(3), description(3))');
 *
 * conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);
 * ```
 *
 * The vec_search function can be used to perform vector search, for example:
 *
 * ``` JavaScript
 * var key = [1, 2, 5.1234];
 * var limit = 1;
 *
 * var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "${JSON.stringify(key)}")`);
 * ```
 *
 * vec_search returns the closest vector set and a distance array, where distances are ordered from smallest to largest. To return multiple closest vector sets, use the :limit parameter, for example:
 *
 * ``` JavaScript
 * var key = [1, 2, 5.1234];
 * var limit = 1;
 *
 * var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "${JSON.stringify(key)}:10")`);
 * ```
 *
 */
declare class Class_SQLite extends Class_DbConnection {
    /**
     * @description The file name of the current database
     */
    readonly fileName: string;

    /**
     * @description Queries and sets the database timeout in milliseconds
     */
    timeout: number;

    /**
     * @description Backs up the current database to a new file
     * 	 @param fileName the database file name to back up to
     */
    backup(fileName: string): void;

    backup(fileName: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Backs up the current database to a new file
     * 	 @param fileName the database file name to back up to
     */
    backupSync(fileName: string): void;

    /**
     * @description Backs up the current database to a new file
     * 	 @param fileName the database file name to back up to
     */
    backupAsync(fileName: string): Promise<void>;

}

