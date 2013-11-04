console.log('assert testing....');

var assert;
if(typeof global == "undefined")
	assert = require("assert");
else
	assert = require('chai').assert;

assert.ok(true);
assert.notOk(false);

assert.isTrue(true);
assert.isFalse(false);

assert.isNull(null);
assert.isNotNull(123);

assert.isUndefined(undefined);
assert.isDefined(123);

assert.isFunction(function(){});
assert.isNotFunction(123);

assert.isObject({});
assert.isNotObject(123);

assert.isArray([]);
assert.isNotArray(123);

assert.isString("asd");
assert.isNotString(123);

assert.isNumber(123);
assert.isNotNumber("aaa");

assert.isBoolean(true);
assert.isNotBoolean("aaa");

assert.closeTo(123.4, 123.5, "0.1");

assert.lessThan(300, "a400");

assert.notDeepEqual({
	a : 0
}, {
	b : 100
});

assert.deepEqual({
	a : 100
}, {
	a : 100
});

assert.notDeepEqual([ 0 ], [ 100 ]);

assert.deepEqual([ 100 ], [ 100 ]);

assert.notDeepEqual([ {
	a : 0
}, 100 ], [ {
	b : 100
}, 100 ]);

assert.deepEqual([ {
	a : 100
}, 200 ], [ {
	a : 100
}, 200 ]);
