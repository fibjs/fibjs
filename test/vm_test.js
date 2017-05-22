var test = require("test");
test.setup();

var test_util = require('./test_util');

var vm = require('vm');
var os = require('os');
var fs = require('fs');
var coroutine = require('coroutine');
var util = require('util');

describe("vm", () => {
    var sbox;

    after(() => {
        fs.unlink(__dirname + "/vm_test/jsc_test.jsc");
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

        sbox.require(__dirname + '/vm_test/t1').fun();
        assert.equal(1000, b.a);
    });

    it("addScript", () => {
        var a = sbox.addScript("t1.js", "module.exports = {a : 100};");
        assert.equal(100, a.a);
        assert.equal(100, sbox.require("t1").a);

        var b = sbox.addScript("t2.js", "module.exports = {a : require('t1').a};");
        assert.equal(100, b.a);
        assert.equal(100, sbox.require("t2").a);
    });

    it("compile & addScript", () => {
        var bin = util.compile("tc1.js", "module.exports = {a : 100};");
        var a = sbox.addScript("tc1.jsc", bin);
        assert.equal(100, a.a);
        assert.equal(100, sbox.require("tc1").a);

        var bin1 = util.compile("tc2.js", "module.exports = {a : require('tc1').a};");
        var b = sbox.addScript("tc2.jsc", bin1);
        assert.equal(100, b.a);
        assert.equal(100, sbox.require("tc2").a);

        var bin2 = util.compile("tc3.js", "module.exports = function(v) {return v;}");
        var b = sbox.addScript("tc3.jsc", bin2);
        assert.equal(100, b(100));
    });

    it("require jsc", () => {
        var bin = util.compile("jsc_test.js", "module.exports = {a : 100};");
        fs.writeFile(__dirname + "/vm_test/jsc_test.jsc", bin);
        var a = sbox.require(__dirname + "/vm_test/jsc_test");
        assert.equal(100, a.a);
    });

    it("require jsc arch test", () => {
        console.log("vm_test/jsc_test_" + os.arch);
        var a = sbox.require(__dirname + "/vm_test/jsc_test_" + os.arch);
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

        assert.equal(200, sbox.require("b"));
        assert.equal(100, sbox.require("c"));

        var o1 = sbox.require("o");
        assert.notEqual(o, o1);
        assert.deepEqual(o, o1);
    });

    it("add native object", () => {
        var b = new Buffer();

        sbox = new vm.SandBox({
            b: b
        });

        var b1 = sbox.require("b");
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

        var b1 = sbox.require("b");
        assert.notEqual(b, b1);
        assert.deepEqual(b, b1);
    });

    it("clone", () => {
        sbox = new vm.SandBox({
            a: 100,
            b: 200
        });
        assert.equal(sbox.require("a"), 100);

        var sb1 = sbox.clone();
        assert.equal(sb1.require("a"), 100);

        sb1.add("c", 300);
        assert.equal(sb1.require("c"), 300);

        assert.throws(() => {
            sbox.require("c");
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
        assert.throws(() => {
            repl;
        });

        assert.throws(() => {
            sbox.addScript("t2.js", "argv[0];");
        });
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
        assert.equal(no1 + 1, test_util.countObject('Buffer'));

        a = undefined;
        GC();

        assert.equal(no1, test_util.countObject('Buffer'));
    });
});

// test.run(console.DEBUG);