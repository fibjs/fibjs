var test = require("test");
test.setup();

describe(
		'assert',
		function() {
			it('assert', function() {
				var foo = 'bar';
				assert.ok(foo == 'bar', "expected foo to equal `bar`");

				assert.throws(function() {
					assert(foo == 'baz', "expected foo to equal `bar`");
				}, "expected foo to equal `bar`");
			});

			it('isTrue', function() {
				assert.isTrue(true);

				assert.throws(function() {
					assert.isTrue(false);
				}, "expected false to be true");

				assert.throws(function() {
					assert.isTrue(1);
				}, "expected 1 to be true");

				assert.throws(function() {
					assert.isTrue('test');
				}, "expected 'test' to be true");
			});

			it('ok', function() {
				assert.ok(true);
				assert.ok(1);
				assert.ok('test');

				assert.throws(function() {
					assert.ok(false);
				}, "expected false to be truthy");

				assert.throws(function() {
					assert.ok(0);
				}, "expected 0 to be truthy");

				assert.throws(function() {
					assert.ok('');
				}, "expected '' to be truthy");
			});

			it('notOk', function() {
				assert.notOk(false);
				assert.notOk(0);
				assert.notOk('');

				assert.throws(function() {
					assert.notOk(true);
				}, "expected true to be falsy");

				assert.throws(function() {
					assert.notOk(1);
				}, "expected 1 to be falsy");

				assert.throws(function() {
					assert.notOk('test');
				}, "expected 'test' to be falsy");
			});

			it('isFalse', function() {
				assert.isFalse(false);

				assert.throws(function() {
					assert.isFalse(true);
				}, "expected true to be false");

				assert.throws(function() {
					assert.isFalse(0);
				}, "expected 0 to be false");
			});

			it('equal', function() {
				var foo;
				assert.equal(foo, undefined);
			});

			it('isObject', function() {
				function Foo() {
				}
				assert.isObject({});
				assert.isObject(new Foo());

				assert.throws(function() {
					assert.isObject(true);
				}, "expected true to be an object");

				assert.throws(function() {
					assert.isObject('foo');
				}, "expected 'foo' to be an object");
			});

			it('isNotObject', function() {
				function Foo() {
				}
				assert.isNotObject(5);

				assert.throws(function() {
					assert.isNotObject({});
				}, "expected {} not to be an object");
			});

			it('notEqual', function() {
				assert.notEqual(3, 4);

				assert.throws(function() {
					assert.notEqual(5, 5);
				}, "expected 5 to not equal 5");
			});

			it('strictEqual', function() {
				assert.strictEqual('foo', 'foo');

				assert.throws(function() {
					assert.strictEqual('5', 5);
				}, "expected \'5\' to equal 5");
			});

			it('notStrictEqual', function() {
				assert.notStrictEqual(5, '5');

				assert.throws(function() {
					assert.notStrictEqual(5, 5);
				}, "expected 5 to not equal 5");
			});

			it(
					'deepEqual',
					function() {
						assert.deepEqual({
							tea : 'chai'
						}, {
							tea : 'chai'
						});

						assert
								.throws(function() {
									assert.deepEqual({
										tea : 'chai'
									}, {
										tea : 'black'
									});
								},
										"expected { tea: \'chai\' } to deeply equal { tea: \'black\' }");

						var obja = Object.create({
							tea : 'chai'
						}), objb = Object.create({
							tea : 'chai'
						});

						assert.deepEqual(obja, objb);

						var obj1 = Object.create({
							tea : 'chai'
						}), obj2 = Object.create({
							tea : 'black'
						});

						assert
								.throws(function() {
									assert.deepEqual(obj1, obj2);
								},
										"expected { tea: \'chai\' } to deeply equal { tea: \'black\' }");
					});

			it('deepEqual (ordering)', function() {
				var a = {
					a : 'b',
					c : 'd'
				}, b = {
					c : 'd',
					a : 'b'
				};
				assert.deepEqual(a, b);
			});

			it('deepEqual /regexp/', function() {
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

			it('deepEqual (Date)', function() {
				var a = new Date(1, 2, 3), b = new Date(4, 5, 6);
				assert.deepEqual(a, a);
				assert.notDeepEqual(a, b);
				assert.notDeepEqual(a, {});
			});

			it(
					'deepEqual (circular)',
					function() {
						var circularObject = {}, secondCircularObject = {};
						circularObject.field = circularObject;
						secondCircularObject.field = secondCircularObject;

						assert.deepEqual(circularObject, secondCircularObject);

						assert
								.throws(
										function() {
											secondCircularObject.field2 = secondCircularObject;
											assert.deepEqual(circularObject,
													secondCircularObject);
										},
										"expected { field: [Circular] } to deeply equal { Object (field, field2) }");
					});

			it(
					'notDeepEqual',
					function() {
						assert.notDeepEqual({
							tea : 'jasmine'
						}, {
							tea : 'chai'
						});

						assert
								.throws(function() {
									assert.notDeepEqual({
										tea : 'chai'
									}, {
										tea : 'chai'
									});
								},
										"expected { tea: \'chai\' } to not deeply equal { tea: \'chai\' }");
					});

			it(
					'notDeepEqual (circular)',
					function() {
						var circularObject = {}, secondCircularObject = {
							tea : 'jasmine'
						};
						circularObject.field = circularObject;
						secondCircularObject.field = secondCircularObject;

						assert.notDeepEqual(circularObject,
								secondCircularObject);

						assert
								.throws(function() {
									delete secondCircularObject.tea;
									assert.notDeepEqual(circularObject,
											secondCircularObject);
								},
										"expected { field: [Circular] } to not deeply equal { field: [Circular] }");
					});

			it('isNull', function() {
				assert.isNull(null);

				assert.throws(function() {
					assert.isNull(undefined);
				}, "expected undefined to equal null");
			});

			it('isNotNull', function() {
				assert.isNotNull(undefined);

				assert.throws(function() {
					assert.isNotNull(null);
				}, "expected null to not equal null");
			});

			it('isUndefined', function() {
				assert.isUndefined(undefined);

				assert.throws(function() {
					assert.isUndefined(null);
				}, "expected null to equal undefined");
			});

			it('isDefined', function() {
				assert.isDefined(null);

				assert.throws(function() {
					assert.isDefined(undefined);
				}, "expected undefined to not equal undefined");
			});

			it('isFunction', function() {
				var func = function() {
				};
				assert.isFunction(func);

				assert.throws(function() {
					assert.isFunction({});
				}, "expected {} to be a function");
			});

			it('isNotFunction', function() {
				assert.isNotFunction(5);

				assert.throws(function() {
					assert.isNotFunction(function() {
					});
				}, "expected [Function] not to be a function");
			});

			it('isArray', function() {
				assert.isArray([]);
				assert.isArray(new Array);

				assert.throws(function() {
					assert.isArray({});
				}, "expected {} to be an array");
			});

			it('isNotArray', function() {
				assert.isNotArray(3);

				assert.throws(function() {
					assert.isNotArray([]);
				}, "expected [] not to be an array");

				assert.throws(function() {
					assert.isNotArray(new Array);
				}, "expected [] not to be an array");
			});

			it('isString', function() {
				assert.isString('Foo');
				assert.isString(new String('foo'));

				assert.throws(function() {
					assert.isString(1);
				}, "expected 1 to be a string");
			});

			it('isNotString', function() {
				assert.isNotString(3);
				assert.isNotString([ 'hello' ]);

				assert.throws(function() {
					assert.isNotString('hello');
				}, "expected 'hello' not to be a string");
			});

			it('isNumber', function() {
				assert.isNumber(1);
				assert.isNumber(Number('3'));

				assert.throws(function() {
					assert.isNumber('1');
				}, "expected \'1\' to be a number");
			});

			it('isNotNumber', function() {
				assert.isNotNumber('hello');
				assert.isNotNumber([ 5 ]);

				assert.throws(function() {
					assert.isNotNumber(4);
				}, "expected 4 not to be a number");
			});

			it('isBoolean', function() {
				assert.isBoolean(true);
				assert.isBoolean(false);

				assert.throws(function() {
					assert.isBoolean('1');
				}, "expected \'1\' to be a boolean");
			});

			it('isNotBoolean', function() {
				assert.isNotBoolean('true');

				assert.throws(function() {
					assert.isNotBoolean(true);
				}, "expected true not to be a boolean");

				assert.throws(function() {
					assert.isNotBoolean(false);
				}, "expected false not to be a boolean");
			});

			it('property', function () {
			    var obj = { foo: { bar: 'baz' } };
			    var simpleObj = { foo: 'bar' };
			    assert.property(obj, 'foo');
			    assert.deepProperty(obj, 'foo.bar');
			    assert.notProperty(obj, 'baz');
			    assert.notProperty(obj, 'foo.bar');
			    assert.notDeepProperty(obj, 'foo.baz');
			    assert.deepPropertyVal(obj, 'foo.bar', 'baz');
			    assert.deepPropertyNotVal(obj, 'foo.bar', 'flow');

			    assert.throws(function () {
			      assert.property(obj, 'baz');
			    }, "expected { foo: { bar: 'baz' } } to have a property 'baz'");

			    assert.throws(function () {
			      assert.deepProperty(obj, 'foo.baz');
			    }, "expected { foo: { bar: 'baz' } } to have a deep property 'foo.baz'");

			    assert.throws(function () {
			      assert.notProperty(obj, 'foo');
			    }, "expected { foo: { bar: 'baz' } } to not have property 'foo'");

			    assert.throws(function () {
			      assert.notDeepProperty(obj, 'foo.bar');
			    }, "expected { foo: { bar: 'baz' } } to not have deep property 'foo.bar'");

			    assert.throws(function () {
			      assert.propertyVal(simpleObj, 'foo', 'ball');
			    }, "expected { foo: 'bar' } to have a property 'foo' of 'ball', but got 'bar'");

			    assert.throws(function () {
			      assert.deepPropertyVal(obj, 'foo.bar', 'ball');
			    }, "expected { foo: { bar: 'baz' } } to have a deep property 'foo.bar' of 'ball', but got 'baz'");

			    assert.throws(function () {
			      assert.propertyNotVal(simpleObj, 'foo', 'bar');
			    }, "expected { foo: 'bar' } to not have a property 'foo' of 'bar'");

			    assert.throws(function () {
			      assert.deepPropertyNotVal(obj, 'foo.bar', 'baz');
			    }, "expected { foo: { bar: 'baz' } } to not have a deep property 'foo.bar' of 'baz'");
			  });

			it(
					'throws',
					function() {
						assert.throws(function() {
							throw new Error('foo');
						});
						assert.throws(function() {
							throw new Error('bar');
						}, 'bar');
						assert.throws(function() {
							throw new Error('bar');
						}, /bar/);

						assert.throws(function() {
							assert.throws(function() {
							});
						}, "expected [Function] to throw an error");
					});

			it(
					'doesNotThrow',
					function() {
						assert.doesNotThrow(function() {
						});
						assert.doesNotThrow(function() {
						}, 'foo');

						assert
								.throws(function() {
									assert.doesNotThrow(function() {
										throw new Error('foo');
									});
								},
										'expected [Function] to not throw an error but [Error: foo] was thrown');
					});

			it('closeTo', function() {
				assert.closeTo(1.5, 1.0, 0.5);
				assert.closeTo(10, 20, 20);
				assert.closeTo(-10, 20, 30);

				assert.throws(function() {
					assert.closeTo(2, 1.0, 0.5);
				}, "expected 2 to be close to 1 +/- 0.5");

				assert.throws(function() {
					assert.closeTo(-10, 20, 29);
				}, "expected -10 to be close to 20 +/- 29");
			});
		});

test.run();
