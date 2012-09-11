console.log('gd testing....');

var assert = require('assert');
var gd = require('gd');
var io = require('io');
var os = require('os');

function gd_test(fmt) {
	var f = io.open('test.pic', 'w');
	img.save(f, fmt);
	f.close();

	f = io.open('test.pic', 'r');
	var img1 = gd.loadFrom(f);
	f.close();

	os.unlink('test.pic');

	img1 = gd.load(img.getData(fmt));

	assert.equal(img1.width, 440);
	assert.equal(img1.height, 240);
	assert.equal(img1.format, fmt);
}

var img = gd.create(440, 240);

var w = img.colorAllocate(255, 255, 255);
img.setPixel(10, 10, w);
assert.equal(img.getPixel(10, 10), w);

var b = img.colorAllocate(0, 0, 255);
img.line(10, 20, 30, 40, b);
assert.equal(img.getPixel(11, 21), b);

img.rectangle(40, 40, 50, 50, img.colorAllocate(255, 0, 0));
img.filledRectangle(60, 40, 70, 50, img.colorAllocate(255, 255, 0));

img.filledRectangle(20, 20, 70, 50, img.colorAllocateAlpha(255, 255, 255, 64));

img.transparent(img.colorAllocate(0, 0, 0));
img.setThickness(3);
img.rectangle(60, 60, 70, 70, img.colorAllocate(255, 0, 0));

img.setThickness(1);
img.openPolygon([[1, 1], [1, 10], [10, 15], [10, 20]], b);
img.polygon([[101, 101], [101, 110], [110, 115], [110, 100]], b);
img.filledPolygon([[101, 1], [101, 10], [110, 15], [110, 10]], b);

img.ellipse(80, 10, 30, 50, b);
img.filledEllipse(80, 100, 30, 50, b);

img.arc(20, 70, 80, 60, 45, 90, b);
img.filledArc(20, 140, 80, 60, 45, 90, b);
img.filledArc(50, 140, 80, 60, 45, 90, b, gd.CHORD);
img.filledArc(80, 140, 80, 60, 45, 90, b, gd.NOFILL);
img.filledArc(110, 140, 80, 60, 45, 90, b, gd.EDGED);
img.filledArc(140, 140, 80, 60, 45, 90, b, gd.EDGED | gd.NOFILL);

gd_test(gd.PNG);
gd_test(gd.JPEG);
gd_test(gd.GIF);
gd_test(gd.TIFF);
gd_test(gd.BMP);
gd_test(gd.GD);
gd_test(gd.GD2);

img.save(io.open('test.png', 'w'), gd.PNG);

