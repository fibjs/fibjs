var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var db = require('db');
var os = require('os');
var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');

var vmid = coroutine.vmid;

var DBNAME = `test`;
var sql_server = {
    // for running test locally, uncomment configurations below and change it accordingly to your DB server.
    // mysql: {
    //     desc: '[mysql] sql db universal test',
    //     conn_str: `mysql://root@localhost/${DBNAME}`,
    // },
    // mssql: {
    //     desc: '[mssql] sql db universal test',
    //     conn_str: `mssql://sa@localhost/${DBNAME}`,
    // },
    // dm: {
    //     desc: '[dm] sql db universal test',
    //     conn_str: `dm://SYSDBA:123456789@localhost/${DBNAME}`,
    // },
}

if (process.env.FIBJS_TEST_PSQL) {
    sql_server.psql = {
        desc: '[psql] sql db universal test',
        conn_str: process.env.FIBJS_TEST_PSQL,
    };
}

describe("db", () => {
    function _test(type, conn_str) {
        var conn;
        var tables = ['test', 'test_null', 'test2', 'test3', 'test4'];

        var initDb = () => {
            switch (type) {
                case 'psql':
                    conn.execute(`SELECT 'CREATE DATABASE ${DBNAME}' WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = '${DBNAME}')`)
                    break;
                case 'mssql':
                case 'mysql':
                    conn.execute(`CREATE DATABASE IF NOT EXISTS \`${DBNAME}\``);
                    break;
                case 'dm':
                    try { conn.execute(`CREATE SCHEMA ${DBNAME}`); } catch (e) { }
                    break;
            }
        }

        before(() => {
            conn = db.open(conn_str);
            // conn.codec = 'gbk';
            initDb();

            tables.forEach(t => {
                try {
                    conn.execute('drop table ' + t);
                } catch (e) { }
            });
        });

        after(() => {
            tables.forEach(t => {
                try {
                    conn.execute('drop table ' + t);
                } catch (e) { }
            });

            conn.close();
        });

        it("empty sql args", () => {
            var rs = conn.execute(`select '?' as v`);
            assert.equal(rs[0].v, '?');
        });

        describe("escape", () => {
            it('null', () => {
                var rs = conn.execute('select ? as v', null);
                assert.isNull(rs[0].v);
            });

            it('string', () => {
                var res = [];
                for (var i = 1; i < 0x80; i++) {
                    var v = String.fromCharCode(i);
                    try {
                        var rs = conn.execute('select ? as v', v);
                        if (rs[0].v != v)
                            res.push(v);
                    } catch (e) {
                        res.push(v);
                    }
                }

                if (conn.type == 'psql' && process.platform === 'win32')
                    assert.deepEqual(res, ['\n']);
                else
                    assert.deepEqual(res, []);
            });

            it('binary', () => {
                var v = new Buffer('0123456789abcdef');
                var rs = conn.execute('select ? as v', v);
                assert.deepEqual(rs[0].v, v);
            });

            it('date', () => {
                var v = new Date(1580646020000);
                var rs = conn.execute('select ? as v', v);
                if (conn.type == 'SQLite')
                    assert.deepEqual(new Date(rs[0].v), v);
                else
                    assert.deepEqual(rs[0].v, v);
            });

            it('typed array', () => {
                var v = new Uint8Array([1, 2, 3]);
                var rs = conn.execute('select ? as v', v);
                assert.deepEqual(rs[0].v, Buffer.from(v));
            });

            it('array buffer', () => {
                const encodedData = new TextEncoder("utf-8").encode("hello world");
                const buffer = new ArrayBuffer(encodedData.length);
                const v = new Uint8Array(buffer);
                v.set(encodedData);

                var rs = conn.execute('select ? as v', buffer);
                const value = new TextDecoder("utf-8").decode(rs[0].v);
                assert.deepEqual(value, "hello world");
            });
        });

        it("create table", () => {
            if (conn.type == 'mssql')
                conn.execute('create table test(t0 INT IDENTITY PRIMARY KEY, t1 int, t2 nvarchar(128), t3 VARBINARY(100), t4 datetime);');
            else if (conn.type == 'dm') {
                conn.execute('create table test(t0 INT IDENTITY(1,1) PRIMARY KEY, t1 int, t2 varchar(128), t3 BLOB, t4 datetime);');
                conn.execute('create table test_null(t1 int NULL, t2 varchar(128) NULL, t3 BLOB NULL, t4 datetime NULL);');
            } else {
                if (conn.type == 'psql') {
                    conn.execute('create table test(t0 SERIAL PRIMARY KEY, t1 int, t2 varchar(128), t3 BYTEA, t4 timestamp);');
                    conn.execute('create table test_null(t1 int NULL, t2 varchar(128) NULL, t3 BYTEA NULL, t4 timestamp NULL);');
                }
                else {
                    conn.execute('create table test(t0 INTEGER AUTO_INCREMENT PRIMARY KEY, t1 int, t2 varchar(128), t3 BLOB, t4 datetime);');
                    conn.execute('create table test_null(t1 int NULL, t2 varchar(128) NULL, t3 BLOB NULL, t4 datetime NULL);');
                }
            }

            if (conn.type == 'mssql') {
                conn.execute('create table test2(t1 nvarchar(10), t2 nvarchar(10));');
                conn.execute('create table test3(t1 nvarchar(10), t2 nvarchar(10));');
            } else {
                conn.execute('create table test2(t1 varchar(10), t2 varchar(10));');
                conn.execute('create table test3(t1 varchar(10), t2 varchar(10));');
            }
        });


        it("insert", () => {
            var rs = conn.execute("insert into test(t1, t2, t3, t4) values(?,?,?,?);", 1123,
                '哈哈哈哈', new Buffer('DDDDDDDDDD'), new Date(
                    '1998-04-14 12:12:12'));

            if (conn.type == 'mysql' || conn.type == 'SQLite')
                assert.equal(rs.insertId, 1);

            if (conn.type != 'mssql') {
                conn.execute("insert into test_null values(?,?,?,?);", null,
                    undefined, null, undefined);
            }
        });

        it("select", () => {
            var rs = conn.execute('select * from test;');
            var r = rs[0];

            if (conn.txBufferSize) {
                console.log(conn.rxBufferSize, conn.txBufferSize);
                conn.txBufferSize = 16777220;
                console.log(conn.rxBufferSize, conn.txBufferSize);
            }

            assert.equal(typeof r['t1'], 'number');
            assert.equal(typeof r['t2'], 'string');
            assert.equal(typeof r['t3'], 'object');
            assert.equal(typeof r['t4'], 'object');

            assert.strictEqual(r['t1'], 1123);
            assert.strictEqual(r['t2'], '哈哈哈哈');
            assert.strictEqual(r['t3'].toString(), 'DDDDDDDDDD');
            assert.deepEqual(r['t4'], new Date('1998-04-14 12:12:12'));

            assert.deepEqual(Object.keys(r), [
                "t0",
                "t1",
                "t2",
                "t3",
                "t4"
            ]);

            if (conn.type != 'mssql') {
                rs = conn.execute('select t1,t2,t3,t4 from test_null')[0];
                assert.isNull(rs.t1);
                assert.isNull(rs.t2);
                assert.isNull(rs.t3);
                assert.isNull(rs.t4);
            }
        });

        it("large text", () => {
            var textType = 'TEXT';
            if (conn.type == 'mssql')
                textType = 'nvarchar(max)';
            else if (conn.type == 'mysql')
                textType = 'LONGTEXT';
            else if (conn.type == 'dm')
                textType = 'CLOB';

            try {
                conn.execute('drop table test4');
            } catch (e) { }

            conn.execute('create table test4(id int, content ' + textType + ')');

            var value = ('0123456789abcdef 中文内容 line\n').repeat(512);
            conn.execute('insert into test4(id, content) values(?, ?)', 1, value);

            var rs = conn.execute('select content from test4 where id = 1');
            assert.equal(rs.length, 1);
            assert.equal(rs[0].content.length, value.length);
            assert.equal(rs[0].content, value);
        });

        it("multi sql", () => {
            assert.deepEqual(conn.execute('select 100 as n'), [{
                n: 100
            }]);

            assert.deepEqual(conn.execute('select 100 as n;select 200 as n1, 300 as n2'), [
                [{
                    n: 100
                }],
                [{
                    n1: 200,
                    n2: 300
                }]
            ]);

            assert.deepEqual(conn.execute('select 100 as n;      '), [{
                n: 100
            }]);
        });

        it("execute async", (done) => {
            conn.execute("select * from test where t1=?", 1123, (e, rs) => {
                if (e)
                    return done(e);

                assert.equal(rs.length, 1);
                done();
            });
        });

        it("update/affected", () => {
            var rs = conn.execute("update test set t2='test101.1' where t1=1123");
            assert.equal(rs.affected, 1);
        });

        it("multilanguage", () => {
            const field_names = [
                "field",
                "字段",
                "フィールド",
                "필드"
            ];

            field_names.forEach((field_name) => {
                if (conn.type == 'mssql')
                    var rs = conn.execute(`select N'${field_name}' as "${field_name}";`);
                else
                    var rs = conn.execute(`select '${field_name}' as "${field_name}";`);
                assert.equal(rs[0][field_name], field_name);
            });
        });

        it("binary", () => {
            var b = Buffer.alloc(1);

            for (var i = 0; i < 256; i++) {
                b[0] = i;
                conn.execute("delete from test;");
                conn.execute("insert into test(t1, t2, t3, t4) values(1,'aa', ?, ?);",
                    b, new Date());
                var rs = conn.execute("select * from test;");
                assert.deepEqual(rs[0].t3, b);
            }
        });

        describe("transaction", () => {
            before(() => {
                try {
                    var b = Buffer.alloc(0);
                    conn.execute("insert into test(t1, t2, t3, t4) values(?,?,?,?);", 101, 'test101', b, new Date());
                } catch (e) { }
            });

            it("begin/commit", () => {
                conn.begin();
                conn.execute("update test set t2='test101.1' where t1=101");
                conn.commit();

                var rs = conn.execute("select * from test where t1=101");
                assert.equal(rs[0].t2, "test101.1");
            });

            describe("savepoint", () => {
                it("begin/commit", () => {
                    conn.begin();

                    conn.begin('p0');
                    conn.execute("update test set t2='test101.100' where t1=101");
                    conn.commit('p0');

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.100");

                    conn.rollback();

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.1");
                });

                it("begin/rollback", () => {
                    conn.begin();

                    conn.begin('p0');
                    conn.execute("update test set t2='test101.100' where t1=101");
                    conn.rollback('p0');

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.1");

                    conn.rollback();
                });

                it("multi trans", () => {
                    conn.begin();

                    conn.begin('p0');
                    conn.execute("update test set t2='test101.100' where t1=101");
                    conn.commit('p0');

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.100");

                    conn.begin('p0');
                    conn.execute("update test set t2='test101.200' where t1=101");
                    conn.rollback('p0');

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.100");

                    conn.begin('p0');
                    conn.execute("update test set t2='test101.300' where t1=101");
                    conn.commit('p0');

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.300");

                    conn.rollback();

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.1");
                });
            });

            it("begin/rollback", () => {
                conn.begin();
                conn.execute("update test set t2='test101.2' where t1=101");
                conn.rollback();

                var rs = conn.execute("select * from test where t1=101");
                assert.equal(rs[0].t2, "test101.1");
            });

            describe("trans()", () => {
                it("auto commit", () => {
                    var res = conn.trans(function (conn1) {
                        assert.equal(this, conn);
                        this.execute("update test set t2='test101.2' where t1=101");
                    });
                    assert.equal(res, true);


                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.2");
                });

                it("auto commit with fiber", () => {
                    var res = conn.trans(function (conn1) {
                        assert.equal(this, conn1);
                        assert.equal(this, conn);

                        coroutine.parallel(() => {
                            this.execute("update test set t2='test101.2.1' where t1=101");
                        });
                    });
                    assert.equal(res, true);

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.2.1");
                });

                it("auto rollback", () => {
                    var res = conn.trans(function (conn1) {
                        this.execute("update test set t2='test101.3' where t1=101");
                        return false;
                    });
                    assert.equal(res, false);

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.2.1");
                });

                it("rollback when throw", () => {
                    assert.throws(() => {
                        conn.trans(function (conn1) {
                            this.execute("update test set t2='test101.3' where t1=101");
                            throw 100;
                        });
                    });

                    var rs = conn.execute("select * from test where t1=101");
                    assert.equal(rs[0].t2, "test101.2.1");
                });

                it("trans savepoint", () => {
                    conn.trans(function (conn1) {
                        conn.trans("point", function (conn2) {
                            this.execute("update test set t2='test101.300' where t1=101");
                            return false;
                        });

                        var rs = conn.execute("select * from test where t1=101");
                        assert.equal(rs[0].t2, "test101.2.1");

                        return false;
                    });
                });
            });
        });

        it("execute bug", () => {
            var a = 0;
            coroutine.start(() => {
                a = 1;
            })
            for (var i = 0; i < 10000 && a == 0; i++)
                conn.execute("select count(*) from test");
            assert.equal(a, 1);
        });

        describe("schema operations", () => {
            it("getTables", () => {
                var tables = conn.getTables();
                assert.isArray(tables);
                assert.greaterThan(tables.length, 0);

                // Check that our test tables are in the list
                var tableNames = tables.map(t => t.name);
                assert.notEqual(tableNames.indexOf('test'), -1);
                assert.notEqual(tableNames.indexOf('test2'), -1);
                assert.notEqual(tableNames.indexOf('test3'), -1);

                if (conn.type != 'mssql') {
                    assert.notEqual(tableNames.indexOf('test_null'), -1);
                }
            });

            it("getTableInfo", () => {
                var columns = conn.getTableInfo('test');
                assert.isArray(columns);
                assert.greaterThan(columns.length, 0);

                // Check column names exist
                var columnNames = columns.map(c => c.column_name);
                assert.notEqual(columnNames.indexOf('t0'), -1);
                assert.notEqual(columnNames.indexOf('t1'), -1);
                assert.notEqual(columnNames.indexOf('t2'), -1);
                assert.notEqual(columnNames.indexOf('t3'), -1);
                assert.notEqual(columnNames.indexOf('t4'), -1);

                // Check each column has expected properties
                columns.forEach(col => {
                    assert.ok(col.hasOwnProperty('column_name'));
                    assert.ok(col.hasOwnProperty('data_type'));
                    assert.ok(col.hasOwnProperty('is_nullable'));

                    assert.equal(typeof col.column_name, 'string');
                    assert.equal(typeof col.data_type, 'string');
                    assert.equal(typeof col.is_nullable, 'string');
                });
            });

            it("getTableInfo for non-existent table", () => {
                var columns = conn.getTableInfo('non_existent_table');
                assert.isArray(columns);
                assert.equal(columns.length, 0);
            });
        });

        switch (type) {
            case 'mysql':
                // Test MySQL specific data types and return values
                it("MySQL data types", () => {
                    // Drop table if exists
                    try {
                        conn.execute('DROP TABLE IF EXISTS test_mysql_types');
                    } catch (e) {
                        // ignore
                    }

                    // Create table with different MySQL data types
                    conn.execute(`
                        CREATE TABLE test_mysql_types (
                            id INT PRIMARY KEY,
                            text_col TEXT,
                            longtext_col LONGTEXT,
                            blob_col BLOB,
                            binary_col BINARY(10),
                            varbinary_col VARBINARY(50),
                            varchar_col VARCHAR(255),
                            char_col CHAR(10),
                            tinytext_col TINYTEXT,
                            mediumtext_col MEDIUMTEXT,
                            tinyblob_col TINYBLOB,
                            mediumblob_col MEDIUMBLOB,
                            longblob_col LONGBLOB,
                            json_col JSON,
                            bit_col BIT(8),
                            boolean_col BOOLEAN,
                            date_col DATE,
                            time_col TIME,
                            datetime_col DATETIME,
                            timestamp_col TIMESTAMP,
                            year_col YEAR,
                            decimal_col DECIMAL(10,2),
                            float_col FLOAT,
                            double_col DOUBLE,
                            tinyint_col TINYINT,
                            smallint_col SMALLINT,
                            mediumint_col MEDIUMINT,
                            bigint_col BIGINT
                        )
                    `);

                    // Insert test data with various types
                    conn.execute(`
                        INSERT INTO test_mysql_types VALUES (
                            1, 
                            'This is text', 
                            'This is longtext',
                            ?,
                            ?,
                            ?,
                            'varchar value',
                            'char value',
                            'tinytext',
                            'mediumtext',
                            ?,
                            ?,
                            ?,
                            '{"key": "value"}',
                            b'10101010',
                            true,
                            '2023-01-01',
                            '12:30:45',
                            '2023-01-01 12:30:45',
                            '2023-01-01 12:30:45',
                            2023,
                            123.45,
                            3.14,
                            2.718,
                            127,
                            32767,
                            8388607,
                            9223372036854775807
                        )
                    `,
                        Buffer.from('blob data'),
                        Buffer.from('binary123'),
                        Buffer.from('varbinary data'),
                        Buffer.from('tinyblob'),
                        Buffer.from('mediumblob'),
                        Buffer.from('longblob data')
                    );

                    // Query the data and verify types
                    var result = conn.execute('SELECT * FROM test_mysql_types');
                    assert.equal(result.length, 1);

                    var row = result[0];

                    // Test string types - should return as string
                    assert.equal(typeof row.text_col, 'string');
                    assert.equal(row.text_col, 'This is text');
                    assert.equal(typeof row.longtext_col, 'string');
                    assert.equal(row.longtext_col, 'This is longtext');
                    assert.equal(typeof row.varchar_col, 'string');
                    assert.equal(row.varchar_col, 'varchar value');
                    assert.equal(typeof row.char_col, 'string');
                    assert.equal(row.char_col, 'char value');
                    assert.equal(typeof row.tinytext_col, 'string');
                    assert.equal(row.tinytext_col, 'tinytext');
                    assert.equal(typeof row.mediumtext_col, 'string');
                    assert.equal(row.mediumtext_col, 'mediumtext');

                    // Test binary types - should return as Buffer
                    assert.equal(typeof row.blob_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.blob_col));
                    assert.equal(row.blob_col.toString(), 'blob data');
                    assert.equal(typeof row.binary_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.binary_col));
                    assert.equal(typeof row.varbinary_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.varbinary_col));
                    assert.equal(row.varbinary_col.toString(), 'varbinary data');
                    assert.equal(typeof row.tinyblob_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.tinyblob_col));
                    assert.equal(row.tinyblob_col.toString(), 'tinyblob');
                    assert.equal(typeof row.mediumblob_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.mediumblob_col));
                    assert.equal(row.mediumblob_col.toString(), 'mediumblob');
                    assert.equal(typeof row.longblob_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.longblob_col));
                    assert.equal(row.longblob_col.toString(), 'longblob data');

                    // Test JSON type - should return as string
                    assert.equal(typeof row.json_col, 'string');
                    assert.equal(row.json_col, '{"key": "value"}');

                    // Test bit type - should return as Buffer
                    assert.equal(typeof row.bit_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.bit_col));

                    // Test boolean type - should return as number (0 or 1)
                    assert.equal(typeof row.boolean_col, 'number');
                    assert.equal(row.boolean_col, 1);

                    // Test date/time types - should return as Date objects
                    assert.equal(typeof row.date_col, 'object');
                    assert.isTrue(row.date_col instanceof Date);
                    assert.equal(typeof row.datetime_col, 'object');
                    assert.isTrue(row.datetime_col instanceof Date);
                    assert.equal(typeof row.timestamp_col, 'object');
                    assert.isTrue(row.timestamp_col instanceof Date);

                    assert.equal(typeof row.time_col, 'string');
                    assert.equal(row.time_col, '12:30:45');

                    // Test year type - should return as number
                    assert.equal(typeof row.year_col, 'number');
                    assert.equal(row.year_col, 2023);

                    // Test numeric types - should return as number
                    assert.equal(typeof row.decimal_col, 'number');
                    assert.equal(row.decimal_col, 123.45);
                    assert.equal(typeof row.float_col, 'number');
                    assert.ok(Math.abs(row.float_col - 3.14) < 0.01);
                    assert.equal(typeof row.double_col, 'number');
                    assert.ok(Math.abs(row.double_col - 2.718) < 0.001);
                    assert.equal(typeof row.tinyint_col, 'number');
                    assert.equal(row.tinyint_col, 127);
                    assert.equal(typeof row.smallint_col, 'number');
                    assert.equal(row.smallint_col, 32767);
                    assert.equal(typeof row.mediumint_col, 'number');
                    assert.equal(row.mediumint_col, 8388607);
                    assert.equal(typeof row.bigint_col, 'number');
                    assert.equal(row.bigint_col, 9223372036854775807);

                    // Test integer type - should return as number
                    assert.equal(typeof row.id, 'number');
                    assert.equal(row.id, 1);

                    // Clean up
                    conn.execute('DROP TABLE test_mysql_types');
                });
                break;
            case 'psql':
                // Test PostgreSQL specific data types and return values
                it("PostgreSQL data types", () => {
                    // Drop table if exists
                    try {
                        conn.execute('DROP TABLE IF EXISTS test_psql_types');
                    } catch (e) {
                        // ignore
                    }

                    // Create table with different PostgreSQL data types
                    conn.execute(`
                        CREATE TABLE test_psql_types (
                            id SERIAL PRIMARY KEY,
                            text_col TEXT,
                            varchar_col VARCHAR(255),
                            char_col CHAR(10),
                            bytea_col BYTEA,
                            json_col JSON,
                            jsonb_col JSONB,
                            boolean_col BOOLEAN,
                            date_col DATE,
                            time_col TIME,
                            timestamp_col TIMESTAMP,
                            timestamptz_col TIMESTAMPTZ,
                            interval_col INTERVAL,
                            numeric_col NUMERIC(10,2),
                            decimal_col DECIMAL(10,2),
                            real_col REAL,
                            double_col DOUBLE PRECISION,
                            smallint_col SMALLINT,
                            integer_col INTEGER,
                            bigint_col BIGINT,
                            uuid_col UUID,
                            inet_col INET,
                            macaddr_col MACADDR,
                            bit_col BIT(8),
                            varbit_col VARBIT(16)
                        )
                    `);

                    // Insert test data with various types
                    conn.execute(`
                        INSERT INTO test_psql_types (
                            text_col, varchar_col, char_col, bytea_col,
                            json_col, jsonb_col, boolean_col,
                            date_col, time_col, timestamp_col, timestamptz_col,
                            interval_col, numeric_col, decimal_col,
                            real_col, double_col, smallint_col, integer_col, bigint_col,
                            uuid_col, inet_col, macaddr_col, bit_col, varbit_col
                        ) VALUES (
                            ?, ?, ?, ?,
                            ?, ?, ?,
                            ?, ?, ?, ?,
                            ?, ?, ?,
                            ?, ?, ?, ?, ?,
                            ?, ?, ?, ?, ?
                        )
                    `,
                        'This is text',
                        'varchar value',
                        'char value',
                        Buffer.from('bytea data'),
                        '{"key": "value"}',
                        '{"key": "jsonb_value"}',
                        true,
                        '2023-01-01',
                        '12:30:45',
                        '2023-01-01 12:30:45',
                        '2023-01-01 12:30:45+00',
                        '1 year 2 months 3 days',
                        123.45,
                        678.90,
                        3.14,
                        2.718281828,
                        32767,
                        123456,
                        123456789012345,
                        '550e8400-e29b-41d4-a716-446655440000',
                        '192.168.1.1',
                        '08:00:2b:01:02:03',
                        '10101010',
                        '1010101010101010'
                    );

                    // Query the data and verify types
                    var result = conn.execute('SELECT * FROM test_psql_types');
                    assert.equal(result.length, 1);

                    var row = result[0];

                    // Test string types - should return as string
                    assert.equal(typeof row.text_col, 'string');
                    assert.equal(row.text_col, 'This is text');
                    assert.equal(typeof row.varchar_col, 'string');
                    assert.equal(row.varchar_col, 'varchar value');
                    assert.equal(typeof row.char_col, 'string');
                    assert.equal(row.char_col, 'char value');

                    // Test binary types - should return as Buffer
                    assert.equal(typeof row.bytea_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.bytea_col));
                    assert.equal(row.bytea_col.toString(), 'bytea data');

                    // Test JSON types - should return as string
                    assert.equal(typeof row.json_col, 'string');
                    assert.equal(row.json_col, '{"key": "value"}');
                    assert.equal(typeof row.jsonb_col, 'string');
                    assert.equal(row.jsonb_col, '{"key": "jsonb_value"}');

                    // Test boolean type - should return as boolean
                    assert.equal(typeof row.boolean_col, 'boolean');
                    assert.equal(row.boolean_col, true);

                    // Test date/time types - should return as Date objects or strings
                    assert.equal(typeof row.date_col, 'object');
                    assert.isTrue(row.date_col instanceof Date);
                    assert.equal(typeof row.timestamp_col, 'object');
                    assert.isTrue(row.timestamp_col instanceof Date);
                    assert.equal(typeof row.timestamptz_col, 'object');
                    assert.isTrue(row.timestamptz_col instanceof Date);

                    if (row.time_col instanceof Date)
                        assert.equal(typeof row.time_col, 'object');
                    else {
                        assert.equal(typeof row.time_col, 'string');
                        assert.equal(row.time_col, '12:30:45');
                    }
                    assert.equal(typeof row.interval_col, 'string');
                    assert.equal(row.interval_col, '1 year 2 mons 3 days');

                    // Test numeric types - should return as number
                    assert.equal(typeof row.numeric_col, 'number');
                    assert.equal(row.numeric_col, 123.45);
                    assert.equal(typeof row.decimal_col, 'number');
                    assert.equal(row.decimal_col, 678.90);
                    assert.equal(typeof row.real_col, 'number');
                    assert.ok(Math.abs(row.real_col - 3.14) < 0.01);
                    assert.equal(typeof row.double_col, 'number');
                    assert.ok(Math.abs(row.double_col - 2.718281828) < 0.000001);
                    assert.equal(typeof row.smallint_col, 'number');
                    assert.equal(row.smallint_col, 32767);
                    assert.equal(typeof row.integer_col, 'number');
                    assert.equal(row.integer_col, 123456);
                    assert.equal(typeof row.bigint_col, 'number');
                    assert.equal(row.bigint_col, 123456789012345);

                    // Test serial type - should return as number
                    assert.equal(typeof row.id, 'number');
                    assert.equal(row.id, 1);

                    // Test special PostgreSQL types
                    assert.equal(typeof row.uuid_col, 'string');
                    assert.equal(row.uuid_col.toLowerCase(), '550e8400-e29b-41d4-a716-446655440000');
                    assert.equal(typeof row.inet_col, 'string');
                    assert.equal(row.inet_col, '192.168.1.1');
                    assert.equal(typeof row.macaddr_col, 'string');
                    assert.equal(row.macaddr_col, '08:00:2b:01:02:03');

                    // Test bit types - should return as string
                    assert.equal(typeof row.bit_col, 'string');
                    assert.equal(row.bit_col, '10101010');
                    assert.equal(typeof row.varbit_col, 'string');
                    assert.equal(row.varbit_col, '1010101010101010');

                    // Clean up
                    conn.execute('DROP TABLE test_psql_types');
                });

                // some special types would not be parsed correctly with old implementation,
                // this case would failed in fibjs 0.34.0 with postgresql 14
                it('special type for psql, but also test for other dialect', () => {
                    var tables = conn.execute('select * from information_schema.tables;');
                    assert.greaterThan(tables, 0);

                    var columns = conn.execute('select * from information_schema.columns;');
                    assert.greaterThan(columns, 0);
                });
                break;
            case 'dm':
                it("DM data types", () => {
                    try {
                        conn.execute('DROP TABLE IF EXISTS test_dm_types');
                    } catch (e) { }

                    conn.execute(`
                        CREATE TABLE test_dm_types (
                            id INT IDENTITY(1,1) PRIMARY KEY,
                            text_col TEXT,
                            varchar_col VARCHAR(255),
                            char_col CHAR(10),
                            blob_col BLOB,
                            clob_col CLOB,
                            boolean_col BIT,
                            date_col DATE,
                            time_col TIME,
                            datetime_col DATETIME,
                            timestamp_col TIMESTAMP,
                            numeric_col NUMERIC(10,2),
                            decimal_col DECIMAL(10,2),
                            real_col REAL,
                            double_col DOUBLE,
                            smallint_col SMALLINT,
                            integer_col INTEGER,
                            bigint_col BIGINT,
                            binary_col BINARY(10),
                            varbinary_col VARBINARY(50),
                            tinyint_col TINYINT
                        )
                    `);

                    conn.execute(`
                        INSERT INTO test_dm_types (
                            text_col, varchar_col, char_col, blob_col, clob_col,
                            boolean_col, date_col, time_col, datetime_col, timestamp_col,
                            numeric_col, decimal_col, real_col, double_col,
                            smallint_col, integer_col, bigint_col,
                            binary_col, varbinary_col, tinyint_col
                        ) VALUES (
                            'This is text', 'varchar value', 'char value', ?, 'clob data',
                            1, '2023-01-01', '12:30:45', '2023-01-01 12:30:45', '2023-01-01 12:30:45',
                            123.45, 678.90, 3.14, 2.718281828,
                            32767, 123456, 123456789012345,
                            ?, ?, 127
                        )
                    `,
                        Buffer.from('blob data'),
                        Buffer.from('binary123\0'),
                        Buffer.from('varbinary data')
                    );

                    var result = conn.execute('SELECT * FROM test_dm_types');
                    assert.equal(result.length, 1);

                    var row = result[0];

                    // Test string types
                    assert.equal(typeof row.text_col, 'string');
                    assert.equal(row.text_col, 'This is text');
                    assert.equal(typeof row.varchar_col, 'string');
                    assert.equal(row.varchar_col, 'varchar value');
                    assert.equal(typeof row.char_col, 'string');
                    assert.equal(row.char_col, 'char value');
                    assert.equal(typeof row.clob_col, 'string');
                    assert.equal(row.clob_col, 'clob data');

                    // Test binary types
                    assert.equal(typeof row.blob_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.blob_col));
                    assert.equal(row.blob_col.toString(), 'blob data');
                    assert.equal(typeof row.binary_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.binary_col));
                    assert.equal(typeof row.varbinary_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.varbinary_col));
                    assert.equal(row.varbinary_col.toString(), 'varbinary data');

                    // Test boolean type
                    assert.equal(typeof row.boolean_col, 'boolean');
                    assert.equal(row.boolean_col, true);

                    // Test date/time types
                    assert.equal(typeof row.date_col, 'object');
                    assert.isTrue(row.date_col instanceof Date);
                    assert.equal(typeof row.time_col, 'object');
                    assert.isTrue(row.time_col instanceof Date);
                    assert.equal(typeof row.datetime_col, 'object');
                    assert.isTrue(row.datetime_col instanceof Date);
                    assert.equal(typeof row.timestamp_col, 'object');
                    assert.isTrue(row.timestamp_col instanceof Date);

                    // Test numeric types
                    assert.equal(typeof row.numeric_col, 'number');
                    assert.equal(row.numeric_col, 123.45);
                    assert.equal(typeof row.decimal_col, 'number');
                    assert.equal(row.decimal_col, 678.90);
                    assert.equal(typeof row.real_col, 'number');
                    assert.ok(Math.abs(row.real_col - 3.14) < 0.01);
                    assert.equal(typeof row.double_col, 'number');
                    assert.ok(Math.abs(row.double_col - 2.718281828) < 0.000001);
                    assert.equal(typeof row.smallint_col, 'number');
                    assert.equal(row.smallint_col, 32767);
                    assert.equal(typeof row.integer_col, 'number');
                    assert.equal(row.integer_col, 123456);
                    assert.equal(typeof row.bigint_col, 'number');
                    assert.equal(row.bigint_col, 123456789012345);
                    assert.equal(typeof row.tinyint_col, 'number');
                    assert.equal(row.tinyint_col, 127);

                    // Test serial type
                    assert.equal(typeof row.id, 'number');
                    assert.equal(row.id, 1);

                    conn.execute('DROP TABLE test_dm_types');
                });

                it("DM extra data types", () => {
                    try {
                        conn.execute('DROP TABLE IF EXISTS test_dm_extra');
                    } catch (e) { }

                    conn.execute(`
                        CREATE TABLE test_dm_extra (
                            id INT IDENTITY(1,1) PRIMARY KEY,
                            image_col IMAGE,
                            tstz_col TIMESTAMP WITH TIME ZONE,
                            timetz_col TIME WITH TIME ZONE,
                            iym_col INTERVAL YEAR(4) TO MONTH,
                            idt_col INTERVAL DAY(3) TO SECOND(2)
                        )
                    `);

                    conn.execute(`
                        INSERT INTO test_dm_extra (image_col, tstz_col, timetz_col, iym_col, idt_col)
                        VALUES (?, TIMESTAMP '2023-01-01 12:30:45 +08:00', TIME '12:30:45 +08:00',
                                INTERVAL '15-8' YEAR(4) TO MONTH, INTERVAL '100 10:20:30.12' DAY(3) TO SECOND(2))
                    `, Buffer.from('image binary data'));

                    var result = conn.execute('SELECT * FROM test_dm_extra');
                    assert.equal(result.length, 1);

                    var row = result[0];

                    // IMAGE - binary type
                    assert.equal(typeof row.image_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row.image_col));
                    assert.equal(row.image_col.toString(), 'image binary data');

                    // TIMESTAMP WITH TIME ZONE - string
                    assert.equal(typeof row.tstz_col, 'string');
                    assert.ok(row.tstz_col.indexOf('2023-01-01') >= 0);
                    assert.ok(row.tstz_col.indexOf('+08:00') >= 0);

                    // TIME WITH TIME ZONE - string
                    assert.equal(typeof row.timetz_col, 'string');
                    assert.ok(row.timetz_col.indexOf('12:30:45') >= 0);

                    // INTERVAL YEAR TO MONTH - string
                    assert.equal(typeof row.iym_col, 'string');
                    assert.ok(row.iym_col.indexOf('15-8') >= 0);

                    // INTERVAL DAY TO SECOND - string
                    assert.equal(typeof row.idt_col, 'string');
                    assert.ok(row.idt_col.indexOf('100') >= 0);
                    assert.ok(row.idt_col.indexOf('10:20:30') >= 0);

                    conn.execute('DROP TABLE test_dm_extra');
                });
                break;
            case 'sqlite':

                it("SQLite data types", () => {
                    var conn = db.open(conn_str);

                    // Drop table if exists
                    try {
                        conn.execute('DROP TABLE IF EXISTS test_sqlite_types');
                    } catch (e) {
                        // ignore
                    }

                    // Create table with different SQLite data types
                    // Note: SQLite has dynamic typing with 5 storage classes: NULL, INTEGER, REAL, TEXT, BLOB
                    conn.execute(`
                CREATE TABLE test_sqlite_types (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    text_col TEXT,
                    varchar_col VARCHAR(255),
                    char_col CHAR(10),
                    blob_col BLOB,
                    integer_col INTEGER,
                    real_col REAL,
                    numeric_col NUMERIC(10,2),
                    decimal_col DECIMAL(10,2),
                    boolean_col BOOLEAN,
                    date_col DATE,
                    time_col TIME,
                    datetime_col DATETIME,
                    timestamp_col TIMESTAMP,
                    json_col JSON
                )
            `);

                    // Insert test data with various types
                    conn.execute(`
                INSERT INTO test_sqlite_types (
                    text_col, varchar_col, char_col, blob_col,
                    integer_col, real_col, numeric_col, decimal_col,
                    boolean_col, date_col, time_col, datetime_col,
                    timestamp_col, json_col
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            `,
                        'This is text',
                        'varchar value',
                        'char value',
                        Buffer.from('blob data'),
                        123456,
                        3.14159,
                        123.45,
                        678.90,
                        true,
                        '2023-01-01',
                        '12:30:45',
                        '2023-01-01 12:30:45',
                        '2023-01-01 12:30:45',
                        '{"key": "value"}'
                    );

                    // Insert additional test data to verify type affinity
                    conn.execute(`
                INSERT INTO test_sqlite_types (
                    text_col, varchar_col, char_col, blob_col,
                    integer_col, real_col, numeric_col, decimal_col,
                    boolean_col, date_col, time_col, datetime_col,
                    timestamp_col, json_col
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            `,
                        123,  // number stored as TEXT
                        456.789,  // number stored as VARCHAR 
                        'abc',
                        new Uint8Array([1, 2, 3, 4]),  // typed array as BLOB
                        '789',  // string stored as INTEGER
                        '2.718',  // string stored as REAL
                        '999.99',
                        456.78,
                        0,  // false as BOOLEAN
                        new Date('2023-12-31'),  // Date object
                        new Date('1970-01-01 15:45:30'),  // Date object for TIME
                        new Date('2023-06-15 09:30:15'),  // Date object for DATETIME
                        new Date('2023-01-01 12:30:45'),  // Date object for TIMESTAMP
                        JSON.stringify({ test: 'data', number: 42 })
                    );

                    // Query the data and verify types
                    var result = conn.execute('SELECT * FROM test_sqlite_types ORDER BY id');
                    assert.equal(result.length, 2);

                    var row1 = result[0];
                    var row2 = result[1];

                    // Test TEXT affinity columns - should return as string
                    assert.equal(typeof row1.text_col, 'string');
                    assert.equal(row1.text_col, 'This is text');
                    assert.equal(typeof row1.varchar_col, 'string');
                    assert.equal(row1.varchar_col, 'varchar value');
                    assert.equal(typeof row1.char_col, 'string');
                    assert.equal(row1.char_col, 'char value');

                    // Numbers stored in TEXT columns should be converted to string
                    assert.equal(typeof row2.text_col, 'string');
                    assert.equal(row2.text_col, '123');
                    assert.equal(typeof row2.varchar_col, 'string');
                    assert.equal(row2.varchar_col, '456.789');

                    // Test BLOB affinity - should return as Buffer
                    assert.equal(typeof row1.blob_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row1.blob_col));
                    assert.equal(row1.blob_col.toString(), 'blob data');
                    assert.equal(typeof row2.blob_col, 'object');
                    assert.isTrue(Buffer.isBuffer(row2.blob_col));
                    assert.deepEqual(row2.blob_col, Buffer.from([1, 2, 3, 4]));

                    // Test INTEGER affinity - should return as number
                    assert.equal(typeof row1.integer_col, 'number');
                    assert.equal(row1.integer_col, 123456);
                    assert.equal(typeof row2.integer_col, 'number');
                    assert.equal(row2.integer_col, 789);  // string '789' converted to number

                    // Test REAL affinity - should return as number
                    assert.equal(typeof row1.real_col, 'number');
                    assert.ok(Math.abs(row1.real_col - 3.14159) < 0.00001);
                    assert.equal(typeof row2.real_col, 'number');
                    assert.ok(Math.abs(row2.real_col - 2.718) < 0.001);

                    // Test NUMERIC affinity - should return as number
                    assert.equal(typeof row1.numeric_col, 'number');
                    assert.equal(row1.numeric_col, 123.45);
                    assert.equal(typeof row1.decimal_col, 'number');
                    assert.equal(row1.decimal_col, 678.90);
                    assert.equal(typeof row2.numeric_col, 'number');
                    assert.equal(row2.numeric_col, 999.99);
                    assert.equal(typeof row2.decimal_col, 'number');
                    assert.equal(row2.decimal_col, 456.78);

                    // Test BOOLEAN - SQLite stores as INTEGER (0 or 1)
                    assert.equal(typeof row1.boolean_col, 'number');
                    assert.equal(row1.boolean_col, 1);  // true -> 1
                    assert.equal(typeof row2.boolean_col, 'number');
                    assert.equal(row2.boolean_col, 0);  // false -> 0

                    // Test date/time types - SQLite parses declared date/time types as Date objects
                    // String dates in DATE/DATETIME/TIMESTAMP/TIME columns should return as Date objects
                    assert.equal(typeof row1.date_col, 'object');
                    assert.isTrue(row1.date_col instanceof Date);
                    assert.equal(typeof row1.datetime_col, 'object');
                    assert.isTrue(row1.datetime_col instanceof Date);
                    assert.equal(typeof row1.timestamp_col, 'object');
                    assert.isTrue(row1.timestamp_col instanceof Date);

                    // TIME columns should also be parsed as Date objects
                    assert.equal(typeof row1.time_col, 'object');
                    assert.isTrue(row1.time_col instanceof Date);

                    // Date objects should be converted to Date objects as well
                    assert.equal(typeof row2.date_col, 'object');
                    assert.isTrue(row2.date_col instanceof Date);
                    assert.equal(typeof row2.time_col, 'object');
                    assert.isTrue(row2.time_col instanceof Date);
                    assert.equal(typeof row2.datetime_col, 'object');
                    assert.isTrue(row2.datetime_col instanceof Date);
                    assert.equal(typeof row2.timestamp_col, 'object');
                    assert.isTrue(row2.timestamp_col instanceof Date);

                    // Test JSON - stored as TEXT in SQLite
                    assert.equal(typeof row1.json_col, 'string');
                    assert.equal(row1.json_col, '{"key": "value"}');
                    assert.equal(typeof row2.json_col, 'string');
                    assert.equal(row2.json_col, '{"test":"data","number":42}');

                    // Test PRIMARY KEY AUTOINCREMENT
                    assert.equal(typeof row1.id, 'number');
                    assert.equal(row1.id, 1);
                    assert.equal(typeof row2.id, 'number');
                    assert.equal(row2.id, 2);

                    // Test SQLite's type affinity with mixed data types in same column
                    conn.execute(`
                INSERT INTO test_sqlite_types (text_col, integer_col, real_col) 
                VALUES (?, ?, ?)
            `, null, null, null);

                    var nullResult = conn.execute('SELECT * FROM test_sqlite_types WHERE id = 3');
                    assert.equal(nullResult.length, 1);
                    var nullRow = nullResult[0];

                    // NULL values should return as null
                    assert.isNull(nullRow.text_col);
                    assert.isNull(nullRow.integer_col);
                    assert.isNull(nullRow.real_col);

                    // Test type coercion - SQLite is very flexible
                    conn.execute(`
                INSERT INTO test_sqlite_types (integer_col, real_col, text_col) 
                VALUES (?, ?, ?)
            `, 3.14, 'string_in_real', 999);

                    var coercionResult = conn.execute('SELECT * FROM test_sqlite_types WHERE id = 4');
                    var coercionRow = coercionResult[0];

                    // 3.14 in INTEGER column - SQLite keeps it as REAL (doesn't truncate)
                    assert.equal(typeof coercionRow.integer_col, 'number');
                    assert.equal(coercionRow.integer_col, 3.14);

                    // String in REAL column should be stored as TEXT (if not numeric)
                    assert.equal(typeof coercionRow.real_col, 'string');
                    assert.equal(coercionRow.real_col, 'string_in_real');

                    // Number in TEXT column should be stored as TEXT
                    assert.equal(typeof coercionRow.text_col, 'string');
                    assert.equal(coercionRow.text_col, '999');

                    // Clean up
                    conn.execute('DROP TABLE test_sqlite_types');
                    conn.close();
                });
        }
    }

    describe("sqlite", () => {
        var conn_str = 'sqlite:' + path.join(__dirname, 'test.db' + vmid);
        after(() => {
            try {
                fs.unlink(path.join(__dirname, "test.db" + vmid));
            } catch (e) { }
            try {
                fs.unlink(path.join(__dirname, "test.db" + vmid + ".backup"));
            } catch (e) { }
        });
        _test('sqlite', conn_str);

        it("check synchronous mode", () => {
            var conn = db.open(conn_str);
            var synchronous = conn.execute("PRAGMA synchronous;")[0].synchronous;
            conn.close();

            assert.equal(synchronous, 1);
        });

        it("check journal mode", () => {
            var conn = db.open(conn_str);
            var journal_mode = conn.execute("PRAGMA journal_mode;")[0].journal_mode;
            conn.close();
            assert.equal(journal_mode, "wal");
        });

        it("fulltext search fts5", () => {
            var conn = db.open(conn_str);

            conn.execute('CREATE VIRTUAL TABLE email USING fts5(sender, title, body)');
            conn.execute("insert into email(sender, title, body) values('tom cat', 'some titles', 'this is body')");
            conn.execute("insert into email(sender, title, body) values('响马', '这里是标题yes', 'this这里是身体')");

            // 基础 MATCH 查询
            var rs = conn.execute("SELECT * FROM email WHERE email MATCH 'body'");
            assert.equal(rs.length, 1);
            assert.equal(rs[0].sender, "tom cat");

            // 字段限定查询
            var rs2 = conn.execute("SELECT * FROM email WHERE email MATCH 'sender:tom'");
            assert.equal(rs2.length, 1);
            assert.equal(rs2[0].sender, "tom cat");

            // 短语查询(用引号包裹成 phrase)
            var rs3 = conn.execute('SELECT * FROM email WHERE email MATCH \'"tom cat"\'');
            assert.equal(rs3.length, 1);
            assert.equal(rs3[0].sender, "tom cat");

            // 前缀查询
            var rs4 = conn.execute("SELECT * FROM email WHERE email MATCH 'ti*'");
            assert.equal(rs4.length, 1);
            assert.equal(rs4[0].title, "some titles");

            // 布尔查询
            var rs5 = conn.execute("SELECT * FROM email WHERE email MATCH 'tom OR 响马'");
            assert.equal(rs5.length, 2);

            // 排序: bm25
            var rs6 = conn.execute("SELECT sender, bm25(email) AS rank FROM email WHERE email MATCH 'body OR 响马' ORDER BY rank");
            assert.equal(rs6.length, 2);

            // 高亮与摘要
            var rs7 = conn.execute("SELECT highlight(email, 1, '[', ']') AS h FROM email WHERE email MATCH 'titles'");
            assert.equal(rs7.length, 1);
            assert.equal(rs7[0].h, "some [titles]");

            // snippet
            var rs8 = conn.execute("SELECT snippet(email, 2, '[', ']', '...', 5) AS s FROM email WHERE email MATCH 'body'");
            assert.equal(rs8.length, 1);
            assert.ok(rs8[0].s.indexOf('[body]') >= 0 || rs8[0].s.indexOf('body') >= 0);

            // rowid 查询
            conn.execute("DELETE FROM email WHERE rowid = 1");
            conn.execute("insert into email(sender, title, body, rowid) values('alice', 'hello', 'world', 100)");
            var rs9 = conn.execute('SELECT sender FROM email WHERE rowid = 100');
            assert.equal(rs9.length, 1);
            assert.equal(rs9[0].sender, "alice");

            // 删除后查询
            var rs10 = conn.execute('SELECT * FROM email');
            assert.equal(rs10.length, 2);

            // 修改
            conn.execute("UPDATE email SET sender='bob' WHERE rowid=100");
            var rs11 = conn.execute('SELECT sender FROM email WHERE rowid=100');
            assert.equal(rs11[0].sender, "bob");

            conn.close();
        });

        it("fts5 tokenize unicode61", () => {
            var conn = db.open(conn_str);

            // unicode61 默认 tokenizer
            conn.execute("CREATE VIRTUAL TABLE ft_u61 USING fts5(content, tokenize = 'unicode61')");
            conn.execute("insert into ft_u61(content) values('Hello World FIBJS')");
            conn.execute("insert into ft_u61(content) values('另一个世界')");

            // 大小写不敏感
            var rs = conn.execute("SELECT * FROM ft_u61 WHERE ft_u61 MATCH 'hello'");
            assert.equal(rs.length, 1);
            assert.equal(rs[0].content, "Hello World FIBJS");

            var rs2 = conn.execute("SELECT * FROM ft_u61 WHERE ft_u61 MATCH 'FIBJS'");
            assert.equal(rs2.length, 1);

            conn.close();
        });

        it("fts5 tokenize porter", () => {
            var conn = db.open(conn_str);

            // porter tokenizer(基于 unicode61 的词干提取)
            conn.execute("CREATE VIRTUAL TABLE ft_porter USING fts5(content, tokenize = 'porter')");
            conn.execute("insert into ft_porter(content) values('running runners ran')");
            conn.execute("insert into ft_porter(content) values('quickly runs the race')");

            // 词干提取: run 可以匹配 running/runners/runs
            var rs = conn.execute("SELECT * FROM ft_porter WHERE ft_porter MATCH 'run'");
            assert.equal(rs.length, 2);

            conn.close();
        });

        it("fts5 tokenize ascii", () => {
            var conn = db.open(conn_str);

            // ascii tokenizer
            conn.execute("CREATE VIRTUAL TABLE ft_ascii USING fts5(content, tokenize = 'ascii')");
            conn.execute("insert into ft_ascii(content) values('hello world')");
            conn.execute("insert into ft_ascii(content) values('case insensitive search')");

            var rs = conn.execute("SELECT * FROM ft_ascii WHERE ft_ascii MATCH 'HELLO'");
            assert.equal(rs.length, 1);
            assert.equal(rs[0].content, "hello world");

            conn.close();
        });

        it("fts5 multi-table join", () => {
            var conn = db.open(conn_str);

            conn.execute('CREATE VIRTUAL TABLE docs USING fts5(title, body)');
            conn.execute('CREATE TABLE docs_meta(id INTEGER PRIMARY KEY, author TEXT)');

            conn.execute("insert into docs(title, body, rowid) values('fts5 guide', 'introduction to full text search', 1)");
            conn.execute("insert into docs(title, body, rowid) values('advanced sql', 'complex queries and joins', 2)");
            conn.execute("insert into docs_meta(id, author) values(1, 'alice')");
            conn.execute("insert into docs_meta(id, author) values(2, 'bob')");

            // join fts5 表与普通表(MATCH 左操作数必须用表名而非别名)
            var rs = conn.execute(
                "SELECT docs.title, m.author FROM docs JOIN docs_meta AS m ON docs.rowid = m.id WHERE docs MATCH 'search' ORDER BY m.author"
            );
            assert.equal(rs.length, 1);
            assert.equal(rs[0].title, "fts5 guide");
            assert.equal(rs[0].author, "alice");

            conn.close();
        });

        it("fts5 rank function", () => {
            var conn = db.open(conn_str);

            conn.execute('CREATE VIRTUAL TABLE ft_rank USING fts5(content)');
            conn.execute("insert into ft_rank(content) values('the quick brown fox')");
            conn.execute("insert into ft_rank(content) values('the lazy dog')");
            conn.execute("insert into ft_rank(content) values('the fox and the hound')");

            // 按 bm25 排序
            var rs = conn.execute(
                "SELECT content, bm25(ft_rank) AS rank FROM ft_rank WHERE ft_rank MATCH 'fox' ORDER BY rank"
            );
            assert.equal(rs.length, 2);

            // rank 列(默认排序权重)
            var rs2 = conn.execute(
                "SELECT content, rank FROM ft_rank WHERE ft_rank MATCH 'the' ORDER BY rank"
            );
            assert.equal(rs2.length, 3);

            conn.close();
        });

        it("fts5 special syntax", () => {
            var conn = db.open(conn_str);

            conn.execute('CREATE VIRTUAL TABLE ft_syn USING fts5(content)');
            conn.execute("insert into ft_syn(content) values('apple banana cherry')");

            // NEAR 查询(语法:NEAR(phrase1 phrase2 [, N]))
            var rs = conn.execute("SELECT * FROM ft_syn WHERE ft_syn MATCH 'NEAR(apple banana)'");
            assert.equal(rs.length, 1);

            // NEAR 带距离参数
            var rs2 = conn.execute("SELECT * FROM ft_syn WHERE ft_syn MATCH 'NEAR(apple banana, 5)'");
            assert.equal(rs2.length, 1);

            // 列过滤器(应用于整个表)
            conn.execute("insert into ft_syn(content) values('banana apple grape')");

            // 按相关度排序
            var rs3 = conn.execute("SELECT content, bm25(ft_syn) AS r FROM ft_syn WHERE ft_syn MATCH 'apple' ORDER BY r");
            assert.equal(rs3.length, 2);

            // 事务支持
            conn.execute('BEGIN TRANSACTION');
            conn.execute("insert into ft_syn(content) values('in transaction')");
            conn.execute('COMMIT');

            var rs4 = conn.execute("SELECT * FROM ft_syn WHERE ft_syn MATCH 'transaction'");
            assert.equal(rs4.length, 1);

            conn.close();
        });

        it("fts5 contentless table", () => {
            var conn = db.open(conn_str);

            // contentless 表:不存储原文,节省空间
            conn.execute("CREATE VIRTUAL TABLE ft_c USING fts5(content, content='')");
            conn.execute("insert into ft_c(rowid, content) values(1, 'hello world')");
            conn.execute("insert into ft_c(rowid, content) values(2, 'fibjs sqlite fts5')");

            // 可正常 MATCH 查询并返回 rowid
            var rs = conn.execute("SELECT rowid FROM ft_c WHERE ft_c MATCH 'hello'");
            assert.equal(rs.length, 1);
            assert.equal(rs[0].rowid, 1);

            // contentless 表无原文,SELECT content 返回 NULL
            var rs2 = conn.execute("SELECT content FROM ft_c WHERE ft_c MATCH 'hello'");
            assert.equal(rs2.length, 1);
            assert.isNull(rs2[0].content);

            conn.close();
        });

        it("fts5 external content table", () => {
            var conn = db.open(conn_str);

            conn.execute('CREATE TABLE articles(id INTEGER PRIMARY KEY, title TEXT, body TEXT)');
            conn.execute("insert into articles(id, title, body) values(1, 'fts5 guide', 'introduction to fts5')");
            conn.execute("insert into articles(id, title, body) values(2, 'advanced sql', 'joins and subqueries')");

            // external content 表:索引指向外部表
            conn.execute("CREATE VIRTUAL TABLE ft_articles USING fts5(title, body, content='articles', content_rowid='id')");
            conn.execute("insert into ft_articles(rowid, title, body) select id, title, body from articles");

            // 通过 fts5 查询,取得外部表内容
            var rs = conn.execute("SELECT ft_articles.rowid, title, body FROM ft_articles WHERE ft_articles MATCH 'fts5'");
            assert.equal(rs.length, 1);
            assert.equal(rs[0].title, "fts5 guide");
            assert.equal(rs[0].body, "introduction to fts5");

            conn.close();
        });

        it("backup", () => {
            var conn = db.open(conn_str);
            conn.backup(conn_str + ".backup");

            var conn1 = db.open(conn_str + ".backup");
            var rs1 = conn1.execute("select * from test");
            var rs = conn.execute("select * from test");

            assert.equal(rs[0].t1, rs1[0].t1);

            conn.close();
            conn1.close();
        });
    });

    // if (global.full_test)
    for (var type in sql_server)
        describe(sql_server[type].desc, () => {
            _test(type, sql_server[type].conn_str);
        });

    describe("leveldb", () => {
        after(clear_db);

        function clear_db() {
            try {
                fs.readdir(path.join(__dirname, "testdb" + vmid)).forEach((s) => {
                    fs.unlink(path.join(__dirname, "testdb" + vmid, s));
                });

                fs.rmdir(path.join(__dirname, "testdb" + vmid));
            } catch (e) { };
        }

        it('open/close', () => {
            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.close();
            clear_db();
        });

        it('set/get', () => {
            var b = "bbbbb";
            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.set("test", b);
            assert.equal(ldb.get("test").toString(), "bbbbb");
            ldb.close();
            clear_db();
        });

        it('binary Key', () => {
            var b = "bbbbb1";
            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.set("test1", b);
            assert.equal(ldb.get("test1").toString(), "bbbbb1");
            ldb.close();
            clear_db();
        });

        it('multi set/get', () => {
            var data = {
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ccc": "ccc value",
                "ddd": "ddd value"
            };

            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.mset(data);
            assert.equal(ldb.get("aaa").toString(), "aaa value");
            assert.equal(ldb.get("bbb").toString(), "bbb value");
            assert.equal(ldb.get("ccc").toString(), "ccc value");
            assert.equal(ldb.get("ddd").toString(), "ddd value");

            assert.deepEqual(ldb.mget(['aaa', 'ccc', 'eee']), [
                new Buffer('aaa value'), new Buffer('ccc value'), null
            ]);

            ldb.close();
            clear_db();
        });

        it('remove/has', () => {
            var b = "bbbbb";
            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            assert.isNull(ldb.get("not_exists"));
            assert.isFalse(ldb.has("not_exists"));
            ldb.set("not_exists", b);
            assert.isTrue(ldb.has("not_exists"));
            ldb.remove("not_exists");
            assert.isFalse(ldb.has("not_exists"));

            ldb.set("not_exists", b);
            assert.isTrue(ldb.has("not_exists"));
            ldb.remove("not_exists");
            assert.isFalse(ldb.has("not_exists"));
            ldb.close();
            clear_db();
        });

        it('batch remove', () => {
            var data = {
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ccc": "ccc value",
                "ddd": "ddd value"
            };

            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.mset(data);

            ldb.mremove(["bbb", "ddd"]);

            assert.equal(ldb.get("aaa").toString(), "aaa value");
            assert.isNull(ldb.get("bbb"));
            assert.equal(ldb.get("ccc").toString(), "ccc value");
            assert.isNull(ldb.get("ddd"));

            ldb.close();
            clear_db();
        });

        it('begin/commit', () => {
            var b = "bbbbb";
            var c = "ccccc";
            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

            ldb.set("test", b);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            var tr = ldb.begin();

            tr.set("test", c);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            tr.commit();

            assert.equal(ldb.get("test").toString(), "ccccc");

            ldb.close();
            clear_db();
        });

        it('begin/close', () => {
            var b = "bbbbb";
            var c = "ccccc";
            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

            ldb.set("test", b);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            var tr = ldb.begin();

            tr.set("test", c);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            tr.close();

            assert.equal(ldb.get("test").toString(), "bbbbb");

            ldb.close();
            clear_db();
        });

        it('firstKey', () => {
            var data = {
                "ccc": "ccc value",
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ddd": "ddd value"
            };

            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

            var count = 0;
            ldb.forEach((v, k) => {
                count++;
            });
            assert.equal(count, 0);

            ldb.mset(data);

            assert.equal(ldb.firstKey().toString(), "aaa");

            ldb.close();
            clear_db();
        });

        it('lastKey', () => {
            var data = {
                "ccc": "ccc value",
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ddd": "ddd value"
            };

            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

            var count = 0;
            ldb.forEach((v, k) => {
                count++;
            });
            assert.equal(count, 0);

            ldb.mset(data);

            assert.equal(ldb.lastKey().toString(), "ddd");

            ldb.close();
            clear_db();
        });

        describe('forEach', () => {
            it('forEach()', () => {
                var data = {
                    "ccc": "ccc value",
                    "aaa": "aaa value",
                    "bbb": "bbb value",
                    "ddd": "ddd value"
                };

                var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

                var count = 0;
                ldb.forEach((v, k) => {
                    count++;
                });
                assert.equal(count, 0);

                ldb.mset(data);

                const keys = [];
                const values = [];
                ldb.forEach((v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['aaa', 'bbb', 'ccc', 'ddd']);
                assert.deepEqual(values, ['aaa value', 'bbb value', 'ccc value', 'ddd value']);

                ldb.close();
                clear_db();
            });

            it('forEach(from)', () => {
                var data = {
                    "ccc": "ccc value",
                    "aaa": "aaa value",
                    "bbb": "bbb value",
                    "ddd": "ddd value"
                };

                var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

                var count = 0;
                ldb.forEach((v, k) => {
                    count++;
                });
                assert.equal(count, 0);

                ldb.mset(data);

                var keys = [];
                var values = [];
                ldb.forEach("aab", (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['bbb', 'ccc', 'ddd']);
                assert.deepEqual(values, ['bbb value', 'ccc value', 'ddd value']);

                var keys = [];
                var values = [];
                ldb.forEach("bbb", (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['bbb', 'ccc', 'ddd']);
                assert.deepEqual(values, ['bbb value', 'ccc value', 'ddd value']);

                ldb.close();
                clear_db();
            });

            it('forEach(from, to)', () => {
                var data = {
                    "ccc": "ccc value",
                    "aaa": "aaa value",
                    "bbb": "bbb value",
                    "ddd": "ddd value"
                };

                var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

                var count = 0;
                ldb.forEach((v, k) => {
                    count++;
                });
                assert.equal(count, 0);

                ldb.mset(data);

                var keys = [];
                var values = [];
                ldb.forEach("aab", "ddd", (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['bbb', 'ccc']);
                assert.deepEqual(values, ['bbb value', 'ccc value']);

                var keys = [];
                var values = [];
                ldb.forEach("bbb", "ddc", (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['bbb', 'ccc']);
                assert.deepEqual(values, ['bbb value', 'ccc value']);

                ldb.close();
                clear_db();
            });

            describe('forEach(opt)', () => {
                it('skip', () => {
                    var data = {
                        "ccc": "ccc value",
                        "aaa": "aaa value",
                        "bbb": "bbb value",
                        "ddd": "ddd value"
                    };

                    var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

                    var count = 0;
                    ldb.forEach((v, k) => {
                        count++;
                    });
                    assert.equal(count, 0);

                    ldb.mset(data);

                    var keys = [];
                    var values = [];
                    ldb.forEach({ skip: 1 }, (v, k) => {
                        keys.push(k.toString());
                        values.push(v.toString());
                    });

                    assert.deepEqual(keys, ['bbb', 'ccc', 'ddd']);
                    assert.deepEqual(values, ['bbb value', 'ccc value', 'ddd value']);

                    ldb.close();
                    clear_db();
                });
            });

            it('limit', () => {
                var data = {
                    "ccc": "ccc value",
                    "aaa": "aaa value",
                    "bbb": "bbb value",
                    "ddd": "ddd value"
                };

                var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

                var count = 0;
                ldb.forEach((v, k) => {
                    count++;
                });
                assert.equal(count, 0);

                ldb.mset(data);

                var keys = [];
                var values = [];
                ldb.forEach({ limit: 2 }, (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['aaa', 'bbb']);
                assert.deepEqual(values, ['aaa value', 'bbb value']);

                ldb.close();
                clear_db();
            });

            it('reverse', () => {
                var data = {
                    "ccc": "ccc value",
                    "aaa": "aaa value",
                    "bbb": "bbb value",
                    "ddd": "ddd value"
                };

                var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));

                var count = 0;
                ldb.forEach((v, k) => {
                    count++;
                });
                assert.equal(count, 0);

                ldb.mset(data);

                var keys = [];
                var values = [];
                ldb.forEach({ reverse: true }, (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['ddd', 'ccc', 'bbb', 'aaa']);
                assert.deepEqual(values, ['ddd value', 'ccc value', 'bbb value', 'aaa value']);

                var keys = [];
                var values = [];
                ldb.forEach("ccc", { reverse: true }, (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['ccc', 'bbb', 'aaa']);
                assert.deepEqual(values, ['ccc value', 'bbb value', 'aaa value']);

                var keys = [];
                var values = [];
                ldb.forEach("ccd", { reverse: true }, (v, k) => {
                    keys.push(k.toString());
                    values.push(v.toString());
                });

                assert.deepEqual(keys, ['ccc', 'bbb', 'aaa']);
                assert.deepEqual(values, ['ccc value', 'bbb value', 'aaa value']);
                ldb.close();
                clear_db();
            });
        });

        it('break', () => {
            var data = {
                "ccc": "ccc value",
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ddd": "ddd value"
            };

            var data1 = {
                "aaa": "aaa value",
                "bbb": "bbb value"
            };

            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.mset(data);

            var count = 0;
            ldb.forEach((v, k) => {
                assert.equal(data1[k], v.toString());
                delete data1[k];
                count++;
                if (count == 2)
                    return true;
            });
            assert.equal(count, 2);

            ldb.close();
            clear_db();
        });

        it("FIX: levelDB.mget results in a segmentation fault", () => {
            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.close();
            assert.throws(() => {
                levelDB_AF = ldb.begin();
            })

            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            levelDB_AF = ldb.begin();
            ldb.close();
            assert.throws(() => {
                levelDB_AF.mget(['str']);
            });

            ldb.close();
            clear_db();
        })
    });
});


