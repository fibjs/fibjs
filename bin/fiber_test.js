
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


function t_switch()
{
	while(1)
		coroutine.sleep();
}

t_switch.start();
console.time('switch 100000 times');
for(var i = 0; i < 100000; i ++)
	coroutine.sleep();
console.timeEnd('switch 100000 times');

