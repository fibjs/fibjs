var test = require("test");
test.setup();

var encoding = require("encoding");
var fs = require("fs");
var path = require("path");

describe('json', function() {
	it("boolean & null", function() {
		assert.strictEqual(encoding.json.decode("true"), true);
		assert.strictEqual(encoding.json.decode("false"), false);
		assert.strictEqual(encoding.json.decode("null"), null);
	});

	it("number", function() {
		assert.strictEqual(encoding.json.decode("0"), 0);
		assert.strictEqual(encoding.json.decode("123"), 123);
		assert.strictEqual(encoding.json.decode("-123"), -123);

		assert.strictEqual(encoding.json.decode("0.123"), 0.123);
		assert.strictEqual(encoding.json.decode("-0.123"), -0.123);

		assert.strictEqual(encoding.json.decode("0.123e2"), 0.123e2);
		assert.strictEqual(encoding.json.decode("-0.123e3"), -0.123e3);
	});

	it("string", function() {
		assert.strictEqual(encoding.json.decode('""'), "");
		assert.strictEqual(encoding.json.decode('"abc"'), "abc");
	});

	it("array", function() {
		assert.deepEqual(encoding.json.decode('[]'), []);
		assert.deepEqual(encoding.json.decode('[1,2,3]'), [1, 2, 3]);
		assert.deepEqual(encoding.json.decode('[1,[2,3]]'), [1, [2, 3]]);
	});

	it("object", function() {
		assert.deepEqual(encoding.json.decode('{}'), {});
		assert.deepEqual(encoding.json.decode('{"a":100}'), {
			"a": 100
		});
	});

	it("test suite", function() {
		var files = fs.readdir("json_files");

		files.forEach(function(f) {
			if (path.extname(f.name) == ".json") {
				var txt = fs.readTextFile("json_files/" + f.name);
				assert.deepEqual(encoding.json.decode(txt), JSON.parse(txt));
			}
		});
	});

});

//test.run(console.DEBUG);