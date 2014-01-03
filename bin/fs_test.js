var test = require("test");
test.setup();

var fs = require('fs');

function unlink(pathname) {
	try {
		fs.rmdir(pathname);
	} catch (e) {}
}

var pathname = 'test_dir';
var pathname1 = 'test1_dir';

describe('fs', function() {
	before(function() {
		unlink(pathname);
		unlink(pathname1);
	});

	it("file open & close", function() {
		var f = fs.open('fs_test.js');
		assert.doesNotThrow(function() {
			f.read(100);
		});
		f.close();
		assert.throws(function() {
			f.read(100);
		});

		assert.ok(fs.exists('fs_test.js'));
	});

	it("mkdir", function() {
		fs.mkdir(pathname);
		assert.equal(fs.exists(pathname), true);
	});

	it("rename", function() {
		fs.rename(pathname, pathname1);
		assert.equal(fs.exists(pathname), false);
		assert.equal(fs.exists(pathname1), true);
	});

	it("rmdir", function() {
		fs.rmdir(pathname1);
		assert.equal(fs.exists(pathname1), false);
	});

	it("stat", function() {
		var st = fs.stat('.');

		assert.equal(st.isDirectory(), true);
		assert.equal(st.isFile(), false);
		assert.equal(st.isExecutable(), true);
		assert.equal(st.isReadable(), true);
		assert.equal(st.isWritable(), true);
	});

	it("file.size", function() {
		var f = fs.open('fs_test.js');
		var st = fs.stat('fs_test.js');
		assert.equal(st.size, f.size());
		f.close();
	});

	it("file read & write", function() {
		f = fs.open('fs_test.js');

		var f1 = fs.open('fs_test.js.bak', 'w+');
		f1.write(f.read(f.size()));

		f1.rewind();
		var b = f1.read(f1.size() + 100);
		assert.equal(true, f1.eof());
		assert.equal(f1.size(), b.length);

		f.close();
		f1.close();
		fs.unlink('fs_test.js.bak');
	});

	it("readFile", function() {
		f = fs.open('fs_test.js');

		var s = fs.readFile("fs_test.js");
		assert.equal(s, f.read(f.size()).toString());

		f.close();
	});

	it("openTextStream", function() {
		f = fs.openTextStream('fs_test.js');
		f.EOL = '\n';

		var a = fs.readFile("fs_test.js").replace(/\r/g, "").split("\n");
		assert.deepEqual(a, f.readLines());

		f.close();
	});

	it("tell", function() {
		f = fs.open('fs_test.js');
		var st = fs.stat('fs_test.js');

		f.read(f.size());

		assert.equal(st.size, f.tell());
		f.rewind();
		assert.equal(0, f.tell());

		f.close();
	});

	it("seek", function() {
		f = fs.open('fs_test.js');
		f.seek(f.size() + 10, fs.SEEK_SET);
		assert.equal(f.tell(), f.size() + 10);
		f.seek(10, fs.SEEK_SET);
		b = f.read(f.size());
		assert.equal(f.size() - 10, b.length);
		f.close();
	});

	it("copyTo", function() {
		f = fs.open('fs_test.js');
		f1 = fs.open('fs_test.js.bak', 'w');

		var s = f.copyTo(f1, 100);
		assert.equal(s, 100);
		assert.equal(f1.size(), 100);
		f.copyTo(f1);
		assert.equal(f1.size(), f.size());

		f.close();
		f1.close();

		fs.unlink('fs_test.js.bak');
	});

	it("readdir", function() {
		var fl = fs.readdir('vm_test');
		assert.equal(fl.length, 4);
		assert.equal(fl[2].name, 't1.js');
		assert.equal(fl[3].name, 't2.js');
	});
});

//test.run(console.DEBUG);