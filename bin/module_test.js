console.log('module testing....');

var assert = require('assert');
var io = require('io');
var os = require('os');
var coroutine = require('coroutine');

var a = require('module/a');
var b = require('b');
assert.strictEqual(a.foo().foo, b.foo,
		'require works with absolute identifiers');

a = require('module/a1');
b = require('module/b1');

assert.ok(a.a, 'a exists');
assert.ok(b.b, 'b exists')
assert.strictEqual(a.a().b, b.b, 'a gets b');
assert.strictEqual(b.b().a, a.a, 'b gets a');

var a = require('module/a2');
var foo = a.foo;
assert.strictEqual(a.foo(), a, 'calling a module member');
assert.strictEqual(foo(), (function() {
	return this
})(), 'members not implicitly bound');
a.set(10);
assert.strictEqual(a.get(), 10, 'get and set')

assert.throws(function() {
	require('bogus');
});

a = require('module/a3');
b = require('b');
assert.strictEqual(a.foo, b.foo, 'a and b share foo through a relative require');

assert.strictEqual(require('module/a4').foo(), 1, 'transitive');

var chkScript = "exports.foo = new Date();";

io.writeFile("module/check.js", chkScript);
foo = require('module/check').foo;

coroutine.sleep(1010);
assert.equal(foo, require('module/check').foo);

io.writeFile("module/check.js", chkScript);
coroutine.sleep(1010);
assert.notEqual(foo, require('module/check').foo);

os.unlink('module/check.js');
