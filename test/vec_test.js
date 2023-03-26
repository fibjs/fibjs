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

    it("create table", () => {
        conn.execute("create virtual table vindex using vec_index(title(128), description(128))");
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vindex_data'"), [
            {
                "name": "vindex_data"
            }
        ]);

        assert.deepEqual(conn.execute(`select rowid, * from vindex_data`), [
            {
                "rowid": 0,
                "name": "title",
                "dim": 128,
                "idx": null
            },
            {
                "rowid": 1,
                "name": "description",
                "dim": 128,
                "idx": null
            }
        ]);

        conn.execute("drop table vindex");
        assert.deepEqual(conn.execute("SELECT name FROM sqlite_master WHERE type='table' AND name = 'vindex_data'"), []);
    });

    it("insert", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "77d6883e77d6083fb2414d3f0100000000000000");
        assert.equal(res[1].idx.hex(), "2439d93ec3d0103ff404353f0100000000000000");

        assert.throws(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        });

        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);
        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "77d6883e77d6083fb2414d3f010000000000000077d6883e77d6083fb2414d3f0200000000000000");
        assert.equal(res[1].idx.hex(), "2439d93ec3d0103ff404353f01000000000000002439d93ec3d0103ff404353f0200000000000000");
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
        assert.deepEqual(conn.execute(`select rowid, distance from vindex where vec_search(title, "[1,2,5.1234]")`), [
            {
                "rowid": 4,
                "distance": 0
            },
            {
                "rowid": 3,
                "distance": 0.053202033042907715
            },
            {
                "rowid": 1,
                "distance": 0.0728190541267395
            },
            {
                "rowid": 2,
                "distance": 0.635004460811615
            }
        ]);
        assert.deepEqual(conn.execute(`select rowid, distance from vindex where vec_search(title, "[1,2,5.1234]:1")`), [{
            "rowid": 4,
            "distance": 0
        }]);
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

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "77d6883e77d6083fb2414d3f010000000000000077d6883e77d6083fb2414d3f0200000000000000");
        assert.equal(res[1].idx.hex(), "2439d93ec3d0103ff404353f01000000000000002439d93ec3d0103ff404353f0200000000000000");

        conn.execute(`update vindex set title="[1,2,4]" where rowid = 2`);

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "77d6883e77d6083fb2414d3f010000000000000082745f3e8274df3e82745f3f0200000000000000");
        assert.equal(res[1].idx.hex(), "2439d93ec3d0103ff404353f01000000000000002439d93ec3d0103ff404353f0200000000000000");
    });

    it("double insert in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);
            assert.throws(() => {
                conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);
            });
        });

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "77d6883e77d6083fb2414d3f0300000000000000");
        assert.equal(res[1].idx.hex(), "2439d93ec3d0103ff404353f0300000000000000");
    });

    it("insert after delete in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);

        conn.trans(() => {
            conn.execute(`delete from vindex where rowid = 3`);
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        });

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "abaaaa3eabaa2a3fabaa2a3f0300000000000000");
        assert.equal(res[1].idx.hex(), "009e163fabd2483fabd2483e0300000000000000");
    });

    it("delete after insert in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
            conn.execute(`delete from vindex where rowid = 3`);
        });

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "");
        assert.equal(res[1].idx.hex(), "");
    });

    it("load from disk db", () => {
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);

        conn.close();

        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "abaaaa3eabaa2a3fabaa2a3f0300000000000000");
        assert.equal(res[1].idx.hex(), "009e163fabd2483fabd2483e0300000000000000");

        assert.deepEqual(conn.execute(`select rowid from vindex`), [
            {
                "rowid": 3
            }
        ]);

        assert.throws(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        });

        conn.execute(`update vindex set title="[1,2,4]" where rowid = 3`);

        var res = conn.execute(`select rowid, * from vindex_data`);
        assert.equal(res.length, 2);
        assert.equal(res[0].idx.hex(), "82745f3e8274df3e82745f3f0300000000000000");
        assert.equal(res[1].idx.hex(), "009e163fabd2483fabd2483e0300000000000000");
    });

    it("benchmark", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        console.time("insert");
        conn.trans(() => {
            for (var i = 0; i < 4096; i++) {
                var title = [Math.random(), Math.random(), Math.random()];
                var description = [Math.random(), Math.random(), Math.random()];
                conn.execute(`insert into vindex(title, description, rowid) values("${JSON.stringify(title)}", "${JSON.stringify(description)}", ${i})`);
            }
        })
        console.timeEnd("insert");

        console.time("vec_search");
        var r1 = conn.execute(`select rowid, distance from vindex where vec_search(title, "[1,2,5.1234]") limit 10`);
        console.timeEnd("vec_search");

        console.time("vec_search limit");
        var r2 = conn.execute(`select rowid, distance from vindex where vec_search(title, "[1,2,5.1234]:10")`);
        console.timeEnd("vec_search limit");

        assert.deepEqual(r1, r2);
    });
});

require.main === module && test.run(console.DEBUG);
