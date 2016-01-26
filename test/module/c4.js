exports.foo = function () {
    return 1;
};

exports.arg_count = function()
{
	return arguments.length;
}

exports.arg_obj = function(o)
{
	return o;
}