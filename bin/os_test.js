console.log('os testing....');

var assert = require('assert');
var os = require('os');
var io = require('io');

console.log(os.CPUs());
console.log(os.uptime(), os.loadavg(), os.totalmem(), os.freemem());
console.dir(os.CPUInfo());
console.dir(os.networkInfo());

var tms = [ '98-4-14', '1998-4', '1998-4-14', '1998-4-14 12:00',
		'1998-4-14 1:00 pm', '1998-4-14 12:12:12.123', '4/14/1998', '4/14',
		'4/14/48', '4/14/49', '4/14/50', '4/14/51',
		'1998-04-14 1:12:12.123 pm', '1998/4/14 12:12:12', '98/4/14 12:12:12',
		'12/4/14 12:12:12', 'Tue Apr 14 1998 09:46:05 GMT+0800 (CST)',
		'Tue 14 Apr 1998 09:46:05 GMT+0800', 'Tue Apr 14 1998 09:46:05 GMT',
		'Tue 14 Apr 1998 09:46:05 GMT', 'Tue Apr 14 1998 09:46:05',
		'Tue 14 Apr 1998 09:46:05', '1998-04-14T12:12:12.123Z' ];

tms.forEach(function(s) {
	assert.equal(os.time(s), new Date(s));
});

assert.equal(os.time(), new Date());
