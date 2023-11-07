var test = require("test");
test.setup();

var db = require('db');
var fs = require('fs')
var path = require('path')

describe("vec", () => {
    var conn;

    beforeEach(() => {
        conn = db.openSQLite(":memory:");
    });

    afterEach(() => {
        conn.close();
        try {
            fs.unlink(path.join(__dirname, "vec_test.db"));
        } catch (e) { }
    });

    it("vec_version", () => {
        assert.equal(conn.execute("select vec_version() as v")[0].v, 'v0.0.1');
    });

    it("vec_distance", () => {
        var n = conn.execute(`select vec_distance("[1,2,3]", "[3,4,5]") as v`)[0].v.toFixed(6);
        assert.equal(n, 0.017292);

        var n = conn.execute(`select vec_distance("[1,2]", "[3,4,5]") as v`)[0].v.toFixed(6);
        var n1 = conn.execute(`select vec_distance("[1,2, 0]", "[3,4,5]") as v`)[0].v.toFixed(6);
        assert.equal(n, n1);
    });

    function decodeVec(buf) {
        var res = [];

        for (var i = 0; i < buf.length; i += 16) {
            var arr = [];

            arr.push(buf.readFloatLE(0).toFixed(6));
            arr.push(buf.readFloatLE(4).toFixed(6));
            arr.push(buf.readFloatLE(8).toFixed(6));
            arr.push(buf.readInt32LE(12));
            res.push(arr);
        }

        return res;
    }

    it("create table", () => {
        conn.execute("create virtual table vindex using vec_index(title(128), description(128))");
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vec_index'"), [
            {
                "name": "vec_index"
            }
        ]);

        assert.deepEqual(conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`), [
            {
                "name": "title",
                "data": null
            },
            {
                "name": "description",
                "data": null
            }
        ]);

        conn.execute("drop table vindex");
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vindex_data'"), []);
    });

    it("insert", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ]
        ]);

        assert.throws(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        });

        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);
        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ],
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ],
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ]
        ]);
    });

    it("insert(binary vector)", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute('insert into vindex(title, description, rowid) values(?, ?, ?)',
            Buffer.from(Float32Array.from([1, 2, 3]).buffer), Buffer.from(Float32Array.from([3, 4, 5]).buffer), 1);

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ]
        ]);
    });

    it("select rowid", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);
        assert.deepEqual(conn.execute(`select rowid from vindex`), [
            {
                "rowid": 1
            },
            {
                "rowid": 2
            }
        ]);

        assert.deepEqual(conn.execute(`select rowid from vindex where rowid in (1,2)`), [
            {
                "rowid": 1
            },
            {
                "rowid": 2
            }
        ]);

        assert.deepEqual(conn.execute(`select rowid from vindex where rowid in (2,3)`), [
            {
                "rowid": 2
            }
        ]);
    });

    it("search", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[2,2,3]", "[3,4,5]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[3,200,1]", "[3,4,5]", 2)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[-1,2,10]", "[3,4,5]", 3)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,5.1234]", "[3,4,5]", 4)`);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[1,2,5.1234]")`);
        assert.equal(res[0].rowid, 4);
        assert.closeTo(res[0].distance, 0, 0.0001);
        assert.equal(res[1].rowid, 3);
        assert.closeTo(res[1].distance, 0.053202, 0.0001);
        assert.equal(res[2].rowid, 1);
        assert.closeTo(res[2].distance, 0.072819, 0.0001);
        assert.equal(res[3].rowid, 2);
        assert.closeTo(res[3].distance, 0.635004, 0.0001);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[1,2,5.1234]:1")`);
        assert.equal(res[0].rowid, 4);
        assert.closeTo(res[0].distance, 0, 0.0001);
    });

    it("detele", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);
        conn.execute(`delete from vindex where rowid = 1`);
        conn.execute(`delete from vindex where rowid = 100`);
        assert.deepEqual(conn.execute(`select rowid from vindex`), [
            {
                "rowid": 2
            }
        ]);
    });

    it("detele all", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);
        conn.execute(`delete from vindex`);
        assert.deepEqual(conn.execute(`select rowid from vindex`), []);
    });

    it("update", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ],
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ],
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ]
        ]);

        conn.execute(`update vindex set title="[1,2,4]" where rowid = 2`);

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ],
            [
                "0.267261",
                "0.534522",
                "0.801784",
                1
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ],
            [
                "0.424264",
                "0.565685",
                "0.707107",
                1
            ]
        ]);
    });

    it("double insert in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);
            assert.throws(() => {
                conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);
            });
        });

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.267261",
                "0.534522",
                "0.801784",
                3
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.424264",
                "0.565685",
                "0.707107",
                3
            ]
        ]);
    });

    it("insert after delete in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);

        conn.trans(() => {
            conn.execute(`delete from vindex where rowid = 3`);
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        });

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.333333",
                "0.666667",
                "0.666667",
                3
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.588348",
                "0.784465",
                "0.196116",
                3
            ]
        ]);
    });

    it("delete after insert in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
            conn.execute(`delete from vindex where rowid = 3`);
        });

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.equal(res[0].data.hex(), "");
        assert.equal(res[1].data.hex(), "");
    });

    it("load from disk db", () => {
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);

        conn.close();

        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.333333",
                "0.666667",
                "0.666667",
                3
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.588348",
                "0.784465",
                "0.196116",
                3
            ]
        ]);

        assert.deepEqual(conn.execute(`select rowid from vindex`), [
            {
                "rowid": 3
            }
        ]);

        assert.throws(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        });

        conn.execute(`update vindex set title="[1,2,4]" where rowid = 3`);

        var res = conn.execute(`select name, data from vec_index where tbl="vindex" order by name desc`);
        assert.equal(res.length, 2);
        assert.deepEqual(decodeVec(res[0].data), [
            [
                "0.218218",
                "0.436436",
                "0.872872",
                3
            ]
        ]);
        assert.deepEqual(decodeVec(res[1].data), [
            [
                "0.588348",
                "0.784465",
                "0.196116",
                3
            ]
        ]);
    });

    it("benchmark", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        console.time("insert");
        conn.trans(() => {
            for (var i = 0; i < 4096; i++) {
                var title = [];
                var description = [];

                for (var j = 0; j < 3; j++) {
                    title.push(Math.random());
                    description.push(Math.random());
                }

                conn.execute("insert into vindex(title, description, rowid) values(?,?,?)",
                    Buffer.from(Float32Array.from(title).buffer), Buffer.from(Float32Array.from(description).buffer), i);
            }
        })
        console.timeEnd("insert");

        var key = [];

        for (var j = 0; j < 3; j++) {
            key.push(Math.random());
        }

        console.time("vec_search");
        var r1 = conn.execute(`select rowid, distance from vindex where vec_search(title, "${JSON.stringify(key)}") limit 10`);
        console.timeEnd("vec_search");

        console.time("vec_search limit");
        var r2 = conn.execute(`select rowid, distance from vindex where vec_search(title, "${JSON.stringify(key)}:10")`);
        console.timeEnd("vec_search limit");

        assert.deepEqual(r1, r2);
    });
});

require.main === module && test.run(console.DEBUG);
