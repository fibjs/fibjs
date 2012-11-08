console.log('db testing....');

var db = require('db');
var assert = require('assert');

function t_db(conn) {
	try {
		conn.execute('drop table test;');
	} catch (e) {
	}

	conn
			.execute('create table test(t1 int, t2 varchar(128), t3 blob, t4 datetime);');
	conn.execute("insert into test values(?,?,?,?);", 1123, 'aaaaa',
			new Buffer('DDDDDDDDDD'), new Date('1998-04-14 12:12:12'));

	var rs = conn.execute('select * from test;');
	var r = rs[0];

	assert.equal(typeof r['t1'], 'number');
	assert.equal(typeof r['t2'], 'string');
	assert.equal(typeof r['t3'], 'object');
	assert.equal(typeof r['t4'], 'object');

	assert.strictEqual(r['t1'], 1123);
	assert.strictEqual(r['t2'], 'aaaaa');
	assert.strictEqual(r['t3'].toString(), 'DDDDDDDDDD');
	assert.strictEqual(r['t4'], new Date('1998-04-14 12:12:12'));

	var b = new Buffer();
	b.resize(1);

	for ( var i = 0; i < 256; i++) {
		b[0] = i;
		conn.execute("delete from test;");
		conn.execute("insert into test values(1,'aa', ?, ?);", b, new Date());
		rs = conn.execute("select * from test;");
		assert.equal(rs[0].t3.length, 1);
		assert.equal(rs[0].t3[0], i);
	}

	conn.execute('drop table test;');

	conn.close();
}

assert.equal('123456\\r\\n\'\'\\\"\\\x1acccds', db.escape(
		'123456\r\n\'\"\x1acccds', true));
assert.equal(db.formatMySQL("test?, ?, ?, ?", 123, 'ds\r\na', new Date(
		'1998-4-14 12:12:12')),
		"test'123', 'ds\\r\\na', '1998-04-14 12:12:12', ''");

assert.equal(db.formatMySQL("test?", [ 1, 2, 3, 4 ]), "test('1','2','3','4')");

t_db(db.open('sqlite:test.db'));
t_db(db.open('mysql://root@localhost/test'));

conn = db.open('sqlite:test.db');
conn.execute("create table ids(name, id)");
conn.execute("insert into ids(name, id) values('name', 0)")
conn.execute("select * from ids where id = 0");
conn.execute("drop table ids");
