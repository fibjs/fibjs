var test = require("test");
test.setup();

var db = require('db');
var os = require('os');
var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');

var vmid = coroutine.vmid;

describe("db", () => {
    it("escape", () => {
        assert.equal('123456\\r\\n\'\'\\\"\\\x1acccds', db.escape(
            '123456\r\n\'\"\x1acccds', true));
    });

    it("format", () => {
        assert.equal(db.format("test?", [1, 2, 3, 4]), "test(1,2,3,4)");
        assert.equal(db.format("test?", [1, [2, 3], 4]), "test(1,(2,3),4)");
        assert.equal(db.format("test?"), "test?");
    });

    it("formatMySQL", () => {
        assert.equal(db.formatMySQL("test?, ?, ?, ?", 123, 'ds\r\na',
                new Date('1998-4-14 12:12:12')),
            "test123, 'ds\\r\\na', '1998-04-14 12:12:12', ?");
        assert.equal(db.formatMySQL("test?"), "test?");
    });

    describe("format.find", () => {

        it('basic', () => {
            assert.equal(db.format("find", {
                table: "test"
            }), "SELECT * FROM `test`");
        });

        it('fields', () => {
            assert.equal(db.format("find", {
                table: "test",
                fields: ["a", "b", "c"]
            }), "SELECT `a`, `b`, `c` FROM `test`");

            assert.equal(db.format("find", {
                table: "test",
                fields: "`a`, `b`, `c`"
            }), "SELECT `a`, `b`, `c` FROM `test`");
        });

        it('alias', () => {
            assert.equal(db.format("find", {
                table: {
                    test: "a",
                    test1: "b"
                },
            }), "SELECT * FROM `test` AS `a`, `test1` AS `b`");

            assert.equal(db.format("find", {
                table: "test",
                fields: {
                    'aaa': 'a'
                }
            }), "SELECT `aaa` AS `a` FROM `test`");
        });

        it('where', () => {
            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: 100
                }
            }), "SELECT * FROM `test` WHERE `a`=100");

            assert.equal(db.format("find", {
                table: "test",
                where: {
                    "aaa`ddd": 100
                }
            }), "SELECT * FROM `test` WHERE `aaa``ddd`=100");

            assert.equal(db.format("find", {
                table: "test",
                fields: ["a.b"],
                where: {
                    "aaa.ddd": 100
                }
            }), "SELECT `a`.`b` FROM `test` WHERE `aaa`.`ddd`=100");

            assert.equal(db.format("find", {
                table: "test",
                where: "`a`=100"
            }), "SELECT * FROM `test` WHERE `a`=100");
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

                assert.equal(db.format("find", {
                    table: "test",
                    where: opts
                }), "SELECT * FROM `test` WHERE `" + o + "`" + ops[o] + "100");
            }

            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: {
                        "$in": [100, 200, 300]
                    }
                }
            }), "SELECT * FROM `test` WHERE `a` IN (100,200,300)");

            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: {
                        "$not_in": [100, 200, 300]
                    }
                }
            }), "SELECT * FROM `test` WHERE `a` NOT IN (100,200,300)");

            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: {
                        "$between": [100, 200]
                    }
                }
            }), "SELECT * FROM `test` WHERE `a` BETWEEN 100 AND 200");

            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: {
                        "$not_between": [100, 200]
                    }
                }
            }), "SELECT * FROM `test` WHERE `a` NOT BETWEEN 100 AND 200");
        });

        it('where and', () => {
            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: 100,
                    b: 200
                }
            }), "SELECT * FROM `test` WHERE `a`=100 AND `b`=200");
        });

        it('where or', () => {
            assert.equal(db.format("find", {
                table: "test",
                where: {
                    "$or": {
                        a: 100,
                        b: 200
                    }
                }
            }), "SELECT * FROM `test` WHERE `a`=100 OR `b`=200");

            assert.equal(db.format("find", {
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
            assert.equal(db.format("find", {
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

            assert.equal(db.format("find", {
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

            assert.equal(db.format("find", {
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

            assert.equal(db.format("find", {
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

            assert.equal(db.format("find", {
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

            assert.equal(db.format("find", {
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
            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: 200
                },
                skip: 100
            }), "SELECT * FROM `test` WHERE `a`=200 OFFSET 100");
        });

        it('limit', () => {
            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: 200
                },
                limit: 100
            }), "SELECT * FROM `test` WHERE `a`=200 LIMIT 100");
        });

        it('skip/limit', () => {
            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: 200
                },
                skip: 100,
                limit: 100
            }), "SELECT * FROM `test` WHERE `a`=200 LIMIT 100 OFFSET 100");
        });

        it('order', () => {
            assert.equal(db.format("find", {
                table: "test",
                where: {
                    a: 200
                },
                limit: 100,
                order: ['a', 'b', '-c']
            }), "SELECT * FROM `test` WHERE `a`=200 ORDER BY `a`, `b`, `c` DESC LIMIT 100");
        });

        it('multi table', () => {
            assert.equal(db.format("find", {
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
            assert.equal(db.format("find", {
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
            assert.equal(db.format("count", {
                table: "test",
            }), "SELECT COUNT(*) FROM `test`");
        });

        it('skip/limit', () => {
            assert.equal(db.format("count", {
                table: "test",
                where: {
                    a: 200
                },
                skip: 100,
                limit: 100
            }), "SELECT COUNT(*) FROM `test` WHERE `a`=200 SKIP 100 LIMIT 100");
        });
    });

    it("format.update", () => {
        assert.equal(db.format("update", {
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

    it("format.insert", () => {
        assert.equal(db.format("insert", {
            table: "test",
            values: {
                a: 100,
                b: 200
            }
        }), "INSERT INTO `test` (`a`, `b`) VALUES (100, 200)");
    });

    it("format.remove", () => {
        assert.equal(db.format("remove", {
            table: "test",
            where: {
                a: 200
            }
        }), "DELETE FROM `test` WHERE `a`=200");
    });

    it("format.createTable", () => {
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
                i1: {
                    type: "integer",
                    size: 1
                },
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

        assert.equal(db.format("createTable", opts), "CREATE TABLE `test`(`t` TEXT, `t1` VARCHAR(100), `n` DOUBLE, `n1` FLOAT, `i` INT, `i1` TINYINT, `i2` SMALLINT, `i3` BIGINT, `d` DATE, `d1` DATETIME, `b` BLOB, `b1` LONGBLOB, `def` INT DEFAULT 123, `required` INT NOT NULL, `unique` INT UNIQUE, `key` INT PRIMARY KEY)");
        assert.equal(db.formatMySQL("createTable", opts), "CREATE TABLE `test`(`t` LONGTEXT, `t1` VARCHAR(100), `n` DOUBLE, `n1` FLOAT, `i` INT, `i1` TINYINT, `i2` SMALLINT, `i3` BIGINT, `d` DATE, `d1` DATETIME, `b` BLOB, `b1` LONGBLOB, `def` INT DEFAULT 123, `required` INT NOT NULL, `unique` INT UNIQUE, `key` INT PRIMARY KEY)");
        assert.equal(db.formatMSSQL("createTable", opts), "CREATE TABLE `test`(`t` TEXT, `t1` VARCHAR(100), `n` REAL, `n1` FLOAT, `i` INT, `i1` TINYINT, `i2` SMALLINT, `i3` BIGINT, `d` DATE, `d1` DATETIME, `b` VARBINARY(MAX), `b1` IMAGE, `def` INT DEFAULT 123, `required` INT NOT NULL, `unique` INT UNIQUE, `key` INT PRIMARY KEY)");
    });

    it("format.dropTable", () => {
        assert.equal(db.format("dropTable", {
            table: "test"
        }), "DROP TABLE `test`");
    });

    it("format.createIndex", () => {
        assert.equal(db.format("createIndex", {
            table: "test",
            index: "idx_test",
            keys: ['t', '-t1']
        }), "CREATE INDEX `idx_test` ON `test`(`t`, `t1` DESC)");
    });

    it("format.dropIndex", () => {
        assert.equal(db.format("dropIndex", {
            index: "test"
        }), "DROP INDEX `test`");
    });

    function _test(conn_str) {
        var conn;
        var tables = ['test', 'test_null', 'test2', 'test3', 'test4'];

        before(() => {
            conn = db.open(conn_str);

            tables.forEach(t => {
                try {
                    conn.execute('drop table ' + t);
                } catch (e) {}
            });
        });

        after(() => {
            tables.forEach(t => {
                try {
                    conn.execute('drop table ' + t);
                } catch (e) {}
            });

            conn.close();
        });

        it("empty sql", () => {
            assert.throws(() => {
                conn.execute("  ");
            })
        });

        it("empty sql args", () => {
            var rs = conn.execute('select "?" as v');
            assert.equal(rs[0].v, '?');
        });

        it("escape", () => {
            var rs = conn.execute("select ? as t;", '123456\r\n\'\"\x1acccds');
            assert.equal(rs[0].t, '123456\r\n\'\"\x1acccds');
        });

        it("create table", () => {
            if (conn.type == 'mssql')
                conn.execute('create table test(t0 INT IDENTITY PRIMARY KEY, t1 int, t2 varchar(128), t3 VARBINARY(100), t4 datetime);');
            else {
                conn.execute('create table test(t0 INTEGER AUTO_INCREMENT PRIMARY KEY, t1 int, t2 varchar(128), t3 BLOB, t4 datetime);');
                conn.execute('create table test_null(t1 int NULL, t2 varchar(128) NULL, t3 BLOB NULL, t4 datetime NULL);');
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

        it("create index", () => {
            conn.createIndex({
                table: "test4",
                index: "idx_test4",
                keys: ['-t1']
            });
        });

        it("insert", () => {
            var rs = conn.execute("insert into test(t1, t2, t3, t4) values(?,?,?,?);", 1123,
                'aaaaa', new Buffer('DDDDDDDDDD'), new Date(
                    '1998-04-14 12:12:12'));

            if (conn.type != 'mssql')
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
            assert.strictEqual(r['t2'], 'aaaaa');
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
            assert.strictEqual(r['t2'], 'aaaaa');
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
            }), 2);

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

            assert.deepEqual(conn.execute('select 100 as n;select 200 as n'), [
                [{
                    n: 100
                }],
                [{
                    n: 200
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
            var b = new Buffer();
            b.resize(1);

            for (var i = 0; i < 256; i++) {
                b[0] = i;
                conn.execute("delete from test;");
                conn.execute("insert into test(t1, t2, t3, t4) values(1,'aa', ?, ?);",
                    b, new Date());
                var rs = conn.execute("select * from test;");
                assert.equal(rs[0].t3.length, 1);
                assert.equal(rs[0].t3[0], i);
            }
        });

        describe("transaction", () => {
            before(() => {
                try {
                    var b = new Buffer();
                    conn.execute("insert into test(t1, t2, t3, t4) values(?,?,?,?);", 101, 'test101', b, new Date());
                } catch (e) {}
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
    }

    describe("sqlite", () => {
        var conn_str = 'sqlite:' + path.join(__dirname, 'test.db' + vmid);
        after(() => {
            fs.unlink(path.join(__dirname, "test.db" + vmid));
            fs.unlink(path.join(__dirname, "test.db" + vmid + ".backup"));
        });
        _test(conn_str);

        it("check synchronous mode", () => {
            var conn = db.open(conn_str);
            var synchronous = conn.execute("PRAGMA synchronous;")[0].synchronous;
            conn.close();

            assert.equal(synchronous, 2);
        });

        it("check journal mode", () => {
            var conn = db.open(conn_str);
            var journal_mode = conn.execute("PRAGMA journal_mode;")[0].journal_mode;
            conn.close();
            assert.equal(journal_mode, "wal");
        });

        it("fulltext search", () => {
            var conn = db.open(conn_str);

            conn.execute('CREATE VIRTUAL TABLE email USING fts5(sender, title, body)');
            conn.execute('insert into email(sender, title, body) values("tom cat", "some titles", "this is body")');
            conn.execute('insert into email(sender, title, body) values("响马", "这里是标题yes", "this这里是身体")');
            assert.deepEqual(conn.execute('SELECT * FROM email WHERE email MATCH "这里"'), []);

            conn.execute('CREATE VIRTUAL TABLE email1 USING fts5(sender, title, body, tokenize = porter)');
            conn.execute('insert into email1(sender, title, body) values("tom cat", "some titles", "this is body")');
            conn.execute('insert into email1(sender, title, body) values("响马", "这里是标题yes", "this这里是身体")');
            assert.deepEqual(conn.execute('SELECT * FROM email1 WHERE email1 MATCH "这里"'), [{
                "body": "this这里是身体",
                "sender": "响马",
                "title": "这里是标题yes"
            }]);

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

    if (global.full_test) {
        describe("mysql", () => {
            _test('mysql://root@localhost/test');
        });

        if (process.platform == 'win32')
            describe("mssql", () => {
                _test('mssql://sa@localhost/test');
            });
    }

    describe("leveldb", () => {
        after(clear_db);

        function clear_db() {
            try {
                fs.readdir(path.join(__dirname, "testdb" + vmid)).forEach((s) => {
                    fs.unlink(path.join(__dirname, "testdb" + vmid, s));
                });

                fs.rmdir(path.join(__dirname, "testdb" + vmid));
            } catch (e) {};
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

    });
});

require.main === module && test.run(console.DEBUG);