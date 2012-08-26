/**
 * 
 */

console.log('mq testing....');

var assert = require('assert');

var mq = require('mq');

var n = 0;

function fun3(v) {
	n = n | 4;
}

function fun2(v) {
	n = n | 2;
	return new mq.JSHandler(fun3);
}

function fun1(v) {
	n = n | 1;
	return fun2;
}

var v = new Buffer('abcd');
var jv = new mq.JSHandler(fun1);

n = 0;
jv.invoke(v);
assert.equal(3, n);

n = 0;
mq.invoke(jv, v);
assert.equal(7, n);

assert.throws(function() {
	new mq.JSHandler(function(v) {
		return 100;
	}).invoke(v);
});

assert.throws(function() {
	mq.invoke(new mq.JSHandler(function(v) {
		return 100;
	}), v);
});

