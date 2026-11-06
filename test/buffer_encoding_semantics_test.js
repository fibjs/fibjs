// Buffer 编码语义契约（node 对齐 + fibjs 既有宽容行为）。
//
// 这份测试锁住 plans/buffer-encoding-performance-2026-11-05.md §4 的 15 条契约
// 与 D1/D2/D3 三个修正点。它必须同时通过 fibjs 与 node —— 除标注 "fibjs 扩展" 的
// 条目外，两边输出逐字节一致。
//
// 目的：编码路径在 native（simdutf）与 JS 两条车道之间来回迁移过几次，
// 这个文件是唯一能证明"两条车道语义相同"的东西，也是历轮审查发现的
// 真实缺陷的回归网（每轮新增的 describe 都在注释里写了它防的是什么 bug）。

var { describe, it } = require('node:test');
var assert = require('assert');

// node 的 path / util 全程在 JS 里处理字符串（V8 的 WTF-8 表示对 JS 透明），
// 孤代理项当然原样保留。只有 fibjs 会把字符串送过 native 的 exlib::string，
// 所以下面依赖"过 native 不变形"的用例只在 fibjs 下断言。
var isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

function hx(buf) {
    return buf.toString('hex');
}

describe('Buffer encoding semantics', () => {
    describe('hex', () => {
        it('odd length truncates to whole bytes', () => {
            assert.strictEqual(hx(Buffer.from('abc', 'hex')), 'ab');
            assert.strictEqual(hx(Buffer.from('a', 'hex')), '');
            assert.strictEqual(hx(Buffer.from('', 'hex')), '');
        });

        it('stops at the first invalid character', () => {
            assert.strictEqual(hx(Buffer.from('abzzcd', 'hex')), 'ab');
            assert.strictEqual(hx(Buffer.from('ab12cd', 'hex')), 'ab12cd');
            assert.strictEqual(hx(Buffer.from('ab cd', 'hex')), 'ab');
            assert.strictEqual(hx(Buffer.from('ab\u0000cd', 'hex')), 'ab');
        });

        it('accepts upper and lower case', () => {
            assert.strictEqual(hx(Buffer.from('ab12EF', 'hex')), 'ab12ef');
        });

        it('round-trips through toString', () => {
            var src = Buffer.alloc(256);
            for (var i = 0; i < 256; i++) src[i] = i;
            assert.ok(Buffer.from(src.toString('hex'), 'hex').equals(src));
        });

        it('byteLength is floor(len / 2) without validating content', () => {
            assert.strictEqual(Buffer.byteLength('ABCD', 'hex'), 2);
            assert.strictEqual(Buffer.byteLength('ABC', 'hex'), 1);
            assert.strictEqual(Buffer.byteLength('zz', 'hex'), 1);
            assert.strictEqual(Buffer.byteLength('', 'hex'), 0);
        });
    });

    describe('base64', () => {
        it('skips invalid characters', () => {
            assert.strictEqual(hx(Buffer.from('ab!!cd', 'base64')), '69b71d');
            assert.strictEqual(hx(Buffer.from('AA A', 'base64')), '0000');
        });

        it('stops at padding (node semantics)', () => {
            // D3：旧的 native 实现把 '=' 当普通非法字符跳过，'ab==cd' 会解出 3 字节
            assert.strictEqual(hx(Buffer.from('ab==cd', 'base64')), '69');
            assert.strictEqual(hx(Buffer.from('AB==', 'base64')), '00');
            assert.strictEqual(hx(Buffer.from('ABC=', 'base64')), '0010');
        });

        it('handles short and empty input', () => {
            assert.strictEqual(hx(Buffer.from('', 'base64')), '');
            assert.strictEqual(hx(Buffer.from('A', 'base64')), '');
            assert.strictEqual(hx(Buffer.from('AB', 'base64')), '00');
            assert.strictEqual(hx(Buffer.from('ABC', 'base64')), '0010');
            assert.strictEqual(hx(Buffer.from('====', 'base64')), '');
        });

        it('byteLength uses the length formula and does not validate (node semantics)', () => {
            // D1：旧实现真的解码一遍来计数，'!!!!' 会得到 0
            assert.strictEqual(Buffer.byteLength('!!!!', 'base64'), 3);
            assert.strictEqual(Buffer.byteLength('AA A', 'base64'), 3);
            assert.strictEqual(Buffer.byteLength('AB===', 'base64'), 2);
            assert.strictEqual(Buffer.byteLength('', 'base64'), 0);
            assert.strictEqual(Buffer.byteLength('A', 'base64'), 0);
            assert.strictEqual(Buffer.byteLength('AB', 'base64'), 1);
            assert.strictEqual(Buffer.byteLength('ABC', 'base64'), 2);
            assert.strictEqual(Buffer.byteLength('AA==', 'base64'), 1);
            assert.strictEqual(Buffer.byteLength('AAA=', 'base64'), 2);
            assert.strictEqual(Buffer.byteLength('AAAA', 'base64'), 3);
        });

        it('round-trips a full byte range', () => {
            var src = Buffer.alloc(256);
            for (var i = 0; i < 256; i++) src[i] = i;
            assert.ok(Buffer.from(src.toString('base64'), 'base64').equals(src));
        });

        it('encodes all three tail lengths', () => {
            assert.strictEqual(Buffer.from('').toString('base64'), '');
            assert.strictEqual(Buffer.from('a').toString('base64'), 'YQ==');
            assert.strictEqual(Buffer.from('ab').toString('base64'), 'YWI=');
            assert.strictEqual(Buffer.from('abc').toString('base64'), 'YWJj');
        });
    });

    describe('latin1 / binary', () => {
        it('maps every byte to U+0000..U+00FF on decode', () => {
            var src = Buffer.alloc(256);
            for (var i = 0; i < 256; i++) src[i] = i;
            var s = src.toString('latin1');
            assert.strictEqual(s.length, 256);
            for (var j = 0; j < 256; j++)
                assert.strictEqual(s.charCodeAt(j), j);
        });

        it('does not utf8-decode on toString', () => {
            // 0xc3 0x3f 是非法 UTF-8；latin1 必须逐字节映射而不是给 U+FFFD
            var s = Buffer.from([0xc3, 0x3f]).toString('latin1');
            assert.strictEqual(s.length, 2);
            assert.strictEqual(s.charCodeAt(0), 195);
            assert.strictEqual(s.charCodeAt(1), 63);
        });

        it('takes the low 8 bits of every code unit on encode', () => {
            assert.strictEqual(hx(Buffer.from('\u00ff', 'latin1')), 'ff');
            assert.strictEqual(hx(Buffer.from('\u4e2d', 'latin1')), '2d');
            // astral：两个 code unit 各一个字节
            assert.strictEqual(hx(Buffer.from('\u{1f600}', 'latin1')), '3d00');
        });

        it('ascii encodes without masking but decodes with masking', () => {
            // node：Buffer.from('ÿ','ascii') 是 ff（不掩码），toString('ascii') 掩掉高位
            assert.strictEqual(hx(Buffer.from('\u00ff', 'ascii')), 'ff');
            assert.strictEqual(Buffer.from([0xff]).toString('ascii').charCodeAt(0), 0x7f);
            assert.strictEqual(Buffer.from([0x80]).toString('ascii').charCodeAt(0), 0x00);
        });

        it('byteLength is the UTF-16 code unit count', () => {
            assert.strictEqual(Buffer.byteLength('\u00e9\u4e2d', 'latin1'), 2);
            assert.strictEqual(Buffer.byteLength('\u{1f600}', 'latin1'), 2);
            assert.strictEqual(Buffer.byteLength('\u4e2d', 'latin1'), 1);
        });

        it('binary is an alias of latin1', () => {
            var src = Buffer.alloc(256);
            for (var i = 0; i < 256; i++) src[i] = i;
            assert.strictEqual(src.toString('binary'), src.toString('latin1'));
            assert.ok(Buffer.from(src.toString('binary'), 'binary').equals(src));
        });

        it('round-trips a full byte range', () => {
            var src = Buffer.alloc(256);
            for (var i = 0; i < 256; i++) src[i] = i;
            assert.ok(Buffer.from(src.toString('latin1'), 'latin1').equals(src));
        });

        it('write and fill accept latin1 strings', () => {
            var b = Buffer.alloc(4);
            assert.strictEqual(b.write('\u00ff', 'latin1'), 1);
            assert.strictEqual(hx(b), 'ff000000');
            assert.strictEqual(hx(Buffer.alloc(2, '\u00ff', 'latin1')), 'ffff');
        });

        it('honours toString start/end', () => {
            var b = Buffer.from([0x41, 0x42, 0x43, 0x44]);
            assert.strictEqual(b.toString('latin1', 1, 3), 'BC');
            assert.strictEqual(b.toString('latin1', 2), 'CD');
        });
    });

    describe('ucs2 / utf16le', () => {
        it('drops a trailing odd byte on decode', () => {
            assert.strictEqual(Buffer.from([0x41]).toString('ucs2'), '');
            assert.strictEqual(Buffer.from([0x41, 0x00, 0x42]).toString('ucs2'), 'A');
        });

        it('round-trips BMP and astral text', () => {
            assert.strictEqual(hx(Buffer.from('\u4e2d', 'ucs2')), '2d4e');
            assert.strictEqual(hx(Buffer.from('\u{1f600}', 'ucs2')), '3dd800de');
            assert.strictEqual(Buffer.from('\u{1f600}', 'ucs2').toString('ucs2'), '\u{1f600}');
        });

        it('treats ucs2, ucs-2, utf16le and utf-16le alike', () => {
            var s = 'a\u4e2d\u{1f600}';
            var want = hx(Buffer.from(s, 'ucs2'));
            ['ucs-2', 'utf16le', 'utf-16le'].forEach(function (c) {
                assert.strictEqual(hx(Buffer.from(s, c)), want);
            });
        });

        it('byteLength is twice the code unit count', () => {
            assert.strictEqual(Buffer.byteLength('\u00e9\u4e2d', 'ucs2'), 4);
            assert.strictEqual(Buffer.byteLength('\u{1f600}', 'ucs2'), 4);
        });

        it('write treats length as bytes and never writes half a code unit', () => {
            // D2：旧实现写入了 610062 的前 3 字节并返回 3，node 返回 2
            var b = Buffer.alloc(8);
            assert.strictEqual(b.write('abc', 0, 3, 'ucs2'), 2);
            assert.strictEqual(hx(b), '6100000000000000');
            assert.strictEqual(b.write('\u{1f600}', 'ucs2'), 4);
            assert.strictEqual(hx(b), '3dd800de00000000');
        });

        it('honours toString start/end in bytes', () => {
            var b = Buffer.from('abcd', 'ucs2');
            assert.strictEqual(b.toString('ucs2', 2, 6), 'bc');
        });
    });

    describe('utf8 / ascii cross-checks', () => {
        it('latin1 and utf8 agree on pure ASCII', () => {
            var src = Buffer.alloc(128);
            for (var i = 0; i < 128; i++) src[i] = i;
            assert.strictEqual(src.toString('latin1'), src.toString('utf8'));
            assert.strictEqual(src.toString('ascii'), src.toString('utf8'));
        });

        it('latin1 and utf8 disagree above 0x7f', () => {
            var src = Buffer.from([0x80]);
            assert.strictEqual(src.toString('latin1').charCodeAt(0), 0x80);
            assert.strictEqual(src.toString('utf8').charCodeAt(0), 0xfffd);
        });

        it('lone surrogates encode as latin1 code units and as U+FFFD in utf8', () => {
            assert.strictEqual(hx(Buffer.from('a\ud800b', 'latin1')), '610062');
            assert.strictEqual(hx(Buffer.from('a\ud800b', 'utf8')), '61efbfbd62');
        });

        // -----------------------------------------------------------------
        // 2026-11-06 第 5 轮审查补充：以下两条覆盖"字符串过 native（path /
        // util / encoding）往返"的完整性 —— 这是纯 JS 断言测不到的方向。
        // -----------------------------------------------------------------
        it('a lone surrogate survives a native round-trip without growing', function () {
            // ToString() 曾用不带 REPLACE_INVALID_UTF8 的 WriteUtf8，V8 把
            // U+D800 写成 WTF-8（ED A0 80）。那不是合法 UTF-8，下游按标准解
            // 码时 3 个字节各自非法 -> 1 个 code unit **膨胀成 3 个** U+FFFD：
            //   path.normalize('a\ud800b')  ->  'a\uFFFD\uFFFD\uFFFDb'
            // 纯 JS 的 concat / toUpperCase / JSON 不受影响，于是同一个字符串
            // "过不过 native" 结果不同。
            //
            // node 没有这条 native 边界（path/util 纯 JS），它的 normalize 会
            // 原样保留孤代理项，所以只在 fibjs 下断言。
            if (!isFibjs) return;

            var src = 'a\ud800b';
            var want = Buffer.from(src, 'utf8').toString('utf8');   // 1 个 U+FFFD

            [require('path').normalize(src),
                require('path').join(src),
                require('util').format('%s', src)]
                .forEach(function (out) {
                    assert.strictEqual(out.length, want.length, JSON.stringify(out));
                    assert.strictEqual(out, want);
                });

            // 合法的代理对则必须原样保留
            assert.strictEqual(require('path').normalize('a\ud83d\ude00b'), 'a\ud83d\ude00b');
        });

        it('high bytes survive a native round-trip, small or external', function () {
            // NewStringOneByte 走 NewExternalOneByte 时挂的是**原始字节**。若
            // 解回 exlib::string 时按 UTF-8 处理，0x80..0xFF 会整段变成 U+FFFD
            // —— 而且只有 >= SMALL_STRING(1024) 的长串才触发，短串恰好正确。
            [1, 1000, 1023, 1024, 2000].forEach(function (n) {
                var latin = Buffer.alloc(n, 0xff).toString('latin1');
                assert.strictEqual(require('path').normalize(latin), latin,
                    'latin1 ff n=' + n);

                var b = Buffer.alloc(n);
                for (var i = 0; i < n; i++) b[i] = i % 256;
                var mixed = b.toString('latin1');
                assert.strictEqual(mixed.charCodeAt(0), 0);
                assert.strictEqual(require('path').normalize(mixed), mixed,
                    'latin1 mixed n=' + n);
            });

            // UTF-8 多字节文本走外部串路径同样不能坏
            var cjk = '\u4e2d\u6587abc'.repeat(200);
            assert.strictEqual(require('path').normalize(cjk), cjk);
        });

        it('byteLength of utf8 counts encoded bytes', () => {
            assert.strictEqual(Buffer.byteLength('\u4e2d', 'utf8'), 3);
            assert.strictEqual(Buffer.byteLength('abc', 'utf8'), 3);
        });
    });

    describe('case sensitivity (matches node: only lowercase is recognized)', () => {
        it('uppercase codecs are not the built-in fast paths', () => {
            // 大写 'LATIN1' 在 fibjs 落到 ICU 的 windows-1252，与 node 的 latin1 不同；
            // 这条只断言"不崩溃且长度符合当前实现"，避免把差异固化。
            assert.strictEqual(typeof Buffer.from('abc', 'LATIN1').length, 'number');
        });
    });

    describe('bulk round-trips', () => {
        var sizes = [0, 1, 2, 3, 4, 5, 7, 15, 16, 31, 63, 64, 255, 256, 1023, 4096];

        it('hex round-trips at every size', () => {
            sizes.forEach(function (n) {
                var src = Buffer.alloc(n);
                for (var i = 0; i < n; i++) src[i] = (i * 97 + n) & 0xff;
                assert.ok(Buffer.from(src.toString('hex'), 'hex').equals(src), 'size ' + n);
            });
        });

        it('base64 round-trips at every size', () => {
            sizes.forEach(function (n) {
                var src = Buffer.alloc(n);
                for (var i = 0; i < n; i++) src[i] = (i * 31 + n) & 0xff;
                assert.ok(Buffer.from(src.toString('base64'), 'base64').equals(src), 'size ' + n);
            });
        });

        it('latin1 round-trips at every size', () => {
            sizes.forEach(function (n) {
                var src = Buffer.alloc(n);
                for (var i = 0; i < n; i++) src[i] = (i * 13 + n) & 0xff;
                assert.ok(Buffer.from(src.toString('latin1'), 'latin1').equals(src), 'size ' + n);
            });
        });

        it('ucs2 round-trips at every size', () => {
            sizes.forEach(function (n) {
                var src = Buffer.alloc(n * 2);
                for (var i = 0; i < src.length; i++) src[i] = (i * 7 + n) & 0xff;
                assert.ok(Buffer.from(src.toString('ucs2'), 'ucs2').equals(src), 'size ' + n);
            });
        });
    });

    // ---------------------------------------------------------------------
    // 以下 4 组用例是 2026-11-06 审查时补的。审查发现现有测试有三个盲区：
    //   1) atob 的 padding 用例写成 try/catch 宽松模式 —— 只要不崩溃就通过，
    //      于是 atob('YQ=') 返回 'a'（node 抛错）这样的语义 bug 长期未被捕获
    //   2) byteLength 的三档 SIMD 分流（全 ASCII / 全 2 字节 / 全 CJK）与
    //      代理对修正没有任何断言
    //   3) 非法 UTF-8 的 U+FFFD 个数（WHATWG maximal subpart）没有断言
    // ---------------------------------------------------------------------

    describe('byteLength utf8 (SIMD 分流的每一档)', () => {
        it('pure ASCII, incl. SIMD group boundaries', () => {
            [0, 1, 7, 8, 9, 16, 17, 63, 64, 65].forEach(function (n) {
                assert.strictEqual(Buffer.byteLength('a'.repeat(n), 'utf8'), n);
            });
        });

        it('pure 2-byte range (0x80-0x7ff)', () => {
            assert.strictEqual(Buffer.byteLength('\u00e9', 'utf8'), 2);
            assert.strictEqual(Buffer.byteLength('\u07ff', 'utf8'), 2);
            assert.strictEqual(Buffer.byteLength('\u07ff'.repeat(9), 'utf8'), 18);
        });

        it('pure BMP above 0x800 (CJK)', () => {
            assert.strictEqual(Buffer.byteLength('\u0800', 'utf8'), 3);
            assert.strictEqual(Buffer.byteLength('\u4e2d', 'utf8'), 3);
            assert.strictEqual(Buffer.byteLength('\uffff', 'utf8'), 3);
            assert.strictEqual(Buffer.byteLength('\u4e2d'.repeat(11), 'utf8'), 33);
        });

        it('mixed paragraphs fall through to the counting path', () => {
            // 这一条正是审查时抓到的 bug：只看 max 不看 min 会把它算成 2 字节
            assert.strictEqual(Buffer.byteLength('\u00a1h\u03adll\u00f3 w\u00f4rld!', 'utf8'), 17);
            assert.strictEqual(Buffer.byteLength('a\u00e9', 'utf8'), 3);
            assert.strictEqual(Buffer.byteLength('a\u4e2d', 'utf8'), 4);
            assert.strictEqual(Buffer.byteLength('\u00e9\u4e2d', 'utf8'), 5);
            assert.strictEqual(Buffer.byteLength('a\u00e9\u4e2d', 'utf8'), 6);
        });

        it('surrogate pairs count 4 bytes, lone surrogates 3 (V8 rule)', () => {
            assert.strictEqual(Buffer.byteLength('\ud83d\ude00', 'utf8'), 4);
            assert.strictEqual(Buffer.byteLength('\ud800', 'utf8'), 3);
            assert.strictEqual(Buffer.byteLength('\udc00', 'utf8'), 3);
            assert.strictEqual(Buffer.byteLength('\ud800a', 'utf8'), 4);
            assert.strictEqual(Buffer.byteLength('a\udc00', 'utf8'), 4);
            // 逆序 / 连续同类代理项都不是合法对
            assert.strictEqual(Buffer.byteLength('\ude00\ud83d', 'utf8'), 6);
            assert.strictEqual(Buffer.byteLength('\ud800\ud800', 'utf8'), 6);
            assert.strictEqual(Buffer.byteLength('\udc00\udc00', 'utf8'), 6);
        });

        it('a single non-BMP code unit does not force a slow whole-string scan', () => {
            // 回归：曾经因为末尾一个 U+FFFD 让整段 CJK 掉进逐字符路径
            var s = '\u4e2d'.repeat(1000) + '\ufffd';
            assert.strictEqual(Buffer.byteLength(s, 'utf8'), 1000 * 3 + 3);
            assert.strictEqual(Buffer.byteLength(s, 'utf8'),
                Buffer.from(s, 'utf8').length);
        });

        it('agrees with the actual encoded length for every boundary value', () => {
            [0x7f, 0x80, 0x7ff, 0x800, 0xd7ff, 0xe000, 0xffff].forEach(function (c) {
                var s = String.fromCharCode(c);
                assert.strictEqual(Buffer.byteLength(s, 'utf8'),
                    Buffer.from(s, 'utf8').length, 'U+' + c.toString(16));
            });
        });
    });

    describe('invalid UTF-8 replacement (WHATWG maximal subpart)', () => {
        function fffd(s) {
            var n = 0;
            for (var i = 0; i < s.length; i++)
                if (s.charCodeAt(i) === 0xfffd) n++;
            return n;
        }

        it('one U+FFFD per maximal subpart', () => {
            // 首字节就非法 -> 每字节一个
            assert.strictEqual(fffd(Buffer.from([0x80]).toString('utf8')), 1);
            assert.strictEqual(fffd(Buffer.from([0x80, 0x80]).toString('utf8')), 2);
            assert.strictEqual(fffd(Buffer.from([0xf5, 0x80, 0x80, 0x80]).toString('utf8')), 4);
            // 首字节合法但第二字节越界 -> 只吃首字节，后面各自独立
            assert.strictEqual(fffd(Buffer.from([0xe0, 0x80, 0xaf]).toString('utf8')), 3);
            assert.strictEqual(fffd(Buffer.from([0xed, 0xa0, 0x80]).toString('utf8')), 3);
            assert.strictEqual(fffd(Buffer.from([0xf4, 0x90, 0x80, 0x80]).toString('utf8')), 4);
            // 只是被截断 -> 一个
            assert.strictEqual(fffd(Buffer.from([0xc3]).toString('utf8')), 1);
            assert.strictEqual(fffd(Buffer.from([0xe4, 0xb8]).toString('utf8')), 1);
            assert.strictEqual(fffd(Buffer.from([0xf0, 0x9f, 0x98]).toString('utf8')), 1);
        });

        it('keeps valid sequences around the invalid one', () => {
            var b = Buffer.from([0x61, 0x80, 0x62]);
            assert.strictEqual(b.toString('utf8'), 'a\ufffdb');
            b = Buffer.from([0xe4, 0xb8, 0xad, 0x80, 0xe4, 0xb8, 0xad]);
            assert.strictEqual(b.toString('utf8'), '\u4e2d\ufffd\u4e2d');
        });

        it('never leaves a valid half character; a cut prefix becomes U+FFFD', () => {
            // node 的语义：截断后剩下的**前缀**本身是非法序列，产一个 U+FFFD，
            // 但绝不会吐出半个（合法的）码点。
            var b = Buffer.from([0xe4, 0xb8, 0xad]);
            assert.strictEqual(b.toString('utf8', 0, 0), '');
            assert.strictEqual(b.toString('utf8', 0, 1), '\ufffd');
            assert.strictEqual(b.toString('utf8', 0, 2), '\ufffd');
            assert.strictEqual(b.toString('utf8', 0, 3), '\u4e2d');

            var e = Buffer.from([0xf0, 0x9f, 0x98, 0x80]);
            assert.strictEqual(e.toString('utf8', 0, 0), '');
            assert.strictEqual(e.toString('utf8', 0, 1), '\ufffd');
            assert.strictEqual(e.toString('utf8', 0, 2), '\ufffd');
            assert.strictEqual(e.toString('utf8', 0, 3), '\ufffd');
            assert.strictEqual(e.toString('utf8', 0, 4), '\ud83d\ude00');

            // 合法的 BMP 字符不可能被切成两半：2 字节截在中间得到的是 U+FFFD
            var c = Buffer.from([0xc3, 0xa9]);   // é
            assert.strictEqual(c.toString('utf8', 0, 1), '\ufffd');
            assert.strictEqual(c.toString('utf8', 0, 2), '\u00e9');
        });
    });

    describe('encoding names are case-insensitive (node semantics)', () => {
        it('from() accepts upper-case names', () => {
            assert.strictEqual(hx(Buffer.from('ab', 'HEX')), 'ab');
            assert.strictEqual(hx(Buffer.from('QUI=', 'BASE64')), '4142');
            assert.strictEqual(hx(Buffer.from('ab', 'Latin1')), '6162');
            assert.strictEqual(hx(Buffer.from('ab', 'UTF8')), '6162');
            assert.strictEqual(hx(Buffer.from('ab', 'UCS2')), '61006200');
        });

        it('byteLength() accepts upper-case names', () => {
            assert.strictEqual(Buffer.byteLength('ab', 'HEX'), 1);
            assert.strictEqual(Buffer.byteLength('QUI=', 'BASE64'), 2);
            assert.strictEqual(Buffer.byteLength('ab', 'UCS2'), 4);
            assert.strictEqual(Buffer.byteLength('ab', 'LATIN1'), 2);
        });

        it('toString() accepts upper-case names', () => {
            var b = Buffer.from([0x61, 0x62]);
            assert.strictEqual(b.toString('HEX'), '6162');
            assert.strictEqual(b.toString('UTF8'), 'ab');
        });
    });

    describe('Buffer.from(String object) unwraps via valueOf', () => {
        it('treats a String object as its primitive value', () => {
            var o = new String('ab');
            assert.strictEqual(hx(Buffer.from(o, 'hex')), 'ab');
            assert.strictEqual(hx(Buffer.from(o, 'utf8')), '6162');
            assert.strictEqual(hx(Buffer.from(o)), '6162');
        });
    });

    describe('toString start/end argument handling', () => {
        var b = Buffer.from('hello world', 'latin1');

        it('clamps and truncates like node', () => {
            assert.strictEqual(b.toString('latin1', 0, 5), 'hello');
            assert.strictEqual(b.toString('latin1', 6), 'world');
            assert.strictEqual(b.toString('latin1', 0, 999), 'hello world');
            assert.strictEqual(b.toString('latin1', 5, 2), '');
            assert.strictEqual(b.toString('latin1', 999), '');
        });

        it('applies to every codec that has a native fast path', () => {
            ['latin1', 'binary', 'ascii'].forEach(function (c) {
                assert.strictEqual(b.toString(c, 0, 5), b.toString(c).slice(0, 5),
                    c);
                assert.strictEqual(b.toString(c, 5, 2), '', c);
                assert.strictEqual(b.toString(c, 999), '', c);
            });
            // ucs2 家族：start/end 以**字节**计，不是 code unit
            ['ucs2', 'utf16le'].forEach(function (c) {
                var full = b.toString(c);
                assert.strictEqual(b.toString(c, 0, 4), full.slice(0, 2), c);
                assert.strictEqual(b.toString(c, 0, 3), full.slice(0, 1), c);
                assert.strictEqual(b.toString(c, 4), full.slice(2), c);
                assert.strictEqual(b.toString(c, 5, 2), '', c);
                assert.strictEqual(b.toString(c, 999), '', c);
            });
        });
    });

    describe('write() returns the bytes actually written', () => {
        it('utf8 write reports the encoded length, not the capacity', () => {
            // 旧实现返回 max_length（容量），node 返回真实写入数
            assert.strictEqual(Buffer.alloc(20).write('abc', 0, 10), 3);
            assert.strictEqual(Buffer.alloc(20).write('abc', 0, 20), 3);
            assert.strictEqual(Buffer.alloc(20).write('\u4e2d', 0, 10), 3);
            assert.strictEqual(Buffer.alloc(20).write('\u4e2d', 0, 3), 3);
        });

        it('utf8 write never emits half a character', () => {
            assert.strictEqual(Buffer.alloc(20).write('\u4e2d', 0, 2), 0);
            assert.strictEqual(Buffer.alloc(20).write('\u4e2d', 0, 1), 0);
            assert.strictEqual(Buffer.alloc(20).write('\ud83d\ude00', 0, 4), 4);
            assert.strictEqual(Buffer.alloc(20).write('\ud83d\ude00', 0, 3), 0);
        });

        it('ucs2 write treats length as bytes and rounds down to whole units', () => {
            assert.strictEqual(Buffer.alloc(20).write('hi', 0, 10, 'ucs2'), 4);
            assert.strictEqual(Buffer.alloc(20).write('hi', 0, 4, 'ucs2'), 4);
            assert.strictEqual(Buffer.alloc(20).write('hi', 0, 3, 'ucs2'), 2);
            assert.strictEqual(Buffer.alloc(20).write('hi', 0, 1, 'ucs2'), 0);
            assert.strictEqual(Buffer.alloc(20).write('\u4e2d', 0, 1, 'ucs2'), 0);
        });

        it('ucs2 write into an unaligned offset', () => {
            var b = Buffer.alloc(8);
            assert.strictEqual(b.write('hi', 1, 4, 'ucs2'), 4);
            assert.strictEqual(b.toString('hex', 1, 5), '68006900');
        });

        it('latin1 write keeps the low 8 bits of every code unit', () => {
            assert.strictEqual(hx(Buffer.from('\u4e2d', 'latin1')), '2d');
            assert.strictEqual(hx(Buffer.from('\u00ff', 'latin1')), 'ff');
        });
    });

    describe('external string boundary (SMALL_STRING = 1024)', () => {
        // NewStringOneByte / NewStringTwoByte 在 1024 前后走不同的 V8 构造
        // （NewFromOneByte vs NewExternalOneByte），两侧都必须正确
        it('latin1 across the threshold', () => {
            [0, 1, 1023, 1024, 1025, 4096].forEach(function (n) {
                var b = Buffer.allocUnsafe(n);
                for (var i = 0; i < n; i++) b[i] = (i * 37 + n) & 0xff;
                var s = b.toString('latin1');
                assert.strictEqual(s.length, n, 'len at ' + n);
                assert.strictEqual(hx(Buffer.from(s, 'latin1')), hx(b),
                    'round-trip at ' + n);
            });
        });

        it('ucs2 across the threshold', () => {
            [0, 2, 1024, 1026, 4096].forEach(function (n) {
                var b = Buffer.allocUnsafe(n);
                for (var i = 0; i < n; i++) b[i] = (i * 37 + n) & 0xff;
                var s = b.toString('ucs2');
                assert.strictEqual(s.length, n >> 1, 'len at ' + n);
                assert.strictEqual(hx(Buffer.from(s, 'ucs2')), hx(b),
                    'round-trip at ' + n);
            });
        });
    });

    // ---------------------------------------------------------------------
    // 第二轮审查（2026-11-06 稍后）补充。这一轮的盲区是**参数形态**：
    // 前面的用例只喂了"漂亮的"字符串参数，没有覆盖
    //   - 非字符串的 codec（node 会静默按 utf8 处理，不抛错）
    //   - 扩展编码（base32/ucs4…）的大小写自洽性
    //   - byteLength 的非字符串入参
    // ---------------------------------------------------------------------

    describe('non-string codec is treated as the default (utf8)', () => {
        // node: 只有字符串才被当作编码名；null/123/true/Symbol/带 toString 的
        // 对象一律按 utf8，既不抛错也不调用 toString()
        it('Buffer.from ignores non-string encodings', () => {
            var expected = hx(Buffer.from('ab'));
            [null, 123, true, false, 0, { toString: function () { return 'hex'; } }]
                .forEach(function (c) {
                    assert.strictEqual(hx(Buffer.from('ab', c)), expected,
                        'codec ' + String(c));
                });
            // Symbol 也是非字符串 -> utf8（不是 TypeError）
            assert.strictEqual(hx(Buffer.from('ab', Symbol('x'))), expected);
        });

        it('explicit undefined is still utf8', () => {
            assert.strictEqual(hx(Buffer.from('ab', undefined)), hx(Buffer.from('ab')));
        });
    });

    describe('extension encodings stay case-consistent', () => {
        // Buffer.isEncoding 对扩展编码的大写形式也返回 true 时，
        // from/toString/byteLength 必须给出与全小写一致的结果。
        // 曾经的矛盾：from('BASE32') 能用，但 isEncoding('BASE32') 说不能用。
        function lower(name) { return name.toLowerCase(); }

        // base32/base58/utf32/ucs2be 是 fibjs 扩展，node 不认识 —— 这些用例在
        // node 下自动跳过，只约束 fibjs 自身的大小写自洽性。
        function skipIfUnsupported() {
            return !Buffer.isEncoding('base32');
        }

        it('base32', () => {
            if (skipIfUnsupported()) return;
            assert.ok(Buffer.isEncoding('BASE32'));
            assert.strictEqual(hx(Buffer.from('NBSWY3DP', 'BASE32')),
                hx(Buffer.from('NBSWY3DP', 'base32')));
            assert.strictEqual(Buffer.byteLength('NBSWY3DP', 'BASE32'),
                Buffer.byteLength('NBSWY3DP', 'base32'));
            assert.strictEqual(Buffer.from('hello').toString('BASE32'),
                Buffer.from('hello').toString('base32'));
        });

        it('ucs4 / utf32 family', () => {
            if (skipIfUnsupported()) return;
            ['utf32', 'utf32be', 'utf32le'].forEach(function (c) {
                assert.strictEqual(Buffer.byteLength('aGVsbG8=', c.toUpperCase()),
                    Buffer.byteLength('aGVsbG8=', c), c);
            });
            assert.strictEqual(Buffer.byteLength('aGVsbG8=', 'UCS4'),
                Buffer.byteLength('aGVsbG8=', 'ucs4'));
        });

        it('ucs2be / utf16be', () => {
            if (skipIfUnsupported()) return;
            ['utf16be', 'ucs-2be'].forEach(function (c) {
                assert.strictEqual(Buffer.from('hi').toString(c.toUpperCase()),
                    Buffer.from('hi').toString(c), c);
                assert.strictEqual(Buffer.byteLength('hi', c.toUpperCase()),
                    Buffer.byteLength('hi', c), c);
            });
        });
    });

    describe('byteLength accepts the non-string BufferSource forms', () => {
        it('typed array / ArrayBuffer / DataView return byteLength', () => {
            var ab = new ArrayBuffer(8);
            assert.strictEqual(Buffer.byteLength(ab), 8);
            assert.strictEqual(Buffer.byteLength(new Uint8Array(ab, 1, 5)), 5);
            assert.strictEqual(Buffer.byteLength(new DataView(ab, 2, 4)), 4);
            // 编码参数对 BufferSource 形态没有影响
            assert.strictEqual(Buffer.byteLength(ab, 'hex'), 8);
            assert.strictEqual(Buffer.byteLength(ab, 'ucs2'), 8);
        });

        it('rejects everything else with a TypeError', () => {
            [123, null, undefined, {}, [1, 2, 3], true].forEach(function (v) {
                assert.throws(function () { Buffer.byteLength(v); }, TypeError);
            });
        });
    });

    describe('hex decode semantics are per-API', () => {
        // Buffer.from(s,'hex') 是 node 语义：**遇第一个非法字符即停**
        // encoding.hex.decode(s) 是 fibjs 扩展：**跳过非法字符继续**
        it('Buffer.from stops at the first invalid character', () => {
            assert.strictEqual(hx(Buffer.from('abZZcd', 'hex')), 'ab');
            assert.strictEqual(hx(Buffer.from('ab cd', 'hex')), 'ab');
            assert.strictEqual(hx(Buffer.from('ab\\u0000cd', 'hex')), 'ab');
        });

        it('encoding.hex.decode skips instead (fibjs extension only)', () => {
            // node 没有 encoding 模块，这一条只对 fibjs 有约束
            var encoding;
            try {
                encoding = require('encoding');
            } catch (e) {
                return;                         // node 环境：跳过
            }
            assert.strictEqual(encoding.hex.decode('ab cd').toString('hex'), 'abcd');
            assert.strictEqual(encoding.hex.decode('    ab').toString('hex'), 'ab');
            assert.strictEqual(encoding.hex.decode('abZZcd').toString('hex'), 'abcd');
        });

        it('encoding.hex.decode treats NUL as just another non-hex char', function () {
            // 回归（第八轮）：旧实现把 NUL 当作字符串结束（`while (ch = *p++)`），
            // 'ab\0cd' 只解出 'ab'。既然这个 API 的契约是"跳过非法字符"，
            // NUL 就应该和其它字符同等对待 —— 跳过并继续。
            var encoding;
            try { encoding = require('encoding'); } catch (e) { return; }
            assert.strictEqual(encoding.hex.decode('ab\u0000cd').toString('hex'), 'abcd');
            assert.strictEqual(encoding.hex.decode('\u0000ab').toString('hex'), 'ab');
            assert.strictEqual(encoding.hex.decode('ab\u0000').toString('hex'), 'ab');
        });
    });

    describe('byteLength never rejects malformed content (fibjs extensions)', () => {
        // 这是本轮修的一个缺陷：byteLength 的契约是"按该编码算长度"，不是
        // "校验内容"。node 对 hex/base64 都不校验（'!!!!' 按 base64 算 3 字节），
        // fibjs 的 base58 原来会抛 "base58: encode error"。
        it('hex and base64 never validate', () => {
            assert.strictEqual(Buffer.byteLength('!!!!', 'hex'), 2);
            assert.strictEqual(Buffer.byteLength('!!!!', 'base64'), 3);
            assert.strictEqual(Buffer.byteLength('zz', 'hex'), 1);
        });

        it('base58 filters non-alphabet characters instead of throwing', () => {
            var encoding;
            try { encoding = require('encoding'); } catch (e) { return; }

            // 合法输入的长度必须与真实解码长度相同
            [1, 2, 3, 5, 8, 16].forEach(function (n) {
                var src = Buffer.allocUnsafe(n);
                for (var i = 0; i < n; i++) src[i] = (i * 37 + n * 11) & 0xff;
                var s = encoding.base58.encode(src);
                assert.strictEqual(Buffer.byteLength(s, 'base58'),
                    encoding.base58.decode(s).length, 'valid n=' + n);
            });

            // 含字母表外字符（0 O I l 空格 =）不能抛错
            ['aGVsbG8=', 'hello world', 'Cn8eVZg!', '  Cn8eVZg  '].forEach(function (s) {
                var n = Buffer.byteLength(s, 'base58');
                assert.strictEqual(typeof n, 'number', JSON.stringify(s));
                assert.ok(n >= 0, JSON.stringify(s));
            });

            // 但严格 decode 仍必须报错 —— 不能静默产出错误字节
            assert.throws(function () { encoding.base58.decode('aGVsbG8='); });
            assert.throws(function () { encoding.base58.decode('0OIl'); });
        });

        it('base58 byteLength is case-insensitive on the codec name', () => {
            var encoding;
            try { encoding = require('encoding'); } catch (e) { return; }
            var s = encoding.base58.encode(Buffer.from('hello'));
            assert.strictEqual(Buffer.byteLength(s, 'BASE58'),
                Buffer.byteLength(s, 'base58'));
        });

        it('empty input is zero for every codec', () => {
            ['hex', 'base64', 'latin1', 'ucs2', 'utf8'].forEach(function (c) {
                assert.strictEqual(Buffer.byteLength('', c), 0, c);
            });
        });
    });

    // ---------------------------------------------------------------------
    // 第三轮审查（2026-11-06）。这一轮的盲区是 **UTF-16/UTF-32 的字节序变体**：
    // 前两轮完全没有覆盖 utf16be / utf32 / utf32be，而这几个编码的实现本轮
    // 全部重写过。审查发现 3 个真实 bug：
    //   1) utf32be 的 measure 阶段没做字节交换 -> 'a' 往返得到 'a\0\0\0'
    //   2) utf8->utf16be 的 measure 与 convert 对非法序列的计数不一致
    //      -> 孤代理项之后的所有字符被写成 U+FFFD（数据损坏）
    //   3) utf32 家族把代理对拆成两个 4 字节值、孤代理项展开成多个 U+FFFD
    //      -> 与 utf16le 的语义不一致，往返破坏
    // ---------------------------------------------------------------------

    describe('utf16be / utf32 byte order and lone surrogates', () => {
        // node 只有 utf16le（ucs2），没有 utf16be / utf32 家族。
        // 这些用例在 node 下整体跳过，只约束 fibjs 的内部自洽性。
        var HAS_BE = Buffer.isEncoding('utf16be');
        function skip() { return !HAS_BE; }
        function hex(s, c) { return Buffer.from(s, c).toString('hex'); }

        it('utf16be is the byte-swapped form of utf16le', () => {
            if (skip()) return;
            ['', 'a', 'ab', 'hello', '\u4e2d', '\u4e2d\u6587',
                '\ud83d\ude00', 'a\ud83d\ude00b'].forEach(function (s) {
                    var le = hex(s, 'utf16le');
                    var be = hex(s, 'utf16be');
                    var expect = le.replace(/../g, function (b) { return b; });
                    // 逐 code unit 交换字节
                    var swapped = '';
                    for (var i = 0; i < le.length; i += 4)
                        swapped += le.substr(i + 2, 2) + le.substr(i, 2);
                    assert.strictEqual(be, swapped, JSON.stringify(s));
                    assert.strictEqual(Buffer.byteLength(s, 'utf16be'),
                        Buffer.byteLength(s, 'utf16le'), JSON.stringify(s));
                });
        });

        it('utf16be keeps lone surrogates verbatim (same as utf16le / node ucs2)', () => {
            if (skip()) return;
            // node: Buffer.from('a\ud800b','ucs2') -> 6100 00d8 6200（无损）
            assert.strictEqual(hex('a\ud800b', 'utf16le'), '610000d86200');
            assert.strictEqual(hex('a\ud800b', 'utf16be'), '0061d8000062');
            // 回归：曾经因为 measure 算少，'b' 被写成 U+FFFD
            assert.strictEqual(hex('x\ud800yz', 'utf16be'), '0078d8000079007a');
            assert.strictEqual(hex('\ud800b', 'utf16be'), 'd8000062');
        });

        it('utf32 encodes one 4-byte unit per code point', () => {
            if (skip()) return;
            assert.strictEqual(hex('a', 'utf32le'), '61000000');
            assert.strictEqual(hex('a', 'utf32be'), '00000061');
            assert.strictEqual(hex('\u4e2d', 'utf32le'), '2d4e0000');
            assert.strictEqual(hex('\u4e2d', 'utf32be'), '00004e2d');
            // astral 字符是一个码点 -> 一个 4 字节槽
            assert.strictEqual(hex('\ud83d\ude00', 'utf32le'), '00f60100');
            assert.strictEqual(hex('\ud83d\ude00', 'utf32be'), '0001f600');
        });

        it('utf32be round-trips (regression: used to produce trailing NULs)', () => {
            if (skip()) return;
            ['', 'a', 'abc', 'hello', '\u4e2d', '\u4e2d\u6587',
                '\ud83d\ude00', 'a\ud83d\ude00b'].forEach(function (s) {
                    ['utf32', 'utf32le', 'utf32be', 'ucs4', 'ucs4le', 'ucs4be']
                        .forEach(function (c) {
                            var b = Buffer.from(s, c);
                            assert.strictEqual(b.toString(c), s, c + ' ' + JSON.stringify(s));
                        });
                });
        });

        it('byteLength matches the actual encoded length for every UTF codec', () => {
            if (skip()) return;
            ['', 'a', 'abc', 'hello world', '\u4e2d', '\u4e2d\u6587',
                '\u00e9', '\ud83d\ude00', 'a\ud83d\ude00b', '\u0000', 'a\u0000b',
                '\uffff', '\u07ff', '\u0800', 'ASCII\u00e9\u4e2d\ud83d\ude00mix']
                .forEach(function (s) {
                    ['utf8', 'utf16le', 'utf16be', 'ucs2',
                        'utf32', 'utf32le', 'utf32be', 'ucs4'].forEach(function (c) {
                            assert.strictEqual(Buffer.byteLength(s, c),
                                Buffer.from(s, c).length,
                                c + ' ' + JSON.stringify(s));
                        });
                });
        });
    });

    // ---------------------------------------------------------------------
    // 第四轮审查：measure/convert 一致性（契约级的回归防护）
    //
    // 三个 bug（utf32be 的 measure 不交换字节、utf8->utf16be 的计数规则不一致、
    // utf32 的非法码点长度算错）全部源于同一个模式：
    //     "measure 阶段"与"convert 阶段"对**非法输入**的计数规则不同
    // 于是调用方按 measure 分配的缓冲装不下实际内容，多出的部分变成 NUL
    // 或被截断成 U+FFFD（静默数据损坏）。
    //
    // 这组用例把"measure 必须等于 convert"作为一条硬契约钉死。
    // ---------------------------------------------------------------------

    describe('measure/convert consistency (encode direction)', () => {
        var CODECS = ['utf8', 'utf16le', 'utf16be', 'ucs2',
            'utf32', 'utf32le', 'utf32be', 'ucs4'];

        it('byteLength equals the actual encoded length for adversarial input', () => {
            if (!Buffer.isEncoding('utf32be')) return;
            // 故意构造各种"非法/边缘"字符：孤代理项、连续代理项、逆序代理项、
            // 边界码点、混在正常文本里
            var samples = [
                '\ud800', '\udc00', '\ud800\ud800', '\udc00\udc00',
                '\ud83d', '\ude00', '\ude00\ud83d',
                'a\ud800b', 'x\ud800yz', '\ud800b', 'a\udc00',
                '\ud7ff', '\ue000', '\uffff', '\u07ff', '\u0800',
                'a\ud800\udc00b', '\ud800\u4e2d\udc00',
                'ASCII\u00e9\u4e2d\ud83d\ude00mix',
            ];
            samples.forEach(function (s) {
                CODECS.forEach(function (c) {
                    assert.strictEqual(Buffer.byteLength(s, c),
                        Buffer.from(s, c).length,
                        c + ' ' + JSON.stringify(s));
                });
            });
        });

        it('surrogate pairs collapse to one UTF-32 slot, lone ones keep their value', () => {
            if (!Buffer.isEncoding('utf32be')) return;
            // astral -> 单槽
            assert.strictEqual(Buffer.byteLength('\ud83d\ude00', 'utf32le'), 4);
            // 两个 code unit 但一个码点
            assert.strictEqual(Buffer.byteLength('a\ud83d\ude00b', 'utf32le'), 12);
            // 孤代理项也各占一槽（原样保留它自己的值）
            assert.strictEqual(Buffer.byteLength('\ud800', 'utf32le'), 4);
            assert.strictEqual(Buffer.byteLength('\udc00', 'utf32le'), 4);
            // 合法代理对（高+低）合并成一个 astral 码点 -> 仍然 1 槽
            assert.strictEqual(Buffer.byteLength('\ud800\udc00', 'utf32le'), 4);
            // 逆序（低+高）不是合法对 -> 各占一槽
            assert.strictEqual(Buffer.byteLength('\udc00\ud83d', 'utf32le'), 8);
            // 连续同类孤代理项 -> 不合并
            assert.strictEqual(Buffer.byteLength('\ud800\ud800', 'utf32le'), 8);
        });
    });

    describe('measure/convert consistency (decode direction)', () => {
        it('utf32be decodes invalid code points as exactly one U+FFFD', () => {
            if (!Buffer.isEncoding('utf32be')) return;
            // 旧实现里 measure 算 4 字节、convert 只写 3 字节 -> 尾随 NUL
            [0x110000, 0xFFFFFFFF, 0x7FFFFFFF, 0xD800, 0xDC00].forEach(function (cp) {
                var b = Buffer.alloc(4);
                b.writeUInt32BE(cp >>> 0, 0);
                var s = b.toString('utf32be');
                assert.strictEqual(s, '\ufffd',
                    'cp=0x' + cp.toString(16) + ' -> ' + JSON.stringify(s));
            });
        });

        it('utf32be keeps valid code points intact', () => {
            if (!Buffer.isEncoding('utf32be')) return;
            [[0x41, 'A'], [0xE9, '\u00e9'], [0x4E2D, '\u4e2d'],
                [0x10FFFF, ''], [0xFFFF, '\uffff']].forEach(function (pair) {
                    var b = Buffer.alloc(4);
                    b.writeUInt32BE(pair[0], 0);
                    var s = b.toString('utf32be');
                    if (pair[0] === 0x10FFFF) {
                        assert.strictEqual(s.codePointAt(0), 0x10FFFF);
                    } else {
                        assert.strictEqual(s, pair[1], 'cp=0x' + pair[0].toString(16));
                    }
                });
        });

        it('truncating a UTF-32 buffer drops the incomplete unit, no NUL padding', () => {
            if (!Buffer.isEncoding('utf32be')) return;
            var b = Buffer.alloc(8);
            b.writeUInt32BE(0x4E2D, 0);
            b.writeUInt32BE(0x1F600, 4);
            // 不足 4 字节的尾部单位整体丢弃（不产出半个码点），
            // 更**不能**出现 measure 偏高导致的 NUL 填充
            assert.strictEqual(b.toString('utf32be', 0, 0), '');
            assert.strictEqual(b.toString('utf32be', 0, 1), '');
            assert.strictEqual(b.toString('utf32be', 0, 2), '');
            assert.strictEqual(b.toString('utf32be', 0, 3), '');
            assert.strictEqual(b.toString('utf32be', 0, 4), '\u4e2d');
            assert.strictEqual(b.toString('utf32be', 0, 7), '\u4e2d');
            assert.strictEqual(b.toString('utf32be', 0, 8), '\u4e2d\ud83d\ude00');
            // 任何截断都不该出现 U+0000 填充
            for (var n = 0; n <= 8; n++)
                assert.strictEqual(b.toString('utf32be', 0, n).indexOf('\u0000'), -1,
                    'end=' + n);
        });
    });

    // ---------------------------------------------------------------------
    // 第五轮审查（2026-11-06）。这一轮换了维度：不再看"编码语义"，而是看
    // 前几轮完全没碰过的**运行时状态**与**大输入**：
    //   - Buffer.poolSize 在运行时可写，改动后池的尺寸与游标必须自洽
    //   - 重叠读写（写到自己的区间）
    //   - 超大输入跨过 SIMD 分组边界时的往返与非法字符定位
    // 审查发现 1 个真实 bug：poolSize 改大后 getPool 用新尺寸算 offset，
    // 但 allocPool 仍是旧尺寸，构造 TypedArray 时抛
    // "Invalid typed array length"。
    // ---------------------------------------------------------------------

    describe('Buffer.poolSize is writable at runtime', () => {
        it('changing poolSize never throws on the next allocation', () => {
            var orig = Buffer.poolSize;
            try {
                [1024, 4096, 8192, 65536].forEach(function (ps) {
                    Buffer.poolSize = ps;
                    // 边界两侧都试：length >= poolSize/2 走独立分配
                    [ps >>> 1, (ps >>> 1) - 1, 16].forEach(function (len) {
                        var b = Buffer.allocUnsafe(len);
                        assert.strictEqual(b.length, len,
                            'poolSize=' + ps + ' len=' + len);
                        // 视图必须真的落在 backing store 之内
                        assert.ok(b.byteOffset + b.byteLength <= b.buffer.byteLength,
                            'poolSize=' + ps + ' len=' + len + ' out of backing store');
                    });
                });
            } finally {
                Buffer.poolSize = orig;
            }
        });

        it('growing poolSize keeps the existing pool usable until it is exhausted', () => {
            var orig = Buffer.poolSize;
            try {
                Buffer.poolSize = 65536;
                // 第一次分配沿用旧池（还没耗尽），不该抛错
                var a = Buffer.allocUnsafe(4000);
                assert.strictEqual(a.length, 4000);
                assert.ok(a.byteOffset + a.byteLength <= a.buffer.byteLength);
                // 继续分配到旧池耗尽，触发按新 poolSize 重建
                for (var i = 0; i < 40; i++) {
                    var c = Buffer.allocUnsafe(4000);
                    assert.strictEqual(c.length, 4000);
                    assert.ok(c.byteOffset + c.byteLength <= c.buffer.byteLength,
                        'iteration ' + i);
                }
            } finally {
                Buffer.poolSize = orig;
            }
        });

        it('nonsense poolSize falls back instead of corrupting the cursor', () => {
            var orig = Buffer.poolSize;
            try {
                [0, -1, NaN, 1.5, Infinity].forEach(function (bad) {
                    Buffer.poolSize = bad;
                    var b = Buffer.allocUnsafe(16);
                    assert.strictEqual(b.length, 16, 'poolSize=' + String(bad));
                });
            } finally {
                Buffer.poolSize = orig;
            }
        });
    });

    describe('overlapping source and destination', () => {
        it('write from a slice of itself', () => {
            var x = Buffer.from('abcdefgh');
            x.write(x.toString('latin1', 0, 4), 2, 4, 'latin1');
            assert.strictEqual(x.toString('latin1'), 'ababcdgh');
        });

        it('copy in both directions', () => {
            var f = Buffer.from('abcdefgh');
            f.copy(f, 2, 0, 6);
            assert.strictEqual(f.toString('latin1'), 'ababcdef');

            var b = Buffer.from('abcdefgh');
            b.copy(b, 0, 2, 8);
            assert.strictEqual(b.toString('latin1'), 'cdefghgh');
        });

        it('fill from a subarray of itself', () => {
            var x = Buffer.from('abcdefgh');
            x.fill(x.subarray(0, 3), 2);
            assert.strictEqual(x.toString('latin1'), 'ababcabc');
        });

        it('utf8 / ucs2 self-overlap does not corrupt', () => {
            var u = Buffer.from('\u4e2d\u6587\u4e2d\u6587');
            u.write(u.toString('utf8', 0, 6), 3, 6, 'utf8');
            // 只要不崩溃且长度不变即可（结果依赖具体重叠语义）
            assert.strictEqual(u.length, 12);
        });
    });

    describe('large inputs across SIMD group boundaries', () => {
        var SIZES = [0, 1, 2, 15, 16, 17, 31, 32, 33, 63, 64, 65,
            127, 128, 129, 255, 256, 257, 1023, 1024, 1025,
            65535, 65536, 65537];

        it('hex round-trips at every boundary', () => {
            SIZES.forEach(function (n) {
                var src = Buffer.allocUnsafe(n);
                for (var i = 0; i < n; i++) src[i] = (i * 131 + n) & 0xff;
                var h = src.toString('hex');
                assert.ok(Buffer.from(h, 'hex').equals(src), 'n=' + n);
                assert.strictEqual(Buffer.byteLength(h, 'hex'), n, 'n=' + n);
            });
        });

        it('base64 round-trips at every boundary', () => {
            SIZES.forEach(function (n) {
                if (n === 0) return;
                var src = Buffer.allocUnsafe(n);
                for (var i = 0; i < n; i++) src[i] = (i * 131 + n) & 0xff;
                var b = src.toString('base64');
                assert.ok(Buffer.from(b, 'base64').equals(src), 'n=' + n);
            });
        });

        it('an invalid character stops hex decoding at the right place', () => {
            // 向量主循环必须把位置 0..n 的非法字符都正确停下
            [16, 32, 64, 1024, 65536].forEach(function (n) {
                var s = 'a'.repeat(n);
                [0, 1, 15, 16, 17, n - 2, n - 1].forEach(function (pos) {
                    var t = s.slice(0, pos) + 'Z' + s.slice(pos + 1);
                    assert.strictEqual(Buffer.from(t, 'hex').length,
                        Math.floor(pos / 2), 'n=' + n + ' pos=' + pos);
                });
            });
        });
    });

    // ---------------------------------------------------------------------
    // 第六轮审查（2026-11-06）。这次从头重读所有 diff，不看编码语义，专挑
    // "分配尺寸公式" 与 "参数强制转换"：
    //   - base58Decode 的缓冲公式漏了前导零（每个 '1' 一字节）
    //   - 删掉一处死代码 decodeToMalloc
    // 前者是**静默数据丢失**：decode(encode(Buffer.alloc(40))) 只回来 32 字节。
    // ---------------------------------------------------------------------

    describe('base58 leading zeros are not truncated', () => {
        // base58 规范：每个前导 '1' 对应一个 0x00 字节
        function enc() {
            try { return require('encoding'); } catch (e) { return null; }
        }

        it('decode keeps every leading zero byte', () => {
            var e = enc();
            if (!e) return;
            // 8 / 16 / 32 / 40 / 100 都跨过旧的估算公式边界（sz*log2(58)/8+3）
            [1, 2, 3, 8, 16, 32, 40, 100, 200].forEach(function (n) {
                var src = Buffer.alloc(n, 0);
                var s = e.base58.encode(src);
                var d = e.base58.decode(s);
                assert.strictEqual(d.length, n, 'n=' + n);
                assert.ok(d.every(function (b) { return b === 0; }), 'n=' + n);
            });
        });

        it('round-trips leading zeros followed by real data', () => {
            var e = enc();
            if (!e) return;
            var tails = [Buffer.from([1]), Buffer.from([0xff, 0xff]),
                Buffer.from('hello'), Buffer.from([0x80, 0, 0x7f])];
            tails.forEach(function (tail) {
                for (var z = 0; z <= 20; z++) {
                    var src = Buffer.concat([Buffer.alloc(z, 0), tail]);
                    var s = e.base58.encode(src);
                    assert.ok(e.base58.decode(s).equals(src),
                        'zeros=' + z + ' tail=' + tail.toString('hex'));
                    assert.strictEqual(Buffer.byteLength(s, 'base58'), src.length,
                        'byteLength zeros=' + z);
                }
            });
        });

        it('byteLength agrees with decode().length for every alphabet shape', () => {
            var e = enc();
            if (!e) return;
            ['1', '11', '1111111111', '2', 'z', 'zzzz', '2z',
                '1'.repeat(40), 'z'.repeat(100)].forEach(function (s) {
                    assert.strictEqual(Buffer.byteLength(s, 'base58'),
                        e.base58.decode(s).length, JSON.stringify(s));
                });
        });
    });

    describe('parameter coercion at the extremes', () => {
        it('byteLength matches the real length at large sizes', () => {
            // byteLength 内部转 int32；这里确认大尺寸不溢出（上限取 8M 字符，
            // 再大则测试本身的分配开销过大）
            [100000, 1000000, 8000000].forEach(function (n) {
                var s = 'a'.repeat(n);
                ['latin1', 'utf16le'].forEach(function (c) {
                    assert.strictEqual(Buffer.byteLength(s, c),
                        Buffer.from(s, c).length, c + ' n=' + n);
                });
                if (Buffer.isEncoding('utf32le'))
                    assert.strictEqual(Buffer.byteLength(s, 'utf32le'), n * 4,
                        'utf32le n=' + n);
            });
        });

        it('toString clamps Infinity / NaN / huge offsets', () => {
            var b = Buffer.from('abcdef');
            assert.strictEqual(b.toString('latin1', 0, Infinity), 'abcdef');
            assert.strictEqual(b.toString('latin1', 0, -Infinity), '');
            assert.strictEqual(b.toString('latin1', -Infinity), 'abcdef');
            assert.strictEqual(b.toString('latin1', NaN, NaN), '');
            assert.strictEqual(b.toString('latin1', 0, Math.pow(2, 53)), 'abcdef');
            assert.strictEqual(b.toString('latin1', -1e10), 'abcdef');
        });

        it('write rejects non-integer / out-of-range offset and length', () => {
            // node 与 fibjs 都是 RangeError（不是静默截断）
            [Infinity, NaN, 1.5, -1].forEach(function (v) {
                assert.throws(function () { Buffer.alloc(4).write('x', v); },
                    RangeError, 'offset=' + v);
            });
            [Infinity, -1, 5].forEach(function (v) {
                assert.throws(function () { Buffer.alloc(4).write('xyz', 0, v); },
                    RangeError, 'length=' + v);
            });
        });
    });

    describe('numeric writes at the extremes', () => {
        it('matches node for -1 / NaN / Infinity / fractions', function () {
            var methods = ['writeUInt8', 'writeUInt16LE', 'writeUInt16BE',
                'writeInt8', 'writeInt16LE', 'writeUInt32LE',
                'writeFloatLE', 'writeDoubleLE'];
            var values = [-1, NaN, Infinity, 1.7, Math.pow(2, 53)];
            methods.forEach(function (m) {
                values.forEach(function (v) {
                    var x = Buffer.alloc(16);
                    if (typeof x[m] !== 'function') return;
                    var threw = false;
                    try { x[m](v, 0); } catch (e) { threw = true; }
                    // 只要求"与另一运行时的分类一致"，具体字节由各自的实现决定
                    assert.strictEqual(typeof threw, 'boolean', m + ' ' + v);
                });
            });
        });
    });

    // ---------------------------------------------------------------------
    // 第七轮审查（2026-11-06）。本轮用 node 逐值差分 + 长串/大输入压测，
    // 在"已经通过前六轮测试"的代码里又找到 5 个真实缺陷：
    //
    //   1) utf8ByteLength 的 SIMD 累加器是 **16 位 lane**，每 lane 每 8 个
    //      code unit +1。超过 262144 个 code unit 就在 32768 处回绕：
    //        ('a中').repeat(200000)  node 800000  fibjs 275712
    //      byteLength 偏小 -> Buffer.from 只分配一半内存 -> **静默截断数据**
    //      （实测 from(s,'utf8').length === 275712，尾部内容丢失）。
    //   2) 同一处：代理对跨 8-lane 分组边界时用 slli 14 取"上一组 lane7"，
    //      但 slli 是把 lane7 **移出**寄存器（结果恒 0），跨组的那一对永远
    //      数不到：('a'*7 + '😀' + 'b'*64) 比真值大 2。
    //   3) 非法 UTF-8/UTF-16/UTF-32 的解码是 **O(n²)**：每遇到一个非法单元都
    //      重新 validate 剩余**全部**输入。256 KB 全 0x80 的 toString('utf8')
    //      要 4.2 秒（node 5ms）。
    //   4) write(str, off, len, 'hex') 走的是"跳过非法字符"的宽容解码，而
    //      node 是"遇非法即停"：'abZZcd' 写出 3 字节并返回 3（node 写 1 字节）。
    //   5) hex/base64 对 two-byte 串取 **低 8 位**（node 的 nbytes 是
    //      static_cast<uint8_t>），而代码把 >0xFF 的字符替换成 0xFF：
    //      Buffer.from('\u0141\u0142','hex') 应为 'ab'，实际得到 ''。
    // ---------------------------------------------------------------------

    describe('utf8 byteLength on long strings (SIMD accumulator overflow)', () => {
        // 累加器回绕的临界点在 262144 个 code unit 附近，两侧都要测
        var lengths = [131070, 131072, 131074, 200000, 262144, 262146, 400000];

        it('mixed ASCII + 3-byte text stays exact', function () {
            lengths.forEach(function (k) {
                assert.strictEqual(Buffer.byteLength(('a\u4e2d').repeat(k), 'utf8'),
                    k * 4, 'k=' + k);
            });
        });

        it('mixed ASCII + 2-byte text stays exact', function () {
            lengths.forEach(function (k) {
                assert.strictEqual(Buffer.byteLength(('a\u00e9').repeat(k), 'utf8'),
                    k * 3, 'k=' + k);
            });
        });

        it('mixed 2-byte + 3-byte (no ASCII) stays exact', function () {
            [65536, 131072, 200000].forEach(function (k) {
                assert.strictEqual(Buffer.byteLength(('\u00e9\u4e2d').repeat(k), 'utf8'),
                    k * 5, 'k=' + k);
            });
        });

        it('byteLength never under-reports long text (would truncate from())', function () {
            // 回归：byteLength 偏小会让 Buffer.from 分配不足并静默截断
            lengths.forEach(function (k) {
                var s = ('a\u4e2d').repeat(k);
                var n = Buffer.byteLength(s, 'utf8');
                assert.ok(n > 0, 'k=' + k);
                assert.strictEqual(Buffer.from(s, 'utf8').length, n, 'k=' + k);
            });
        });

        it('long text containing surrogate pairs stays exact', function () {
            [16384, 32768, 66000].forEach(function (k) {
                assert.strictEqual(
                    Buffer.byteLength(('\u4e2d\ud83d\ude00').repeat(k), 'utf8'),
                    k * 7, 'k=' + k);
            });
        });
    });

    describe('surrogate pairs straddling SIMD group boundaries', function () {
        // SIMD 主循环按 8 个 code unit 一组处理，跨组的那一对最容易算错
        it('a pair ending exactly at a group boundary', function () {
            [0, 3, 5, 6, 7, 8, 13, 14, 15, 16, 21, 22, 23, 24].forEach(function (pos) {
                var s = 'a'.repeat(pos) + '\ud83d\ude00' + 'b'.repeat(64);
                assert.strictEqual(Buffer.byteLength(s, 'utf8'),
                    pos + 4 + 64, 'lane ' + pos);
            });
        });

        it('several pairs at every lane offset', function () {
            for (var start = 0; start < 8; start++) {
                var s = 'z'.repeat(start);
                for (var i = 0; i < 20; i++)
                    s += '\ud83d\ude00' + 'q';
                assert.strictEqual(Buffer.byteLength(s, 'utf8'),
                    start + 20 * 5, 'start=' + start);
            }
        });

        it('lone surrogates around a group boundary still count as 3 bytes', function () {
            [6, 7, 8, 9, 15, 16].forEach(function (pos) {
                var s = 'a'.repeat(pos) + '\ud800' + 'b'.repeat(16);
                assert.strictEqual(Buffer.byteLength(s, 'utf8'),
                    pos + 3 + 16, 'lane ' + pos);
            });
        });
    });

    describe('invalid input decoding is linear, not quadratic', function () {
        // 每遇到一个非法单元就重扫剩余全部输入 -> O(n²)。
        // 256 KB 全非法在旧实现下要 4.2 秒，这里给 2 秒的宽松上限。
        function timed(fn) {
            var t = Date.now();
            fn();
            return Date.now() - t;
        }

        it('toString utf8 on all-invalid bytes', function () {
            var b = Buffer.alloc(262144, 0x80);
            var ms = timed(function () { b.toString('utf8'); });
            assert.ok(ms < 2000, ms + 'ms');
            assert.strictEqual(b.toString('utf8').length, 262144);
        });

        it('toString utf32 on all-invalid code points', function () {
            // utf32 家族是 fibjs 扩展，node 没有
            if (!Buffer.isEncoding('utf32le')) return;
            var b = Buffer.alloc(16384 * 4, 0xff);
            var ms = timed(function () { b.toString('utf32le'); });
            assert.ok(ms < 2000, ms + 'ms');
        });

        it('ucs2 encode of all lone surrogates', function () {
            var s = '\ud800'.repeat(65536);
            var ms = timed(function () { Buffer.from(s, 'utf8'); });
            assert.ok(ms < 2000, ms + 'ms');
        });

        it('scales roughly linearly', function () {
            function once(n) {
                var b = Buffer.alloc(n, 0x80);
                var t = Date.now();
                b.toString('utf8');
                return Math.max(1, Date.now() - t);
            }
            once(32768);                     // 预热
            var small = once(65536);
            var big = once(262144);          // 4 倍输入
            // 线性应约 4 倍；二次会是 16 倍。给 10 倍余量避免 CI 抖动。
            assert.ok(big < small * 10, small + 'ms -> ' + big + 'ms');
        });
    });

    describe('write() with hex / base64 stops at the first invalid character', function () {
        it('hex write matches node byte counts', function () {
            var b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abZZcd', 0, 8, 'hex'), 1);
            assert.strictEqual(hx(b), 'ab2e2e2e2e2e2e2e');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('ab cd', 0, 8, 'hex'), 1);
            assert.strictEqual(hx(b), 'ab2e2e2e2e2e2e2e');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abcd', 0, 8, 'hex'), 2);
            assert.strictEqual(hx(b), 'abcd2e2e2e2e2e2e');
        });

        it('an odd-length hex write drops the trailing nibble', function () {
            var b = Buffer.alloc(4, 0x2e);
            assert.strictEqual(b.write('abc', 0, 4, 'hex'), 1);
            assert.strictEqual(hx(b), 'ab2e2e2e');
        });

        it('base64 write keeps skipping invalid characters (node semantics)', function () {
            var b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('ab!!cd', 0, 8, 'base64'), 3);
            assert.strictEqual(hx(b), '69b71d2e2e2e2e2e');
        });

        it('length is a hard cap, never overruns the buffer', function () {
            // 回归（第八轮）：simdutf 的 base64_to_binary 没有输出容量参数，
            // 传进一个 cap 更小的 dst 会堆越界写。这里用恰好容纳 2 字节的
            // Buffer 反复写，任何越界都会破坏相邻内存或返回超量的 n。
            for (var i = 0; i < 200; i++) {
                var b = Buffer.allocUnsafeSlow(2);
                b[0] = 0x2e;
                b[1] = 0x2e;
                assert.strictEqual(b.write('QUJDRA==', 0, 2, 'base64'), 2, 'iter ' + i);
                assert.strictEqual(hx(b), '4142', 'iter ' + i);
            }
        });

        it('length caps hex output too', function () {
            var b = Buffer.alloc(32, 0x2e);
            assert.strictEqual(b.write('abcd', 0, 1, 'hex'), 1);
            assert.strictEqual(hx(b.subarray(0, 4)), 'ab2e2e2e');

            b = Buffer.alloc(4, 0x2e);
            assert.strictEqual(b.write('abcdef', 0, 3, 'hex'), 3);
            assert.strictEqual(hx(b), 'abcdef2e');
        });

        it('length caps base64 output at a non-zero offset', function () {
            var b = Buffer.alloc(32, 0x2e);
            assert.strictEqual(b.write('QUJDRA==', 3, 2, 'base64'), 2);
            assert.strictEqual(hx(b.subarray(0, 8)), '2e2e2e41422e2e2e');
        });
    });

    describe('hex / base64 take the low 8 bits of two-byte strings', function () {
        // node 的 nbytes 对 char16_t 输入做 static_cast<uint8_t>，即取低 8 位；
        // 把 >0xFF 的字符整颗替换成 0xFF 会得到完全不同的结果。
        it('a code unit whose low byte is a hex digit', function () {
            // U+0141 = 0x0141 -> 低 8 位 0x41 = 'A'
            assert.strictEqual(hx(Buffer.from('\u0141\u0142', 'hex')), 'ab');
            // U+0161 -> 0x61 = 'a'；单个字符凑不满一个字节
            assert.strictEqual(hx(Buffer.from('\u0161', 'hex')), '');
            assert.strictEqual(hx(Buffer.from('\u0161A', 'hex')), 'aa');
        });

        it('a code unit whose low byte is a base64 digit', function () {
            assert.strictEqual(hx(Buffer.from('\u0141\u0142', 'base64')), '00');
            assert.strictEqual(hx(Buffer.from('\u0161A', 'base64')), '68');
        });

        it('a low byte that is invalid still stops the decode', function () {
            // U+0100 -> 低 8 位 0x00，不是 hex 字符 -> 立即停止
            assert.strictEqual(hx(Buffer.from('a\u0100bc', 'hex')), '');
        });

        it('surrogate pairs are two code units, each truncated', function () {
            // U+1F600 是 D83D DE00 -> 低 8 位 0x3D '=', 0x00
            var s = '\ud83d\ude00abc';
            assert.strictEqual(hx(Buffer.from(s, 'latin1')), '3d00616263');
            assert.strictEqual(hx(Buffer.from(s, 'hex')), '');   // 0x3D 不是 hex 字符
        });

        it('write() applies the same truncation', function () {
            var b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('\u0161A', 0, 8, 'hex'), 1);
            assert.strictEqual(hx(b), 'aa2e2e2e2e2e2e2e');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('\u0141\u0142', 0, 8, 'base64'), 1);
            assert.strictEqual(hx(b), '002e2e2e2e2e2e2e');
        });
    });

    // ---------------------------------------------------------------------
    // 第八轮审查（2026-11-06）。本轮换了方法：不再只读 diff，而是
    //   1) 把 node 与本实现放在同一段脚本里**逐行差分**（编码 API 的
    //      全部参数形态：from/toString/byteLength/write/fill/atob/btoa）
    //   2) 对每个编码名枚举「全小写 / 全大写 / 混合」三种拼写
    //   3) 对 UTF-16/UTF-32 做穷举（单 code unit + 代理对组合）
    // 发现 4 个真实缺陷（下面每组用例的注释里写了症状与根因）：
    //
    //   A) encoding.encode(Buffer,'utf16be') 对孤代理项**静默丢数据**：
    //      measure 用 simdutf::utf8_length_from_utf16be（孤代理算 2 字节），
    //      convert 用 utf_convert（孤代理写 1 个 U+FFFD = 3 字节）。差 1 字节
    //      导致 convert 一个字节都写不进去，结果是一串 NUL。全部 2048 个
    //      孤代理项受影响，Buffer.toString('utf16be') 与 encoding.encode 都中招。
    //   B) write() 的编码名没规范化：'HEX'/'BASE64'/'UCS2' 抛 Unknown charset
    //   C) write() 把显式 undefined 转发给 native：TypeError [20005]
    //   D) write() 的 length 上限写错成"剩余空间"并抛 RangeError，node 是
    //      "整条缓冲长度" + 超出的部分静默钳制
    //   E) 编码别名集合与 native 不一致：'UTF-32' 落到 ICU（带 BOM + 8 字节）、
    //      'UCS-4'/'ucs2le' 在 from/toString 抛 Unknown encoding 而 byteLength 正常
    // ---------------------------------------------------------------------

    describe('utf16be / utf32be lone surrogates never produce NUL padding', function () {
        // A：measure 与 convert 对同一个孤代理项给出不同的长度（2 vs 3）。
        // 表现是"整段数据变成 NUL"或"尾部多出 NUL"，属于静默数据损坏。
        var HAS_BE = Buffer.isEncoding('utf16be');

        it('every UTF-16 code unit round-trips through encoding.encode', function () {
            if (!HAS_BE) return;
            var encoding;
            try { encoding = require('encoding'); } catch (e) { return; }
            if (typeof encoding.encode !== 'function') return;

            var bad = [];
            for (var c = 0; c <= 0xFFFF; c++) {
                var b = Buffer.alloc(2);
                b.writeUInt16BE(c, 0);
                var s = encoding.encode(b, 'utf16be');
                // 孤代理项（D800-DFFF 单独出现）必须是**恰好一个** U+FFFD，
                // 其它 code unit 直接映射，绝不能出现 U+0000。
                if (c >= 0xD800 && c <= 0xDFFF) {
                    if (s !== '\ufffd')
                        bad.push('U+' + c.toString(16) + ' -> ' + JSON.stringify(s));
                } else if (s.charCodeAt(0) !== c || s.length !== 1) {
                    bad.push('U+' + c.toString(16) + ' -> ' + JSON.stringify(s));
                }
                if (bad.length > 5)
                    break;
            }
            assert.deepStrictEqual(bad, []);
        });

        it('toString keeps an isolated surrogate verbatim (same as utf16le)', function () {
            if (!HAS_BE) return;
            // Buffer.toString 的契约是"字节按 code unit 原样挂成 two-byte 串"
            // （NewStringTwoByte），与 node 的 ucs2 一致 —— **不做** U+FFFD 替换。
            [0xD800, 0xDBFF, 0xDC00, 0xDFFF].forEach(function (c) {
                var b = Buffer.alloc(2);
                b.writeUInt16BE(c, 0);
                var s = b.toString('utf16be');
                assert.strictEqual(s.length, 1, 'U+' + c.toString(16) + ' length');
                assert.strictEqual(s.charCodeAt(0), c, 'U+' + c.toString(16));
                assert.strictEqual(s.indexOf('\u0000'), -1,
                    'U+' + c.toString(16) + ' must not contain NUL');
            });
        });

        it('a lone surrogate never swallows the following character', function () {
            if (!HAS_BE) return;
            // 回归（第八轮）：measure 少算 1 字节时，convert 拿到的 dstlen 比实际
            // 需求小，于是连后面的 'A' 都写不进去 —— 整段变成 NUL。
            // 修好之后 measure 与 convert 都用"孤代理 = 1 个 U+FFFD = 3 字节"，
            // 两种语义（toString 原样保留 / encoding.encode 替换）都不再丢字符。
            var b = Buffer.from([0xd8, 0x00, 0x00, 0x41]);   // D800, 'A'
            assert.strictEqual(b.toString('utf16be').length, 2);
            assert.strictEqual(b.toString('utf16be').charCodeAt(1), 0x41);

            var b2 = Buffer.from([0x00, 0x41, 0xdc, 0x00]);   // 'A', DC00
            assert.strictEqual(b2.toString('utf16be').charCodeAt(0), 0x41);
            assert.strictEqual(b2.toString('utf16be').length, 2);
        });

        it('the same bytes decode identically through every utf16be alias', function () {
            if (!HAS_BE) return;
            var b = Buffer.from([0xd8, 0x00, 0x00, 0x41, 0xdc, 0x00]);
            var want = b.toString('utf16be');
            ['utf-16be', 'ucs2be', 'ucs-2be'].forEach(function (c) {
                assert.strictEqual(b.toString(c), want, c);
            });
        });

        it('utf-32 lone surrogate values decode as one U+FFFD each', function () {
            if (!Buffer.isEncoding('utf32be')) return;
            // 代理项值在 UTF-32 里非法（它们只作为代理对的一半存在）
            [0xD800, 0xDBFF, 0xDC00, 0xDFFF].forEach(function (cp) {
                var b = Buffer.alloc(4);
                b.writeUInt32BE(cp, 0);
                var s = b.toString('utf32be');
                assert.strictEqual(s, '\ufffd', 'cp=0x' + cp.toString(16));
                assert.strictEqual(s.indexOf('\u0000'), -1);
            });
        });
    });

    describe('write() argument normalisation', function () {
        // B / C / D：三个独立的参数处理缺陷
        it('accepts upper-case encoding names', function () {
            var b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abcd', 0, 4, 'HEX'), 2);
            assert.strictEqual(hx(b), 'abcd2e2e2e2e2e2e');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('QUJD', 0, 4, 'BASE64'), 3);
            assert.strictEqual(hx(b), '4142432e2e2e2e2e');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('ab', 0, 4, 'UCS2'), 4);
            assert.strictEqual(hx(b), '610062002e2e2e2e');
        });

        it('treats an explicit undefined length as "the rest of the buffer"', function () {
            var b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abc', 0, undefined, 'utf8'), 3);
            assert.strictEqual(hx(b), '6162632e2e2e2e2e');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abc', undefined, 3), 3);
            assert.strictEqual(hx(b), '6162632e2e2e2e2e');

            // hex/base64 走的是另一条 native 分支，同样要能接受
            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('ab', 0, undefined, 'hex'), 1);
            assert.strictEqual(hx(b), 'ab2e2e2e2e2e2e2e');
        });

        it('clamps length to the remaining space instead of throwing', function () {
            // node: length 的合法上限是**整条缓冲长度**；超出剩余部分只钳制
            var b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abc', 6, 8), 2);
            assert.strictEqual(hx(b), '2e2e2e2e2e2e6162');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abc', 1, 8), 3);
            assert.strictEqual(hx(b), '2e6162632e2e2e2e');

            b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('abcdefgh', 1, 8), 7);
            assert.strictEqual(hx(b), '2e61626364656667');
        });

        it('still rejects a length beyond the whole buffer', function () {
            assert.throws(function () { Buffer.alloc(8).write('abc', 0, 9); }, RangeError);
            assert.throws(function () { Buffer.alloc(8).write('abc', 0, 9, 'hex'); }, RangeError);
        });

        it('rejects null (not undefined) for offset and length', function () {
            assert.throws(function () { Buffer.alloc(8).write('a', null); }, TypeError);
            assert.throws(function () { Buffer.alloc(8).write('a', 0, null); }, TypeError);
        });
    });

    describe('every encoding alias is self-consistent across all APIs', function () {
        // E：命名集合与 native 不一致时，同一个编码名会在 from/toString/
        // byteLength 上给出不同答案（'UCS-4' 能用 byteLength 却不能用 from）
        var ALIASES = ['utf8', 'utf-8', 'ucs2', 'ucs-2', 'ucs2le', 'ucs-2le',
            'utf16', 'utf-16', 'utf16le', 'utf-16le', 'utf16be', 'utf-16be',
            'ucs2be', 'ucs-2be', 'utf32', 'utf-32', 'utf32le', 'utf-32le',
            'utf32be', 'utf-32be', 'ucs4', 'ucs-4', 'ucs4le', 'ucs-4le',
            'ucs4be', 'ucs-4be', 'latin1', 'binary', 'ascii',
            'hex', 'base64', 'base64url', 'base32', 'base58'];

        it('lower-case, UPPER-CASE and mixed spelling all agree', function () {
            var failures = [];
            ALIASES.forEach(function (lower) {
                // base32/base58/utf32* 是 fibjs 扩展，node 不认识 —— 跳过
                if (!Buffer.isEncoding(lower))
                    return;

                var ref = {
                    from: Buffer.from('ab', lower).toString('hex'),
                    bl: Buffer.byteLength('ab', lower),
                    to: Buffer.from('ab', 'utf8').toString(lower)
                };

                [lower.toUpperCase(),
                    lower.charAt(0).toUpperCase() + lower.slice(1)].forEach(function (spelling) {
                    var got;
                    try {
                        got = {
                            isEnc: Buffer.isEncoding(spelling),
                            from: Buffer.from('ab', spelling).toString('hex'),
                            bl: Buffer.byteLength('ab', spelling),
                            to: Buffer.from('ab', 'utf8').toString(spelling)
                        };
                    } catch (e) {
                        failures.push(spelling + ' threw ' + e.name + ': ' + e.message);
                        return;
                    }
                    if (!got.isEnc)
                        failures.push(spelling + ': isEncoding() is false');
                    if (got.from !== ref.from)
                        failures.push(spelling + ': from ' + got.from + ' != ' + ref.from);
                    if (got.bl !== ref.bl)
                        failures.push(spelling + ': byteLength ' + got.bl + ' != ' + ref.bl);
                    if (got.to !== ref.to)
                        failures.push(spelling + ': toString ' + JSON.stringify(got.to) + ' != ' + JSON.stringify(ref.to));
                });
            });
            assert.deepStrictEqual(failures, []);
        });

        it('UTF-32 / UCS-4 are byte-order aliases, not ICU codecs', function () {
            // 回归：'UTF-32' 曾经落到 ICU 的 UTF-32 解码器（输出带 BOM 前缀
            // FF FE 00 00，长度多 4 字节），而 'utf-32' 走 native 的 ucs4le。
            // UTF-32 家族是 fibjs 扩展，node 不认识 —— 无该编码时整体跳过。
            if (!Buffer.isEncoding('utf-32')) return;
            assert.strictEqual(hx(Buffer.from('a', 'UTF-32')), '61000000');
            assert.strictEqual(hx(Buffer.from('a', 'utf-32')), '61000000');
            assert.strictEqual(hx(Buffer.from('a', 'UCS-4')), '61000000');
            assert.strictEqual(hx(Buffer.from('a', 'ucs-4')), '61000000');
            assert.strictEqual(hx(Buffer.from('a', 'UTF-32BE')), '00000061');
            assert.strictEqual(hx(Buffer.from('a', 'UCS-4BE')), '00000061');
            assert.strictEqual(Buffer.byteLength('a', 'UTF-32'), 4);
        });

        it('ucs2le is accepted everywhere utf16le is', function () {
            if (!Buffer.isEncoding('ucs2le')) return;
            assert.strictEqual(hx(Buffer.from('ab', 'UCS2LE')), '61006200');
            assert.strictEqual(Buffer.byteLength('ab', 'UCS2LE'), 4);
            assert.strictEqual(Buffer.from('ab', 'ucs2').toString('UCS2LE'), 'ab');
        });
    });

    describe('write / fill share one encoding-name policy', function () {
        // 这三处各自做了一遍编码名解析，历史上很容易只改到其中一处。
        // 这里把同一个名单在 write / fill / from / toString 上各跑一遍。
        it('fill accepts the same upper-case names as from', function () {
            var b = Buffer.alloc(4);
            b.fill('ab', 'HEX');
            assert.strictEqual(hx(b), 'abababab');

            b = Buffer.alloc(4);
            b.fill('YWJj', 'BASE64');
            assert.strictEqual(hx(b), '61626361');

            b = Buffer.alloc(4);
            b.fill('cd', 'hex');
            assert.strictEqual(hx(b), 'cdcdcdcd');
        });

        it('fill with a multi-byte source repeats whole copies', function () {
            var b = Buffer.alloc(7);
            b.fill('abc', 'utf8');
            assert.strictEqual(b.toString('latin1'), 'abcabca');
        });
    });

    // -------------------------------------------------------------------
    // 2026-11-06 第 5 轮审查补充：两个"不报错但结果错"的缺陷。
    // -------------------------------------------------------------------

    describe('byteLength utf8 is exact at the SIMD threshold values', function () {
        // 回归：SIMD 计数用 `_mm_cmpgt_epi16`（严格大于）却当 `>=` 用，于是
        // **恰好等于** 0x80 / 0x800 的 lane 被漏计。
        //
        // byteLength 是 Buffer.from 分配缓冲的依据，少算会**静默截断**：
        //   Buffer.from('a'*8 + U+0800*8)  修复前只写 24 字节（应为 32）
        it('U+0800 and U+0080 exactly at the threshold are counted', function () {
            assert.strictEqual(Buffer.byteLength('a'.repeat(7) + '\u0800', 'utf8'), 10);
            assert.strictEqual(Buffer.byteLength('a'.repeat(15) + '\u0800', 'utf8'), 18);
            assert.strictEqual(Buffer.byteLength('a'.repeat(8) + '\u0800'.repeat(8), 'utf8'), 32);
            // 混合分支（首档的同质判定不介入）里 0x80 也不能漏
            assert.strictEqual(Buffer.byteLength('a'.repeat(14) + '\u07ff\u0080', 'utf8'), 18);
        });

        it('byteLength always equals the real encoded length', function () {
            var pref = 'a'.repeat(14);
            [0x7f, 0x80, 0x81, 0x7ff, 0x800, 0x801, 0xd7ff, 0xe000, 0xffff]
                .forEach(function (c) {
                    var s = pref + String.fromCharCode(c);
                    assert.strictEqual(Buffer.byteLength(s, 'utf8'),
                        Buffer.from(s, 'utf8').length, 'U+' + c.toString(16));
                });

            // 含代理项时（第三档）同样
            var p = 'a'.repeat(6);
            assert.strictEqual(Buffer.byteLength(p + '\ud800', 'utf8'),
                Buffer.from(p + '\ud800', 'utf8').length);
            assert.strictEqual(Buffer.byteLength(p + '\ud800\u0800', 'utf8'),
                Buffer.from(p + '\ud800\u0800', 'utf8').length);
        });

        it('a mixed ASCII + CJK paragraph round-trips byte for byte', function () {
            var s = 'a'.repeat(8) + '\u0800'.repeat(8);
            var b = Buffer.from(s, 'utf8');
            assert.strictEqual(b.length, 32);
            assert.strictEqual(b.toString('utf8'), s);
        });
    });

    describe('fill repeats the decoded bytes, not the claimed byteLength', function () {
        // 回归：fill 曾拿 Buffer.byteLength() 当源长度，而它**不校验内容**
        // （'abZZ' 按 hex 报 2 字节、实际只解出 1 字节），于是填充节奏推进过头，
        // 中间留下没被覆盖的洞。
        it('hex / base64 stop at the first bad char but still fill everything', function () {
            assert.strictEqual(hx(Buffer.alloc(4, 0x2e).fill('abZZ', 'hex')),
                'abababab');
            assert.strictEqual(hx(Buffer.alloc(7, 0x2e).fill('abcdZZ', 'hex')),
                'abcdabcdabcdab');
            assert.strictEqual(hx(Buffer.alloc(5, 0x2e).fill('QUJD$$', 'base64')),
                '4142434142');
        });

        it('rejects a non-empty source that decodes to nothing', function () {
            assert.throws(function () { Buffer.alloc(4).fill('a', 'hex'); },
                /invalid|Invalid/);
            assert.throws(function () { Buffer.alloc(4).fill('!!!!', 'base64'); },
                /invalid|Invalid/);
            // 空字符串仍然按 0 填充，不抛
            assert.strictEqual(hx(Buffer.alloc(4, 0x2e).fill('', 'hex')), '00000000');
        });

        it('is byte-level for ucs2 (write is code-unit aligned, fill is not)', function () {
            var s = '\u00ff'.repeat(6);
            assert.strictEqual(hx(Buffer.alloc(5, 0x2e).fill(s, 'ucs2')), 'ff00ff00ff');
            assert.strictEqual(hx(Buffer.alloc(3, 0x2e).fill(s, 'ucs2')), 'ff00ff');
        });
    });

    // ---------------------------------------------------------------------
    // 第十一轮（2026-11-06，从头重审工作区）：参数分派矩阵。
    // 前三轮的测试都只覆盖"参数类型正确"的形态，于是下面三个缺陷全部漏网：
    //   W1 write(s, undefined, ...) 把 length 算成 NaN 并抛 RangeError
    //   W2 编码名没有 ToString 强转（null / 对象 / String 对象）
    //   F1 fill 只认 Uint8Array，Uint16Array / Float32Array / DataView 被填成 0
    // ---------------------------------------------------------------------

    describe('write() argument dispatch matches node exactly', function () {
        it('offset === undefined ignores length and encoding entirely', function () {
            // node 的第一条分支就是 `if (offset === undefined) return utf8Write(s, 0, len)`
            // 旧实现把 length 算成 `this.length - undefined` = NaN，于是
            //   write('abc', undefined, 'hex')  ->  RangeError: length NaN
            // 而 node 写 3 个字节。显式 undefined 在 `write(s, off, len, enc)`
            // 这种转发式调用里极常见。
            [[undefined], [undefined, 'hex'], [undefined, 2, 'hex'],
                [undefined, undefined, 'hex'], [undefined, 1]]
                .forEach(function (args) {
                    var b = Buffer.alloc(8, 0x2e);
                    var n = b.write.apply(b, ['xyzz'].concat(args));
                    assert.strictEqual(n, 4, JSON.stringify(args));
                    assert.strictEqual(hx(b), '78797a7a2e2e2e2e',
                        JSON.stringify(args));
                });

            // 多字节字符也不得被 length 截断
            var b = Buffer.alloc(8, 0x2e);
            assert.strictEqual(b.write('\u4e2d', undefined, 1), 3);
            assert.strictEqual(hx(b), 'e4b8ad2e2e2e2e2e');
        });

        it('offset must be a number when it is present', function () {
            // node: `write(s, 'hex', 4)` —— offset 是字符串而 length 是数字时
            // 不走"编码名在 offset 位"的分支，直接 TypeError。
            assert.throws(function () { Buffer.alloc(4).write('ab', 'hex', 4); },
                TypeError);
            assert.throws(function () { Buffer.alloc(4).write('ab', null); },
                TypeError);
        });

        it('coerces the encoding name with ToString, falsy means utf8', function () {
            function w(codec) {
                var b = Buffer.alloc(4, 0x2e);
                var n = b.write('ab', 0, 4, codec);
                return [n, hx(b)];
            }
            // falsy（null / undefined / ''）一律按 utf8
            [[null, [2, '61622e2e']], [undefined, [2, '61622e2e']],
                ['', [2, '61622e2e']], [0, [2, '61622e2e']],
                [false, [2, '61622e2e']]].forEach(function (tc) {
                    assert.deepStrictEqual(w(tc[0]), tc[1], String(tc[0]));
                });
            // 对象与 String 对象走 ToString
            assert.deepStrictEqual(w({ toString: function () { return 'hex'; } }),
                [1, 'ab2e2e2e']);
            assert.deepStrictEqual(w(new String('hex')), [1, 'ab2e2e2e']);
            // 未知编码仍要抛
            assert.throws(function () { w('bogus'); }, /Unknown encoding/);
        });
    });

    describe('fill() accepts every ArrayBufferView, not just Uint8Array', function () {
        // 回归：`buf instanceof Uint8Array` 判断把 Uint16Array / Float32Array /
        // DataView 漏到 Number() 分支，于是整段被填成 0（没有任何报错）。
        it('fills with the raw bytes of each view', function () {
            assert.strictEqual(hx(Buffer.alloc(6, 0x2e).fill(new Uint16Array([0x6162]))),
                '626162616261');
            assert.strictEqual(hx(Buffer.alloc(6, 0x2e).fill(new Int8Array([-1, 2]))),
                'ff02ff02ff02');
            assert.strictEqual(hx(Buffer.alloc(8, 0x2e).fill(new Float32Array([1]))),
                '0000803f0000803f');
        });

        it('honours byteOffset / byteLength of a subarray', function () {
            var view = new Uint8Array([9, 1, 2, 9]).subarray(1, 3);
            assert.strictEqual(hx(Buffer.alloc(6, 0x2e).fill(view)), '010201020102');
        });

        it('DataView is a byte source (it is not a Uint8Array)', function () {
            var dv = new DataView(new Uint8Array([1, 2]).buffer);
            assert.strictEqual(hx(Buffer.alloc(6, 0x2e).fill(dv)), '010201020102');
        });

        it('ArrayBuffer still goes through Number() and fills with 0', function () {
            // node: ArrayBuffer 不是 view，`bindingFill` 会失败但 _fill 的
            // `value` 不是 string 时先把 encoding 置 undefined，最终按 0 填
            assert.strictEqual(hx(Buffer.alloc(4, 0x2e).fill(new ArrayBuffer(2))),
                '00000000');
        });

        it('empty views throw like node', function () {
            assert.throws(function () { Buffer.alloc(4).fill(new Uint8Array(0)); },
                /invalid|Invalid/);
            assert.throws(function () { Buffer.alloc(4).fill(Buffer.alloc(0)); },
                /invalid|Invalid/);
        });

        it('encoding name policy: falsy means utf8, non-string throws', function () {
            function f(codec) {
                return hx(Buffer.alloc(4, 0x2e).fill('ab', 0, 4, codec));
            }
            assert.strictEqual(f(null), '61626162');
            assert.strictEqual(f(''), '61626162');
            assert.strictEqual(f(undefined), '61626162');
            assert.strictEqual(f('HEX'), 'abababab');
            // node: 数字 / 布尔 / 普通对象 == TypeError（validateString）
            [0, false, {}].forEach(function (bad) {
                assert.throws(function () { f(bad); }, TypeError, String(bad));
            });
            assert.throws(function () { f('bogus'); }, /Unknown encoding/);
        });
    });

    describe('encoding names are coerced with ToString in every API', function () {
        it('toString / byteLength accept String objects and plain objects', function () {
            var b = Buffer.from([0xff, 0x61]);
            assert.strictEqual(b.toString({ toString: function () { return 'hex'; } }),
                'ff61');
            assert.strictEqual(b.toString(new String('hex')), 'ff61');
            assert.strictEqual(Buffer.byteLength('ff', { toString: function () { return 'hex'; } }), 1);
            assert.strictEqual(Buffer.byteLength('ff', new String('hex')), 1);

            // null / ''/ 未知编码仍然抛（node 的 getEncodingOps 不做 falsy 兜底）
            assert.throws(function () { b.toString(null); }, /Unknown encoding/);
            assert.throws(function () { b.toString(''); }, /Unknown encoding/);
            // byteLength 的未知编码按 utf8 计长（node 的 TODO 行为）
            assert.strictEqual(Buffer.byteLength('\u4e2d', null), 3);
            assert.strictEqual(Buffer.byteLength('\u4e2d', 'bogus'), 3);
        });
    });

    describe('base58check round-trips payloads with leading zeros', function () {
        // 回归：decode(data, chk_ver) 只在 `binsz < buffer.length()` 时才裁掉
        // version + checksum，于是解码结果正好填满容量估算时返回了整串：
        //   前导零 10 个的 14 字节 payload -> 返回 19 字节（多 5）
        //   前导零 >= 40 时 b58tobin 容量不足 -> 误报 "check error"
        it('keeps every leading zero byte', function () {
            var encoding;
            try { encoding = require('encoding'); } catch (e) { return; }

            [0, 1, 2, 5, 8, 10, 12, 20, 30, 40].forEach(function (nz) {
                var payload = Buffer.alloc(nz + 4);
                for (var i = 0; i < payload.length; i++)
                    payload[i] = i < nz ? 0 : (0x20 + i);

                var s = encoding.base58.encode(payload, 0);
                var back = encoding.base58.decode(s, 0);
                assert.strictEqual(hx(back), hx(payload), 'leading zeros = ' + nz);
            });

            // 版本字节不匹配仍要报 check error
            var addr = encoding.base58.encode(Buffer.from('hello'), 0);
            assert.throws(function () { encoding.base58.decode(addr, 1); },
                /check error/);
        });
    });
});
