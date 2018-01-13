var test = require("test");
test.setup();

var win = process.platform == 'win32';

if (win) {
    var registry = require('registry');

    describe("registry", () => {
        before(() => {
            try {
                registry.del(registry.CURRENT_USER, "Software\\fibjs_test");
            } catch (e) {}
        });

        it("get", () => {
            assert.equal(registry.get(registry.LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir"),
                process.env.ProgramFiles);
        });

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

        it("list", () => {
            assert.deepEqual(registry.listSubKey(registry.CURRENT_USER, "Software\\fibjs_test"), ["test_key"]);
            assert.deepEqual(registry.listValue(registry.CURRENT_USER, "Software\\fibjs_test\\test_key").sort(), [
                "a",
                "b",
                "ms",
                "s"
            ]);
        });

        it("del", () => {
            registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms");
            assert.deepEqual(registry.listValue(registry.CURRENT_USER, "Software\\fibjs_test\\test_key").sort(), [
                "a",
                "b",
                "s"
            ]);

            registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key");
            assert.deepEqual(registry.listSubKey(registry.CURRENT_USER, "Software\\fibjs_test"), []);
        });
    });
}

require.main === module && test.run(console.DEBUG);