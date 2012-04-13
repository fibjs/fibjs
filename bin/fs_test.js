/**
 * 
 */

var assert = require('assert');
var os = require('os');

var fs = require('fs');

var isWindows = os.type() === 'Windows';

assert.ok(fs.exists('fs_test.js'));

function unlink(pathname) {
	try {
		fs.rmdir(pathname);
	} catch (e) {
	}
}

var pathname = 'test_dir';
var pathname1 = 'test1_dir';
unlink(pathname);
unlink(pathname1);

fs.mkdir(pathname);
assert.equal(fs.exists(pathname), true);

fs.rename(pathname, pathname1);
assert.equal(fs.exists(pathname), false);
assert.equal(fs.exists(pathname1), true);

fs.rmdir(pathname1);
assert.equal(fs.exists(pathname1), false);

var st = fs.stat('.');

assert.equal(st.isDirectory(), true);
assert.equal(st.isFile(), false);
assert.equal(st.isExecutable(), true);
assert.equal(st.isReadable(), true);
assert.equal(st.isWritable(), true);

var f = fs.open('fs_test.js');
assert.doesNotThrow(function() {
	f.read(100);
});
f.close();
assert.throws(function() {
	f.read(100);
});

f = fs.open('fs_test.js');

var st = fs.stat('fs_test.js');
assert.equal(st.size, f.size());

var f1 = fs.open('fs_test.js.bak', 'w');
f1.write(f.read());

f.rewind();
var b = f.read(st.size + 100);
assert.equal(st.size, b.length);
assert.equal(true, f.eof());

f.close();

st = fs.stat('fs_test.js');
assert.equal(st.size, f1.size());

assert.equal(st.size, f1.tell());
f1.rewind();
assert.equal(0, f1.tell());

f1.close();

fs.unlink('fs_test.js.bak');

