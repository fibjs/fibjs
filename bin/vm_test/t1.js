var assert = require('assert');

assert.equal(100, require('a'));
assert.equal(300, require('c'));

var b = require('b');
assert.equal(1000, b.a);

b.a = 2000;

function fun() {
	run('./t2');
}

fun();
fun.start().join();

exports.fun = fun;
