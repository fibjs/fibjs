var test = require("test");
test.setup();

var encoding = require("encoding");
var fs = require("fs");
var path = require("path");

describe('json', function() {
	it("boolean & null", function() {
		assert.strictEqual(encoding.jsonDecode("true"), true);
		assert.strictEqual(encoding.jsonDecode("false"), false);
		assert.strictEqual(encoding.jsonDecode("null"), null);
	});

	it("number", function() {
		assert.strictEqual(encoding.jsonDecode("0"), 0);
		assert.strictEqual(encoding.jsonDecode("123"), 123);
		assert.strictEqual(encoding.jsonDecode("-123"), -123);

		assert.strictEqual(encoding.jsonDecode("0.123"), 0.123);
		assert.strictEqual(encoding.jsonDecode("-0.123"), -0.123);

		assert.strictEqual(encoding.jsonDecode("0.123e2"), 0.123e2);
		assert.strictEqual(encoding.jsonDecode("-0.123e3"), -0.123e3);
	});

	it("string", function() {
		assert.strictEqual(encoding.jsonDecode('""'), "");
		assert.strictEqual(encoding.jsonDecode('"abc"'), "abc");
	});

	it("array", function() {
		assert.deepEqual(encoding.jsonDecode('[]'), []);
		assert.deepEqual(encoding.jsonDecode('[1,2,3]'), [1, 2, 3]);
		assert.deepEqual(encoding.jsonDecode('[1,[2,3]]'), [1, [2, 3]]);
	});

	it("object", function() {
		assert.deepEqual(encoding.jsonDecode('{}'), {});
		assert.deepEqual(encoding.jsonDecode('{"a":100}'), {
			"a": 100
		});
	});

	it("test suite", function() {
		var files = fs.readdir("json_files");

		files.forEach(function(f) {
			if (path.extname(f.name) == ".json") {
				var txt = fs.readFile("json_files/" + f.name);
				assert.deepEqual(encoding.jsonDecode(txt), JSON.parse(txt));
			}
		});
	});

});

//test.run(console.DEBUG);