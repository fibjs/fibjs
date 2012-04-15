
var assert = require('assert');
var coroutine = require('coroutine');

var n = 123;

function t_fiber(v1, v2)
{
	n = v1 + v2;
}

var f = t_fiber.start(100, 200);
assert.equal(n, 123);
coroutine.sleep();
assert.equal(n, 300);


var f = coroutine.start(t_fiber, 123, 200);
assert.equal(n, 300);
f.join();
assert.equal(n, 323);


function t_fiber1(v1, v2)
{
	n = v1 + v2 + this.v;
}

var f = t_fiber1.start(100, 200);
assert.equal(n, 323);
f.v = 1000;
f.join();
assert.equal(n, 1300);


function t_fiber1(v1, v2)
{
	n = v1 + v2 + coroutine.current().v;
}

var f = t_fiber1.start(100, 200);
assert.equal(n, 1300);
f.v = 2000;
f.join();
assert.equal(n, 2300);

var nCount = 1000000;
var bDone = false;

function t_switch()
{
	while(!bDone)
		coroutine.sleep();
}

t_switch.start();
console.time('switch '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	coroutine.sleep();
console.timeEnd('switch '+nCount+' times');

bDone = true;
coroutine.sleep();

var n = 0;

function t_nop(a, b)
{
	return n + a + b;
}

console.time('start '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	t_nop.start(1, 2);
console.timeEnd('start '+nCount+' times');

console.time('run '+nCount+' times');
coroutine.sleep();
console.timeEnd('run '+nCount+' times');

console.time('call '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	n = t_nop(1, 2);
console.timeEnd('call '+nCount+' times');

