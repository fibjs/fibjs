exports.foo = () => {
    return 1;
};

exports.arg_count = function() {
    return arguments.length;
}

exports.arg_obj = (o) => {
    return o;
}
