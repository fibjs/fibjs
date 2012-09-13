console.log('zlib testing....');

var assert = require('assert');
var zlib = require('zlib');
var io = require('io');

var M = 1024000;
var b = new Buffer(M);

for(i = 0; i < M; i ++)
	b[i] = Math.random() * 70 + 33;

assert.equal(zlib.inflate(zlib.deflate(b)).toString(), b.toString());
assert.equal(zlib.gunzip(zlib.gzip(b)).toString(), b.toString());
