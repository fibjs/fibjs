var test = require("test");
test.setup();

var test_util = require('./test_util');

var vm = require('vm');
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

        assert.isUndefined(repl);

        assert.throws(() => {
            sbox.addScript("t2.js", "argv[0];");
        });
    });

    it("standalone global", () => {
        function _t() {}
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

repl && test.run(console.DEBUG);