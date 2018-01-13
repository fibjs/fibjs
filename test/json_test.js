var test = require("test");
test.setup();

var encoding = require("encoding");
var fs = require("fs");
var path = require("path");

describe('json', () => {
    it("boolean & null", () => {
        assert.strictEqual(encoding.json.decode("true"), true);
        assert.strictEqual(encoding.json.decode("false"), false);
        assert.strictEqual(encoding.json.decode("null"), null);
    });

    it("number", () => {
        assert.strictEqual(encoding.json.decode("0"), 0);
        assert.strictEqual(encoding.json.decode("123"), 123);
        assert.strictEqual(encoding.json.decode("-123"), -123);

        assert.strictEqual(encoding.json.decode("0.123"), 0.123);
        assert.strictEqual(encoding.json.decode("-0.123"), -0.123);

        assert.strictEqual(encoding.json.decode("0.123e2"), 0.123e2);
        assert.strictEqual(encoding.json.decode("-0.123e3"), -0.123e3);
    });

    it("string", () => {
        assert.strictEqual(encoding.json.decode('""'), "");
        assert.strictEqual(encoding.json.decode('"abc"'), "abc");
    });

    it("array", () => {
        assert.deepEqual(encoding.json.decode('[]'), []);
        assert.deepEqual(encoding.json.decode('[1,2,3]'), [1, 2, 3]);
        assert.deepEqual(encoding.json.decode('[1,[2,3]]'), [1, [2, 3]]);
    });

    it("object", () => {
        assert.deepEqual(encoding.json.decode('{}'), {});
        assert.deepEqual(encoding.json.decode('{"a":100}'), {
            "a": 100
        });
    });

    it("test suite", () => {
        var files = fs.readdir(path.join(__dirname, "json_files"));

        files.forEach((f) => {
            if (path.extname(f) == ".json") {
                var txt = fs.readTextFile(path.join(__dirname, "json_files", f));
                assert.deepEqual(encoding.json.decode(txt), JSON.parse(txt));
            }
        });
    });

});

require.main === module && test.run(console.DEBUG);