exports.find_least_common_str = function (str1 = '', str2 = '') {
    let idx = 0;
    while (str1[idx] === str2[idx]) idx++;

    return str1.slice(0, idx);
}

exports.ensure_prefix = function (base = '', prefix = '/') {
    if (base.slice(prefix.length) !== prefix)
        base = prefix + base

    return base;
}

exports.ensure_unprefix = function (base = '', prefix = '/') {
    const len = prefix.length
    if (base.slice(0, len) === prefix)
        base = base.slice(len)

    return base;
}

exports.ensure_suffx = function (base = '', suffix = '/') {
    if (base.slice(base.length - suffix.length) !== suffix)
        base += suffix

    return base;
}

exports.ensure_unsuffx = function (base = '', suffix = '/') {
    const lidx = base.lastIndexOf(suffix)
    if (base.slice(lidx) === suffix)
        base = base.slice(0, lidx)

    return base;
}

