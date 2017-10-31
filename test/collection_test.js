var test = require("test");
test.setup();

var test_util = require('./test_util');

var collection = require('collection');
var os = require('os');

describe("collection", () => {
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

repl && test.run(console.DEBUG);