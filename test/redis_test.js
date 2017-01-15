var test = require("test");
test.setup();

var os = require('os');
var db = require('db');
var encoding = require('encoding');
var coroutine = require('coroutine');

var dbs = "redis://127.0.0.1";

var rdb = db.open(dbs);

function listEquals(list, arr) {
    var arr1 = [];
    var i, v;

    for (i = 0; i < list.length; i++) {
        v = list[i];
        if (v)
            arr1.push(v.toString());
        else arr1.push(v);
    }

    assert.deepEqual(arr1, arr);
}

describe("redis", function() {
    before(function() {
        var keys = rdb.command("keys", "*").toArray();
        if (keys.length)
            rdb.del(keys);
    });

    describe("base", function() {
        it("command", function() {
            rdb.command("set", "test", "aaa");
            rdb.command("set", "test1", "aaa");
            assert.equal(rdb.command("get", "test"), "aaa");

            assert.equal(rdb.command("exists", "test"), 1);
            listEquals(rdb.command("keys", "*").toArray().sort(), ["test", "test1"]);
            listEquals(rdb.command("keys", "aa*").toArray(), []);

            listEquals(rdb.command("scan", "0")[1].toArray().sort(), ["test", "test1"]);
        });

        it("exists", function() {
            assert.isTrue(rdb.exists("test"));
            assert.isFalse(rdb.exists("test2"));
        });

        it("type", function() {
            assert.equal(rdb.type("test"), "string");
            assert.equal(rdb.type("test2"), "none");
        });

        it("get", function() {
            assert.equal(rdb.get("test"), "aaa");
            assert.isNull(rdb.get("test2"));
        });

        it("mget", function() {
            listEquals(rdb.mget("test", "test1").toArray(), ["aaa", "aaa"]);
            listEquals(rdb.mget(["test", "test1"]).toArray(), ["aaa", "aaa"]);
            listEquals(rdb.mget("test", "test2").toArray(), ["aaa", null]);
        });

        it("set", function() {
            rdb.set("test", "aaa1")
            assert.equal(rdb.get("test"), "aaa1");

            rdb.set("test2", "aaa2", 100)
            assert.equal(rdb.get("test2"), "aaa2");
            coroutine.sleep(150);
            assert.isFalse(rdb.exists("test2"));
        });

        it("mset", function() {
            rdb.mset("test", "bbb", "test1", "bbb1");
            listEquals(rdb.mget("test", "test1").toArray(), ["bbb", "bbb1"]);

            rdb.mset({
                "test": "bbb1",
                "test1": "bbb2"
            });
            listEquals(rdb.mget("test", "test1").toArray(), ["bbb1", "bbb2"]);
        });

        it("msetNX", function() {
            rdb.msetNX("test", "bbb", "test2", "bbb1");
            listEquals(rdb.mget("test", "test1", "test2").toArray(), ["bbb1", "bbb2", null]);
        });

        it("setNX", function() {
            rdb.set("test", "aaa1")
            rdb.setNX("test", "aaa3")
            assert.equal(rdb.get("test"), "aaa1");
        });

        it("setXX", function() {
            rdb.set("test", "aaa1")
            rdb.setXX("test", "aaa3")
            assert.equal(rdb.get("test"), "aaa3");
        });

        it("decr", function() {
            rdb.set("test", "100");
            assert.equal(rdb.decr("test"), 99);
            assert.equal(rdb.decr("test", 9), 90);
        });

        it("incr", function() {
            rdb.set("test", "100");
            assert.equal(rdb.incr("test"), 101);
            assert.equal(rdb.incr("test", 9), 110);
        });

        it("setBit/getBit", function() {
            rdb.set("test", "aaa");
            assert.equal(rdb.getBit("test", 5), 0);
            assert.equal(rdb.setBit("test", 5, 1), 0);
            assert.equal(rdb.getBit("test", 5), 1);
        });

        it("getset", function() {
            rdb.set("test", "aaa");
            assert.equal(rdb.getset("test", "bbb"), "aaa");
            assert.equal(rdb.get("test"), "bbb");
        });

        it("append", function() {
            rdb.set("test", "aaa1")
            assert.equal(rdb.append("test", "aaa2"), 8);
            assert.equal(rdb.get("test"), "aaa1aaa2");

            assert.equal(rdb.append("test2", "aaa2"), 4);
            assert.equal(rdb.get("test2"), "aaa2");
        });

        it("strlen", function() {
            rdb.set("test", "aaa1");
            assert.equal(rdb.strlen("test"), 4);
            assert.equal(rdb.strlen("test3"), 0);
        });

        it("setRange", function() {
            rdb.set("test", "hello world");
            rdb.setRange("test", 6, "Redis");
            assert.equal(rdb.get("test"), "hello Redis");
        });

        it("getRange", function() {
            rdb.set("test", "hello, my friend");
            assert.equal(rdb.getRange("test", 0, 4), "hello");
            assert.equal(rdb.getRange("test", -1, -5), "");
            assert.equal(rdb.getRange("test", -3, -1), "end");
            assert.equal(rdb.getRange("test", 0, -1), "hello, my friend");
        });

        it("bitcount", function() {
            rdb.set("test2", "foobar");
            assert.equal(rdb.bitcount("test2"), 26);
            assert.equal(rdb.bitcount("test2", 0, 0), 4);
            assert.equal(rdb.bitcount("test2", 1, 1), 6);
            assert.equal(rdb.bitcount("test3"), 0);
        });

        it("expire/ttl", function() {
            rdb.set("test2", "aaa2")
            assert.equal(rdb.ttl("test2"), -1);
            assert.equal(rdb.ttl("test3"), -2);

            assert.isFalse(rdb.expire("test3", 100));
            assert.isTrue(rdb.expire("test2", 100));

            assert.greaterThan(rdb.ttl("test2"), 0);

            coroutine.sleep(150);
            assert.isFalse(rdb.exists("test2"));
        });

        it("persist", function() {
            rdb.set("test", "aaa2", 100);
            assert.greaterThan(rdb.ttl("test"), 0);
            assert.isTrue(rdb.persist("test"));
            assert.equal(rdb.ttl("test"), -1);

            assert.isFalse(rdb.persist("test3"));
        });

        it("rename", function() {
            rdb.set("test", "aaa222");

            assert.isTrue(rdb.exists("test"));
            assert.isFalse(rdb.exists("test2"));

            rdb.rename("test", "test2");

            assert.isFalse(rdb.exists("test"));
            assert.isTrue(rdb.exists("test2"));

            rdb.rename("test2", "test1");
            assert.equal(rdb.get("test1"), "aaa222");

            rdb.set("test", "aaa");
        });

        it("renameNX", function() {
            assert.isTrue(rdb.exists("test"));
            assert.isFalse(rdb.exists("test2"));

            assert.isTrue(rdb.renameNX("test", "test2"));
            assert.equal(rdb.get("test2"), "aaa");

            assert.isFalse(rdb.exists("test"));
            assert.isTrue(rdb.exists("test2"));

            assert.isFalse(rdb.renameNX("test2", "test1"));
            assert.equal(rdb.get("test1"), "aaa222");
            assert.equal(rdb.get("test2"), "aaa");

            rdb.renameNX("test2", "test");
        });

        it("keys", function() {
            listEquals(rdb.keys("*").toArray().sort(), ["test", "test1"]);
        });

        it("del", function() {
            listEquals(rdb.command("keys", "*").toArray().sort(), ["test", "test1"]);
            assert.equal(rdb.del("test", "test1"), 2);
            listEquals(rdb.command("keys", "*").toArray(), []);

            rdb.set("test", "aaa");
            rdb.set("test1", "aaa");

            listEquals(rdb.command("keys", "*").toArray().sort(), ["test", "test1"]);
            assert.equal(rdb.del(["test", "test1"]), 2);
            listEquals(rdb.command("keys", "*").toArray(), []);

            assert.equal(rdb.del(["test", "test1"]), 0);
        });

        it("dump", function() {
            rdb.set("greeting", "hello, dumping world!");
            assert.equal(rdb.dump("greeting").hex(), "001568656c6c6f2c2064756d70696e6720776f726c642107002c7fe7f125ed2857");
            assert.isNull(rdb.dump("greeting1"));
            rdb.del("greeting");
        });

        it("restore", function() {
            rdb.restore("greeting", encoding.hex.decode("001568656c6c6f2c2064756d70696e6720776f726c642107002c7fe7f125ed2857"));
            assert.equal(rdb.command("get", "greeting"), "hello, dumping world!");
        });
    });

    describe("Hash", function() {
        it("set/get", function() {
            var hash = rdb.getHash("testHash");

            hash.set("test", "hash aaaa");
            assert.equal(hash.get("test"), "hash aaaa");
            assert.isNull(hash.get("test1"));
        });

        it("exists", function() {
            var hash = rdb.getHash("testHash");

            assert.isTrue(hash.exists("test"));
            assert.isFalse(hash.exists("test1"));
        });

        it("len", function() {
            var hash = rdb.getHash("testHash");

            assert.equal(hash.len(), 1);
        });

        it("keys", function() {
            var hash = rdb.getHash("testHash");

            listEquals(hash.keys().toArray(), ["test"]);
        });

        it("getAll", function() {
            var hash = rdb.getHash("testHash");

            listEquals(hash.getAll().toArray(), ["test", "hash aaaa"]);
        });

        it("setNX", function() {
            var hash = rdb.getHash("testHash");

            hash.setNX("test", "hash1");
            assert.equal(hash.get("test"), "hash aaaa");
            hash.setNX("test new", "hash1");
            assert.equal(hash.get("test new"), "hash1");
        });

        it("mset/mget", function() {
            var hash = rdb.getHash("testHash");

            hash.mset("test1", "bbb1", "test2", "bbb2");
            hash.mset({
                "test3": "bbb3",
                "test4": "bbb4"
            });

            listEquals(hash.mget("test1", "test2").toArray(), ["bbb1", "bbb2"]);
            listEquals(hash.mget(["test1", "test2"]).toArray(), ["bbb1", "bbb2"]);
        });

        it("incr", function() {
            var hash = rdb.getHash("testHash");

            assert.equal(hash.incr("pv"), 1);
            assert.equal(hash.incr("pv", 100), 101);
            assert.equal(hash.incr("pv"), 102);
        });

        it("del", function() {
            var hash = rdb.getHash("testHash");

            assert.equal(hash.del("test1", "test2", "test001"), 2);
            assert.equal(hash.del(["test3", "test2", "test002"]), 1);

            assert.isFalse(hash.exists("test1"));
            assert.isFalse(hash.exists("test2"));
            assert.isFalse(hash.exists("test3"));
        });
    });

    describe("List", function() {
        it("push", function() {
            var list = rdb.getList("testList");
            assert.equal(list.push("a0", "a1", "a2"), 3);
            assert.equal(list.push(["a4", "a5", "a6"]), 6);
        });

        it("pop", function() {
            var list = rdb.getList("testList");
            assert.equal(list.pop(), "a6");
        });

        it("set/get", function() {
            var list = rdb.getList("testList");

            list.set(0, "hash aaaa");
            assert.equal(list.get(0), "hash aaaa");
            assert.isNull(list.get(9));
        });

        it("len", function() {
            var list = rdb.getList("testList");
            assert.equal(list.len(), 5);
        });

        it("range", function() {
            var list = rdb.getList("testList");
            listEquals(list.range(0, 4).toArray(), ["hash aaaa", "a4", "a2", "a1", "a0"]);
        });

        it("insert", function() {
            var list = rdb.getList("testList");

            assert.equal(list.insertBefore("a4", "a3"), 6);
            listEquals(list.range(0, 4).toArray(), ["hash aaaa", "a3", "a4", "a2", "a1"]);

            assert.equal(list.insertAfter("a4", "a9"), 7);
            listEquals(list.range(0, 4).toArray(), ["hash aaaa", "a3", "a4", "a9", "a2"]);
        });

        it("remove", function() {
            var list = rdb.getList("testList");

            assert.equal(list.remove(1, "a4"), 1);
            listEquals(list.range(0, 4).toArray(), ["hash aaaa", "a3", "a9", "a2", "a1"]);
        });

        it("trim", function() {
            var list = rdb.getList("testList");

            list.trim(1, 3);
            listEquals(list.range(0, 2).toArray(), ["a3", "a9", "a2"]);
        });

        it("rpush", function() {
            var list = rdb.getList("testList");
            assert.equal(list.rpush("a0", "a1", "a2"), 6);
            assert.equal(list.rpush("a4", "a5", "a6"), 9);
        });

        it("rpop", function() {
            var list = rdb.getList("testList");
            assert.equal(list.rpop(), "a6");
        });
    });

    describe("Set", function() {
        it("add", function() {
            var set = rdb.getSet("testSet");

            assert.equal(set.add("a0", "a1", "a2"), 3);
            assert.equal(set.add(["a4", "a5", "a6"]), 3);
        });

        it("exists", function() {
            var set = rdb.getSet("testSet");

            assert.isTrue(set.exists("a1"));
            assert.isFalse(set.exists("a10"));
        });

        it("remove", function() {
            var set = rdb.getSet("testSet");

            assert.isTrue(set.exists("a1"));

            assert.equal(set.remove("a1", "a2"), 2);
            assert.equal(set.remove(["a5", "a4"]), 2);

            assert.equal(set.remove("a10"), 0);

            assert.isFalse(set.exists("a1"));
        });

        it("len", function() {
            var set = rdb.getSet("testSet");

            assert.equal(set.len(), 2);
        });

        it("members", function() {
            var set = rdb.getSet("testSet");

            listEquals(set.members().toArray().sort(), ["a0", "a6"]);
        });

        it("pop", function() {
            var set = rdb.getSet("testSet");

            var m = set.pop();
            assert.ok(m.toString() === "a6" || m.toString() === "a0");
        });

        it("randMember", function() {
            var set = rdb.getSet("testSet");

            var m = set.randMember();
            assert.ok(m == "a6" || m == "a0");

            var m = set.randMember(2);
            assert.equal(m.length, 1);
            assert.ok(m[0] == "a6" || m[0] == "a0");
        });
    });

    describe("SortedSet", function() {
        it("add", function() {
            var zset = rdb.getSortedSet("testSortedSet");

            assert.equal(zset.add("a0", 1, "a1", 2, "a2", 3), 3);
            assert.equal(zset.add({
                "a4": 4,
                "a5": 5,
                "a6": 6
            }), 3);
        });

        it("len", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            assert.equal(zset.len(), 6);
        });

        it("count", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            assert.equal(zset.count(2, 5), 4);
        });

        it("range", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            listEquals(zset.range(2, 5).toArray(), ["a2", "a4", "a5", "a6"]);
            listEquals(zset.range(2, 5, true).toArray(), ["a2", "3", "a4", "4", "a5", "5", "a6", "6"]);
        });

        it("rangeRev", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            listEquals(zset.rangeRev(2, 5).toArray(), ["a4", "a2", "a1", "a0"]);
            listEquals(zset.rangeRev(2, 5, true).toArray(), ["a4", "4", "a2", "3", "a1", "2", "a0", "1"]);
        });

        it("rank", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            assert.equal(zset.rank("a4"), 3);
        });

        it("rankRev", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            assert.equal(zset.rankRev("a4"), 2);
        });

        it("remove", function() {
            var zset = rdb.getSortedSet("testSortedSet");

            assert.equal(zset.remove("a1", "a2"), 2);
            assert.equal(zset.len(), 4);

            assert.equal(zset.remove(["a1", "a4"]), 1);
            assert.equal(zset.len(), 3);
        });

        it("incr", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            assert.equal(zset.incr("a5", 2), "7");
        });

        it("score", function() {
            var zset = rdb.getSortedSet("testSortedSet");
            assert.equal(zset.score("a5"), "7");
        });
    });

    describe("PubSub", function() {
        var c, m, p,
            n1 = 0,
            n2 = 0;

        function subf1(ch, msg, pattern) {
            c = ch;
            m = msg;
            p = pattern;
            n1++;
        }

        function subf2(ch, msg, pattern) {
            c = ch;
            m = msg;
            p = pattern;
            n2++;
        }

        var rdb1;

        before(function() {
            rdb1 = db.open(dbs);
        });

        it("pub", function() {
            rdb.pub("test.channel", "test value");
        });

        it("sub", function() {
            rdb.sub("test.ch1", subf1);

            assert.throws(function() {
                rdb.pub("test.ch1", "test value");
            });

            rdb1.pub("test.ch1", "test value 1");

            coroutine.sleep(100);
            assert.equal(c, "test.ch1");
            assert.equal(m, "test value 1");
            assert.equal(n1, 1);

            rdb.sub("test.ch1", subf1);

            rdb1.pub("test.ch1", "test value 1");

            coroutine.sleep(100);
            assert.equal(n1, 2);
        });

        it("sub multi", function() {
            n1 = 0;
            rdb.sub({
                "test.ch1": subf1,
                "test.ch2": subf2,
                "test.ch3": subf1
            });

            rdb1.pub("test.ch2", "test value 2");

            coroutine.sleep(100);
            assert.equal(c, "test.ch2");
            assert.equal(m, "test value 2");
            assert.equal(n2, 1);

            rdb1.pub("test.ch3", "test value 3");

            coroutine.sleep(100);
            assert.equal(c, "test.ch3");
            assert.equal(m, "test value 3");
            assert.equal(n1, 1);

            rdb1.pub("test.ch1", "test value 1");

            coroutine.sleep(100);
            assert.equal(n1, 2);
        });

        it("unsub", function() {
            n1 = 0;
            rdb.unsub("test.ch1");
            rdb1.pub("test.ch1", "test value 4");

            coroutine.sleep(100);
            assert.equal(m, "test value 1");
            assert.equal(n1, 0);

            rdb.unsub(["test.ch2", "test.ch3"]);

            rdb1.pub("test.ch2", "test value 5");
            rdb1.pub("test.ch3", "test value 6");

            coroutine.sleep(100);
            assert.equal(m, "test value 1");
            assert.equal(n1, 0);
        });

        it("psub", function() {
            n1 = 0;
            rdb.psub("test.*", subf1);

            rdb1.pub("test.ch100", "test value 100");

            coroutine.sleep(100);
            assert.equal(c, "test.ch100");
            assert.equal(m, "test value 100");
            assert.equal(p, "test.*");
            assert.equal(n1, 1);
        });

        it("unpsub", function() {
            n1 = 0;
            rdb.unpsub("test.*");
            rdb1.pub("test.ch200", "test value 200");

            coroutine.sleep(100);
            assert.equal(c, "test.ch100");
            assert.equal(m, "test value 100");
            assert.equal(p, "test.*");
            assert.equal(n1, 0);
        });

        it("onsuberror", function() {
            var err_num = 0;

            function my_err() {
                err_num++;
            }

            rdb1.onsuberror(my_err);
            assert.throws(function() {
                rdb1.command("xxxxx");
            });
            coroutine.sleep(100);
            assert.equal(err_num, 0);

            rdb.onsuberror(my_err);
            rdb.close();
            coroutine.sleep(100);
            assert.equal(err_num, 1);
        });

        it("Memory Leak detect", function() {
            rdb = undefined;
            GC();
            var no1 = os.memoryUsage().nativeObjects.objects;

            rdb = db.open(dbs);

            assert.equal(no1 + 3, os.memoryUsage().nativeObjects.objects);

            rdb.sub("test.ch1", subf1);

            coroutine.sleep(100);

            GC();
            assert.equal(no1 + 3, os.memoryUsage().nativeObjects.objects);

            rdb.close();
            coroutine.sleep(100);

            GC();
            assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);

            rdb = undefined;

            GC();
            assert.equal(no1, os.memoryUsage().nativeObjects.objects);
        });

    });
});

// test.run(console.DEBUG);
