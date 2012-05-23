console.log('lock testing....');

var assert = require('assert');
var coroutine = require('coroutine');

var l = new coroutine.Lock();

assert.throws(function(){
	l.release();
});

var v = 100;
function f()
{
	l.acquire();
	v = 200;
	coroutine.sleep();
	v = 300;
	l.release();
}

f.start();
coroutine.sleep();
l.acquire();
assert.equal(300, v);
l.release();

var sem = new coroutine.Semaphore();

assert.equal(true, sem.trywait());
assert.equal(false, sem.trywait());

v = 100;
function f1()
{
	while(1)
	{
		sem.wait();
		v ++;
	}
}

f1.start();
coroutine.sleep();
assert.equal(100, v);

sem.post();
coroutine.sleep();
assert.equal(101, v);

for(var i = 0; i < 10; i ++)
	sem.post();
coroutine.sleep();
assert.equal(111, v);

v = 0;
var cond = new coroutine.Condition();

assert.throws(function(){
	cond.wait();
});

function f3()
{
	cond.acquire();
	while(v <= 0)
		cond.wait();
	assert.equal(100, v);
	v += 100;
	cond.release();
}

f3.start();

cond.acquire();
coroutine.sleep();
cond.notify();
coroutine.sleep();
assert.equal(0, v);

cond.acquire();
v = 100;
cond.notify();
coroutine.sleep();
assert.equal(200, v);
