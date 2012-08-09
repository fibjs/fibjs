/**
 * 
 */

console.log('http testing....');

var assert = require('assert');
var io = require('io');
var http = require('http');

var d = new http.Request().headers;

d.add('a', '100');
d.add('b', '200');
d.add('a', '300');
d.add('c', '400');
d.add('d', '500');
d.add('c', '600');
d.add('d', '700');

assert.equal(d['a'], '100');

assert.equal(d.has('a'), true);
assert.equal(d.first('a'), '100');

a = d.all('a');
assert.equal(a.length, 2);
assert.equal(a[0], '100');
assert.equal(a[1], '300');

d.remove('a');
assert.equal(d.has('a'), false);
assert.strictEqual(d.first('a'), undefined);

assert.strictEqual(d['a'], undefined);

assert.equal(d.first('c'), '400');

a = d.all('c');
assert.equal(a.length, 2);
assert.equal(a[0], '400');
assert.equal(a[1], '600');

d.set('c', '800');

a = d.all('c');
assert.equal(a.length, 1);
assert.equal(a[0], '800');

assert.equal(d.first('c'), '800');

d.add({
	d : "900",
	b : "1000"
});

a = d.all('d');
assert.equal(a.length, 3);
assert.equal(a[0], '500');
assert.equal(a[1], '700');
assert.equal(a[2], '900');

a = d.all('b');
assert.equal(a.length, 2);
assert.equal(a[0], '200');
assert.equal(a[1], '1000');

d.set({
	d : "900",
	b : "1000"
});

a = d.all('d');
assert.equal(a.length, 1);
assert.equal(a[0], '900');

a = d.all('b');
assert.equal(a.length, 1);
assert.equal(a[0], '1000');

