var test = require("test");
test.setup();

var vm = require('vm');

describe("vm", function() {
	var sbox;

	it("sandbox.add", function() {
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

	it("sandbox.addScript", function() {
		var a = sbox.addScript("t1.js", "module.exports = {a : 100};");
		assert.equal(100, a.a);
		assert.equal(100, sbox.require("t1").a);

		var b = sbox.addScript("t2.js", "module.exports = {a : require('t1').a};");
		assert.equal(100, b.a);
		assert.equal(100, sbox.require("t2").a);
	});
});

//test.run();