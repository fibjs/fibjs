var test = require("test");
test.setup();

describe('assert', () => {
    it('assert', () => {
        var foo = 'bar';
        assert(foo == 'bar', "expected foo to equal `bar`");
    });

    it('isTrue', () => {
        assert.isTrue(true);

        assert.throws(() => {
            assert.isTrue(false);
        }, "expected false to be true");

        assert.throws(() => {
            assert.isTrue(1);
        }, "expected 1 to be true");

        assert.throws(() => {
            assert.isTrue('test');
        }, "expected 'test' to be true");
    });

    it('ok', () => {
        assert.ok(true);
        assert.ok(1);
        assert.ok('test');

        assert.throws(() => {
            assert.ok(false);
        }, "expected false to be truthy");

        assert.throws(() => {
            assert.ok(0);
        }, "expected 0 to be truthy");

        assert.throws(() => {
            assert.ok('');
        }, "expected '' to be truthy");
    });

    it('notOk', () => {
        assert.notOk(false);
        assert.notOk(0);
        assert.notOk('');

        assert.throws(() => {
            assert.notOk(true);
        }, "expected true to be falsy");

        assert.throws(() => {
            assert.notOk(1);
        }, "expected 1 to be falsy");

        assert.throws(() => {
            assert.notOk('test');
        }, "expected 'test' to be falsy");
    });

    it('isFalse', () => {
        assert.isFalse(false);

        assert.throws(() => {
            assert.isFalse(true);
        }, "expected true to be false");

        assert.throws(() => {
            assert.isFalse(0);
        }, "expected 0 to be false");
    });

    it('equal', () => {
        var foo;
        assert.equal(foo, undefined);
    });

    it('isObject', () => {
        function Foo() {}
        assert.isObject({});
        assert.isObject(new Foo());

        assert.throws(() => {
            assert.isObject(true);
        }, "expected true to be an object");

        assert.throws(() => {
            assert.isObject('foo');
        }, "expected 'foo' to be an object");
    });

    it('isNotObject', () => {
        function Foo() {}
        assert.isNotObject(5);

        assert.throws(() => {
            assert.isNotObject({});
        }, "expected {} not to be an object");
    });

    it('notEqual', () => {
        assert.notEqual(3, 4);

        assert.throws(() => {
            assert.notEqual(5, 5);
        }, "expected 5 to not equal 5");
    });

    it('strictEqual', () => {
        assert.strictEqual('foo', 'foo');

        assert.throws(() => {
            assert.strictEqual('5', 5);
        }, "expected \'5\' to equal 5");
    });

    it('notStrictEqual', () => {
        assert.notStrictEqual(5, '5');

        assert.throws(() => {
            assert.notStrictEqual(5, 5);
        }, "expected 5 to not equal 5");
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
            },
            "expected { tea: \'chai\' } to deeply equal { tea: \'black\' }");

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
            },
            "expected { tea: \'chai\' } to deeply equal { tea: \'black\' }");

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
    });

    it('deepEqual (ordering)', () => {
        var a = {
                a: 'b',
                c: 'd'
            },
            b = {
                c: 'd',
                a: 'b'
            };
        assert.deepEqual(a, b);
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
            },
            "expected { field: [Circular] } to deeply equal { Object (field, field2) }");
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
            },
            "expected { tea: \'chai\' } to not deeply equal { tea: \'chai\' }");
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
            },
            "expected { field: [Circular] } to not deeply equal { field: [Circular] }");
    });

    it('isNull', () => {
        assert.isNull(null);

        assert.throws(() => {
            assert.isNull(undefined);
        }, "expected undefined to equal null");
    });

    it('isNotNull', () => {
        assert.isNotNull(undefined);

        assert.throws(() => {
            assert.isNotNull(null);
        }, "expected null to not equal null");
    });

    it('isUndefined', () => {
        assert.isUndefined(undefined);

        assert.throws(() => {
            assert.isUndefined(null);
        }, "expected null to equal undefined");
    });

    it('isDefined', () => {
        assert.isDefined(null);

        assert.throws(() => {
            assert.isDefined(undefined);
        }, "expected undefined to not equal undefined");
    });

    it('isFunction', () => {
        var func = () => {};
        assert.isFunction(func);

        assert.throws(() => {
            assert.isFunction({});
        }, "expected {} to be a function");
    });

    it('isNotFunction', () => {
        assert.isNotFunction(5);

        assert.throws(() => {
            assert.isNotFunction(() => {});
        }, "expected [Function] not to be a function");
    });

    it('isArray', () => {
        assert.isArray([]);
        assert.isArray(new Array);

        assert.throws(() => {
            assert.isArray({});
        }, "expected {} to be an array");
    });

    it('isNotArray', () => {
        assert.isNotArray(3);

        assert.throws(() => {
            assert.isNotArray([]);
        }, "expected [] not to be an array");

        assert.throws(() => {
            assert.isNotArray(new Array);
        }, "expected [] not to be an array");
    });

    it('isString', () => {
        assert.isString('Foo');
        assert.isString(new String('foo'));

        assert.throws(() => {
            assert.isString(1);
        }, "expected 1 to be a string");
    });

    it('isNotString', () => {
        assert.isNotString(3);
        assert.isNotString(['hello']);

        assert.throws(() => {
            assert.isNotString('hello');
        }, "expected 'hello' not to be a string");
    });

    it('isNumber', () => {
        assert.isNumber(1);
        assert.isNumber(Number('3'));

        assert.throws(() => {
            assert.isNumber('1');
        }, "expected \'1\' to be a number");
    });

    it('isNotNumber', () => {
        assert.isNotNumber('hello');
        assert.isNotNumber([5]);

        assert.throws(() => {
            assert.isNotNumber(4);
        }, "expected 4 not to be a number");
    });

    it('isBoolean', () => {
        assert.isBoolean(true);
        assert.isBoolean(false);

        assert.throws(() => {
            assert.isBoolean('1');
        }, "expected \'1\' to be a boolean");
    });

    it('isNotBoolean', () => {
        assert.isNotBoolean('true');

        assert.throws(() => {
            assert.isNotBoolean(true);
        }, "expected true not to be a boolean");

        assert.throws(() => {
            assert.isNotBoolean(false);
        }, "expected false not to be a boolean");
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
        }, "expected { foo: { bar: 'baz' } } to have a property 'baz'");

        assert.throws(() => {
            assert.deepProperty(obj, 'foo.baz');
        }, "expected { foo: { bar: 'baz' } } to have a deep property 'foo.baz'");

        assert.throws(() => {
            assert.notProperty(obj, 'foo');
        }, "expected { foo: { bar: 'baz' } } to not have property 'foo'");

        assert.throws(() => {
            assert.notDeepProperty(obj, 'foo.bar');
        }, "expected { foo: { bar: 'baz' } } to not have deep property 'foo.bar'");

        assert.throws(() => {
            assert.propertyVal(simpleObj, 'foo', 'ball');
        }, "expected { foo: 'bar' } to have a property 'foo' of 'ball', but got 'bar'");

        assert.throws(() => {
            assert.deepPropertyVal(obj, 'foo.bar', 'ball');
        }, "expected { foo: { bar: 'baz' } } to have a deep property 'foo.bar' of 'ball', but got 'baz'");

        assert.throws(() => {
            assert.propertyNotVal(simpleObj, 'foo', 'bar');
        }, "expected { foo: 'bar' } to not have a property 'foo' of 'bar'");

        assert.throws(() => {
            assert.deepPropertyNotVal(obj, 'foo.bar', 'baz');
        }, "expected { foo: { bar: 'baz' } } to not have a deep property 'foo.bar' of 'baz'");
    });

    it('throws', () => {
        assert.throws(() => {
            throw new Error('foo');
        });
        assert.throws(() => {
            throw new Error('bar');
        }, 'bar');
        assert.throws(() => {
            throw new Error('bar');
        }, /bar/);

        assert.throws(() => {
            assert.throws(() => {});
        }, "expected [Function] to throw an error");
    });

    it('doesNotThrow', () => {
        assert.doesNotThrow(() => {});
        assert.doesNotThrow(() => {}, 'foo');

        assert.throws(() => {
                assert.doesNotThrow(() => {
                    throw new Error('foo');
                });
            },
            'expected [Function] to not throw an error but [Error: foo] was thrown');
    });

    it('closeTo', () => {
        assert.closeTo(1.5, 1.0, 0.5);
        assert.closeTo(10, 20, 20);
        assert.closeTo(-10, 20, 30);

        assert.throws(() => {
            assert.closeTo(2, 1.0, 0.5);
        }, "expected 2 to be close to 1 +/- 0.5");

        assert.throws(() => {
            assert.closeTo(-10, 20, 29);
        }, "expected -10 to be close to 20 +/- 29");
    });

    it('ifError', () => {
        assert.throws(() => assert.ifError(1));
        assert.throws(() => assert.ifError('a'));
        assert.throws(() => assert.ifError(new Error('error')));
        assert.throws(function() { assert.ifError(new Error('test error')); });
        assert.doesNotThrow(function() { assert.ifError(null); });
        assert.doesNotThrow(function() { assert.ifError(); });

        let threw = false;
        try {
            assert.throws(() => assert.ifError(null));
        } catch (e) {
            threw = true;
            assert.strictEqual(e.message, 'Missing expected exception.');
        }
        assert.ok(threw);
    });
});

repl && test.run(console.DEBUG);