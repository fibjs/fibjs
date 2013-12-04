var db = require('db');
var assert = require('assert');

var mdb = db.open("mongodb://127.0.0.1/test");

function clear() {
	try {
		mdb.test.drop();
	} catch (e) {
	}
}

clear();

var datas = [];

for (i = 0; i < 30; i++)
	datas[i] = {
		_id : i,
		data : 'data_' + i
	}

mdb.test.insert(datas);

assert.equal(mdb.test.find().skip(10).count(), 30);
assert.equal(mdb.test.find().skip(10).size(), 20);

assert.deepEqual(mdb.test.find().toJSON(), datas);
assert.deepEqual(mdb.test.findOne({
	_id : 3
}), datas[3]);
assert.deepEqual(mdb.test.find().skip(10).toJSON(), datas.slice(10));
assert.deepEqual(mdb.test.find().skip(10).limit(10).toJSON(), datas.slice(10, 20));

datas[0].data = 'new data';
mdb.test.save(datas[0]);

assert.deepEqual(mdb.test.find().sort({
	_id : 1
}).toJSON(), datas);

var r = mdb.test.findAndModify({
	query : {
		_id : 10
	},
	update : {
		data : "new data1"
	}
});

assert.deepEqual(r.value, datas[10]);
datas[10].data = "new data1";

assert.deepEqual(mdb.test.find().sort({
	_id : 1
}).toJSON(), datas);

clear();

mdb.test.ensureIndex({
	data : 1
});

mdb.fs.chunks.ensureIndex({
	files_id : 1,
	n : 1
}, {
	unique : true
});
mdb.fs.chunks.reIndex();

mdb.fs.files.ensureIndex({
	filename : 1
}, {
	unique : true
});

mdb.fs.files.reIndex();

// console.dir(mdb.test.getIndexes());
// mdb.test.reIndex();
// mdb.test.dropIndexes();

var fs = mdb.fs;
var data = new Buffer("this is a gridfs file.");

fs.store("test.txt", data);
assert.ok(fs.exists("test.txt"), "file created.");

assert.deepEqual(fs.retrieve("test.txt").readAll(), data);

fs.remove('test.txt');
assert.ok(!fs.exists("test.txt"), "file removed.");

//assert.equal(fs.retrieve("test.txt").readAll(), data);
