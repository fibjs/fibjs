/**
 * 
 */

var assert = require('assert');
var os = require('os');

var fs = require('fs');
/*
var isWindows = os.type() === 'Windows';

assert.ok(fs.exists('fs.js'));

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
*/
var st = fs.stat('.');

for (var p in st)
	console.log(p, st[p], st.hasOwnProperty(p));

console.dir({st:st});
