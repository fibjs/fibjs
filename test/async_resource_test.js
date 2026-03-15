const { describe, it } = require('node:test');
const assert = require('assert');
const { AsyncResource, AsyncLocalStorage } = require('async_hooks');

describe('AsyncResource', () => {
    describe('Constructor', () => {
        it('should create an instance with type string', () => {
            const ar = new AsyncResource('TEST');
            assert.ok(ar instanceof AsyncResource);
        });

        it('should accept options object as second argument', () => {
            const ar = new AsyncResource('TEST', { requireManualDestroy: true });
            assert.ok(ar instanceof AsyncResource);
        });

        it('should accept triggerAsyncId in options', () => {
            const ar = new AsyncResource('TEST', { triggerAsyncId: 0 });
            assert.ok(ar instanceof AsyncResource);
        });

        it('should accept numeric second argument as triggerAsyncId', () => {
            const ar = new AsyncResource('TEST', 123);
            assert.ok(ar instanceof AsyncResource);
        });

        it('should throw if type is not provided', () => {
            assert.throws(() => new AsyncResource());
        });

        // In Node.js, non-string type throws TypeError.
        // In fibjs, numeric type is auto-converted to string by IDL binding.
        if (typeof process.versions.fibjs === 'undefined') {
            it('should throw if type is not a string', () => {
                assert.throws(() => new AsyncResource(123), {
                    name: 'TypeError'
                });
            });
        }
    });

    describe('asyncId and triggerAsyncId', () => {
        it('should return numeric asyncId', () => {
            const ar = new AsyncResource('TEST');
            assert.strictEqual(typeof ar.asyncId(), 'number');
        });

        it('should return unique asyncIds', () => {
            const ar1 = new AsyncResource('TEST1');
            const ar2 = new AsyncResource('TEST2');
            assert.notStrictEqual(ar1.asyncId(), ar2.asyncId());
        });

        it('should return numeric triggerAsyncId', () => {
            const ar = new AsyncResource('TEST');
            assert.strictEqual(typeof ar.triggerAsyncId(), 'number');
        });

        it('should return positive asyncId', () => {
            const ar = new AsyncResource('TEST');
            assert.ok(ar.asyncId() > 0);
        });
    });

    describe('runInAsyncScope', () => {
        it('should execute callback and return its result', () => {
            const ar = new AsyncResource('TEST');
            const result = ar.runInAsyncScope(() => 42);
            assert.strictEqual(result, 42);
        });

        it('should pass arguments to callback', () => {
            const ar = new AsyncResource('TEST');
            const result = ar.runInAsyncScope((a, b) => a + b, null, 3, 4);
            assert.strictEqual(result, 7);
        });

        it('should use thisArg for callback', () => {
            const ar = new AsyncResource('TEST');
            const obj = { value: 99 };
            const result = ar.runInAsyncScope(function () {
                return this.value;
            }, obj);
            assert.strictEqual(result, 99);
        });

        it('should propagate exceptions', () => {
            const ar = new AsyncResource('TEST');
            assert.throws(() => {
                ar.runInAsyncScope(() => {
                    throw new Error('test error');
                });
            }, { message: 'test error' });
        });

        it('should work with async local storage', () => {
            const als = new AsyncLocalStorage();
            const ar = als.run('test-store', () => {
                return new AsyncResource('TEST');
            });

            // Outside the run context, store should be undefined
            assert.strictEqual(als.getStore(), undefined);

            // runInAsyncScope should restore the context from creation time
            ar.runInAsyncScope(() => {
                assert.strictEqual(als.getStore(), 'test-store');
            });
        });

        it('should support nested runInAsyncScope calls', () => {
            const als = new AsyncLocalStorage();
            const ar1 = als.run('ctx1', () => new AsyncResource('TEST1'));
            const ar2 = als.run('ctx2', () => new AsyncResource('TEST2'));

            ar1.runInAsyncScope(() => {
                assert.strictEqual(als.getStore(), 'ctx1');
                ar2.runInAsyncScope(() => {
                    assert.strictEqual(als.getStore(), 'ctx2');
                });
                // After nested scope, original context should be restored
                assert.strictEqual(als.getStore(), 'ctx1');
            });
        });

        it('should work with multiple AsyncLocalStorage instances', () => {
            const als1 = new AsyncLocalStorage();
            const als2 = new AsyncLocalStorage();

            const ar = als1.run('store1', () => {
                return als2.run('store2', () => {
                    return new AsyncResource('TEST');
                });
            });

            ar.runInAsyncScope(() => {
                assert.strictEqual(als1.getStore(), 'store1');
                assert.strictEqual(als2.getStore(), 'store2');
            });
        });
    });

    describe('emitDestroy', () => {
        it('should return the AsyncResource instance', () => {
            const ar = new AsyncResource('TEST');
            const result = ar.emitDestroy();
            assert.strictEqual(result, ar);
        });

        it('should be callable multiple times without error', () => {
            const ar = new AsyncResource('TEST');
            ar.emitDestroy();
            ar.emitDestroy();
        });
    });

    describe('bind', () => {
        it('should return a function', () => {
            const ar = new AsyncResource('TEST');
            const fn = ar.bind(() => { });
            assert.strictEqual(typeof fn, 'function');
        });

        it('should execute the original function', () => {
            const ar = new AsyncResource('TEST');
            const fn = ar.bind((x) => x * 2);
            assert.strictEqual(fn(5), 10);
        });

        it('should preserve async context from creation time', () => {
            const als = new AsyncLocalStorage();
            let fn;

            als.run('bound-context', () => {
                const ar = new AsyncResource('TEST');
                fn = ar.bind(() => als.getStore());
            });

            // Called outside the run context
            assert.strictEqual(fn(), 'bound-context');
        });

        it('should use provided thisArg', () => {
            const ar = new AsyncResource('TEST');
            const obj = { val: 42 };
            const fn = ar.bind(function () { return this.val; }, obj);
            assert.strictEqual(fn(), 42);
        });

        it('should pass arguments through', () => {
            const ar = new AsyncResource('TEST');
            const fn = ar.bind((a, b, c) => a + b + c);
            assert.strictEqual(fn(1, 2, 3), 6);
        });
    });

    describe('static bind', () => {
        it('should return a function', () => {
            const fn = AsyncResource.bind(() => 42);
            assert.strictEqual(typeof fn, 'function');
        });

        it('should execute the original function', () => {
            const fn = AsyncResource.bind(() => 42);
            assert.strictEqual(fn(), 42);
        });

        it('should preserve async context from binding time', () => {
            const als = new AsyncLocalStorage();
            let fn;

            als.run('static-bound-ctx', () => {
                fn = AsyncResource.bind(() => als.getStore());
            });

            assert.strictEqual(fn(), 'static-bound-ctx');
        });

        it('should preserve context even when called in different context', async () => {
            const als = new AsyncLocalStorage();
            let fn;

            als.run('original', () => {
                fn = AsyncResource.bind(() => als.getStore());
            });

            const result = await new Promise(resolve => {
                als.run('different', () => {
                    resolve(fn());
                });
            });

            assert.strictEqual(result, 'original');
        });

        it('should accept type parameter', () => {
            const fn = AsyncResource.bind(() => 42, 'CustomType');
            assert.strictEqual(fn(), 42);
        });

        it('should accept thisArg parameter', () => {
            const obj = { val: 10 };
            const fn = AsyncResource.bind(function () { return this.val; }, 'Custom', obj);
            assert.strictEqual(fn(), 10);
        });
    });

    describe('Subclass / extends', () => {
        it('should support extending AsyncResource', () => {
            class MyResource extends AsyncResource {
                constructor() {
                    super('MyResource');
                    this.data = 42;
                }
            }
            const my = new MyResource();
            assert.ok(my instanceof AsyncResource);
            assert.ok(my instanceof MyResource);
            assert.strictEqual(my.data, 42);
        });

        it('should allow calling runInAsyncScope from subclass', () => {
            class MyResource extends AsyncResource {
                constructor() { super('MyResource'); }
                doWork(fn) { return this.runInAsyncScope(fn, this); }
            }

            const my = new MyResource();
            const result = my.doWork(() => 100);
            assert.strictEqual(result, 100);
        });

        it('should preserve async context in subclass', () => {
            const als = new AsyncLocalStorage();

            class MyResource extends AsyncResource {
                constructor() { super('MyResource'); }
                run(fn) { return this.runInAsyncScope(fn, this); }
            }

            const resource = als.run('subclass-ctx', () => new MyResource());

            resource.run(() => {
                assert.strictEqual(als.getStore(), 'subclass-ctx');
            });
        });

        it('should work with subclass used like undici pattern', () => {
            const als = new AsyncLocalStorage();

            class RequestHandler extends AsyncResource {
                constructor(opts, callback) {
                    super('RequestHandler');
                    this.callback = callback;
                    this.opaque = opts.opaque;
                }

                onComplete(result) {
                    this.runInAsyncScope(this.callback, null, null, {
                        result,
                        opaque: this.opaque
                    });
                }

                onError(err) {
                    this.runInAsyncScope(this.callback, null, err, {
                        opaque: this.opaque
                    });
                }
            }

            let capturedStore;
            const handler = als.run('request-ctx', () => {
                return new RequestHandler({ opaque: 'data' }, (err, data) => {
                    capturedStore = als.getStore();
                    assert.strictEqual(err, null);
                    assert.strictEqual(data.result, 'ok');
                    assert.strictEqual(data.opaque, 'data');
                });
            });

            // Simulate callback from outside the context
            handler.onComplete('ok');
            assert.strictEqual(capturedStore, 'request-ctx');
        });
    });

    describe('async context propagation', () => {
        it('should propagate context through setTimeout', async () => {
            const als = new AsyncLocalStorage();

            const result = await new Promise(resolve => {
                als.run('timeout-ctx', () => {
                    const ar = new AsyncResource('TEST');
                    setTimeout(() => {
                        ar.runInAsyncScope(() => {
                            resolve(als.getStore());
                        });
                    }, 10);
                });
            });

            assert.strictEqual(result, 'timeout-ctx');
        });

        it('should propagate context through Promise', async () => {
            const als = new AsyncLocalStorage();

            const ar = als.run('promise-ctx', () => new AsyncResource('TEST'));

            const result = await Promise.resolve().then(() => {
                let val;
                ar.runInAsyncScope(() => {
                    val = als.getStore();
                });
                return val;
            });

            assert.strictEqual(result, 'promise-ctx');
        });
    });
});
