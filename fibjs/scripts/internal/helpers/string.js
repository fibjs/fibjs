exports.find_least_common_str = function (str1 = '', str2 = '') {
    var idx = 0;
    var str1_parts = str1.split('/');
    var str2_parts = str2.split('/');

    while (idx < str1_parts.length - 1 && str1_parts[idx] === str2_parts[idx])
        idx++;

    if (idx === 0)
        return '';

    return str1_parts.slice(0, idx).join('/') + '/';
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

