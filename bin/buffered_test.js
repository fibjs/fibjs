/**
 * 
 */

console.log('buffered testing....');

var assert = require('assert');
var fs = require('fs');

var s = '0123456789\r\n';

for ( var i = 0; i < 10; i++)
	s = s + s;

var f = fs.open("test0000", 'w');
f.write(new Buffer(s));
f.close();

function t_read(sz) {
	f = fs.open("test0000");
	var r = new Buffer.Reader(f);
	var p = 0;
	var d;

	while (p < s.length) {
		d = r.read(sz);
		if (!d)
			break;
		assert.equal(d.toString(), s.substring(p, p + sz));
		p += sz;
	}
	f.close();
}

for ( var i = 3; i < 100000; i *= 7)
	t_read(i);

f.close();
fs.unlink("test0000");
