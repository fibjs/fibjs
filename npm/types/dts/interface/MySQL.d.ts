/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description The MySQL object is a class for operating on MySQL databases,
 *
 * Below is an example of using the MySQL object.
 *
 * ```JavaScript
 * var db = require('db');
 *
 * var conn = db.openMySQL('mysql://root:password@localhost/test');
 *
 * // call execute method to insert data
 * var res = conn.execute("insert into user(username, password) values ('testuser', '123456')");
 * console.log(res);
 *
 * // call execute method to query data
 * res = conn.execute("select * from user where username = 'testuser'");
 * console.log(res);
 *
 * conn.close();
 * ```
 * In the example above, we first use the db.openMySQL method to create a MySQL connection object and specify the connection information.
 * Then we use the execute method to add a new user to the user table prepared in advance, and afterwards we call the execute method to query the user record just created.
 * Finally we call the close method to close the connection object, completing our MySQL operations.
 *
 */
declare class Class_MySQL extends Class_DbConnection {
    /**
     * @description The receive buffer size of the database connection
     */
    rxBufferSize: number;

    /**
     * @description The send buffer size of the database connection
     */
    txBufferSize: number;

}

