var test = require("test");
test.setup();

describe('Int64', function() {
	it('can be constructed', function testConstructor() {
		var x = new Int64(),
			y = new Int64(42),
			z = new Int64(0xfedcba98, 0x76543210),
			w = new Int64('0xfedcba9876543210'),
			r = new Int64('372528006791240803');
		assert.equal(x.toString(16), '0x0');
		assert.equal(y.toString(16), '0x2a');
		assert.equal(z.toString(16), '0xfedcba9876543210');
		assert.equal(w.toString(16), '0xfedcba9876543210');
		assert.equal(r.toString(16), '0x52b7c3f99ad1c63');
	});

	it('can be converted to Number', function testNumberConversion() {
		var a = new Int64(2),
			b = new Int64(3);
		assert.equal(a + b, 5);
		var x = new Int64(),
			y = new Int64(42),
			z = new Int64(0xfedcba98, 0x76543210),
			w = new Int64('0xfedcba9876543210')
		assert.equal(+x, 0);
		assert.equal(+y, 42);
		assert.equal(+z, Infinity);
		assert.equal(+w, Infinity);
	});

	it('can be compared', function testComparison() {
		var a = new Int64(2),
			b = new Int64(3);
		assert.isTrue(a.equal(a));
		assert.isFalse(a.equal(b));
		assert.equal(a.compare(a), 0);
		assert.equal(a.compare(b), -1);
		assert.equal(b.compare(a), 1);
	});

	it('can be bit-manipulated', function testBitManipulation() {
		var x = new Int64('0x1edcba9876543210');
		assert.equal(x.hi, 0x1edcba98);
		assert.equal(x.lo, 0x76543210);
		var y = x.and(new Int64(0xffff)),
			z = x.or(new Int64(0xffff)),
			w = x.xor(new Int64('0xffffffffffffffff'));
		assert.equal(y.toString(16), '0x3210');
		assert.equal(z.toString(16), '0x1edcba987654ffff');
		assert.equal(w.toString(16), '0xe123456789abcdef');
		assert.equal(x.and(0xffff).toString(16), '0x3210');
		assert.equal(x.or(0xffff).toString(16), '0x1edcba987654ffff');
		assert.equal(x.xor(0xffff).toString(16), '0x1edcba987654cdef');
		assert.equal(x.and(0x1ffffffff).toString(16), '0x76543210');
		assert.equal(x.or(0x1ffffffff).toString(16), '0x1edcba99ffffffff');
		assert.equal(x.xor(0x1ffffffff).toString(16), '0x1edcba9989abcdef');
		var a = new Int64(7),
			b = a.shiftLeft(1),
			c = a.shiftRight(1);
		assert.equal(b.toString(16), '0xe');
		assert.equal(c.toString(16), '0x3');
	});

	it('can be converted to a decimal string', function testDecimalString() {
		var positive = new Int64('0x52B7C3F99AD1C63');
		assert.equal(positive.toString(), '372528006791240803');

		var minusOne = new Int64('0xFFFFFFFFFFFFFFFF');
		assert.equal(minusOne.toString(), '-1');

		var jsOverflow = new Int64('0x2fffffffffffff');
		assert.equal(jsOverflow.toString(), '13510798882111487');

		var decimal = new Int64('13510798882111487');
		assert.equal(decimal.toString(), '13510798882111487');
	});

	it('can be added', function testAdd() {
		var a = new Int64(3),
			b = new Int64(2),
			c = new Int64('0xfffffffffffffffe');
		assert.isTrue(a.add(b).equal(new Int64(5)));
		assert.isTrue(a.add(4).equal(new Int64(7)));

		// unsigned integer overflow
		assert.isTrue(c.add(3).equal(new Int64(1)));

		// numbers larger than int32
		assert.equal(a.add(0x100000000).toString(16), '0x100000003');
	});

	it('can be subtracted', function testSub() {
		var a = new Int64(3),
			b = new Int64(2),
			c = new Int64('0xffffffffffffffff');
		assert.isTrue(a.sub(b).equal(new Int64(1)));
		assert.isTrue(a.sub(1).equal(new Int64(2)));

		// unsigned integer underflow
		assert.isTrue(a.sub(4).equal(new Int64('0xffffffffffffffff')));

		// numbers larger than int32
		assert.equal(c.sub(0x100000000).toString(16), '0xfffffffeffffffff');
	});

	it("toString and decode", function() {
		var n = new Int64(-12345);
		assert.equal(n, -12345);
		assert.equal(n.toString(10), "-12345");

		n = new Int64("-1234567");
		assert.equal(n, -1234567);

		n = new Int64(0xfedcba98, 0x76543210);

		assert.equal(n.toString(2), "1111111011011100101110101001100001110110010101000011001000010000");
		assert.isTrue(n.equal(new Int64("1111111011011100101110101001100001110110010101000011001000010000", 2)));

		assert.equal(n.toString(8), "1773345651416625031020");
		assert.isTrue(n.equal(new Int64("1773345651416625031020", 8)));

		assert.equal(n.toString(16), "0xfedcba9876543210");
		assert.isTrue(n.equal(new Int64("0xfedcba9876543210", 16)));

		assert.equal(n.toString(32), "p5xf2tb3fimqq");
		assert.isTrue(n.equal(new Int64("p5xf2tb3fimqq", 32)));

		assert.equal(n.toString(32), "p5xf2tb3fimqq");
		assert.isTrue(n.equal(new Int64("p5xf2tb3fimqq", 32)));

		assert.equal(n.toString(64), "P7cuph2VDIQ");
		assert.isTrue(n.equal(new Int64("P7cuph2VDIQ", 64)));

		assert.throws(function() {
			n.toString(17);
		});

		assert.throws(function() {
			n.toString(1);
		});

		assert.throws(function() {
			new Int64("111111101101110010", 17);
		});

		assert.throws(function() {
			new Int64("111111101101110010", 1);
		});
	});
});

//test.run(console.DEBUG);