var test = require("test");
test.setup();

var fs = require('fs');
var coroutine = require('coroutine');
var a, b;

describe("module", () => {
    it("native module toJSON", () => {
        JSON.stringify(require("os"));
    });

    it("absolute identifiers", () => {
        a = require('module/a');
        b = require('b.js');
        assert.strictEqual(a.foo().foo, b.foo,
            'require works with absolute identifiers');
    });

    it("circular dependency", () => {
        a = require('module/a1');
        b = require('module/b1');

        assert.property(a, "a", 'a exists');
        assert.property(b, "b", 'b exists')
        assert.strictEqual(a.a().b, b.b, 'a gets b');
        assert.strictEqual(b.b().a, a.a, 'b gets a');
    });

    it("exports", () => {
        var a = require('module/a2');
        var foo = a.foo;
        assert.strictEqual(a.foo(), a, 'calling a module member');
        a.set(10);
        assert.strictEqual(a.get(), 10, 'get and set')
    });

    it("require error", () => {
        assert.throws(() => {
            require('bogus');
        });
    });

    it("share require", () => {
        a = require('module/a3');
        b = require('b');
        assert.strictEqual(a.foo, b.foo,
            'a and b share foo through a relative require');
    });

    it("transitive", () => {
        assert.strictEqual(require('module/a4').foo(), 1, 'transitive');
    });

    it("require json", () => {
        assert.deepEqual(require('module/data'), {
            "a": 100,
            "b": 200
        });
    });

    it("require .js module folder", () => {
        assert.deepEqual(require('module/p4.js'), {
            "v": 100
        });
    });

    describe("package.json", () => {
        it("main", () => {
            var a = require('module/p1');
            assert.deepEqual(a, {
                "a": 100
            });

            assert.equal(a, require('module/p1'));
            assert.equal(a, require('module/p1/main'));
        });

        it("default entry", () => {
            var a = require('module/p2');
            assert.deepEqual(a, {
                "a": 200
            });

            assert.equal(a, require('module/p2'));
            assert.equal(a, require('module/p2/index'));
        });

        it("no json", () => {
            var a = require('module/p3');
            assert.deepEqual(a, {
                "a": 300
            });

            assert.equal(a, require('module/p3'));
            assert.equal(a, require('module/p3/index'));
        });
    });

    describe("node_modules", () => {
        it("root folder", () => {
            var a = require('node_mod1');
            assert.deepEqual(a, {
                "a": 100
            });

            assert.equal(a, require('./node_modules/node_mod1'));
            assert.equal(a, require('node_modules/node_mod1'));
        });

        it("current folder", () => {
            var a = require('./module/mod_test').require("node_mod2");
            assert.deepEqual(a, {
                "a": 200
            });

            assert.equal(a, require('./module/node_modules/node_mod2'));
            assert.equal(a, require('module/node_modules/node_mod2'));
        });

        it("parent folder", () => {
            var a = require('./module/mod_test').require("node_mod4");
            assert.deepEqual(a, {
                "a": 400
            });

            assert.equal(a, require('./node_modules/node_mod4'));
            assert.equal(a, require('node_modules/node_mod4'));
        });

        it("priority", () => {
            var a = require('./module/mod_test').require("node_mod3");
            assert.deepEqual(a, {
                "a": 300
            });

            assert.equal(a, require('./module/node_modules/node_mod3'));
            assert.equal(a, require('module/node_modules/node_mod3'));
        });
    });

    it("zip virtual path", () => {
        assert.deepEqual(require('./module/test.src/folder/b.js'),
            require('./module/test.zip?/folder/b.js'));

        assert.equal(require('./module/p4.zip?').a, 100);
        assert.equal(require('./module/p4').a, 100);
    });

    it("strack", () => {
        assert.ok(require("module/stack").func().match(/module_test/));
    });
});

// test.run(console.DEBUG);