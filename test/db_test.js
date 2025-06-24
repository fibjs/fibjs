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
    //     conn_str: `mysql://root@localhost/${DBNAME}`,
    // },
    // psql: {
    //     desc: '[psql] sql db universal test',
    //     conn_str: `psql://postgres@localhost/${DBNAME}`,
    // },
    // mssql: {
    //     desc: '[mssql] sql db universal test',
    //     conn_str: `mssql://sa@localhost/${DBNAME}`,
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

            it('typed array', () => {
                var v = new Uint8Array([1, 2, 3]);
                var rs = conn.execute('select ? as v', v);
                assert.deepEqual(rs[0].v, v);
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

            if (conn.type == 'SQLite') {
                rs = conn.execute('select t1,t2,t3,t4 from test_null')[0];
                assert.isNull(rs.t1);
                assert.isNull(rs.t2);
                assert.isNull(rs.t3);
                assert.isNull(rs.t4);
            }
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

        switch (type) {
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


