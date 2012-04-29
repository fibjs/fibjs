/**
 * 
 */

var coroutine = require('coroutine');

var e = new coroutine.EventObject();

function t() {
	console.log('test');
}

e.on('test', t);
e.trigger('test');

e.off('test', t);
e.trigger('test');

e.once('test', t);
e.trigger('test');
e.trigger('test');

coroutine.sleep(1000);
