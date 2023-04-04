/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DbConnection.d.ts" />
/**
 * @description SQLite 对象是内置模块 db 的一个成员，主要负责 SQLite 数据库的连接和操作，可用于 SQLite 数据库的创建、查询、插入、更新等操作。同时，SQLite 对象也提供了一些类似备份、格式化 SQL 等高级操作。SQLite 连接对象还支持事务操作
 * 
 * 在实际应用中，我们通常根据业务需求创建 SQLite 类型的数据表，然后进行数据的增删改查等操作，例如：
 * 
 * ```JavaScript
 * var db = require('db')
 * 
 * // 根据以上代码示例中的代码，获取一个 SQLite 数据库连接对象
 * var sqlite = db.openSQLite('test.db')
 * 
 * // 创建表
 * sqlite.execute('CREATE TABLE test (id INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, age INT NOT NULL)')
 * 
 * // 插入数据
 * sqlite.execute('INSERT INTO test (id, name, age) VALUES (?, ?, ?)', 1, 'Alice', 18)
 * sqlite.execute('INSERT INTO test (id, name, age) VALUES (?, ?, ?)', 2, 'Bob', 20)
 * sqlite.execute('INSERT INTO test (id, name, age) VALUES (?, ?, ?)', 3, 'Charlie', 22)
 * 
 * // 查询数据
 * var rs = sqlite.execute('SELECT * FROM test')
 * console.log(rs)
 * 
 * // 更新数据
 * sqlite.execute('UPDATE test SET name=?, age=? WHERE id=?', 'Marry', 19, 1)
 * 
 * // 删除数据
 * sqlite.execute('DELETE FROM test WHERE id=?', 2)
 * ```
 * 
 * SQLite 还内置了 vec_index 模块，我们可以在 SQLite 数据库上创建对向量字段的索引，基于向量字段进行检索，得到与目标向量最近似的向量集合。支持使用数值类型的数组表示向量，如：[1, 2, 3]，同时支持向量维度。此外，vec_index 支持在事务内批量操作。
 * 
 * 下面是一个简单的示例：
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
 * 可以使用 vec_search 函数执行向量检索，例如：
 * 
 * ``` JavaScript
 * var key = [1, 2, 5.1234];
 * var limit = 1;
 * 
 * var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "${JSON.stringify(key)}")`);
 * ``` 
 * 
 * vec_search 返回一个最接近的向量集合和距离数组，其中距离按照从小到大顺序排列。如果需要返回多个最接近的向量集合，可以使用 :limit 参数，例如：
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
     * @description 当前数据库文件名 
     */
    readonly fileName: string;

    /**
     * @description 查询和设置数据库超时时间，以毫秒为单位 
     */
    timeout: number;

    /**
     * @description 备份当前数据库到新文件
     * 	 @param fileName 指定备份的数据库文件名 
     */
    backup(fileName: string): void;

    backup(fileName: string, callback: (err: Error | undefined | null)=>any): void;

}

