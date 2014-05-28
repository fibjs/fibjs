exports.func = function() {
	return new Error().stack;
};