var test = require("test");
test.setup();

var collection = require('collection');
var os = require('os');

describe("collection", () => {
    describe('List', () => {
        function isBigEnough(element, index) {
            return (element >= 10);
        }

        it("create by array", () => {
            var a = new collection.List([1, 2, 3]);
            assert.deepEqual(a.toArray(), [1, 2, 3]);
        });


        it("pop empty list", () => {
            var a = new collection.List();
            assert.equal(a.pop(), null);
        });

        it("resize", () => {
            var a = new collection.List();
            a.resize(256);
            assert.equal(a.length, 256);
            assert.isUndefined(a[0]);
        });

        it("index", () => {
            var a = new collection.List();
            a.resize(256);
            for (var i = 0; i < 256; i++) {
                a[i] = 256 - i;
                assert.equal(a[i], 256 - i);
            }
            a.resize(0);
        });

        it('indexOf', () => {
            var nuList = new collection.List([2, 9, 9e10]);
            assert.equal(nuList.indexOf(2), 0);
            assert.equal(nuList.indexOf(7), -1);
            assert.equal(nuList.indexOf(9e10, 2), 2);
            assert.equal(nuList.indexOf(2, -1), -1);
            assert.equal(nuList.indexOf(2, -3), 0);

            var nullist = new collection.List([null, undefined]);
            assert.equal(nullist.indexOf(0), -1);
            assert.equal(nullist.indexOf(null), 0);
            assert.equal(nullist.indexOf(undefined), 1);

            var strlist = new collection.List(['a', 'b', 'c']);
            assert.equal(strlist.indexOf('a'), 0);
            assert.equal(strlist.indexOf('d'), -1);
            assert.equal(strlist.indexOf('c', 2), 2);
            assert.equal(strlist.indexOf('a', -1), -1);
            assert.equal(strlist.indexOf('a', -3), 0);

            var boollist = new collection.List([true, false, true]);
            assert.equal(boollist.indexOf(true), 0);
            assert.equal(boollist.indexOf(false), 1);

            var sl = new collection.List(['aaa', 'bbb', 'ccc', 'aaa', 'ddd']);
            assert.equal(sl.indexOf('aaa'), 0);
            assert.equal(sl.indexOf('bbb'), 1);
            assert.equal(sl.indexOf('ccc'), 2);
            assert.equal(sl.indexOf('ddd'), 4);
            assert.equal(sl.indexOf('eee'), -1);

            var obj1 = {
                'name': 'obj1'
            };
            var obj2 = {
                'name': 'obj2'
            };
            var obj3 = {
                'name': 'obj3'
            };
            var obj4 = {
                'name': 'obj4'
            };
            var objlist = new collection.List([obj1, obj2, obj3, obj4]);
            assert.equal(objlist.indexOf({
                'name': 'obj1'
            }), -1);
            assert.equal(objlist.indexOf(obj1), 0);
            assert.equal(objlist.indexOf(obj2), 1);

            var fs = require('fs');
            var http = require('http');
            var coroutine = require('coroutine');
            var fibobjlist = new collection.List([fs, http, coroutine]);
            assert.equal(fibobjlist.indexOf(1), -1);
            assert.equal(fibobjlist.indexOf(fs), 0);
            assert.equal(fibobjlist.indexOf(http), 1);
            assert.equal(fibobjlist.indexOf(coroutine), 2);
        });

        it("Memory Leak detect", () => {
            var a = new collection.List();
            GC();
            var no1 = os.memoryUsage().nativeObjects.objects;
            a.push(new Buffer());
            GC();
            assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);
            a.resize(0);
            a = undefined;
            GC();
            assert.equal(no1 - 1, os.memoryUsage().nativeObjects.objects);
            new Buffer();
            assert.equal(no1, os.memoryUsage().nativeObjects.objects);
            GC();
            assert.equal(no1 - 1, os.memoryUsage().nativeObjects.objects);
        });

        it("toArray", () => {
            var a = new collection.List();
            a.push(1);
            assert.deepEqual(a.toArray(), [1]);
        });

        it("push", () => {
            var a = new collection.List([1]);
            assert.equal(a.length, 1);
            assert.equal(a.push(2, 3, 4), 4);
            assert.deepEqual(a.toArray(), [1, 2, 3, 4]);
        });

        it("pushArray", () => {
            var a = new collection.List([1]);
            a.pushArray([2, 3, 4]);
            assert.deepEqual(a.toArray(), [1, 2, 3, 4]);
        });

        it("pop", () => {
            var a = new collection.List([1, 2, 3, 4]);
            assert.equal(a.pop(), 4);
            assert.equal(a.length, 3);
        });

        it("slice", () => {
            var a = new collection.List([1, 2, 3]);
            assert.deepEqual(a.slice(1).toArray(), [2, 3]);
            assert.deepEqual(a.slice(1, 2).toArray(), [2]);
            assert.deepEqual(a.slice(1, 0).toArray(), []);
        });

        it("concat", () => {
            var a = new collection.List([1, 2, 3]);
            assert.deepEqual(a.concat(a).toArray(), [1, 2, 3, 1, 2, 3]);
            assert.deepEqual(a.concat(new collection.List()).toArray(), [1, 2,
                3
            ]);
            assert.deepEqual(a.concat([4, 5, 6]).toArray(), [1, 2, 3, 4, 5,
                6
            ]);
            assert.deepEqual(a.concat(a, [4, 5, 6], a).toArray(), [1, 2, 3,
                1, 2, 3, 4, 5, 6, 1, 2, 3
            ]);
        });

        it("every", () => {
            var passed = new collection.List();
            passed.push(12, 5, 8, 130, 44);
            assert.equal(passed.every(isBigEnough), false);

            passed = new collection.List();
            passed.push(12, 54, 18, 130, 44);
            assert.equal(passed.every(isBigEnough), true);
        });

        it("some", () => {
            var passed = new collection.List();
            passed.push(12, 5, 8, 130, 44);
            assert.ok(passed.some(isBigEnough));

            passed = new collection.List();
            passed.push(1, 5, 8, 3, 4);
            assert.notOk(passed.some(isBigEnough));
        });

        it("filter", () => {
            var passed = new collection.List();
            passed.push(12, 5, 8, 130, 44);
            assert.deepEqual(passed.filter(isBigEnough).toArray(), [12, 130,
                44
            ]);
        });

        it("map", () => {
            var passed = new collection.List();
            passed.push(1, 4, 9);
            assert.deepEqual(passed.map(Math.sqrt).toArray(), [1, 2, 3]);
        });

        it("reduce", () => {
            var passed = new collection.List();
            passed.push(1, 4, 9);
            assert.deepEqual(passed.reduce((sum, n) => sum + n, 0), 14);
        });

        it("forEach", () => {
            var passed = new collection.List();
            passed.push(1, 4, 9);
            passed.forEach((v, i) => {
                assert.equal(passed[i], v);
            });
        });

        it("sort", () => {
            var passed = new collection.List();
            passed.push(1, 4, 9, 100);

            passed.sort();
            assert.deepEqual(passed.toArray(), [1, 100, 4, 9]);

            passed.sort((a, b) => {
                return a - b;
            });
            assert.deepEqual(passed.toArray(), [1, 4, 9, 100]);
        });

        it("freeze", () => {
            var passed = new collection.List();
            passed.push(1, 4, 9);
            passed.freeze();

            assert.throws(() => {
                passed[0] = 100;
            });

            assert.throws(() => {
                passed.resize(10);
            });

            assert.throws(() => {
                passed.push(10);
            });

            assert.throws(() => {
                passed.pop();
            });

            assert.throws(() => {
                passed.concat([4, 5, 6]);
            });
        });

    });

    describe("Queue", () => {
        it("add", () => {
            var q = new collection.Queue(3);

            assert.equal(q.add(100), true);
            assert.equal(q.add(200), true);
            assert.equal(q.add(300), true);
            assert.deepEqual(q.toArray(), [100, 200, 300]);

            assert.throws(() => {
                q.add(400);
            });
            assert.deepEqual(q.toArray(), [100, 200, 300]);
        });

        it("offer", () => {
            var q = new collection.Queue(3);

            assert.equal(q.offer(100), true);
            assert.equal(q.offer(200), true);
            assert.equal(q.offer(300), true);
            assert.deepEqual(q.toArray(), [100, 200, 300]);

            assert.equal(q.offer(400), false);
            assert.deepEqual(q.toArray(), [100, 200, 300]);
        });

        it("remove", () => {
            var q = new collection.Queue(3);

            q.add(100);
            q.add(200);
            assert.equal(q.remove(), 100);
            assert.deepEqual(q.toArray(), [200]);

            assert.equal(q.remove(), 200);
            assert.deepEqual(q.toArray(), []);

            assert.throws(() => {
                q.remove();
            });
            assert.deepEqual(q.toArray(), []);
        });

        it("poll", () => {
            var q = new collection.Queue(3);

            q.add(100);
            q.add(200);
            assert.equal(q.poll(), 100);
            assert.deepEqual(q.toArray(), [200]);

            assert.equal(q.poll(), 200);
            assert.deepEqual(q.toArray(), []);

            assert.isNull(q.poll());
            assert.deepEqual(q.toArray(), []);
        });

        it("clear/length", () => {
            var q = new collection.Queue(3);

            q.add(100);
            q.add(200);
            assert.deepEqual(q.toArray(), [100, 200]);

            q.clear();
            assert.deepEqual(q.toArray(), []);
            assert.equal(q.length, 0);
        });

        it("element", () => {
            var q = new collection.Queue(3);

            q.add(100);
            q.add(200);
            assert.equal(q.element(), 100);
            assert.deepEqual(q.toArray(), [100, 200]);

            q.clear();
            assert.throws(() => {
                q.element();
            });
        });

        it("peek", () => {
            var q = new collection.Queue(3);

            q.add(100);
            q.add(200);
            assert.equal(q.element(), 100);
            assert.deepEqual(q.toArray(), [100, 200]);

            q.clear();
            assert.isNull(q.peek());
        });
    });
});

// test.run(console.DEBUG);
