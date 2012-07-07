console.log('uuid testing....');

var assert = require('assert');
var uuid = require('uuid');
var encoding = require('encoding');

var id = uuid.node();
var id1 = uuid.uuid(id);
var id2 = uuid.uuid(id.data());

assert.equal(id.toString(), id1.toString());
assert.equal(id.toString(), id2.toString());

console.log(id);
