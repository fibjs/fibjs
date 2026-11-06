// Buffer 编码路径性能基准（fibjs vs node 同脚本对跑）。
//
//   node   test/perf/buffer_encoding_bench.js
//   fibjs  test/perf/buffer_encoding_bench.js
//
// 不进 test/main.js：它是基准而不是断言测试，只用来人工对比两个运行时。
//
// 负载固定 64 KB（hex 行 131072 字符，base64 行 87384 字符），
// 每项 median of 7 轮，用 sink 防 DCE。

var SZ = 65536;
var ROUNDS = 7;

function med(fn, n) {
    for (var i = 0; i < Math.min(50, n); i++) fn();
    var runs = [];
    for (var r = 0; r < ROUNDS; r++) {
        var t0 = process.hrtime.bigint();
        for (var i2 = 0; i2 < n; i2++) fn();
        runs.push(Number(process.hrtime.bigint() - t0) / 1000 / n);
    }
    runs.sort(function (a, b) { return a - b; });
    return runs[(ROUNDS - 1) >> 1];
}

var ascii = Buffer.alloc(SZ, 0x41);
var hi = Buffer.alloc(SZ);
for (var i = 0; i < SZ; i++) hi[i] = 0x80 + (i % 0x78);
var cjk = Buffer.alloc(SZ);
for (var j = 0; j < SZ; j += 3) { cjk[j] = 0xe4; cjk[j + 1] = 0xb8; cjk[j + 2] = 0xad; }

var asciiStr = ascii.toString('latin1');
var hiStr = hi.toString('latin1');
var cjkStr = cjk.toString('utf8');
var hexStr = ascii.toString('hex');
var b64Str = ascii.toString('base64');
var u16Str = ascii.toString('ucs2');

var sink = 0;
function S(v) {
    sink += (typeof v === 'string' ? v.length : (v && v.length) || 1);
    return v;
}

var rows = [
    ['toString utf8 (ascii 64K)', function () { return S(ascii.toString('utf8')); }, 200],
    ['toString utf8 (cjk 64K)', function () { return S(cjk.toString('utf8')); }, 200],
    ['toString hex (64K->128K)', function () { return S(ascii.toString('hex')); }, 200],
    ['toString base64 (64K)', function () { return S(ascii.toString('base64')); }, 200],
    ['toString latin1 (ascii 64K)', function () { return S(ascii.toString('latin1')); }, 200],
    ['toString latin1 (hi 64K)', function () { return S(hi.toString('latin1')); }, 200],
    ['toString ucs2 (64K)', function () { return S(ascii.toString('ucs2')); }, 200],
    ['toString ascii (hi 64K)', function () { return S(hi.toString('ascii')); }, 200],
    ['from utf8 str (ascii 64K)', function () { return S(Buffer.from(asciiStr, 'utf8')); }, 200],
    ['from latin1 str (ascii 64K)', function () { return S(Buffer.from(asciiStr, 'latin1')); }, 200],
    ['from latin1 str (hi 64K)', function () { return S(Buffer.from(hiStr, 'latin1')); }, 200],
    ['from ucs2 str (64K)', function () { return S(Buffer.from(u16Str, 'ucs2')); }, 200],
    ['from hex str (128K chars)', function () { return S(Buffer.from(hexStr, 'hex')); }, 100],
    ['from base64 str (87K chars)', function () { return S(Buffer.from(b64Str, 'base64')); }, 100],
    ['write utf8 (ascii 64K)', function () { return S(Buffer.allocUnsafe(SZ).write(asciiStr)); }, 200],
    ['write latin1 (hi 64K)', function () { return S(Buffer.allocUnsafe(SZ).write(hiStr, 'latin1')); }, 200],
    ['byteLength utf8 (ascii 64K)', function () { return S(Buffer.byteLength(asciiStr, 'utf8')); }, 500],
    ['byteLength utf8 (cjk 64K)', function () { return S(Buffer.byteLength(cjkStr, 'utf8')); }, 500],
    ['byteLength latin1 (hi 64K)', function () { return S(Buffer.byteLength(hiStr, 'latin1')); }, 500],
    ['byteLength base64 (87K)', function () { return S(Buffer.byteLength(b64Str, 'base64')); }, 500],
    ['byteLength hex (128K)', function () { return S(Buffer.byteLength(hexStr, 'hex')); }, 500]
];

var runtime = (typeof process !== 'undefined' && process.versions && process.versions.fibjs)
    ? 'fibjs' : 'node';

console.log('# ' + runtime + ' / ' + SZ + ' bytes / median of ' + ROUNDS);
for (var k = 0; k < rows.length; k++)
    console.log(rows[k][0] + '\t' + med(rows[k][1], rows[k][2]).toFixed(2));

if (!sink)
    console.log('(sink empty - measurement was optimized away)');
