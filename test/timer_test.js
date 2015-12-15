var test = require("test");
test.setup();

var coroutine = require("coroutine");
var os = require("os");

describe("timer", function() {
	it("setTimeout", function(argument) {
		var n = 0;

		GC();
		var no1 = os.memoryUsage().nativeObjects.objects;

		setTimeout(function() {
			n = 1;
		}, 1);

		GC();
		var no2 = os.memoryUsage().nativeObjects.objects;
		assert.equal(no1 + 1, no2);

		assert.equal(n, 0);
		coroutine.sleep(10);
		assert.equal(n, 1);

		GC();
		no2 = os.memoryUsage().nativeObjects.objects;
		assert.equal(no1, no2);
	});

	it("clearTimeout", function(argument) {
		var n = 0;

		GC();
		var no1 = os.memoryUsage().nativeObjects.objects;

		var t = setTimeout(function() {
			n = 1;
		}, 1);

		assert.equal(n, 0);
		clearTimeout(t);
		t = undefined;
		coroutine.sleep(10);
		assert.equal(n, 0);

		GC();
		var no2 = os.memoryUsage().nativeObjects.objects;
		assert.equal(no1, no2);
	});

	it("setInterval/clearInterval", function(argument) {
		var n = 0;

		GC();
		var no1 = os.memoryUsage().nativeObjects.objects;

		var t = setInterval(function() {
			if (n < 2)
				n++;
		}, 1);

		GC();
		var no2 = os.memoryUsage().nativeObjects.objects;
		assert.equal(no1 + 1, no2);

		assert.equal(n, 0);
		coroutine.sleep(10);
		assert.equal(n, 2);

		n = 0;
		clearInterval(t);
		t = undefined;
		coroutine.sleep(10);
		assert.equal(n, 0);

		GC();
		var no2 = os.memoryUsage().nativeObjects.objects;
		assert.equal(no1, no2);
	});

	it("clearInterval in callback", function(argument) {
		var n = 0;

		GC();
		var no1 = os.memoryUsage().nativeObjects.objects;

		setInterval(function() {
			n++;
			clearInterval(this);
		}, 1);

		assert.equal(n, 0);
		coroutine.sleep(10);
		assert.equal(n, 1);

		GC();
		var no2 = os.memoryUsage().nativeObjects.objects;
		assert.equal(no1, no2);
	});
});

// test.run(console.DEBUG);