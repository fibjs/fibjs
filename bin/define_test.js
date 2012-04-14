/**
 * 
 */

var assert = require('assert');

var a = require('define/a');
assert.equal(a, 100);

a = require('define/a1');
assert.equal(a.v, 100);

a = require('define/a2');
assert.equal(a.v, 200);

a = require('define/a3');
assert.equal(a.v, 200);

a = require('define/a4');
assert.equal(a.v, 200);
a = require('aaa');
assert.equal(a.v, 100);

a = require('define/a5');
assert.equal(a.v, 200);

a = require('define/a6');
assert.equal(a.aaa4.aaa5.v, 200);
