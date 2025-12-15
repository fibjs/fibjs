var { describe, it, xit, before } = require('node:test');
var assert = require('assert');


var win = process.platform == 'win32';

if (win) {
    var registry = require('registry');

    describe("registry", () => {
        before(() => {
            try {
                registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key");
            } catch (e) { }
        });

        describe("get operations", () => {
            xit("get", () => {
                assert.equal(registry.get(registry.LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir"),
                    process.env.ProgramFiles);
            });

            it("get with name", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name", "test_value");
                assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name"), "test_value");
            });
        });

        describe("set operations", () => {
            it("set: number", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\a", 100);
                assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\a"), 100);
            });

            it("set: string", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\s", "this is a string");
                assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\s"), "this is a string");
            });

            it("set: multi string", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms", [
                    "string1",
                    "string2",
                    "string3"
                ]);
                assert.deepEqual(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms"), [
                    "string1",
                    "string2",
                    "string3"
                ]);
            });

            it("set: binary", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\b",
                    new Buffer("this is binary data"));
                assert.deepEqual(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\b"),
                    new Buffer("this is binary data"));
            });

            it("set: qword", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\q", 1234567890123456789, registry.QWORD);
                assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\q"), 1234567890123456789);
            });

            it("set: expand string", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\es", "%SystemRoot%", registry.EXPAND_SZ);
                assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\es"), "%SystemRoot%");
            });

            it("set with name: number", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_num", 200);
                assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_num"), 200);
            });

            it("set with name: string", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_str", "another string");
                assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_str"), "another string");
            });

            it("set with name: multi string", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_ms", [
                    "string1",
                    "string2",
                    "string3"
                ]);
                assert.deepEqual(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_ms"), [
                    "string1",
                    "string2",
                    "string3"
                ]);
            });

            it("set with name: binary", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_bin",
                    new Buffer("binary data"));
                assert.deepEqual(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_bin"),
                    new Buffer("binary data"));
            });
        });

        describe("list operations", () => {
            it("list", () => {
                assert.deepEqual(registry.listSubKey(registry.CURRENT_USER, "Software\\fibjs_test"), ["test_key"]);
                assert.deepEqual(registry.listValue(registry.CURRENT_USER, "Software\\fibjs_test\\test_key").sort(), [
                    "a",
                    "b",
                    "es",
                    "ms",
                    "q",
                    "s",
                    "test_name",
                    "test_name_bin",
                    "test_name_ms",
                    "test_name_num",
                    "test_name_str"
                ]);
            });
        });

        describe("has operations", () => {
            it("has", () => {
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\a"), true);
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\b"), true);
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms"), true);
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\s"), true);
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\q"), true);
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\es"), true);
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\not_exist"), false);
            });
        });

        describe("delete operations", () => {
            it("del", () => {
                registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms");
                assert.deepEqual(registry.listValue(registry.CURRENT_USER, "Software\\fibjs_test\\test_key").sort(), [
                    "a",
                    "b",
                    "es",
                    "q",
                    "s",
                    "test_name",
                    "test_name_bin",
                    "test_name_ms",
                    "test_name_num",
                    "test_name_str"
                ]);

                registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key");
                assert.deepEqual(registry.listSubKey(registry.CURRENT_USER, "Software\\fibjs_test"), []);
            });

            it("del with name", () => {
                registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_del", "to be deleted");
                registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_del");
                assert.equal(registry.has(registry.CURRENT_USER, "Software\\fibjs_test\\test_key", "test_name_del"), false);
            });
        });
    });
}

