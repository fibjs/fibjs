var test = require("test");
test.setup();

var fs = require('fs');
var coroutine = require('coroutine');
var a, b;

describe("module", function() {
	it("native module toJSON", function() {
		JSON.stringify(require("os"));
	});

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

	it("require json", function() {
		assert.deepEqual(require('module/data'), {
			"a": 100,
			"b": 200
		});
	});

	describe("package.json", function() {
		it("main", function() {
			var a = require('module/p1');
			assert.deepEqual(a, {
				"a": 100
			});

			assert.equal(a, require('module/p1'));
			assert.equal(a, require('module/p1/main'));
		});

		it("default entry", function() {
			var a = require('module/p2');
			assert.deepEqual(a, {
				"a": 200
			});

			assert.equal(a, require('module/p2'));
			assert.equal(a, require('module/p2/index'));
		});
	});

	describe(".modules", function() {
		it("root folder", function() {
			var a = require('mod1');
			assert.deepEqual(a, {
				"a": 100
			});

			assert.equal(a, require('./.modules/mod1'));
			assert.equal(a, require('.modules/mod1'));
		});

		it("current folder", function() {
			var a = require('./module/mod_test').require("mod2");
			assert.deepEqual(a, {
				"a": 200
			});

			assert.equal(a, require('./module/.modules/mod2'));
			assert.equal(a, require('module/.modules/mod2'));
		});

		it("parent folder", function() {
			var a = require('./module/mod_test').require("mod4");
			assert.deepEqual(a, {
				"a": 400
			});

			assert.equal(a, require('./.modules/mod4'));
			assert.equal(a, require('.modules/mod4'));
		});

		it("priority", function() {
			var a = require('./module/mod_test').require("mod3");
			assert.deepEqual(a, {
				"a": 300
			});

			assert.equal(a, require('./module/.modules/mod3'));
			assert.equal(a, require('module/.modules/mod3'));
		});

		it("no .modules folder", function() {
			var a = require('./module/no.modules/mod_test').require("mod5");
			assert.deepEqual(a, {
				"a": 500
			});
		});
	});

	it("strack", function() {
		assert.ok(require("module/stack").func().match(/module_test/));
	});
});

//test.run(console.DEBUG);