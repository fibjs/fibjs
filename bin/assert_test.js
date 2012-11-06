console.log('assert testing....');

var assert = require("assert");

assert.notEqual({
	a : 0
}, {
	b : 100
});

assert.equal({
	a : 100
}, {
	a : 100
});

assert.notEqual([ 0 ], [ 100 ]);

assert.equal([ 100 ], [ 100 ]);

assert.notEqual([ {
	a : 0
}, 100 ], [ {
	b : 100
}, 100 ]);

assert.equal([ {
	a : 100
}, 200 ], [ {
	a : 100
}, 200 ]);
