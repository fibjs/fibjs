var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

describe('MessageChannel', () => {
    it('should be available globally', () => {
        assert.equal(typeof MessageChannel, 'function');
        assert.equal(typeof MessagePort, 'function');
    });

    it('should create a channel with two ports', () => {
        var mc = new MessageChannel();
        assert.ok(mc.port1 instanceof MessagePort);
        assert.ok(mc.port2 instanceof MessagePort);
        assert.notStrictEqual(mc.port1, mc.port2);
        mc.port1.close();
        mc.port2.close();
    });

    describe('postMessage / onmessage', () => {
        it('should send message from port1 to port2', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                mc.port2.onmessage = (ev) => {
                    try {
                        assert.deepStrictEqual(ev.data, { hello: 'world' });
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                };
                mc.port1.postMessage({ hello: 'world' });
            });
        });

        it('should send message from port2 to port1', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                mc.port1.onmessage = (ev) => {
                    try {
                        assert.deepStrictEqual(ev.data, 42);
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                };
                mc.port2.postMessage(42);
            });
        });

        it('should support bidirectional messaging', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var received = [];

                mc.port1.onmessage = (ev) => {
                    received.push('port1:' + ev.data);
                    if (received.length === 2) check();
                };

                mc.port2.onmessage = (ev) => {
                    received.push('port2:' + ev.data);
                    if (received.length === 2) check();
                };

                function check() {
                    try {
                        assert.ok(received.includes('port1:from2'));
                        assert.ok(received.includes('port2:from1'));
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                }

                mc.port1.postMessage('from1');
                mc.port2.postMessage('from2');
            });
        });

        it('should send various data types', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var values = [
                    null,
                    true,
                    123,
                    'string',
                    [1, 2, 3],
                    { nested: { key: 'value' } }
                ];
                var received = [];

                mc.port2.onmessage = (ev) => {
                    received.push(ev.data);
                    if (received.length === values.length) {
                        try {
                            assert.strictEqual(received[0], null);
                            assert.strictEqual(received[1], true);
                            assert.strictEqual(received[2], 123);
                            assert.strictEqual(received[3], 'string');
                            assert.deepStrictEqual(received[4], [1, 2, 3]);
                            assert.deepStrictEqual(received[5], { nested: { key: 'value' } });
                            mc.port1.close();
                            mc.port2.close();
                            resolve();
                        } catch (e) {
                            reject(e);
                        }
                    }
                };

                values.forEach(v => mc.port1.postMessage(v));
            });
        });

        it('should deliver messages in order', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var received = [];
                var count = 5;

                mc.port2.onmessage = (ev) => {
                    received.push(ev.data);
                    if (received.length === count) {
                        try {
                            assert.deepStrictEqual(received, [0, 1, 2, 3, 4]);
                            mc.port1.close();
                            mc.port2.close();
                            resolve();
                        } catch (e) {
                            reject(e);
                        }
                    }
                };

                for (var i = 0; i < count; i++) {
                    mc.port1.postMessage(i);
                }
            });
        });
    });

    describe('addEventListener / message event', () => {
        it('should work with addEventListener', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                mc.port2.addEventListener('message', (ev) => {
                    try {
                        assert.strictEqual(ev.data, 'hello');
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                });
                mc.port2.start();
                mc.port1.postMessage('hello');
            });
        });

        it('should support start() method', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var received = false;

                mc.port2.addEventListener('message', (ev) => {
                    received = true;
                    try {
                        assert.strictEqual(ev.data, 'test');
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                });

                mc.port2.start();
                mc.port1.postMessage('test');
            });
        });

        it('should auto-start when onmessage is set', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                mc.port1.postMessage('auto');

                // Setting onmessage should auto-start
                mc.port2.onmessage = (ev) => {
                    try {
                        assert.strictEqual(ev.data, 'auto');
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                };
            });
        });

        it('should support multiple addEventListener handlers', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var results = [];

                mc.port2.addEventListener('message', (ev) => {
                    results.push('handler1:' + ev.data);
                });
                mc.port2.addEventListener('message', (ev) => {
                    results.push('handler2:' + ev.data);
                    try {
                        assert.deepStrictEqual(results, ['handler1:test', 'handler2:test']);
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                });

                mc.port2.start();
                mc.port1.postMessage('test');
            });
        });

        it('should support removeEventListener', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var results = [];

                function handler1(ev) {
                    results.push('handler1');
                }
                function handler2(ev) {
                    results.push('handler2');
                    try {
                        assert.deepStrictEqual(results, ['handler2']);
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                }

                mc.port2.addEventListener('message', handler1);
                mc.port2.addEventListener('message', handler2);
                mc.port2.removeEventListener('message', handler1);
                mc.port2.start();
                mc.port1.postMessage('test');
            });
        });
    });

    describe('close', () => {
        it('should stop receiving messages after close', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var received = [];

                mc.port2.onmessage = (ev) => {
                    received.push(ev.data);
                };

                mc.port1.postMessage('before');

                setTimeout(() => {
                    mc.port2.close();
                    mc.port1.postMessage('after');

                    setTimeout(() => {
                        try {
                            assert.deepStrictEqual(received, ['before']);
                            mc.port1.close();
                            resolve();
                        } catch (e) {
                            reject(e);
                        }
                    }, 50);
                }, 50);
            });
        });

        it('should fire close event', () => {
            return new Promise((resolve) => {
                var mc = new MessageChannel();
                mc.port1.addEventListener('close', () => {
                    resolve();
                });
                mc.port1.close();
            });
        });
    });

    describe('messageerror event', () => {
        it('should have onmessageerror property', () => {
            var mc = new MessageChannel();
            assert.strictEqual(mc.port1.onmessageerror, null);
            mc.port1.close();
            mc.port2.close();
        });
    });

    describe('postMessage data passing', () => {
        it('should clone objects in same isolate', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var obj = { a: 1, b: [2, 3] };

                mc.port2.onmessage = (ev) => {
                    try {
                        assert.deepStrictEqual(ev.data, { a: 1, b: [2, 3] });
                        assert.notStrictEqual(ev.data, obj);
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                };

                mc.port1.postMessage(obj);
                obj.a = 9;
                obj.b.push(4);
            });
        });

        it('should transfer ArrayBuffer', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                var buf = new ArrayBuffer(8);
                var view = new Uint8Array(buf);
                view[0] = 1;
                view[1] = 2;

                mc.port2.onmessage = (ev) => {
                    try {
                        // Original buffer should be detached after transfer
                        assert.strictEqual(buf.byteLength, 0);
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                };

                mc.port1.postMessage(buf, [buf]);
            });
        });
    });

    describe('MessageEvent', () => {
        it('should have correct event properties', () => {
            return new Promise((resolve, reject) => {
                var mc = new MessageChannel();
                mc.port2.onmessage = (ev) => {
                    try {
                        assert.strictEqual(ev.data, 'test');
                        assert.ok(ev instanceof MessageEvent);
                        mc.port1.close();
                        mc.port2.close();
                        resolve();
                    } catch (e) {
                        reject(e);
                    }
                };
                mc.port1.postMessage('test');
            });
        });
    });

    describe('port ref/unref', () => {
        it('should have ref and unref methods', () => {
            var mc = new MessageChannel();
            assert.equal(typeof mc.port1.ref, 'function');
            assert.equal(typeof mc.port1.unref, 'function');
            // Should not throw
            mc.port1.ref();
            mc.port1.unref();
            mc.port1.close();
            mc.port2.close();
        });
    });
});
