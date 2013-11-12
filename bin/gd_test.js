var test = require("test");
test.setup();

var gd = require('gd');
var fs = require('fs');

describe("gd", function() {
	var img = gd.create(440, 240);

	var w = img.colorAllocate(255, 255, 255);
	var blk = img.colorAllocate(0, 0, 0);
	var r = img.colorAllocate(255, 0, 0);
	var g = img.colorAllocate(0, 255, 0);
	var b = img.colorAllocate(0, 0, 255);

	function gd_test(fmt) {
		var f = fs.open('test.pic', 'w');
		img.save(f, fmt);
		f.close();

		f = fs.open('test.pic', 'r');
		var img1 = gd.load(f);
		f.close();

		fs.unlink('test.pic');

		img1 = gd.load(img.getData(fmt));

		assert.equal(img1.width, 440);
		assert.equal(img1.height, 240);
		assert.equal(img1.format, fmt);
	}

	it("pixel", function() {
		img.setPixel(10, 10, w);
		assert.equal(img.getPixel(10, 10), w);
	});

	it("line", function() {
		img.line(10, 20, 30, 40, b);
		assert.equal(img.getPixel(11, 21), b);
	});

	it("rectangle", function() {
		img.rectangle(40, 40, 50, 50, r);
		img.filledRectangle(60, 40, 70, 50, img.colorAllocate(255, 255, 0));

		img.filledRectangle(20, 20, 70, 50, img.colorAllocateAlpha(255, 255,
				255, 64));
	});

	it("transparent", function() {
		img.transparent = blk;
		assert.equal(img.transparent, blk);
	});

	it("thickness", function() {
		img.setThickness(3);
		img.rectangle(60, 60, 70, 70, r);
	});

	it("polygon", function() {
		img.setThickness(1);
		img.openPolygon([ [ 1, 1 ], [ 1, 10 ], [ 10, 15 ], [ 10, 20 ] ], b);
		img.polygon([ [ 101, 101 ], [ 101, 110 ], [ 110, 115 ], [ 110, 100 ] ],
				b);
		img.filledPolygon(
				[ [ 101, 1 ], [ 101, 10 ], [ 110, 15 ], [ 110, 10 ] ], b);
	});

	it("ellipse", function() {
		img.ellipse(80, 10, 30, 50, b);
		img.filledEllipse(80, 100, 30, 50, b);
	});

	it("arc", function() {
		img.arc(20, 70, 80, 60, 45, 90, b);
		img.filledArc(20, 140, 80, 60, 45, 90, b);
		img.filledArc(50, 140, 80, 60, 45, 90, b, gd.CHORD);
		img.filledArc(80, 140, 80, 60, 45, 90, b, gd.NOFILL);
		img.filledArc(110, 140, 80, 60, 45, 90, b, gd.EDGED);
		img.filledArc(140, 140, 80, 60, 45, 90, b, gd.EDGED | gd.NOFILL);
	});

	it("fill", function() {
		img.rectangle(70, 10, 120, 60, r);
		img.fill(80, 40, b);
	});

	it("fillToBorder", function() {
		img.rectangle(5, 30, 50, 80, r);
		img.fillToBorder(10, 40, r, b);
	});

	it("png", function() {
		gd_test(gd.PNG);
	});

	it("jpeg", function() {
		gd_test(gd.JPEG);
	});

	it("gif", function() {
		gd_test(gd.GIF);
	});

	it("tiff", function() {
		gd_test(gd.TIFF);
	});

	it("bmp", function() {
		gd_test(gd.BMP);
	});

	it("gd", function() {
		gd_test(gd.GD);
	});

	it("gd2", function() {
		gd_test(gd.GD2);
	});

	it("flip", function() {
		img.save(fs.open('test.png', 'w'), gd.PNG);
		img1 = img.resample(220, 120);
		img1.flip();
		img1.save(fs.open('test1.png', 'w'), gd.PNG);
	});
});

//test.run();
