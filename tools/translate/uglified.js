/* eslint-disable */
// BEGIN
function sM(a) {
    var b;
    if (null !== yr)
        b = yr;
    else {
        b = wr(String.fromCharCode(84));
        var c = wr(String.fromCharCode(75));
        b = [b(), b()];
        b[1] = c();
        b = (yr = window[b.join(c())] || "") || "";
    }
    var d = wr(String.fromCharCode(116)), c = wr(String.fromCharCode(107)), d = [d(), d()];
    d[1] = c();
    c = "&" + d.join("") + "=";
    d = b.split(".");
    b = Number(d[0]) || 0;
    for (var e = [], f = 0, g = 0; g < a.length; g++) {
        var l = a.charCodeAt(g);
        128 > l ? e[f++] = l : (2048 > l ? e[f++] = l >> 6 | 192 : (55296 == (l & 64512) && g + 1 < a.length && 56320 == (a.charCodeAt(g + 1) & 64512) ? (l = 65536 + ((l & 1023) << 10) + (a.charCodeAt(++g) & 1023),
            e[f++] = l >> 18 | 240,
            e[f++] = l >> 12 & 63 | 128) : e[f++] = l >> 12 | 224,
            e[f++] = l >> 6 & 63 | 128),
            e[f++] = l & 63 | 128);
    }
    a = b;
    for (f = 0; f < e.length; f++)
        a += e[f],
            a = xr(a, "+-a^+6");
    a = xr(a, "+-3^+b+-f");
    a ^= Number(d[1]) || 0;
    0 > a && (a = (a & 2147483647) + 2147483648);
    a %= 1E6;
    return c + (a.toString() + "." + (a ^ b));
}
var yr = null;
var wr = function (a) {
    return function () {
        return a;
    };
}, xr = function (a, b) {
    for (var c = 0; c < b.length - 2; c += 3) {
        var d = b.charAt(c + 2), d = "a" <= d ? d.charCodeAt(0) - 87 : Number(d), d = "+" == b.charAt(c + 1) ? a >>> d : a << d;
        a = "+" == b.charAt(c) ? a + d & 4294967295 : a ^ d;
    }
    return a;
};
var window = {
    TKK: '0'
};
// END
/* eslint-enable */
const http = require('http');
/**
 * @why in google translation service, when you post
 * `https://translate.google.{TLD}/translate_a/t`, you must
 * provide source text in body WITH encoded key like `705518.787302`
 * in querystring field `tk`, which is FULLLY
 * from source text. If we don't post the filed `tk`, google translation
 * service would refusing us and respond 403 error.
 *
 * the core method `sM` is extracted from uglifed JS of `https://translate.google.{TLD}`
 *
 * @param {@} text text string to translate
 */
exports.encodeTextToTk = function encodeTextToTk(text, tld = 'cn') {
    var now = Math.floor(Date.now() / 3600000);
    if (Number(window.TKK.split('.')[0]) !== now) {
        var res = http.get(`https://translate.google.${tld}`);
        var matches = res.data.toString().match(/tkk:\s?'(.+?)'/i);
        if (matches)
            window.TKK = matches[1];
    }
    var tk = sM(text);
    tk = tk.replace('&tk=', '');
    return tk;
};
