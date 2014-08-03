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

	it("command", function() {
		rdb.command("set", "test", "aaa");
		rdb.command("set", "test1", "aaa");
		assert.equal(rdb.command("get", "test"), "aaa");

		assert.equal(rdb.command("exists", "test"), 1);
		assert.deepEqual(rdb.command("keys", "*").toArray(), ["test", "test1"]);
		assert.deepEqual(rdb.command("keys", "aa*").toArray(), []);

		assert.deepEqual(rdb.command("scan", "0")[1].toArray(), ["test", "test1"]);
		assert.deepEqual(rdb.command("scan", "17")[1].toArray(), []);
	});

	it("exists", function() {
		assert.isTrue(rdb.exists("test"));
		assert.isFalse(rdb.exists("test2"));
	});

	it("get", function() {
		assert.equal(rdb.get("test"), "aaa");
		assert.isNull(rdb.get("test2"));
	});

	it("set", function() {
		rdb.set("test", "aaa1")
		assert.equal(rdb.get("test"), "aaa1");

		rdb.set("test2", "aaa2", 100)
		assert.equal(rdb.get("test2"), "aaa2");
		coroutine.sleep(150);
		assert.isFalse(rdb.exists("test2"));
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
});

test.run(console.DEBUG);