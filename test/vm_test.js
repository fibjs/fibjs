var test = require("test");
test.setup();

var vm = require('vm');
var os = require('os');

describe("vm", function() {
	var sbox;

	it("add", function() {
		var b = {
			a: 1000
		}

		sbox = new vm.SandBox({
			a: 100,
			assert: assert,
			b: b
		}, function(name) {
			if (name == 'c')
				return 300;
		});

		sbox.add('a', new Number(100));
		sbox.add('a', 100);

		sbox.require('./vm_test/t1').fun();
		assert.equal(1000, b.a);
	});

	it("addScript", function() {
		var a = sbox.addScript("t1.js", "module.exports = {a : 100};");
		assert.equal(100, a.a);
		assert.equal(100, sbox.require("t1").a);

		var b = sbox.addScript("t2.js", "module.exports = {a : require('t1').a};");
		assert.equal(100, b.a);
		assert.equal(100, sbox.require("t2").a);
	});

	it("callback", function() {
		var b = 200;
		var o = {
			a: 100,
			b: 200
		};

		sbox = new vm.SandBox({
			b: b
		}, function(n) {
			if (n == 'c')
				return 100;
			if (n == 'o')
				return o;
		});

		assert.equal(200, sbox.require("b"));
		assert.equal(100, sbox.require("c"));

		var o1 = sbox.require("o");
		assert.notEqual(o, o1);
		assert.deepEqual(o, o1);
	});

	it("add native object", function() {
		var b = new Buffer();

		sbox = new vm.SandBox({
			b: b
		});

		var b1 = sbox.require("b");
		assert.equal(b, b1);
	});

	it("add javascript object", function() {
		var b = {
			a: 100,
			b: 200
		};

		sbox = new vm.SandBox({
			b: b
		});

		var b1 = sbox.require("b");
		assert.notEqual(b, b1);
		assert.deepEqual(b, b1);
	});

	xit("disable global.repl", function() {
		assert.throws(function() {
			repl.toString();
		});
	});

	it("name", function() {
		sbox = new vm.SandBox({
			assert: assert
		}, "test");

		sbox.run("./vm_test/name_test1.js");
		var t1 = sbox.require("./vm_test/name_test2");
		t1.func();
	});

	it("Garbage Collection", function() {
		sbox = undefined;
		GC();
		var no1 = os.memoryUsage().nativeObjects;

		sbox = new vm.SandBox({});
		assert.equal(no1 + 1, os.memoryUsage().nativeObjects);

		var a = sbox.addScript("t1.js", "module.exports = {a : new Buffer()};");

		GC();
		assert.equal(no1 + 2, os.memoryUsage().nativeObjects);

		sbox = undefined;

		GC();
		assert.equal(no1 + 1, os.memoryUsage().nativeObjects);

		a = undefined;

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects);
	});

	it("Garbage Collection ERROR!!!", function() {
		GC();
		var no1 = os.memoryUsage().nativeObjects;

		var a = {
			b: new vm.SandBox({}).addScript('b.js', "exports.a = new Buffer()")
		};
		a = undefined;

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects);
	});
});

//test.run(console.DEBUG);