var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var coroutine = require('coroutine');
var events = require('events');
var util = require('util');
var path = require('path');

var Worker = coroutine.Worker;

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

        beforeEach(() => {
            v1 = 123456;
            v2 = 654321;
        });

        it("on", () => {
            assert.equal(e.on('test', t1), e);
            assert.isTrue(e.emit('test', 200, 100));
            assert.equal(124790, v1);
            assert.equal(654321, v2);

            assert.equal(e.on('test', t2), e);
            assert.isTrue(e.emit('test', 2000, 1000));
            assert.equal(127024, v1);
            assert.equal(659642, v2);
        });

        it("listenerCount", () => {
            assert.equal(e.listenerCount('test'), 2);
            assert.equal(events.listenerCount(e, 'test'), 2);
        });

        it("prependListener", () => {
            v1 = v2 = 0;
            assert.equal(e.prependListener('test', t1), e);
            assert.isTrue(e.emit('test', 200, 100));
            assert.equal(2668, v1);
            assert.equal(4421, v2);

            assert.equal(e.prependListener('test', t2), e);
            assert.isTrue(e.emit('test', 2000, 1000));
            assert.equal(7136, v1);
            assert.equal(15063, v2);
        });

        it("off", () => {
            assert.equal(e.off('test', t1), e);
            assert.isTrue(e.emit('test', 20, 10));
            assert.equal(124700, v1);
            assert.equal(662983, v2);
        });

        it("prependOnceListener", () => {
            assert.equal(e.prependOnceListener('test', t1), e);
            assert.isTrue(e.emit('test', 20, 10));
            assert.equal(125944, v1);
            assert.equal(662983, v2);
        });

        it("once", () => {
            assert.equal(e.once('test', t1), e);
            assert.isTrue(e.emit('test', 20, 10));
            assert.equal(125944, v1);
            assert.equal(662983, v2);
        });

        it("off all", () => {
            assert.equal(e.off('test', t1), e);
            assert.equal(e.off('test', t2), e);
            assert.equal(e.off('test', t2), e);
            assert.isFalse(e.emit('test', 20, 10));
            assert.equal(123456, v1);
            assert.equal(654321, v2);
        });

        it("addEventListener", () => {
            assert.equal(e.addEventListener('test7', t1), e);
            assert.isTrue(e.emit('test7', 200, 100));
            assert.equal(124790, v1);
            assert.equal(654321, v2);

            assert.equal(e.addEventListener('test8', t2, {
                once: true
            }), e);
            assert.isTrue(e.emit('test8', 200, 100));
            assert.equal(124790, v1);
            assert.equal(658742, v2);

            assert.isFalse(e.emit('test8', 200, 100));
            assert.equal(124790, v1);
            assert.equal(658742, v2);
        });

        it("removeEventListener", () => {
            assert.equal(e.removeEventListener('test', t1), e);
            assert.isFalse(e.emit('test', 20, 10));
            assert.equal(123456, v1);
            assert.equal(654321, v2);
        });

        it("on({...})", () => {
            assert.equal(e.on({
                test: t1,
                test1: t2
            }), e);
            assert.isTrue(e.emit('test', 20, 10));
            assert.equal(124700, v1);
            assert.equal(654321, v2);
            assert.isTrue(e.emit('test1', 20, 10));
            assert.equal(124700, v1);
            assert.equal(658652, v2);
        });

        it("off({...})", () => {
            assert.equal(e.off({
                test: t1,
                test1: t2
            }), e);
            assert.isFalse(e.emit('test', 20, 10));
            assert.isFalse(e.emit('test1', 20, 10));
            assert.equal(123456, v1);
            assert.equal(654321, v2);
        });

        it("off(name)", () => {
            assert.equal(e.on('test', t1), e);
            assert.equal(e.on('test', t2), e);
            e.emit('test', 20, 10);
            assert.equal(124700, v1);
            assert.equal(658652, v2);

            assert.equal(e.off("test"), e);

            e.emit('test', 20, 10);
            assert.equal(124700, v1);
            assert.equal(658652, v2);
        });

        it("more than on listeners", () => {
            assert.equal(e.on('test', t1), e);
            assert.equal(e.once('test', t1), e);
            e.emit('test', 20, 10);
            assert.equal(125944, v1);
            assert.equal(654321, v2);

            e.emit('test', 20, 10);
            assert.equal(127188, v1);
            assert.equal(654321, v2);
        });

        it("order", () => {
            var res = [];
            var fn1 = () => {
                res.push(1);
            };
            var fn2 = () => {
                res.push(2);
            };
            assert.equal(e.on('order', fn1), e);
            assert.equal(e.on('order', fn2), e);
            assert.equal(e.on('order', fn1), e);
            assert.equal(e.on('order', fn2), e);
            assert.equal(e.on('order', fn1), e);

            e.emit('order');
            assert.deepEqual(res, [
                1, 2, 1, 2, 1
            ]);

            e.off('order', fn1);
            res = [];

            e.emit('order');
            assert.deepEqual(res, [
                1, 2, 1, 2
            ]);

            e.off('order', fn1);
            res = [];

            e.emit('order');
            assert.deepEqual(res, [
                1, 2, 2
            ]);

            e.off('order', fn2);
            res = [];

            e.emit('order');
            assert.deepEqual(res, [
                1, 2
            ]);

            e.off('order');
        });

        it("on error", () => {
            e.on('error', () => { });
            e.emit('error');
            e.off('error');

            e.on('error', () => {
                throw new Error('error');
            });
            assert.throws(() => {
                e.emit('error');
            });
            e.off('error');

            e.on('error', () => { });
            e.on('error', () => {
                throw new Error('error');
            });
            assert.throws(() => {
                e.emit('error');
            });
            e.off('error');
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

            e.emit('test2', 20, 10);
            assert.equal(124700, v1);

            assert.equal(e.removeAllListeners(['test2', "test3"]), e);

            e.emit('test2', 20, 10);
            assert.equal(124700, v1);
        });

        it("removeAllListeners(name)", () => {
            assert.equal(e.on('test2', t1), e);
            assert.equal(e.on('test2', t2), e);

            assert.equal(e.on('test3', t1), e);
            assert.equal(e.on('test3', t2), e);

            e.emit('test2', 20, 10);
            assert.equal(124700, v1);

            assert.equal(e.removeAllListeners(), e);

            e.emit('test2', 20, 10);
            assert.equal(124700, v1);
        });

        it("eventNames()", () => {
            e.on('test', t1);
            e.on('test1', t1);
            e.on('test2', t1);
            e.on('test3', t1);
            e.on('test4', t1);
            assert.deepEqual(e.eventNames(), [
                "test",
                "test1",
                "test2",
                "test3",
                "test4",
            ]);
        });

        it("getMaxListeners()", () => {
            assert.equal(e.getMaxListeners(), 10);
        });

        it("setMaxListeners()", () => {
            e.setMaxListeners(1);
            assert.equal(e.getMaxListeners(), 1);
        });

        describe("newListener Event", () => {
            it("once", () => {
                var type, fn;

                var _fn1 = () => { };
                var _fn2 = () => { };

                e.once('newListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.once('a', _fn1);
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.once('a', _fn2);
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.on('newListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.once('a', _fn2);
                assert.equal(type, 'a');
                assert.deepEqual(_fn2, fn);

                e.off('newListener');
            });

            it("on", () => {
                var type, fn;

                var _fn1 = () => { };
                var _fn2 = () => { };

                e.once('newListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.on('a', _fn1);
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.on('a', _fn2);
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.on('newListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.on('a', _fn2);
                assert.equal(type, 'a');
                assert.deepEqual(_fn2, fn);

                e.off('newListener');
                e.off('a');
            });
        });

        describe("removeListener Event", () => {
            it("once", () => {
                var type, fn;

                var _fn1 = () => { };
                var _fn2 = () => { };

                e.once('removeListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.once('a', _fn1);
                e.emit('a');
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.once('a', _fn2);
                e.emit('a');
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.on('removeListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.once('a', _fn2);
                e.emit('a');
                assert.equal(type, 'a');
                assert.deepEqual(_fn2, fn);

                e.off('removeListener');
            });

            it("on", () => {
                var type, fn;

                var _fn1 = () => { };
                var _fn2 = () => { };

                e.once('removeListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.on('a', _fn1);
                e.off('a', _fn1);
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.on('a', _fn2);
                assert.equal(type, 'a');
                assert.deepEqual(_fn1, fn);

                e.on('removeListener', (...argvs) => {
                    type = argvs[0];
                    fn = argvs[1];
                });

                e.on('a', _fn2);
                e.off('a', _fn2);
                assert.equal(type, 'a');
                assert.deepEqual(_fn2, fn);

                e.off('removeListener');
                e.off('a');
            });

            it("off all", () => {
                var _fn1 = () => { };
                var _fn2 = () => { };
                var fns = [];

                e.on('removeListener', (type, fn) => {
                    fns.push(fn);
                });

                e.on('a', _fn1);
                e.on('a', _fn2);
                e.off('a');

                assert.equal(fns[0], _fn2);
                assert.equal(fns[1], _fn1);

                e.off('removeListener');
            });
        });
    });
}

describe("Trigger/EventEmitter", () => {
    evevt_test("events", new events());
    evevt_test("events.EventEmitter", new events.EventEmitter());

    function MyEmitter() { }

    util.inherits(MyEmitter, events.EventEmitter);
    evevt_test("util.inherits(EventEmitter)", new MyEmitter());

    function MyEmitter1() { }

    util.inherits(MyEmitter1, events);
    evevt_test("util.inherits(events)", new MyEmitter1());

    function MyEmitter2() {
        events.EventEmitter.call(this);
    }

    evevt_test("events.EventEmitter.call", new MyEmitter2());

    it('bugfix: crash when off function on new Emitter', () => {
        var ev = new events.EventEmitter();
        ev.off('a', () => { });
    })

    describe("defaultMaxListeners", () => {
        it('getter', () => {
            assert.equal(events.defaultMaxListeners, 10);
            assert.equal(events.EventEmitter.defaultMaxListeners, 10);
        });

        it('setter', () => {
            events.defaultMaxListeners = 12;
            assert.equal(events.defaultMaxListeners, 12);
            assert.equal(events.EventEmitter.defaultMaxListeners, 12);

            events.EventEmitter.defaultMaxListeners = 13;
            assert.equal(events.defaultMaxListeners, 13);
            assert.equal(events.EventEmitter.defaultMaxListeners, 13);

            assert.throws(() => events.defaultMaxListeners = -1);

            events.defaultMaxListeners = 10;
        });

        it('isolate', () => {
            var worker = new Worker(path.join(__dirname, 'event_files', 'worker.js'));
            worker.onload = () => worker.postMessage('get');

            var get_worker_max_listeners = util.sync((done) => {
                worker.onmessage = (evt) => done(null, evt.data);
            });

            var change_worker_max_listeners = util.sync((done) => {
                worker.onmessage = (evt) => done(null, evt.data);
                worker.postMessage('');
            });

            events.defaultMaxListeners = 11;
            assert.equal(get_worker_max_listeners(), 10);

            assert.equal(change_worker_max_listeners(), 12);
            assert.equal(events.defaultMaxListeners, 11);

            events.defaultMaxListeners = 10;
        });
    });

    describe("error event without listener", () => {
        it("should throw when emitting error with Error object and no listener", () => {
            var e = new events.EventEmitter();
            assert.throws(() => {
                e.emit('error', new Error('test error'));
            }, err => {
                assert.equal(err.message, 'test error');
                return true;
            });
        });

        it("should throw wrapped error when emitting error with string and no listener", () => {
            var e = new events.EventEmitter();
            assert.throws(() => {
                e.emit('error', 'something went wrong');
            }, err => {
                assert.ok(err.message.indexOf('Unhandled error') >= 0);
                assert.ok(err.message.indexOf('something went wrong') >= 0);
                return true;
            });
        });

        it("should throw when emitting error with no arguments and no listener", () => {
            var e = new events.EventEmitter();
            assert.throws(() => {
                e.emit('error');
            }, err => {
                assert.ok(err.message.indexOf('Unhandled error') >= 0);
                return true;
            });
        });

        it("should not throw when error listener is registered", () => {
            var e = new events.EventEmitter();
            var caught = null;
            e.on('error', (err) => {
                caught = err;
            });
            assert.isTrue(e.emit('error', new Error('handled error')));
            assert.equal(caught.message, 'handled error');
        });

        it("should not throw for non-error events without listener", () => {
            var e = new events.EventEmitter();
            assert.isFalse(e.emit('someEvent', 'data'));
        });
    });

    describe("rawListeners", () => {
        it("should return wrapper for once listeners", () => {
            var e = new events();
            var fn = function myFunc() {};
            e.once('test', fn);
            e.on('test', fn);

            var raw = e.rawListeners('test');
            assert.equal(raw.length, 2);
            assert.equal(typeof raw[0]._func, 'function');
            assert.equal(raw[0]._func, fn);
            assert.equal(raw[1], fn);
        });

        it("listeners should unwrap once wrappers", () => {
            var e = new events();
            var fn = function myFunc() {};
            e.once('test', fn);

            var lis = e.listeners('test');
            assert.equal(lis.length, 1);
            assert.equal(lis[0], fn);
            assert.equal(lis[0].name, 'myFunc');
        });

        it("rawListeners wrapper should be callable", () => {
            var e = new events();
            var called = false;
            e.once('test', () => { called = true; });

            var raw = e.rawListeners('test');
            assert.equal(raw.length, 1);

            // Call the wrapper directly - it should fire and auto-remove
            raw[0]();
            assert.isTrue(called);
        });
    });

    describe("removeListener with once wrapper", () => {
        it("should remove once listener by original function", () => {
            var e = new events();
            var fn = () => {};
            e.once('test', fn);
            assert.equal(e.listenerCount('test'), 1);
            e.removeListener('test', fn);
            assert.equal(e.listenerCount('test'), 0);
        });

        it("should remove correct once listener among multiple", () => {
            var e = new events();
            var fn1 = () => {};
            var fn2 = () => {};
            e.once('test', fn1);
            e.once('test', fn2);
            assert.equal(e.listenerCount('test'), 2);
            e.removeListener('test', fn1);
            assert.equal(e.listenerCount('test'), 1);

            var remaining = e.listeners('test');
            assert.equal(remaining[0], fn2);
        });
    });

    describe("events.addAbortListener", () => {
        it("should call listener on abort", () => {
            var ac = new AbortController();
            var called = false;
            events.addAbortListener(ac.signal, () => { called = true; });
            ac.abort();
            assert.isTrue(called);
        });

        it("should return disposable with Symbol.dispose", () => {
            var ac = new AbortController();
            var d = events.addAbortListener(ac.signal, () => {});
            assert.equal(typeof d[Symbol.dispose], 'function');
        });

        it("should call listener immediately if already aborted", () => {
            var ac = new AbortController();
            ac.abort();
            var called = false;
            events.addAbortListener(ac.signal, () => { called = true; });
            assert.isTrue(called);
        });

        it("should not call listener after dispose", () => {
            var ac = new AbortController();
            var called = false;
            var d = events.addAbortListener(ac.signal, () => { called = true; });
            d[Symbol.dispose]();
            ac.abort();
            assert.isFalse(called);
        });
    });

    describe("events.once (static)", () => {
        it("should resolve with args array on event", async () => {
            var e = new events();
            setTimeout(() => e.emit('data', 'hello', 42), 10);
            var result = await events.once(e, 'data');
            assert.isTrue(Array.isArray(result));
            assert.equal(result[0], 'hello');
            assert.equal(result[1], 42);
        });

        it("should reject on error event", async () => {
            var e = new events();
            setTimeout(() => e.emit('error', new Error('boom')), 10);
            try {
                await events.once(e, 'data');
                assert.ok(false, 'should have rejected');
            } catch (err) {
                assert.equal(err.message, 'boom');
            }
        });

        it("should resolve when listening for error event itself", async () => {
            var e = new events();
            setTimeout(() => e.emit('error', new Error('expected')), 10);
            var result = await events.once(e, 'error');
            assert.equal(result[0].message, 'expected');
        });

        it("should remove error listener after event fires", async () => {
            var e = new events();
            setTimeout(() => e.emit('data', 'ok'), 10);
            await events.once(e, 'data');
            assert.equal(e.listenerCount('data'), 0);
            assert.equal(e.listenerCount('error'), 0);
        });

        it("should reject with AbortError if signal already aborted", async () => {
            var ac = new AbortController();
            ac.abort();
            try {
                await events.once(new events(), 'data', { signal: ac.signal });
                assert.ok(false, 'should have rejected');
            } catch (err) {
                assert.equal(err.name, 'AbortError');
            }
        });

        it("should reject with AbortError when signal fires", async () => {
            var e = new events();
            var ac = new AbortController();
            setTimeout(() => ac.abort(), 10);
            try {
                await events.once(e, 'data', { signal: ac.signal });
                assert.ok(false, 'should have rejected');
            } catch (err) {
                assert.equal(err.name, 'AbortError');
            }
        });
    });

    describe("events.on (static)", () => {
        it("should yield event args as arrays", async () => {
            var e = new events();
            setTimeout(() => {
                e.emit('data', 'a');
                e.emit('data', 'b');
                e.emit('data', 'c');
            }, 10);

            var collected = [];
            var iter = events.on(e, 'data');
            for await (var args of iter) {
                collected.push(args[0]);
                if (collected.length >= 3) break;
            }
            assert.deepEqual(collected, ['a', 'b', 'c']);
        });

        it("should support close option", async () => {
            var e = new events();
            setTimeout(() => {
                e.emit('data', 'x');
                e.emit('end');
            }, 10);

            var collected = [];
            for await (var args of events.on(e, 'data', { close: ['end'] })) {
                collected.push(args[0]);
            }
            assert.deepEqual(collected, ['x']);
        });

        it("should reject on error event", async () => {
            var e = new events();
            setTimeout(() => e.emit('error', new Error('fail')), 10);

            try {
                for await (var args of events.on(e, 'data')) {
                    // should not reach here
                }
                assert.ok(false, 'should have thrown');
            } catch (err) {
                assert.equal(err.message, 'fail');
            }
        });

        it("should reject with AbortError if signal already aborted", async () => {
            var ac = new AbortController();
            ac.abort();
            var iter = events.on(new events(), 'data', { signal: ac.signal });
            try {
                await iter.next();
                assert.ok(false, 'should have rejected');
            } catch (err) {
                assert.equal(err.name, 'AbortError');
            }
        });

        it("return() should end iteration", async () => {
            var e = new events();
            var iter = events.on(e, 'data');
            var ret = await iter.return();
            assert.isTrue(ret.done);
            assert.isUndefined(ret.value);
        });

        it("throw() should reject", async () => {
            var e = new events();
            var iter = events.on(e, 'data');
            try {
                await iter.throw(new Error('thrown'));
                assert.ok(false, 'should have rejected');
            } catch (err) {
                assert.equal(err.message, 'thrown');
            }
        });

        it("should have Symbol.asyncIterator", () => {
            var e = new events();
            var iter = events.on(e, 'data');
            assert.equal(typeof iter[Symbol.asyncIterator], 'function');
            assert.equal(iter[Symbol.asyncIterator](), iter);
            iter.return();
        });
    });
});

