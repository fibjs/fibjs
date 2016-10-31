var assert = require("assert");

exports.func = function() {
	assert.equal(__sbname, "test");
}