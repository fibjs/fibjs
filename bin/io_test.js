/**
 * 
 */

console.log('io testing....');

var assert = require('assert');

var io = require('io');
var os = require('os');

var f = io.open('io_test.js');
assert.doesNotThrow(function() {
	f.read(100);
});
f.close();
assert.throws(function() {
	f.read(100);
});

f = io.open('io_test.js');

var st = os.stat('io_test.js');
assert.equal(st.size, f.size());

var f1 = io.open('io_test.js.bak', 'w');
f1.write(f.read(f.size()));

f.rewind();
var b = f.read(st.size + 100);
assert.equal(st.size, b.length);
assert.equal(true, f.eof());

f.close();

var s = io.readFile("io_test.js");
assert.equal(s, b.toString());

st = os.stat('io_test.js');
assert.equal(st.size, f1.size());

assert.equal(st.size, f1.tell());
f1.rewind();
assert.equal(0, f1.tell());

f1.close();

os.unlink('io_test.js.bak');

f = io.open('io_test.js');
f.seek(f.size() + 10);
assert.equal(f.tell(), f.size() + 10);
f.seek(10);
b = f.read(f.size());
assert.equal(f.size() - 10, b.length);
f.close();

f = io.open('io_test.js');
f1 = io.open('io_test.js.bak', 'w');

var s = f.copyTo(f1, 100);
assert.equal(s, 100);
assert.equal(f1.size(), 100);
f.copyTo(f1);
assert.equal(f1.size(), f.size());

f.close();
f1.close();

os.unlink('io_test.js.bak');
