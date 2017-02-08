var test = require("test");
test.setup();

var coroutine = require('coroutine');
var events = require('events');
var util = require('util');

function evevt_test(name, e) {
    describe(name, () => {
        var v1, v2;

        function t1(a1, a2) {
            assert.equal(this, e);
            v1 = v1 + a1 - a2 + 1234;
        }

        function t2(a1, a2) {
            assert.equal(this, e);
            v2 = v2 + a1 - a2 + 4321;
        }

        it("on", () => {
            v1 = v2 = 0;
            assert.equal(e.on('test', t1), 1);
            e.trigger('test', 200, 100);
            assert.equal(1334, v1);
            assert.equal(0, v2);

            assert.equal(e.on('test', t2), 1);
            e.trigger('test', 2000, 1000);
            assert.equal(3568, v1);
            assert.equal(5321, v2);
        });

        it("off", () => {
            assert.equal(e.off('test', t1), 1);
            e.trigger('test', 20, 10);
            assert.equal(3568, v1);
            assert.equal(9652, v2);
        });

        it("once", () => {
            assert.equal(e.once('test', t1), 1);
            e.trigger('test', 20, 10);
            assert.equal(4812, v1);
            assert.equal(13983, v2);
        });

        it("off all", () => {
            assert.equal(e.off('test', t2), 1);
            e.trigger('test', 20, 10);
            assert.equal(4812, v1);
            assert.equal(13983, v2);
        });

        it("on({...})", () => {
            assert.equal(e.on({
                test: t1,
                test1: t2
            }), 2);
            e.trigger('test', 20, 10);
            assert.equal(6056, v1);
            assert.equal(13983, v2);
            e.trigger('test1', 20, 10);
            assert.equal(6056, v1);
            assert.equal(18314, v2);
        });

        it("off({...})", () => {
            assert.equal(e.off({
                test: t1,
                test1: t2
            }), 2);
            e.trigger('test', 20, 10);
            e.trigger('test1', 20, 10);
            assert.equal(6056, v1);
            assert.equal(18314, v2);
        });

        it("off(name)", () => {
            assert.equal(e.on('test', t1), 1);
            assert.equal(e.on('test', t2), 1);
            e.trigger('test', 20, 10);
            assert.equal(7300, v1);
            assert.equal(22645, v2);

            assert.equal(e.off("test"), 2);

            e.trigger('test', 20, 10);
            assert.equal(7300, v1);
            assert.equal(22645, v2);
        });

        it("overwrite", () => {
            assert.equal(e.on('test', t1), 1);
            assert.equal(e.once('test', t1), 0);
            e.trigger('test', 20, 10);
            assert.equal(8544, v1);
            assert.equal(22645, v2);

            e.trigger('test', 20, 10);
            assert.equal(8544, v1);
            assert.equal(22645, v2);
        });

        it("listeners(name)", () => {
            assert.equal(e.on('test1', t1), 1);
            assert.equal(e.on('test1', t2), 1);
            assert.deepEqual(e.listeners("test1"), [t1, t2]);

            assert.equal(e.off('test1', t1), 1);
            assert.deepEqual(e.listeners("test1"), [t2]);
        });

        it("removeAllListeners(name)", () => {
            assert.equal(e.on('test2', t1), 1);
            assert.equal(e.on('test2', t2), 1);

            assert.equal(e.on('test3', t1), 1);
            assert.equal(e.on('test3', t2), 1);

            assert.equal(e.removeAllListeners(['test2', "test3"]), 4);
        });
    });
}

describe("Trigger/EventEmitter", () => {
    evevt_test("coroutine.Trigger", new coroutine.Trigger());
    evevt_test("events.EventEmitter", new events.EventEmitter());

    function MyEmitter() {}

    util.inherits(MyEmitter, events.EventEmitter);
    evevt_test("util.inherits", new MyEmitter());
});

// test.run(console.DEBUG);
