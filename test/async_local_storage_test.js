const { describe, it } = require('node:test');
const assert = require('assert');
const { AsyncLocalStorage } = require('async_hooks');

describe('AsyncLocalStorage', () => {
    describe('Core API', () => {
        it('should create an instance', () => {
            const als = new AsyncLocalStorage();
            assert.ok(als instanceof AsyncLocalStorage);
        });

        it('should create an instance with options', () => {
            const als = new AsyncLocalStorage({ name: 'test-storage' });
            assert.ok(als instanceof AsyncLocalStorage);
        });

        it('should return undefined when no store is set', () => {
            const als = new AsyncLocalStorage();
            assert.strictEqual(als.getStore(), undefined);
        });

        it('should expose run method', () => {
            const als = new AsyncLocalStorage();
            assert.strictEqual(typeof als.run, 'function');
        });

        it('should expose exit method', () => {
            const als = new AsyncLocalStorage();
            assert.strictEqual(typeof als.exit, 'function');
        });

        it('should expose enterWith method', () => {
            const als = new AsyncLocalStorage();
            assert.strictEqual(typeof als.enterWith, 'function');
        });

        it('should expose disable method', () => {
            const als = new AsyncLocalStorage();
            assert.strictEqual(typeof als.disable, 'function');
        });

        it('should expose static bind method', () => {
            assert.strictEqual(typeof AsyncLocalStorage.bind, 'function');
        });

        it('should expose static snapshot method', () => {
            assert.strictEqual(typeof AsyncLocalStorage.snapshot, 'function');
        });
    });

    describe('constructor options', () => {
        describe('name option', () => {
            it('should have empty string as default name', () => {
                const als = new AsyncLocalStorage();
                assert.strictEqual(als.name, '');
            });

            it('should set name from options', () => {
                const als = new AsyncLocalStorage({ name: 'request-context' });
                assert.strictEqual(als.name, 'request-context');
            });

            it('should convert name to string', () => {
                const als = new AsyncLocalStorage({ name: 123 });
                assert.strictEqual(als.name, '123');
            });

            it('should not conflict with same name instances', () => {
                const als1 = new AsyncLocalStorage({ name: 'same-name' });
                const als2 = new AsyncLocalStorage({ name: 'same-name' });

                assert.strictEqual(als1.name, als2.name);
                assert.notStrictEqual(als1, als2);

                als1.run('store-1', () => {
                    als2.run('store-2', () => {
                        assert.strictEqual(als1.getStore(), 'store-1');
                        assert.strictEqual(als2.getStore(), 'store-2');
                    });
                });
            });

            it('should propagate name through async operations', async () => {
                const als = new AsyncLocalStorage({ name: 'async-name-test' });
                let name, store;

                await new Promise(resolve => {
                    als.run({ value: 42 }, () => {
                        setTimeout(() => {
                            name = als.name;
                            store = als.getStore();
                            resolve();
                        }, 10);
                    });
                });

                assert.strictEqual(name, 'async-name-test');
                assert.strictEqual(store.value, 42);
            });
        });

        describe('defaultValue option', () => {
            it('should return undefined when no defaultValue is set', () => {
                const als = new AsyncLocalStorage();
                assert.strictEqual(als.getStore(), undefined);
            });

            it('should return defaultValue when no store is set', () => {
                const defaultStore = { default: true };
                const als = new AsyncLocalStorage({ defaultValue: defaultStore });
                assert.strictEqual(als.getStore(), defaultStore);
            });

            it('should return run store instead of defaultValue inside run', () => {
                const defaultStore = { default: true };
                const runStore = { custom: 'value' };
                const als = new AsyncLocalStorage({ defaultValue: defaultStore });

                als.run(runStore, () => {
                    assert.strictEqual(als.getStore(), runStore);
                });
            });

            it('should return defaultValue after run completes', () => {
                const defaultStore = { default: true };
                const als = new AsyncLocalStorage({ defaultValue: defaultStore });

                als.run({ custom: 'value' }, () => { });

                assert.strictEqual(als.getStore(), defaultStore);
            });

            it('should support primitive defaultValue', () => {
                const als = new AsyncLocalStorage({ defaultValue: 'default-string' });
                assert.strictEqual(als.getStore(), 'default-string');
            });

            it('should support null as defaultValue', () => {
                const als = new AsyncLocalStorage({ defaultValue: null });
                assert.strictEqual(als.getStore(), undefined);
            });

            it('should support Symbol as defaultValue', () => {
                const sym = Symbol('default-symbol');
                const als = new AsyncLocalStorage({ defaultValue: sym });
                assert.strictEqual(als.getStore(), sym);
            });

            it('should propagate defaultValue through async operations when no store set', async () => {
                const defaultStore = { default: true };
                const als = new AsyncLocalStorage({ defaultValue: defaultStore });
                let store;

                await new Promise(resolve => {
                    setTimeout(() => {
                        store = als.getStore();
                        resolve();
                    }, 10);
                });

                assert.strictEqual(store, defaultStore);
            });

            it('should return undefined after exit from run', () => {
                const defaultStore = { default: true };
                const als = new AsyncLocalStorage({ defaultValue: defaultStore });

                als.run({ custom: 'value' }, () => {
                    als.exit(() => {
                        // exit() temporarily exits the context, so getStore() returns undefined
                        assert.strictEqual(als.getStore(), undefined);
                    });
                });
            });

            it('should return defaultValue after disable', () => {
                const defaultStore = { default: true };
                const als = new AsyncLocalStorage({ defaultValue: defaultStore });

                als.run({}, () => {
                    als.disable();
                    // disable() clears the context but defaultValue is still returned
                    assert.strictEqual(als.getStore(), defaultStore);
                });
            });
        });

        describe('combined name and defaultValue options', () => {
            it('should support both name and defaultValue', () => {
                const defaultStore = { default: true };
                const als = new AsyncLocalStorage({
                    name: 'combined-test',
                    defaultValue: defaultStore
                });

                assert.strictEqual(als.name, 'combined-test');
                assert.strictEqual(als.getStore(), defaultStore);
            });

            it('should work correctly with multiple instances having different options', async () => {
                const als1 = new AsyncLocalStorage({
                    name: 'storage-1',
                    defaultValue: { id: 'default-1' }
                });
                const als2 = new AsyncLocalStorage({
                    name: 'storage-2',
                    defaultValue: { id: 'default-2' }
                });

                assert.strictEqual(als1.getStore().id, 'default-1');
                assert.strictEqual(als2.getStore().id, 'default-2');

                let als1Store, als2Store;
                await new Promise(resolve => {
                    als1.run({ id: 'run-1' }, () => {
                        setTimeout(() => {
                            als1Store = als1.getStore();
                            als2Store = als2.getStore();
                            resolve();
                        }, 10);
                    });
                });

                assert.strictEqual(als1Store.id, 'run-1');
                assert.strictEqual(als2Store.id, 'default-2');
                assert.strictEqual(als1.getStore().id, 'default-1');
                assert.strictEqual(als2.getStore().id, 'default-2');
            });
        });
    });

    describe('run', () => {
        it('should set store within callback', () => {
            const als = new AsyncLocalStorage();
            const store = { key: 'value' };

            als.run(store, () => {
                assert.strictEqual(als.getStore(), store);
            });
        });

        it('should return callback result', () => {
            const als = new AsyncLocalStorage();
            const result = als.run({}, () => 42);
            assert.strictEqual(result, 42);
        });

        it('should pass arguments to callback', () => {
            const als = new AsyncLocalStorage();
            als.run({}, (arg) => {
                assert.strictEqual(arg, 'foo');
            }, 'foo');
        });

        it('should restore store after callback', () => {
            const als = new AsyncLocalStorage();
            als.run({}, () => { });
            assert.strictEqual(als.getStore(), undefined);
        });

        it('should support nested runs', () => {
            const als = new AsyncLocalStorage();
            const outer = { id: 'outer' };
            const inner = { id: 'inner' };

            als.run(outer, () => {
                assert.strictEqual(als.getStore(), outer);

                als.run(inner, () => {
                    assert.strictEqual(als.getStore(), inner);
                });

                assert.strictEqual(als.getStore(), outer);
            });

            assert.strictEqual(als.getStore(), undefined);
        });

        it('should support string as store', () => {
            const als = new AsyncLocalStorage();
            als.run('hello node', () => {
                assert.strictEqual(als.getStore(), 'hello node');
            });
        });

        it('should support object as store', () => {
            const als = new AsyncLocalStorage();
            const runStore = { hello: 'node' };
            als.run(runStore, () => {
                assert.strictEqual(als.getStore(), runStore);
            });
        });

        it('should support Map as store', () => {
            const als = new AsyncLocalStorage();
            const store = new Map();
            store.set('key', 'value');

            als.run(store, () => {
                assert.strictEqual(als.getStore().get('key'), 'value');
            });
        });
    });

    describe('exit', () => {
        it('should exit from current store context', () => {
            const als = new AsyncLocalStorage();
            const store = { key: 'value' };

            als.run(store, () => {
                assert.strictEqual(als.getStore(), store);

                als.exit(() => {
                    assert.strictEqual(als.getStore(), undefined);
                });

                assert.strictEqual(als.getStore(), store);
            });
        });

        it('should pass arguments to callback', () => {
            const als = new AsyncLocalStorage();

            als.run({}, () => {
                als.exit((arg) => {
                    assert.strictEqual(arg, 'bar');
                }, 'bar');
            });
        });

        it('should return callback result', () => {
            const als = new AsyncLocalStorage();

            const result = als.run({}, () => {
                return als.exit(() => 123);
            });

            assert.strictEqual(result, 123);
        });
    });

    describe('enterWith', () => {
        it('should set store for current execution context', async () => {
            const als = new AsyncLocalStorage();
            const store = { foo: 'bar' };
            let storeAfterEnter, storeInTimeout;

            await new Promise(resolve => {
                setImmediate(() => {
                    als.enterWith(store);
                    storeAfterEnter = als.getStore();

                    setTimeout(() => {
                        storeInTimeout = als.getStore();
                        resolve();
                    }, 10);
                });
            });

            assert.strictEqual(storeAfterEnter, store);
            assert.strictEqual(storeInTimeout, store);
        });

        it('should not affect other async contexts', async () => {
            const als = new AsyncLocalStorage();

            setImmediate(() => {
                als.enterWith({ id: 1 });
            });

            await new Promise(resolve => setTimeout(resolve, 20));
            assert.strictEqual(als.getStore(), undefined);
        });
    });

    describe('disable', () => {
        it('should disable store tracking', async () => {
            const als = new AsyncLocalStorage();
            let store;

            await new Promise(resolve => {
                als.run(new Map(), () => {
                    als.getStore().set('foo', 'bar');

                    process.nextTick(() => {
                        als.disable();
                        store = als.getStore();
                        resolve();
                    });
                });
            });

            assert.strictEqual(store, undefined);
        });

        it('should allow re-enabling with run after disable', async () => {
            const als = new AsyncLocalStorage();
            let store;

            als.run(new Map(), () => {
                als.disable();
            });

            await new Promise(resolve => {
                process.nextTick(() => {
                    als.run(new Map().set('bar', 'foo'), () => {
                        store = als.getStore();
                    });
                    resolve();
                });
            });

            assert.strictEqual(store.get('bar'), 'foo');
        });
    });

    describe('async context propagation', () => {
        it('should propagate store through setTimeout', async () => {
            const als = new AsyncLocalStorage();
            const store = new Map();
            store.set('hello', 'world');
            let resultStore;

            await new Promise(resolve => {
                als.run(store, () => {
                    setTimeout(() => {
                        resultStore = als.getStore();
                        resolve();
                    }, 10);
                });
            });

            assert.strictEqual(resultStore.get('hello'), 'world');
        });

        it('should propagate store through setImmediate', async () => {
            const als = new AsyncLocalStorage();
            const store = { value: 42 };
            let resultStore;

            await new Promise(resolve => {
                als.run(store, () => {
                    setImmediate(() => {
                        resultStore = als.getStore();
                        resolve();
                    });
                });
            });

            assert.strictEqual(resultStore.value, 42);
        });

        it('should propagate store through process.nextTick', async () => {
            const als = new AsyncLocalStorage();
            const store = new Map();
            store.set('foo', 'bar');
            let resultStore;

            await new Promise(resolve => {
                als.run(store, () => {
                    process.nextTick(() => {
                        resultStore = als.getStore();
                        resolve();
                    });
                });
            });

            assert.strictEqual(resultStore.get('foo'), 'bar');
        });

        it('should propagate store through Promise.then', async () => {
            const als = new AsyncLocalStorage();
            const store = new Map();
            store.set('a', 1);
            let resultStore;

            await new Promise(resolve => {
                als.run(store, () => {
                    Promise.resolve()
                        .then(() => {
                            resultStore = als.getStore();
                            resolve();
                        });
                });
            });

            assert.strictEqual(resultStore.get('a'), 1);
        });

        it('should propagate store through async/await', async () => {
            const als = new AsyncLocalStorage();

            async function test() {
                als.getStore().set('foo', 'bar');
                await Promise.resolve();
                assert.strictEqual(als.getStore().get('foo'), 'bar');
            }

            await als.run(new Map(), test);
            assert.strictEqual(als.getStore(), undefined);
        });

        it('should propagate store through nested async functions', async () => {
            const als = new AsyncLocalStorage();

            async function inner() {
                await Promise.resolve();
                assert.notStrictEqual(als.getStore(), undefined);
                assert.strictEqual(als.getStore().get('key'), 'value');
            }

            async function outer() {
                const store = als.getStore();
                store.set('key', 'value');
                await inner();
            }

            await als.run(new Map(), outer);
        });
    });

    describe('multiple AsyncLocalStorage instances', () => {
        it('should not mix contexts between instances', async () => {
            const als1 = new AsyncLocalStorage();
            const als2 = new AsyncLocalStorage();
            let store1, store2;

            await new Promise(resolve => {
                als1.run(new Map(), () => {
                    als2.run(new Map(), () => {
                        const s1 = als1.getStore();
                        const s2 = als2.getStore();
                        s1.set('hello', 'world');
                        s2.set('hello', 'foo');

                        setTimeout(() => {
                            store1 = als1.getStore();
                            store2 = als2.getStore();
                            resolve();
                        }, 10);
                    });
                });
            });

            assert.strictEqual(store1.get('hello'), 'world');
            assert.strictEqual(store2.get('hello'), 'foo');
        });

        it('should handle exit independently', () => {
            const als1 = new AsyncLocalStorage();
            const als2 = new AsyncLocalStorage();
            let als1StoreInExit, als2StoreInExit;
            let als1StoreAfterExit, als2StoreAfterExit;

            als1.run(new Map().set('a', 1), () => {
                als2.run(new Map().set('b', 2), () => {
                    als1.exit(() => {
                        als1StoreInExit = als1.getStore();
                        als2StoreInExit = als2.getStore();
                    });

                    als1StoreAfterExit = als1.getStore();
                    als2StoreAfterExit = als2.getStore();
                });
            });

            assert.strictEqual(als1StoreInExit, undefined);
            assert.strictEqual(als2StoreInExit.get('b'), 2);
            assert.strictEqual(als1StoreAfterExit.get('a'), 1);
            assert.strictEqual(als2StoreAfterExit.get('b'), 2);
        });
    });

    describe('static bind', () => {
        it('should bind function to current async context', () => {
            const als = new AsyncLocalStorage();

            const bound = als.run({ value: 42 }, () => {
                return AsyncLocalStorage.bind(() => als.getStore());
            });

            const result = als.run({ value: 100 }, () => {
                return bound();
            });

            assert.strictEqual(result.value, 42);
        });

        it('should throw on non-function argument', () => {
            [1, false, '', {}, []].forEach((i) => {
                assert.throws(() => AsyncLocalStorage.bind(i), {
                    code: 'ERR_INVALID_ARG_TYPE'
                });
            });
        });
    });

    it('should capture current async context', () => {
        const als = new AsyncLocalStorage();

        const runInAsyncScope = als.run(123, () => AsyncLocalStorage.snapshot());

        const result = als.run(321, () => {
            return runInAsyncScope(() => {
                return als.getStore();
            });
        });

        assert.strictEqual(result, 123);
    });

    describe('edge cases', () => {
        // fibjs has stack limit, use smaller number
        it('should handle deeply nested run calls', () => {
            const als = new AsyncLocalStorage();

            function run(count) {
                if (count !== 0) return als.run({}, run, --count);
                return true;
            }

            assert.ok(run(100));
        });

        it('should not leak store after exit', () => {
            const als = new AsyncLocalStorage();
            const data = true;

            function run(count) {
                if (count === 0) return;
                assert.notStrictEqual(als.getStore(), data);
                als.run(data, () => {
                    als.exit(run, --count);
                });
            }

            run(100);
        });

        it('should maintain store context on error in Promise chain', async () => {
            const als = new AsyncLocalStorage();
            const err = new Error();

            const next = () => Promise.resolve()
                .then(() => {
                    assert.strictEqual(als.getStore().get('a'), 1);
                    throw err;
                });

            await new Promise((resolve, reject) => {
                als.run(new Map(), () => {
                    const store = als.getStore();
                    store.set('a', 1);
                    next().then(resolve, reject);
                });
            })
                .catch((e) => {
                    assert.strictEqual(als.getStore(), undefined);
                    assert.strictEqual(e, err);
                });

            assert.strictEqual(als.getStore(), undefined);
        });

        it('should work with clearImmediate', async () => {
            const als = new AsyncLocalStorage();
            let resultStore;

            await new Promise(resolve => {
                als.run(new Map(), () => {
                    const immediate = setImmediate(() => {
                        resultStore = als.getStore();
                        resolve();
                    });

                    const store = als.getStore();
                    store.set('immediate', immediate);
                });
            });

            const storedImmediate = resultStore.get('immediate');
            // fibjs uses Timer class for all timer types
            assert.ok(storedImmediate.constructor.name === 'Immediate' || storedImmediate.constructor.name === 'Timer');
            clearImmediate(storedImmediate);
        });
    });

    describe('thenable support', () => {
        it('should maintain store when awaiting a thenable', async () => {
            const als = new AsyncLocalStorage();
            const data = Symbol('verifier');
            let storeInThenable;

            function thenable() {
                return {
                    then(cb) {
                        storeInThenable = als.getStore();
                        setImmediate(cb);
                    }
                };
            }

            await als.run(data, async () => {
                assert.strictEqual(als.getStore(), data);
                await thenable();
                assert.strictEqual(als.getStore(), data);
            });

            assert.strictEqual(storeInThenable, data);
        });

        it('should maintain store when returning a thenable', async () => {
            const als = new AsyncLocalStorage();
            const data = Symbol('verifier');
            let storeInThenable;

            function thenable() {
                return {
                    then(cb) {
                        storeInThenable = als.getStore();
                        setImmediate(cb);
                    }
                };
            }

            await als.run(data, async () => {
                try {
                    assert.strictEqual(als.getStore(), data);
                    return thenable();
                } finally {
                    assert.strictEqual(als.getStore(), data);
                }
            });

            assert.strictEqual(storeInThenable, data);
        });

        it('should maintain store when resolving a thenable', async () => {
            const als = new AsyncLocalStorage();
            const data = Symbol('verifier');

            let thenCalled = false;
            let storeInThenable;
            function thenable() {
                return {
                    then(cb) {
                        thenCalled = true;
                        storeInThenable = als.getStore();
                        setImmediate(cb);
                    }
                };
            }

            als.run(data, () => {
                assert.strictEqual(als.getStore(), data);
                Promise.resolve(thenable());
                assert.strictEqual(als.getStore(), data);
            });

            await new Promise(resolve => setImmediate(resolve));
            assert.ok(thenCalled);
            assert.strictEqual(storeInThenable, data);
        });
    });

    describe('Symbol as store', () => {
        it('should support Symbol as store value', () => {
            const als = new AsyncLocalStorage();
            const sym = Symbol('test-store');

            als.run(sym, () => {
                assert.strictEqual(als.getStore(), sym);
            });
        });

        it('should propagate Symbol store through async operations', async () => {
            const als = new AsyncLocalStorage();
            const sym = Symbol('async-test');
            let storeValue;

            await new Promise(resolve => {
                als.run(sym, () => {
                    setTimeout(() => {
                        storeValue = als.getStore();
                        resolve();
                    }, 10);
                });
            });

            assert.strictEqual(storeValue, sym);
        });
    });

    // queueMicrotask propagation tests only run in Node.js
    if (typeof process.versions.fibjs === 'undefined') {
        describe('queueMicrotask propagation', () => {
            it('should propagate store through queueMicrotask', async () => {
                const als = new AsyncLocalStorage();
                const store = { value: 'microtask' };
                let storeValue;

                await new Promise(resolve => {
                    als.run(store, () => {
                        queueMicrotask(() => {
                            storeValue = als.getStore();
                            resolve();
                        });
                    });
                });

                assert.strictEqual(storeValue, store);
            });

            it('should maintain store through nested queueMicrotask', async () => {
                const als = new AsyncLocalStorage();
                const store = new Map().set('key', 'value');
                let store1, store2;

                await new Promise(resolve => {
                    als.run(store, () => {
                        queueMicrotask(() => {
                            store1 = als.getStore();
                            queueMicrotask(() => {
                                store2 = als.getStore();
                                resolve();
                            });
                        });
                    });
                });

                assert.strictEqual(store1.get('key'), 'value');
                assert.strictEqual(store2.get('key'), 'value');
            });
        });
    }

    describe('concurrent context isolation', () => {
        it('should isolate concurrent setTimeout contexts', async () => {
            const als = new AsyncLocalStorage();
            const results = [];

            await Promise.all([
                new Promise(resolve => {
                    als.run({ id: 1 }, () => {
                        setTimeout(() => {
                            results.push({ expected: 1, store: als.getStore() });
                            resolve();
                        }, 10);
                    });
                }),
                new Promise(resolve => {
                    als.run({ id: 2 }, () => {
                        setTimeout(() => {
                            results.push({ expected: 2, store: als.getStore() });
                            resolve();
                        }, 5);
                    });
                }),
                new Promise(resolve => {
                    als.run({ id: 3 }, () => {
                        setTimeout(() => {
                            results.push({ expected: 3, store: als.getStore() });
                            resolve();
                        }, 15);
                    });
                })
            ]);

            assert.strictEqual(results.length, 3);
            results.forEach(r => {
                assert.strictEqual(r.store.id, r.expected);
            });
        });

        it('should isolate concurrent Promise contexts', async () => {
            const als = new AsyncLocalStorage();

            const promises = [1, 2, 3, 4, 5].map(id => {
                return als.run({ id }, async () => {
                    await Promise.resolve();
                    assert.strictEqual(als.getStore().id, id);
                    return id;
                });
            });

            const results = await Promise.all(promises);
            assert.deepStrictEqual(results, [1, 2, 3, 4, 5]);
        });
    });

    describe('exit behavior edge cases', () => {
        it('should work when called outside of run context', () => {
            const als = new AsyncLocalStorage();

            const result = als.exit(() => {
                assert.strictEqual(als.getStore(), undefined);
                return 'success';
            });

            assert.strictEqual(result, 'success');
        });

        it('should restore context after nested exit calls', () => {
            const als = new AsyncLocalStorage();

            als.run({ level: 1 }, () => {
                als.run({ level: 2 }, () => {
                    als.exit(() => {
                        assert.strictEqual(als.getStore(), undefined);
                        als.exit(() => {
                            assert.strictEqual(als.getStore(), undefined);
                        });
                    });
                    assert.strictEqual(als.getStore().level, 2);
                });
                assert.strictEqual(als.getStore().level, 1);
            });
        });
    });

    describe('disable edge cases', () => {
        it('should disable store propagation to scheduled callbacks', async () => {
            const als = new AsyncLocalStorage();
            let store1, store2, store3, storeAfterDisable;

            await new Promise(resolve => {
                als.run(new Map().set('foo', 'bar'), () => {
                    store1 = als.getStore();

                    process.nextTick(() => {
                        store2 = als.getStore();

                        process.nextTick(() => {
                            store3 = als.getStore();
                            resolve();
                        });

                        als.disable();
                        storeAfterDisable = als.getStore();
                    });
                });
            });

            assert.strictEqual(store1.get('foo'), 'bar');
            assert.strictEqual(store2.get('foo'), 'bar');
            assert.strictEqual(store3, undefined);
            assert.strictEqual(storeAfterDisable, undefined);
        });

        it('exit should work after disable', () => {
            const als = new AsyncLocalStorage();

            als.run(new Map(), () => {
                als.disable();

                als.exit(() => {
                    assert.strictEqual(als.getStore(), undefined);
                });

                assert.strictEqual(als.getStore(), undefined);
            });
        });
    });

    describe('async callback propagation', () => {
        it('should propagate store through fs.readFile callback', async () => {
            const fs = require('fs');
            const path = require('path');
            const als = new AsyncLocalStorage();
            const store = { requestId: 'test-123' };
            let storeInCallback;

            await new Promise((resolve, reject) => {
                als.run(store, () => {
                    // Use a file that exists on all platforms
                    const testFile = path.join(__dirname, 'async_local_storage_test.js');
                    fs.readFile(testFile, (err, data) => {
                        if (err) return reject(err);
                        storeInCallback = als.getStore();
                        resolve();
                    });
                });
            });

            assert.strictEqual(storeInCallback, store);
            assert.strictEqual(storeInCallback.requestId, 'test-123');
        });

        it('should propagate store through fs.stat callback', async () => {
            const fs = require('fs');
            const path = require('path');
            const als = new AsyncLocalStorage();
            const store = { traceId: 'trace-456' };
            let storeInCallback;

            await new Promise((resolve, reject) => {
                als.run(store, () => {
                    const testFile = path.join(__dirname, 'async_local_storage_test.js');
                    fs.stat(testFile, (err, stats) => {
                        if (err) return reject(err);
                        storeInCallback = als.getStore();
                        resolve();
                    });
                });
            });

            assert.strictEqual(storeInCallback, store);
            assert.strictEqual(storeInCallback.traceId, 'trace-456');
        });

        it('should propagate store through nested async callbacks', async () => {
            const fs = require('fs');
            const path = require('path');
            const als = new AsyncLocalStorage();
            const store = { depth: 0 };
            let stores = [];

            await new Promise((resolve, reject) => {
                als.run(store, () => {
                    const testFile = path.join(__dirname, 'async_local_storage_test.js');
                    fs.stat(testFile, (err) => {
                        if (err) return reject(err);
                        stores.push(als.getStore());
                        
                        fs.readFile(testFile, (err) => {
                            if (err) return reject(err);
                            stores.push(als.getStore());
                            
                            fs.stat(testFile, (err) => {
                                if (err) return reject(err);
                                stores.push(als.getStore());
                                resolve();
                            });
                        });
                    });
                });
            });

            assert.strictEqual(stores.length, 3);
            stores.forEach(s => {
                assert.strictEqual(s, store);
            });
        });

        it('should isolate stores across concurrent async callbacks', async () => {
            const fs = require('fs');
            const path = require('path');
            const als = new AsyncLocalStorage();
            const testFile = path.join(__dirname, 'async_local_storage_test.js');

            const results = await Promise.all([
                new Promise((resolve, reject) => {
                    als.run({ id: 1 }, () => {
                        fs.readFile(testFile, (err) => {
                            if (err) return reject(err);
                            resolve({ expected: 1, actual: als.getStore() });
                        });
                    });
                }),
                new Promise((resolve, reject) => {
                    als.run({ id: 2 }, () => {
                        fs.stat(testFile, (err) => {
                            if (err) return reject(err);
                            resolve({ expected: 2, actual: als.getStore() });
                        });
                    });
                }),
                new Promise((resolve, reject) => {
                    als.run({ id: 3 }, () => {
                        fs.readFile(testFile, (err) => {
                            if (err) return reject(err);
                            resolve({ expected: 3, actual: als.getStore() });
                        });
                    });
                })
            ]);

            results.forEach(r => {
                assert.ok(r.actual, `Store should not be undefined for id ${r.expected}`);
                assert.strictEqual(r.actual.id, r.expected);
            });
        });

        // fs.createReadStream is not available in fibjs
        if (typeof process.versions.fibjs === 'undefined') {
            it('should propagate store through stream events', async () => {
                const fs = require('fs');
                const path = require('path');
                const als = new AsyncLocalStorage();
                const store = { streamTest: true };
                let storeOnData, storeOnEnd;

                await new Promise((resolve, reject) => {
                    als.run(store, () => {
                        const testFile = path.join(__dirname, 'async_local_storage_test.js');
                        const stream = fs.createReadStream(testFile);
                        
                        stream.on('data', () => {
                            if (!storeOnData) {
                                storeOnData = als.getStore();
                            }
                        });
                        
                        stream.on('end', () => {
                            storeOnEnd = als.getStore();
                            resolve();
                        });
                        
                        stream.on('error', reject);
                    });
                });

                assert.strictEqual(storeOnData, store);
                assert.strictEqual(storeOnEnd, store);
            });
        }
    });

    if (typeof process.versions.fibjs !== 'undefined') {
        describe('parallel fiber isolation', () => {
            it('should isolate stores across parallel fibers', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const results = [];

                // Pre-condition: no store before any run
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined before test");

                const fibers = [];
                for (let i = 0; i < 5; i++) {
                    fibers.push(coroutine.start(() => {
                        // Pre-condition in fiber: no store before run
                        assert.strictEqual(als.getStore(), undefined, `Fiber ${i} Pre: store should be undefined before run`);
                        
                        als.run({ fiber: i }, () => {
                            // Verify store at entry
                            assert.deepStrictEqual(als.getStore(), { fiber: i }, `Fiber ${i}: store should be {fiber: ${i}} at entry`);
                            
                            // Small delay to allow interleaving
                            coroutine.sleep(10);
                            
                            // Verify store after sleep
                            const store = als.getStore();
                            assert.deepStrictEqual(store, { fiber: i }, `Fiber ${i}: store should still be {fiber: ${i}} after sleep`);
                            results.push({ fiber: i, storeValue: store.fiber });
                        });
                        
                        // Post-condition in fiber: store should be undefined after run
                        assert.strictEqual(als.getStore(), undefined, `Fiber ${i} Post: store should be undefined after run`);
                    }));
                }

                // Wait for all fibers to complete
                fibers.forEach(fb => fb.join());

                // Post-condition: verify results
                assert.strictEqual(results.length, 5, "Should have 5 results");
                results.forEach(r => {
                    assert.strictEqual(r.fiber, r.storeValue, `Fiber ${r.fiber} got wrong store value ${r.storeValue}`);
                });
                
                // Final: store should still be undefined
                assert.strictEqual(als.getStore(), undefined, "Final: store should be undefined after all fibers");
            });

        it('should handle GC stress with parallel fibers', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                let errors = [];

                // Pre-condition: no store before test
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined before test");

                const fibers = [];
                for (let i = 0; i < 10; i++) {
                    fibers.push(coroutine.start(() => {
                        // Pre-condition in fiber
                        assert.strictEqual(als.getStore(), undefined, `Fiber ${i} Pre: store should be undefined`);
                        
                        als.run({ id: i, data: 'x'.repeat(1000) }, () => {
                            // Verify store at entry
                            const entryStore = als.getStore();
                            assert.strictEqual(entryStore.id, i, `Fiber ${i}: store.id should be ${i} at entry`);
                            
                            for (let j = 0; j < 5; j++) {
                                // Trigger GC
                                global.gc && global.gc();
                                coroutine.sleep(1);
                                
                                const store = als.getStore();
                                if (!store || store.id !== i) {
                                    errors.push(`Fiber ${i} iteration ${j}: expected id=${i}, got ${store ? store.id : 'undefined'}`);
                                }
                                // Also verify with assert
                                assert.ok(store, `Fiber ${i} iteration ${j}: store should exist`);
                                assert.strictEqual(store.id, i, `Fiber ${i} iteration ${j}: store.id should be ${i}`);
                            }
                        });
                        
                        // Post-condition in fiber
                        assert.strictEqual(als.getStore(), undefined, `Fiber ${i} Post: store should be undefined`);
                    }));
                }

                fibers.forEach(fb => fb.join());
                
                // Final verification
                assert.strictEqual(errors.length, 0, errors.join('\n'));
                assert.strictEqual(als.getStore(), undefined, "Final: store should be undefined");
            });

        it('should propagate context through setTimeout in parallel fibers', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const results = [];
                const lock = new coroutine.Lock();
                const event = new coroutine.Event();
                let completed = 0;

                // Pre-condition
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined");

                const fibers = [];
                for (let i = 0; i < 5; i++) {
                    fibers.push(coroutine.start(() => {
                        // Pre-condition in fiber
                        assert.strictEqual(als.getStore(), undefined, `Fiber ${i} Pre: store should be undefined`);
                        
                        als.run({ fiber: i }, () => {
                            // Verify store at entry
                            assert.deepStrictEqual(als.getStore(), { fiber: i }, `Fiber ${i}: store should be {fiber: ${i}} at entry`);
                            
                            setTimeout(() => {
                                const store = als.getStore();
                                // Verify store in setTimeout callback
                                assert.ok(store, `Fiber ${i} setTimeout: store should exist`);
                                assert.strictEqual(store.fiber, i, `Fiber ${i} setTimeout: store.fiber should be ${i}`);
                                
                                lock.acquire();
                                results.push({ fiber: i, storeValue: store ? store.fiber : undefined });
                                completed++;
                                if (completed === 5) event.set();
                                lock.release();
                            }, 10);
                        });
                        
                        // Post-condition in fiber (after scheduling setTimeout, before it fires)
                        assert.strictEqual(als.getStore(), undefined, `Fiber ${i} Post: store should be undefined after run`);
                    }));
                }

                // Wait for all setTimeout callbacks to complete
                event.wait();

                // Final verification
                assert.strictEqual(results.length, 5, "Should have 5 results");
                results.forEach(r => {
                    assert.strictEqual(r.fiber, r.storeValue, `Fiber ${r.fiber} got wrong store value in setTimeout`);
                });
                assert.strictEqual(als.getStore(), undefined, "Final: store should be undefined");
            });

        it('should inherit context in coroutine.start', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const store = { parentContext: true, value: 42 };
                let childStore;
                let storeBeforeRun, storeInRun, storeAfterRun;

                // Pre-condition
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined");

                als.run(store, () => {
                    // Verify store at entry
                    storeInRun = als.getStore();
                    assert.strictEqual(storeInRun, store, "Parent run: store should be set");
                    assert.strictEqual(storeInRun.parentContext, true, "Parent run: store.parentContext should be true");
                    assert.strictEqual(storeInRun.value, 42, "Parent run: store.value should be 42");
                    
                    const fiber = coroutine.start(() => {
                        // Child fiber should inherit parent's async context
                        childStore = als.getStore();
                        
                        // Verify inherited store
                        assert.strictEqual(childStore, store, "Child fiber: should inherit parent store");
                        assert.strictEqual(childStore.parentContext, true, "Child fiber: store.parentContext should be true");
                        assert.strictEqual(childStore.value, 42, "Child fiber: store.value should be 42");
                    });
                    fiber.join();
                    
                    // Verify store after child fiber completes
                    storeAfterRun = als.getStore();
                    assert.strictEqual(storeAfterRun, store, "Parent run after child: store should still be set");
                });

                // Post-condition
                assert.strictEqual(als.getStore(), undefined, "Post: store should be undefined after run");

                // Final verification
                assert.strictEqual(childStore, store, "Final: childStore should equal parent store");
                assert.strictEqual(childStore.parentContext, true);
                assert.strictEqual(childStore.value, 42);
            });

        it('should inherit context through nested coroutine.start', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const store = { level: 'root' };
                let level1Store, level2Store, level3Store;
                let storeAtEntry;

                // Pre-condition
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined");

                als.run(store, () => {
                    // Verify store at entry
                    storeAtEntry = als.getStore();
                    assert.strictEqual(storeAtEntry, store, "Root run: store should be set");
                    assert.strictEqual(storeAtEntry.level, 'root', "Root run: store.level should be 'root'");
                    
                    const fiber1 = coroutine.start(() => {
                        level1Store = als.getStore();
                        // Verify level 1 inheritance
                        assert.strictEqual(level1Store, store, "Level 1: should inherit root store");
                        assert.strictEqual(level1Store.level, 'root', "Level 1: store.level should be 'root'");
                        
                        const fiber2 = coroutine.start(() => {
                            level2Store = als.getStore();
                            // Verify level 2 inheritance
                            assert.strictEqual(level2Store, store, "Level 2: should inherit root store");
                            assert.strictEqual(level2Store.level, 'root', "Level 2: store.level should be 'root'");
                            
                            const fiber3 = coroutine.start(() => {
                                level3Store = als.getStore();
                                // Verify level 3 inheritance
                                assert.strictEqual(level3Store, store, "Level 3: should inherit root store");
                                assert.strictEqual(level3Store.level, 'root', "Level 3: store.level should be 'root'");
                            });
                            fiber3.join();
                            
                            // Verify store after level 3 fiber
                            assert.strictEqual(als.getStore(), store, "Level 2 after level 3: store should still be set");
                        });
                        fiber2.join();
                        
                        // Verify store after level 2 fiber
                        assert.strictEqual(als.getStore(), store, "Level 1 after level 2: store should still be set");
                    });
                    fiber1.join();
                    
                    // Verify store after level 1 fiber
                    assert.strictEqual(als.getStore(), store, "Root after level 1: store should still be set");
                });

                // Post-condition
                assert.strictEqual(als.getStore(), undefined, "Post: store should be undefined after run");

                // Final verification
                assert.strictEqual(level1Store, store, "Final: level1Store should equal root store");
                assert.strictEqual(level2Store, store, "Final: level2Store should equal root store");
                assert.strictEqual(level3Store, store, "Final: level3Store should equal root store");
            });

        it('should allow child fiber to override inherited context', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const parentStore = { type: 'parent' };
                const childStore = { type: 'child' };
                let storeBeforeRun, storeInRun, storeAfterRun;
                let parentStoreAtEntry, parentStoreAfterChild;

                // Pre-condition
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined");

                als.run(parentStore, () => {
                    // Verify parent store at entry
                    parentStoreAtEntry = als.getStore();
                    assert.strictEqual(parentStoreAtEntry, parentStore, "Parent run: store should be parentStore");
                    assert.strictEqual(parentStoreAtEntry.type, 'parent', "Parent run: store.type should be 'parent'");
                    
                    const fiber = coroutine.start(() => {
                        storeBeforeRun = als.getStore();
                        // Verify inherited store before child run
                        assert.strictEqual(storeBeforeRun, parentStore, "Child before run: should inherit parentStore");
                        assert.strictEqual(storeBeforeRun.type, 'parent', "Child before run: store.type should be 'parent'");
                        
                        als.run(childStore, () => {
                            storeInRun = als.getStore();
                            // Verify child store during child run
                            assert.strictEqual(storeInRun, childStore, "Child run: store should be childStore");
                            assert.strictEqual(storeInRun.type, 'child', "Child run: store.type should be 'child'");
                        });
                        
                        storeAfterRun = als.getStore();
                        // Verify store restored after child run
                        assert.strictEqual(storeAfterRun, parentStore, "Child after run: store should be parentStore again");
                        assert.strictEqual(storeAfterRun.type, 'parent', "Child after run: store.type should be 'parent'");
                    });
                    fiber.join();
                    
                    // Verify parent store after child fiber
                    parentStoreAfterChild = als.getStore();
                    assert.strictEqual(parentStoreAfterChild, parentStore, "Parent after child fiber: store should still be parentStore");
                });

                // Post-condition
                assert.strictEqual(als.getStore(), undefined, "Post: store should be undefined after run");

                // Final verification
                assert.strictEqual(storeBeforeRun, parentStore, "Final: storeBeforeRun should be parentStore");
                assert.strictEqual(storeInRun, childStore, "Final: storeInRun should be childStore");
                assert.strictEqual(storeAfterRun, parentStore, "Final: storeAfterRun should be parentStore");
            });

        it('should isolate context modifications between parent and child fibers', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const parentStore = { shared: true };
                let parentStoreAtEntry, parentStoreAfterChild, childStore, childStoreInOwnRun;

                // Pre-condition
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined");

                als.run(parentStore, () => {
                    parentStoreAtEntry = als.getStore();
                    // Verify parent store at entry
                    assert.strictEqual(parentStoreAtEntry, parentStore, "Parent run: store should be parentStore");
                    assert.strictEqual(parentStoreAtEntry.shared, true, "Parent run: store.shared should be true");
                    
                    const fiber = coroutine.start(() => {
                        // Child inherits parent context
                        childStore = als.getStore();
                        // Verify inherited store
                        assert.strictEqual(childStore, parentStore, "Child fiber: should inherit parentStore");
                        assert.strictEqual(childStore.shared, true, "Child fiber: store.shared should be true");
                        
                        // Child creates its own context
                        als.run({ childOwn: true }, () => {
                            childStoreInOwnRun = als.getStore();
                            // Verify child's own store
                            assert.deepStrictEqual(childStoreInOwnRun, { childOwn: true }, "Child own run: store should be {childOwn: true}");
                            assert.strictEqual(childStoreInOwnRun.childOwn, true, "Child own run: store.childOwn should be true");
                            
                            coroutine.sleep(10);
                            
                            // Verify store after sleep in child's own context
                            assert.deepStrictEqual(als.getStore(), { childOwn: true }, "Child own run after sleep: store should still be {childOwn: true}");
                        });
                        
                        // Verify store restored after child's own run
                        assert.strictEqual(als.getStore(), parentStore, "Child fiber after own run: store should be parentStore");
                    });
                    fiber.join();
                    
                    parentStoreAfterChild = als.getStore();
                    
                    // Parent's context should not be affected by child's als.run
                    assert.strictEqual(parentStoreAtEntry, parentStoreAfterChild, "Parent after child: store should be same as at entry");
                    assert.strictEqual(parentStoreAfterChild, parentStore, "Parent after child: store should be parentStore");
                });

                // Post-condition
                assert.strictEqual(als.getStore(), undefined, "Post: store should be undefined after run");

                // Final verification
                assert.strictEqual(childStore, parentStore, "Final: childStore should equal parentStore");
                assert.strictEqual(parentStoreAfterChild, parentStore, "Final: parentStoreAfterChild should equal parentStore");
            });

        it('should propagate context through coroutine.parallel', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const store = { parallelTest: true };
                let stores = [];
                let storeAtEntry;

                // Pre-condition
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined");

                als.run(store, () => {
                    // Verify store at entry
                    storeAtEntry = als.getStore();
                    assert.strictEqual(storeAtEntry, store, "Run entry: store should be set");
                    assert.strictEqual(storeAtEntry.parallelTest, true, "Run entry: store.parallelTest should be true");
                    
                    coroutine.parallel([
                        () => { 
                            const s = als.getStore();
                            assert.strictEqual(s, store, "Parallel task 1: store should be set");
                            assert.strictEqual(s.parallelTest, true, "Parallel task 1: store.parallelTest should be true");
                            stores.push(s); 
                        },
                        () => { 
                            const s = als.getStore();
                            assert.strictEqual(s, store, "Parallel task 2: store should be set");
                            assert.strictEqual(s.parallelTest, true, "Parallel task 2: store.parallelTest should be true");
                            stores.push(s); 
                        },
                        () => { 
                            const s = als.getStore();
                            assert.strictEqual(s, store, "Parallel task 3: store should be set");
                            assert.strictEqual(s.parallelTest, true, "Parallel task 3: store.parallelTest should be true");
                            stores.push(s); 
                        }
                    ]);
                    
                    // Verify store after parallel
                    assert.strictEqual(als.getStore(), store, "Run after parallel: store should still be set");
                });

                // Post-condition
                assert.strictEqual(als.getStore(), undefined, "Post: store should be undefined after run");

                // Final verification
                assert.strictEqual(stores.length, 3, "Final: should have 3 stores");
                stores.forEach((s, i) => {
                    assert.strictEqual(s, store, `Final: stores[${i}] should equal store`);
                });
            });
        });

        describe('concurrent Promise chains with sleep', () => {
            it('should isolate context in concurrent Promise chains with interleaved sleep', () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const results = [];

                // Pre-condition: no store before any run
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined before any run");

                // Chain A
                als.run("A", () => {
                    // Verify store is set correctly at run entry
                    assert.strictEqual(als.getStore(), "A", "Chain A: store should be 'A' at run entry");
                    
                    Promise.resolve().then(() => {
                        results.push({ chain: 'A-1', store: als.getStore() });
                        assert.strictEqual(als.getStore(), "A", "A-1: store should be 'A'");
                        
                        Promise.resolve().then(() => {
                            results.push({ chain: 'A-2-before', store: als.getStore() });
                            assert.strictEqual(als.getStore(), "A", "A-2-before: store should be 'A'");
                            coroutine.sleep(5);
                            results.push({ chain: 'A-2-after', store: als.getStore() });
                            assert.strictEqual(als.getStore(), "A", "A-2-after: store should still be 'A' after sleep");
                        });
                        
                        coroutine.sleep(10);
                        results.push({ chain: 'A-1-after', store: als.getStore() });
                        assert.strictEqual(als.getStore(), "A", "A-1-after: store should still be 'A' after sleep");
                    });
                });

                // Post-condition: store should be undefined after run
                assert.strictEqual(als.getStore(), undefined, "Post Chain A: store should be undefined after run");

                // Chain B
                als.run("B", () => {
                    // Verify store is set correctly at run entry
                    assert.strictEqual(als.getStore(), "B", "Chain B: store should be 'B' at run entry");
                    
                    Promise.resolve().then(() => {
                        results.push({ chain: 'B-1', store: als.getStore() });
                        assert.strictEqual(als.getStore(), "B", "B-1: store should be 'B'");
                        
                        Promise.resolve().then(() => {
                            results.push({ chain: 'B-2-before', store: als.getStore() });
                            assert.strictEqual(als.getStore(), "B", "B-2-before: store should be 'B'");
                            coroutine.sleep(5);
                            results.push({ chain: 'B-2-after', store: als.getStore() });
                            assert.strictEqual(als.getStore(), "B", "B-2-after: store should still be 'B' after sleep");
                        });
                        
                        coroutine.sleep(10);
                        results.push({ chain: 'B-1-after', store: als.getStore() });
                        assert.strictEqual(als.getStore(), "B", "B-1-after: store should still be 'B' after sleep");
                    });
                });

                // Post-condition: store should be undefined after run
                assert.strictEqual(als.getStore(), undefined, "Post Chain B: store should be undefined after run");

                coroutine.sleep(50);

                // Final verification: all results collected
                assert.strictEqual(results.length, 8, "Should have 8 results (4 from each chain)");
                
                // Verify all A results have store "A" and all B results have store "B"
                results.forEach(r => {
                    const expected = r.chain.startsWith('A') ? 'A' : 'B';
                    assert.strictEqual(r.store, expected, `${r.chain} expected store ${expected}, got ${r.store}`);
                });

                // Post-condition: store should still be undefined after all callbacks complete
                assert.strictEqual(als.getStore(), undefined, "Final: store should be undefined after all callbacks");
            });

            it('should restore outer context after await inner als.run with sleep', async () => {
                const coroutine = require('coroutine');
                const als = new AsyncLocalStorage();
                const results = [];

                // Pre-condition: no store before run
                assert.strictEqual(als.getStore(), undefined, "Pre: store should be undefined before run");

                await als.run("OUTER", async () => {
                    // Step 1: verify OUTER context at entry
                    results.push({ step: 1, store: als.getStore() });
                    assert.strictEqual(als.getStore(), "OUTER", "Step 1: store should be 'OUTER' at entry");
                    
                    await als.run("INNER", async () => {
                        // Step 2: verify INNER context at inner entry
                        results.push({ step: 2, store: als.getStore() });
                        assert.strictEqual(als.getStore(), "INNER", "Step 2: store should be 'INNER' at inner entry");
                        
                        coroutine.sleep(10);
                        
                        // Step 3: verify INNER context after sleep
                        results.push({ step: 3, store: als.getStore() });
                        assert.strictEqual(als.getStore(), "INNER", "Step 3: store should still be 'INNER' after sleep");
                    });
                    
                    // Step 4: verify OUTER context is restored after await
                    results.push({ step: 4, store: als.getStore() });
                    assert.strictEqual(als.getStore(), "OUTER", "Step 4: store should be 'OUTER' after await inner run");
                });

                // Post-condition: store should be undefined after outer run
                assert.strictEqual(als.getStore(), undefined, "Post: store should be undefined after outer run");

                // Final verification of all collected results
                assert.strictEqual(results.length, 4, "Should have 4 results");
                assert.strictEqual(results[0].store, "OUTER", "Result 1 should have OUTER");
                assert.strictEqual(results[1].store, "INNER", "Result 2 should have INNER");
                assert.strictEqual(results[2].store, "INNER", "Result 3 should have INNER");
                assert.strictEqual(results[3].store, "OUTER", "Result 4 should have OUTER after await");
            });
        });
    }

    // Tests ported from Node.js test suite
    describe('Node.js compatibility tests', () => {
        it('should pass run and exit arguments correctly', () => {
            // From: test-async-local-storage-args.js
            const als = new AsyncLocalStorage();

            als.run({}, (runArg) => {
                assert.strictEqual(runArg, 'foo');
                als.exit((exitArg) => {
                    assert.strictEqual(exitArg, 'bar');
                }, 'bar');
            }, 'foo');
        });

        it('should handle nested run and exit correctly', () => {
            // From: test-async-local-storage-nested.js
            const als = new AsyncLocalStorage();
            const outer = {};
            const inner = {};

            function testInner() {
                assert.strictEqual(als.getStore(), outer);

                als.run(inner, () => {
                    assert.strictEqual(als.getStore(), inner);
                });
                assert.strictEqual(als.getStore(), outer);

                als.exit(() => {
                    assert.strictEqual(als.getStore(), undefined);
                });
                assert.strictEqual(als.getStore(), outer);
            }

            als.run(outer, testInner);
            assert.strictEqual(als.getStore(), undefined);
        });

        it('should not mix contexts between multiple instances with setTimeout', async () => {
            // From: test-async-local-storage-no-mix-contexts.js
            const als1 = new AsyncLocalStorage();
            const als2 = new AsyncLocalStorage();

            await new Promise(resolve => {
                als1.run(new Map(), () => {
                    als2.run(new Map(), () => {
                        const store = als1.getStore();
                        const store2 = als2.getStore();
                        store.set('hello', 'world');
                        store2.set('hello', 'foo');
                        setTimeout(() => {
                            assert.strictEqual(als1.getStore().get('hello'), 'world');
                            assert.strictEqual(als2.getStore().get('hello'), 'foo');
                            als1.exit(() => {
                                assert.strictEqual(als1.getStore(), undefined);
                                assert.strictEqual(als2.getStore().get('hello'), 'foo');
                            });
                            assert.strictEqual(als1.getStore().get('hello'), 'world');
                            assert.strictEqual(als2.getStore().get('hello'), 'foo');
                            resolve();
                        }, 50);
                    });
                });
            });

            await new Promise(resolve => {
                als1.run(new Map(), () => {
                    const store = als1.getStore();
                    store.set('hello', 'earth');
                    setTimeout(() => {
                        assert.strictEqual(als1.getStore().get('hello'), 'earth');
                        resolve();
                    }, 30);
                });
            });
        });

        it('should work with async/await pattern', async () => {
            // From: test-async-local-storage-async-await.js
            const als = new AsyncLocalStorage();

            async function test() {
                als.getStore().set('foo', 'bar');
                await Promise.resolve();
                assert.strictEqual(als.getStore().get('foo'), 'bar');
            }

            await als.run(new Map(), test);
            assert.strictEqual(als.getStore(), undefined);
        });

        it('should work with async functions and exit', async () => {
            // From: test-async-local-storage-async-functions.js
            const als = new AsyncLocalStorage();

            async function foo() {}

            async function testOut() {
                await foo();
                assert.strictEqual(als.getStore(), undefined);
            }

            async function testAwait() {
                await foo();
                assert.notStrictEqual(als.getStore(), undefined);
                assert.strictEqual(als.getStore().get('key'), 'value');
                await als.exit(testOut);
            }

            als.run(new Map(), () => {
                const store = als.getStore();
                store.set('key', 'value');
                testAwait();
            });
            assert.strictEqual(als.getStore(), undefined);
        });

        it('should bind function and preserve return value', () => {
            // From: test-async-local-storage-bind.js
            const fn = AsyncLocalStorage.bind(() => 123);
            assert.strictEqual(fn(), 123);

            let argReceived;
            const fn2 = AsyncLocalStorage.bind((arg) => { argReceived = arg; });
            fn2('test');
            assert.strictEqual(argReceived, 'test');
        });

        it('should handle snapshot correctly', () => {
            // From: test-async-local-storage-snapshot.js
            const als = new AsyncLocalStorage();
            const runInAsyncScope = als.run(123, () => AsyncLocalStorage.snapshot());
            const result = als.run(321, () => {
                return runInAsyncScope(() => {
                    return als.getStore();
                });
            });
            assert.strictEqual(result, 123);
        });

        it('should not leak store after multiple exit calls', () => {
            // From: test-async-local-storage-exit-does-not-leak.js
            const als = new AsyncLocalStorage();
            const data = true;
            let callCount = 0;

            function run(count) {
                if (count === 0) {
                    callCount++;
                    return;
                }
                assert.notStrictEqual(als.getStore(), data);
                als.run(data, () => {
                    als.exit(run, --count);
                });
            }
            run(100);
            assert.strictEqual(callCount, 1);
        });

        it('should handle thenable correctly', async () => {
            // From: test-async-local-storage-thenable.js
            const als = new AsyncLocalStorage();
            const data = Symbol('verifier');
            let thenCallCount = 0;

            function thenable() {
                return {
                    then(cb) {
                        thenCallCount++;
                        assert.strictEqual(als.getStore(), data);
                        setImmediate(cb);
                    }
                };
            }

            // Await a thenable
            await als.run(data, async () => {
                assert.strictEqual(als.getStore(), data);
                await thenable();
                assert.strictEqual(als.getStore(), data);
            });

            // Returning a thenable in an async function
            await als.run(data, async () => {
                try {
                    assert.strictEqual(als.getStore(), data);
                    return thenable();
                } finally {
                    assert.strictEqual(als.getStore(), data);
                }
            });

            assert.ok(thenCallCount >= 2, `then should be called at least 2 times, got ${thenCallCount}`);
        });
    });
});
