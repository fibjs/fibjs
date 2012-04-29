/**
 * 
 */

var assert = require('assert');
var coroutine = require('coroutine');

var e = new coroutine.EventObject();

var v1, v2;

function t1(a1, a2) {
	v1 = v1 + a1 - a2 + 1234;
}

function t2(a1, a2) {
	v2 = v2 + a1 - a2 + 4321;
}

v1 = v2 = 0;
e.on('test', t1);
e.trigger('test', 200, 100);
assert.equal(0, v1);
assert.equal(0, v2);
coroutine.sleep();
assert.equal(1334, v1);
assert.equal(0, v2);

e.on('test', t2);
e.trigger('test', 2000, 1000);
coroutine.sleep();
assert.equal(3568, v1);
assert.equal(5321, v2);

e.off('test', t1);
e.trigger('test', 20, 10);
coroutine.sleep();
assert.equal(3568, v1);
assert.equal(9652, v2);

e.once('test', t1);
e.trigger('test', 20, 10);
coroutine.sleep();
assert.equal(4812, v1);
assert.equal(13983, v2);



