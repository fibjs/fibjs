var test = require("test");
test.setup();

var collection = require('collection');

var a = new collection.List();

function isBigEnough(element, index) {
	return (element >= 10);
}

describe('list', function() {
	it("pop empty list", function() {
		assert.equal(a.pop(), null);
	});

	it("toArray", function() {
		a.push(1);
		assert.deepEqual(a.toArray(), [ 1 ]);
	});

	it("push", function() {
		a.push(2, 3, 4);
		assert.deepEqual(a.toArray(), [ 1, 2, 3, 4 ]);
	});

	it("pop", function() {
		assert.equal(a.pop(), 4);
		assert.equal(a.length, 3);
	});

	it("slice", function() {
		assert.deepEqual(a.slice(1).toArray(), [ 2, 3 ]);
		assert.deepEqual(a.slice(1, 2).toArray(), [ 2 ]);
		assert.deepEqual(a.slice(1, 0).toArray(), []);
	});

	it("concat",
			function() {
				assert.deepEqual(a.concat(a).toArray(), [ 1, 2, 3, 1, 2, 3 ]);
				assert.deepEqual(a.concat(new collection.List()).toArray(), [
						1, 2, 3 ]);
				assert.deepEqual(a.concat([ 4, 5, 6 ]).toArray(), [ 1, 2, 3, 4,
						5, 6 ]);
				assert.deepEqual(a.concat(a, [ 4, 5, 6 ], a).toArray(), [ 1, 2,
						3, 1, 2, 3, 4, 5, 6, 1, 2, 3 ]);
			});

	it("every", function() {
		var passed = new collection.List();
		passed.push(12, 5, 8, 130, 44);
		assert.equal(passed.every(isBigEnough), false);

		passed = new collection.List();
		passed.push(12, 54, 18, 130, 44);
		assert.equal(passed.every(isBigEnough), true);
	});

	it("filter",
			function() {
				var passed = new collection.List();
				passed.push(12, 5, 8, 130, 44);
				assert.deepEqual(passed.filter(isBigEnough).toArray(), [ 12,
						130, 44 ]);
			});

	it("map", function() {
		var passed = new collection.List();
		passed.push(1, 4, 9);
		assert.deepEqual(passed.map(Math.sqrt).toArray(), [ 1, 2, 3 ]);
	});

	it("forEach", function() {
		var passed = new collection.List();
		passed.push(1, 4, 9);
		passed.forEach(function(v, i) {
			assert.equal(passed[i], v);
		});
	});
});

//test.run();
