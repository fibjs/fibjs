const { Console } = require('console');

var result;

const con = new Console({
    write: (x) => {
        result = x;
    }
}, process.stderr, false);

function ucs32(code) {
    if (code < 0xd800)
        return String.fromCharCode(code);

    return String.fromCharCode(((code - 0x10000) >> 10) + 0xd800)
        + String.fromCharCode((code & 0x3ff) + 0xdc00);
}

function charWidth(txt) {
    const regex1 = /│ *Values *│/g;
    con.table([`---${txt}---`]);
    var width = regex1.exec(result)[0].length - 12;
    return result.indexOf('\\') == -1 ? width : 1;
}

var wid_1 = {
    1564: 1,
    6158: 1,
    8203: 1,
    8204: 1,
    8206: 1,
    8207: 1,
    8234: 1,
    8235: 1,
    8236: 1,
    8237: 1,
    8238: 1,
    8288: 1,
    8289: 1,
    8290: 1,
    8291: 1,
    8292: 1,
    8294: 1,
    8295: 1,
    8296: 1,
    8297: 1,
    8298: 1,
    8299: 1,
    8300: 1,
    8301: 1,
    8302: 1,
    8303: 1,
    113824: 1,
    113825: 1,
    113826: 1,
    113827: 1,
    119155: 1,
    119156: 1,
    119157: 1,
    119158: 1,
    119159: 1,
    119160: 1,
    119161: 1,
    119162: 1,
    917505: 1,
}

var w1 = 0;
var cw = 1;
var ws = [];

for (var i = 127; i < 0x100000; i++) {
    var txt = ucs32(i);
    var w = charWidth(txt);

    if (w == 0)
        w = wid_1[i] ? 0 : 1;
    else if (w == 3)
        w = 1;

    if (w != cw) {
        ws.push([w1, cw]);
        w1 = i;
        cw = w;
    }
}

ws.push([w1, cw]);

var txts = [];
ws.forEach((w, i) => {
    txts.push(`{0x${w[0].toString(16)}, ${w[1]}},`);
    if (txts.length == 6) {
        console.log(txts.join(""));
        txts = [];
    }
});
console.log(txts.join(""));