var test = require("test");
test.setup();

var utils = require('utils');

describe('utils', function() {
	describe('Stats', function() {
		var s;

		it("new", function() {
			s = new utils.Stats([ 'a' ], [ 'b', 'c' ]);
			assert.deepEqual(s.toJSON(), {
				"a" : 0,
				"b" : 0,
				"c" : 0
			});

			var s1 = new utils.Stats([ 'a', 'b', 'c' ]);
			assert.deepEqual(s1.toJSON(), {
				"a" : 0,
				"b" : 0,
				"c" : 0
			});

			assert.throws(function() {
				var s = new utils.Stats();
			});
		});

		it("inc", function() {
			s.inc('a');
			assert.deepEqual(s.toJSON(), {
				"a" : 1,
				"b" : 0,
				"c" : 0
			});

			s.inc('c');
			assert.deepEqual(s.toJSON(), {
				"a" : 1,
				"b" : 0,
				"c" : 1
			});
		});

		it("dec", function() {
			s.dec('a');
			assert.deepEqual(s.toJSON(), {
				"a" : 0,
				"b" : 0,
				"c" : 1
			});
		});

		it("add", function() {
			s.add('a', 100);
			assert.deepEqual(s.toJSON(), {
				"a" : 100,
				"b" : 0,
				"c" : 1
			});
		});

		it("reset", function() {
			s.reset();
			assert.deepEqual(s.toJSON(), {
				"a" : 100,
				"b" : 0,
				"c" : 0
			});
		});

		it("named", function() {
			assert.equal(s.a, 100);
			assert.equal(s["a"], 100);
		});
	});
});

// test.run();
