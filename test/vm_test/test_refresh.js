var n = 100;

module.n = 100;
require.n = 100;
exports.n = 100;

exports.test = () => {
    return {
        n: ++n,
        'module.n': ++module.n,
        'require.n': ++require.n,
        'exports.n': ++exports.n
    };
}