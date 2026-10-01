var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

async function sleep(ms) {
    return new Promise((resolve) => {
        setTimeout(resolve, ms);
    });
}

describe('assert', () => {
    it('assert', () => {
        var foo = 'bar';
        assert(foo == 'bar', "expected foo to equal `bar`");
    });

    describe('throws', () => {
        it('throws', () => {
            assert.throws(() => {
                throw new Error('foo');
            });
        });

        it('throws with regex', () => {
            assert.throws(() => {
                throw new Error('bar');
            }, /bar/);

            try {
                assert.throws(() => {
                    throw new Error('bar');
                }, /foo/);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message.replace(/\u001b\[[\d;]*m/g, '').trim(), "The input did not match the regular expression /foo/. Input:\n\nError: bar");
            }
        });

        it('throws with function', () => {
            assert.throws(() => {
                throw new Error('bar');
            }, (err) => {
                assert.equal(err.message, 'bar');
                return true;
            });

            try {
                assert.throws(() => {
                    throw new Error('bar');
                }, (err) => {
                    return err.message === 'foo';
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The validation function is expected to return \"true\". Received false");
            }

            try {
                assert.throws(() => {
                    throw new Error('bar');
                }, (err) => {
                    assert.equal(err.message, 'foo');
                    console.error(err.message);
                    return true;
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "Expected \"bar\" == \"foo\"");
            }
        });

        it('throws with Object and String properties', () => {
            assert.throws(() => {
                throw new Error('bar');
            }, {
                message: 'bar'
            });

            try {
                assert.throws(() => {
                    throw new Error('bar');
                }, {
                    message: 'foo'
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message.replace(/\u001b\[[\d;]*m/g, '').trim(), "Expected values to be strictly deep-equal:\n+ actual - expected\n\n  {\n+   \"message\": \"bar\"\n-   \"message\": \"foo\"\n  }");
            }
        });

        it('throws with Object and Number properties', () => {
            assert.throws(() => {
                throw new Error(1234);
            }, {
                message: '1234'
            });

            try {
                assert.throws(() => {
                    throw new Error(1234);
                }, {
                    message: 1234
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message.replace(/\u001b\[[\d;]*m/g, '').trim(), "Expected values to be strictly deep-equal:\n+ actual - expected\n\n  {\n+   \"message\": \"1234\"\n-   \"message\": 1234\n  }");
            }

            try {
                assert.throws(() => {
                    throw new Error(1234);
                }, {
                    message: '4567'
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message.replace(/\u001b\[[\d;]*m/g, '').trim(), "Expected values to be strictly deep-equal:\n+ actual - expected\n\n  {\n+   \"message\": \"1234\"\n-   \"message\": \"4567\"\n  }");
            }
        });

        it('throws with Object and String RegExp properties', () => {
            assert.throws(() => {
                throw new Error('bar');
            }, {
                message: /bar/
            });

            try {
                assert.throws(() => {
                    throw new Error('bar');
                }, {
                    message: /foo/
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message.replace(/\u001b\[[\d;]*m/g, '').trim(), "Expected values to be strictly deep-equal:\n+ actual - expected\n\n  {\n+   \"message\": \"bar\"\n-   \"message\": /foo/\n  }");
            }
        });

        it('throws with Error Object', () => {
            assert.throws(() => {
                throw new TypeError('Type error');
            }, TypeError);

            try {
                assert.throws(() => {
                    throw new Error('Type error');
                }, TypeError);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The error is expected to be an instance of \"TypeError\". Received \"Error\"");
            }

            // Test with different error type
            try {
                assert.throws(() => {
                    throw new RangeError('Range error');
                }, TypeError);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The error is expected to be an instance of \"TypeError\". Received \"RangeError\"");
            }

            // Test with validation function returning non-boolean
            try {
                assert.throws(() => {
                    throw new Error('error');
                }, (err) => {
                    return 1;
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The validation function is expected to return \"true\". Received false");
            }

            // Test nested error messages
            try {
                assert.throws(() => {
                    throw new Error('level 1');
                }, (err) => {
                    assert.throws(() => {
                        throw new Error('level 2');
                    }, /level 1/);
                    return true;
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.match(e.message, /level 2/);
            }
        });

        it('throws with arbitrary value', () => {
            const values = [42, {}, [], Symbol('xyzzy'), true, 'ball', undefined, null, NaN];
            for (let i = 0; i < values.length; i++) {
                assert.throws(() => {
                    throw values[i];
                });
            }
        });
    });

    describe('rejects', () => {
        it('rejects with no arguments', async () => {
            await assert.rejects(async () => {
                throw new Error('Rejection');
            });

            try {
                await assert.rejects(async () => {
                    // No rejection
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "Missing expected rejection");
            }
        });

        it('rejects with error message', async () => {
            await assert.rejects(async () => {
                throw new Error('Specific error');
            }, /Specific error/);

            try {
                await assert.rejects(async () => {
                    // No rejection
                }, /Specific error/);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "Missing expected rejection");
            }
        });

        it('rejects with error type', async () => {
            await assert.rejects(async () => {
                throw new TypeError('Type error');
            }, TypeError);

            try {
                await assert.rejects(async () => {
                    // No rejection
                }, TypeError);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "Missing expected rejection");
            }
        });

        it('rejects with error predicate', async () => {
            await assert.rejects(async () => {
                throw new Error('Custom error');
            }, (err) => {
                assert.equal(err.message, 'Custom error');
                return true;
            });

            try {
                await assert.rejects(async () => {
                    // No rejection
                }, (err) => {
                    return err.message === 'Custom error';
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "Missing expected rejection");
            }
        });

        it('rejects with promise', async () => {
            const rejectedPromise = Promise.reject(new Error('Promise rejection'));
            await assert.rejects(rejectedPromise);

            try {
                const resolvedPromise = Promise.resolve();
                await assert.rejects(resolvedPromise);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "Missing expected rejection");
            }
        });

        it('rejects with promise and error matching', async () => {
            const rejectedPromise = Promise.reject(new TypeError('Specific type error'));

            await assert.rejects(rejectedPromise, TypeError);
            await assert.rejects(rejectedPromise, /type error/i);

            try {
                await assert.rejects(rejectedPromise, RangeError);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The error is expected to be an instance of \"RangeError\". Received \"TypeError\"");
            }
        });

        it('rejects with promise and predicate', async () => {
            const rejectedPromise = Promise.reject(new Error('Predicate error'));

            await assert.rejects(rejectedPromise, (err) => {
                assert.equal(err.message, 'Predicate error');
                return true;
            });

            try {
                await assert.rejects(rejectedPromise, (err) => {
                    return err.message === 'Wrong error';
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The validation function is expected to return \"true\". Received false");
            }
        });

        it('rejects with arbitrary value', async () => {
            const values = [42, {}, [], Symbol('xyzzy'), true, 'ball', undefined, null, NaN];
            for (let i = 0; i < values.length; i++) {
                await assert.rejects(async () => {
                    throw values[i];
                });
            }
        });

        it("should not catch error in sync function", async () => {
            await assert.rejects(async () => {
                await assert.rejects(() => {
                    throw new Error("error");
                });
            });
        });

        it("should throw error in sync function directly", async () => {
            await assert.rejects(async () => {
                await assert.rejects(() => {
                });
            });
        });

        it('rejects with Error Object', async () => {
            await assert.rejects(async () => {
                throw new TypeError('Type error');
            }, TypeError);

            try {
                await assert.rejects(async () => {
                    throw new Error('Type error');
                }, TypeError);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The error is expected to be an instance of \"TypeError\". Received \"Error\"");
            }

            // Test with different error type
            try {
                await assert.rejects(async () => {
                    throw new RangeError('Range error');
                }, TypeError);
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The error is expected to be an instance of \"TypeError\". Received \"RangeError\"");
            }

            // Test with validation function returning non-boolean
            try {
                await assert.rejects(async () => {
                    throw new Error('error');
                }, (err) => {
                    return 1;
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The validation function is expected to return \"true\". Received false");
            }

            // Test nested error messages
            try {
                await assert.rejects(async () => {
                    throw new Error('level 1');
                }, async (err) => {
                    assert.equal(err.message, 'level 1');
                    await assert.rejects(async () => {
                        throw new Error('level 2');
                    });
                    return false; // Explicitly return false to trigger validation error
                });
                assert.fail('Should have thrown an error');
            } catch (e) {
                assert.equal(e.message, "The validation function is expected to return \"true\". Received false");
            }
        });
    });

    it('isTrue', () => {
        assert.isTrue(true);

        assert.throws(() => {
            assert.isTrue(false);
        }, { message: "Expected the expression to strictly equal true" });

        assert.throws(() => {
            assert.isTrue(1);
        }, { message: "Expected the expression to strictly equal true" });

        assert.throws(() => {
            assert.isTrue('test');
        }, { message: "Expected the expression to strictly equal true" });
    });

    it('fail', () => {
        assert.throws(() => {
            assert.fail();
        }, { message: "Failed" });

        assert.throws(() => {
            assert.fail("custom message");
        }, { message: "custom message" });
    });

    it('ok', () => {
        assert.ok(true);
        assert.ok(1);
        assert.ok('test');

        assert.throws(() => {
            assert.ok(false);
        }, { message: "Expected the expression to be truthy" });

        assert.throws(() => {
            assert.ok(0);
        }, { message: "Expected the expression to be truthy" });

        assert.throws(() => {
            assert.ok('');
        }, { message: "Expected the expression to be truthy" });
    });

    it('notOk', () => {
        assert.notOk(false);
        assert.notOk(0);
        assert.notOk('');

        assert.throws(() => {
            assert.notOk(true);
        }, { message: "Expected the expression to be falsy" });

        assert.throws(() => {
            assert.notOk(1);
        }, { message: "Expected the expression to be falsy" });

        assert.throws(() => {
            assert.notOk('test');
        }, { message: "Expected the expression to be falsy" });
    });

    it('isFalse', () => {
        assert.isFalse(false);

        assert.throws(() => {
            assert.isFalse(true);
        }, { message: "Expected the expression to strictly equal false" });

        assert.throws(() => {
            assert.isFalse(0);
        }, { message: "Expected the expression to strictly equal false" });
    });

    it('equal', () => {
        var foo;
        assert.equal(foo, undefined);
    });

    it('isObject', () => {
        function Foo() { }
        assert.isObject({});
        assert.isObject(new Foo());

        assert.throws(() => {
            assert.isObject(true);
        }, { message: "Expected the value to be an object" });

        assert.throws(() => {
            assert.isObject('foo');
        }, { message: "Expected the value to be an object" });
    });

    it('isNotObject', () => {
        function Foo() { }
        assert.isNotObject(5);

        assert.throws(() => {
            assert.isNotObject({});
        }, { message: "Expected the value not to be an object" });
    });

    it('notEqual', () => {
        assert.notEqual(3, 4);

        assert.throws(() => {
            assert.notEqual(5, 5);
        }, { message: "Expected 5 != 5" });
    });

    it("FIX: assert crash when valueOf throw error", () => {
        var obj = {
            valueOf() {
                throw new Error("abb");
            }
        };

        assert.throws(() => {
            assert.equal(obj, "abb");
        }, { name: 'AssertionError' });
    });

    it('strictEqual', () => {
        assert.strictEqual('foo', 'foo');

        assert.throws(() => {
            assert.strictEqual('5', 5);
        }, { message: "Expected values to be strictly equal:\n\n\"5\" !== 5\n" });
    });

    it('notStrictEqual', () => {
        assert.notStrictEqual(5, '5');

        assert.throws(() => {
            assert.notStrictEqual(5, 5);
        }, { message: "Expected \"actual\" to be strictly unequal to: 5" });
    });

    it('deepEqual', () => {
        assert.deepEqual({
            tea: 'chai'
        }, {
            tea: 'chai'
        });

        assert.throws(() => {
            assert.deepEqual({
                tea: 'chai'
            }, {
                tea: 'black'
            });
        }, { message: "Expected values to be loosely deep-equal:\n\n{\n  \"tea\": \"chai\"\n}\n\nshould loosely deep-equal\n\n{\n  \"tea\": \"black\"\n}" });

        var obja = Object.create({
            tea: 'chai'
        }),
            objb = Object.create({
                tea: 'chai'
            });

        assert.deepEqual(obja, objb);

        var obj1 = Object.create({
            tea: 'chai'
        }),
            obj2 = Object.create({
                tea: 'black'
            });

        assert.throws(() => {
            assert.deepEqual(obj1, obj2);
        }, { message: "Expected values to be loosely deep-equal:\n\n{\n  \"tea\": \"chai\"\n}\n\nshould loosely deep-equal\n\n{\n  \"tea\": \"black\"\n}" });

        assert.throws(() => {
            assert.deepEqual({
                "100": 2,
                "5": 2
            }, {
                "100": 2,
                "5": 4
            });
        });

        assert.throws(() => {
            assert.deepEqual({
                "100": 2,
                "5": 2
            }, {
                "1": 2,
                "5": 2
            });
        });

        assert.notDeepEqual({
            "100": 2,
            "5": 2
        }, {
            "1": 2,
            "5": 4
        });

        assert.throws(() => {
            assert.notDeepEqual({
                "100": 2,
                "5": 2
            }, {
                "100": 2,
                "5": 2
            });
        });

        assert.deepEqual({
            tea: 100
        }, {
            tea: '100'
        });

        assert.throws(() => {
            assert.deepStrictEqual({
                tea: 100
            }, {
                tea: '100'
            });
        });
    });

    it('deepEqual (ordering)', () => {
        assert.deepEqual({
            a: 'b',
            c: 'd'
        }, {
            c: 'd',
            a: 'b'
        });
    });

    it('deepEqual /regexp/', () => {
        assert.deepEqual(/a/, /a/);
        assert.notDeepEqual(/a/, /b/);
        assert.notDeepEqual(/a/, {});
        assert.deepEqual(/a/g, /a/g);
        assert.notDeepEqual(/a/g, /b/g);
        assert.deepEqual(/a/i, /a/i);
        assert.notDeepEqual(/a/i, /b/i);
        assert.deepEqual(/a/m, /a/m);
        assert.notDeepEqual(/a/m, /b/m);
    });

    it('deepEqual (Date)', () => {
        var a = new Date(1, 2, 3),
            b = new Date(4, 5, 6);
        assert.deepEqual(a, a);
        assert.notDeepEqual(a, b);
        assert.notDeepEqual(a, {});
    });

    it('deepEqual (Buffer)', () => {
        var a = new Buffer("123"),
            b = new Buffer("456");
        assert.deepEqual(a, a);
        assert.notDeepEqual(a, b);
        assert.notDeepEqual(a, {});
    });

    it('deepEqual (circular)', () => {
        var circularObject = {},
            secondCircularObject = {};
        circularObject.field = circularObject;
        secondCircularObject.field = secondCircularObject;

        assert.deepEqual(circularObject, secondCircularObject);

        assert.throws(() => {
            secondCircularObject.field2 = secondCircularObject;
            assert.deepEqual(circularObject,
                secondCircularObject);
        }, { message: "Expected values to be loosely deep-equal:\n\n{\n  \"field\": [Circular]\n}\n\nshould loosely deep-equal\n\n{\n  \"field\": [Circular],\n  \"field2\": [Circular]\n}" });
    });

    it('notDeepEqual', () => {
        assert.notDeepEqual({
            tea: 'jasmine'
        }, {
            tea: 'chai'
        });

        assert.throws(() => {
            assert.notDeepEqual({
                tea: 'chai'
            }, {
                tea: 'chai'
            });
        }, { message: "Expected \"actual\" not to be loosely deep-equal to:\n\n{\n  \"tea\": \"chai\"\n}" });
    });

    it('notDeepEqual (circular)', () => {
        var circularObject = {},
            secondCircularObject = {
                tea: 'jasmine'
            };
        circularObject.field = circularObject;
        secondCircularObject.field = secondCircularObject;

        assert.notDeepEqual(circularObject,
            secondCircularObject);

        assert.throws(() => {
            delete secondCircularObject.tea;
            assert.notDeepEqual(circularObject,
                secondCircularObject);
        }, { message: "Expected \"actual\" not to be loosely deep-equal to:\n\n{\n  \"field\": [Circular]\n}" });
    });

    it("match", () => {
        assert.match('foobar', /^foo/);

        assert.throws(() => {
            assert.match('foobar', /^bar/);
        }, { message: "Expected \"foobar\" to match /^bar/" });
    });

    it("doesNotMatch", () => {
        assert.doesNotMatch('foobar', /^bar/);

        assert.throws(() => {
            assert.doesNotMatch('foobar', /^foo/);
        }, { message: "Expected \"foobar\" not to match /^foo/" });
    });

    it('isNull', () => {
        assert.isNull(null);

        assert.throws(() => {
            assert.isNull(undefined);
        }, { message: "Expected the value to be null" });
    });

    it('isNotNull', () => {
        assert.isNotNull(undefined);

        assert.throws(() => {
            assert.isNotNull(null);
        }, { message: "Expected the value not to be null" });
    });

    it('isUndefined', () => {
        assert.isUndefined(undefined);

        assert.throws(() => {
            assert.isUndefined(null);
        }, { message: "Expected the value to be undefined" });
    });

    it('isDefined', () => {
        assert.isDefined(null);

        assert.throws(() => {
            assert.isDefined(undefined);
        }, { message: "Expected the value not to be undefined" });
    });

    it('isFunction', () => {
        var func = () => { };
        assert.isFunction(func);

        assert.throws(() => {
            assert.isFunction({});
        }, { message: "Expected the value to be a function" });
    });

    it('isNotFunction', () => {
        assert.isNotFunction(5);

        assert.throws(() => {
            assert.isNotFunction(() => { });
        }, { message: "Expected the value not to be a function" });
    });

    it('isArray', () => {
        assert.isArray([]);
        assert.isArray(new Array);

        assert.throws(() => {
            assert.isArray({});
        }, { message: "Expected the value to be an array" });
    });

    it('isNotArray', () => {
        assert.isNotArray(3);

        assert.throws(() => {
            assert.isNotArray([]);
        }, { message: "Expected the value not to be an array" });

        assert.throws(() => {
            assert.isNotArray(new Array);
        }, { message: "Expected the value not to be an array" });
    });

    it('isString', () => {
        assert.isString('Foo');
        assert.isString(new String('foo'));

        assert.throws(() => {
            assert.isString(1);
        }, { message: "Expected the value to be a string" });
    });

    it('isNotString', () => {
        assert.isNotString(3);
        assert.isNotString(['hello']);

        assert.throws(() => {
            assert.isNotString('hello');
        }, { message: "Expected the value not to be a string" });
    });

    it('isNumber', () => {
        assert.isNumber(1);
        assert.isNumber(Number('3'));

        assert.throws(() => {
            assert.isNumber('1');
        }, { message: "Expected the value to be a number" });
    });

    it('isNotNumber', () => {
        assert.isNotNumber('hello');
        assert.isNotNumber([5]);

        assert.throws(() => {
            assert.isNotNumber(4);
        }, { message: "Expected the value not to be a number" });
    });

    it('isBoolean', () => {
        assert.isBoolean(true);
        assert.isBoolean(false);

        assert.throws(() => {
            assert.isBoolean('1');
        }, { message: "Expected the value to be a boolean" });
    });

    it('isNotBoolean', () => {
        assert.isNotBoolean('true');

        assert.throws(() => {
            assert.isNotBoolean(true);
        }, { message: "Expected the value not to be a boolean" });

        assert.throws(() => {
            assert.isNotBoolean(false);
        }, { message: "Expected the value not to be a boolean" });
    });

    it('property', () => {
        var obj = {
            foo: {
                bar: 'baz'
            }
        };
        var simpleObj = {
            foo: 'bar'
        };
        assert.property(obj, 'foo');
        assert.deepProperty(obj, 'foo.bar');
        assert.notProperty(obj, 'baz');
        assert.notProperty(obj, 'foo.bar');
        assert.notDeepProperty(obj, 'foo.baz');
        assert.deepPropertyVal(obj, 'foo.bar', 'baz');
        assert.deepPropertyNotVal(obj, 'foo.bar', 'flow');

        assert.throws(() => {
            assert.property(obj, 'baz');
        }, { message: "Expected {\n  \"foo\": {\n    \"bar\": \"baz\"\n  }\n} to have property \"baz\"" });

        assert.throws(() => {
            assert.deepProperty(obj, 'foo.baz');
        }, { message: "Expected {\n  \"foo\": {\n    \"bar\": \"baz\"\n  }\n} to have deep property \"foo.baz\"" });

        assert.throws(() => {
            assert.notProperty(obj, 'foo');
        }, { message: "Expected {\n  \"foo\": {\n    \"bar\": \"baz\"\n  }\n} not to have property \"foo\"" });

        assert.throws(() => {
            assert.notDeepProperty(obj, 'foo.bar');
        }, { message: "Expected {\n  \"foo\": {\n    \"bar\": \"baz\"\n  }\n} not to have deep property \"foo.bar\"" });

        assert.throws(() => {
            assert.propertyVal(simpleObj, 'foo', 'ball');
        }, { message: "Expected {\n  \"foo\": \"bar\"\n} to have property with value \"ball\"" });

        assert.throws(() => {
            assert.deepPropertyVal(obj, 'foo.bar', 'ball');
        }, { message: "Expected {\n  \"foo\": {\n    \"bar\": \"baz\"\n  }\n} to have deep property with value \"ball\"" });

        assert.throws(() => {
            assert.propertyNotVal(simpleObj, 'foo', 'bar');
        }, { message: "Expected {\n  \"foo\": \"bar\"\n} not to have property with value \"bar\"" });

        assert.throws(() => {
            assert.deepPropertyNotVal(obj, 'foo.bar', 'baz');
        }, { message: "Expected {\n  \"foo\": {\n    \"bar\": \"baz\"\n  }\n} not to have deep property with value \"baz\"" });
    });

    it('throws', () => {
        assert.throws(() => {
            throw new Error('foo');
        });
        assert.throws(() => {
            throw new Error('bar');
        }, { message: 'bar' });
        assert.throws(() => {
            throw new Error('bar');
        }, /bar/);

        assert.throws(() => {
            assert.throws(() => { });
        }, { message: "Missing expected exception" });
    });

    it("throws async", async () => {
        await sleep(1);
        await assert.rejects(async () => {
            throw "error";
        });
    });

    it('doesNotThrow', () => {
        assert.doesNotThrow(() => { });
        assert.doesNotThrow(() => { }, 'foo');

        assert.throws(() => {
            assert.doesNotThrow(() => {
                throw new Error('foo');
            });
        }, { message: "Got unwanted exception" });
    });

    it('closeTo', () => {
        assert.closeTo(1.5, 1.0, 0.5);
        assert.closeTo(10, 20, 20);
        assert.closeTo(-10, 20, 30);

        assert.throws(() => {
            assert.closeTo(2, 1.0, 0.5);
        }, { message: "Expected 2 to be close to 1" });

        assert.throws(() => {
            assert.closeTo(-10, 20, 29);
        }, { message: "Expected -10 to be close to 20" });
    });

    it('ifError', () => {
        assert.throws(() => assert.ifError(1));
        assert.throws(() => assert.ifError('a'));
        assert.throws(() => assert.ifError(new Error('error')));
        assert.throws(function () { assert.ifError(new Error('test error')); }); // ifError throws the value itself
        assert.doesNotThrow(function () { assert.ifError(null); });
        assert.doesNotThrow(function () { assert.ifError(); });

        let threw = false;
        try {
            assert.throws(() => assert.ifError(null));
        } catch (e) {
            threw = true;
            assert.strictEqual(e.message, 'Missing expected exception');
        }
        assert.ok(threw);
    });

    it('assert.strict', () => {
        var strict1 = require('assert').strict;
        var strict2 = require('assert/strict');
        var strict3 = require('fibjs:assert/strict');
        var strict4 = require('node:assert/strict');

        assert.equal(strict1, strict2);
        assert.equal(strict1, strict3);
        assert.equal(strict1, strict4);
    });
});
