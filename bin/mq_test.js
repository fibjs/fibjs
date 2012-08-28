/**
 * 
 */

console.log('mq testing....');

var assert = require('assert');

var mq = require('mq');
var http = require('http');

var n = 0;

function fun3(v) {
	n = n | 4;
}

function fun2(v) {
	n = n | 2;
	return mq.jsHandler(fun3);
}

function fun1(v) {
	n = n | 1;
	return fun2;
}

var v = new Buffer('abcd');
var jv = mq.jsHandler(fun1);

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
	mq.invoke(function(v) {
		return 100;
	}, v);
});

// ------------- chain handler

function chain1(v) {
	n = n | 1;
}

function chain2(v) {
	n = n | 2;
}

function chain3(v) {
	n = n | 4;
}

var chain = new mq.Chain([ chain1, chain2, chain3 ]);

n = 0;
chain.invoke(v);
assert.equal(7, n);

var r = new mq.Routing({
	'a' : chain1,
	'c' : chain3,
	'b' : chain2
});

var m = new http.Request();

n = 0;
m.value = 'a';
mq.invoke(r, m);
assert.equal(1, n);

n = 0;
m.value = 'b';
mq.invoke(r, m);
assert.equal(2, n);

n = 0;
m.value = 'c';
mq.invoke(r, m);
assert.equal(4, n);

n = 0;
m.value = 'd';
assert.throws(function() {
	mq.invoke(r, m);
});
