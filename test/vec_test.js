var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

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

            arr.push(buf.readFloatLE(i).toFixed(6));
            arr.push(buf.readFloatLE(i + 4).toFixed(6));
            arr.push(buf.readFloatLE(i + 8).toFixed(6));
            arr.push(buf.readInt32LE(i + 12));
            res.push(arr);
        }

        return res;
    }

    function splitPages(buf, pageSize) {
        var pages = [];

        for (var offset = 0; offset < buf.length; offset += pageSize)
            pages.push(buf.slice(offset, Math.min(offset + pageSize, buf.length)));

        return pages;
    }

    it("create table", () => {
        conn.execute("create virtual table vindex using vec_index(title(128), description(128))");
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vec_index'"), [
            {
                "name": "vec_index"
            }
        ]);
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vec_index_meta'"), [
            {
                "name": "vec_index_meta"
            }
        ]);
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vec_index_pages'"), [
            {
                "name": "vec_index_pages"
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
        assert.deepEqual(conn.execute(`select name, format, page_size from vec_index_meta where tbl="vindex" order by name desc`), [
            {
                "name": "title",
                "format": 0,
                "page_size": 4096
            },
            {
                "name": "description",
                "format": 0,
                "page_size": 4096
            }
        ]);

        conn.execute("drop table vindex");
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vindex_data'"), []);
        assert.deepEqual(conn.execute(`select * from vec_index_meta where tbl="vindex"`), []);
        assert.deepEqual(conn.execute(`select * from vec_index_pages where tbl="vindex"`), []);
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
                2
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
                2
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
                2
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
                2
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
                "0.218218",
                "0.436436",
                "0.872872",
                2
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
                2
            ]
        ]);
    });

    it("update after delete", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[1,1,1]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[4,5,6]", "[1,1,1]", 2)`);

        conn.execute(`delete from vindex where rowid = 1`);
        conn.execute(`update vindex set title="[7,8,9]" where rowid = 2`);

        assert.deepEqual(conn.execute(`select rowid from vindex`), [
            {
                "rowid": 2
            }
        ]);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[7,8,9]:1")`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 2);
        assert.closeTo(res[0].distance, 0, 0.0001);
    });

    it("update rowid", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,0,0]", "[1,1,1]", 10)`);

        conn.execute(`update vindex set rowid = 20 where rowid = 10`);

        assert.deepEqual(conn.execute(`select rowid from vindex`), [
            {
                "rowid": 20
            }
        ]);
        assert.deepEqual(conn.execute(`select rowid from vindex where rowid = 10`), []);
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

    it("load from disk db (paged format)", () => {
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.trans(() => {
            for (var rowid = 1; rowid <= 300; rowid++) {
                conn.execute(`insert into vindex(title, description, rowid) values(?, ?, ?)`,
                    JSON.stringify([rowid, rowid + 1, rowid + 2]),
                    JSON.stringify([rowid + 3, rowid + 4, rowid + 5]),
                    rowid);
            }
        });

        var rows = conn.execute(`select name, data from vec_index where tbl="vindex" order by name asc`);
        rows.forEach((row) => {
            conn.execute(`update vec_index_meta set format = 1 where tbl="vindex" and name = ?`, row.name);
            conn.execute(`delete from vec_index_pages where tbl="vindex" and name = ?`, row.name);

            splitPages(row.data, 4096).forEach((page, index) => {
                conn.execute(`insert into vec_index_pages(tbl, name, page_no, data) values("vindex", ?, ?, ?)`, row.name, index, page);
            });

            conn.execute(`update vec_index set data = x'' where tbl="vindex" and name = ?`, row.name);
        });

        conn.close();

        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        assert.deepEqual(conn.execute(`select count(*) as count from vindex`), [
            {
                "count": 300
            }
        ]);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[3,4,5]:1")`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
        assert.closeTo(res[0].distance, 0, 0.0001);

        conn.execute(`update vindex set title="[3,4,9]", description="[6,7,8]" where rowid = 3`);
        conn.close();

        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        assert.deepEqual(conn.execute(`select name, format from vec_index_meta where tbl="vindex" order by name asc`), [
            {
                "name": "description",
                "format": 1
            },
            {
                "name": "title",
                "format": 1
            }
        ]);
        assert.deepEqual(conn.execute(`select name, page_no from vec_index_pages where tbl="vindex" order by name asc, page_no asc`), [
            {
                "name": "description",
                "page_no": 0
            },
            {
                "name": "description",
                "page_no": 1
            },
            {
                "name": "title",
                "page_no": 0
            },
            {
                "name": "title",
                "page_no": 1
            }
        ]);
        var blobRows = conn.execute(`select name, data from vec_index where tbl="vindex" order by name asc`);
        assert.equal(blobRows[0].data.hex(), "");
        assert.equal(blobRows[1].data.hex(), "");

        res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[3,4,9]:1")`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
        assert.closeTo(res[0].distance, 0, 0.0001);
    });

    it("paged format keeps single blob below page size", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`update vec_index_meta set format = 1 where tbl="vindex"`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,4]", "[2,2,1]", 4)`);

        assert.deepEqual(conn.execute(`select name, format, page_size from vec_index_meta where tbl="vindex" order by name asc`), [
            {
                "name": "description",
                "format": 1,
                "page_size": 4096
            },
            {
                "name": "title",
                "format": 1,
                "page_size": 4096
            }
        ]);

        assert.deepEqual(conn.execute(`select * from vec_index_pages where tbl="vindex"`), []);

        var rows = conn.execute(`select name, data from vec_index where tbl="vindex" order by name asc`);
        assert.equal(rows.length, 2);
        assert.notEqual(rows[0].data.hex(), "");
        assert.notEqual(rows[1].data.hex(), "");

        assert.deepEqual(conn.execute(`select rowid from vindex order by rowid`), [
            {
                "rowid": 3
            },
            {
                "rowid": 4
            }
        ]);
    });

    it("paged auto page size adapts by dimension", () => {
        conn.execute("create virtual table vindex using vec_index(small(3), wide(512))");
        conn.execute(`update vec_index_meta set format = 1 where tbl="vindex"`);
        conn.execute(`insert into vindex(small, wide, rowid) values("[1,2,2]", "[1,2,3]", 3)`);

        var rows = conn.execute(`select name, format, page_size from vec_index_meta where tbl="vindex" order by name asc`);
        assert.equal(rows.length, 2);
        assert.equal(rows[0].format, 1);
        assert.equal(rows[1].format, 1);
        assert.equal(rows[0].name, "small");
        assert.equal(rows[1].name, "wide");
        assert.ok(rows[0].page_size < rows[1].page_size);
        assert.equal(rows[0].page_size, 4096);
        assert.equal(rows[1].page_size, 135168);

        assert.deepEqual(conn.execute(`select * from vec_index_pages where tbl="vindex"`), []);
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


