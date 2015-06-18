var assert = require('assert');
var coroutine = require('coroutine');

assert.equal(100, require('a'));
assert.equal(300, require('c'));

var b = require('b');
assert.equal(1000, b.a);

b.a = 2000;

function fun() {
	run('./t2.js');
}

fun();
coroutine.start(fun).join();

exports.fun = fun;