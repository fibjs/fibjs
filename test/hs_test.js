var test = require("test");
var hs = require("hyperscan");
test.setup();

describe("hs", () => {
	describe("single pattern", () => {
		it("use 'L' flag", () => {
			var reg = hs.compile("hello", "L");
			var res = reg.scan("this is a hello world test text");
			assert.deepEqual(res, {
				"hello": [
					[10, 15]
				]
			})
		})
		it("without 'L' flag", () => {
			var reg = hs.compile("hello");
			var res = reg.scan("this is a hello world test text");
			assert.deepEqual(res, {
				"hello": [
					[0, 15]
				]
			})
		})
		it("default 'g' flag", () => {
			var reg = hs.compile("hello");
			var res = reg.scan("this is a hello world hello test text");
			assert.deepEqual(res, {
				"hello": [
					[0, 15],
					[0, 27]
				]
			})
		})
		it("use 'H' flag", () => {
			var reg = hs.compile("hello", "H");
			var res = reg.scan("this is a hello world hello test text");
			assert.deepEqual(res, {
				"hello": [
					[0, 15]
				]
			})
		})
		it("use 'i' flag", () => {
			var reg = hs.compile("Hello", "Li");
			var res = reg.scan("this is a hello world hello test text");
			assert.deepEqual(res, {
				"Hello": [
					[10, 15],
					[22, 27]
				]
			})
		})
		it("use 'm' flag", () => {
			var reg = hs.compile("/\*(.*?)\*/", "m");
			var res = reg.scan("/* this is a \n comment */");
			assert.deepEqual(res, {
				"/*(.*?)*/": [
					[0, 1],
					[0, 25]
				]
			})
		})
		it("use 's' flag", () => {
			var reg = hs.compile("/\*(.*?)\*/", "s");
			var res = reg.scan("/* this is a \n comment */");
			assert.deepEqual(res, {
				"/*(.*?)*/": [
					[0, 1],
					[0, 25]
				]
			})
		})
		it("not match", () => {
			var reg = hs.compile("hello");
			var res = reg.scan("only world");
			assert.isNull(res);
		})
	})
	describe("multi pattern", () => {
		it("use 'L' flag", () => {
			var reg = hs.compile(["hello", "world", "hello.*world"], ["L", "L", "L"]);
			var res = reg.scan("this is a hello world test text");
			assert.deepEqual(res, {
				"hello": [
					[10, 15]
				],
				"hello.*world": [
					[10, 21]
				],
				"world": [
					[16, 21]
				]
			})
		})
		it("without 'L' flag", () => {
			var reg = hs.compile(["hello", "world", "hello.*world"]);
			var res = reg.scan("this is a hello world test text");
			assert.deepEqual(res, {
				"hello": [
					[0, 15]
				],
				"hello.*world": [
					[0, 21]
				],
				"world": [
					[0, 21]
				]
			})
		})
	})
	describe("scan buffer with encoding", () => {
		it("use 'L' flag", () => {
			var reg = hs.compile("测试", "L");
			var res = reg.scan(new Buffer("这是一个测试", "gbk"), "gbk");
			assert.deepEqual(res, {
				"测试": [
					[12, 18]
				]
			})
		})
		it("without 'L' flag", () => {
			var reg = hs.compile("测试");
			var res = reg.scan(new Buffer("这是一个测试", "gbk"), "gbk");
			assert.deepEqual(res, {
				"测试": [
					[0, 18]
				]
			})
		})
		it("default 'g' flag", () => {
			var reg = hs.compile("测试");
			var res = reg.scan(new Buffer("this is a 测试 world 测试 test text", "gbk"), "gbk");
			assert.deepEqual(res, {
				"测试": [
					[0, 16],
					[0, 29]
				]
			})
		})
		it("use 'H' flag", () => {
			var reg = hs.compile("测试", "H");
			var res = reg.scan(new Buffer("this is a 测试 world 测试 test text", "gbk"), "gbk");
			assert.deepEqual(res, {
				"测试": [
					[0, 16]
				]
			})
		})
		it("use 'i' flag", () => {
			var reg = hs.compile("测试", "Li");
			var res = reg.scan(new Buffer("this is a 测试 world 测试 test text", "gbk"), "gbk");
			assert.deepEqual(res, {
				"测试": [
					[10, 16],
					[23, 29]
				]
			})
		})
		it("use 'm' flag", () => {
			var reg = hs.compile("/\*(.*?)\*/", "m");
			var res = reg.scan(new Buffer("/* this is a \n comment */", "gbk"), "gbk");
			assert.deepEqual(res, {
				"/*(.*?)*/": [
					[0, 1],
					[0, 25]
				]
			})
		})
		it("use 's' flag", () => {
			var reg = hs.compile("/\*(.*?)\*/", "s");
			var res = reg.scan(new Buffer("/* this is a \n comment */", "gbk"), "gbk");
			assert.deepEqual(res, {
				"/*(.*?)*/": [
					[0, 1],
					[0, 25]
				]
			})
		})
		it("not match", () => {
			var reg = hs.compile("hello");
			var res = reg.scan(new Buffer("only world", "gbk"), "gbk");
			assert.isNull(res);
		})
	})
});

repl && test.run(console.DEBUG);