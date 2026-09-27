// test/node_buffer_compat_test.js
//
// Buffer 模块 Node 兼容性回归（配套 plans/buffer-node-compat-audit.md / -remediation-plan.md）
//
// 约定：每条用例的期望值都在 node v23.9.0 上实测过（同一份文件可直接 `node --test` 跑通），
// 即「Node 行为即规格」。批次 1（R1–R6）修的正是 P0 级差异。
//
// ⚠ 本文件在**修复前**会在 R1/R3 的用例上死循环（这正是被测缺陷）：
//   单项复现：fibjs --test test/node_buffer_compat_test.js --test-name-pattern '<用例名>'
//   或跑 python3 temp/buffer_audit/run_compat.py（带超时，逐条隔离）。

const { describe, it } = require("node:test");
const assert = require("assert");

const hex = (b) => Buffer.from(b).toString("hex");

describe('Buffer node-compat (node v23.9 baseline)', () => {

    /* ============================ R1 ============================ */
    /* Buffer.cpp:658 while (args[arg_cnt-1]->IsUndefined()) --arg_cnt; 下溢 → 死循环 */

    it('R1 Buffer.byteLength() 无参抛 TypeError', () => {
        assert.throws(() => Buffer.byteLength(), TypeError);
    });

    it('R1 Buffer.byteLength(undefined) 抛 TypeError', () => {
        assert.throws(() => Buffer.byteLength(undefined), TypeError);
    });

    it('R1 Buffer.byteLength(undefined, "utf8") 抛 TypeError', () => {
        assert.throws(() => Buffer.byteLength(undefined, "utf8"), TypeError);
    });

    /* ============================ R2 ============================ */
    /* buffer.js:19 getPool() 无尺寸校验 → Buffer.alloc(NaN) 让 poolOffset 变 NaN，此后所有池化 Buffer 互相别名 */

    it('R2 Buffer.alloc(NaN) 抛 RangeError', () => {
        assert.throws(() => Buffer.alloc(NaN), RangeError);
    });

    it('R2 Buffer.alloc(-1) 抛 RangeError；alloc(2.5) 按 ToIndex 截断（node 语义）', () => {
        assert.throws(() => Buffer.alloc(-1), RangeError);
        assert.throws(() => Buffer.allocUnsafe(-1), RangeError);
        assert.equal(Buffer.alloc(2.5).length, 2);
        assert.equal(Buffer.allocUnsafe(2.5).length, 2);
        assert.equal(Buffer.allocUnsafeSlow(2.5).length, 2);
    });

    it('R2 Buffer.allocUnsafe(NaN) / new Buffer(NaN) 抛 RangeError', () => {
        assert.throws(() => Buffer.allocUnsafe(NaN), RangeError);
        assert.throws(() => new Buffer(NaN), RangeError);
    });

    it('R2 Buffer.concat(list, NaN) 抛 RangeError', () => {
        assert.throws(() => Buffer.concat([Buffer.from("ab")], NaN), RangeError);
    });

    it('R2 非法尺寸不得毒化 pool（后续 Buffer 不得别名）', () => {
        const keeper = Buffer.allocUnsafe(8);
        keeper.fill(0x11);

        // 非法/边界尺寸：该抛的抛，不该抛的不得破坏池游标
        assert.throws(() => Buffer.alloc(NaN), RangeError);
        assert.throws(() => Buffer.alloc(-1), RangeError);
        assert.throws(() => Buffer.allocUnsafe(NaN), RangeError);
        assert.throws(() => new Buffer(NaN), RangeError);
        Buffer.alloc(2.5);        // node: 接受，长度 2（ToIndex 截断）

        const later = Buffer.allocUnsafe(8);
        later.fill(0x22);

        assert.equal(hex(keeper), "1111111111111111", "keeper 被后来的 Buffer 覆盖 → pool 已被毒化");
        assert.notEqual(keeper.byteOffset, later.byteOffset);

        const x = Buffer.allocUnsafe(8), y = Buffer.allocUnsafe(8);
        assert.ok(y.byteOffset > x.byteOffset, "池游标未前进（被小数/NaN 污染）");
    });

    it('R2 非法尺寸后 Buffer.from(string) 不得互相别名', () => {
        assert.throws(() => Buffer.alloc(NaN), RangeError);
        const s1 = Buffer.from("hello");
        const s2 = Buffer.from("world");
        s2.write("XXXXX");
        assert.equal(s1.toString(), "hello");
    });

    it('R2 健康 pool 的 byteOffset 必须递增', () => {
        const a = Buffer.allocUnsafe(8);
        const b = Buffer.allocUnsafe(8);
        const c = Buffer.allocUnsafe(8);
        assert.ok(a.byteOffset < b.byteOffset && b.byteOffset < c.byteOffset,
            `byteOffset 未递增: ${a.byteOffset},${b.byteOffset},${c.byteOffset}`);
    });

    /* ============================ R3 ============================ */
    /* buffer.js:373 fill(): buf_byteLength === 0 时 while (offset < end) 死循环；
       越界/缺参语义与 Node 相反 */

    it('R3 fill("") 零填充（node 语义，且必须不死循环）', () => {
        const b = Buffer.alloc(4, 0x41);
        assert.strictEqual(b.fill(""), b);
        assert.equal(hex(b), "00000000");
    });

    it('R3 Buffer.alloc(4, "") 得到零填充', () => {
        assert.equal(hex(Buffer.alloc(4, "")), "00000000");
    });

    it('R3 fill() / fill(undefined) 零填充且返回自身', () => {
        const b = Buffer.from([1, 2]);
        assert.strictEqual(b.fill(), b);
        assert.equal(hex(b), "0000");
        assert.equal(hex(Buffer.from([1, 2]).fill(undefined)), "0000");
    });

    it('R3 fill offset 越界为 no-op', () => {
        assert.equal(hex(Buffer.alloc(4).fill(0x41, 9)), "00000000");
        assert.equal(hex(Buffer.alloc(4).fill(0x41, 3, 1)), "00000000");
    });

    it('R3 fill 负数/非整数 offset 抛 RangeError', () => {
        assert.throws(() => Buffer.alloc(4).fill("a", -1), RangeError);
        assert.throws(() => Buffer.alloc(4).fill("a", 0, 99), RangeError);
        assert.throws(() => Buffer.alloc(4).fill("a", 1.5, 2.5), RangeError);
    });

    it('R3 fill 数组不得被当作字节源', () => {
        // node: 不报错也不拷贝数组内容（仍是零）
        const b = Buffer.alloc(5).fill([1, 2]);
        assert.equal(hex(b), "00000000 00".replace(/ /g, ""));
    });

    it('R3 fill(string, offset[, end]) 常规语义保持', () => {
        assert.equal(hex(Buffer.alloc(4).fill(0x41, 1)), "00414141");
        assert.equal(hex(Buffer.alloc(4).fill(0x41, 1, 3)), "00414100");
        assert.equal(Buffer.alloc(6).fill("中").toString("hex"), "e4b8ade4b8ad");
    });

    /* ============================ R4 ============================ */
    /* Buffer.from/alloc 对非法类型静默成功；allocUnsafeSlow  仍走 pool；alloc 非零填充风险 */

    it('R4 Buffer.from(非 buffer 类型) 抛 TypeError', () => {
        [3, null, undefined, true, {}, new Map()].forEach((v) => {
            assert.throws(() => Buffer.from(v), TypeError, `Buffer.from(${String(v)}) 应抛 TypeError`);
        });
    });

    it('R4 Buffer.alloc(非 number) 抛 TypeError', () => {
        ["5", null, undefined, true, {}].forEach((v) => {
            assert.throws(() => Buffer.alloc(v), TypeError, `Buffer.alloc(${String(v)}) 应抛 TypeError`);
        });
    });

    it('R4 Buffer.alloc 必须零填充（即使 pool 已被写过）', () => {
        for (let i = 0; i < 8; i++) {
            const dirty = Buffer.allocUnsafe(4000);
            dirty.fill(0xaa);
        }
        assert.equal(hex(Buffer.alloc(64)), "0".repeat(128));
        assert.equal(hex(Buffer.alloc(64, 0)), "0".repeat(128));
    });

    it('R4 Buffer.alloc / allocUnsafeSlow 非池化，allocUnsafe(0) 长度为 0', () => {
        assert.equal(Buffer.alloc(5).buffer.byteLength, 5);
        assert.equal(Buffer.allocUnsafeSlow(5).buffer.byteLength, 5);
        assert.equal(Buffer.allocUnsafe(0).buffer.byteLength, 0);
    });

    it('R4 Buffer.poolSize 存在且可写', () => {
        assert.equal(Buffer.poolSize, 8192);
        const old = Buffer.poolSize;
        Buffer.poolSize = 4096;
        assert.equal(Buffer.poolSize, 4096);
        Buffer.poolSize = old;
    });

    it('R4 alloc(size, fill, codec) 重载保持', () => {
        assert.equal(Buffer.alloc(11, 'aGVsbG8gd29ybGQ=', 'base64').toString(), 'hello world');
        assert.equal(Buffer.alloc(5, 'ab').toString(), 'ababa');
    });

    /* ============================ R5 ============================ */
    /* buffer.js:1237 utf8Slice 一族：receiver 非 Buffer 时落到 %TypedArray%.prototype.toString
       → join 整段视图（忽略 start/end），大视图抛 RangeError: Invalid string length */

    it('R5 utf8Slice receiver 必须是 Uint8Array', () => {
        assert.throws(() => Buffer.prototype.utf8Slice.call({}, 0, 1), TypeError);
        assert.throws(() => Buffer.prototype.utf8Slice.call(null, 0, 1), TypeError);
    });

    it('R5 utf8Slice 支持普通 Uint8Array 且遵守 start/end', () => {
        assert.equal(Buffer.prototype.utf8Slice.call(new Uint8Array([0x61, 0x62, 0x63]), 0, 2), "ab");
        assert.equal(Buffer.prototype.utf8Slice.call(new Uint8Array([0x61, 0x62, 0x63]), 1), "bc");
    });

    it('R5 utf8Slice 大视图不得返回 join 串（Invalid string length 同源）', () => {
        const big = new Uint8Array(1 << 20);
        big[0] = 0x61;
        const r = Buffer.prototype.utf8Slice.call(big, 0, 1);
        assert.equal(r, "a", "返回了整段视图的拼接串（join 回退）");
    });

    it('R5 真 Buffer 的 utf8Slice 越界 end 抛 RangeError', () => {
        assert.throws(() => Buffer.from("ab").utf8Slice(0, 99), RangeError);
        assert.equal(Buffer.from("hello").utf8Slice(0, 3), "hel");
        assert.equal(Buffer.from("hello").hexSlice(0, 2), "6865");
    });

    it('R5 base64urlSlice / base64urlWrite 存在', () => {
        assert.equal(typeof Buffer.prototype.base64urlSlice, "function");
        assert.equal(typeof Buffer.prototype.base64urlWrite, "function");
        assert.equal(Buffer.from([0xfb, 0xff]).base64urlSlice(0, 2), "-_8");
    });

    /* ============================ R6 ============================ */
    /* Buffer.cpp:551 proto_write: 无 codec 分支返回 max_length（容量）而非写入字节数；区间不校验 */

    it('R6 write 返回实际写入字节数', () => {
        assert.equal(Buffer.alloc(8).write("abc"), 3);
        assert.equal(Buffer.alloc(8).write(""), 0);
        assert.equal(Buffer.alloc(8).write("abc", 1), 3);
        assert.equal(Buffer.alloc(3).write("abcd"), 3);
        assert.equal(Buffer.alloc(8).write("中文", 0, 4), 3);
    });

    it('R6 write 带 encoding 的返回值保持', () => {
        assert.equal(Buffer.alloc(10).write("abcd", "utf8"), 4);
        assert.equal(Buffer.alloc(3).write("abcd", "utf8"), 3);
        assert.equal(Buffer.alloc(10).write("31323334", 0, 4, "hex"), 4);
    });

    it('R6 write length 越界/负数抛 RangeError', () => {
        assert.throws(() => Buffer.alloc(4).write("abc", 0, 99), RangeError);
        assert.throws(() => Buffer.alloc(4).write("abc", 0, -1), RangeError);
    });

    it('R6 write offset 越界抛 RangeError', () => {
        assert.throws(() => Buffer.alloc(4).write("abc", 99), RangeError);
        assert.throws(() => Buffer.alloc(4).write("abc", -1), RangeError);
    });

    /* ============================ R7 ============================ */
    /* buf.slice 必须是共享内存的视图（继承的 %TypedArray%.slice 是拷贝） */

    it('R7 slice 是共享视图', () => {
        const b = Buffer.from('abcd');
        const s = b.slice(1, 3);
        s[0] = 0x5a;
        assert.equal(b.toString('hex'), '615a6364');
        assert.equal(hex(s), '5a63');
        assert.equal(b.slice(1, 3).buffer, b.buffer);
        assert.ok(Buffer.isBuffer(b.slice()));
    });

    it('R7 slice 的负数与钳制语义', () => {
        const b = Buffer.from('abcdefghih');
        assert.equal(b.slice(0, 3).toString(), 'abc');
        assert.equal(b.slice(6, 5).toString(), '');
        assert.equal(b.slice(0, 11).toString(), 'abcdefghih');
        assert.equal(b.slice(8).toString(), 'ih');
        assert.equal(b.slice(-20, 2).toString(), 'ab');
        assert.equal(Buffer.from('buffer').slice(-6, -1).toString(), 'buffe');
    });

    /* ============================ R8 ============================ */
    /* includes 落到 %TypedArray%.includes（恒 false）；indexOf/lastIndexOf 缺三参形式 */

    it('R8 includes 支持 string/Buffer/byteOffset', () => {
        const b = Buffer.from('hello');
        assert.equal(b.includes('ell'), true);
        assert.equal(b.includes('ell', 1), true);
        assert.equal(b.includes('ell', 2), false);
        assert.equal(b.includes(Buffer.from('ell')), true);
        assert.equal(b.includes(new Uint8Array([0x65, 0x6c, 0x6c])), true);
        assert.equal(b.includes(0x6c), true);
        assert.equal(b.includes(0x7a), false);
        assert.equal(b.includes(''), true);
        assert.equal(b.includes('', 99), true);
        assert.equal(Object.prototype.hasOwnProperty.call(Buffer.prototype, 'includes'), true);
    });

    it('R8 indexOf/lastIndexOf 三参（encoding）形式', () => {
        assert.equal(Buffer.from('hello').indexOf('l', 0, 'utf8'), 2);
        assert.equal(Buffer.from([0x61, 0x62, 0x63]).indexOf('62', 0, 'hex'), 1);
        assert.equal(Buffer.from([0x61, 0x62, 0x63]).lastIndexOf('62', 1, 'hex'), 1);
        assert.equal(Buffer.from([0x61, 0x62, 0x63]).lastIndexOf('62', 0, 'hex'), -1);
        assert.equal(Buffer.from('hello').indexOf('l', 3), 3);
    });

    it('R8 includes/indexOf/lastIndexOf 非法 value 抛 TypeError', () => {
        const b = Buffer.from('abc');
        [undefined, null, {}].forEach((v) => {
            assert.throws(() => b.includes(v), TypeError);
            assert.throws(() => b.indexOf(v), TypeError);
            assert.throws(() => b.lastIndexOf(v), TypeError);
        });
    });

    /* ============================ R9 ============================ */
    /* compare 只接受 1 个参数（区间形式全缺）；equals/compare 静默接受字符串 */

    it('R9 compare 区间形式', () => {
        const abc = Buffer.from('abc');
        assert.equal(abc.compare(Buffer.from('zabc'), 1), 0);
        assert.equal(abc.compare(Buffer.from('zabc'), 1, 3), 1);
        assert.equal(abc.compare(Buffer.from('zabc'), 0, 3, 1), -1);
        assert.equal(Buffer.from('abcd').compare(Buffer.from('wxbcd'), 1, 4, 0, 3), -1);
        assert.equal(abc.compare(Buffer.from('abc'), 2, 1), 1);
    });

    it('R9 compare 区间越界/负数抛 RangeError', () => {
        const abc = Buffer.from('abc');
        assert.throws(() => abc.compare(Buffer.from('abc'), 0, 99), RangeError);
        assert.throws(() => abc.compare(Buffer.from('abc'), -1), RangeError);
    });

    it('R9 compare/equals 拒绝非 Uint8Array', () => {
        assert.throws(() => Buffer.from('a').equals('a'), TypeError);
        assert.throws(() => Buffer.from('a').equals(null), TypeError);
        assert.throws(() => Buffer.from('a').compare('a'), TypeError);
        assert.throws(() => Buffer.compare('a', 'b'), TypeError);
        assert.equal(Buffer.from('a').equals(new Uint8Array([0x61])), true);
        assert.equal(Buffer.from('a').equals(Buffer.alloc(0)), false);
    });

    /* ============================ R10 ============================ */
    /* copy 源区间越界时越界读且返回值虚高；负数参数静默 */

    it('R10 copy 源区间按源长度截断且返回实际字节数', () => {
        const dst = Buffer.alloc(4);
        assert.equal(Buffer.from('ab').copy(dst, 0, 0, 99), 2);
        assert.equal(hex(dst), '61620000');
        assert.equal(Buffer.from('abc').copy(Buffer.alloc(10), 0, 1, 3), 2);
        assert.equal(Buffer.from('abcd').copy(Buffer.alloc(4), 0, 3, 1), 0);
        assert.equal(Buffer.from('ab').copy(Buffer.alloc(2), 5), 0);
    });

    it('R10 copy 负数/越界参数抛 RangeError', () => {
        assert.throws(() => Buffer.from('ab').copy(Buffer.alloc(4), -1), RangeError);
        assert.throws(() => Buffer.from('ab').copy(Buffer.alloc(4), 0, -1), RangeError);
        assert.throws(() => Buffer.from('abcd').copy(Buffer.alloc(4), 0, 0, -1), RangeError);
    });

    it('R10 copy target 必须是 Uint8Array', () => {
        assert.throws(() => Buffer.from('ab').copy(new ArrayBuffer(4)), TypeError);
        assert.throws(() => Buffer.from('ab').copy({}), TypeError);
        const u = new Uint8Array(4);
        assert.equal(Buffer.from('ab').copy(u), 2);
        assert.equal(hex(u), '61620000');
    });

    /* ============================ R11 ============================ */
    /* 缺失 API：24 个 *Uint* 别名、swap16/32/64、copyBytesFrom、inspect、parent/offset */

    it('R11 readUint*/writeUint* 别名与 UInt 版本同函数', () => {
        ['readUint8', 'readUint16LE', 'readUint16BE', 'readUint32LE', 'readUint32BE', 'readUintLE', 'readUintBE',
            'writeUint8', 'writeUint16LE', 'writeUint16BE', 'writeUint32LE', 'writeUint32BE', 'writeUintLE', 'writeUintBE'].forEach((m) => {
                assert.equal(typeof Buffer.prototype[m], 'function', m + ' 缺失');
                const upper = m.replace('Uint', 'UInt');
                assert.strictEqual(Buffer.prototype[m], Buffer.prototype[upper], m + ' 未复用 ' + upper);
            });

        const b = Buffer.from([0x01, 0x02, 0x03, 0x04]);
        assert.equal(b.readUint16LE(0), 0x0201);
        assert.equal(b.readUint32BE(0), 0x01020304);
        const w = Buffer.alloc(4);
        w.writeUint16LE(0x0201, 0);
        assert.equal(hex(w), '01020000');
    });

    it('R11 readBigUint64LE / writeBigUint64LE', () => {
        assert.equal(String(Buffer.from([1, 0, 0, 0, 0, 0, 0, 0]).readBigUint64LE(0)), '1');
        const b = Buffer.alloc(8);
        b.writeBigUint64LE(1n, 0);
        assert.equal(hex(b), '0100000000000000');
    });

    it('R11 swap16/32/64 原地翻转并返回自身', () => {
        const b = Buffer.from([1, 2]);
        assert.strictEqual(b.swap16(), b);
        assert.equal(hex(b), '0201');

        const c = Buffer.from([1, 2, 3, 4, 5, 6, 7, 8]);
        c.swap32();
        assert.equal(hex(c), '0403020108070605');
        c.swap64();
        assert.equal(hex(c), '0506070801020304');

        assert.throws(() => Buffer.from([1]).swap16(), RangeError);
        assert.throws(() => Buffer.from([1, 2, 3, 4, 5]).swap32(), RangeError);
        assert.throws(() => Buffer.from([1, 2, 3, 4]).swap64(), RangeError);
    });

    it('R11 Buffer.copyBytesFrom', () => {
        assert.equal(hex(Buffer.copyBytesFrom(new Uint8Array([1, 2, 3]))), '010203');
        assert.equal(hex(Buffer.copyBytesFrom(new Uint8Array([1, 2, 3]), 1)), '0203');
        assert.equal(hex(Buffer.copyBytesFrom(new Uint8Array([1, 2, 3]), 1, 1)), '02');
        assert.throws(() => Buffer.copyBytesFrom('abc'), TypeError);
    });

    it('R11 inspect() 形态', () => {
        assert.equal(Buffer.from('ab').inspect(), '<Buffer 61 62>');
        assert.ok(Buffer.alloc(60, 0x41).inspect().endsWith('... 10 more bytes>'));
        assert.equal(Buffer.alloc(0).inspect(), '<Buffer >');
    });

    it('R11 parent/offset 访问器', () => {
        const b = Buffer.from('ab');
        assert.strictEqual(b.parent, b.buffer);
        assert.strictEqual(b.offset, b.byteOffset);
    });

    /* ============================ R12 ============================ */
    /* require('buffer') 是类本身，缺 SlowBuffer / constants / kMaxLength / transcode / atob … */

    it('R12 require("buffer") 是模块对象且 Buffer 与全局同源', () => {
        const bm = require('buffer');
        assert.equal(typeof bm, 'object');
        assert.strictEqual(bm.Buffer, Buffer);
    });

    it('R12 SlowBuffer 非池化且可用 new 调用', () => {
        const SB = require('buffer').SlowBuffer;
        assert.equal(typeof SB, 'function');
        assert.equal(Buffer.isBuffer(SB(3)), true);
        assert.equal(SB(3).buffer.byteLength, 3);
        assert.equal(new SB(3).length, 3);
    });

    it('R12 constants / kMaxLength / kStringMaxLength / INSPECT_MAX_BYTES', () => {
        const bm = require('buffer');
        assert.equal(bm.kMaxLength, Number.MAX_SAFE_INTEGER);
        assert.equal(bm.kStringMaxLength, 536870888);
        assert.equal(bm.INSPECT_MAX_BYTES, 50);
        assert.equal(bm.constants.MAX_LENGTH, Number.MAX_SAFE_INTEGER);
        assert.equal(bm.constants.MAX_STRING_LENGTH, 536870888);
    });

    it('R12 transcode', () => {
        const bm = require('buffer');
        assert.equal(bm.transcode(Buffer.from('ab'), 'utf8', 'utf16le').toString('hex'), '61006200');
        assert.equal(bm.transcode(Buffer.from('ab'), 'utf8', 'latin1').toString('hex'), '6162');
    });

    it('R12 atob / btoa / isUtf8 / isAscii / Blob', () => {
        const bm = require('buffer');
        assert.equal(bm.atob('aGVsbG8='), 'hello');
        assert.equal(bm.btoa('hello'), 'aGVsbG8=');
        assert.equal(bm.isUtf8(Buffer.from('hello')), true);
        assert.equal(bm.isUtf8(Buffer.from([0xff])), false);
        assert.equal(bm.isAscii(Buffer.from('hello')), true);
        assert.equal(bm.isAscii(Buffer.from([0xe9])), false);
        assert.equal(typeof bm.Blob, 'function');
    });

    /* ============================ R13 ============================ */
    /* isEncoding 接受空串/含空白串；byteLength 拒绝 DataView/SAB、对未知编码抛错 */

    it('R13 isEncoding 拒绝空白/空串，保留扩展编码', () => {
        assert.equal(Buffer.isEncoding('utf8'), true);
        assert.equal(Buffer.isEncoding('UTF-8'), true);
        assert.equal(Buffer.isEncoding('base64url'), true);
        assert.equal(Buffer.isEncoding('utf8 '), false);
        assert.equal(Buffer.isEncoding('utf 8'), false);
        assert.equal(Buffer.isEncoding(''), false);
        assert.equal(Buffer.isEncoding('bogus'), false);
    });

    it('R13 byteLength 忽略未知编码，接受 DataView/SharedArrayBuffer', () => {
        assert.equal(Buffer.byteLength('abc', 'bogus'), 3);
        assert.equal(Buffer.byteLength(new DataView(new ArrayBuffer(3))), 3);
        assert.equal(Buffer.byteLength(new SharedArrayBuffer(3)), 3);
        assert.equal(Buffer.byteLength('中'), 3);
    });

    /* ============================ R14 ============================ */
    /* 错误对象必须带 node 的 code 与正确类型（ERR_OUT_OF_RANGE / ERR_INVALID_ARG_TYPE / …） */

    it('R14 错误对象带 code 与正确类型', () => {
        const check = (fn, ctor, code) => {
            let err = null;
            try { fn(); } catch (e) { err = e; }
            assert.ok(err, '应当抛错：' + fn);
            assert.ok(err instanceof ctor,
                `期望 ${ctor.name}，实际 ${err && err.constructor && err.constructor.name}（code=${err && err.code}）`);
            assert.equal(err.code, code);
        };

        check(() => Buffer.alloc(-1), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(NaN), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc('3'), TypeError, 'ERR_INVALID_ARG_TYPE');
        check(() => Buffer.from(3), TypeError, 'ERR_INVALID_ARG_TYPE');
        check(() => Buffer.from('a').toString('bogus'), TypeError, 'ERR_UNKNOWN_ENCODING');
        check(() => Buffer.from('a', 'bogus'), TypeError, 'ERR_UNKNOWN_ENCODING');
        check(() => Buffer.alloc(2).readUInt8(-1), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(2).readUInt8(0.5), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(2).readUInt32LE(0), RangeError, 'ERR_BUFFER_OUT_OF_BOUNDS');
        check(() => Buffer.alloc(8).readUInt32LE(5), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(2).writeUInt8(256, 0), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(2).writeUInt8(1, 5), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(2).writeUInt8(1, 0.5), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(8).writeUIntLE(1, 0, 7), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.alloc(2).write(5), TypeError, 'ERR_INVALID_ARG_TYPE');
        check(() => Buffer.alloc(4).write('abc', 0, 99), RangeError, 'ERR_OUT_OF_RANGE');
        check(() => Buffer.from('ab').copy(new ArrayBuffer(4)), TypeError, 'ERR_INVALID_ARG_TYPE');
        check(() => Buffer.from([1]).swap16(), RangeError, 'ERR_INVALID_BUFFER_SIZE');
        check(() => Buffer.alloc(2).fill('a', -1), RangeError, 'ERR_OUT_OF_RANGE');
    });

    it('R14 错误消息形态与 node 一致（取值范围/非整数/越界）', () => {
        let err = null;
        try { Buffer.alloc(2).readUInt8(-1); } catch (e) { err = e; }
        assert.equal(err.message, 'The value of "offset" is out of range. It must be >= 0 and <= 1. Received -1');

        err = null;
        try { Buffer.alloc(2).readUInt8(0.5); } catch (e) { err = e; }
        assert.equal(err.message, 'The value of "offset" is out of range. It must be an integer. Received 0.5');

        err = null;
        try { Buffer.alloc(2).writeUInt8(256, 0); } catch (e) { err = e; }
        assert.equal(err.message, 'The value of "value" is out of range. It must be >= 0 and <= 255. Received 256');

        err = null;
        try { Buffer.alloc(2).readUInt32LE(0); } catch (e) { err = e; }
        assert.equal(err.message, 'Attempt to access memory outside buffer bounds');

        err = null;
        try { Buffer.from('a').toString('bogus'); } catch (e) { err = e; }
        assert.equal(err.message, 'Unknown encoding: bogus');
    });

    /* ============================ R14 补充（静默数据/形态） ============================ */

    it('R14b toLocaleString 与 toString 同义', () => {
        assert.strictEqual(Buffer.prototype.toLocaleString, Buffer.prototype.toString);
        assert.equal(Buffer.from('ab').toLocaleString(), 'ab');
    });

    it('R14b toString 区间负数按 0 钳制', () => {
        assert.equal(Buffer.from('abcd').toString('utf8', -2), 'abcd');
        assert.equal(Buffer.from('abcd').toString('utf8', 0, -1), '');
        assert.equal(Buffer.from('abcd').toString('utf8', 0, 99), 'abcd');
        assert.equal(Buffer.from('abcd').toString('utf8', 1.5, 3), 'bc');
    });

    it('R14b hex 遇到非法字符即停止（不再跳过）', () => {
        assert.equal(Buffer.from('61zz62', 'hex').toString('hex'), '61');
        assert.equal(Buffer.from(' 61', 'hex').length, 0);
        assert.equal(Buffer.from('AAbb', 'hex').toString('hex'), 'aabb');
        assert.equal(Buffer.from('61626', 'hex').toString('hex'), '6162');
    });

    it('R14c byteLength/alloc/from(ab)/concat/toString 的校验细节', () => {
        const check = (fn, ctor, code, msg) => {
            let err = null;
            try { fn(); } catch (e) { err = e; }
            assert.ok(err, '应当抛错');
            assert.ok(err instanceof ctor, `期望 ${ctor.name}，实际 ${err.constructor.name}`);
            assert.equal(err.code, code);
            if (msg !== undefined) assert.equal(err.message, msg);
        };

        check(() => Buffer.byteLength(123), TypeError, 'ERR_INVALID_ARG_TYPE',
            'The "string" argument must be of type string or an instance of Buffer or ArrayBuffer. Received type number (123)');
        check(() => Buffer.byteLength(null), TypeError, 'ERR_INVALID_ARG_TYPE',
            'The "string" argument must be of type string or an instance of Buffer or ArrayBuffer. Received null');
        check(() => Buffer.byteLength([1, 2, 3]), TypeError, 'ERR_INVALID_ARG_TYPE',
            'The "string" argument must be of type string or an instance of Buffer or ArrayBuffer. Received an instance of Array');
        check(() => Buffer.alloc(NaN), RangeError, 'ERR_OUT_OF_RANGE',
            'The value of "size" is out of range. It must be >= 0 && <= 9007199254740991. Received NaN');
        check(() => Buffer.from(new ArrayBuffer(4), 5), RangeError, 'ERR_BUFFER_OUT_OF_BOUNDS',
            '"offset" is outside of buffer bounds');
        check(() => Buffer.from(new ArrayBuffer(4), 2, 99), RangeError, 'ERR_BUFFER_OUT_OF_BOUNDS',
            '"length" is outside of buffer bounds');

        // node: from(ab, offset, length) 是宽松转换（offset 原样交给 V8，length 走 ToLength）
        assert.equal(Buffer.from(new ArrayBuffer(4), 2.5).length, 2);
        assert.equal(Buffer.from(new ArrayBuffer(4), '1').length, 3);
        assert.equal(Buffer.from(new ArrayBuffer(4), 'x').length, 4);
        assert.equal(Buffer.from(new ArrayBuffer(4), NaN).length, 4);
        assert.equal(Buffer.from(new ArrayBuffer(4), 0, 2.5).length, 2);
        assert.equal(Buffer.from(new ArrayBuffer(4), 0, '3').length, 3);
        assert.equal(Buffer.from(new ArrayBuffer(4), 0, -1).length, 0);
        assert.equal(Buffer.from(new ArrayBuffer(4), 4, 0).length, 0);
        assert.throws(() => Buffer.from(new ArrayBuffer(4), -1), RangeError);
        assert.throws(() => Buffer.from(new ArrayBuffer(4), 3, 2), RangeError);
        check(() => Buffer.concat([7]), TypeError, 'ERR_INVALID_ARG_TYPE',
            'The "list[0]" argument must be an instance of Buffer or Uint8Array. Received type number (7)');
        check(() => Buffer.from('a').toString(''), TypeError, 'ERR_UNKNOWN_ENCODING', 'Unknown encoding: ');
        check(() => Buffer.from('a').toString(null), TypeError, 'ERR_UNKNOWN_ENCODING', 'Unknown encoding: null');

        // 合法路径不受影响
        assert.equal(Buffer.byteLength(new ArrayBuffer(3)), 3);
        assert.equal(Buffer.byteLength(new DataView(new ArrayBuffer(3))), 3);
        assert.equal(Buffer.byteLength('中'), 3);
        assert.equal(Buffer.from(new ArrayBuffer(4), 2, 2).length, 2);
        assert.equal(Buffer.from('ab').toString(), 'ab');
        assert.equal(Buffer.concat([Buffer.from('ab')], 5).toString('hex'), '6162000000');

        // node: from(arrayBuffer, offset, length) 的 offset/length 是宽松转换
        // （交给 V8 的 TypedArray 构造：2.5→2、'1'→1、'x'/NaN→0、-1 当 length 变 0）
        assert.equal(Buffer.from(new ArrayBuffer(4), 2.5).length, 2);
        assert.equal(Buffer.from(new ArrayBuffer(4), '1').length, 3);
        assert.equal(Buffer.from(new ArrayBuffer(4), 'x').length, 4);
        assert.equal(Buffer.from(new ArrayBuffer(4), NaN).length, 4);
        assert.equal(Buffer.from(new ArrayBuffer(4), 1, -1).length, 0);
        assert.equal(Buffer.from(new ArrayBuffer(4), 1, 2.5).length, 2);
        assert.equal(Buffer.from(new ArrayBuffer(4), 1, '2').length, 2);
        assert.throws(() => Buffer.from(new ArrayBuffer(4), -1), RangeError);
    });

    it('R14d BigInt 写入类型 / readLE 缺参 / write codec 校验', () => {
        const b = Buffer.alloc(8);
        const MIX = 'Cannot mix BigInt and other types, use explicit conversions';
        const mixErr = (fn, what) => {
            let err = null;
            try { fn(); } catch (e) { err = e; }
            assert.ok(err instanceof TypeError, what + ' 应抛 TypeError');
            assert.equal(err.message, MIX, what);
        };

        // node: writeBigInt64*/writeBigUInt64* 只接受 bigint
        [1, 1.5, '1', null, undefined, true].forEach((v) => mixErr(() => b.writeBigInt64LE(v, 0), 'writeBigInt64LE(' + String(v) + ')'));
        [1, null].forEach((v) => mixErr(() => b.writeBigUInt64LE(v, 0), 'writeBigUInt64LE(' + String(v) + ')'));
        assert.equal(b.writeBigInt64LE(1n, 0), 8);
        assert.equal(hex(b), '0100000000000000');

        // 指针形式缺 offset → TypeError ERR_INVALID_ARG_TYPE
        ['readUIntLE', 'readUIntBE', 'readIntLE', 'readIntBE'].forEach((m) => {
            let err = null;
            try { Buffer.alloc(8)[m](); } catch (e) { err = e; }
            assert.ok(err instanceof TypeError, m + ' 应抛 TypeError');
            assert.equal(err.code, 'ERR_INVALID_ARG_TYPE', m);
        });

        // write 的 codec 校验
        let err = null;
        try { Buffer.alloc(4).write('a', 0, 'bogus'); } catch (e) { err = e; }
        assert.ok(err instanceof TypeError);
        assert.equal(err.code, 'ERR_UNKNOWN_ENCODING');
        assert.equal(err.message, 'Unknown encoding: bogus');
    });

    it('R4b 0 长度 Buffer 可安全传给 native API（data() 不能是 nullptr）', () => {
        const crypto = require('crypto');
        const h = crypto.createHash('sha256');
        h.update(Buffer.alloc(0));
        h.update(Buffer.from(''));
        h.update(Buffer.allocUnsafe(0));
        assert.equal(typeof h.digest('hex'), 'string');

        // 0 长度缓冲的 buffer.byteLength 语义与 node 一致
        assert.equal(Buffer.alloc(0).buffer.byteLength, 0);
        assert.equal(Buffer.from('').buffer.byteLength, 0);
        assert.equal(Buffer.allocUnsafe(0).buffer.byteLength, 0);
    });

    it('R12b isUtf8 与 node 一致（overlong / 代理区 / 越界 / 截断）', () => {
        const isUtf8 = require('buffer').isUtf8;
        const cases = [
            [[0x61], true], [[0xc3, 0xa9], true], [[0xe4, 0xb8, 0xad], true],
            [[0xf0, 0x9f, 0x98, 0x80], true], [[0xc2, 0x80], true],
            [[0x80], false], [[0xc0, 0x80], false], [[0xc1, 0xbf], false],
            [[0xe0, 0x80, 0x80], false], [[0xed, 0xa0, 0x80], false], [[0xed, 0xbf, 0xbf], false],
            [[0xf4, 0x90, 0x80, 0x80], false], [[0xf7, 0xbf, 0xbf, 0xbf], false],
            [[0xc3], false], [[0xe4, 0xb8], false], [[0xf0, 0x9f, 0x98], false],
            [[0xfe], false], [[0xff], false]
        ];
        cases.forEach(([bytes, expected]) => {
            assert.equal(isUtf8(Buffer.from(bytes)), expected, 'isUtf8(' + bytes.toString('hex') + ')');
        });
    });
});
