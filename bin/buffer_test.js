var test = require("test");
test.setup();

describe('Buffer', function() {
	it('new', function() {
		var buf = new Buffer("abcd");
		assert.equal(buf.length, 4);
	});

	it('resize', function() {
		var buf = new Buffer();
		buf.resize(100);
		assert.equal(buf.length, 100);

		assert.doesNotThrow(function() {
			buf.resize("12")
		});
		assert.throws(function() {
			buf.resize("a12")
		});

		assert.throws(function() {
			buf.resize("12a")
		});

		assert.throws(function() {
			buf.resize("12.a")
		});
	});
});

//test.run();
