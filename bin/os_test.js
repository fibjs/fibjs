console.log('os testing....');

var assert = require('assert');
var os = require('os');
var io = require('io');

console.log(os.CPUs());

assert.ok(os.exists('os_test.js'));

function unlink(pathname) {
	try {
		os.rmdir(pathname);
	} catch (e) {
	}
}

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

var pathname = 'test_dir';
var pathname1 = 'test1_dir';
unlink(pathname);
unlink(pathname1);

os.mkdir(pathname);
assert.equal(os.exists(pathname), true);

os.rename(pathname, pathname1);
assert.equal(os.exists(pathname), false);
assert.equal(os.exists(pathname1), true);

os.rmdir(pathname1);
assert.equal(os.exists(pathname1), false);

var st = os.stat('.');

assert.equal(st.isDirectory(), true);
assert.equal(st.isFile(), false);
assert.equal(st.isExecutable(), true);
assert.equal(st.isReadable(), true);
assert.equal(st.isWritable(), true);

assert.equal(os.time(), new Date());

//var bs = os.exec('x64_Release\\fibjs exec_test');
var bs = os.exec('Release/fibjs exec_test');
bs.EOL = '\n'
var s;

while((s = bs.readLine()) != null)
	console.log('>>>', s);
