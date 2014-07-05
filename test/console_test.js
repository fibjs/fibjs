var test = require("test");
test.setup();

describe("console", function() {
	it("add", function() {
		console.add({
			type: "console"
		});
		console.add("syslog");

		console.add(["console", {
			type: "syslog"
		}]);

		console.reset();
	});

	it("too many logger", function() {
		console.add(["console", "console", "console", "console",
			"console", "console", "console", "console", "console", "console"
		]);

		assert.throws(function() {
			console.add("console");
		});

		console.reset();
	});

	it("log levels", function() {
		console.add({
			type: "console",
			levels: [console.DEBUG, console.ERROR]
		});

		assert.throws(function() {
			console.add({
				type: "console",
				levels: [console.DEBUG, "other"]
			});
		});

		assert.throws(function() {
			console.add({
				type: "console",
				levels: [console.DEBUG, 100]
			});
		});

		console.reset();
	});

	it("add file logger", function() {
		console.add("console");

		console.add({
			type: "file",
			path: "test_log",
			split: "day",
			count: 12
		});

		assert.throws(function() {
			console.add({
				type: "file"
			});
		});

		assert.throws(function() {
			console.add({
				type: "file",
				path: "test_log",
				count: 10
			});
		}, "Missing split mode.");

		assert.throws(function() {
			console.add({
				type: "file",
				path: "test_log",
				count: 1
			});
		}, "Too few file count.");

		assert.throws(function() {
			console.add({
				type: "file",
				path: "test_log",
				count: 129
			});
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "day"
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "hour"
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "minute"
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "1k"
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "10k"
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "100k"
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "100m"
		});

		console.add({
			type: "file",
			path: "test_log",
			split: "100g"
		});

		assert.throws(function() {
			console.add({
				type: "file",
				path: "test_log",
				split: "100n"
			});
		});

		assert.throws(function() {
			console.add({
				type: "file",
				path: "test_log",
				split: "100n"
			});
		});

		assert.throws(function() {
			console.add({
				type: "file",
				path: "test_log",
				split: "1000k"
			});
		});

		assert.throws(function() {
			console.add({
				type: "file",
				path: "test_log",
				split: "k"
			});
		});

		console.reset();
	});
});

//test.run(console.DEBUG);