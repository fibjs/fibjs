var test = require("test");
test.setup();

var os = require('os');
var io = require('io');

describe('os', function() {
	it('stat', function() {
		console.dir({
			CPUs : os.CPUs(),
			uptime : os.uptime(),
			loadavg : os.loadavg(),
			totalmem : os.totalmem(),
			freemem : os.freemem()
		});
		console.dir(os.CPUInfo());
		console.dir(os.networkInfo());
	});

	it('time', function() {
		var tms = [ '98-4-14', '1998-4', '1998-4-14', '1998-4-14 12:00',
		    		'1998-4-14 1:00 pm', '1998-4-14 12:12:12.123', '4/14/1998', '4/14',
		    		'4/14/48', '4/14/49', '4/14/50', '4/14/51',
		    		'1998-04-14 1:12:12.123 pm', '1998/4/14 12:12:12', '98/4/14 12:12:12',
		    		'12/4/14 12:12:12', 'Tue Apr 14 1998 09:46:05 GMT+0800 (CST)',
		    		'Tue 14 Apr 1998 09:46:05 GMT+0800', 'Tue Apr 14 1998 09:46:05 GMT',
		    		'Tue 14 Apr 1998 09:46:05 GMT', 'Tue Apr 14 1998 09:46:05',
		    		'Tue 14 Apr 1998 09:46:05', '1998-04-14T12:12:12.123Z' ];

		    tms.forEach(function(s) {
		    	assert.deepEqual(os.time(s), new Date(s));
		    });

		    var tmse = [ '2000-1-32', '2001-2-29', '2000-13-1', '2000-12-32', '13/13/13' ];

		    tmse.forEach(function(t) {
		    	assert.ok(isNaN(os.time(t).getTime()), t);
		    });

		    var tmso = [ '2000-1-31', '2001-2-28', '2000-12-1', '2000-12-31', '12/13/13' ];

		    tmso.forEach(function(t) {
		    	assert.ok(!isNaN(os.time(t).getTime()), t);
		    });
});
});

//test.run();
