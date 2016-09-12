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
		sbox.add('coroutine', require('coroutine'));

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

	it("module", function() {
		sbox = new vm.SandBox({
			assert: assert
		});

		sbox.addScript("t1.js", "assert.equal(module.exports, exports);");
		sbox.addScript("t2.js", "assert.equal(module.require, require);");
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

	it("clone", function() {
		sbox = new vm.SandBox({
			a: 100,
			b: 200
		});
		assert.equal(sbox.require("a"), 100);

		var sb1 = sbox.clone();
		assert.equal(sb1.require("a"), 100);

		sb1.add("c", 300);
		assert.equal(sb1.require("c"), 300);

		assert.throws(function() {
			sbox.require("c");
		});
	});

	it("block global hacker", function() {
		sbox = new vm.SandBox({});
		assert.throws(function() {
			sbox.addScript("t1.js", "});(function(){");
		});
	});

	it("block global repl&argv", function() {
		sbox = new vm.SandBox({});
		assert.isUndefined(repl);

		assert.throws(function() {
			sbox.addScript("t2.js", "argv[0];");
		});
	});

	it("block function return", function() {
		sbox = new vm.SandBox({});
		assert.throws(function() {
			sbox.addScript("t1.js", "return 100;");
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
		var no1 = os.memoryUsage().nativeObjects.objects;

		sbox = new vm.SandBox({});
		assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);

		var a = sbox.addScript("t1.js", "module.exports = {a : new Buffer()};");

		GC();
		assert.equal(no1 + 2, os.memoryUsage().nativeObjects.objects);

		sbox = undefined;

		GC();
		assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);

		a = undefined;

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);
	});

	it("Garbage Collection 1", function() {
		GC();
		var no1 = os.memoryUsage().nativeObjects.objects;

		var a = {
			b: new vm.SandBox({}).addScript('b.js', "exports.a = new Buffer()")
		};
		a = undefined;

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);
	});

	it("Garbage Collection 2", function() {
		GC();
		var no1 = os.memoryUsage().nativeObjects.objects;

		new vm.SandBox({
			rpc: require('rpc')
		}).addScript('server.js', 'exports.a = require("rpc").json(require)');

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);
	});
});

// test.run(console.DEBUG);