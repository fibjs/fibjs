var test = require("test");
test.setup();

var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');
var a, b;

const bin_path = path.dirname(process.execPath);

describe("module", () => {
    after(() => {
        try {
            fs.unlink(path.join(__dirname, 'module', 'p6_1'));
        } catch (e) { }
    });

    it("native module toJSON", () => {
        JSON.stringify(require("os"));
    });

    it("resolve", () => {
        assert.equal(require.resolve('./module/a'),
            path.join(__dirname, 'module', 'a.js'));
        assert.equal(require.resolve('./module/test.zip$/folder/b'),
            path.join(__dirname, 'module', 'test.zip$', 'folder', 'b.js'));
        assert.equal(require.resolve('node_mod1'),
            path.join(__dirname, 'node_modules', 'node_mod1.js'));

        assert.equal(require.resolve('./module/d1'),
            path.join(__dirname, 'module', 'd1'));
    });

    it("circular dependency", () => {
        a = require('./module/a1');
        b = require('./module/b1');

        assert.property(a, "a", 'a exists');
        assert.property(b, "b", 'b exists')
        assert.strictEqual(a.a().b, b.b, 'a gets b');
        assert.strictEqual(b.b().a, a.a, 'b gets a');
    });

    it("exports", () => {
        var a = require('./module/a2');
        var foo = a.foo;
        assert.strictEqual(a.foo(), a, 'calling a module member');
        a.set(10);
        assert.strictEqual(a.get(), 10, 'get and set')
    });

    it("require error", () => {
        assert.throws(() => {
            require('./bogus');
        });
    });

    it("require compile error", () => {
        assert.throws(() => {
            require('./module/require_bug');
        });
    });

    it("share require", () => {
        a = require('./module/a3');
        b = require('./b');
        assert.strictEqual(a.foo, b.foo,
            'a and b share foo through a relative require');
    });

    it("transitive", () => {
        assert.strictEqual(require('./module/a4').foo(), 1, 'transitive');
    });

    it("require json", () => {
        assert.deepEqual(require('./module/data'), {
            "a": 100,
            "b": 200
        });
    });

    it("require error json", () => {
        assert.throws(() => {
            require('./module/data_err');
        });
    });

    it("require .js module folder", () => {
        assert.deepEqual(require('./module/p4.js'), {
            "v": 100
        });
    });

    it("require ./..", () => {
        assert.equal(require('./module/p7/t1').p7, require('./module/p7'));
        assert.equal(require('./module/p7/t.js').p7, require('./module/p7'));
    });

    it("require no-ext file", () => {
        assert.deepEqual(require('./module/d1'), {
            "d": 100
        });
    });

    xit("require async file", () => {
        assert.equal(require('./module/d2'), 100);
    });

    it("support symlink", () => {
        fs.symlink(path.join(__dirname, 'module', 'p6'), path.join(__dirname, 'module', 'p6_1'));
        assert.equal(require('./module/p6/t'), require('./module/p6_1/t'));
    });

    describe("require cjs", () => {
        it("require .cjs", () => {
            assert.equal(require('./module/p10.cjs').test, "p10.cjs");
        });

        it("require .js", () => {
            assert.equal(require('./module/p10.js').test, "p10.js");
        });

        it("js first", () => {
            assert.equal(require('./module/p10').test, "p10.js");
        });
    });

    describe("package.json", () => {
        it("main", () => {
            var a = require('./module/p1');
            assert.deepEqual(a, {
                "a": 100
            });

            assert.equal(a, require('./module/p1'));
            assert.equal(a, require('./module/p1/main'));
        });

        describe("exports", () => {
            it("simple", () => {
                var a = require('./module/p1.1');
                assert.deepEqual(a, {
                    "a": 101
                });

                assert.equal(a, require('./module/p1.1'));
                assert.equal(a, require('./module/p1.1/main'));
            });

            it(".", () => {
                var a = require('./module/p1.2');
                assert.deepEqual(a, {
                    "a": 102
                });

                assert.equal(a, require('./module/p1.2'));
                assert.equal(a, require('./module/p1.2/main'));
            });

            it("require", () => {
                var a = require('./module/p1.3');
                assert.deepEqual(a, {
                    "a": 103
                });

                assert.equal(a, require('./module/p1.3'));
                assert.equal(a, require('./module/p1.3/main'));
            });

            it("default", () => {
                var a = require('./module/p1.4');
                assert.deepEqual(a, {
                    "a": 104
                });

                assert.equal(a, require('./module/p1.4'));
                assert.equal(a, require('./module/p1.4/main'));
            });

            it("./require", () => {
                var a = require('./module/p1.5');
                assert.deepEqual(a, {
                    "a": 105
                });

                assert.equal(a, require('./module/p1.5'));
                assert.equal(a, require('./module/p1.5/main'));
            });

            it("./require/default", () => {
                var a = require('./module/p1.6');
                assert.deepEqual(a, {
                    "a": 106
                });

                assert.equal(a, require('./module/p1.6'));
                assert.equal(a, require('./module/p1.6/main'));
            });

            it("./default/require", () => {
                var a = require('./module/p1.7');
                assert.deepEqual(a, {
                    "a": 107
                });

                assert.equal(a, require('./module/p1.7'));
                assert.equal(a, require('./module/p1.7/main'));
            });

            it("./node/require", () => {
                var a = require('./module/p1.8');
                assert.deepEqual(a, {
                    "a": 108
                });

                assert.equal(a, require('./module/p1.8'));
                assert.equal(a, require('./module/p1.8/main'));
            });

            it("./require|default", () => {
                var a = require('./module/p1.9');
                assert.deepEqual(a, {
                    "a": 109
                });

                assert.equal(a, require('./module/p1.9'));
                assert.equal(a, require('./module/p1.9/main'));
            });

            it("./[Array]", () => {
                var a = require('./module/p1.10');
                assert.deepEqual(a, {
                    "a": 1010
                });

                assert.equal(a, require('./module/p1.10'));
                assert.equal(a, require('./module/p1.10/main'));
            });
        });

        it("default entry", () => {
            var a = require('./module/p2');
            assert.deepEqual(a, {
                "a": 200
            });

            assert.equal(a, require('./module/p2'));
            assert.equal(a, require('./module/p2/index'));
        });

        it("no json", () => {
            var a = require('./module/p3');
            assert.deepEqual(a, {
                "a": 300
            });

            assert.equal(a, require('./module/p3'));
            assert.equal(a, require('./module/p3/index'));
        });

        it("entry is folder", () => {
            var a = require('./module/p5');
            assert.deepEqual(a, {
                "a": 500
            });

            assert.equal(a, require('./module/p5'));
            assert.equal(a, require('./module/p5/lib'));
            assert.equal(a, require('./module/p5/lib/index'));
        });

        it("type", () => {
            var a = require('./module/p11');
            assert.deepEqual(a, {
                "p11": "p11"
            });
        });
    });

    describe("node_modules", () => {
        it("root folder", () => {
            var a = require('node_mod1');
            assert.deepEqual(a, {
                "a": 100
            });

            assert.equal(a, require('./node_modules/node_mod1'));
        });

        it("current folder", () => {
            var a = require('./module/mod_test').require("node_mod2");
            assert.deepEqual(a, {
                "a": 200
            });

            assert.equal(a, require('./module/node_modules/node_mod2'));
        });

        it("parent folder", () => {
            var a = require('./module/mod_test').require("node_mod4");
            assert.deepEqual(a, {
                "a": 400
            });

            assert.equal(a, require('./node_modules/node_mod4'));
        });

        it("priority", () => {
            var a = require('./module/mod_test').require("node_mod3");
            assert.deepEqual(a, {
                "a": 300
            });

            assert.equal(a, require('./module/node_modules/node_mod3'));
        });

        describe("sub script", () => {
            it("sub script when package has no main", () => {
                var a = require('sub_script1/test');
                assert.deepEqual(a, {
                    "test": "sub_script1"
                });
            });

            it("sub script when package has main", () => {
                var a = require('sub_script2/test');
                assert.deepEqual(a, {
                    "test": "sub_script2"
                });
            });

            it("simple sub script in exports", () => {
                var a = require('sub_script3/module2');
                assert.deepEqual(a, {
                    "test": "sub_script3"
                });
            });

            it("cannt require sub script not in exports", () => {
                assert.throws(() => {
                    require('sub_script3/script2.js');
                });
            });

            it("pattern sub script", () => {
                var a = require('sub_script4/module4/test_script3.js');
                assert.deepEqual(a, {
                    "test": "sub_script4"
                });
            });

            it("pattern order", () => {
                var a = require('sub_script5/module5/test_script3.js');
                assert.deepEqual(a, {
                    "test": "sub_script5"
                });

                var a = require('sub_script5/module5/test_first.js');
                assert.deepEqual(a, {
                    "test": "sub_script5_first"
                });
            });

            it("internal module", () => {
                var a = require('sub_script6/module6');
                assert.deepEqual(a, {
                    "test": {
                        "test": "sub_script6 native script"
                    }
                });
            });
        });

        it("sub package", () => {
            var a = require("sub_package");
            assert.deepEqual(a, {
                "test": "sub package"
            });

            var a = require("sub_package/other");
            assert.deepEqual(a, {
                "test": "other sub package"
            });
        });

        describe("exports order", () => {
            it("default first", async () => {
                var a = require("test_order1");
                assert.deepEqual(a, {
                    "test": "test3"
                });

                var a = await import("test_order1");
                assert.deepEqual(a, {
                    "test": "test3"
                });
            });

            it("node first", async () => {
                var a = require("test_order2");
                assert.deepEqual(a, {
                    "test": "test2"
                });

                var a = await import("test_order2");
                assert.deepEqual(a, {
                    "test": "test2"
                });
            });

            it("import first", async () => {
                var a = require("test_order3");
                assert.deepEqual(a, {
                    "test": "test4"
                });

                var a = await import("test_order3");
                assert.deepEqual(a, {
                    "test": "test5"
                });
            });

            it("require first", async () => {
                var a = require("test_order4");
                assert.deepEqual(a, {
                    "test": "test4"
                });

                var a = await import("test_order4");
                assert.deepEqual(a, {
                    "test": "test5"
                });
            });
        });
    });

    it("zip virtual path", () => {
        assert.deepEqual(require('./module/test.src/folder/b.js'),
            require('./module/test.zip$/folder/b.js'));

        assert.equal(require('./module/p4.zip$').a, 100);
        assert.equal(require('./module/p4').a, 100);
    });

    it("strack", () => {
        assert.ok(require("./module/stack").func().match(/module_test/));
    });

    it("support exports in script", () => {
        run('./module/exec18');
    });

    it("parallel require", () => {
        var v1;
        var ev = new coroutine.Event();

        setImmediate(() => {
            v1 = require('./module/p8');
            ev.set();
        });

        var v2 = require('./module/p8');
        ev.wait();

        assert.equal(v1, v2);
    });

    it("error process in parallel require", () => {
        var v1, v2;
        var ev = new coroutine.Event();

        setImmediate(() => {
            try {
                require('./module/p9');
            } catch (e) {
                v1 = 1;
            }
            ev.set();
        });

        try {
            require('./module/p9');
        } catch (e) {
            v2 = 1;
        }
        ev.wait();

        assert.equal(v1, v2);
    });

    it("support embed script module", () => {
        var s = require('stream');
        var s1 = require('stream.js');
        assert.equal(s, s1);
    });

    it("addon module", () => {
        var m = require(path.join(bin_path, '1_hello_world'));
        assert.equal(m.hello(), "world");
    });
});

require.main === module && test.run(console.DEBUG);