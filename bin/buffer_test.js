/**
 * 
 */

var assert = require('assert');

var buf = new Buffer(100);
assert.equal(buf.length, 100);

var buf = new Buffer("abcd");
assert.equal(buf.length, 4);

