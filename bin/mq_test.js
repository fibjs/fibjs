/**
 * 
 */

console.log('mq testing....');

var assert = require('assert');

var mq = require('mq');
var http = require('http');

// ------------- function handler

var m = new http.Request();
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

// ------------- object handler

function hdlr1(v) {
	n = n | 1;
}

function hdlr2(v) {
	n = n | 2;
}

function hdlr3(v) {
	n = n | 4;
}

var o = mq.jsHandler({
	a : hdlr1,
	b : hdlr2,
	c : hdlr3,
	d : {
		d1 : hdlr1,
		d2 : {
			d3 : hdlr2,
			d4 : hdlr3,
			d5 : 1123
		}
	}
});

var ot = {
	'a' : 1,
	'/b' : 2,
	'.c' : 4,
	'a.a' : 1,
	'a/a' : 1,
	'd.d1' : 1,
	'd\\d1' : 1,
	'd.d2.d3' : 2,
	'd.d2.d4' : 4
};

for (t in ot) {
	n = 0;
	m.value = t;
	o.invoke(m);
	assert.equal(ot[t], n);
}

assert.throws(function() {
	m.value = 'asd';
	o.invoke(m);
});

assert.throws(function() {
	m.value = 'd.d2.d5';
	o.invoke(m);
});

// ------------- module handler

var mod = mq.moduleHandler("mq_test/t1", "foo");

m.value = 'a';
mod.invoke(m);
assert.equal(m.value, 'a100');

mod = mq.moduleHandler("mq_test/t1");

m.value = '/foo/b';
mod.invoke(m);
assert.equal(m.value, 'b100');

assert.throws(function() {
	m.value = '/foo1/b';
	mod.invoke(m);
});

mod = mq.moduleHandler("mq_test/");

m.value = 't1.foo.a';
mod.invoke(m);
assert.equal(m.value, 'a100');

m.value = '/t1/foo/123456789';
mod.invoke(m);
assert.equal(m.value, '123456789100');

assert.throws(function() {
	m.value = '/t1/foo1/b';
	mod.invoke(m);
});

assert.throws(function() {
	m.value = '/t2/foo/b';
	mod.invoke(m);
});

// ------------- chain handler

var chain = mq.chain([ hdlr1, hdlr2, mq.jsHandler(hdlr3) ]);

n = 0;
chain.invoke(v);
assert.equal(7, n);

// ------------- routing handler

function params(v) {
	assert.equal(v.params.length, 2);
	assert.equal(v.params[0], "123");
	assert.equal(v.params[1], "456");
}

var r = mq.routing({
	'^a$' : hdlr1,
	'^c$' : hdlr3,
	'^b$' : mq.jsHandler(hdlr2),
	'^params/(([0-9]*)\.([0-9]*)\.html)$' : params
});

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

m.value = 'params/123.456.html';
mq.invoke(r, m);

n = 0;
m.value = 'd';
assert.throws(function() {
	mq.invoke(r, m);
});
