/**
 * 
 */

console.log('buffer testing....');

var assert = require('assert');

var buf = new Buffer(100);
assert.equal(buf.length, 100);

var buf = new Buffer("abcd");
assert.equal(buf.length, 4);

var buf = new Buffer();
assert.doesNotThrow(function() {
	buf.resize("12")
});

assert.throws(function() {
	buf.resize("a12")
});

assert.throws(function() {
	buf.resize("12a")
});

assert.throws(function() {
	buf.resize("12.a")
});
