/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description MySQL is the DbConnection implementation for MySQL servers
 *
 *  Obtained from:
 *  - `db.openMySQL('mysql://user:password@host:port/database')`;
 *  - `db.open('mysql://...')` — the same connection through the generic entry.
 *
 *  Concepts:
 *
 *  - **Connection string**: `mysql://user:password@host:port/database`; the user and
 *    password are URI-decoded, the database name is the URL path (may be empty) and the
 *    port defaults to 3306. Query-string options are not read, so there is no option
 *    object for charset or TLS: the driver connects with utf8mb4 and credentials belong in
 *    the URL.
 *  - **Binding**: the driver speaks the text protocol with client-side escaping. prepare
 *    returns a Statement that assembles the escaped SQL on every execution; there is no
 *    server-side prepared statement, so Statement.columns is not available (error number
 *    20009).
 *  - **Result values**: integer and decimal columns come back as number, DATE/DATETIME and
 *    TIMESTAMP as Date, TIME as a string, binary columns as Buffer and everything else as
 *    string. INSERT results carry `insertId`, the auto-increment key of the new row.
 *  - **Transactions**: the connection methods behave as described in DbConnection; the
 *    server transaction is used and named points are savepoints.
 *  - **Buffers**: rxBufferSize and txBufferSize tune the packet reader and writer buffers
 *    of the client, which helps with very large rows or bulk transfers.
 *
 *  Example 1 — connect, insert and query through the shared API (needs a MySQL server):
 *  ```JavaScript
 *  // requires: mysql
 *  const db = require('db');
 *  const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
 *
 *  conn.execute('CREATE TABLE IF NOT EXISTS user ('
 *      + 'id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(64))');
 *  const inserted = conn.execute('INSERT INTO user (name) VALUES (?)', 'alice');
 *  console.log(inserted.affected, inserted.insertId); // 1 1
 *
 *  const rows = conn.execute('SELECT name FROM user WHERE id = ?', inserted.insertId);
 *  console.log(rows[0].name); // alice
 *
 *  conn.execute('DROP TABLE user');
 *  conn.close();
 *  ```
 *
 *  Example 2 — tune the packet buffers before a bulk read (needs a MySQL server):
 *  ```JavaScript
 *  // requires: mysql
 *  const db = require('db');
 *  const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
 *
 *  console.log(conn.rxBufferSize, conn.txBufferSize); // current sizes in bytes
 *  conn.rxBufferSize = 1 << 20; // grow the receive buffer to 1 MiB
 *  conn.txBufferSize = 1 << 20; // grow the send buffer to 1 MiB
 *  console.log(conn.rxBufferSize, conn.txBufferSize);
 *
 *  conn.close();
 *  ```
 *
 */
declare class Class_MySQL extends Class_DbConnection {
    /**
     * @description The receive buffer size of the database connection
     *
     *      Size in bytes of the packet reader buffer used by the driver; readable and
     *      assignable. Assigning resizes the buffer, but a value smaller than the data already
     *      buffered is ignored, so read the property back to confirm the new size. Raise it for
     *      very large rows, lower it for workloads of many small statements. Reading or
     *      assigning it on a closed connection fails with error number 20009.
     *
     *      Example — grow the receive buffer before reading large rows (needs a MySQL server):
     *      ```JavaScript
     *      // requires: mysql
     *      const db = require('db');
     *      const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
     *
     *      conn.rxBufferSize = 1 << 20; // 1 MiB
     *      console.log(conn.rxBufferSize);
     *
     *      conn.close();
     *      ```
     *
     */
    rxBufferSize: number;

    /**
     * @description The send buffer size of the database connection
     *
     *      Size in bytes of the packet writer buffer used by the driver; readable and
     *      assignable, with the same resize rule as rxBufferSize (a value smaller than the data
     *      already buffered is ignored). Raise it when large statements or bulk parameter sets
     *      are sent. Reading or assigning it on a closed connection fails with error number
     *      20009.
     *
     *      Example — grow the send buffer before a bulk insert (needs a MySQL server):
     *      ```JavaScript
     *      // requires: mysql
     *      const db = require('db');
     *      const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
     *
     *      conn.txBufferSize = 1 << 20; // 1 MiB
     *      console.log(conn.txBufferSize);
     *
     *      conn.close();
     *      ```
     *
     */
    txBufferSize: number;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/**
 * The promise variant of the MySQL class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_MySQLPromise extends Class_DbConnectionPromise {
    /**
     * @description The receive buffer size of the database connection
     *
     *      Size in bytes of the packet reader buffer used by the driver; readable and
     *      assignable. Assigning resizes the buffer, but a value smaller than the data already
     *      buffered is ignored, so read the property back to confirm the new size. Raise it for
     *      very large rows, lower it for workloads of many small statements. Reading or
     *      assigning it on a closed connection fails with error number 20009.
     *
     *      Example — grow the receive buffer before reading large rows (needs a MySQL server):
     *      ```JavaScript
     *      // requires: mysql
     *      const db = require('db');
     *      const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
     *
     *      conn.rxBufferSize = 1 << 20; // 1 MiB
     *      console.log(conn.rxBufferSize);
     *
     *      conn.close();
     *      ```
     *
     */
    rxBufferSize: number;

    /**
     * @description The send buffer size of the database connection
     *
     *      Size in bytes of the packet writer buffer used by the driver; readable and
     *      assignable, with the same resize rule as rxBufferSize (a value smaller than the data
     *      already buffered is ignored). Raise it when large statements or bulk parameter sets
     *      are sent. Reading or assigning it on a closed connection fails with error number
     *      20009.
     *
     *      Example — grow the send buffer before a bulk insert (needs a MySQL server):
     *      ```JavaScript
     *      // requires: mysql
     *      const db = require('db');
     *      const conn = db.openMySQL('mysql://root:password@127.0.0.1:3306/test');
     *
     *      conn.txBufferSize = 1 << 20; // 1 MiB
     *      console.log(conn.txBufferSize);
     *
     *      conn.close();
     *      ```
     *
     */
    txBufferSize: number;

}


declare namespace Class_MySQL {
    const promises: FIBJS.GeneralObject;
}
