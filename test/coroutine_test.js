var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var test_util = require('./test_util');

var path = require('path');
var os = require('os');
var net = require('net');
var util = require('util');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

var n;

describe('coroutine', () => {
    describe('Fiber', () => {
        function t_fiber(v1, v2) {
            n = v1 + v2;
        }

        it('start', () => {
            n = 123;

            var f = coroutine.start(t_fiber, 100, 200);
            assert.equal(n, 123);

            for (var i = 0; i < 1000 && n == 123; i++)
                coroutine.sleep(1);

            assert.equal(n, 300);

        });

        it('join', () => {
            n = 300;

            var f = coroutine.start(t_fiber, 123, 200);
            assert.equal(n, 300);
            f.join();
            assert.equal(n, 323);
        });

        it("Memory Leak detect", () => {
            test_util.gc();
            var no1 = test_util.countObject('Event');
            var f = coroutine.start((v) => { }, new coroutine.Event());
            test_util.gc();
            assert.equal(no1 + 1, test_util.countObject('Event'));
            f.join();

            test_util.gc();
            assert.equal(no1, test_util.countObject('Event'));

            f = undefined;
            test_util.gc();
            assert.equal(no1, test_util.countObject('Event'));
        });

        it('Fiber-local storage', () => {
            function t_fiber1(v1, v2) {
                n = v1 + v2 + this.v;
            }

            n = 323;

            var f = coroutine.start(t_fiber1, 100, 200);
            assert.equal(n, 323);
            f.v = 1000;
            f.join();
            assert.equal(n, 1300);
        });

        it('current', () => {
            function t_fiber2(v1, v2) {
                n = v1 + v2 + coroutine.current().v;
            }

            n = 1300;

            var f = coroutine.start(t_fiber2, 100, 200);
            assert.equal(n, 1300);
            f.v = 2000;
            f.join();
            assert.equal(n, 2300);

            f = coroutine.start(t_fiber2, 100, 200);
            assert.equal(n, 2300);
            f.v = 1000;
            coroutine.sleep(10);
            f.join();
            assert.equal(n, 1300);
        });

        it('do not recycle fiber objects during gc', () => {
            coroutine.current().v = 100;
            gc();
            assert.equal(coroutine.current().v, 100);
        });

        it("new fiber stack_usage error", () => {
            coroutine.start(() => { }).stack_usage;
        });
    });

    it('parallel', () => {
        var funs = [
            () => {
                coroutine.sleep(10);
                return 1;
            },
            () => {
                coroutine.sleep(10);
                return 2;
            },
            () => {
                coroutine.sleep(10);
                return 3;
            },
            () => {
                coroutine.sleep(10);
            },
            () => {
                coroutine.sleep(10);
            }
        ];

        var rs = coroutine
            .parallel(funs[0], funs[1], funs[2], funs[3], funs[4]);
        assert.equal(rs[0], 1);
        assert.equal(rs[1], 2);
        assert.equal(rs[2], 3);

        rs = coroutine.parallel(funs);
        assert.equal(rs[0], 1);
        assert.equal(rs[1], 2);
        assert.equal(rs[2], 3);

        assert.throws(() => {
            coroutine.parallel(funs[0], funs[1], funs[2], funs[3], funs[4],
                () => {
                    console.log(notExistsValue);
                });
        });

        assert.throws(() => {
            coroutine.parallel(() => {
                console.log(notExistsValue);
            }, funs[0], funs[1], funs[2], funs[3], funs[4]);
        });

        assert.deepEqual(coroutine.parallel([], (v) => {
            return 1;
        }), []);

        assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], (v) => {
            return v + 1;
        }), [2, 3, 4, 5, 6]);

        assert.deepEqual(coroutine.parallel((v) => {
            return v + 1;
        }, 5), [1, 2, 3, 4, 5]);
    });

    it('parallel fibers limit', () => {
        var num = 0;
        var funs = [
            () => {
                var v = num++;
                coroutine.sleep(10);
                num--;
                return v;
            },
            () => {
                var v = num++;
                coroutine.sleep(10);
                num--;
                return v;
            },
            () => {
                var v = num++;
                coroutine.sleep(10);
                num--;
                return v;
            },
            () => {
                var v = num++;
                coroutine.sleep(10);
                num--;
                return v;
            },
            () => {
                var v = num++;
                coroutine.sleep(10);
                num--;
                return v;
            },
            () => {
                var v = num++;
                coroutine.sleep(10);
                num--;
                return v;
            }
        ];

        var rs = coroutine.parallel(funs);
        assert.deepEqual(rs, [0, 1, 2, 3, 4, 5]);

        var rs = coroutine.parallel(funs, 2);
        assert.deepEqual(rs, [0, 1, 1, 1, 1, 1]);

        var rs = coroutine.parallel(funs, 3);
        assert.deepEqual(rs, [0, 1, 2, 2, 2, 2]);

        var rs = coroutine.parallel(funs, 4);
        assert.deepEqual(rs, [0, 1, 2, 3, 3, 3]);

        var num = 0;
        assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], (v) => {
            var n = num++;
            coroutine.sleep(10);
            num--;
            return v + n;
        }), [1, 3, 5, 7, 9]);

        var num = 0;
        assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], (v) => {
            var n = num++;
            coroutine.sleep(10);
            num--;
            return v + n;
        }, 2), [1, 3, 4, 5, 6]);

        var num = 0;
        assert.deepEqual(coroutine.parallel([1, 2, 3, 4, 5], (v) => {
            var n = num++;
            coroutine.sleep(10);
            num--;
            return v + n;
        }, 3), [1, 3, 5, 6, 7]);
    });

    it('stack overflow', () => {
        function stack_size() {
            function t() {
                t();
            }

            assert.throws(() => {
                t();
            });
        }
        coroutine.start(stack_size);
        coroutine.sleep();
    });
});
