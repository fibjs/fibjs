var assert = require('assert');

assert.equal(100, require('a'));

function fun() {
	run('./t2');
}

fun();
fun.start().join();

exports.fun = fun;
