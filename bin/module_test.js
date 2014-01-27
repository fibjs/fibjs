var test = require("test");
test.setup();

var fs = require('fs');
var coroutine = require('coroutine');
var a, b;

describe("module", function() {
	it("absolute identifiers", function() {
		a = require('module/a');
		b = require('b');
		assert.strictEqual(a.foo().foo, b.foo,
			'require works with absolute identifiers');
	});

	it("circular dependency", function() {
		a = require('module/a1');
		b = require('module/b1');

		assert.property(a, "a", 'a exists');
		assert.property(b, "b", 'b exists')
		assert.strictEqual(a.a().b, b.b, 'a gets b');
		assert.strictEqual(b.b().a, a.a, 'b gets a');
	});

	it("exports", function() {
		var a = require('module/a2');
		var foo = a.foo;
		assert.strictEqual(a.foo(), a, 'calling a module member');
		assert.deepEqual(foo(), (function() {
			return this
		})(), 'members not implicitly bound');
		a.set(10);
		assert.strictEqual(a.get(), 10, 'get and set')
	});

	it("require error", function() {
		assert.throws(function() {
			require('bogus');
		});
	});

	it("share require", function() {
		a = require('module/a3');
		b = require('b');
		assert.strictEqual(a.foo, b.foo,
			'a and b share foo through a relative require');
	});

	it("transitive", function() {
		assert.strictEqual(require('module/a4').foo(), 1, 'transitive');
	});

	it("require check modify", function() {
		var chkScript = "exports.foo = new Date();";

		fs.writeFile("module/check.js", chkScript);
		foo = require('module/check').foo;

		coroutine.sleep(1010);
		assert.equal(foo, require('module/check').foo);

		fs.writeFile("module/check.js", chkScript);
		coroutine.sleep(1010);
		assert.notEqual(foo, require('module/check').foo);

		fs.unlink('module/check.js');
	});
});

//test.run(console.DEBUG);