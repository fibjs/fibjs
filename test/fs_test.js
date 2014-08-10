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

var win = require("os").type == "Windows";

describe('fs', function() {
	before(function() {
		if (!win) fs.umask(0);
		unlink(pathname);
		unlink(pathname1);
	});

	if (!win)
		it("umask", function() {
			assert.equal(fs.umask(18), 0);
			assert.equal(fs.umask(0), 18);
		});

	it("stat", function() {
		var st = fs.stat('.');

		assert.equal(st.isDirectory(), true);
		assert.equal(st.isFile(), false);
		assert.equal(st.isExecutable(), true);
		assert.equal(st.isReadable(), true);
		assert.equal(st.isWritable(), true);
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
		fs.mkdir(pathname, 511);
		assert.equal(fs.exists(pathname), true);

		if (!win) {
			var st = fs.stat(pathname);
			assert.equal(st.mode & 511, 511);
		}
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

	it("file.size", function() {
		var f = fs.open('fs_test.js');
		var st = fs.stat('fs_test.js');
		assert.equal(st.size, f.size());
		f.close();
	});

	if (!win) {
		it("fs.chmod", function() {
			var st = fs.stat('fs_test.js');
			var oldm = st.mode;

			fs.chmod('fs_test.js', 511);
			var st = fs.stat('fs_test.js');
			assert.equal(st.mode & 511, 511);

			fs.chmod('fs_test.js', oldm);
			var st = fs.stat('fs_test.js');
			assert.equal(st.mode & 511, oldm & 511);
		});

		it("file.chmod", function() {
			var f = fs.open('fs_test.js');
			var st = fs.stat('fs_test.js');
			var oldm = st.mode;

			f.chmod(511);
			var st = fs.stat('fs_test.js');
			assert.equal(st.mode & 511, 511);

			f.chmod(oldm);
			var st = fs.stat('fs_test.js');
			assert.equal(st.mode & 511, oldm & 511);

			f.close();
		});
	}

	it("async gc", function() {
		var f = fs.open('fs_test.js');
		var num = 0;

		(function() {
			num = 1;
		}).start();
		f = undefined;
		assert.equal(num, 0);
		GC();
		assert.equal(num, 1);
	});

	it("file read & write", function() {
		var f = fs.open('fs_test.js');

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
		var f = fs.open('fs_test.js');

		var s = fs.readFile("fs_test.js");
		assert.equal(s, f.read(f.size()).toString());

		f.close();
	});

	it("openTextStream", function() {
		var f = fs.openTextStream('fs_test.js');
		f.EOL = '\n';

		var a = fs.readFile("fs_test.js").replace(/\r/g, "").split("\n");
		assert.deepEqual(a, f.readLines());

		f.close();
	});

	it("tell", function() {
		var f = fs.open('fs_test.js');
		var st = fs.stat('fs_test.js');

		f.read(f.size());

		assert.equal(st.size, f.tell());
		f.rewind();
		assert.equal(0, f.tell());

		f.close();
	});

	it("seek", function() {
		var f = fs.open('fs_test.js');
		f.seek(f.size() + 10, fs.SEEK_SET);
		assert.equal(f.tell(), f.size() + 10);
		f.seek(10, fs.SEEK_SET);
		var b = f.read(f.size());
		assert.equal(f.size() - 10, b.length);
		f.close();
	});

	it("seek 64 bits", function() {
		var f = fs.open('fs_test.js');
		f.seek(f.size() + 8589934592, fs.SEEK_SET);
		assert.equal(f.tell(), f.size() + 8589934592);
		f.close();
	});

	it("copyTo", function() {
		var f = fs.open('fs_test.js');
		var f1 = fs.open('fs_test.js.bak', 'w');

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
		var fl = fs.readdir('vm_test').toJSON();
		fl.sort(function(a, b) {
			if (a.name > b.name)
				return 1;
			if (a.name < b.name)
				return -1;
			return 0;
		});
		var sz = fl.length;
		assert.greaterThan(sz, 3);
		assert.equal(fl[sz - 2].name, 't1.js');
		assert.equal(fl[sz - 1].name, 't2.js');
	});
});

//test.run(console.DEBUG);