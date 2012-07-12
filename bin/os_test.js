console.log('os testing....');

var assert = require('assert');
var os = require('os');

console.log(os.CPUs());

assert.ok(os.exists('os_test.js'));

function unlink(pathname) {
	try {
		os.rmdir(pathname);
	} catch (e) {
	}
}

var pathname = 'test_dir';
var pathname1 = 'test1_dir';
unlink(pathname);
unlink(pathname1);

os.mkdir(pathname);
assert.equal(os.exists(pathname), true);

os.rename(pathname, pathname1);
assert.equal(os.exists(pathname), false);
assert.equal(os.exists(pathname1), true);

os.rmdir(pathname1);
assert.equal(os.exists(pathname1), false);

var st = os.stat('.');

assert.equal(st.isDirectory(), true);
assert.equal(st.isFile(), false);
assert.equal(st.isExecutable(), true);
assert.equal(st.isReadable(), true);
assert.equal(st.isWritable(), true);

assert.equal(os.time(), new Date());
