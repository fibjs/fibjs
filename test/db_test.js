var test = require("test");
test.setup();

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
    //     conn_str: `mysql://root@localhost:3306/${DBNAME}`,
    // },
    // psql: {
    //     desc: '[psql] sql db universal test',
    //     conn_str: `psql://postgres@localhost:5432/${DBNAME}`,
    // },
    // mssql: {
    //     desc: '[mssql] sql db universal test',
    //     conn_str: `mssql://sa@192.168.67.63/${DBNAME}`,
    // },
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

        describe("format", () => {
            it("basic", () => {
                assert.equal(conn.format("test?", [1, 2, 3, 4]), "test(1,2,3,4)");
                assert.equal(conn.format("test?", [1, [2, 3], 4]), "test(1,(2,3),4)");
                assert.equal(conn.format("test?"), "test?");
            });

            function sql_equal(conn, sql1, sql2) {
                if (conn.type == 'mssql') {
                    var cnt = 0;
                    sql2 = sql2.replace(/`/g, (s, i) => {
                        cnt++;
                        return (cnt % 2) ? '[' : ']';
                    });
                    sql2 = sql2.replace(/\]\[/g, '`');
                    assert.equal(sql1, sql2);
                } else if (conn.type == 'psql') {
                    var cnt = 0;
                    sql2 = sql2.replace(/`/g, '"');
                    assert.equal(sql1, sql2);
                } else {
                    assert.equal(sql1, sql2);
                }
            }

            describe("format.find", () => {
                it('basic', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test"
                    }), "SELECT * FROM `test`");
                });

                it('fields', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        fields: ["a", "b", "c"]
                    }), "SELECT `a`, `b`, `c` FROM `test`");

                    if (conn.type == 'mssql') {
                        sql_equal(conn, conn.format("find", {
                            table: "test",
                            fields: "[a], [b], [c]"
                        }), "SELECT `a`, `b`, `c` FROM `test`");
                    } else if (conn.type == 'psql') {
                        sql_equal(conn, conn.format("find", {
                            table: "test",
                            fields: '"a", "b", "c"'
                        }), "SELECT `a`, `b`, `c` FROM `test`");
                    } else {
                        sql_equal(conn, conn.format("find", {
                            table: "test",
                            fields: "`a`, `b`, `c`"
                        }), "SELECT `a`, `b`, `c` FROM `test`");
                    }
                });

                it('alias', () => {
                    sql_equal(conn, conn.format("find", {
                        table: {
                            test: "a",
                            test1: "b"
                        },
                    }), "SELECT * FROM `test` AS `a`, `test1` AS `b`");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        fields: {
                            'aaa': 'a'
                        }
                    }), "SELECT `aaa` AS `a` FROM `test`");
                });

                it('where', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: 100
                        }
                    }), "SELECT * FROM `test` WHERE `a`=100");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        fields: ["a.b"],
                        where: {
                            "aaa.ddd": 100
                        }
                    }), "SELECT `a`.`b` FROM `test` WHERE `aaa`.`ddd`=100");

                    if (conn.type == 'mssql') {
                        sql_equal(conn, conn.format("find", {
                            table: "test",
                            where: "[a]=100"
                        }), "SELECT * FROM `test` WHERE `a`=100");
                    } else if (conn.type == 'psql') {
                        sql_equal(conn, conn.format("find", {
                            table: "test",
                            where: '"a"=100'
                        }), "SELECT * FROM `test` WHERE `a`=100");

                    } else {
                        sql_equal(conn, conn.format("find", {
                            table: "test",
                            where: "`a`=100"
                        }), "SELECT * FROM `test` WHERE `a`=100");
                    }
                });

                it('operator', () => {
                    var ops = {
                        "$eq": "=",
                        "$ne": "<>",
                        "$gt": ">",
                        "$gte": ">=",
                        "$lt": "<",
                        "$lte": "<=",
                        "$like": " LIKE ",
                        "$not_like": " NOT LIKE "
                    }

                    for (var o in ops) {
                        var opts = {}
                        opts[o] = {};
                        opts[o][o] = 100;

                        sql_equal(conn, conn.format("find", {
                            table: "test",
                            where: opts
                        }), "SELECT * FROM `test` WHERE `" + o + "`" + ops[o] + "100");
                    }

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: {
                                "$in": [100, 200, 300]
                            }
                        }
                    }), "SELECT * FROM `test` WHERE `a` IN (100,200,300)");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: {
                                "$not_in": [100, 200, 300]
                            }
                        }
                    }), "SELECT * FROM `test` WHERE `a` NOT IN (100,200,300)");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: {
                                "$between": [100, 200]
                            }
                        }
                    }), "SELECT * FROM `test` WHERE `a` BETWEEN 100 AND 200");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: {
                                "$not_between": [100, 200]
                            }
                        }
                    }), "SELECT * FROM `test` WHERE `a` NOT BETWEEN 100 AND 200");
                });

                it('where and', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: 100,
                            b: 200
                        }
                    }), "SELECT * FROM `test` WHERE `a`=100 AND `b`=200");
                });

                it('where or', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            "$or": {
                                a: 100,
                                b: 200
                            }
                        }
                    }), "SELECT * FROM `test` WHERE `a`=100 OR `b`=200");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            "$or": [{
                                a: 100
                            },
                            {
                                b: 200
                            }
                            ]
                        }
                    }), "SELECT * FROM `test` WHERE `a`=100 OR `b`=200");
                });

                it('where or/and', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            "$or": [{
                                a: 100,
                                c: 300
                            },
                            {
                                b: 200,
                                d: 400
                            }
                            ]
                        }
                    }), "SELECT * FROM `test` WHERE (`a`=100 AND `c`=300) OR (`b`=200 AND `d`=400)");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            "$or": [{
                                "$or": {
                                    a: 100,
                                    c: 300
                                }
                            },
                            {
                                b: 200,
                                d: 400
                            }
                            ]
                        }
                    }), "SELECT * FROM `test` WHERE `a`=100 OR `c`=300 OR (`b`=200 AND `d`=400)");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            "$or": {
                                "$or": {
                                    a: 100,
                                    c: 300
                                }
                            }
                        }
                    }), "SELECT * FROM `test` WHERE `a`=100 OR `c`=300");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            "$and": {
                                "$and": {
                                    a: 100,
                                    c: 300
                                }
                            }
                        }
                    }), "SELECT * FROM `test` WHERE `a`=100 AND `c`=300");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: [{
                            a: 100,
                            c: 300
                        },
                        {
                            "$or": {
                                b: 200,
                                d: 400
                            }
                        }
                        ]
                    }), "SELECT * FROM `test` WHERE `a`=100 AND `c`=300 AND (`b`=200 OR `d`=400)");

                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: [{
                            a: 100,
                            c: 300
                        },
                        {
                            b: 200,
                            d: 400
                        }
                        ]
                    }), "SELECT * FROM `test` WHERE `a`=100 AND `c`=300 AND `b`=200 AND `d`=400");
                });

                it('skip', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: 200
                        },
                        skip: 100
                    }), "SELECT * FROM `test` WHERE `a`=200 OFFSET 100");
                });

                it('limit', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: 200
                        },
                        limit: 100
                    }), "SELECT * FROM `test` WHERE `a`=200 LIMIT 100");
                });

                it('skip/limit', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: 200
                        },
                        skip: 100,
                        limit: 100
                    }), "SELECT * FROM `test` WHERE `a`=200 LIMIT 100 OFFSET 100");
                });

                it('order', () => {
                    sql_equal(conn, conn.format("find", {
                        table: "test",
                        where: {
                            a: 200
                        },
                        limit: 100,
                        order: ['a', 'b', '-c']
                    }), "SELECT * FROM `test` WHERE `a`=200 ORDER BY `a`, `b`, `c` DESC LIMIT 100");
                });

                it('multi table', () => {
                    sql_equal(conn, conn.format("find", {
                        table: ['test1', 'test2'],
                        fields: ['test1.a', 'test2.b'],
                        where: {
                            "test1.a": {
                                "$field": "test2.b"
                            },
                            "test1.b": {
                                "$gt": {
                                    "$field": "test2.b"
                                }
                            }
                        }
                    }), "SELECT `test1`.`a`, `test2`.`b` FROM `test1`, `test2` WHERE `test1`.`a`=`test2`.`b` AND `test1`.`b`>`test2`.`b`");
                });

                it('sub query', () => {
                    sql_equal(conn, conn.format("find", {
                        table: 'test1',
                        fields: ['a', 'b'],
                        where: {
                            "a": {
                                "$in": [1, 2, 3, 4, 5]
                            },
                            "b": {
                                "$in": {
                                    table: 'test1',
                                    fields: ['a'],
                                    where: {
                                        d: 100
                                    }
                                }
                            }
                        }
                    }), "SELECT `a`, `b` FROM `test1` WHERE `a` IN (1,2,3,4,5) AND `b` IN (SELECT `a` FROM `test1` WHERE `d`=100)");
                });
            });

            describe("format.count", () => {
                it('basic', () => {
                    sql_equal(conn, conn.format("count", {
                        table: "test",
                    }), "SELECT COUNT(*) FROM `test`");
                });

                it('skip/limit', () => {
                    sql_equal(conn, conn.format("count", {
                        table: "test",
                        where: {
                            a: 200
                        },
                        skip: 100,
                        limit: 100
                    }), "SELECT COUNT(*) FROM `test` WHERE `a`=200 SKIP 100 LIMIT 100");
                });
            });

            it("update", () => {
                sql_equal(conn, conn.format("update", {
                    table: "test",
                    values: {
                        a: 100,
                        b: 200
                    },
                    where: {
                        a: 200
                    }
                }), "UPDATE `test` SET `a`=100, `b`=200 WHERE `a`=200");
            });

            it("insert", () => {
                sql_equal(conn, conn.format("insert", {
                    table: "test",
                    values: {
                        a: 100,
                        b: 200
                    }
                }), "INSERT INTO `test` (`a`, `b`) VALUES (100, 200)");
            });

            it("remove", () => {
                sql_equal(conn, conn.format("remove", {
                    table: "test",
                    where: {
                        a: 200
                    }
                }), "DELETE FROM `test` WHERE `a`=200");
            });

            it("createTable", () => {
                var opts = {
                    table: "test",
                    fields: {
                        t: 'text',
                        t1: {
                            type: "text",
                            size: 100
                        },
                        n: 'number',
                        n1: {
                            type: "number",
                            size: 4
                        },
                        i: 'integer',
                        i2: {
                            type: "integer",
                            size: 2
                        },
                        i3: {
                            type: "integer",
                            size: 8
                        },
                        d: 'date',
                        d1: {
                            type: "date",
                            time: true
                        },
                        b: 'binary',
                        b1: {
                            type: "binary",
                            big: true
                        },
                        def: {
                            type: 'integer',
                            defaultValue: 123
                        },
                        required: {
                            type: 'integer',
                            required: true
                        },
                        unique: {
                            type: 'integer',
                            unique: true
                        },
                        key: {
                            type: 'integer',
                            key: true
                        }
                    }
                };

                switch (conn.type) {
                    case "SQLite":
                        sql_equal(conn, conn.format("createTable", opts), "CREATE TABLE `test`(`t` TEXT, `t1` VARCHAR(100), `n` DOUBLE, `n1` FLOAT, `i` INT, `i2` SMALLINT, `i3` BIGINT, `d` DATE, `d1` DATETIME, `b` BLOB, `b1` LONGBLOB, `def` INT DEFAULT 123, `required` INT NOT NULL, `unique` INT UNIQUE, `key` INT PRIMARY KEY)");
                        break;
                    case "mysql":
                        sql_equal(conn, conn.format("createTable", opts), "CREATE TABLE `test`(`t` LONGTEXT, `t1` VARCHAR(100), `n` DOUBLE, `n1` FLOAT, `i` INT, `i2` SMALLINT, `i3` BIGINT, `d` DATE, `d1` DATETIME, `b` BLOB, `b1` LONGBLOB, `def` INT DEFAULT 123, `required` INT NOT NULL, `unique` INT UNIQUE, `key` INT PRIMARY KEY)");
                        break;
                    case "mssql":
                        sql_equal(conn, conn.format("createTable", opts), "CREATE TABLE [test]([t] VARCHAR(MAX), [t1] VARCHAR(100), [n] FLOAT, [n1] REAL, [i] INT, [i2] SMALLINT, [i3] BIGINT, [d] DATE, [d1] DATETIME, [b] VARBINARY(MAX), [b1] IMAGE, [def] INT DEFAULT 123, [required] INT NOT NULL, [unique] INT UNIQUE, [key] INT PRIMARY KEY)");
                        break;
                    case "psql":
                        sql_equal(conn, conn.format("createTable", opts), "CREATE TABLE \"test\"(\"t\" TEXT, \"t1\" VARCHAR(100), \"n\" FLOAT, \"n1\" REAL, \"i\" INT, \"i2\" SMALLINT, \"i3\" BIGINT, \"d\" DATE, \"d1\" TIMESTAMP, \"b\" BYTEA, \"b1\" BYTEA, \"def\" INT DEFAULT 123, \"required\" INT NOT NULL, \"unique\" INT UNIQUE, \"key\" INT PRIMARY KEY)");
                        break;
                    default:
                        throw new Error('not support');
                }
            });

            it("dropTable", () => {
                sql_equal(conn, conn.format("dropTable", {
                    table: "test"
                }), "DROP TABLE `test`");
            });

            it("createIndex", () => {
                sql_equal(conn, conn.format("createIndex", {
                    table: "test",
                    index: "idx_test",
                    keys: ['t', '-t1']
                }), "CREATE INDEX `idx_test` ON `test`(`t`, `t1` DESC)");
            });

            it("dropIndex", () => {
                sql_equal(conn, conn.format("dropIndex", {
                    index: "test"
                }), "DROP INDEX `test`");
            });
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
                if (conn.type == 'mysql')
                    assert.equal(rs[0].v, v.toString());
                else
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

            it('field', () => {
                var res = [];
                for (var i = 0x21; i < 0x7f; i++) {
                    var v = String.fromCharCode(i);
                    if (v == '.')
                        continue;

                    try {
                        var sql = conn.format('find', {
                            table: v
                        }).replace('* FROM', '1 AS');
                        var rs = conn.execute(sql);
                        if (Object.keys(rs[0])[0] != v)
                            res.push(v);
                    } catch (e) {
                        res.push(v);
                    }
                }
                assert.deepEqual(res, []);
            });
        });

        it("create table", () => {
            if (conn.type == 'mssql')
                conn.execute('create table test(t0 INT IDENTITY PRIMARY KEY, t1 int, t2 varchar(128), t3 VARBINARY(100), t4 datetime);');
            else {
                if (conn.type == 'psql') {
                    conn.execute('create table test(t0 SERIAL PRIMARY KEY, t1 int, t2 varchar(128), t3 BYTEA, t4 timestamp);');
                    conn.execute('create table test_null(t1 int NULL, t2 varchar(128) NULL, t3 BYTEA NULL, t4 timestamp NULL);');
                }
                else {
                    conn.execute('create table test(t0 INTEGER AUTO_INCREMENT PRIMARY KEY, t1 int, t2 varchar(128), t3 BLOB, t4 datetime);');
                    conn.execute('create table test_null(t1 int NULL, t2 varchar(128) NULL, t3 BLOB NULL, t4 datetime NULL);');
                }
            }

            conn.execute('create table test2(t1 varchar(10), t2 varchar(10));');
            conn.execute('create table test3(t1 varchar(10), t2 varchar(10));');

            conn.createTable({
                table: "test4",
                fields: {
                    t: 'text',
                    t1: {
                        type: "text",
                        size: 100
                    }
                }
            });
        });

        describe("data type", () => {
            afterEach(() => {
                try {
                    conn.execute('drop table test_type');
                } catch (e) { }
            });


            it("number(4)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: {
                            type: "number",
                            size: 4
                        }
                    }
                });

                var v = Number("0.55555555555555555555555555555555555555555555");
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');

                if (conn.type !== 'SQLite')
                    assert.greaterThan((rs[0].v - v) * 65536 * 65536, 1);
                else
                    assert.lessThan((rs[0].v - v) * 65536 * 65536, 1);
            });

            it("number(8)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: "number"
                    }
                });

                var v = Number("0.55555555555555555555555555555555555555555555");
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.lessThan((rs[0].v - v) * 65536 * 65536, 1);
            });

            it("number(null)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: "number"
                    }
                });

                var v = null;
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.equal(rs[0].v, v);
            });

            function test_integer(sz) {
                it(`integer(${sz})`, () => {
                    conn.createTable({
                        table: "test_type",
                        fields: {
                            v: {
                                type: "integer",
                                size: sz
                            }
                        }
                    });

                    if (conn.type !== 'SQLite') {
                        var int_limit = Math.pow(256, sz) / 2;
                        if (sz == 8)
                            int_limit /= 256;

                        conn.execute('insert into test_type values(?)', - int_limit);
                        var rs = conn.execute('select * from test_type');
                        assert.equal(rs[0].v, - int_limit);
                        conn.execute('delete from test_type');

                        conn.execute('insert into test_type values(?)', int_limit - 1);
                        var rs = conn.execute('select * from test_type');
                        assert.equal(rs[0].v, int_limit - 1);
                        conn.execute('delete from test_type');

                        if (sz < 8) {
                            assert.throws(() => {
                                conn.execute('insert into test_type values(?)', - int_limit - 1);
                            });

                            assert.throws(() => {
                                conn.execute('insert into test_type values(?)', int_limit);
                            })
                        }
                    }
                });
            }

            test_integer(2);
            test_integer(4);
            test_integer(8);

            it("integer(null)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: "integer"
                    }
                });

                var v = null;
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.equal(rs[0].v, v);
            });

            it("date", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: {
                            type: "date",
                            time: true
                        }
                    }
                });

                var v = new Date('1998-04-14 12:12:12');
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.deepEqual(rs[0].v, v);
            });

            it("date(null)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: "date"
                    }
                });

                var v = null;
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.equal(rs[0].v, v);
            });

            it("text(n)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: {
                            type: "text",
                            size: 8
                        }
                    }
                });

                var v = "01234567";
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.equal(rs[0].v, v);

                if (conn.type !== 'SQLite')
                    assert.throws(() => {
                        var v = "012345678";
                        conn.execute('insert into test_type values(?)', v);
                    });
            });

            it("text", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: "text"
                    }
                });

                var v = "0123456789abcdef".repeat(65536);
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.equal(rs[0].v, v);
            });

            it("text(null)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: {
                            type: "text",
                            size: 8
                        }
                    }
                });

                var v = null;
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.equal(rs[0].v, v);
            });

            it("query(tinytext)", () => {
                if (conn.type == 'mysql') {
                    conn.execute("CREATE TABLE test_type(`v` tinytext)");
                    var v = "0123456789abcdef";
                    conn.execute('insert into test_type values(?)', v);
                    var rs = conn.execute('select * from test_type');
                    assert.strictEqual(rs[0].v, v);
                }
            });

            it("query(text)", () => {
                if (conn.type == 'mysql') {
                    conn.execute("CREATE TABLE test_type(`v` text)");
                    var v = "0123456789abcdef";
                    conn.execute('insert into test_type values(?)', v);
                    var rs = conn.execute('select * from test_type');
                    assert.strictEqual(rs[0].v, v);
                }
            });

            it("binary", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: "binary"
                    }
                });

                var v = new Buffer("0123456789abcdef".repeat(4095));
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.deepEqual(rs[0].v, v);

                if (conn.type == 'mysql')
                    assert.throws(() => {
                        var v = new Buffer("0123456789abcdef".repeat(4096));
                        conn.execute('insert into test_type values(?)', v);
                    });
            });

            it("big binary", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: {
                            type: "binary",
                            big: true
                        }
                    }
                });

                var v = new Buffer("0123456789abcdef".repeat(65536));
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.deepEqual(rs[0].v, v);
            });

            it("binary(null)", () => {
                conn.createTable({
                    table: "test_type",
                    fields: {
                        v: "binary"
                    }
                });

                var v = null;
                conn.execute('insert into test_type values(?)', v);
                var rs = conn.execute('select * from test_type');
                assert.equal(rs[0].v, v);
            });
        });

        it("create index", () => {
            conn.createIndex({
                table: "test4",
                index: "idx_test4",
                keys: ['-t1']
            });
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

            if (conn.type == 'SQLite') {
                rs = conn.execute('select t1,t2,t3,t4 from test_null')[0];
                assert.isNull(rs.t1);
                assert.isNull(rs.t2);
                assert.isNull(rs.t3);
                assert.isNull(rs.t4);
            }
        });

        it("find", () => {
            var rs = conn.find({
                table: "test"
            });
            var r = rs[0];

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
        });

        it("sub query", () => {
            conn.insert({
                table: "test2",
                values: {
                    t1: 'aaa',
                    t2: 'bbb'
                }
            });

            conn.insert({
                table: "test3",
                values: {
                    t1: 'aaa',
                    t2: '200'
                }
            });

            var rs = conn.find({
                table: 'test2',
                where: {
                    t1: {
                        "$in": {
                            table: 'test3',
                            fields: ['t1'],
                            where: {
                                t2: '200'
                            }
                        }
                    }
                }
            })

            assert.deepEqual(rs, [{
                "t1": "aaa",
                "t2": "bbb"
            }]);
        });

        it("conn.count", () => {
            assert.equal(conn.count({
                table: "test"
            }), 1);
        });

        it("conn.update", () => {
            assert.equal(conn.update({
                table: "test",
                values: {
                    t2: "200"
                }
            }), 1);
        });

        it("conn.insert/remove", () => {
            assert.equal(conn.insert({
                table: "test",
                values: {
                    t2: "2200"
                }
            }), conn.type == 'mysql' || conn.type == 'SQLite' ? 2 : 0);

            assert.equal(conn.remove({
                table: "test",
                where: {
                    t2: "2200"
                }
            }), 1);
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

        switch (type) {
            case 'mssql':
            case 'mysql':
            case 'psql':
                // some special types would not be parsed correctly with old implementation,
                // this case would failed in fibjs 0.34.0 with postgresql 14
                it('special type for psql, but also test for other dialect', () => {
                    var tables = conn.execute('select * from information_schema.tables;');
                    assert.greaterThan(tables, 0);

                    var columns = conn.execute('select * from information_schema.columns;');
                    assert.greaterThan(columns, 0);
                });
                break;
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

        it('forEach', () => {
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

            count = 0;
            ldb.forEach((v, k) => {
                assert.equal(data[k], v.toString());
                delete data[k];
                count++;
            });
            assert.equal(count, 4);

            ldb.close();
            clear_db();
        });

        it('between', () => {
            var data = {
                "ccc": "ccc value",
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ddd": "ddd value",
                "bbb_1": "bbb_1 value",
            };

            var data1 = {
                "bbb_1": "bbb_1 value",
                "ccc": "ccc value"
            };

            var ldb = db.openLevelDB(path.join(__dirname, "testdb" + vmid));
            ldb.mset(data);

            var count = 0;
            ldb.between("bbb", "ddd", (v, k) => {
                assert.equal(data1[k], v.toString());
                delete data1[k];
                count++;
            });
            assert.equal(count, 2);

            ldb.close();
            clear_db();
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

require.main === module && test.run(console.DEBUG);