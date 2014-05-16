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
});

// test.run(console.DEBUG);