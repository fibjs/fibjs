var test = require("test");
test.setup();

const path = require('path');
const coroutine = require('coroutine');

const bin_path = path.dirname(process.execPath);

describe('addons api', () => {
    it("1_hello_world", () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, '1_hello_world.node'));

        assert.equal(module.exports.hello(), "world");
    });

    it('2_function_arguments', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, '2_function_arguments.node'));

        assert.equal(module.exports.add(3, 5), 8);
    });

    it('3_callbacks', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, '3_callbacks.node'));

        function testRecv(desiredRecv) {
            module.exports.RunCallbackWithRecv(function () {
                assert.equal(this, desiredRecv);
            }, desiredRecv);
        }

        // testRecv(undefined);
        // testRecv(null);
        testRecv(5);
        testRecv(true);
        testRecv('Hello');
        testRecv([]);
        testRecv({});

    });

    it('4_object_factory', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, '4_object_factory.node'));

        const obj1 = module.exports('hello');
        const obj2 = module.exports('world');
        assert.equal(`${obj1.msg} ${obj2.msg}`, 'hello world');
    });

    it('5_function_factory', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, '5_function_factory.node'));

        const fn = module.exports();
        assert.equal(fn(), 'hello world');
    });

    it('6_object_wrap', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, '6_object_wrap.node'));

        const valueDescriptor = Object.getOwnPropertyDescriptor(
            module.exports.MyObject.prototype, 'value');
        const valueReadonlyDescriptor = Object.getOwnPropertyDescriptor(
            module.exports.MyObject.prototype, 'valueReadonly');
        const plusOneDescriptor = Object.getOwnPropertyDescriptor(
            module.exports.MyObject.prototype, 'plusOne');
        assert.equal(typeof valueDescriptor.get, 'function');
        assert.equal(typeof valueDescriptor.set, 'function');
        assert.equal(valueDescriptor.value, undefined);
        assert.equal(valueDescriptor.enumerable, false);
        assert.equal(valueDescriptor.configurable, false);
        assert.equal(typeof valueReadonlyDescriptor.get, 'function');
        assert.equal(valueReadonlyDescriptor.set, undefined);
        assert.equal(valueReadonlyDescriptor.value, undefined);
        assert.equal(valueReadonlyDescriptor.enumerable, false);
        assert.equal(valueReadonlyDescriptor.configurable, false);

        assert.equal(plusOneDescriptor.get, undefined);
        assert.equal(plusOneDescriptor.set, undefined);
        assert.equal(typeof plusOneDescriptor.value, 'function');
        assert.equal(plusOneDescriptor.enumerable, false);
        assert.equal(plusOneDescriptor.configurable, false);

        const obj = new module.exports.MyObject(9);
        assert.equal(obj.value, 9);
        obj.value = 10;
        assert.equal(obj.value, 10);
        assert.equal(obj.valueReadonly, 10);
        // assert.throws(() => { obj.valueReadonly = 14; });
        assert.equal(obj.plusOne(), 11);
        assert.equal(obj.plusOne(), 12);
        assert.equal(obj.plusOne(), 13);

        assert.equal(obj.multiply().value, 13);
        assert.equal(obj.multiply(10).value, 130);

        const newobj = obj.multiply(-1);
        assert.equal(newobj.value, -13);
        assert.equal(newobj.valueReadonly, -13);
        assert.notEqual(obj, newobj);
    });

    it('test_array', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_array.node'));


        const array = [
            1,
            9,
            48,
            13493,
            9459324,
            { name: 'hello' },
            [
                'world',
                'node',
                'abi',
            ],
        ];

        assert.throws(
            () => {
                module.exports.TestGetElement(array, array.length + 1);
            },
            /^Error: assertion \(\(\(uint32_t\)index < length\)\) failed: Index out of bounds!$/,
        );

        assert.throws(
            () => {
                module.exports.TestGetElement(array, -2);
            },
            /^Error: assertion \(index >= 0\) failed: Invalid index\. Expects a positive integer\.$/,
        );

        array.forEach(function (element, index) {
            assert.strictEqual(module.exports.TestGetElement(array, index), element);
        });


        assert.deepEqual(module.exports.New(array), array);

        assert(module.exports.TestHasElement(array, 0));
        assert.strictEqual(module.exports.TestHasElement(array, array.length + 1), false);

        assert(module.exports.NewWithLength(0) instanceof Array);
        assert(module.exports.NewWithLength(1) instanceof Array);
        // Check max allowed length for an array 2^32 -1
        assert(module.exports.NewWithLength(4294967295) instanceof Array);

        {
            // Verify that array elements can be deleted.
            const arr = ['a', 'b', 'c', 'd'];

            assert.strictEqual(arr.length, 4);
            assert.strictEqual(2 in arr, true);
            assert.strictEqual(module.exports.TestDeleteElement(arr, 2), true);
            assert.strictEqual(arr.length, 4);
            assert.strictEqual(2 in arr, false);
        }

    });

    it('test_bigint', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_bigint.node'));

        const {
            IsLossless,
            TestInt64,
            TestUint64,
            TestWords,
            CreateTooBigBigInt,
            MakeBigIntWordsThrow,
        } = module.exports;

        [
            0n,
            -0n,
            1n,
            -1n,
            100n,
            2121n,
            -1233n,
            986583n,
            -976675n,
            98765432213456789876546896323445679887645323232436587988766545658n,
            -4350987086545760976737453646576078997096876957864353245245769809n,
        ].forEach((num) => {
            if (num > -(2n ** 63n) && num < 2n ** 63n) {
                assert.strictEqual(TestInt64(num), num);
                assert.strictEqual(IsLossless(num, true), true);
            } else {
                assert.strictEqual(IsLossless(num, true), false);
            }

            if (num >= 0 && num < 2n ** 64n) {
                assert.strictEqual(TestUint64(num), num);
                assert.strictEqual(IsLossless(num, false), true);
            } else {
                assert.strictEqual(IsLossless(num, false), false);
            }

            assert.strictEqual(num, TestWords(num));
        });

        assert.throws(() => CreateTooBigBigInt(), {
            name: 'Error',
            message: 'Invalid argument',
        });

        // Test that we correctly forward exceptions from the engine.
        assert.throws(() => MakeBigIntWordsThrow(), {
            name: 'RangeError',
            message: 'Maximum BigInt size exceeded',
        });
    });

    it('test_buffer', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_buffer.node'));
        const binding = module.exports;

        assert.strictEqual(binding.newBuffer().toString(), binding.theText);
        assert.strictEqual(binding.newExternalBuffer().toString(), binding.theText);

        assert.strictEqual(binding.getDeleterCallCount(), 0);
        GC();
        assert.strictEqual(binding.getDeleterCallCount(), 1);
        assert.strictEqual(binding.copyBuffer().toString(), binding.theText);

        assert.strictEqual(binding.copyBuffer().toString(), binding.theText);

        let buffer = binding.staticBuffer();
        assert.strictEqual(binding.bufferHasInstance(buffer), true);
        assert.strictEqual(binding.bufferInfo(buffer), true);

        buffer = null;
        assert.strictEqual(binding.getDeleterCallCount(), 1);
        GC();
        assert.strictEqual(binding.getDeleterCallCount(), 2);

    });

    it('test_cannot_run_js', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_cannot_run_js.node'));

        function mustNotCall() {
            assert(false, "must not call");
        }

        module.exports.createRef(mustNotCall);
    });

    it('test_constructor', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_constructor.node'));

        const TestConstructor = module.exports;
        assert.strictEqual(TestConstructor.constructorName.name, 'MyObject');

        const test_object = new TestConstructor();

        assert.strictEqual(test_object.echo('hello'), 'hello');

        test_object.readwriteValue = 1;
        assert.strictEqual(test_object.readwriteValue, 1);
        test_object.readwriteValue = 2;
        assert.strictEqual(test_object.readwriteValue, 2);

        // assert.throws(() => { test_object.readonlyValue = 3; },
        //     /^TypeError: Cannot assign to read only property 'readonlyValue' of object '#<MyObject>'$/);

        assert.ok(test_object.hiddenValue);

        // Properties with napi_enumerable attribute should be enumerable.
        const propertyNames = [];
        for (const name in test_object) {
            propertyNames.push(name);
        }
        assert.ok(propertyNames.includes('echo'));
        assert.ok(propertyNames.includes('readwriteValue'));
        assert.ok(propertyNames.includes('readonlyValue'));
        assert.ok(!propertyNames.includes('hiddenValue'));
        assert.ok(!propertyNames.includes('readwriteAccessor1'));
        assert.ok(!propertyNames.includes('readwriteAccessor2'));
        assert.ok(!propertyNames.includes('readonlyAccessor1'));
        assert.ok(!propertyNames.includes('readonlyAccessor2'));

        // The napi_writable attribute should be ignored for accessors.
        test_object.readwriteAccessor1 = 1;
        assert.strictEqual(test_object.readwriteAccessor1, 1);
        assert.strictEqual(test_object.readonlyAccessor1, 1);
        // assert.throws(() => { test_object.readonlyAccessor1 = 3; });
        test_object.readwriteAccessor2 = 2;
        assert.strictEqual(test_object.readwriteAccessor2, 2);
        assert.strictEqual(test_object.readonlyAccessor2, 2);
        // assert.throws(() => { test_object.readonlyAccessor2 = 3; });

        // Validate that static properties are on the class as opposed
        // to the instance
        assert.strictEqual(TestConstructor.staticReadonlyAccessor1, 10);
        assert.strictEqual(test_object.staticReadonlyAccessor1, undefined);

        // Verify that passing NULL to napi_define_class() results in the correct
        // error.
        assert.deepEqual(TestConstructor.TestDefineClass(), {
            envIsNull: 'Invalid argument',
            nameIsNull: 'Invalid argument',
            cbIsNull: 'Invalid argument',
            cbDataIsNull: 'napi_ok',
            propertiesIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
        });
    });

    it('test_conversions', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_conversions.node'));

        const boolExpected = /boolean was expected/;
        const numberExpected = /number was expected/;
        const stringExpected = /string was expected/;

        const testSym = Symbol('test');

        assert.strictEqual(module.exports.asBool(false), false);
        assert.strictEqual(module.exports.asBool(true), true);
        assert.throws(() => module.exports.asBool(undefined), boolExpected);
        assert.throws(() => module.exports.asBool(null), boolExpected);
        assert.throws(() => module.exports.asBool(Number.NaN), boolExpected);
        assert.throws(() => module.exports.asBool(0), boolExpected);
        assert.throws(() => module.exports.asBool(''), boolExpected);
        assert.throws(() => module.exports.asBool('0'), boolExpected);
        assert.throws(() => module.exports.asBool(1), boolExpected);
        assert.throws(() => module.exports.asBool('1'), boolExpected);
        assert.throws(() => module.exports.asBool('true'), boolExpected);
        assert.throws(() => module.exports.asBool({}), boolExpected);
        assert.throws(() => module.exports.asBool([]), boolExpected);
        assert.throws(() => module.exports.asBool(testSym), boolExpected);

        [module.exports.asInt32, module.exports.asUInt32, module.exports.asInt64].forEach((asInt) => {
            assert.strictEqual(asInt(0), 0);
            assert.strictEqual(asInt(1), 1);
            assert.strictEqual(asInt(1.0), 1);
            assert.strictEqual(asInt(1.1), 1);
            assert.strictEqual(asInt(1.9), 1);
            assert.strictEqual(asInt(0.9), 0);
            assert.strictEqual(asInt(999.9), 999);
            assert.strictEqual(asInt(Number.NaN), 0);
            assert.throws(() => asInt(undefined), numberExpected);
            assert.throws(() => asInt(null), numberExpected);
            assert.throws(() => asInt(false), numberExpected);
            assert.throws(() => asInt(''), numberExpected);
            assert.throws(() => asInt('1'), numberExpected);
            assert.throws(() => asInt({}), numberExpected);
            assert.throws(() => asInt([]), numberExpected);
            assert.throws(() => asInt(testSym), numberExpected);
        });

        assert.strictEqual(module.exports.asInt32(-1), -1);
        assert.strictEqual(module.exports.asInt64(-1), -1);
        assert.strictEqual(module.exports.asUInt32(-1), Math.pow(2, 32) - 1);

        assert.strictEqual(module.exports.asDouble(0), 0);
        assert.strictEqual(module.exports.asDouble(1), 1);
        assert.strictEqual(module.exports.asDouble(1.0), 1.0);
        assert.strictEqual(module.exports.asDouble(1.1), 1.1);
        assert.strictEqual(module.exports.asDouble(1.9), 1.9);
        assert.strictEqual(module.exports.asDouble(0.9), 0.9);
        assert.strictEqual(module.exports.asDouble(999.9), 999.9);
        assert.strictEqual(module.exports.asDouble(-1), -1);
        assert.ok(Number.isNaN(module.exports.asDouble(Number.NaN)));
        assert.throws(() => module.exports.asDouble(undefined), numberExpected);
        assert.throws(() => module.exports.asDouble(null), numberExpected);
        assert.throws(() => module.exports.asDouble(false), numberExpected);
        assert.throws(() => module.exports.asDouble(''), numberExpected);
        assert.throws(() => module.exports.asDouble('1'), numberExpected);
        assert.throws(() => module.exports.asDouble({}), numberExpected);
        assert.throws(() => module.exports.asDouble([]), numberExpected);
        assert.throws(() => module.exports.asDouble(testSym), numberExpected);

        assert.strictEqual(module.exports.asString(''), '');
        assert.strictEqual(module.exports.asString('test'), 'test');
        assert.throws(() => module.exports.asString(undefined), stringExpected);
        assert.throws(() => module.exports.asString(null), stringExpected);
        assert.throws(() => module.exports.asString(false), stringExpected);
        assert.throws(() => module.exports.asString(1), stringExpected);
        assert.throws(() => module.exports.asString(1.1), stringExpected);
        assert.throws(() => module.exports.asString(Number.NaN), stringExpected);
        assert.throws(() => module.exports.asString({}), stringExpected);
        assert.throws(() => module.exports.asString([]), stringExpected);
        assert.throws(() => module.exports.asString(testSym), stringExpected);

        assert.strictEqual(module.exports.toBool(true), true);
        assert.strictEqual(module.exports.toBool(1), true);
        assert.strictEqual(module.exports.toBool(-1), true);
        assert.strictEqual(module.exports.toBool('true'), true);
        assert.strictEqual(module.exports.toBool('false'), true);
        assert.strictEqual(module.exports.toBool({}), true);
        assert.strictEqual(module.exports.toBool([]), true);
        assert.strictEqual(module.exports.toBool(testSym), true);
        assert.strictEqual(module.exports.toBool(false), false);
        assert.strictEqual(module.exports.toBool(undefined), false);
        assert.strictEqual(module.exports.toBool(null), false);
        assert.strictEqual(module.exports.toBool(0), false);
        assert.strictEqual(module.exports.toBool(Number.NaN), false);
        assert.strictEqual(module.exports.toBool(''), false);

        assert.strictEqual(module.exports.toNumber(0), 0);
        assert.strictEqual(module.exports.toNumber(1), 1);
        assert.strictEqual(module.exports.toNumber(1.1), 1.1);
        assert.strictEqual(module.exports.toNumber(-1), -1);
        assert.strictEqual(module.exports.toNumber('0'), 0);
        assert.strictEqual(module.exports.toNumber('1'), 1);
        assert.strictEqual(module.exports.toNumber('1.1'), 1.1);
        assert.strictEqual(module.exports.toNumber([]), 0);
        assert.strictEqual(module.exports.toNumber(false), 0);
        assert.strictEqual(module.exports.toNumber(null), 0);
        assert.strictEqual(module.exports.toNumber(''), 0);
        assert.ok(Number.isNaN(module.exports.toNumber(Number.NaN)));
        assert.ok(Number.isNaN(module.exports.toNumber({})));
        assert.ok(Number.isNaN(module.exports.toNumber(undefined)));
        assert.throws(() => module.exports.toNumber(testSym), TypeError);

        assert.deepEqual({}, module.exports.toObject({}));
        assert.deepEqual({ 'test': 1 }, module.exports.toObject({ 'test': 1 }));
        assert.deepEqual([], module.exports.toObject([]));
        assert.deepEqual([1, 2, 3], module.exports.toObject([1, 2, 3]));
        assert.deepEqual(new Boolean(false), module.exports.toObject(false));
        assert.deepEqual(new Boolean(true), module.exports.toObject(true));
        assert.deepEqual(new String(''), module.exports.toObject(''));
        assert.deepEqual(new Number(0), module.exports.toObject(0));
        assert.deepEqual(new Number(Number.NaN), module.exports.toObject(Number.NaN));
        assert.deepEqual(new Object(testSym), module.exports.toObject(testSym));
        assert.notStrictEqual(module.exports.toObject(false), false);
        assert.notStrictEqual(module.exports.toObject(true), true);
        assert.notStrictEqual(module.exports.toObject(''), '');
        assert.notStrictEqual(module.exports.toObject(0), 0);
        assert.ok(!Number.isNaN(module.exports.toObject(Number.NaN)));

        assert.strictEqual(module.exports.toString(''), '');
        assert.strictEqual(module.exports.toString('test'), 'test');
        assert.strictEqual(module.exports.toString(undefined), 'undefined');
        assert.strictEqual(module.exports.toString(null), 'null');
        assert.strictEqual(module.exports.toString(false), 'false');
        assert.strictEqual(module.exports.toString(true), 'true');
        assert.strictEqual(module.exports.toString(0), '0');
        assert.strictEqual(module.exports.toString(1.1), '1.1');
        assert.strictEqual(module.exports.toString(Number.NaN), 'NaN');
        assert.strictEqual(module.exports.toString({}), '[object Object]');
        assert.strictEqual(module.exports.toString({ toString: () => 'test' }), 'test');
        assert.strictEqual(module.exports.toString([]), '');
        assert.strictEqual(module.exports.toString([1, 2, 3]), '1,2,3');
        assert.throws(() => module.exports.toString(testSym), TypeError);

        assert.deepEqual(module.exports.testNull.getValueBool(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'A boolean was expected',
        });

        assert.deepEqual(module.exports.testNull.getValueInt32(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'A number was expected',
        });

        assert.deepEqual(module.exports.testNull.getValueUint32(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'A number was expected',
        });

        assert.deepEqual(module.exports.testNull.getValueInt64(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'A number was expected',
        });


        assert.deepEqual(module.exports.testNull.getValueDouble(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'A number was expected',
        });

        assert.deepEqual(module.exports.testNull.coerceToBool(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'napi_ok',
        });

        assert.deepEqual(module.exports.testNull.coerceToObject(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'napi_ok',
        });

        assert.deepEqual(module.exports.testNull.coerceToString(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
            inputTypeCheck: 'napi_ok',
        });

        assert.deepEqual(module.exports.testNull.getValueStringUtf8(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            wrongTypeIn: 'A string was expected',
            bufAndOutLengthIsNull: 'Invalid argument',
        });

        assert.deepEqual(module.exports.testNull.getValueStringLatin1(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            wrongTypeIn: 'A string was expected',
            bufAndOutLengthIsNull: 'Invalid argument',
        });

        assert.deepEqual(module.exports.testNull.getValueStringUtf16(), {
            envIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
            wrongTypeIn: 'A string was expected',
            bufAndOutLengthIsNull: 'Invalid argument',
        });
    });

    it('test_dataview', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_dataview.node'));

        const test_dataview = module.exports;

        // Test for creating dataview
        {
            const buffer = new ArrayBuffer(128);
            const template = Reflect.construct(DataView, [buffer]);

            const theDataview = test_dataview.CreateDataViewFromJSDataView(template);
            assert.ok(theDataview instanceof DataView,
                `Expect ${theDataview} to be a DataView`);
        }

        // Test for creating dataview with invalid range
        {
            const buffer = new ArrayBuffer(128);
            assert.throws(() => {
                test_dataview.CreateDataView(buffer, 10, 200);
            }, RangeError);
        }
    });

    it('test_date', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_date.node'));
        const test_date = module.exports;

        const dateTypeTestDate = test_date.createDate(1549183351);
        assert.strictEqual(test_date.isDate(dateTypeTestDate), true);

        assert.strictEqual(test_date.isDate(new Date(1549183351)), true);

        assert.strictEqual(test_date.isDate(2.4), false);
        assert.strictEqual(test_date.isDate('not a date'), false);
        assert.strictEqual(test_date.isDate(undefined), false);
        assert.strictEqual(test_date.isDate(null), false);
        assert.strictEqual(test_date.isDate({}), false);

        assert.strictEqual(test_date.getDateValue(new Date(1549183351)), 1549183351);
    });

    it('test_error', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_error.node'));
        const test_error = module.exports;

        const theError = new Error('Some error');
        const theTypeError = new TypeError('Some type error');
        const theSyntaxError = new SyntaxError('Some syntax error');
        const theRangeError = new RangeError('Some type error');
        const theReferenceError = new ReferenceError('Some reference error');
        const theURIError = new URIError('Some URI error');
        const theEvalError = new EvalError('Some eval error');

        class MyError extends Error { }
        const myError = new MyError('Some MyError');

        // Test that native error object is correctly classed
        assert.strictEqual(test_error.checkError(theError), true);

        // Test that native type error object is correctly classed
        assert.strictEqual(test_error.checkError(theTypeError), true);

        // Test that native syntax error object is correctly classed
        assert.strictEqual(test_error.checkError(theSyntaxError), true);

        // Test that native range error object is correctly classed
        assert.strictEqual(test_error.checkError(theRangeError), true);

        // Test that native reference error object is correctly classed
        assert.strictEqual(test_error.checkError(theReferenceError), true);

        // Test that native URI error object is correctly classed
        assert.strictEqual(test_error.checkError(theURIError), true);

        // Test that native eval error object is correctly classed
        assert.strictEqual(test_error.checkError(theEvalError), true);

        // Test that class derived from native error is correctly classed
        assert.strictEqual(test_error.checkError(myError), true);

        // Test that non-error object is correctly classed
        assert.strictEqual(test_error.checkError({}), false);

        // Test that non-error primitive is correctly classed
        assert.strictEqual(test_error.checkError('non-object'), false);

        assert.throws(() => {
            test_error.throwExistingError();
        }, /^Error: existing error$/);

        assert.throws(() => {
            test_error.throwError();
        }, /^Error: error$/);

        assert.throws(() => {
            test_error.throwRangeError();
        }, /^RangeError: range error$/);

        assert.throws(() => {
            test_error.throwTypeError();
        }, /^TypeError: type error$/);

        assert.throws(() => {
            test_error.throwSyntaxError();
        }, /^SyntaxError: syntax error$/);

        [42, {}, [], Symbol('xyzzy'), true, 'ball', undefined, null, NaN]
            .forEach((value) => assert.throws(
                () => test_error.throwArbitrary(value),
                (err) => {
                    assert.strictEqual(err, value);
                    return true;
                },
            ));

        assert.throws(
            () => test_error.throwErrorCode(),
            {
                code: 'ERR_TEST_CODE',
                message: 'Error [error]',
            });

        assert.throws(
            () => test_error.throwRangeErrorCode(),
            {
                code: 'ERR_TEST_CODE',
                message: 'RangeError [range error]',
            });

        assert.throws(
            () => test_error.throwTypeErrorCode(),
            {
                code: 'ERR_TEST_CODE',
                message: 'TypeError [type error]',
            });

        assert.throws(
            () => test_error.throwSyntaxErrorCode(),
            {
                code: 'ERR_TEST_CODE',
                message: 'SyntaxError [syntax error]',
            });

        let error = test_error.createError();
        assert.ok(error instanceof Error, 'expected error to be an instance of Error');
        assert.strictEqual(error.message, 'error');

        error = test_error.createRangeError();
        assert.ok(error instanceof RangeError,
            'expected error to be an instance of RangeError');
        assert.strictEqual(error.message, 'range error');

        error = test_error.createTypeError();
        assert.ok(error instanceof TypeError,
            'expected error to be an instance of TypeError');
        assert.strictEqual(error.message, 'type error');

        error = test_error.createSyntaxError();
        assert.ok(error instanceof SyntaxError,
            'expected error to be an instance of SyntaxError');
        assert.strictEqual(error.message, 'syntax error');

        error = test_error.createErrorCode();
        assert.ok(error instanceof Error, 'expected error to be an instance of Error');
        assert.strictEqual(error.code, 'ERR_TEST_CODE');
        assert.strictEqual(error.message, 'Error [error]');
        assert.strictEqual(error.name, 'Error');

        error = test_error.createRangeErrorCode();
        assert.ok(error instanceof RangeError,
            'expected error to be an instance of RangeError');
        assert.strictEqual(error.message, 'RangeError [range error]');
        assert.strictEqual(error.code, 'ERR_TEST_CODE');
        assert.strictEqual(error.name, 'RangeError');

        error = test_error.createTypeErrorCode();
        assert.ok(error instanceof TypeError,
            'expected error to be an instance of TypeError');
        assert.strictEqual(error.message, 'TypeError [type error]');
        assert.strictEqual(error.code, 'ERR_TEST_CODE');
        assert.strictEqual(error.name, 'TypeError');

        error = test_error.createSyntaxErrorCode();
        assert.ok(error instanceof SyntaxError,
            'expected error to be an instance of SyntaxError');
        assert.strictEqual(error.message, 'SyntaxError [syntax error]');
        assert.strictEqual(error.code, 'ERR_TEST_CODE');
        assert.strictEqual(error.name, 'SyntaxError');
    });

    it('test_exception', () => {
        const test_exception = (function () {
            let resultingException;
            try {
                var module = {
                    exports: {}
                }
                process.dlopen(module, path.join(bin_path, 'test_exception.node'));
            } catch (anException) {
                resultingException = anException;
            }
            console.log(resultingException.binding);
            // assert.strictEqual(resultingException.message, 'Error during Init');
            return resultingException.binding;
        })();

        const theError = new Error('Some error');

        {
            const throwTheError = () => { throw theError; };

            // Test that the native side successfully captures the exception
            let returnedError = test_exception.returnException(throwTheError);
            assert.strictEqual(returnedError, theError);

            // Test that the native side passes the exception through
            assert.throws(
                () => { test_exception.allowException(throwTheError); },
                (err) => err === theError,
            );

            // Test that the exception thrown above was marked as pending
            // before it was handled on the JS side
            const exception_pending = test_exception.wasPending();
            assert.strictEqual(exception_pending, true,
                'Exception not pending as expected,' +
                ` .wasPending() returned ${exception_pending}`);

            // Test that the native side does not capture a non-existing exception
            // returnedError = test_exception.returnException(common.mustCall());
            // assert.strictEqual(returnedError, undefined,
            //     'Returned error should be undefined when no exception is' +
            //     ` thrown, but ${returnedError} was passed`);
        }


        {
            const throwTheError = class { constructor() { throw theError; } };

            // Test that the native side successfully captures the exception
            let returnedError = test_exception.constructReturnException(throwTheError);
            assert.strictEqual(returnedError, theError);

            // Test that the native side passes the exception through
            assert.throws(
                () => { test_exception.constructAllowException(throwTheError); },
                (err) => err === theError,
            );

            // Test that the exception thrown above was marked as pending
            // before it was handled on the JS side
            const exception_pending = test_exception.wasPending();
            assert.strictEqual(exception_pending, true,
                'Exception not pending as expected,' +
                ` .wasPending() returned ${exception_pending}`);

            // Test that the native side does not capture a non-existing exception
            // returnedError = test_exception.constructReturnException(common.mustCall());
            // assert.strictEqual(returnedError, undefined,
            //     'Returned error should be undefined when no exception is' +
            //     ` thrown, but ${returnedError} was passed`);
        }
    });

    it('test_function', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_function.node'));
        const test_function = module.exports;

        function func1() {
            return 1;
        }
        assert.strictEqual(test_function.TestCall(func1), 1);

        function func2() {
            console.log('hello world!');
            return null;
        }
        assert.strictEqual(test_function.TestCall(func2), null);

        function func3(input) {
            return input + 1;
        }
        assert.strictEqual(test_function.TestCall(func3, 1), 2);

        function func4(input) {
            return func3(input);
        }
        assert.strictEqual(test_function.TestCall(func4, 1), 2);

        assert.strictEqual(test_function.TestName.name, 'Name');
        assert.strictEqual(test_function.TestNameShort.name, 'Name_');

        // let tracked_function = test_function.MakeTrackedFunction(common.mustCall());
        // assert(!!tracked_function);
        // tracked_function = null;
        global.GC();

        assert.deepEqual(test_function.TestCreateFunctionParameters(), {
            envIsNull: 'Invalid argument',
            nameIsNull: 'napi_ok',
            cbIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
        });

        assert.throws(
            () => test_function.TestBadReturnExceptionPending(),
            {
                code: 'throwing exception',
                name: 'Error',
            },
        );
    });

    describe('test_general', () => {
        it('test', () => {
            var module = {
                exports: {}
            }
            process.dlopen(module, path.join(bin_path, 'test_general.node'));
            const test_general = module.exports;

            const val1 = '1';
            const val2 = 1;
            const val3 = 1;

            class BaseClass {
            }

            class ExtendedClass extends BaseClass {
            }

            const baseObject = new BaseClass();
            const extendedObject = new ExtendedClass();

            // Test napi_strict_equals
            assert.ok(test_general.testStrictEquals(val1, val1));
            assert.strictEqual(test_general.testStrictEquals(val1, val2), false);
            assert.ok(test_general.testStrictEquals(val2, val3));

            // Test napi_get_prototype
            assert.strictEqual(test_general.testGetPrototype(baseObject),
                Object.getPrototypeOf(baseObject));
            assert.strictEqual(test_general.testGetPrototype(extendedObject),
                Object.getPrototypeOf(extendedObject));
            // Prototypes for base and extended should be different.
            assert.notStrictEqual(test_general.testGetPrototype(baseObject),
                test_general.testGetPrototype(extendedObject));

            // Test version management functions
            assert.strictEqual(test_general.testGetVersion(), 9);

            [
                123,
                'test string',
                function () { },
                new Object(),
                true,
                undefined,
                Symbol(),
            ].forEach((val) => {
                assert.strictEqual(test_general.testNapiTypeof(val), typeof val);
            });

            // Since typeof in js return object need to validate specific case
            // for null
            assert.strictEqual(test_general.testNapiTypeof(null), 'null');

            // Assert that wrapping twice fails.
            const x = {};
            test_general.wrap(x);
            assert.throws(() => test_general.wrap(x));
            // Clean up here, otherwise derefItemWasCalled() will be polluted.
            test_general.removeWrap(x);

            // Ensure that wrapping, removing the wrap, and then wrapping again works.
            const y = {};
            test_general.wrap(y);
            test_general.removeWrap(y);
            // Wrapping twice succeeds if a remove_wrap() separates the instances
            test_general.wrap(y);
            // Clean up here, otherwise derefItemWasCalled() will be polluted.
            test_general.removeWrap(y);

            // Test napi_adjust_external_memory
            const adjustedValue = test_general.testAdjustExternalMemory();
            assert.strictEqual(typeof adjustedValue, 'number');
            assert(adjustedValue > 0);

            // async function runGCTests() {
            //     // Ensure that garbage collecting an object with a wrapped native item results
            //     // in the finalize callback being called.
            //     assert.strictEqual(test_general.derefItemWasCalled(), false);

            //     (() => test_general.wrap({}))();
            //     await common.gcUntil('deref_item() was called upon garbage collecting a ' +
            //         'wrapped object.',
            //         () => test_general.derefItemWasCalled());

            //     // Ensure that removing a wrap and garbage collecting does not fire the
            //     // finalize callback.
            //     let z = {};
            //     test_general.testFinalizeWrap(z);
            //     test_general.removeWrap(z);
            //     z = null;
            //     await common.gcUntil(
            //         'finalize callback was not called upon garbage collection.',
            //         () => (!test_general.finalizeWasCalled()));
            // }
            // runGCTests();
        });

        it('env cleanup', () => {
            var module = {
                exports: {}
            }
            process.dlopen(module, path.join(bin_path, 'test_general.node'));
            const test_general = module.exports;

            // The second argument to `envCleanupWrap()` is an index into the global
            // static string array named `env_cleanup_finalizer_messages` on the native
            // side. A reverse mapping is reproduced here for clarity.
            const finalizerMessages = {
                'simple wrap': 0,
                'wrap, removeWrap': 1,
                'first wrap': 2,
                'second wrap': 3,
            };

            // We attach the three objects we will test to `module.exports` to ensure they
            // will not be garbage-collected before the process exits.

            // Make sure the finalizer for a simple wrap will be called at env cleanup.
            module.exports['simple wrap'] =
                test_general.envCleanupWrap({}, finalizerMessages['simple wrap']);

            // Make sure that a removed wrap does not result in a call to its finalizer at
            // env cleanup.
            module.exports['wrap, removeWrap'] =
                test_general.envCleanupWrap({}, finalizerMessages['wrap, removeWrap']);
            test_general.removeWrap(module.exports['wrap, removeWrap']);

            // Make sure that only the latest attached version of a re-wrapped item's
            // finalizer gets called at env cleanup.
            module.exports['first wrap'] =
                test_general.envCleanupWrap({}, finalizerMessages['first wrap']);
            test_general.removeWrap(module.exports['first wrap']);
            test_general.envCleanupWrap(module.exports['first wrap'],
                finalizerMessages['second wrap']);
        });

        it('global', () => {
            var module = {
                exports: {}
            }
            process.dlopen(module, path.join(bin_path, 'test_general.node'));
            const test_globals = module.exports;

            assert.strictEqual(test_globals.getUndefined(), undefined);
            assert.strictEqual(test_globals.getNull(), null);
        });
    });

    it('test_handle_scope', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_handle_scope.node'));
        const testHandleScope = module.exports;

        testHandleScope.NewScope();

        assert.ok(testHandleScope.NewScopeEscape() instanceof Object);

        testHandleScope.NewScopeEscapeTwice();

        assert.throws(
            () => {
                testHandleScope.NewScopeWithException(() => { throw new RangeError(); });
            },
            RangeError);
    });

    it('test_init_order', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_init_order.node'));
        const test_init_order = module.exports;

        assert.strictEqual(test_init_order.cppIntValue, 42);
        assert.strictEqual(test_init_order.cppStringValue, '123');
    });

    it('test_new_target', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_new_target.node'));
        const binding = module.exports;

        class Class extends binding.BaseClass {
            constructor() {
                super();
                this.method();
            }
            method() {
                this.ok = true;
            }
        }

        assert.ok(new Class() instanceof binding.BaseClass);
        assert.ok(new Class().ok);
        assert.ok(binding.OrdinaryFunction());
        assert.ok(
            new binding.Constructor(binding.Constructor) instanceof binding.Constructor);
    });

    it('test_number', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_number.node'));
        const test_number = module.exports;

        // Testing api calls for number
        function testNumber(num) {
            assert.strictEqual(num, test_number.Test(num));
        }

        testNumber(0);
        testNumber(-0);
        testNumber(1);
        testNumber(-1);
        testNumber(100);
        testNumber(2121);
        testNumber(-1233);
        testNumber(986583);
        testNumber(-976675);

        /* eslint-disable no-loss-of-precision */
        testNumber(
            98765432213456789876546896323445679887645323232436587988766545658);
        testNumber(
            -4350987086545760976737453646576078997096876957864353245245769809);
        /* eslint-enable no-loss-of-precision */
        testNumber(Number.MIN_SAFE_INTEGER);
        testNumber(Number.MAX_SAFE_INTEGER);
        testNumber(Number.MAX_SAFE_INTEGER + 10);

        testNumber(Number.MIN_VALUE);
        testNumber(Number.MAX_VALUE);
        testNumber(Number.MAX_VALUE + 10);

        testNumber(Number.POSITIVE_INFINITY);
        testNumber(Number.NEGATIVE_INFINITY);
        // testNumber(Number.NaN);

        function testUint32(input, expected = input) {
            assert.strictEqual(expected, test_number.TestUint32Truncation(input));
        }

        // Test zero
        testUint32(0.0, 0);
        testUint32(-0.0, 0);

        // Test overflow scenarios
        testUint32(4294967295);
        testUint32(4294967296, 0);
        testUint32(4294967297, 1);
        testUint32(17 * 4294967296 + 1, 1);
        testUint32(-1, 0xffffffff);

        // Validate documented behavior when value is retrieved as 32-bit integer with
        // `napi_get_value_int32`
        function testInt32(input, expected = input) {
            assert.strictEqual(expected, test_number.TestInt32Truncation(input));
        }

        // Test zero
        testInt32(0.0, 0);
        testInt32(-0.0, 0);

        // Test min/max int32 range
        testInt32(-Math.pow(2, 31));
        testInt32(Math.pow(2, 31) - 1);

        // Test overflow scenarios
        testInt32(4294967297, 1);
        testInt32(4294967296, 0);
        testInt32(4294967295, -1);
        testInt32(4294967296 * 5 + 3, 3);

        // Test min/max safe integer range
        testInt32(Number.MIN_SAFE_INTEGER, 1);
        testInt32(Number.MAX_SAFE_INTEGER, -1);

        // Test within int64_t range (with precision loss)
        testInt32(-Math.pow(2, 63) + (Math.pow(2, 9) + 1), 1024);
        testInt32(Math.pow(2, 63) - (Math.pow(2, 9) + 1), -1024);

        // Test min/max double value
        testInt32(-Number.MIN_VALUE, 0);
        testInt32(Number.MIN_VALUE, 0);
        testInt32(-Number.MAX_VALUE, 0);
        testInt32(Number.MAX_VALUE, 0);

        // Test outside int64_t range
        testInt32(-Math.pow(2, 63) + (Math.pow(2, 9)), 0);
        testInt32(Math.pow(2, 63) - (Math.pow(2, 9)), 0);

        // Test non-finite numbers
        testInt32(Number.POSITIVE_INFINITY, 0);
        testInt32(Number.NEGATIVE_INFINITY, 0);
        testInt32(Number.NaN, 0);

        // Validate documented behavior when value is retrieved as 64-bit integer with
        // `napi_get_value_int64`
        function testInt64(input, expected = input) {
            assert.strictEqual(expected, test_number.TestInt64Truncation(input));
        }

        // Both V8 and ChakraCore return a sentinel value of `0x8000000000000000` when
        // the conversion goes out of range, but V8 treats it as unsigned in some cases.
        const RANGEERROR_POSITIVE = Math.pow(2, 63);
        const RANGEERROR_NEGATIVE = -Math.pow(2, 63);

        // Test zero
        testInt64(0.0, 0);
        testInt64(-0.0, 0);

        // Test min/max safe integer range
        testInt64(Number.MIN_SAFE_INTEGER);
        testInt64(Number.MAX_SAFE_INTEGER);

        // Test within int64_t range (with precision loss)
        testInt64(-Math.pow(2, 63) + (Math.pow(2, 9) + 1));
        testInt64(Math.pow(2, 63) - (Math.pow(2, 9) + 1));

        // Test min/max double value
        testInt64(-Number.MIN_VALUE, 0);
        testInt64(Number.MIN_VALUE, 0);
        testInt64(-Number.MAX_VALUE, RANGEERROR_NEGATIVE);
        testInt64(Number.MAX_VALUE, RANGEERROR_POSITIVE);

        // Test outside int64_t range
        testInt64(-Math.pow(2, 63) + (Math.pow(2, 9)), RANGEERROR_NEGATIVE);
        testInt64(Math.pow(2, 63) - (Math.pow(2, 9)), RANGEERROR_POSITIVE);

        // Test non-finite numbers
        testInt64(Number.POSITIVE_INFINITY, 0);
        testInt64(Number.NEGATIVE_INFINITY, 0);
        testInt64(Number.NaN, 0);
    });

    it('test_object', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_object.node'));
        const test_object = module.exports;

        const object = {
            hello: 'world',
            array: [
                1, 94, 'str', 12.321, { test: 'obj in arr' },
            ],
            newObject: {
                test: 'obj in obj',
            },
        };

        assert.strictEqual(test_object.Get(object, 'hello'), 'world');
        assert.strictEqual(test_object.GetNamed(object, 'hello'), 'world');
        assert.deepEqual(test_object.Get(object, 'array'),
            [1, 94, 'str', 12.321, { test: 'obj in arr' }]);
        assert.deepEqual(test_object.Get(object, 'newObject'),
            { test: 'obj in obj' });

        assert(test_object.Has(object, 'hello'));
        assert(test_object.HasNamed(object, 'hello'));
        assert(test_object.Has(object, 'array'));
        assert(test_object.Has(object, 'newObject'));

        const newObject = test_object.New();
        assert(test_object.Has(newObject, 'test_number'));
        assert.strictEqual(newObject.test_number, 987654321);
        assert.strictEqual(newObject.test_string, 'test string');

        {
            // Verify that napi_get_property() walks the prototype chain.
            function MyObject() {
                this.foo = 42;
                this.bar = 43;
            }

            MyObject.prototype.bar = 44;
            MyObject.prototype.baz = 45;

            const obj = new MyObject();

            assert.strictEqual(test_object.Get(obj, 'foo'), 42);
            assert.strictEqual(test_object.Get(obj, 'bar'), 43);
            assert.strictEqual(test_object.Get(obj, 'baz'), 45);
            assert.strictEqual(test_object.Get(obj, 'toString'),
                Object.prototype.toString);
        }

        {
            // Verify that napi_has_own_property() fails if property is not a name.
            [true, false, null, undefined, {}, [], 0, 1, () => { }].forEach((value) => {
                assert.throws(() => {
                    test_object.HasOwn({}, value);
                }, /^Error: A string or symbol was expected$/);
            });
        }

        {
            // Verify that napi_has_own_property() does not walk the prototype chain.
            const symbol1 = Symbol();
            const symbol2 = Symbol();

            function MyObject() {
                this.foo = 42;
                this.bar = 43;
                this[symbol1] = 44;
            }

            MyObject.prototype.bar = 45;
            MyObject.prototype.baz = 46;
            MyObject.prototype[symbol2] = 47;

            const obj = new MyObject();

            assert.strictEqual(test_object.HasOwn(obj, 'foo'), true);
            assert.strictEqual(test_object.HasOwn(obj, 'bar'), true);
            assert.strictEqual(test_object.HasOwn(obj, symbol1), true);
            assert.strictEqual(test_object.HasOwn(obj, 'baz'), false);
            assert.strictEqual(test_object.HasOwn(obj, 'toString'), false);
            assert.strictEqual(test_object.HasOwn(obj, symbol2), false);
        }

        {
            // test_object.Inflate increases all properties by 1
            const cube = {
                x: 10,
                y: 10,
                z: 10,
            };

            assert.deepEqual(test_object.Inflate(cube), { x: 11, y: 11, z: 11 });
            assert.deepEqual(test_object.Inflate(cube), { x: 12, y: 12, z: 12 });
            assert.deepEqual(test_object.Inflate(cube), { x: 13, y: 13, z: 13 });
            cube.t = 13;
            assert.deepEqual(
                test_object.Inflate(cube), { x: 14, y: 14, z: 14, t: 14 });

            const sym1 = Symbol('1');
            const sym2 = Symbol('2');
            const sym3 = Symbol('3');
            const sym4 = Symbol('4');
            const object2 = {
                [sym1]: '@@iterator',
                [sym2]: sym3,
            };

            assert(test_object.Has(object2, sym1));
            assert(test_object.Has(object2, sym2));
            assert.strictEqual(test_object.Get(object2, sym1), '@@iterator');
            assert.strictEqual(test_object.Get(object2, sym2), sym3);
            assert(test_object.Set(object2, 'string', 'value'));
            assert(test_object.SetNamed(object2, 'named_string', 'value'));
            assert(test_object.Set(object2, sym4, 123));
            assert(test_object.Has(object2, 'string'));
            assert(test_object.HasNamed(object2, 'named_string'));
            assert(test_object.Has(object2, sym4));
            assert.strictEqual(test_object.Get(object2, 'string'), 'value');
            assert.strictEqual(test_object.Get(object2, sym4), 123);
        }

        {
            // Wrap a pointer in a JS object, then verify the pointer can be unwrapped.
            const wrapper = {};
            test_object.Wrap(wrapper);

            assert(test_object.Unwrap(wrapper));
        }

        {
            // Verify that wrapping doesn't break an object's prototype chain.
            const wrapper = {};
            const protoA = { protoA: true };
            Object.setPrototypeOf(wrapper, protoA);
            test_object.Wrap(wrapper);

            assert(test_object.Unwrap(wrapper));
            assert(wrapper.protoA);
        }

        {
            // Verify the pointer can be unwrapped after inserting in the prototype chain.
            const wrapper = {};
            const protoA = { protoA: true };
            Object.setPrototypeOf(wrapper, protoA);
            test_object.Wrap(wrapper);

            const protoB = { protoB: true };
            Object.setPrototypeOf(protoB, Object.getPrototypeOf(wrapper));
            Object.setPrototypeOf(wrapper, protoB);

            assert(test_object.Unwrap(wrapper));
            assert(wrapper.protoA, true);
            assert(wrapper.protoB, true);
        }

        {
            // Verify that objects can be type-tagged and type-tag-checked.
            const obj1 = test_object.TypeTaggedInstance(0);
            const obj2 = test_object.TypeTaggedInstance(1);
            const obj3 = test_object.TypeTaggedInstance(2);
            const obj4 = test_object.TypeTaggedInstance(3);
            const external = test_object.TypeTaggedExternal(2);
            const plainExternal = test_object.PlainExternal();

            // Verify that we do not allow type tag indices greater than the largest
            // available index.
            assert.throws(() => test_object.TypeTaggedInstance(39), {
                name: 'RangeError',
                message: 'Invalid type index',
            });
            assert.throws(() => test_object.TypeTaggedExternal(39), {
                name: 'RangeError',
                message: 'Invalid type index',
            });

            // Verify that type tags are correctly accepted.
            assert.strictEqual(test_object.CheckTypeTag(0, obj1), true);
            assert.strictEqual(test_object.CheckTypeTag(1, obj2), true);
            assert.strictEqual(test_object.CheckTypeTag(2, obj3), true);
            assert.strictEqual(test_object.CheckTypeTag(3, obj4), true);
            assert.strictEqual(test_object.CheckTypeTag(2, external), true);

            // Verify that wrongly tagged objects are rejected.
            assert.strictEqual(test_object.CheckTypeTag(0, obj2), false);
            assert.strictEqual(test_object.CheckTypeTag(1, obj1), false);
            assert.strictEqual(test_object.CheckTypeTag(0, obj3), false);
            assert.strictEqual(test_object.CheckTypeTag(1, obj4), false);
            assert.strictEqual(test_object.CheckTypeTag(2, obj4), false);
            assert.strictEqual(test_object.CheckTypeTag(3, obj3), false);
            assert.strictEqual(test_object.CheckTypeTag(4, obj3), false);
            assert.strictEqual(test_object.CheckTypeTag(0, external), false);
            assert.strictEqual(test_object.CheckTypeTag(1, external), false);
            assert.strictEqual(test_object.CheckTypeTag(3, external), false);
            assert.strictEqual(test_object.CheckTypeTag(4, external), false);

            // Verify that untagged objects are rejected.
            assert.strictEqual(test_object.CheckTypeTag(0, {}), false);
            assert.strictEqual(test_object.CheckTypeTag(1, {}), false);
            assert.strictEqual(test_object.CheckTypeTag(0, plainExternal), false);
            assert.strictEqual(test_object.CheckTypeTag(1, plainExternal), false);
            assert.strictEqual(test_object.CheckTypeTag(2, plainExternal), false);
            assert.strictEqual(test_object.CheckTypeTag(3, plainExternal), false);
            assert.strictEqual(test_object.CheckTypeTag(4, plainExternal), false);
        }

        {
            // Verify that normal and nonexistent properties can be deleted.
            const sym = Symbol();
            const obj = { foo: 'bar', [sym]: 'baz' };

            assert.strictEqual('foo' in obj, true);
            assert.strictEqual(sym in obj, true);
            assert.strictEqual('does_not_exist' in obj, false);
            assert.strictEqual(test_object.Delete(obj, 'foo'), true);
            assert.strictEqual('foo' in obj, false);
            assert.strictEqual(sym in obj, true);
            assert.strictEqual('does_not_exist' in obj, false);
            assert.strictEqual(test_object.Delete(obj, sym), true);
            assert.strictEqual('foo' in obj, false);
            assert.strictEqual(sym in obj, false);
            assert.strictEqual('does_not_exist' in obj, false);
        }

        {
            // Verify that non-configurable properties are not deleted.
            const obj = {};

            Object.defineProperty(obj, 'foo', { configurable: false });
            assert.strictEqual(test_object.Delete(obj, 'foo'), false);
            assert.strictEqual('foo' in obj, true);
        }

        {
            // Verify that prototype properties are not deleted.
            function Foo() {
                this.foo = 'bar';
            }

            Foo.prototype.foo = 'baz';

            const obj = new Foo();

            assert.strictEqual(obj.foo, 'bar');
            assert.strictEqual(test_object.Delete(obj, 'foo'), true);
            assert.strictEqual(obj.foo, 'baz');
            assert.strictEqual(test_object.Delete(obj, 'foo'), true);
            assert.strictEqual(obj.foo, 'baz');
        }

        {
            // Verify that napi_get_property_names gets the right set of property names,
            // i.e.: includes prototypes, only enumerable properties, skips symbols,
            // and includes indices and converts them to strings.

            const object = {
                __proto__: {
                    inherited: 1,
                }
            };

            const fooSymbol = Symbol('foo');

            object.normal = 2;
            object[fooSymbol] = 3;
            Object.defineProperty(object, 'unenumerable', {
                value: 4,
                enumerable: false,
                writable: true,
                configurable: true,
            });
            Object.defineProperty(object, 'writable', {
                value: 4,
                enumerable: true,
                writable: true,
                configurable: false,
            });
            Object.defineProperty(object, 'configurable', {
                value: 4,
                enumerable: true,
                writable: false,
                configurable: true,
            });
            object[5] = 5;

            assert.deepEqual(test_object.GetPropertyNames(object),
                ['5',
                    'normal',
                    'writable',
                    'configurable',
                    'inherited']);

            assert.deepEqual(test_object.GetSymbolNames(object),
                [fooSymbol]);

            assert.deepEqual(test_object.GetEnumerableWritableNames(object),
                ['5',
                    'normal',
                    'writable',
                    fooSymbol,
                    'inherited']);

            assert.deepEqual(test_object.GetOwnWritableNames(object),
                ['5',
                    'normal',
                    'unenumerable',
                    'writable',
                    fooSymbol]);

            assert.deepEqual(test_object.GetEnumerableConfigurableNames(object),
                ['5',
                    'normal',
                    'configurable',
                    fooSymbol,
                    'inherited']);

            assert.deepEqual(test_object.GetOwnConfigurableNames(object),
                ['5',
                    'normal',
                    'unenumerable',
                    'configurable',
                    fooSymbol]);
        }

        // Verify that passing NULL to napi_set_property() results in the correct
        // error.
        assert.deepEqual(test_object.TestSetProperty(), {
            envIsNull: 'Invalid argument',
            objectIsNull: 'Invalid argument',
            keyIsNull: 'Invalid argument',
            valueIsNull: 'Invalid argument',
        });

        // Verify that passing NULL to napi_has_property() results in the correct
        // error.
        assert.deepEqual(test_object.TestHasProperty(), {
            envIsNull: 'Invalid argument',
            objectIsNull: 'Invalid argument',
            keyIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
        });

        // Verify that passing NULL to napi_get_property() results in the correct
        // error.
        assert.deepEqual(test_object.TestGetProperty(), {
            envIsNull: 'Invalid argument',
            objectIsNull: 'Invalid argument',
            keyIsNull: 'Invalid argument',
            resultIsNull: 'Invalid argument',
        });

        {
            const obj = { x: 'a', y: 'b', z: 'c' };

            test_object.TestSeal(obj);

            assert.strictEqual(Object.isSealed(obj), true);

            // assert.throws(() => {
            //     obj.w = 'd';
            // }, /Cannot add property w, object is not extensible/);

            // assert.throws(() => {
            //     delete obj.x;
            // }, /Cannot delete property 'x' of #<Object>/);

            // Sealed objects allow updating existing properties,
            // so this should not throw.
            obj.x = 'd';
        }

        {
            const obj = { x: 10, y: 10, z: 10 };

            test_object.TestFreeze(obj);

            assert.strictEqual(Object.isFrozen(obj), true);

            // assert.throws(() => {
            //     obj.x = 10;
            // }, /Cannot assign to read only property 'x' of object '#<Object>/);

            // assert.throws(() => {
            //     obj.w = 15;
            // }, /Cannot add property w, object is not extensible/);

            // assert.throws(() => {
            //     delete obj.x;
            // }, /Cannot delete property 'x' of #<Object>/);
        }
    });

    it('test_promise', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_promise.node'));
        const test_promise = module.exports;
        var ev;
        var test_result;

        // A resolution
        {
            test_result = {};
            ev = new coroutine.Event();

            const expected_result = 42;
            const promise = test_promise.createPromise();
            promise.then(
                function (result) {
                    test_result.result = result;
                    ev.set();
                },
                function () {
                    test_result.rejected = true;
                    ev.set();
                }
            );
            test_promise.concludeCurrentPromise(expected_result, true);
            ev.wait();
            assert.equal(test_result.result, expected_result);
            assert.equal(test_result.rejected, undefined);
        }

        // A rejection
        {
            test_result = {};
            ev = new coroutine.Event();

            const expected_result = 'It\'s not you, it\'s me.';
            const promise = test_promise.createPromise();
            promise.then(
                function (result) {
                    test_result.result = result;
                    ev.set();
                },
                function (result) {
                    test_result.rejected = result;
                    ev.set();
                }
            );
            test_promise.concludeCurrentPromise(expected_result, false);

            ev.wait();
            assert.equal(test_result.result, undefined);
            assert.equal(test_result.rejected, expected_result);
        }

        // Chaining
        {
            test_result = {};
            ev = new coroutine.Event();

            const expected_result = 'chained answer';
            const promise = test_promise.createPromise();
            promise.then(
                function (result) {
                    test_result.result = result;
                    ev.set();
                },
                function (result) {
                    test_result.rejected = result;
                    ev.set();
                }
            );
            test_promise.concludeCurrentPromise(Promise.resolve('chained answer'), true);

            ev.wait();
            assert.equal(test_result.result, expected_result);
            assert.equal(test_result.rejected, undefined);
        }

        const promiseTypeTestPromise = test_promise.createPromise();
        assert.strictEqual(test_promise.isPromise(promiseTypeTestPromise), true);
        test_promise.concludeCurrentPromise(undefined, true);

        const rejectPromise = Promise.reject(-1);
        const expected_reason = -1;
        assert.strictEqual(test_promise.isPromise(rejectPromise), true);
        rejectPromise.catch((reason) => {
            assert.strictEqual(reason, expected_reason);
        });

        assert.strictEqual(test_promise.isPromise(2.4), false);
        assert.strictEqual(test_promise.isPromise('I promise!'), false);
        assert.strictEqual(test_promise.isPromise(undefined), false);
        assert.strictEqual(test_promise.isPromise(null), false);
        assert.strictEqual(test_promise.isPromise({}), false);
    });

    it('test_properties', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_properties.node'));
        const test_object = module.exports;

        assert.strictEqual(test_object.echo('hello'), 'hello');

        test_object.readwriteValue = 1;
        assert.strictEqual(test_object.readwriteValue, 1);
        test_object.readwriteValue = 2;
        assert.strictEqual(test_object.readwriteValue, 2);

        // assert.throws(() => { test_object.readonlyValue = 3; });

        assert.ok(test_object.hiddenValue);

        // Properties with napi_enumerable attribute should be enumerable.
        const propertyNames = [];
        for (const name in test_object) {
            propertyNames.push(name);
        }
        assert.ok(propertyNames.includes('echo'));
        assert.ok(propertyNames.includes('readwriteValue'));
        assert.ok(propertyNames.includes('readonlyValue'));
        assert.ok(!propertyNames.includes('hiddenValue'));
        assert.ok(propertyNames.includes('NameKeyValue'));
        assert.ok(!propertyNames.includes('readwriteAccessor1'));
        assert.ok(!propertyNames.includes('readwriteAccessor2'));
        assert.ok(!propertyNames.includes('readonlyAccessor1'));
        assert.ok(!propertyNames.includes('readonlyAccessor2'));

        // Validate properties created with symbol
        const propertySymbols = Object.getOwnPropertySymbols(test_object);
        assert.strictEqual(propertySymbols[0].toString(), 'Symbol(NameKeySymbol)');
        assert.strictEqual(propertySymbols[1].toString(), 'Symbol()');
        assert.strictEqual(propertySymbols[2], Symbol.for('NameKeySymbolFor'));

        // The napi_writable attribute should be ignored for accessors.
        const readwriteAccessor1Descriptor =
            Object.getOwnPropertyDescriptor(test_object, 'readwriteAccessor1');
        const readonlyAccessor1Descriptor =
            Object.getOwnPropertyDescriptor(test_object, 'readonlyAccessor1');
        assert.ok(readwriteAccessor1Descriptor.get != null);
        assert.ok(readwriteAccessor1Descriptor.set != null);
        assert.ok(readwriteAccessor1Descriptor.value === undefined);
        assert.ok(readonlyAccessor1Descriptor.get != null);
        assert.ok(readonlyAccessor1Descriptor.set === undefined);
        assert.ok(readonlyAccessor1Descriptor.value === undefined);
        test_object.readwriteAccessor1 = 1;
        assert.strictEqual(test_object.readwriteAccessor1, 1);
        assert.strictEqual(test_object.readonlyAccessor1, 1);
        // assert.throws(() => { test_object.readonlyAccessor1 = 3; }, getterOnlyErrorRE);
        test_object.readwriteAccessor2 = 2;
        assert.strictEqual(test_object.readwriteAccessor2, 2);
        assert.strictEqual(test_object.readonlyAccessor2, 2);
        // assert.throws(() => { test_object.readonlyAccessor2 = 3; }, getterOnlyErrorRE);

        assert.strictEqual(test_object.hasNamedProperty(test_object, 'echo'), true);
        assert.strictEqual(test_object.hasNamedProperty(test_object, 'hiddenValue'),
            true);
        assert.strictEqual(test_object.hasNamedProperty(test_object, 'doesnotexist'),
            false);
    });

    it('test_reference', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_reference.node'));
        const test_reference = module.exports;


        // This test script uses external values with finalizer callbacks
        // in order to track when values get garbage-collected. Each invocation
        // of a finalizer callback increments the finalizeCount property.
        assert.strictEqual(test_reference.finalizeCount, 0);

        // Run each test function in sequence,
        // with an async delay and GC call between each.
        async function runTests() {
            (() => {
                const symbol = test_reference.createSymbol('testSym');
                test_reference.createReference(symbol, 0);
                assert.strictEqual(test_reference.referenceValue, symbol);
            })();
            test_reference.deleteReference();

            (() => {
                const symbol = test_reference.createSymbolFor('testSymFor');
                test_reference.createReference(symbol, 0);
                assert.strictEqual(test_reference.referenceValue, symbol);
            })();
            test_reference.deleteReference();

            (() => {
                const symbol = test_reference.createSymbolFor('testSymFor');
                test_reference.createReference(symbol, 1);
                assert.strictEqual(test_reference.referenceValue, symbol);
                assert.strictEqual(test_reference.referenceValue, Symbol.for('testSymFor'));
            })();
            test_reference.deleteReference();

            (() => {
                const symbol = test_reference.createSymbolForEmptyString();
                test_reference.createReference(symbol, 0);
                assert.strictEqual(test_reference.referenceValue, Symbol.for(''));
            })();
            test_reference.deleteReference();

            (() => {
                const symbol = test_reference.createSymbolForEmptyString();
                test_reference.createReference(symbol, 1);
                assert.strictEqual(test_reference.referenceValue, symbol);
                assert.strictEqual(test_reference.referenceValue, Symbol.for(''));
            })();
            test_reference.deleteReference();

            assert.throws(() => test_reference.createSymbolForIncorrectLength(),
                /Invalid argument/);

            (() => {
                const value = test_reference.createExternal();
                assert.strictEqual(test_reference.finalizeCount, 0);
                assert.strictEqual(typeof value, 'object');
                test_reference.checkExternal(value);
            })();
            // await gcUntil('External value without a finalizer',
            //     () => (test_reference.finalizeCount === 0));

            (() => {
                const value = test_reference.createExternalWithFinalize();
                assert.strictEqual(test_reference.finalizeCount, 0);
                assert.strictEqual(typeof value, 'object');
                test_reference.checkExternal(value);
            })();
            // await gcUntil('External value with a finalizer',
            //     () => (test_reference.finalizeCount === 1));

            (() => {
                const value = test_reference.createExternalWithFinalize();
                assert.strictEqual(test_reference.finalizeCount, 0);
                test_reference.createReference(value, 0);
                assert.strictEqual(test_reference.referenceValue, value);
            })();
            // Value should be GC'd because there is only a weak ref
            // await gcUntil('Weak reference',
            //     () => (test_reference.referenceValue === undefined &&
            //         test_reference.finalizeCount === 1));
            test_reference.deleteReference();

            (() => {
                const value = test_reference.createExternalWithFinalize();
                assert.strictEqual(test_reference.finalizeCount, 0);
                test_reference.createReference(value, 1);
                assert.strictEqual(test_reference.referenceValue, value);
            })();
            // Value should NOT be GC'd because there is a strong ref
            // await gcUntil('Strong reference',
            //     () => (test_reference.finalizeCount === 0));
            test_reference.deleteReference();
            // await gcUntil('Strong reference (cont.d)',
            //     () => (test_reference.finalizeCount === 1));

            (() => {
                const value = test_reference.createExternalWithFinalize();
                assert.strictEqual(test_reference.finalizeCount, 0);
                test_reference.createReference(value, 1);
            })();
            // Value should NOT be GC'd because there is a strong ref
            // await gcUntil('Strong reference, increment then decrement to weak reference',
            //     () => (test_reference.finalizeCount === 0));
            assert.strictEqual(test_reference.incrementRefcount(), 2);
            // Value should NOT be GC'd because there is a strong ref
            // await gcUntil(
            //     'Strong reference, increment then decrement to weak reference (cont.d-1)',
            //     () => (test_reference.finalizeCount === 0));
            assert.strictEqual(test_reference.decrementRefcount(), 1);
            // Value should NOT be GC'd because there is a strong ref
            // await gcUntil(
            //     'Strong reference, increment then decrement to weak reference (cont.d-2)',
            //     () => (test_reference.finalizeCount === 0));
            assert.strictEqual(test_reference.decrementRefcount(), 0);
            // Value should be GC'd because the ref is now weak!
            // await gcUntil(
            //     'Strong reference, increment then decrement to weak reference (cont.d-3)',
            //     () => (test_reference.finalizeCount === 1));
            test_reference.deleteReference();
            // Value was already GC'd
            // await gcUntil(
            //     'Strong reference, increment then decrement to weak reference (cont.d-4)',
            //     () => (test_reference.finalizeCount === 1));
        }
        runTests();

        // This test creates a napi_ref on an object that has
        // been wrapped by napi_wrap and for which the finalizer
        // for the wrap calls napi_delete_ref on that napi_ref.
        //
        // Since both the wrap and the reference use the same
        // object the finalizer for the wrap and reference
        // may run in the same gc and in any order.
        //
        // It does that to validate that napi_delete_ref can be
        // called before the finalizer has been run for the
        // reference (there is a finalizer behind the scenes even
        // though it cannot be passed to napi_create_reference).
        //
        // Since the order is not guarranteed, run the
        // test a number of times maximize the chance that we
        // get a run with the desired order for the test.
        //
        // 1000 reliably recreated the problem without the fix
        // required to ensure delete could be called before
        // the finalizer in manual testing.
        for (let i = 0; i < 1000; i++) {
            const wrapObject = new Object();
            test_reference.validateDeleteBeforeFinalize(wrapObject);
            global.GC();
        }
    });

    it('test_string', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_string.node'));
        const test_string = module.exports;

        const empty = '';
        assert.strictEqual(test_string.TestLatin1(empty), empty);
        assert.strictEqual(test_string.TestUtf8(empty), empty);
        assert.strictEqual(test_string.TestUtf16(empty), empty);
        assert.strictEqual(test_string.TestLatin1AutoLength(empty), empty);
        assert.strictEqual(test_string.TestUtf8AutoLength(empty), empty);
        assert.strictEqual(test_string.TestUtf16AutoLength(empty), empty);
        assert.strictEqual(test_string.TestLatin1External(empty), empty);
        assert.strictEqual(test_string.TestUtf16External(empty), empty);
        assert.strictEqual(test_string.TestLatin1ExternalAutoLength(empty), empty);
        assert.strictEqual(test_string.TestUtf16ExternalAutoLength(empty), empty);
        assert.strictEqual(test_string.Utf16Length(empty), 0);
        assert.strictEqual(test_string.Utf8Length(empty), 0);

        const str1 = 'hello world';
        assert.strictEqual(test_string.TestLatin1(str1), str1);
        assert.strictEqual(test_string.TestUtf8(str1), str1);
        assert.strictEqual(test_string.TestUtf16(str1), str1);
        assert.strictEqual(test_string.TestLatin1AutoLength(str1), str1);
        assert.strictEqual(test_string.TestUtf8AutoLength(str1), str1);
        assert.strictEqual(test_string.TestUtf16AutoLength(str1), str1);
        assert.strictEqual(test_string.TestLatin1External(str1), str1);
        assert.strictEqual(test_string.TestUtf16External(str1), str1);
        assert.strictEqual(test_string.TestLatin1ExternalAutoLength(str1), str1);
        assert.strictEqual(test_string.TestUtf16ExternalAutoLength(str1), str1);
        assert.strictEqual(test_string.TestLatin1Insufficient(str1), str1.slice(0, 3));
        assert.strictEqual(test_string.TestUtf8Insufficient(str1), str1.slice(0, 3));
        assert.strictEqual(test_string.TestUtf16Insufficient(str1), str1.slice(0, 3));
        assert.strictEqual(test_string.Utf16Length(str1), 11);
        assert.strictEqual(test_string.Utf8Length(str1), 11);

        const str2 = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
        assert.strictEqual(test_string.TestLatin1(str2), str2);
        assert.strictEqual(test_string.TestUtf8(str2), str2);
        assert.strictEqual(test_string.TestUtf16(str2), str2);
        assert.strictEqual(test_string.TestLatin1AutoLength(str2), str2);
        assert.strictEqual(test_string.TestUtf8AutoLength(str2), str2);
        assert.strictEqual(test_string.TestUtf16AutoLength(str2), str2);
        assert.strictEqual(test_string.TestLatin1External(str2), str2);
        assert.strictEqual(test_string.TestUtf16External(str2), str2);
        assert.strictEqual(test_string.TestLatin1ExternalAutoLength(str2), str2);
        assert.strictEqual(test_string.TestUtf16ExternalAutoLength(str2), str2);
        assert.strictEqual(test_string.TestLatin1Insufficient(str2), str2.slice(0, 3));
        assert.strictEqual(test_string.TestUtf8Insufficient(str2), str2.slice(0, 3));
        assert.strictEqual(test_string.TestUtf16Insufficient(str2), str2.slice(0, 3));
        assert.strictEqual(test_string.Utf16Length(str2), 62);
        assert.strictEqual(test_string.Utf8Length(str2), 62);

        const str3 = '?!@#$%^&*()_+-=[]{}/.,<>\'"\\';
        assert.strictEqual(test_string.TestLatin1(str3), str3);
        assert.strictEqual(test_string.TestUtf8(str3), str3);
        assert.strictEqual(test_string.TestUtf16(str3), str3);
        assert.strictEqual(test_string.TestLatin1AutoLength(str3), str3);
        assert.strictEqual(test_string.TestUtf8AutoLength(str3), str3);
        assert.strictEqual(test_string.TestUtf16AutoLength(str3), str3);
        assert.strictEqual(test_string.TestLatin1External(str3), str3);
        assert.strictEqual(test_string.TestUtf16External(str3), str3);
        assert.strictEqual(test_string.TestLatin1ExternalAutoLength(str3), str3);
        assert.strictEqual(test_string.TestUtf16ExternalAutoLength(str3), str3);
        assert.strictEqual(test_string.TestLatin1Insufficient(str3), str3.slice(0, 3));
        assert.strictEqual(test_string.TestUtf8Insufficient(str3), str3.slice(0, 3));
        assert.strictEqual(test_string.TestUtf16Insufficient(str3), str3.slice(0, 3));
        assert.strictEqual(test_string.Utf16Length(str3), 27);
        assert.strictEqual(test_string.Utf8Length(str3), 27);

        const str4 = '¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿';
        assert.strictEqual(test_string.TestLatin1(str4), str4);
        assert.strictEqual(test_string.TestUtf8(str4), str4);
        assert.strictEqual(test_string.TestUtf16(str4), str4);
        assert.strictEqual(test_string.TestLatin1AutoLength(str4), str4);
        assert.strictEqual(test_string.TestUtf8AutoLength(str4), str4);
        assert.strictEqual(test_string.TestUtf16AutoLength(str4), str4);
        assert.strictEqual(test_string.TestLatin1External(str4), str4);
        assert.strictEqual(test_string.TestUtf16External(str4), str4);
        assert.strictEqual(test_string.TestLatin1ExternalAutoLength(str4), str4);
        assert.strictEqual(test_string.TestUtf16ExternalAutoLength(str4), str4);
        assert.strictEqual(test_string.TestLatin1Insufficient(str4), str4.slice(0, 3));
        assert.strictEqual(test_string.TestUtf8Insufficient(str4), str4.slice(0, 1));
        assert.strictEqual(test_string.TestUtf16Insufficient(str4), str4.slice(0, 3));
        assert.strictEqual(test_string.Utf16Length(str4), 31);
        assert.strictEqual(test_string.Utf8Length(str4), 62);

        const str5 = 'ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþ';
        assert.strictEqual(test_string.TestLatin1(str5), str5);
        assert.strictEqual(test_string.TestUtf8(str5), str5);
        assert.strictEqual(test_string.TestUtf16(str5), str5);
        assert.strictEqual(test_string.TestLatin1AutoLength(str5), str5);
        assert.strictEqual(test_string.TestUtf8AutoLength(str5), str5);
        assert.strictEqual(test_string.TestUtf16AutoLength(str5), str5);
        assert.strictEqual(test_string.TestLatin1External(str5), str5);
        assert.strictEqual(test_string.TestUtf16External(str5), str5);
        assert.strictEqual(test_string.TestLatin1ExternalAutoLength(str5), str5);
        assert.strictEqual(test_string.TestUtf16ExternalAutoLength(str5), str5);
        assert.strictEqual(test_string.TestLatin1Insufficient(str5), str5.slice(0, 3));
        assert.strictEqual(test_string.TestUtf8Insufficient(str5), str5.slice(0, 1));
        assert.strictEqual(test_string.TestUtf16Insufficient(str5), str5.slice(0, 3));
        assert.strictEqual(test_string.Utf16Length(str5), 63);
        assert.strictEqual(test_string.Utf8Length(str5), 126);

        const str6 = '\u{2003}\u{2101}\u{2001}\u{202}\u{2011}';
        assert.strictEqual(test_string.TestUtf8(str6), str6);
        assert.strictEqual(test_string.TestUtf16(str6), str6);
        assert.strictEqual(test_string.TestUtf8AutoLength(str6), str6);
        assert.strictEqual(test_string.TestUtf16AutoLength(str6), str6);
        assert.strictEqual(test_string.TestUtf16External(str6), str6);
        assert.strictEqual(test_string.TestUtf16ExternalAutoLength(str6), str6);
        assert.strictEqual(test_string.TestUtf8Insufficient(str6), str6.slice(0, 1));
        assert.strictEqual(test_string.TestUtf16Insufficient(str6), str6.slice(0, 3));
        assert.strictEqual(test_string.Utf16Length(str6), 5);
        assert.strictEqual(test_string.Utf8Length(str6), 14);

        assert.throws(() => {
            test_string.TestLargeUtf8();
        }, /^Error: Invalid argument$/);

        assert.throws(() => {
            test_string.TestLargeLatin1();
        }, /^Error: Invalid argument$/);

        assert.throws(() => {
            test_string.TestLargeUtf16();
        }, /^Error: Invalid argument$/);

        test_string.TestMemoryCorruption(' '.repeat(64 * 1024));
    });

    it('test_symbol', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_symbol.node'));
        const test_symbol = module.exports;

        const sym = test_symbol.New('test');
        assert.strictEqual(sym.toString(), 'Symbol(test)');

        {
            const myObj = {};
            const fooSym = test_symbol.New('foo');
            const otherSym = test_symbol.New('bar');
            myObj.foo = 'bar';
            myObj[fooSym] = 'baz';
            myObj[otherSym] = 'bing';
            assert.strictEqual(myObj.foo, 'bar');
            assert.strictEqual(myObj[fooSym], 'baz');
            assert.strictEqual(myObj[otherSym], 'bing');
        }

        {
            const fooSym = test_symbol.New('foo');
            assert.strictEqual(fooSym.toString(), 'Symbol(foo)');

            const myObj = {};
            myObj.foo = 'bar';
            myObj[fooSym] = 'baz';

            assert.deepEqual(Object.keys(myObj), ['foo']);
            assert.deepEqual(Object.getOwnPropertyNames(myObj), ['foo']);
            assert.deepEqual(Object.getOwnPropertySymbols(myObj), [fooSym]);
        }

        {
            assert.notStrictEqual(test_symbol.New(), test_symbol.New());
            assert.notStrictEqual(test_symbol.New('foo'), test_symbol.New('foo'));
            assert.notStrictEqual(test_symbol.New('foo'), test_symbol.New('bar'));

            const foo1 = test_symbol.New('foo');
            const foo2 = test_symbol.New('foo');
            const object = {
                [foo1]: 1,
                [foo2]: 2,
            };
            assert.strictEqual(object[foo1], 1);
            assert.strictEqual(object[foo2], 2);
        }
    });

    it('test_typedarray', () => {
        var module = {
            exports: {}
        }
        process.dlopen(module, path.join(bin_path, 'test_typedarray.node'));
        const test_typedarray = module.exports;

        const byteArray = new Uint8Array(3);
        byteArray[0] = 0;
        byteArray[1] = 1;
        byteArray[2] = 2;
        assert.strictEqual(byteArray.length, 3);

        const doubleArray = new Float64Array(3);
        doubleArray[0] = 0.0;
        doubleArray[1] = 1.1;
        doubleArray[2] = 2.2;
        assert.strictEqual(doubleArray.length, 3);

        const byteResult = test_typedarray.Multiply(byteArray, 3);
        assert.ok(byteResult instanceof Uint8Array);
        assert.strictEqual(byteResult.length, 3);
        assert.strictEqual(byteResult[0], 0);
        assert.strictEqual(byteResult[1], 3);
        assert.strictEqual(byteResult[2], 6);

        const doubleResult = test_typedarray.Multiply(doubleArray, -3);
        assert.ok(doubleResult instanceof Float64Array);
        assert.strictEqual(doubleResult.length, 3);
        assert.strictEqual(doubleResult[0], -0);
        assert.strictEqual(Math.round(10 * doubleResult[1]) / 10, -3.3);
        assert.strictEqual(Math.round(10 * doubleResult[2]) / 10, -6.6);

        const externalResult = test_typedarray.External();
        assert.ok(externalResult instanceof Int8Array);
        assert.strictEqual(externalResult.length, 3);
        assert.strictEqual(externalResult[0], 0);
        assert.strictEqual(externalResult[1], 1);
        assert.strictEqual(externalResult[2], 2);

        // Validate creation of all kinds of TypedArrays
        const buffer = new ArrayBuffer(128);
        const arrayTypes = [Int8Array, Uint8Array, Uint8ClampedArray, Int16Array,
            Uint16Array, Int32Array, Uint32Array, Float32Array,
            Float64Array, BigInt64Array, BigUint64Array];

        arrayTypes.forEach((currentType) => {
            const template = Reflect.construct(currentType, buffer);
            const theArray = test_typedarray.CreateTypedArray(template, buffer);

            assert.ok(theArray instanceof currentType,
                'Type of new array should match that of the template. ' +
                `Expected type: ${currentType.name}, ` +
                `actual type: ${template.constructor.name}`);
            assert.notStrictEqual(theArray, template);
            assert.strictEqual(theArray.buffer, buffer);
        });

        arrayTypes.forEach((currentType) => {
            const template = Reflect.construct(currentType, buffer);
            assert.throws(() => {
                test_typedarray.CreateTypedArray(template, buffer, 0, 136);
            }, RangeError);
        });

        const nonByteArrayTypes = [Int16Array, Uint16Array, Int32Array, Uint32Array,
            Float32Array, Float64Array,
            BigInt64Array, BigUint64Array];
        nonByteArrayTypes.forEach((currentType) => {
            const template = Reflect.construct(currentType, buffer);
            assert.throws(() => {
                test_typedarray.CreateTypedArray(template, buffer,
                    currentType.BYTES_PER_ELEMENT + 1, 1);
                console.log(`start of offset ${currentType}`);
            }, RangeError);
        });

        // Test detaching
        arrayTypes.forEach((currentType) => {
            const buffer = Reflect.construct(currentType, [8]);
            assert.strictEqual(buffer.length, 8);
            assert.ok(!test_typedarray.IsDetached(buffer.buffer));
            test_typedarray.Detach(buffer);
            assert.ok(test_typedarray.IsDetached(buffer.buffer));
            assert.strictEqual(buffer.length, 0);
        });
        {
            const buffer = test_typedarray.External();
            assert.ok(externalResult instanceof Int8Array);
            assert.strictEqual(externalResult.length, 3);
            assert.strictEqual(externalResult.byteLength, 3);
            assert.ok(!test_typedarray.IsDetached(buffer.buffer));
            test_typedarray.Detach(buffer);
            assert.ok(test_typedarray.IsDetached(buffer.buffer));
            assert.ok(externalResult instanceof Int8Array);
            assert.strictEqual(buffer.length, 0);
            assert.strictEqual(buffer.byteLength, 0);
        }

        {
            const buffer = new ArrayBuffer(128);
            assert.ok(!test_typedarray.IsDetached(buffer));
        }

        {
            const buffer = test_typedarray.NullArrayBuffer();
            assert.ok(buffer instanceof ArrayBuffer);
            // assert.ok(test_typedarray.IsDetached(buffer));
        }
    });
});

require.main === module && test.run(console.DEBUG);
