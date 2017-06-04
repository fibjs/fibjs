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
            assert.equal(e.on('test', t1), e);
            assert.isTrue(e.emit('test', 200, 100));
            assert.equal(1334, v1);
            assert.equal(0, v2);

            assert.equal(e.on('test', t2), e);
            assert.isTrue(e.emit('test', 2000, 1000));
            assert.equal(3568, v1);
            assert.equal(5321, v2);
        });

        it("off", () => {
            assert.equal(e.off('test', t1), e);
            assert.isTrue(e.emit('test', 20, 10));
            assert.equal(3568, v1);
            assert.equal(9652, v2);
        });

        it("once", () => {
            assert.equal(e.once('test', t1), e);
            assert.isTrue(e.emit('test', 20, 10));
            assert.equal(4812, v1);
            assert.equal(13983, v2);
        });

        it("off all", () => {
            assert.equal(e.off('test', t2), e);
            assert.isFalse(e.emit('test', 20, 10));
            assert.equal(4812, v1);
            assert.equal(13983, v2);
        });

        it("on({...})", () => {
            assert.equal(e.on({
                test: t1,
                test1: t2
            }), e);
            assert.isTrue(e.emit('test', 20, 10));
            assert.equal(6056, v1);
            assert.equal(13983, v2);
            assert.isTrue(e.emit('test1', 20, 10));
            assert.equal(6056, v1);
            assert.equal(18314, v2);
        });

        it("off({...})", () => {
            assert.equal(e.off({
                test: t1,
                test1: t2
            }), e);
            assert.isFalse(e.emit('test', 20, 10));
            assert.isFalse(e.emit('test1', 20, 10));
            assert.equal(6056, v1);
            assert.equal(18314, v2);
        });

        it("off(name)", () => {
            assert.equal(e.on('test', t1), e);
            assert.equal(e.on('test', t2), e);
            e.emit('test', 20, 10);
            assert.equal(7300, v1);
            assert.equal(22645, v2);

            assert.equal(e.off("test"), e);

            e.emit('test', 20, 10);
            assert.equal(7300, v1);
            assert.equal(22645, v2);
        });

        it("overwrite", () => {
            assert.equal(e.on('test', t1), e);
            assert.equal(e.once('test', t1), e);
            e.emit('test', 20, 10);
            assert.equal(8544, v1);
            assert.equal(22645, v2);

            e.emit('test', 20, 10);
            assert.equal(8544, v1);
            assert.equal(22645, v2);
        });

        it("listeners(name)", () => {
            assert.equal(e.on('test1', t1), e);
            assert.equal(e.on('test1', t2), e);
            assert.deepEqual(e.listeners("test1"), [t1, t2]);

            assert.equal(e.off('test1', t1), e);
            assert.deepEqual(e.listeners("test1"), [t2]);
        });

        it("removeAllListeners(name)", () => {
            assert.equal(e.on('test2', t1), e);
            assert.equal(e.on('test2', t2), e);

            assert.equal(e.on('test3', t1), e);
            assert.equal(e.on('test3', t2), e);

            assert.equal(e.removeAllListeners(['test2', "test3"]), e);
        });

        it("eventNames()", () => {
            assert.deepEqual(e.eventNames(), [
                "test1",
                "test"
            ]);
        });
    });
}

describe("Trigger/EventEmitter", () => {
    evevt_test("events", new events());
    evevt_test("events.EventEmitter", new events.EventEmitter());

    function MyEmitter() {}

    util.inherits(MyEmitter, events.EventEmitter);
    evevt_test("util.inherits(EventEmitter)", new MyEmitter());

    function MyEmitter1() {}

    util.inherits(MyEmitter1, events);
    evevt_test("util.inherits(events)", new MyEmitter1());

    function MyEmitter2() {
        events.EventEmitter.call(this);
    }

    evevt_test("events.EventEmitter.call", new MyEmitter2());
});

// test.run(console.DEBUG);