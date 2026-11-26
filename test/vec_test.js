var { describe, it, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var db = require('db');
var fs = require('fs');
var path = require('path');

function readFloats(buf, n) {
    var res = [];

    for (var i = 0; i < n; i++)
        res.push(buf.readFloatLE(i * 4).toFixed(6));

    return res;
}

describe("vec", () => {
    var conn;

    beforeEach(() => {
        conn = db.openSQLite(":memory:");
    });

    afterEach(() => {
        try { conn.close(); } catch (e) { }
        try { fs.unlink(path.join(__dirname, "vec_test.db")); } catch (e) { }
        try { fs.unlink(path.join(__dirname, "vec_test.db-wal")); } catch (e) { }
        try { fs.unlink(path.join(__dirname, "vec_test.db-shm")); } catch (e) { }
    });

    it("vec_version", () => {
        assert.equal(conn.execute("select vec_version() as v")[0].v, 'v2.0.0');
    });

    it("vec_distance", () => {
        var n = conn.execute(`select vec_distance("[1,2,3]", "[3,4,5]") as v`)[0].v.toFixed(6);
        assert.equal(n, 0.017292);

        var n = conn.execute(`select vec_distance("[1,2]", "[3,4,5]") as v`)[0].v.toFixed(6);
        var n1 = conn.execute(`select vec_distance("[1,2, 0]", "[3,4,5]") as v`)[0].v.toFixed(6);
        assert.equal(n, n1);
    });

    it("vec_distance_ip / vec_normalize", () => {
        var d1 = conn.execute(`select vec_distance("[1,2,3]", "[3,4,5]") as v`)[0].v;
        var d2 = conn.execute(
            `select vec_distance_ip(vec_normalize("[1,2,3]"), vec_normalize("[3,4,5]")) as v`)[0].v;
        assert.closeTo(d1, d2, 1e-5);

        var b = conn.execute(`select vec_normalize("[3,4]") as v`)[0].v;
        assert.ok(b instanceof Buffer);
        assert.closeTo(b.readFloatLE(0), 0.6, 1e-6);
        assert.closeTo(b.readFloatLE(4), 0.8, 1e-6);
    });

    it("create table", () => {
        conn.execute("create virtual table vindex using vec_index(title(128), description(128))");

        var names = conn.execute(`SELECT name FROM sqlite_master WHERE type='table'
            AND name IN ('vec_index_state','vec_index_part','vec_index_seg') ORDER BY name`)
            .map(r => r.name);
        assert.deepEqual(names, ['vec_index_part', 'vec_index_seg', 'vec_index_state']);

        var state = conn.execute(`select tbl, version, store_id from vec_index_state where tbl="vindex"`);
        assert.equal(state.length, 1);
        assert.equal(state[0].version, 0);
        assert.equal(state[0].store_id.length, 32);

        conn.execute("insert into vindex(title, description, rowid) values('[1,2]', '[3,4]', 1)");

        conn.execute("drop table vindex");
        assert.deepEqual(conn.execute(`select * from vec_index_part where tbl="vindex"`), []);
        assert.deepEqual(conn.execute(`select * from vec_index_seg where tbl="vindex"`), []);
        assert.deepEqual(conn.execute(`select * from vec_index_state where tbl="vindex"`), []);
    });

    it("insert", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);

        var res = conn.execute(`select rowid, title, description from vindex`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 1);
        assert.deepEqual(readFloats(res[0].title, 3), ["0.267261", "0.534522", "0.801784"]);
        assert.deepEqual(readFloats(res[0].description, 3), ["0.424264", "0.565685", "0.707107"]);

        assert.throws(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        }, { name: 'Error', number: 20009 });

        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);

        var res = conn.execute(`select rowid, title from vindex order by rowid`);
        assert.equal(res.length, 2);
        assert.equal(res[1].rowid, 2);
        assert.deepEqual(readFloats(res[1].title, 3), ["0.267261", "0.534522", "0.801784"]);
    });

    it("insert(binary vector)", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute('insert into vindex(title, description, rowid) values(?, ?, ?)',
            Buffer.from(Float32Array.from([1, 2, 3]).buffer),
            Buffer.from(Float32Array.from([3, 4, 5]).buffer), 1);

        var res = conn.execute(`select rowid, title, description from vindex`);
        assert.equal(res.length, 1);
        assert.deepEqual(readFloats(res[0].title, 3), ["0.267261", "0.534522", "0.801784"]);
        assert.deepEqual(readFloats(res[0].description, 3), ["0.424264", "0.565685", "0.707107"]);
    });

    it("select rowid", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);

        assert.deepEqual(conn.execute(`select rowid from vindex`), [
            { "rowid": 1 },
            { "rowid": 2 }
        ]);

        assert.deepEqual(conn.execute(`select rowid from vindex where rowid in (1,2)`), [
            { "rowid": 1 },
            { "rowid": 2 }
        ]);

        assert.deepEqual(conn.execute(`select rowid from vindex where rowid in (2,3)`), [
            { "rowid": 2 }
        ]);
    });

    it("search", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,0,0]", "[0,1,0]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[0,1,0]", "[0,0,1]", 2)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,1,0]", "[1,0,0]", 3)`);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[1,0,0]")`);
        assert.deepEqual(res.map(r => r.rowid), [1, 3, 2]);
        assert.equal(res[0].distance, 0);

        var res = conn.execute(`select rowid from vindex where vec_search(title, "[1,0,0]:1")`);
        assert.deepEqual(res, [{ "rowid": 1 }]);

        var res = conn.execute(`select rowid from vindex where vec_search(description, "[0,1,0]:1")`);
        assert.deepEqual(res, [{ "rowid": 1 }]);
    });

    it("partitioned table: key and order", () => {
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key, ordinal order)");

        conn.trans(() => {
            for (var d = 0; d < 3; d++)
                for (var i = 0; i < 4; i++)
                    conn.execute('insert into kv(rowid, emb, doc_id, ordinal) values(?, ?, ?, ?)',
                        d * 10 + i + 1, JSON.stringify([1 + d, 1 + i, 1]), 'd' + d, i);
        });

        var parts = conn.execute(`select key, count from vec_index_part where tbl="kv" order by key`);
        assert.deepEqual(parts, [
            { "key": "d0", "count": 4 },
            { "key": "d1", "count": 4 },
            { "key": "d2", "count": 4 }
        ]);

        var q = conn.execute(`select vec_normalize("[0.5,0.5,1]") as v`)[0].v;

        var ref = conn.execute(
            `select rowid, vec_distance_ip(emb, ?) as d, ordinal from kv
             where doc_id in ('d1','d2') order by d, ordinal, rowid limit 5`, q);
        var got = conn.execute(
            `select rowid, distance from kv where vec_search(emb, ?)
             and doc_id in ('d1','d2') order by distance limit 5`, q);

        assert.deepEqual(got.map(r => r.rowid), ref.map(r => r.rowid));
        for (var i = 0; i < ref.length; i++)
            assert.equal(got[i].distance, ref[i].d);

        var one = conn.execute(
            `select rowid, distance from kv where vec_search(emb, ?) and doc_id = 'd2' limit 2`, q);
        assert.equal(one.length, 2);
        for (var r of one)
            assert.ok(r.rowid > 20);

        var all = conn.execute(`select rowid from kv where vec_search(emb, ?) limit 3`, q);
        assert.equal(all.length, 3);
    });

    it("order key breaks ties", () => {
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key, ordinal order)");
        conn.execute(`insert into kv(rowid, emb, doc_id, ordinal) values(1, "[1,0,0]", 'a', 2)`);
        conn.execute(`insert into kv(rowid, emb, doc_id, ordinal) values(2, "[1,0,0]", 'a', 1)`);

        var res = conn.execute(`select rowid, ordinal from kv where vec_search(emb, "[1,0,0]") limit 2`);
        assert.deepEqual(res.map(r => r.rowid), [2, 1]);

        var res = conn.execute(`select rowid, ordinal from kv where vec_search(emb, "[1,0,0]")
            order by distance, ordinal limit 2`);
        assert.deepEqual(res.map(r => r.rowid), [2, 1]);
    });

    it("update", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 2)`);

        conn.execute(`update vindex set title="[1,2,4]" where rowid = 2`);

        var res = conn.execute(`select rowid, title, description from vindex order by rowid`);
        assert.equal(res.length, 2);
        assert.deepEqual(readFloats(res[0].title, 3), ["0.267261", "0.534522", "0.801784"]);
        assert.deepEqual(readFloats(res[1].title, 3), ["0.218218", "0.436436", "0.872872"]);
        assert.deepEqual(readFloats(res[1].description, 3), ["0.424264", "0.565685", "0.707107"]);
    });

    it("update after delete", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[1,1,1]", 1)`);
        conn.execute(`insert into vindex(title, description, rowid) values("[4,5,6]", "[1,1,1]", 2)`);

        conn.execute(`delete from vindex where rowid = 1`);
        conn.execute(`update vindex set title="[7,8,9]" where rowid = 2`);

        assert.deepEqual(conn.execute(`select rowid from vindex`), [{ "rowid": 2 }]);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[7,8,9]:1")`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 2);
        assert.closeTo(res[0].distance, 0, 0.0001);
    });

    it("update rowid", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,0,0]", "[1,1,1]", 10)`);

        conn.execute(`update vindex set rowid = 20 where rowid = 10`);

        assert.deepEqual(conn.execute(`select rowid from vindex`), [{ "rowid": 20 }]);
        assert.deepEqual(conn.execute(`select rowid from vindex where rowid = 10`), []);
    });

    it("update partition key", () => {
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key, ordinal order)");
        conn.execute(`insert into kv(rowid, emb, doc_id, ordinal) values(1, "[1,0,0]", 'a', 0)`);
        conn.execute(`update kv set doc_id = 'b' where rowid = 1`);

        assert.deepEqual(conn.execute(`select key, count from vec_index_part where tbl="kv"`),
            [{ "key": "b", "count": 1 }]);

        assert.deepEqual(conn.execute(`select rowid from kv where vec_search(emb, "[1,0,0]") and doc_id = 'a'`), []);
        assert.deepEqual(conn.execute(`select rowid from kv where vec_search(emb, "[1,0,0]") and doc_id = 'b'`),
            [{ "rowid": 1 }]);
    });

    it("double insert in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);
            assert.throws(() => {
                conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);
            }, { name: 'Error', number: 20009 });
        });

        var res = conn.execute(`select rowid, title from vindex`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
        assert.deepEqual(readFloats(res[0].title, 3), ["0.267261", "0.534522", "0.801784"]);
    });

    it("insert after delete in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,3]", "[3,4,5]", 3)`);

        conn.trans(() => {
            conn.execute(`delete from vindex where rowid = 3`);
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        });

        var res = conn.execute(`select rowid, title, description from vindex`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
        assert.deepEqual(readFloats(res[0].title, 3), ["0.333333", "0.666667", "0.666667"]);
        assert.deepEqual(readFloats(res[0].description, 3), ["0.588348", "0.784465", "0.196116"]);
    });

    it("delete after insert in trans", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
            conn.execute(`delete from vindex where rowid = 3`);
        });

        assert.deepEqual(conn.execute(`select count(*) as c from vindex`), [{ "c": 0 }]);
    });

    it("segment layout", () => {
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            for (var rowid = 1; rowid <= 70; rowid++)
                conn.execute(`insert into vindex(title, description, rowid) values(?, ?, ?)`,
                    JSON.stringify([rowid, rowid + 1, rowid + 2]),
                    JSON.stringify([rowid + 3, rowid + 4, rowid + 5]), rowid);
        });

        var segs = conn.execute(`select name, count, cap, length(vecs) as vlen, length(meta) as mlen
            from vec_index_seg where tbl="vindex" order by name`);
        assert.equal(segs.length, 2);

        for (var s of segs) {
            assert.equal(s.count, 70);
            assert.equal(s.cap, 128);
            assert.equal(s.vlen, 128 * 16 * 4);
            assert.equal(s.mlen, 128 * 16);
        }

        var res = conn.execute(`select count(*) as c from vindex`);
        assert.equal(res[0].c, 70);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[3,4,5]:1")`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
    });

    it("load from disk db", () => {
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");
        conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        conn.close();

        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        var res = conn.execute(`select rowid, title, description from vindex`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
        assert.deepEqual(readFloats(res[0].title, 3), ["0.333333", "0.666667", "0.666667"]);
        assert.deepEqual(readFloats(res[0].description, 3), ["0.588348", "0.784465", "0.196116"]);

        assert.throws(() => {
            conn.execute(`insert into vindex(title, description, rowid) values("[1,2,2]", "[3,4,1]", 3)`);
        }, { name: 'Error', number: 20009 });

        conn.execute(`update vindex set title="[1,2,4]" where rowid = 3`);
        conn.close();

        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        var res = conn.execute(`select rowid, title, description from vindex`);
        assert.equal(res.length, 1);
        assert.deepEqual(readFloats(res[0].title, 3), ["0.218218", "0.436436", "0.872872"]);
        assert.deepEqual(readFloats(res[0].description, 3), ["0.588348", "0.784465", "0.196116"]);
    });

    it("load from disk db (segment layout)", () => {
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));
        conn.execute("create virtual table vindex using vec_index(title(3), description(3))");

        conn.trans(() => {
            for (var rowid = 1; rowid <= 300; rowid++)
                conn.execute(`insert into vindex(title, description, rowid) values(?, ?, ?)`,
                    JSON.stringify([rowid, rowid + 1, rowid + 2]),
                    JSON.stringify([rowid + 3, rowid + 4, rowid + 5]), rowid);
        });

        var segs = conn.execute(`select name, count, cap from vec_index_seg where tbl="vindex" order by name`);
        assert.equal(segs.length, 2);

        for (var s of segs) {
            assert.equal(s.count, 300);
            assert.equal(s.cap, 512);
        }

        conn.close();
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        assert.deepEqual(conn.execute(`select count(*) as count from vindex`), [{ "count": 300 }]);

        var res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[3,4,5]:1")`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
        assert.closeTo(res[0].distance, 0, 0.0001);

        conn.execute(`update vindex set title="[3,4,9]", description="[6,7,8]" where rowid = 3`);
        conn.close();
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));

        res = conn.execute(`select rowid, distance from vindex where vec_search(title, "[3,4,9]:1")`);
        assert.equal(res.length, 1);
        assert.equal(res[0].rowid, 3);
        assert.closeTo(res[0].distance, 0, 0.0001);

        var segs = conn.execute(`select count, cap from vec_index_seg where tbl="vindex" and name="title"`);
        assert.deepEqual(segs, [{ "count": 300, "cap": 512 }]);
    });

    it("lazy segment load before update", () => {
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));
        conn.execute("create virtual table vindex using vec_index(title(3), doc_id key, ordinal order)");
        conn.execute(`insert into vindex(rowid, title, doc_id, ordinal) values(1, "[1,0,0]", "a", 1)`);
        conn.execute(`insert into vindex(rowid, title, doc_id, ordinal) values(2, "[0,1,0]", "a", 2)`);
        conn.close();

        // Reopen and update without reading the vector columns first: the
        // segment vectors are loaded lazily, and both an updated vector and a
        // key/order-only update must not touch a null buffer.
        conn = db.openSQLite(path.join(__dirname, "vec_test.db"));
        conn.execute('update vindex set title = ? where rowid = 2',
            Buffer.from(Float32Array.from([0, 0, 1]).buffer));
        conn.execute(`update vindex set title = null, doc_id = 'b' where rowid = 1`);

        assert.deepEqual(conn.execute(`select rowid, doc_id from vindex order by rowid`), [
            { "rowid": 1, "doc_id": "b" },
            { "rowid": 2, "doc_id": "a" }
        ]);

        var res = conn.execute(`select rowid from vindex where vec_search(title, "[0,0,1]:1")`);
        assert.deepEqual(res, [{ "rowid": 2 }]);
    });

    it("segment capacity adapts by dimension", () => {
        conn.execute("create virtual table vindex using vec_index(small(3), wide(512))");
        conn.execute(`insert into vindex(small, wide, rowid) values("[1,2,2]", "[1,2,3]", 3)`);

        var segs = conn.execute(`select name, count, cap, length(vecs) as vlen from vec_index_seg
            where tbl="vindex" order by name`);
        assert.equal(segs.length, 2);
        assert.deepEqual(segs.map(s => s.name), ["small", "wide"]);
        assert.equal(segs[0].cap, 64);
        assert.equal(segs[1].cap, 64);
        assert.equal(segs[0].vlen, 64 * 16 * 4);
        assert.equal(segs[1].vlen, 64 * 512 * 4);

        var res = conn.execute(`select rowid from vindex`);
        assert.deepEqual(res, [{ "rowid": 3 }]);
    });

    it("strict constraints", () => {
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key, ordinal order)");
        conn.execute(`insert into kv(rowid, emb, doc_id, ordinal) values(1, "[1,0,0]", 'a', 0)`);

        var q = conn.execute(`select vec_normalize("[1,0,0]") as v`)[0].v;

        assert.throws(() => {
            conn.execute(`select rowid from kv where vec_search(emb, ?) and ordinal < 3`, q);
        }, { name: 'Error', number: 20009 });

        assert.throws(() => {
            conn.execute(`select rowid from kv where vec_search(emb, ?) and doc_id like 'a%'`, q);
        }, { name: 'Error', number: 20009 });

        assert.throws(() => {
            conn.execute(`insert into kv(rowid, emb) values(2, "[1,0,0]")`);
        }, { name: 'Error', number: 20009 });

        // Compatibility with the v1 planner: rowid equality next to
        // vec_search runs the search and post-filters by rowid.
        assert.deepEqual(
            conn.execute(`select rowid from kv where vec_search(emb, ?) and rowid = 1`, q),
            [{ "rowid": 1 }]);
        assert.deepEqual(
            conn.execute(`select rowid from kv where vec_search(emb, ?) and rowid = 2`, q),
            []);

        assert.throws(() => {
            conn.execute(`select rowid from kv where vec_search(emb, ?) limit 1 offset 1`, q);
        }, { name: 'Error', number: 20009 });

        // rowid IN cannot be filled back to k after the post-filter, so it
        // stays rejected.
        assert.throws(() => {
            conn.execute(`select rowid from kv where vec_search(emb, ?) and rowid in (1, 2)`, q);
        }, { name: 'Error', number: 20009 });

        // Evaluating the constraint as an ordinary scalar must not silently
        // return NULL (zero rows).
        assert.throws(() => {
            conn.execute(`select vec_search(emb, ?) as v from kv`, q);
        }, { name: 'Error', number: 20009 });
    });

    it("rowid join with vec_search uses the search plan", () => {
        // Regression: v2 used to reject the statement while planning because
        // the join term `m.rowid = v.rowid` and vec_search were both offered
        // to xBestIndex.  It must instead plan as a search and let SQLite
        // post-filter the top-k rows by rowid.
        conn.execute("create virtual table legacy using vec_index(emb(3))");
        conn.execute("create table legacy_map(rowid integer primary key, tag text)");
        conn.execute(`insert into legacy(rowid, emb) values(1, "[1,0,0]")`);
        conn.execute(`insert into legacy(rowid, emb) values(2, "[0.9,0.1,0]")`);
        conn.execute(`insert into legacy(rowid, emb) values(3, "[0,1,0]")`);
        conn.execute(`insert into legacy_map(rowid, tag) values(1, 'a')`);
        conn.execute(`insert into legacy_map(rowid, tag) values(2, 'b')`);
        conn.execute(`insert into legacy_map(rowid, tag) values(3, 'c')`);

        var rows = conn.execute(`select m.tag as tag, v.distance as distance
            from legacy v
            inner join legacy_map m on m.rowid = v.rowid
            where vec_search(v.emb, '[1,0,0]:2')
            order by v.distance`);

        assert.deepEqual(rows.map(r => r.tag), ['a', 'b']);
    });

    it("vec_stats", () => {
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key)");
        conn.execute(`insert into kv(rowid, emb, doc_id) values(1, "[1,0,0]", 'a')`);
        conn.execute(`insert into kv(rowid, emb, doc_id) values(2, "[0,1,0]", 'b')`);

        var s = JSON.parse(conn.execute(`select vec_stats('kv') as s`)[0].s);
        assert.equal(s.rows, 2);
        assert.equal(s.partitions, 2);
        assert.equal(s.segments, 2);
        assert.ok(s.bytes > 0);
        assert.equal(s.cache_limit, 64 * 1024 * 1024);
        assert.ok(s.cache_bytes >= 0);
        assert.ok(s.cache_segments >= 0);
        assert.ok(s.cache_hits >= 0);
        assert.ok(s.cache_misses >= 0);
        assert.ok(s.cache_evictions >= 0);
        assert.ok(s.cache_loads >= 0);
    });

    it("cache limit evicts and reloads", () => {
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key, ordinal order)");
        conn.trans(() => {
            for (var d = 0; d < 4; d++)
                for (var i = 0; i < 100; i++)
                    conn.execute('insert into kv(rowid, emb, doc_id, ordinal) values(?,?,?,?)',
                        d * 100 + i + 1, JSON.stringify([d + 1, i + 1, 1]), 'doc' + d, i);
        });

        var q = conn.execute('select vec_normalize("[1,2,3]") as v')[0].v;
        var sql = 'select rowid, distance from kv where vec_search(emb, ?) order by distance limit 10';
        var r1 = conn.execute(sql, q);

        var s = JSON.parse(conn.execute(`select vec_stats('kv') as s`)[0].s);
        assert.ok(s.cache_bytes > 0);
        assert.ok(s.cache_segments > 0);
        assert.ok(s.cache_hits + s.cache_misses > 0);
        assert.equal(s.cache_limit, 64 * 1024 * 1024);

        // Shrink the budget to a single byte: the next statement has to evict
        // every unpinned segment and reload them one at a time.
        assert.equal(conn.execute("select vec_cache_limit('kv', 1) as v")[0].v, 1);

        var r2 = conn.execute(sql, q);
        assert.deepEqual(r2, r1);

        s = JSON.parse(conn.execute(`select vec_stats('kv') as s`)[0].s);
        assert.equal(s.cache_limit, 1);
        assert.ok(s.cache_evictions > 0);
        assert.ok(s.cache_loads > 0);
        assert.equal(s.cache_bytes, 0);
        assert.equal(s.cache_segments, 0);

        assert.equal(conn.execute('select vec_cache_limit(?, ?) as v', 'kv', 64 * 1024 * 1024)[0].v,
            64 * 1024 * 1024);
    });

    it("cache bypass keeps hot segments", () => {
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key, ordinal order)");
        conn.trans(() => {
            for (var d = 0; d < 4; d++)
                for (var i = 0; i < 100; i++)
                    conn.execute('insert into kv(rowid, emb, doc_id, ordinal) values(?,?,?,?)',
                        d * 100 + i + 1, JSON.stringify([d + 1, i + 1, 1]), 'doc' + d, i);
        });

        var q = conn.execute('select vec_normalize("[1,2,3]") as v')[0].v;
        var hot = "select rowid, distance from kv where vec_search(emb, ?) and doc_id = 'doc0' order by distance limit 10";
        var all = 'select rowid, distance from kv where vec_search(emb, ?) order by distance limit 10';

        // One doc = 100 rows = one segment of cap 128, i.e. 8KB of vectors.
        // Shrink the budget first, then prime the hot doc: the cache now holds
        // exactly that segment.
        var limit = 8 * 1024;
        conn.execute('select vec_cache_limit(?, ?) as v', 'kv', limit);

        var r0 = conn.execute(hot, q);
        var s = JSON.parse(conn.execute(`select vec_stats('kv') as s`)[0].s);
        assert.ok(s.cache_bytes > 0);
        assert.ok(s.cache_bytes <= limit);

        // The 32KB full scan must stream its misses instead of evicting the
        // hot segment; that segment is still a cache hit during the scan.
        var hits = s.cache_hits;
        conn.execute(all, q);

        s = JSON.parse(conn.execute(`select vec_stats('kv') as s`)[0].s);
        assert.ok(s.cache_bytes > 0);
        assert.ok(s.cache_bytes <= limit);
        assert.ok(s.cache_loads > 0);
        assert.equal(s.cache_hits, hits + 1);

        assert.deepEqual(conn.execute(hot, q), r0);
        s = JSON.parse(conn.execute(`select vec_stats('kv') as s`)[0].s);
        assert.ok(s.cache_hits > hits + 1);

        conn.execute('select vec_cache_limit(?, ?) as v', 'kv', 64 * 1024 * 1024);
    });

    it("vec_cache_limit clamps and reports", () => {
        conn.execute("create virtual table kv using vec_index(emb(3))");
        conn.execute(`insert into kv(rowid, emb) values(1, "[1,0,0]")`);

        var cur = conn.execute("select vec_cache_limit('kv') as v")[0].v;
        assert.ok(cur >= 64 * 1024 * 1024);

        var big = 1024 * 1024 * 1024;
        var got = conn.execute('select vec_cache_limit(?, ?) as v', 'kv', big)[0].v;

        // A 32-bit process is bounded by its address space, not by RAM.
        if (require('os').arch() === 'ia32' || require('os').arch() === 'arm')
            assert.equal(got, 256 * 1024 * 1024);
        else
            assert.equal(got, big);

        assert.equal(conn.execute('select vec_cache_limit(?, ?) as v', 'kv', 64 * 1024 * 1024)[0].v,
            64 * 1024 * 1024);

        assert.throws(() => conn.execute("select vec_cache_limit('missing') as v"),
            { number: 20009 });
        assert.throws(() => conn.execute('select vec_cache_limit(1) as v'),
            { number: 20009 });
    });

    it("auto budgets share one process-wide RAM bound", () => {
        // The default budget is "what is left of the RAM bound once the
        // resident metadata is paid for", split between the stores that want
        // it, so several tables cannot each claim half of memory.  The bound is
        // fixed at startup (FIBJS_VEC_RAM_LIMIT), hence the child process.
        var os = require('os');
        var child_process = require('child_process');
        var scriptFile = path.join(os.tmpdir(), "vec_budget_child_" + process.pid + ".js");

        fs.writeFile(scriptFile, `
var db = require('db');
var fs = require('fs');
var os = require('os');
var path = require('path');

var file = path.join(os.tmpdir(), 'vec_budget_child.db');
['', '-wal', '-shm'].forEach((s) => { try { fs.unlink(file + s); } catch (e) { } });

var conn = db.openSQLite(file);
var vec = JSON.stringify([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8,
    0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6]);

conn.execute("create virtual table a using vec_index(emb(16))");
conn.execute("create virtual table b using vec_index(emb(16))");
conn.execute('begin');

for (var n = 1; n <= 20000; n++) {
    conn.execute('insert into a(rowid, emb) values(?, ?)', n, vec);
    conn.execute('insert into b(rowid, emb) values(?, ?)', n, vec);
}

conn.execute('commit');

function stats(t) {
    return JSON.parse(conn.execute('select vec_stats(?) as s', t)[0].s);
}

function search(t) {
    conn.execute('select rowid, distance from ' + t +
        ' where vec_search(emb, ?) order by distance limit 10', vec);
}

search('a');
search('b');

var a = stats('a'), b = stats('b');

conn.close();
['', '-wal', '-shm'].forEach((s) => { try { fs.unlink(file + s); } catch (e) { } });

console.log(JSON.stringify({
    limit_a: a.cache_limit, limit_b: b.cache_limit,
    bytes_a: a.bytes, bytes_b: b.bytes
}));
`);

        try {
            var ret = child_process.execFile(process.execPath, [scriptFile], {
                env: { FIBJS_VEC_RAM_LIMIT: String(1024 * 1024) }
            });

            var r = JSON.parse(ret.stdout.trim());

            // Both stores stay well inside the bound instead of each asking for
            // their whole index (2 * 1.6MB fits easily, so without the shared
            // bound both would report the full index)...
            assert.ok(r.limit_a > 0);
            assert.ok(r.limit_b > 0);
            assert.ok(r.limit_a < r.bytes_a);
            assert.ok(r.limit_b < r.bytes_b);

            // ...the vector budgets never exceed the bound but are visibly
            // below it, because the resident metadata is charged first.
            assert.ok(r.limit_a + r.limit_b <= 512 * 1024);

            // ...and they get comparable shares, since they want the same.
            assert.ok(Math.abs(r.limit_a - r.limit_b) <= 4096);
        } finally {
            try { fs.unlink(scriptFile); } catch (e) { }
        }
    });

    it("vec_cache_limit before the table is opened", () => {
        var dbFile = path.join(__dirname, "vec_test.db");

        conn.close();
        conn = db.openSQLite(dbFile);
        conn.execute("create virtual table kv using vec_index(emb(3), doc_id key, ordinal order)");
        conn.execute(`insert into kv(rowid, emb, doc_id, ordinal) values(1, "[1,0,0]", "d1", 0)`);
        conn.close();

        // The store only exists once the vtab has been opened, but a budget can
        // still be configured first: the value is held until the table opens.
        conn = db.openSQLite(dbFile);
        assert.equal(conn.execute("select vec_cache_limit('kv', 4096) as v")[0].v, 4096);
        assert.equal(conn.execute("select vec_cache_limit('kv') as v")[0].v, 4096);

        // Reading the budget of a table that was never opened reports that,
        // instead of pretending the budget is unlimited.
        assert.throws(() => conn.execute("select vec_cache_limit('other') as v"),
            { number: 20009 });

        // The first query applies it, so nothing is cached under the default
        // budget in between.
        assert.deepEqual(conn.execute('select rowid from kv'), [{ "rowid": 1 }]);
        assert.equal(conn.execute("select vec_cache_limit('kv') as v")[0].v, 4096);

        var s = JSON.parse(conn.execute(`select vec_stats('kv') as s`)[0].s);
        assert.equal(s.cache_limit, 4096);
    });

    it("shared cache refreshes across connections", () => {
        var dbFile = path.join(__dirname, "vec_test.db");

        conn.close();
        conn = db.openSQLite(dbFile);

        conn.execute("create virtual table vindex using vec_index(title(3))");
        conn.execute(`insert into vindex(title, rowid) values("[1,0,0]", 1)`);

        var conn2 = db.openSQLite(dbFile);

        assert.deepEqual(conn2.execute(`select rowid from vindex`), [{ "rowid": 1 }]);

        conn.execute(`insert into vindex(title, rowid) values("[0,1,0]", 2)`);
        assert.deepEqual(conn2.execute(`select rowid from vindex order by rowid`),
            [{ "rowid": 1 }, { "rowid": 2 }]);

        conn.execute(`delete from vindex where rowid = 1`);
        assert.deepEqual(conn2.execute(`select rowid from vindex`), [{ "rowid": 2 }]);

        conn2.close();
    });

    it("drop and recreate does not reuse stale data", () => {
        conn.execute("create virtual table vindex using vec_index(title(3))");
        conn.execute(`insert into vindex(title, rowid) values("[1,0,0]", 1)`);

        conn.execute("drop table vindex");
        conn.execute("create virtual table vindex using vec_index(title(3))");

        assert.deepEqual(conn.execute(`select count(*) as c from vindex`), [{ "c": 0 }]);

        conn.execute(`insert into vindex(title, rowid) values("[0,1,0]", 2)`);
        assert.deepEqual(conn.execute(`select rowid from vindex`), [{ "rowid": 2 }]);
    });

    it("benchmark", () => {
        conn.execute("create virtual table kv using vec_index(emb(128), doc_id key, ordinal order)");

        var docs = 50, per = 40;
        conn.trans(() => {
            for (var d = 0; d < docs; d++) {
                var doc = 'doc' + d;
                for (var i = 0; i < per; i++) {
                    var v = [];
                    for (var k = 0; k < 128; k++)
                        v.push(Math.sin(d * 7 + i * 3 + k));
                    conn.execute('insert into kv(rowid, emb, doc_id, ordinal) values(?, ?, ?, ?)',
                        d * per + i + 1, JSON.stringify(v), doc, i);
                }
            }
        });

        var q = [];
        for (var k = 0; k < 128; k++)
            q.push(Math.cos(k));
        var qb = conn.execute('select vec_normalize(?) as v', JSON.stringify(q))[0].v;

        var t0 = Date.now();
        var res = conn.execute(`select rowid, distance from kv where vec_search(emb, ?)
            and doc_id in ('doc1','doc2','doc3') order by distance limit 10`, qb);
        var dt = Date.now() - t0;

        assert.equal(res.length, 10);
        assert.ok(dt < 1000, 'filtered search too slow: ' + dt + 'ms');

        t0 = Date.now();
        var res = conn.execute(`select rowid, distance from kv where vec_search(emb, ?) limit 10`, qb);
        dt = Date.now() - t0;
        assert.equal(res.length, 10);
        console.log('   benchmark: 3 docs ' + dt + 'ms, rows ' + (docs * per));
    });
});
