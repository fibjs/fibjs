var test = require("test");
test.setup();

var fs = require('fs');
var io = require('io');

var ms = new io.MemoryStream();
var cms;

describe('ms', function() {
	it("new", function() {
		ms = new io.MemoryStream();
		assert.equal(ms.stat().size, ms.size());
	});

	it("write", function() {
		ms.write('abcdefghijklmnopqrstuvwxyz');
		assert.equal(ms.stat().size, ms.size());
	});

	it("size", function() {
		assert.equal(26, ms.size());
	});

	it("tell", function() {
		assert.equal(26, ms.tell());
	});

	it("seek", function() {
		ms.seek(-10, fs.SEEK_END);
		assert.equal(16, ms.tell());
		ms.seek(ms.size() + 10, fs.SEEK_SET);
		assert.equal(26, ms.tell());
	});

	it("rewind", function() {
		ms.rewind();
		assert.equal(0, ms.tell());
		assert.equal('abcdefghijklmnopqrstuvwxyz', ms.read().toString());
	});

	it("seek & read", function() {
		ms.seek(10, fs.SEEK_SET);
		assert.equal('klmnopqrstuvwxyz', ms.read().toString());

		ms.seek(10, fs.SEEK_SET);
		assert.equal('klmnopqrstuvwxyz', ms.read(ms.size()).toString());

		ms.seek(10, fs.SEEK_SET);
		ms.seek(10, fs.SEEK_CUR);
		assert.equal('uvwxyz', ms.read(ms.size()).toString());

		ms.seek(-10, fs.SEEK_END);
		assert.equal('qrstuvwxyz', ms.read(ms.size()).toString());
	});

	it("clone", function() {
		cms = ms.clone();
		assert.deepEqual(cms.stat().mtime, ms.stat().mtime);

		assert.equal('abcdefghijklmnopqrstuvwxyz', cms.read().toString());

		cms.seek(10, fs.SEEK_SET);
		assert.equal(cms.tell(), 10);
		assert.equal('klmnopqrstuvwxyz', cms.read().toString());

		cms.seek(10, fs.SEEK_SET);
		assert.equal(cms.tell(), 10);
		assert.equal('klmnopqrstuvwxyz', cms.read(cms.size()).toString());

		cms.seek(10, fs.SEEK_SET);
		cms.seek(10, fs.SEEK_CUR);
		assert.equal(cms.tell(), 20);
		assert.equal('uvwxyz', cms.read(cms.size()).toString());

		cms.seek(-10, fs.SEEK_END);
		assert.equal(cms.tell(), 16);
		assert.equal('qrstuvwxyz', cms.read(cms.size()).toString());
	});

	it("seek & write", function() {
		ms.seek(10, fs.SEEK_SET);
		ms.write('abcdefghijklmnopqrstuvwxyz');
		assert.equal(36, ms.size());
		assert.equal(36, ms.tell());

		ms.rewind();
		assert.equal('abcdefghijabcdefghijklmnopqrstuvwxyz', ms.read().toString());
	});

});

//test.run();