console.log('list testing....');

var collection = require('collection');
var assert = require('assert');

var a = new collection.List();

assert.equal(a.pop(), null);

a.push(1);
assert.equal(a.toArray(), [ 1 ]);

a.push(2, 3, 4);
assert.equal(a.toArray(), [ 1, 2, 3, 4 ]);

assert.equal(a.pop(), 4);
assert.equal(a.length, 3);

assert.equal(a.slice(1).toArray(), [ 2, 3 ]);
assert.equal(a.slice(1, 2).toArray(), [ 2 ]);
assert.equal(a.slice(1, 0).toArray(), []);

assert.equal(a.concat(a).toArray(), [ 1, 2, 3, 1, 2, 3 ]);
assert.equal(a.concat(new collection.List()).toArray(), [ 1, 2, 3 ]);
assert.equal(a.concat([ 4, 5, 6 ]).toArray(), [ 1, 2, 3, 4, 5, 6 ]);
assert.equal(a.concat(a, [ 4, 5, 6 ], a).toArray(), [ 1, 2, 3, 1, 2, 3, 4, 5,
		6, 1, 2, 3 ]);

function isBigEnough(element, index) {
	return (element >= 10);
}

var passed = new collection.List();
passed.push(12, 5, 8, 130, 44);

assert.equal(passed.every(isBigEnough), false);

assert.equal(passed.filter(isBigEnough).toArray(), [ 12, 130, 44 ]);

var passed = new collection.List();
passed.push(12, 54, 18, 130, 44);

assert.equal(passed.every(isBigEnough), true);

var passed = new collection.List();
passed.push(1, 4, 9);
assert.equal(passed.map(Math.sqrt).toArray(), [ 1, 2, 3 ]);

passed.forEach(function(v, i) {
	assert.equal(passed[i], v);
});
