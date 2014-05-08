var test = require("test");
test.setup();

describe("test", function() {
	it("it throw when not in describe", function() {
		assert.throws(function() {
			it("x", function() {});
		});
	});

	it("before throw when not in describe", function() {
		assert.throws(function() {
			before(function() {});
		});
	});

	it("after throw when not in describe", function() {
		assert.throws(function() {
			after(function() {});
		});
	});

	it("beforeEach throw when not in describe", function() {
		assert.throws(function() {
			beforeEach(function() {});
		});
	});

	it("afterEach throw when not in describe", function() {
		assert.throws(function() {
			afterEach(function() {});
		});
	});
});

//test.run(console.DEBUG);