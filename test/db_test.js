var test = require("test");
test.setup();

var db = require('db');
var fs = require('fs');

describe("db", function() {
	it("escape", function() {
		assert.equal('123456\\r\\n\'\'\\\"\\\x1acccds', db.escape(
			'123456\r\n\'\"\x1acccds', true));
	});

	it("formatMySQL", function() {
		assert.equal(db.formatMySQL("test?, ?, ?, ?", 123, 'ds\r\na',
				new Date('1998-4-14 12:12:12')),
			"test123, 'ds\\r\\na', '1998-04-14 12:12:12', ''");

		assert.equal(db.formatMySQL("test?", [1, 2, 3, 4]),
			"test(1,2,3,4)");
	});

	function _test(conn_str) {
		var conn;

		before(function() {
			conn = db.open(conn_str);
			try {
				conn.execute('drop table test;');
			} catch (e) {}
		});

		after(function() {
			conn.execute('drop table test;');
			conn.close();
		});

		it("empty sql", function() {
			assert.throws(function() {
				conn.execute("  ");
			})
		});

		it(
			"create table",
			function() {
				conn.execute('create table test(t1 int, t2 varchar(128), t3 blob, t4 datetime);');
			});

		it("insert", function() {
			conn.execute("insert into test values(?,?,?,?);", 1123,
				'aaaaa', new Buffer('DDDDDDDDDD'), new Date(
					'1998-04-14 12:12:12'));
		});

		it("select", function() {
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
		});

		it("binary", function() {
			var b = new Buffer();
			b.resize(1);

			for (var i = 0; i < 256; i++) {
				b[0] = i;
				conn.execute("delete from test;");
				conn.execute("insert into test values(1,'aa', ?, ?);",
					b, new Date());
				rs = conn.execute("select * from test;");
				assert.equal(rs[0].t3.length, 1);
				assert.equal(rs[0].t3[0], i);
			}
		});
	}

	describe("sqlite", function() {
		after(function() {
			fs.unlink("test.db");
		});
		_test('sqlite:test.db');
	});

	xdescribe("mysql", function() {
		_test('mysql://root@localhost/test');
	});


	describe("leveldb", function() {
		function clear_db() {
			fs.readdir("testdb").forEach(function(s) {
				if (s.name != "." && s.name != "..")
					fs.unlink("testdb/" + s.name);
			});

			fs.rmdir("testdb");
		}

		it('open/close', function() {
			var ldb = db.openLevelDB("testdb");
			ldb.close();
			clear_db();
		});

		it('put/get', function() {
			var b = new Buffer("bbbbb");
			var ldb = db.openLevelDB("testdb");
			ldb.put("test", b);
			assert.equal(ldb.get("test").toString(), "bbbbb");
			ldb.close();
			clear_db();
		});

		it('binary Key', function() {
			var b = new Buffer("bbbbb1");
			var ldb = db.openLevelDB("testdb");
			ldb.put(new Buffer("test1"), b);
			assert.equal(ldb.get(new Buffer("test1")).toString(), "bbbbb1");
			ldb.close();
			clear_db();
		});

		it('batch put', function() {
			var data = {
				"aaa": new Buffer("aaa value"),
				"bbb": new Buffer("bbb value"),
				"ccc": new Buffer("ccc value"),
				"ddd": new Buffer("ddd value")
			};

			var ldb = db.openLevelDB("testdb");
			ldb.put(data);
			assert.equal(ldb.get("aaa").toString(), "aaa value");
			assert.equal(ldb.get("bbb").toString(), "bbb value");
			assert.equal(ldb.get("ccc").toString(), "ccc value");
			assert.equal(ldb.get("ddd").toString(), "ddd value");
			ldb.close();
			clear_db();
		});

		it('remove/has', function() {
			var b = new Buffer("bbbbb");
			var ldb = db.openLevelDB("testdb");
			assert.isNull(ldb.get("not_exists"));
			assert.isFalse(ldb.has("not_exists"));
			ldb.put("not_exists", b);
			assert.isTrue(ldb.has("not_exists"));
			ldb.remove("not_exists");
			assert.isFalse(ldb.has("not_exists"));

			ldb.put("not_exists", b);
			assert.isTrue(ldb.has(new Buffer("not_exists")));
			ldb.remove(new Buffer("not_exists"));
			assert.isFalse(ldb.has("not_exists"));
			ldb.close();
			clear_db();
		});

		it('begin/commit', function() {
			var b = new Buffer("bbbbb");
			var c = new Buffer("ccccc");
			var ldb = db.openLevelDB("testdb");

			ldb.put("test", b);
			assert.equal(ldb.get("test").toString(), "bbbbb");

			var tr = ldb.begin();

			tr.put("test", c);
			assert.equal(ldb.get("test").toString(), "bbbbb");

			tr.commit();

			assert.equal(ldb.get("test").toString(), "ccccc");

			ldb.close();
			clear_db();
		});

		it('begin/close', function() {
			var b = new Buffer("bbbbb");
			var c = new Buffer("ccccc");
			var ldb = db.openLevelDB("testdb");

			ldb.put("test", b);
			assert.equal(ldb.get("test").toString(), "bbbbb");

			var tr = ldb.begin();

			tr.put("test", c);
			assert.equal(ldb.get("test").toString(), "bbbbb");

			tr.close();

			assert.equal(ldb.get("test").toString(), "bbbbb");

			ldb.close();
			clear_db();
		});

		it('forEach', function() {
			var data = {
				"ccc": new Buffer("ccc value"),
				"aaa": new Buffer("aaa value"),
				"bbb": new Buffer("bbb value"),
				"ddd": new Buffer("ddd value")
			};

			var ldb = db.openLevelDB("testdb");

			var count = 0;
			ldb.forEach(function(k, v) {
				count++;
			});
			assert.equal(count, 0);

			ldb.put(data);

			count = 0;
			ldb.forEach(function(k, v) {
				assert.equal(data[k].toString(), v.toString());
				delete data[k];
				count++;
			});
			assert.equal(count, 4);

			ldb.close();
			clear_db();
		});

		it('between', function() {
			var data = {
				"ccc": new Buffer("ccc value"),
				"aaa": new Buffer("aaa value"),
				"bbb": new Buffer("bbb value"),
				"ddd": new Buffer("ddd value")
			};

			var data1 = {
				"ccc": new Buffer("ccc value"),
				"bbb": new Buffer("bbb value")
			};

			var ldb = db.openLevelDB("testdb");
			ldb.put(data);

			var count = 0;
			ldb.between("bbb", "ddd", function(k, v) {
				assert.equal(data1[k].toString(), v.toString());
				delete data1[k];
				count++;
			});
			assert.equal(count, 2);

			ldb.close();
			clear_db();
		});

		it('break', function() {
			var data = {
				"ccc": new Buffer("ccc value"),
				"aaa": new Buffer("aaa value"),
				"bbb": new Buffer("bbb value"),
				"ddd": new Buffer("ddd value")
			};

			var data1 = {
				"aaa": new Buffer("aaa value"),
				"bbb": new Buffer("bbb value")
			};

			var ldb = db.openLevelDB("testdb");
			ldb.put(data);

			count = 0;
			ldb.forEach(function(k, v) {
				assert.equal(data1[k].toString(), v.toString());
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

//test.run(console.DEBUG);