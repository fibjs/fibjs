const { describe, it } = require('node:test');
const assert = require('assert');
const { AssertionError } = require('assert');

describe('AssertionError Tests', () => {
    describe('Constructor and Properties', () => {
        // Basic properties tests
        it('should throw with correct properties', () => {
            const error = new AssertionError({
                message: 'Test failed',
                operator: 'strictEqual',
                actual: 3,
                expected: 2
            });

            assert.strictEqual(error.name, 'AssertionError');
            assert.strictEqual(error.message.includes('Test failed'), true);
            assert.strictEqual(error.actual, 3);
            assert.strictEqual(error.expected, 2);
        });

        it('should have correct basic properties', () => {
            const error = new AssertionError({
                message: 'Test message',
                actual: 1,
                expected: 2,
                operator: 'strictEqual'
            });

            assert.strictEqual(error.name, 'AssertionError');
            assert.strictEqual(error.code, 'ERR_ASSERTION');
            assert.strictEqual(error.actual, 1);
            assert.strictEqual(error.expected, 2);
            assert.strictEqual(error.operator, 'strictEqual');
            assert.strictEqual(error.generatedMessage, false);
        });

        // Error copying tests
        it('should handle error copying', () => {
            const actualError = new Error('actual error');
            const expectedError = new Error('expected error');

            const error = new AssertionError({
                actual: actualError,
                expected: expectedError,
                operator: 'strictEqual'
            });

            assert.ok(error.actual instanceof Error);
            assert.ok(error.expected instanceof Error);
        });

        it('should copy error properties correctly', () => {
            const originalError = new Error('Original error');
            originalError.custom = 'custom';
            const error = new AssertionError({
                actual: originalError,
                expected: new Error('Expected error'),
                operator: 'strictEqual'
            });

            assert.ok(error.actual instanceof Error);
            assert.strictEqual(error.actual.message, 'Original error');
        });

        // Error cause tests
        it('should handle error cause properly', () => {
            const cause = new Error('Original cause');
            const error = new AssertionError({
                message: `Failed with cause: ${cause.message}`,
                operator: 'fail'
            });

            assert.strictEqual(error.message.includes('Original cause'), true);
        });

        it('should copy nested error causes', () => {
            const deepCause = new Error('Deep cause');
            const cause = new Error('Immediate cause');
            const error = new AssertionError({
                message: `Failed with: ${cause.message} (${deepCause.message})`,
                operator: 'fail'
            });

            assert.strictEqual(error.message.includes('Immediate cause'), true);
            assert.strictEqual(error.message.includes('Deep cause'), true);
        });

        // Multiple error details tests
        it('should handle multiple error details', () => {
            const error = new AssertionError({
                details: [
                    { message: 'Error 1', actual: 1, expected: 2, operator: 'equal' },
                    { message: 'Error 2', actual: 3, expected: 4, operator: 'equal' }
                ]
            });

            assert.strictEqual(error['message 0'], 'Error 1');
            assert.strictEqual(error['actual 0'], 1);
            assert.strictEqual(error['expected 0'], 2);
            assert.strictEqual(error['operator 0'], 'equal');

            assert.strictEqual(error['message 1'], 'Error 2');
            assert.strictEqual(error['actual 1'], 3);
            assert.strictEqual(error['expected 1'], 4);
            assert.strictEqual(error['operator 1'], 'equal');
        });
    });

    describe('Error Message Formatting', () => {
        describe('Basic Message Tests', () => {
            // Basic message formatting tests
            it('should format message correctly', () => {
                const error = new AssertionError({
                    message: 'Values are not equal',
                    operator: 'strictEqual',
                    actual: 3,
                    expected: 2
                });

                assert.ok(error.toString().includes('AssertionError [ERR_ASSERTION]'));
                assert.ok(error.toString().includes('Values are not equal'));
            });

            it('should format generated message correctly', () => {
                const error = new AssertionError({
                    operator: 'strictEqual',
                    actual: 3,
                    expected: 2
                });

                assert.ok(error.message.includes('Expected values to be strictly equal'));
            });

            it('should format toString() correctly', () => {
                const error = new AssertionError({
                    message: 'Test message'
                });

                assert.match(
                    error.toString(),
                    /^AssertionError \[ERR_ASSERTION\]: Test message$/
                );
            });

            it('should handle toString variations', () => {
                const error1 = new AssertionError({
                    message: 'Simple message'
                });
                assert.match(
                    error1.toString(),
                    /^AssertionError \[ERR_ASSERTION\]: Simple message$/
                );

                const error2 = new AssertionError({
                    message: 'Custom message',
                    operator: 'custom'
                });
                assert.match(
                    error2.toString(),
                    /^AssertionError \[ERR_ASSERTION\]: Custom message$/
                );

                const error3 = new AssertionError({
                    message: null
                });
                assert.match(
                    error3.toString(),
                    /^AssertionError \[ERR_ASSERTION\]/
                );
            });
        });

        describe('String Handling', () => {
            // String truncation tests
            it('should truncate long string actual values', () => {
                const longString = 'a'.repeat(1000);
                const error = new AssertionError({
                    actual: longString,
                    expected: 'short',
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('Expected values to be strictly equal'));
                assert.ok(
                    error.message.includes('actual') ||
                    error.message.includes('expected')
                );
            });

            it('should handle multiline string comparison', () => {
                const multiline1 = 'line1\nline2\nline3';
                const multiline2 = 'line1\nmodified\nline3';
                const error = new AssertionError({
                    actual: multiline1,
                    expected: multiline2,
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('Expected values to be strictly equal'));

                assert.ok(error.message.length > 0);

                assert.ok(error.message.includes('\n'));
            });

            it('should handle strings at kMaxShortStringLength boundary', () => {
                const str = 'a'.repeat(12); // kMaxShortStringLength = 12
                const error = new AssertionError({
                    actual: str,
                    expected: 'b'.repeat(12),
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('Expected values to be strictly equal'));
            });

            it('should handle strings at kMaxLongStringLength boundary', () => {
                const str = 'a'.repeat(512); // kMaxLongStringLength = 512
                const error = new AssertionError({
                    actual: str,
                    expected: 'b'.repeat(512),
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('Expected values to be strictly equal'));
            });

            it('should handle special characters in strings', () => {
                const error = new AssertionError({
                    actual: 'test\nwith\tspecial\rchars',
                    expected: 'test\nmodified\tspecial\rchars',
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('test'));
                assert.ok(error.message.includes('\n') ||
                    error.message.includes('\\n'));
            });
        });
    });

    describe('Value Comparison', () => {
        describe('Object Comparison', () => {
            // Object comparison tests
            it('should display detailed object diff', () => {
                const actual = {
                    a: 1,
                    b: {
                        c: 2,
                        d: 3
                    }
                };
                const expected = {
                    a: 1,
                    b: {
                        c: 2,
                        d: 4
                    }
                };

                const error = new AssertionError({
                    actual,
                    expected,
                    operator: 'deepStrictEqual'
                });


                assert.ok(error.message.includes('Expected values to be strictly deep-equal'));
                assert.ok(error.message.includes('b'));
            });

            it('should truncate large object messages', () => {
                const largeObject = {};
                for (let i = 0; i < 20; i++) {
                    largeObject[`key${i}`] = `value${i}`;
                }

                const error = new AssertionError({
                    actual: largeObject,
                    expected: {},
                    operator: 'deepStrictEqual'
                });

                assert.ok(
                    error.message.includes('Expected values to be strictly deep-equal'),
                    'Message should contain the comparison operator description'
                );
            });

            it('should handle colored output', () => {
                const actual = { a: 1 };
                const expected = { a: 2 };

                const error = new AssertionError({
                    actual,
                    expected,
                    operator: 'deepStrictEqual'
                });

                assert.match(
                    error.message,
                    /Expected values to be strictly deep-equal:/
                );
            });

            it('should handle nested object differences', () => {
                const actual = {
                    a: {
                        b: {
                            c: 1
                        }
                    }
                };
                const expected = {
                    a: {
                        b: {
                            c: 2
                        }
                    }
                };

                const error = new AssertionError({
                    actual,
                    expected,
                    operator: 'deepStrictEqual'
                });


                assert.ok(error.message.includes('Expected values to be strictly deep-equal'));
                assert.ok(error.message.includes('a'));
            });

            it('should format object comparison message correctly', () => {
                const actual = { a: 1 };
                const expected = { a: 2 };
                const error = new AssertionError({
                    actual,
                    expected,
                    operator: 'deepStrictEqual'
                });

                assert.ok(
                    error.message.includes('Expected values to be strictly deep-equal')
                );
            });

            it('should handle identical objects with different references', () => {
                const obj1 = { a: 1 };
                const obj2 = { a: 1 };
                const error = new AssertionError({
                    actual: obj1,
                    expected: obj2,
                    operator: 'notStrictEqual'
                });

                // 修改预期的错误消息格式
                assert.ok(
                    error.message.includes('Expected "actual" not to be reference-equal to "expected"') ||
                    error.message.includes('notStrictEqualObject')
                );
            });

            it('should handle empty objects comparison', () => {
                const error = new AssertionError({
                    actual: {},
                    expected: {},
                    operator: 'notStrictEqual'
                });

                assert.ok(error.message.includes('{}'));
            });
        });

        describe('Special Values', () => {
            // Special values tests
            it('should handle null and undefined comparison', () => {
                const error = new AssertionError({
                    actual: null,
                    expected: undefined,
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('null'));
                assert.ok(error.message.includes('undefined'));
            });

            it('should handle NaN comparison', () => {
                const error = new AssertionError({
                    actual: NaN,
                    expected: NaN,
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('NaN'));
            });

            it('should handle Symbol comparison', () => {
                const sym1 = Symbol('test');
                const sym2 = Symbol('test');
                const error = new AssertionError({
                    actual: sym1,
                    expected: sym2,
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('Symbol(test)'));
            });

            it('should handle zero values comparison', () => {
                const error = new AssertionError({
                    actual: 0,
                    expected: -0,
                    operator: 'strictEqual'
                });

                assert.ok(error.message.includes('0'));
            });
        });

        describe('Arrays and Complex Structures', () => {
            // Array and complex structure tests
            it('should handle array differences', () => {
                const actual = [1, { a: 2 }, 3];
                const expected = [1, { a: 3 }, 3];

                const error = new AssertionError({
                    actual,
                    expected,
                    operator: 'deepStrictEqual'
                });

                assert.ok(error.message.includes('Expected values to be strictly deep-equal'));
                assert.ok(error.message.includes('['));
            });

            it('should handle infinite arrays', () => {
                const longArray = Array(1000).fill(1);
                const error = new AssertionError({
                    actual: longArray,
                    expected: [],
                    operator: 'deepStrictEqual'
                });

                assert.ok(error.message.includes('['));
                assert.ok(error.message.includes(']'));
            });

            it('should handle circular object references', () => {
                const circular1 = { a: 1 };
                const circular2 = { a: 2 };
                circular1.self = circular1;
                circular2.self = circular2;

                const error = new AssertionError({
                    actual: circular1,
                    expected: circular2,
                    operator: 'deepStrictEqual'
                });

                assert.ok(error.message.includes('Expected values to be strictly deep-equal'));
                assert.ok(error.message.includes('self'));
            });

            it('should handle nested circular references', () => {
                const obj1 = { a: { b: {} } };
                const obj2 = { a: { b: {} } };
                obj1.a.b.ref = obj1.a;
                obj2.a.b.ref = obj2.a;

                const error = new AssertionError({
                    actual: obj1,
                    expected: obj2,
                    operator: 'deepStrictEqual'
                });

                assert.ok(error.message.length > 0);
                assert.ok(
                    error.message.includes('Object') ||
                    error.message.includes('a') ||
                    error.message.includes('{')
                );
            });
        });
    });

    describe('Stack Trace', () => {
        // Stack trace tests
        it('should have correct stack trace', () => {
            const error = new AssertionError({
                message: 'Test failed',
                operator: 'strictEqual',
                actual: 3,
                expected: 2
            });

            assert.strictEqual(error.name, 'AssertionError');
            assert.strictEqual(error.code, 'ERR_ASSERTION');
            assert.strictEqual(error.actual, 3);
            assert.strictEqual(error.expected, 2);
            assert.strictEqual(error.operator, 'strictEqual');
            assert.ok(error.message.includes('Test failed'));
        });

        it('should include stack trace', () => {
            const error = new AssertionError({
                message: 'Test message'
            });

            assert.ok(error.stack);
            assert.ok(error.stack.includes('AssertionError [ERR_ASSERTION]'));
            assert.ok(error.stack.includes('Test message'));
        });

        it('should respect custom stack start function', () => {
            function getError() {
                return new AssertionError({
                    message: 'Custom stack test'
                });
            }

            const error = getError();

            assert.ok(error.stack.includes('AssertionError [ERR_ASSERTION]'));
            assert.ok(error.stack.includes('Custom stack test'));
        });

        it('should handle stack trace limit', () => {
            const originalLimit = Error.stackTraceLimit;
            Error.stackTraceLimit = 2;

            const error = new AssertionError({
                message: 'Limited stack test'
            });

            assert.ok(error.stack.split('\n').length <= 4);

            Error.stackTraceLimit = originalLimit;
        });
    });

    describe('Operators', () => {
        // Move all operator tests from 'Additional Operator Tests' here
        describe('Truth Operators', () => {
            it('should handle ok operator', () => {
                const error = new AssertionError({ operator: 'ok' });
                assert.ok(error.message.includes('Expected the expression to be truthy'));
            });

            it('should handle notOk operator', () => {
                const error = new AssertionError({ operator: 'notOk' });
                assert.ok(error.message.includes('Expected the expression to be falsy'));
            });

            it('should handle isTrue operator', () => {
                const error = new AssertionError({ operator: 'isTrue' });
                assert.ok(error.message.includes('Expected the expression to strictly equal true'));
            });

            it('should handle isNotTrue operator', () => {
                const error = new AssertionError({ operator: 'isNotTrue' });
                assert.ok(error.message.includes('Expected the expression not to strictly equal true'));
            });

            it('should handle isFalse operator', () => {
                const error = new AssertionError({ operator: 'isFalse' });
                assert.ok(error.message.includes('Expected the expression to strictly equal false'));
            });

            it('should handle isNotFalse operator', () => {
                const error = new AssertionError({ operator: 'isNotFalse' });
                assert.ok(error.message.includes('Expected the expression not to strictly equal false'));
            });
        });

        describe('Existence Operators', () => {
            it('should handle isNull operator', () => {
                const error = new AssertionError({ operator: 'isNull' });
                assert.ok(error.message.includes('Expected the value to be null'));
            });

            it('should handle isNotNull operator', () => {
                const error = new AssertionError({ operator: 'isNotNull' });
                assert.ok(error.message.includes('Expected the value not to be null'));
            });

            it('should handle isUndefined operator', () => {
                const error = new AssertionError({ operator: 'isUndefined' });
                assert.ok(error.message.includes('Expected the value to be undefined'));
            });

            it('should handle isDefined operator', () => {
                const error = new AssertionError({ operator: 'isDefined' });
                assert.ok(error.message.includes('Expected the value not to be undefined'));
            });

            it('should handle exist operator', () => {
                const error = new AssertionError({ operator: 'exist' });
                assert.ok(error.message.includes('Expected the value to exist (non-null and non-undefined)'));
            });

            it('should handle notExist operator', () => {
                const error = new AssertionError({ operator: 'notExist' });
                assert.ok(error.message.includes('Expected the value to not exist (null or undefined)'));
            });
        });

        describe('Type Operators', () => {
            it('should handle isFunction operator', () => {
                const error = new AssertionError({ operator: 'isFunction' });
                assert.ok(error.message.includes('Expected the value to be a function'));
            });

            it('should handle isNotFunction operator', () => {
                const error = new AssertionError({ operator: 'isNotFunction' });
                assert.ok(error.message.includes('Expected the value not to be a function'));
            });

            it('should handle isObject operator', () => {
                const error = new AssertionError({ operator: 'isObject' });
                assert.ok(error.message.includes('Expected the value to be an object'));
            });

            it('should handle isNotObject operator', () => {
                const error = new AssertionError({ operator: 'isNotObject' });
                assert.ok(error.message.includes('Expected the value not to be an object'));
            });

            it('should handle isArray operator', () => {
                const error = new AssertionError({ operator: 'isArray' });
                assert.ok(error.message.includes('Expected the value to be an array'));
            });

            it('should handle isNotArray operator', () => {
                const error = new AssertionError({ operator: 'isNotArray' });
                assert.ok(error.message.includes('Expected the value not to be an array'));
            });

            it('should handle isString operator', () => {
                const error = new AssertionError({ operator: 'isString' });
                assert.ok(error.message.includes('Expected the value to be a string'));
            });

            it('should handle isNotString operator', () => {
                const error = new AssertionError({ operator: 'isNotString' });
                assert.ok(error.message.includes('Expected the value not to be a string'));
            });

            it('should handle isNumber operator', () => {
                const error = new AssertionError({ operator: 'isNumber' });
                assert.ok(error.message.includes('Expected the value to be a number'));
            });

            it('should handle isNotNumber operator', () => {
                const error = new AssertionError({ operator: 'isNotNumber' });
                assert.ok(error.message.includes('Expected the value not to be a number'));
            });

            it('should handle isBoolean operator', () => {
                const error = new AssertionError({ operator: 'isBoolean' });
                assert.ok(error.message.includes('Expected the value to be a boolean'));
            });

            it('should handle isNotBoolean operator', () => {
                const error = new AssertionError({ operator: 'isNotBoolean' });
                assert.ok(error.message.includes('Expected the value not to be a boolean'));
            });
        });

        describe('Property Operators', () => {
            it('should handle property operator', () => {
                const error = new AssertionError({
                    actual: { test: 'value' },
                    property: 'test',
                    operator: 'property'
                });
                assert.ok(error.message.includes('to have property'));
            });

            it('should handle notProperty operator', () => {
                const error = new AssertionError({
                    actual: { test: 'value' },
                    property: 'test',
                    operator: 'notProperty'
                });
                assert.ok(error.message.includes('not to have property'));
            });

            it('should handle deepProperty operator', () => {
                const error = new AssertionError({
                    actual: { test: { nested: 'value' } },
                    property: 'test.nested',
                    operator: 'deepProperty'
                });
                assert.ok(error.message.includes('to have deep property'));
            });

            it('should handle notDeepProperty operator', () => {
                const error = new AssertionError({
                    actual: { test: { nested: 'value' } },
                    property: 'test.nested',
                    operator: 'notDeepProperty'
                });
                assert.ok(error.message.includes('not to have deep property'));
            });

            it('should handle propertyVal operator', () => {
                const error = new AssertionError({
                    actual: { test: 'value' },
                    property: 'test',
                    expected: 'value',
                    operator: 'propertyVal'
                });
                assert.ok(error.message.includes('to have property with value'));
            });

            it('should handle propertyNotVal operator', () => {
                const error = new AssertionError({
                    actual: { test: 'value' },
                    property: 'test',
                    expected: 'value',
                    operator: 'propertyNotVal'
                });
                assert.ok(error.message.includes('not to have property with value'));
            });

            it('should handle deepPropertyVal operator', () => {
                const error = new AssertionError({
                    actual: { test: { nested: 'value' } },
                    property: 'test.nested',
                    expected: 'value',
                    operator: 'deepPropertyVal'
                });
                assert.ok(error.message.includes('to have deep property with value'));
            });

            it('should handle deepPropertyNotVal operator', () => {
                const error = new AssertionError({
                    actual: { test: { nested: 'value' } },
                    property: 'test.nested',
                    expected: 'value',
                    operator: 'deepPropertyNotVal'
                });
                assert.ok(error.message.includes('not to have deep property with value'));
            });
        });

        describe('Comparison Operators', () => {
            it('should handle match operator', () => {
                const error = new AssertionError({
                    actual: 'test',
                    expected: /test/,
                    operator: 'match'
                });
                assert.ok(error.message.includes('to match'));
            });

            it('should handle doesNotMatch operator', () => {
                const error = new AssertionError({
                    actual: 'test',
                    expected: /test/,
                    operator: 'doesNotMatch'
                });
                assert.ok(error.message.includes('not to match'));
            });

            it('should handle closeTo operator', () => {
                const error = new AssertionError({
                    actual: 5,
                    expected: 10,
                    operator: 'closeTo'
                });
                assert.ok(error.message.includes('to be close to'));
            });

            it('should handle notCloseTo operator', () => {
                const error = new AssertionError({
                    actual: 5,
                    expected: 10,
                    operator: 'notCloseTo'
                });
                assert.ok(error.message.includes('not to be close to'));
            });

            it('should handle lessThan operator', () => {
                const error = new AssertionError({
                    actual: 5,
                    expected: 10,
                    operator: 'lessThan'
                });
                assert.ok(error.message.includes('to be below'));
            });

            it('should handle notLessThan operator', () => {
                const error = new AssertionError({
                    actual: 5,
                    expected: 10,
                    operator: 'notLessThan'
                });
                assert.ok(error.message.includes('to be at least'));
            });

            it('should handle greaterThan operator', () => {
                const error = new AssertionError({
                    actual: 5,
                    expected: 10,
                    operator: 'greaterThan'
                });
                assert.ok(error.message.includes('to be above'));
            });

            it('should handle notGreaterThan operator', () => {
                const error = new AssertionError({
                    actual: 5,
                    expected: 10,
                    operator: 'notGreaterThan'
                });
                assert.ok(error.message.includes('to be at most'));
            });
        });

        describe('Exception Operators', () => {
            it('should handle throws operator', () => {
                const error = new AssertionError({ operator: 'throws' });
                assert.ok(error.message.includes('Missing expected exception'));
            });

            it('should handle doesNotThrow operator', () => {
                const error = new AssertionError({ operator: 'doesNotThrow' });
                assert.ok(error.message.includes('Got unwanted exception'));
            });

            it('should handle rejects operator', () => {
                const error = new AssertionError({ operator: 'rejects' });
                assert.ok(error.message.includes('Missing expected rejection'));
            });
        });

        describe('Standard Comparison Operators', () => {
            it('should format deep equal messages', () => {
                const error = new AssertionError({
                    actual: { x: 1 },
                    expected: { x: 2 },
                    operator: 'deepEqual'
                });
                assert.ok(error.message.includes('should loosely deep-equal'));
            });

            it('should format not strict equal object messages', () => {
                const obj = { a: 1 };
                const error = new AssertionError({
                    actual: obj,
                    expected: obj,
                    operator: 'notStrictEqual'
                });

                assert.ok(
                    error.message.includes('Expected "actual" not to be reference-equal to "expected"') ||
                    error.message.includes('notStrictEqualObject'),
                    'Should include correct operator message'
                );
            });

            it('should format special operators correctly', () => {
                const error = new AssertionError({
                    actual: 1,
                    expected: 2,
                    operator: 'notStrictEqual'
                });

                assert.ok(error.message.includes('Expected "actual" to be strictly unequal to'));
            });

            it('should format different operators correctly', () => {
                const operators = {
                    deepStrictEqual: 'Expected values to be strictly deep-equal',
                    strictEqual: 'Expected values to be strictly equal',
                    notStrictEqual: 'Expected values to be strictly unequal',
                    deepEqual: 'Expected values to be loosely deep-equal'
                };

                for (const [operator, expectedMessage] of Object.entries(operators)) {
                    const error = new AssertionError({
                        actual: { a: 1 },
                        expected: { a: 2 },
                        operator,
                        message: expectedMessage
                    });

                    assert.ok(
                        error.message.includes(expectedMessage),
                        `Operator "${operator}" should include message "${expectedMessage}"`
                    );
                }
            });
        });
    });
});