var test = require("test");
test.setup();

var test_util = require('./test_util');

var vm = require('vm');
var gd = require('gd');
var os = require('os');
var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');
var util = require('util');

describe("vm", () => {
    var sbox;

    after(() => {
        fs.unlink(path.join(__dirname, "vm_test", "jsc_test.jsc"));
    });

    it("add", () => {
        var b = {
            a: 1000
        }

        sbox = new vm.SandBox({
            a: 100,
            assert: assert,
            b: b
        }, (name) => {
            if (name == 'c')
                return 300;
        });

        sbox.add('a', new Number(100));
        sbox.add('a', 100);
        sbox.add('coroutine', require('coroutine'));

        sbox.require('./vm_test/t1', __dirname).fun();
        assert.equal(1000, b.a);
    });
    it("has", () => {
        var b = {
            a: 1000
        }

        sbox = new vm.SandBox({
            a: 100,
            assert: assert,
            b: b
        }, (name) => {
            if (name == 'c')
                return 300;
        });

        sbox.add('a', new Number(100));
        sbox.add('coroutine', require('coroutine'));
        assert.equal(true, sbox.has('a'));
        assert.equal(true, sbox.has('coroutine'));
        assert.equal(false, sbox.has('foo'));
    });

    it("addScript", () => {
        var a = sbox.addScript("t1.js", "module.exports = {a : 100};");
        assert.equal(100, a.a);
        assert.equal(100, sbox.require("t1", __dirname).a);

        var b = sbox.addScript("t2.js", "module.exports = {a : require('t1').a};");
        assert.equal(100, b.a);
        assert.equal(100, sbox.require("t2", __dirname).a);
    });

    it("compile & addScript", () => {
        var bin = util.compile("tc1.js", "module.exports = {a : 100};");
        var a = sbox.addScript("tc1.jsc", bin);
        assert.equal(100, a.a);
        assert.equal(100, sbox.require("tc1", __dirname).a);

        var bin1 = util.compile("tc2.js", "module.exports = {a : require('tc1').a};");
        var b = sbox.addScript("tc2.jsc", bin1);
        assert.equal(100, b.a);
        assert.equal(100, sbox.require("tc2", __dirname).a);

        var bin2 = util.compile("tc3.js", "module.exports = function(v) {return v;}");
        var b = sbox.addScript("tc3.jsc", bin2);
        assert.equal(100, b(100));

        var bin3 = util.compile("tc4.js", "\n\n\nmodule.exports = \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nfunction() {\nreturn new Error().stack;}");
        var b = sbox.addScript("tc4.jsc", bin3);
        assert.equal(b().split('\n')[1].trim(), "at module.exports (tc4.js:26:8)");
    });

    it("require jsc", () => {
        var bin = util.compile("jsc_test.js", "module.exports = {a : 100};");
        fs.writeFile(path.join(__dirname, "vm_test", "jsc_test.jsc"), bin);
        var a = sbox.require("./vm_test/jsc_test", __dirname);
        assert.equal(100, a.a);
    });

    it("require jsc arch test", () => {
        var a = sbox.require("./vm_test/jsc_test_" + os.arch(), __dirname);
        assert.equal(100, a.a);
    });

    it("module", () => {
        sbox = new vm.SandBox({
            assert: assert
        });

        sbox.addScript("t1.js", "assert.equal(module.exports, exports);");
        sbox.addScript("t2.js", "assert.equal(module.require, require);");
    });

    it("callback", () => {
        var b = 200;
        var o = {
            a: 100,
            b: 200
        };

        sbox = new vm.SandBox({
            b: b
        }, (n) => {
            if (n == 'c')
                return 100;
            if (n == 'o')
                return o;
        });

        assert.equal(200, sbox.require("b", __dirname));
        assert.equal(100, sbox.require("c", __dirname));

        var o1 = sbox.require("o", __dirname);
        assert.equal(o, o1);
        // assert.deepEqual(o, o1);
    });

    it("add native object", () => {
        var b = new Buffer();

        sbox = new vm.SandBox({
            b: b
        });

        var b1 = sbox.require("b", __dirname);
        assert.equal(b, b1);
    });

    it("add javascript object", () => {
        var b = {
            a: 100,
            b: 200
        };

        sbox = new vm.SandBox({
            b: b
        });

        var b1 = sbox.require("b", __dirname);
        assert.notEqual(b, b1);
        assert.deepEqual(b, b1);
    });


    it("setModuleLoader: ext recognition", () => {
        var sbox = new vm.SandBox({});

        assert.throws(() => {
            sbox.require('./vm_test/custom_ext', __dirname);
        })
        sbox.require('./vm_test/custom_ext.abc', __dirname);

        sbox.setModuleLoader('.abc', function (buf) {
        });
        sbox.require('./vm_test/custom_ext', __dirname);
    })

    it("setModuleLoader: basic", () => {
        var sbox = new vm.SandBox({});

        var testVarValue = 'lalala'

        sbox.setModuleLoader('.abc', function (buf) {
            return testVarValue;
        });

        var testVar = sbox.require('./vm_test/custom_ext', __dirname);
        assert.equal(testVar, 'lalala');

        assert.throws(() => {
            (new SandBox({})).setModuleLoader('^abc', function (buf) {
                return testVarValue;
            });
        });
        assert.throws(() => {
            (new SandBox({})).setModuleLoader('-abc', function (buf) {
                return testVarValue;
            });
        })
    })

    it("setModuleLoader: requireInfo", () => {
        var sbox = new vm.SandBox({});

        sbox.setModuleLoader('.abc', function (buf, requireInfo) {
            return requireInfo;
        });

        var expectedRequireInfo = {
            dirname: path.resolve(__dirname, './vm_test'),
            filename: path.resolve(__dirname, './vm_test/custom_ext.abc')
        }

        var testArgs = sbox.require('./vm_test/custom_ext', __dirname);
        assert.deepEqual(testArgs, expectedRequireInfo);
    })

    it("setModuleLoader: cached require", () => {
        var sbox = new vm.SandBox({});

        var testVarValue = 'I am not increasing:'
        var initialCnt = cnt = 0;
        sbox.setModuleLoader('.abc', function (buf) {
            var _return = testVarValue + cnt;

            cnt++;
            return _return;
        });
        assert.equal(cnt, initialCnt);

        for (var i = initialCnt; i < 10; i++) {
            var requiredVal = sbox.require('./vm_test/custom_ext', __dirname);
            assert.equal(cnt, initialCnt + 1);
            assert.equal(requiredVal, testVarValue + initialCnt);
        }
    })

    it("setModuleLoader: re-setModuleLoader", () => {
        var sbox1 = new vm.SandBox({});
        var sbox2 = new vm.SandBox({});

        var testVarValue = 'miaomiaomiao'

        var t0 = Date.now();
        sbox1.setModuleLoader('.abc', function (buf) {
            return { test: testVarValue + t0 };
        });

        var var0;

        var0 = sbox1.require('./vm_test/custom_ext', __dirname);

        assert.deepEqual(var0, { test: testVarValue + t0 })
        assert.equal(JSON.stringify(var0), JSON.stringify({ test: testVarValue + t0 }))

        var0 = sbox1.require('./vm_test/custom_ext', __dirname);

        assert.equal(JSON.stringify(var0), JSON.stringify({ test: testVarValue + t0 }));
        assert.notEqual(var0, testVarValue);

        var testVar1;

        /* 
            #4d04262b-d752-4d18-a9ad-a107cbd05682

            moduld would be cached via its moduleId. so if you really want to re-setModuleLoader,
            it's better to delete the sandbox and build new one, then re-require all modules.
         */
        sbox1 = new vm.SandBox({})
        sbox1.setModuleLoader('.abc', function (buf) {
            return testVarValue;
        });
        testVar1 = sbox1.require('./vm_test/custom_ext', __dirname);
        assert.equal(testVar1, testVarValue);

        var t = Date.now();

        var testVar2;
        sbox2.setModuleLoader('.abc', function (buf) {
            return testVarValue + t;
        });
        testVar2 = sbox2.require('./vm_test/custom_ext.abc', __dirname);
        assert.equal(testVar2, testVarValue + t);
    })

    it("setModuleLoader: internal extname", () => {
        assert.throws(() => { (new vm.SandBox({})).setModuleLoader('.js', () => undefined); });
        assert.throws(() => { (new vm.SandBox({})).setModuleLoader('.jsc', () => undefined); });
        assert.throws(() => { (new vm.SandBox({})).setModuleLoader('.json', () => undefined); });
        assert.throws(() => { (new vm.SandBox({})).setModuleLoader('.wasm', () => undefined); });

        (new vm.SandBox({})).setModuleLoader('.ts', () => undefined);
    })

    it("setModuleLoader: binary", () => {
        const sbox = new vm.SandBox({});
        sbox.setModuleLoader('.png', (buf) => {
            return gd.load(buf);
        });

        var image1 = sbox.require('./vm_test/custom_ext.png', __dirname);
        var imageBuf1 = image1.getData(gd.PNG, 1);

        var image2 = gd.load(
            path.resolve(__dirname, './vm_test/custom_ext.png')
        );
        var imageBuf2 = image2.getData(gd.PNG, 1);

        assert.equal(imageBuf1.toString('base64'), imageBuf2.toString('base64'))

        sbox.setModuleLoader('.error', (base64Str) => {
            nonExistedValue = nonExistedValue1
            gd.load(base64Str);
            return nonExistedValue;
        })

        assert.throws(() => {
            sbox.require('./vm_test/custom_ext.error, __dirname')
        })
    })

    it("clone", () => {
        sbox = new vm.SandBox({
            a: 100,
            b: 200
        });
        assert.equal(sbox.require("a", __dirname), 100);

        var sb1 = sbox.clone();
        assert.equal(sb1.require("a", __dirname), 100);

        sb1.add("c", 300);
        assert.equal(sb1.require("c", __dirname), 300);

        assert.throws(() => {
            sbox.require("c", __dirname);
        });
    });

    xit("block global hacker", () => {
        sbox = new vm.SandBox({});
        assert.throws(() => {
            sbox.addScript("t1.js", "});(() => {");
        });
    });

    it("block global repl&argv", () => {
        sbox = new vm.SandBox({});

        if (require.main === module) {
            assert.isFunction(repl);
        } else {
            // when this file is tested alone, 'repl' is undefined
            assert.isUndefined(repl);
        }

        assert.throws(() => {
            sbox.addScript("t2.js", "argv[0];");
        });
    });

    it("standalone global", () => {
        function _t() { }
        var sbox1 = new vm.SandBox({}, {
            var1: 100,
            func: _t
        });

        sbox1.require('./vm_test/hack_global', __dirname);
        sbox1.run(path.join(__dirname, 'vm_test', 'hack_global_1'));
        var a = sbox1.addScript("t1.js", "global.var2 = 200;module.exports={var1:var1,var2:var2,var3:var3,var4:var4};");

        assert.equal(a.var1, 100);
        assert.equal(a.var2, 200);
        assert.equal(a.var3, 300);
        assert.equal(a.var4, 400);

        assert.equal(sbox1.global.var1, 100);
        assert.equal(sbox1.global.var2, 200);
        assert.equal(sbox1.global.var3, 300);
        assert.equal(sbox1.global.var4, 400);
        assert.equal(sbox1.global.func, _t);
        assert.isUndefined(sbox1.global.console);

        assert.isUndefined(global.var1);
        assert.isUndefined(global.var2);
        assert.isUndefined(global.var3);
        assert.isUndefined(global.var4);
    });

    it("standalone global obj type error", () => {
        var sbox1 = new vm.SandBox({}, {
            mq: require('mq')
        });

        sbox1.addScript("t1.js", "new mq.Routing({});");
    });

    it("require.cache", () => {
        assert.isObject(require.cache);
    });

    xit("block function return", () => {
        sbox = new vm.SandBox({});
        assert.throws(() => {
            sbox.addScript("t1.js", "return 100;");
        });
    });

    it("Garbage Collection", () => {
        sbox = undefined;

        GC();
        var no1 = test_util.countObject('Buffer');

        sbox = new vm.SandBox({});
        assert.equal(no1, test_util.countObject('Buffer'));

        var a = sbox.addScript("t1.js", "module.exports = {a : new Buffer()};");
        GC();
        assert.equal(no1 + 1, test_util.countObject('Buffer'));

        sbox = undefined;

        GC();
        assert.equal(no1 + 1, test_util.countObject('Buffer'));

        a = undefined;

        GC();
        assert.equal(no1, test_util.countObject('Buffer'));
    });

    it("Garbage Collection 1", () => {
        GC();
        var no1 = test_util.countObject('Buffer');

        var a = {
            b: new vm.SandBox({}).addScript('b.js', "module.exports = new Buffer()")
        };
        GC();
        assert.equal(no1 + 1, test_util.countObject('Buffer'));

        a = undefined;
        GC();

        assert.equal(no1, test_util.countObject('Buffer'));
    });
});

require.main === module && test.run(console.DEBUG);
