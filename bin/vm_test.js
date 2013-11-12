var test = require("test");
test.setup();

var vm = require('vm');

describe("vm", function() {
	it("sandbox", function() {
		var b = {
			a : 1000
		}

		var sbox = vm.create({
			a : 100,
			assert : assert,
			b : b
		}, function(name) {
			if (name == 'c')
				return 300;
		});

		sbox.add('a', new Number(100));
		sbox.add('a', 100);

		sbox.require('./vm_test/t1').fun();
		assert.equal(1000, b.a);
	});
});

//test.run();
