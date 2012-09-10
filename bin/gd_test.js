console.log('gd testing....');

var assert = require('assert');
var gd = require('gd');
var io = require('io');
var os = require('os');

function gd_test(fmt){
	var img = gd.create(440, 240);
	var f = io.open('test.pic', 'w');
	img.save(f, fmt);
	f.close();

	f = io.open('test.pic', 'r');
	var img1 = gd.loadFrom(f);
	f.close();

	os.unlink('test.pic');

	img1 = gd.load(img.getData(fmt));
}
gd_test(gd.PNG);
gd_test(gd.JPEG);
gd_test(gd.GIF);
gd_test(gd.TIFF);
gd_test(gd.BMP);
gd_test(gd.GD);
gd_test(gd.GD2);
