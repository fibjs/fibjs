var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var base32 = require('base32');
var hex = require('hex');
var util = require('util');

describe('huge buffer', () => {
    it("FIX: big Buffer base64 encode", () => {
        var buf = Buffer.alloc(1700601981);
        assert.throws(() => {
            buf.toString('base64');
        });
    });

    it("FIX: big Buffer hex encode", () => {
        var buf = Buffer.alloc(1700601981);
        assert.throws(() => {
            buf.toString('hex');
        });
    });

    it("FIX: big string format", () => {
        // 原用例写的是 `base32.encode(530543385)`，依赖 2021 年那版 IDL 把
        // **数字隐式当作 Buffer 长度**的宽松转换（见 #658）。该转换在 IDL 收紧为
        // `Buffer|String` 后被移除，于是这条用例退化成了"验证参数校验"。
        //
        // 而且原来那个尺寸本身就不成立：base32 输出 ceil(n*8/5) = 848869416
        // 字符，已经超过 v8::String::kMaxLength（(1<<29)-24 = 536870888），
        // 就算转换还在也只会抛 RangeError。
        //
        // 这里改成它本来想覆盖的场景：**一个大字符串**交给 util.format 不应崩。
        // base32 不补 padding，输出长度 = ceil(n * 8 / 5)。
        var n = 84858;
        var s = base32.encode(Buffer.alloc(n));
        assert.strictEqual(s.length, Math.ceil(n * 8 / 5));
        assert.strictEqual(util.format(s), s);
    });

    it("FIX: big Buffer Iterator", () => {
        var buf = global.Buffer.alloc(1498850885);
        buf.values();
    });

    it("FIX: huge string transfer between fibjs and v8", () => {
        // 同上：`hex.encode(502045304)` 原意是编码一个 502045304 字节的 Buffer，
        // hex 输出是它的两倍（1004090608 字符）。但 1004090608 远超
        // v8::String::kMaxLength（536870888），在任何实现下都不可能构造出这个
        // 字符串 —— 这条断言从写下那天起就不可能通过。
        //
        // 改成真正能覆盖"fibjs 的 exlib::string 与 V8 之间零拷贝传递大字符串"
        // 的尺寸：接近但不超过 kMaxLength。
        var n = 100000000;                       // 1 亿字节 -> 2 亿字符
        var s = hex.encode(Buffer.alloc(n));
        assert.strictEqual(s.length, n * 2);
        assert.strictEqual(util.format(s), s);
    });

    // -------------------------------------------------------------------
    // V8 的字符串上限是 v8::String::kMaxLength = (1<<29)-24 = 536870888
    //（64 位），比 INT_MAX 小得多。NewString 曾经只校验 INT_MAX，于是
    // 536870889..INT_MAX 之间的长度会让 NewExternalOneByte / NewFromOneByte
    // 失败，而失败结果被 .FromMaybe() 静默吞掉 —— 调用方拿到 undefined
    // 而不是异常：
    //
    //   hex.encode(Buffer.alloc(300000000))    // 输出 6 亿字符
    //   旧行为：返回 undefined（512 MB 白占，调用方紧接着 "Cannot read
    //           properties of undefined"）
    //   期望：  RangeError
    //
    // 这几条用例把边界钉住。都放在 huge_test 里，因为峰值内存是 500 MB 级。
    // -------------------------------------------------------------------
    var V8_MAX = 536870888;                      // (1<<29)-24

    it("hex encode throws at the V8 string limit", () => {
        // 最后一个能成功的：2 * 268435444 = 536870888
        var ok = hex.encode(Buffer.alloc(V8_MAX / 2));
        assert.strictEqual(ok.length, V8_MAX);

        // 再大一个字节就超限，必须抛 RangeError（不是返回 undefined）
        assert.throws(() => hex.encode(Buffer.alloc(V8_MAX / 2 + 1)), RangeError);
    });

    it("base32 encode throws past the V8 string limit", () => {
        // base32 输出 = ceil(n*8/5)，找到刚好超过 V8_MAX 的 n
        var n = Math.floor(V8_MAX * 5 / 8) + 1;
        assert.ok(Math.ceil(n * 8 / 5) > V8_MAX);
        assert.throws(() => base32.encode(Buffer.alloc(n)), RangeError);
    });

    it("Buffer.toString('hex' / 'base64') throws, never returns undefined", () => {
        // 各编码的展开比例不同，要用各自的越界尺寸：
        //   hex       2n         -> n = V8_MAX/2 + 1
        //   base64    ceil(n/3)*4 -> n = V8_MAX*3/4 + 1
        assert.throws(() => Buffer.alloc(V8_MAX / 2 + 1).toString('hex'),
            RangeError, 'hex');
        var n64 = Math.floor(V8_MAX * 3 / 4) + 1;
        assert.throws(() => Buffer.alloc(n64).toString('base64'),
            RangeError, 'base64');
        assert.throws(() => Buffer.alloc(n64).toString('base64url'),
            RangeError, 'base64url');
    });

    it("latin1 / ucs2 toString also throw past the limit", () => {
        // 这两条走 NewStringOneByte / NewStringTwoByte，路径不同但上限相同
        var one = Buffer.alloc(V8_MAX + 1);
        assert.throws(() => one.toString('latin1'), RangeError);
        var two = Buffer.alloc((V8_MAX + 1) * 2);
        assert.throws(() => two.toString('ucs2'), RangeError);
    });
});

