/**
 * 
 */

define([ 'aaa4' ], function(aaa4) {
	return {
		aaa4 : aaa4
	};
});

define('aaa4', [ 'aaa5', 'exports' ], function(aaa5, exports) {
	exports.aaa5 = aaa5;
});

define('aaa5', [ 'aaa4', 'exports' ], function(aaa4, exports) {
	exports.v = 200;
});
