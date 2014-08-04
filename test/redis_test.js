var test = require("test");
test.setup();

var db = require('db');
var encoding = require('encoding');
var coroutine = require('coroutine');

var rdb = db.open("redis://192.168.65.155");

describe("redis", function() {
	before(function() {
		var keys = rdb.command("keys", "*").toArray();
		if (keys.length)
			rdb.del(keys);
	});

	after(function() {
		var keys = rdb.command("keys", "*").toArray();
		if (keys.length)
			rdb.del(keys);
	});

	it("command", function() {
		rdb.command("set", "test", "aaa");
		rdb.command("set", "test1", "aaa");
		assert.equal(rdb.command("get", "test"), "aaa");

		assert.equal(rdb.command("exists", "test"), 1);
		assert.deepEqual(rdb.command("keys", "*").toArray(), ["test", "test1"]);
		assert.deepEqual(rdb.command("keys", "aa*").toArray(), []);

		assert.deepEqual(rdb.command("scan", "0")[1].toArray(), ["test", "test1"]);
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
		assert.deepEqual(rdb.mget("test", "test1").toArray(), ["aaa", "aaa"]);
		assert.deepEqual(rdb.mget(["test", "test1"]).toArray(), ["aaa", "aaa"]);
		assert.deepEqual(rdb.mget("test", "test2").toArray(), ["aaa", null]);
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
		assert.deepEqual(rdb.mget("test", "test1").toArray(), ["bbb", "bbb1"]);

		rdb.mset(["test", "bbb1", "test1", "bbb2"]);
		assert.deepEqual(rdb.mget("test", "test1").toArray(), ["bbb1", "bbb2"]);
	});

	it("msetNX", function() {
		rdb.msetNX("test", "bbb", "test2", "bbb1");
		assert.deepEqual(rdb.mget("test", "test1", "test2").toArray(), ["bbb1", "bbb2", null]);
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
		assert.deepEqual(rdb.keys("*").toArray(), ["test", "test1"]);
	});

	it("del", function() {
		assert.deepEqual(rdb.command("keys", "*").toArray(), ["test", "test1"]);
		assert.equal(rdb.del("test", "test1"), 2);
		assert.deepEqual(rdb.command("keys", "*").toArray(), []);

		rdb.set("test", "aaa");
		rdb.set("test1", "aaa");

		assert.deepEqual(rdb.command("keys", "*").toArray(), ["test", "test1"]);
		assert.equal(rdb.del(["test", "test1"]), 2);
		assert.deepEqual(rdb.command("keys", "*").toArray(), []);

		assert.equal(rdb.del(["test", "test1"]), 0);
	});

	it("dump", function() {
		rdb.set("greeting", "hello, dumping world!");
		assert.equal(rdb.dump("greeting").hex(), "001568656c6c6f2c2064756d70696e6720776f726c6421060045a05a82d872c1de");
		assert.isNull(rdb.dump("greeting1"));
		rdb.del("greeting");
	});

	it("restore", function() {
		rdb.restore("greeting", encoding.hexDecode("001568656c6c6f2c2064756d70696e6720776f726c6421060045a05a82d872c1de"));
		assert.equal(rdb.command("get", "greeting"), "hello, dumping world!");
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

			assert.deepEqual(hash.keys().toArray(), ["test"]);
		});

		it("getAll", function() {
			var hash = rdb.getHash("testHash");

			assert.deepEqual(hash.getAll().toArray(), ["test", "hash aaaa"]);
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
			hash.mset(["test3", "bbb3", "test4", "bbb4"]);

			assert.deepEqual(hash.mget("test1", "test2").toArray(), ["bbb1", "bbb2"]);
			assert.deepEqual(hash.mget(["test1", "test2"]).toArray(), ["bbb1", "bbb2"]);
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
			assert.equal(list.push("a4", "a5", "a6"), 6);
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
			assert.deepEqual(list.range(0, 4).toArray(), ["hash aaaa", "a4", "a2", "a1", "a0"]);
		});

		it("insert", function() {
			var list = rdb.getList("testList");

			assert.equal(list.insertBefore("a4", "a3"), 6);
			assert.deepEqual(list.range(0, 4).toArray(), ["hash aaaa", "a3", "a4", "a2", "a1"]);

			assert.equal(list.insertAfter("a4", "a9"), 7);
			assert.deepEqual(list.range(0, 4).toArray(), ["hash aaaa", "a3", "a4", "a9", "a2"]);
		});

		it("remove", function() {
			var list = rdb.getList("testList");

			assert.equal(list.remove(1, "a4"), 1);
			assert.deepEqual(list.range(0, 4).toArray(), ["hash aaaa", "a3", "a9", "a2", "a1"]);
		});

		it("trim", function() {
			var list = rdb.getList("testList");

			list.trim(1, 3);
			assert.deepEqual(list.range(0, 2).toArray(), ["a3", "a9", "a2"]);
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

});

test.run(console.DEBUG);