
var assert = require('assert');
var os = require('os');
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


function t_fiber2(v1, v2)
{
	n = v1 + v2 + coroutine.current().v;
}

var f = t_fiber2.start(100, 200);
assert.equal(n, 1300);
f.v = 2000;
f.join();
assert.equal(n, 2300);

var f = t_fiber2.start(100, 200);
assert.equal(n, 2300);
f.v = 1000;
coroutine.sleep(100);
f.join();
assert.equal(n, 1300);

function t_fiber3(v1, v2)
{
	n = v1 + v2 + this.caller.v;
}

var f = t_fiber3.start(100, 200);
assert.equal(n, 1300);
coroutine.current().v = 1234;
f.join();
assert.equal(n, 1534);

var nCount = 2000000;

var bDone = false;

function t_switch()
{
	while(!bDone)
		coroutine.sleep();
}

//t_switch.start();
console.time('switch '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	coroutine.sleep();
console.timeEnd('switch '+nCount+' times');

bDone = true;
coroutine.sleep();

var n = 0;

function t_nop()
{
}

console.time('start '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	t_nop.start();
console.timeEnd('start '+nCount+' times');

console.time('run '+nCount+' times');
coroutine.sleep();
console.timeEnd('run '+nCount+' times');

console.time('call '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	n = t_nop();
console.timeEnd('call '+nCount+' times');


console.time('os.CPUs '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	os.CPUs();
console.timeEnd('os.CPUs '+nCount+' times');

console.time('new Buffer '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	new Buffer();
console.timeEnd('new Buffer '+nCount+' times');

console.time('require '+nCount+' times');
for(var i = 0; i < nCount; i ++)
	require('os');
console.timeEnd('require '+nCount+' times');
