var test = require("test");
test.setup();

describe("define", function() {
	it("define", function() {
		var a = require('define/a');
		assert.equal(a, 100);
	});

	it("exports", function() {
		a = require('define/a1');
		assert.equal(a.v, 100);
	});

	it("exports overwrite", function() {
		a = require('define/a2');
		assert.equal(a.v, 200);
	});

	it("multi define", function() {
		a = require('define/a3');
		assert.equal(a.v, 200);
		a = require('define/a5');
		assert.equal(a.v, 200);
	});

	it("named define", function() {
		a = require('define/a4');
		assert.equal(a.v, 200);
		a = require('aaa');
		assert.equal(a.v, 100);
	});

	it("circular dependency", function() {
		a = require('define/a6');
		assert.equal(a.aaa4.aaa5.v, 200);
	});
});

//test.run();
