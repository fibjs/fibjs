
console.log('MemoryStream testing....');

var assert = require('assert');

var io = require('io');

var ms = new io.MemoryStream();
assert.equal(ms.stat().size, ms.size());
assert.equal(ms.stat().mtime.toString(), new Date().toString());

ms.write(new Buffer('abcdefghijklmnopqrstuvwxyz'));
assert.equal(ms.stat().size, ms.size());
assert.equal(26, ms.size());
assert.equal(26, ms.tell());

ms.seek(-10, io.SEEK_END);
assert.equal(16, ms.tell());

ms.seek(ms.size() + 10);

ms.rewind();
assert.equal(0, ms.tell());
assert.equal('abcdefghijklmnopqrstuvwxyz', ms.read().toString());

ms.seek(10);
assert.equal('klmnopqrstuvwxyz', ms.read().toString());

ms.seek(10);
assert.equal('klmnopqrstuvwxyz', ms.read(ms.size()).toString());

ms.seek(10);
ms.seek(10, io.SEEK_CUR);
assert.equal('uvwxyz', ms.read(ms.size()).toString());

ms.seek(-10, io.SEEK_END);
assert.equal('qrstuvwxyz', ms.read(ms.size()).toString());

ms.seek(10);
ms.write(new Buffer('abcdefghijklmnopqrstuvwxyz'));
assert.equal(36, ms.size());
assert.equal(36, ms.tell());

ms.rewind();
assert.equal('abcdefghijabcdefghijklmnopqrstuvwxyz', ms.read().toString());
