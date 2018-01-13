var test = require("test");
test.setup();

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
            GC();
            var no1 = test_util.countObject('Buffer');
            var f = coroutine.start((v) => {}, new Buffer());
            GC();
            assert.equal(no1 + 1, test_util.countObject('Buffer'));
            f.join();

            GC();
            assert.equal(no1, test_util.countObject('Buffer'));

            f = undefined;
            GC();
            assert.equal(no1, test_util.countObject('Buffer'));
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

        it('caller', () => {
            function t_fiber3(v1, v2) {
                n = v1 + v2 + this.caller.v;
            }

            n = 1300;

            var f = coroutine.start(t_fiber3, 100, 200);
            assert.equal(n, 1300);
            coroutine.current().v = 1234;
            f.join();
            assert.equal(n, 1534);
        });

        it('inherit local storage', () => {
            function t_fiber4() {
                n = coroutine.current().v;
            }

            n = 0;

            coroutine.current().v = 1000;

            var f = coroutine.start(t_fiber4);
            assert.equal(n, 0);

            coroutine.current().v = 2000;

            f.join();
            assert.equal(n, 1000);
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

    describe('Worker', () => {
        var worker;

        it("new", () => {
            worker = new coroutine.Worker(path.join(__dirname, 'worker_main.js'));
        });

        describe("message", () => {
            var msg_trans = util.sync((msg, done) => {
                worker.onmessage = (evt) => {
                    done(null, evt.data);
                };
                worker.postMessage(msg);
            });

            it('value', () => {
                assert.strictEqual(msg_trans(123), 123);

                var d = new Date();
                assert.equal(msg_trans(d).getTime(), d.getTime());

                assert.strictEqual(msg_trans('abcded阿斯蒂芬'), 'abcded阿斯蒂芬');
            });

            it('array', () => {
                var a = [1, 2, 3, 4];
                assert.deepEqual(msg_trans(a), a);

                var a = [1, 2, [3, 4, 5, 6], 4];
                assert.deepEqual(msg_trans(a), a);
            });

            it('object', () => {
                var o = {
                    a: 1,
                    b: 2,
                    c: 3
                };
                assert.deepEqual(msg_trans(o), o);

                var o = {
                    a: 1,
                    b: {
                        d: 4,
                        e: 5
                    },
                    c: 3
                };
                assert.deepEqual(msg_trans(o), o);

                var o = {
                    a: 1,
                    b: [
                        4,
                        5
                    ],
                    c: 3
                };
                assert.deepEqual(msg_trans(o), o);
            });

            describe('native object', () => {
                it('default', () => {
                    assert.throws(() => {
                        msg_trans(worker);
                    });
                });

                it('Int64', () => {
                    var v = new Int64(1000);
                    var v1 = msg_trans(v);
                    assert.notEqual(v, v1);
                    assert.deepEqual(v, v1);
                });

                it('Buffer', () => {
                    var v = new Buffer("1234567890");
                    var v1 = msg_trans(v);
                    assert.notEqual(v, v1);
                    assert.ok(Buffer.isBuffer(v1));
                    assert.deepEqual(v, v1);
                });

                it('Socket', () => {
                    var v = new net.Socket(net.AF_INET, net.SOCK_STREAM);
                    v.bind(8899 + base_port);

                    var v1 = msg_trans(v);
                    assert.throws(() => {
                        v.localPort;
                    });
                    assert.equal(v1.localPort, 8899 + base_port);
                    v1.close();
                });
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);