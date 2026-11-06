var encoding = require('encoding');
var {
    Uint8Array,
    ArrayBuffer,
    TypedArrayPrototypeFill,
    TypedArrayPrototypeCopyWithin,
    TypedArrayPrototypeSet
} = require('internal/primordials.js');

var {
    typeErrorWithCode,
    rangeErrorWithCode,
    unknownEncoding,
    bufferOutOfBounds,
    invalidBufferSize,
} = require('internal/errors');

let defaultPoolSize = 8 * 1024;
let poolOffset, allocPool;

// Internal marker used as the `byte_offset` argument of an internal `new Buffer(size, NO_POOL)`
// call to request an allocation outside the pool (node does not pool alloc()/allocUnsafeSlow()).
const NO_POOL = {};

// node-compatible error objects: `code` + the right error class are what the
// callers switch on (`e.code === 'ERR_OUT_OF_RANGE'`, `e instanceof RangeError`).
function describeReceived(value) {
    if (value === null)
        return 'null';
    if (typeof value === 'number')
        return 'type number (' + value + ')';
    if (typeof value === 'string')
        return "type string ('" + value + "')";
    if (typeof value === 'object')
        return 'an instance of ' + (value.constructor ? value.constructor.name : 'Object');
    return 'type ' + typeof value + ' (' + String(value) + ')';
}

function makeInvalidArgType(msg) {
    return typeErrorWithCode('ERR_INVALID_ARG_TYPE', msg);
}

function invalidArgType(name, expected, value) {
    return makeInvalidArgType(`The "${name}" argument must be ${expected}. Received ${describeReceived(value)}`);
}

function invalidArgValue(name, value) {
    return typeErrorWithCode('ERR_INVALID_ARG_VALUE', `The argument '${name}' is invalid. Received ${describeReceived(value)}`);
}

function outOfRange(name, range, value) {
    return rangeErrorWithCode('ERR_OUT_OF_RANGE', `The value of "${name}" is out of range. It must be ${range}. Received ${value}`);
}

// node: size must be a number (TypeError); NaN/Infinity/negative/too large are RangeError;
// a fractional size is truncated (ToIndex).  The pool cursor MUST stay an integer — a NaN
// cursor keeps `poolOffset + length > poolSize` false forever and makes every later pooled
// allocation land on pool offset 0 (all pooled Buffers aliasing each other).
const MAX_LENGTH = Number.MAX_SAFE_INTEGER;

function validateSize(size, name) {
    name = name || 'size';
    if (typeof size !== 'number')
        throw invalidArgType(name, 'of type number', size);
    if (size !== size || size < 0 || size > MAX_LENGTH)
        throw outOfRange(name, `>= 0 && <= ${MAX_LENGTH}`, size);   // node 的尺寸校验用 &&
    return Math.floor(size);
}

function isAnyArrayBuffer(value) {
    return value instanceof ArrayBuffer
        || (typeof SharedArrayBuffer !== 'undefined' && value instanceof SharedArrayBuffer);
}

// ECMA ToLength：NaN/负数 → 0，小数截断（node 的 from(arrayBuffer, off, length) 对 length 就是这个语义）
function toLength(value) {
    let len = Number(value);
    if (len !== len || len <= 0)
        return 0;
    return Math.floor(Math.min(len, Number.MAX_SAFE_INTEGER));
}

// node: hex 解码遇到第一个非法字符即停止（fibjs 的 hex.decode 是宽松跳过）。
// Buffer.from(str,'hex') 与 buf.write(str,...,'hex') 都在 native 侧用
// hexDecodeTo(kStop) 直接完成，不需要在 JS 层先规整字符串。

// node 的 *Slice 家族是半公开的内部 API：receiver 必须是 Uint8Array（Buffer 是它的子类），
// start/end 必须是区间内的整数。旧实现直接 `this.toString(enc, start, end)`，当 receiver 不是
// fibjs Buffer 时会落到 %TypedArray%.prototype.toString（= join 整段视图且忽略 start/end）——
// 大视图会抛 RangeError: Invalid string length。见 plans/buffer-node-compat-audit.md B7。
function sliceView(target, start, end) {
    if (!(target instanceof Uint8Array))
        throw invalidArgType('this', 'an instance of Uint8Array', target);

    const len = target.byteLength;

    if (start === undefined)
        start = 0;
    else if (typeof start !== 'number')
        throw invalidArgType('start', 'of type number', start);

    if (end === undefined)
        end = len;
    else if (typeof end !== 'number')
        throw invalidArgType('end', 'of type number', end);

    if (start % 1 !== 0 || end % 1 !== 0 || start < 0 || end < 0 || start > len || end > len) {
        const bad = (start % 1 !== 0 || start < 0 || start > len) ? start : end;
        throw outOfRange('start', `>= 0 and <= ${len}`, bad);
    }

    if (start === 0 && end === len)
        return target;

    return new Uint8Array(target.buffer, target.byteOffset + start, end - start);
}

// node: indexOf/lastIndexOf/includes 的 value/offset/encoding 归一化，检索本身交给 native 引擎
// 注意：形参不能叫 encoding —— 会遮蔽模块级 `var encoding = require('encoding')`
function indexOfImpl(buf, value, byteOffset, codec, reverse) {
    let offset = byteOffset;
    if (typeof offset === 'string') {        // indexOf(value, encoding)
        codec = offset;
        offset = undefined;
    }

    if (codec !== undefined && codec !== null && typeof codec !== 'string')
        throw invalidArgType('encoding', 'of type string', codec);

    let needle = value;
    if (typeof needle === 'string') {
        if (!Buffer.isEncoding(codec || 'utf8'))
            throw unknownEncoding(codec);
        needle = encoding.decode(needle, codec || 'utf8');
    } else if (typeof needle === 'number') {
        needle &= 255;                       // node: ToInt32 后取低 8 位（256→0、-1→255、NaN→0）
    } else if (!(needle instanceof Uint8Array)) {
        throw invalidArgType('value', 'of type string, number, or Uint8Array', needle);
    }

    if (offset !== undefined && typeof offset !== 'number')
        throw invalidArgType('byteOffset', 'of type number', offset);

    const len = buf.length;
    const empty = (needle instanceof Uint8Array) && needle.length === 0;

    let o = offset === undefined ? undefined : Math.trunc(offset);
    if (o !== undefined && o < 0) {
        if (empty)
            o = Math.max(0, len + o);        // 空 needle：负值钳制到 0
        else if (reverse) {
            o = len + o;                     // node: lastIndexOf 负数超界 → 直接 -1
            if (o < 0)
                return -1;
        } else
            o = Math.max(0, len + o);
    }

    // 空 needle：命中「钳制后的 offset」
    if (empty)
        return o === undefined ? (reverse ? len : 0) : Math.min(o, len);

    if (o === undefined)
        return reverse
            ? Buffer.native_lastIndexOf.call(buf, needle, -1)
            : Buffer.native_indexOf.call(buf, needle, 0);

    if (reverse) {
        if (o >= len)
            o = len - 1;
        if (o < 0)
            return -1;
        return Buffer.native_lastIndexOf.call(buf, needle, o);
    }

    if (o >= len)
        return -1;
    return Buffer.native_indexOf.call(buf, needle, o);
}

// node: compare 的区间参数（targetStart > targetEnd 允许，得到空切片）
function subRange(buf, start, end, startName, endName) {
    if (typeof start !== 'number' && start !== undefined)
        throw invalidArgType(startName, 'of type number', start);
    if (typeof end !== 'number' && end !== undefined)
        throw invalidArgType(endName, 'of type number', end);

    let s = start === undefined ? 0 : Math.floor(start);
    let e = end === undefined ? buf.length : Math.floor(end);

    if (s < 0)
        throw outOfRange(startName, `>= 0 and <= ${buf.length}`, start);
    if (e < 0 || e > buf.length)
        throw outOfRange(endName, `>= 0 and <= ${buf.length}`, end);

    return buf.subarray(s, e);
}

function swapBytes(buf, width) {
    if (buf.byteLength % width !== 0)
        throw invalidBufferSize(`Buffer size must be a multiple of ${width * 8}-bits`);

    for (let i = 0; i < buf.byteLength; i += width)
        for (let j = 0, k = width - 1; j < k; j++, k--) {
            const t = buf[i + j];
            buf[i + j] = buf[i + k];
            buf[i + k] = t;
        }

    return buf;
}

// node: isUtf8（严格校验：拒绝 overlong / 代理区 / 超出 U+10FFFF / 截断序列）
function isValidUtf8(buf) {
    for (let i = 0; i < buf.byteLength;) {
        const b = buf[i];
        let n, cp;

        if (b < 0x80) { i++; continue; }
        else if ((b & 0xe0) === 0xc0) { n = 1; cp = b & 0x1f; if (cp < 2) return false; }
        else if ((b & 0xf0) === 0xe0) { n = 2; cp = b & 0x0f; }
        else if ((b & 0xf8) === 0xf0) { n = 3; cp = b & 0x07; }
        else return false;

        if (i + n >= buf.byteLength)
            return false;

        for (let j = 1; j <= n; j++) {
            const c = buf[i + j];
            if ((c & 0xc0) !== 0x80) return false;
            cp = (cp << 6) | (c & 0x3f);
        }

        if (cp > 0x10ffff) return false;
        if (cp >= 0xd800 && cp <= 0xdfff) return false;
        if (n === 1 && cp < 0x80) return false;
        if (n === 2 && cp < 0x800) return false;
        if (n === 3 && cp < 0x10000) return false;

        i += n + 1;
    }
    return true;
}

const TRANSCODE_ENCODINGS = ['ascii', 'utf8', 'utf-8', 'utf16le', 'utf-16le', 'ucs2', 'ucs-2', 'latin1', 'binary'];

function normalizeTranscodeEncoding(name) {
    if (name === 'utf-8') return 'utf8';
    if (name === 'utf-16le' || name === 'ucs2' || name === 'ucs-2') return 'utf16le';
    if (name === 'binary') return 'latin1';
    return name;
}

function createPool() {
    allocPool = new ArrayBuffer(currentPoolSize());
    poolOffset = 0;
}
createPool();
function getPool(length) {
    // defensive: a bad length must never poison the cursor (see validateSize)
    if (!(length >= 0) || length % 1 !== 0 || length > MAX_LENGTH)
        return -1;

    const pool_size = currentPoolSize();

    // node 的零长度分配一律不走池（.buffer.byteLength 为 0）
    if (length === 0 || length >= pool_size >>> 1)
        return -1;

    // Buffer.poolSize 是可写的。改小之后 allocPool 仍是旧的大缓冲（还能用），
    // 改大之后旧缓冲装不下新的 poolSize —— 但**不必立刻重建**：
    // 只要当前池的剩余空间还够这一次分配就继续用，等它自然耗尽时
    // createPool() 会按新的 poolSize 建。node 也是这样（它的判断是
    // poolOffset + length > poolSize，失败后才 createPool）。
    //
    // 唯一的硬约束：offset + length 绝不能超出 allocPool 的实际长度，
    // 否则构造 TypedArray 会抛 "Invalid typed array length"。
    if (allocPool === undefined || poolOffset + length > allocPool.byteLength)
        createPool();

    if (poolOffset + length > allocPool.byteLength)
        return -1;          // 连新池都装不下（极端 poolSize），退回独立分配

    const offset = poolOffset;
    poolOffset += length;

    return offset;
}

const float32Array = new Float32Array(1);
const uInt8Float32Array = new Uint8Array(float32Array.buffer);
const float64Array = new Float64Array(1);
const uInt8Float64Array = new Uint8Array(float64Array.buffer);

function validateArray(value, name, minLength = 0) {
    if (!Array.isArray(value))
        throw invalidArgType(name, 'an instance of Array', value);

    if (value.length < minLength) {
        throw invalidArgType(name, 'an instance of Array', value);
    }
}

// node: lib/internal/validators.js 的等价实现（错误类/消息逐字对齐）
function validateNumber(value, name, min = undefined, max) {
    if (typeof value !== 'number')
        throw invalidArgType(name, 'of type number', value);

    if ((min != null && value < min) || (max != null && value > max) ||
        ((min != null || max != null) && Number.isNaN(value))) {
        throw outOfRange(name, `${min != null ? `>= ${min}` : ''}${min != null && max != null ? ' and ' : ''}${max != null ? `<= ${max}` : ''}`, value);
    }
}

function boundsError(value, length, type) {
    if (Math.floor(value) !== value) {
        validateNumber(value, type);
        throw outOfRange(type || 'offset', 'an integer', value);
    }

    if (length < 0)
        throw bufferOutOfBounds();

    throw outOfRange(type || 'offset', `>= 0 and <= ${length}`, value);
}

function checkBounds(buf, offset, byteLength) {
    validateNumber(offset, 'offset');
    if (buf[offset] === undefined || buf[offset + byteLength] === undefined)
        boundsError(offset, buf.length - (byteLength + 1));
}

function checkInt(value, min, max, buf, offset, byteLength) {
    if (value > max || value < min) {
        const n = typeof min === 'bigint' ? 'n' : '';
        let range;
        if (byteLength > 3) {
            if (min === 0 || min === 0n) {
                range = `>= 0${n} and < 2${n} ** ${(byteLength + 1) * 8}${n}`;
            } else {
                range = `>= -(2${n} ** ${(byteLength + 1) * 8 - 1}${n}) and ` +
                    `< 2${n} ** ${(byteLength + 1) * 8 - 1}${n}`;
            }
        } else {
            range = `>= ${min}${n} and <= ${max}${n}`;
        }
        throw outOfRange('value', range, value);
    }
    checkBounds(buf, offset, byteLength);
}

function writeBigU_Int64LE(buf, value, offset, min, max) {
    // node: 非 bigint 值直接抛（V8 的 BigInt 混算错误，无 code）
    if (typeof value !== 'bigint')
        throw new TypeError('Cannot mix BigInt and other types, use explicit conversions');

    checkInt(value, min, max, buf, offset, 7);

    let lo = Number(value & 0xffffffffn);
    buf[offset++] = lo;
    lo = lo >> 8;
    buf[offset++] = lo;
    lo = lo >> 8;
    buf[offset++] = lo;
    lo = lo >> 8;
    buf[offset++] = lo;
    let hi = Number(value >> 32n & 0xffffffffn);
    buf[offset++] = hi;
    hi = hi >> 8;
    buf[offset++] = hi;
    hi = hi >> 8;
    buf[offset++] = hi;
    hi = hi >> 8;
    buf[offset++] = hi;
    return offset;
}

function writeBigU_Int64BE(buf, value, offset, min, max) {
    // node: 非 bigint 值直接抛（V8 的 BigInt 混算错误，无 code）
    if (typeof value !== 'bigint')
        throw new TypeError('Cannot mix BigInt and other types, use explicit conversions');

    checkInt(value, min, max, buf, offset, 7);

    let lo = Number(value & 0xffffffffn);
    buf[offset + 7] = lo;
    lo = lo >> 8;
    buf[offset + 6] = lo;
    lo = lo >> 8;
    buf[offset + 5] = lo;
    lo = lo >> 8;
    buf[offset + 4] = lo;
    let hi = Number(value >> 32n & 0xffffffffn);
    buf[offset + 3] = hi;
    hi = hi >> 8;
    buf[offset + 2] = hi;
    hi = hi >> 8;
    buf[offset + 1] = hi;
    hi = hi >> 8;
    buf[offset] = hi;
    return offset + 8;
}

function writeU_Int48LE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 5);

    const newVal = Math.floor(value * 2 ** -32);
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    buf[offset++] = newVal;
    buf[offset++] = (newVal >>> 8);
    return offset;
}

function writeU_Int40LE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 4);

    const newVal = value;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    buf[offset++] = Math.floor(newVal * 2 ** -32);
    return offset;
}

function writeU_Int32LE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 3);

    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    return offset;
}

function writeU_Int24LE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 2);

    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    value = value >>> 8;
    buf[offset++] = value;
    return offset;
}

function writeU_Int16LE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 1);

    buf[offset++] = value;
    buf[offset++] = (value >>> 8);
    return offset;
}

function writeU_Int8(buf, value, offset, min, max) {
    value = +value;
    // `checkInt()` can not be used here because it checks two entries.
    validateNumber(offset, 'offset');
    if (value > max || value < min) {
        throw outOfRange('value', `>= ${min} and <= ${max}`, value);
    }
    if (buf[offset] === undefined)
        boundsError(offset, buf.length - 1);

    buf[offset] = value;
    return offset + 1;
}

function writeU_Int48BE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 5);

    const newVal = Math.floor(value * 2 ** -32);
    buf[offset++] = (newVal >>> 8);
    buf[offset++] = newVal;
    buf[offset + 3] = value;
    value = value >>> 8;
    buf[offset + 2] = value;
    value = value >>> 8;
    buf[offset + 1] = value;
    value = value >>> 8;
    buf[offset] = value;
    return offset + 4;
}

function writeU_Int40BE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 4);

    buf[offset++] = Math.floor(value * 2 ** -32);
    buf[offset + 3] = value;
    value = value >>> 8;
    buf[offset + 2] = value;
    value = value >>> 8;
    buf[offset + 1] = value;
    value = value >>> 8;
    buf[offset] = value;
    return offset + 4;
}

function writeU_Int32BE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 3);

    buf[offset + 3] = value;
    value = value >>> 8;
    buf[offset + 2] = value;
    value = value >>> 8;
    buf[offset + 1] = value;
    value = value >>> 8;
    buf[offset] = value;
    return offset + 4;
}

function writeU_Int24BE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 2);

    buf[offset + 2] = value;
    value = value >>> 8;
    buf[offset + 1] = value;
    value = value >>> 8;
    buf[offset] = value;
    return offset + 3;
}

function writeU_Int16BE(buf, value, offset, min, max) {
    value = +value;
    checkInt(value, min, max, buf, offset, 1);

    buf[offset++] = (value >>> 8);
    buf[offset++] = value;
    return offset;
}

class Buffer extends Uint8Array {
    constructor(bufferOrLength, byte_offset, byte_length) {
        if (typeof bufferOrLength === 'number') {
            const size = validateSize(bufferOrLength);
            const offset = byte_offset === NO_POOL ? -1 : getPool(size);
            if (offset === -1)
                super(size);
            else
                super(allocPool, offset, size);
        }
        else if (bufferOrLength instanceof Uint8Array || Array.isArray(bufferOrLength))
            super(bufferOrLength);
        else if(bufferOrLength instanceof DataView)
            super(bufferOrLength.buffer, bufferOrLength.byteOffset, bufferOrLength.byteLength);
        else {
            if (bufferOrLength instanceof Date)
                bufferOrLength = bufferOrLength.toString();

            if (typeof bufferOrLength === 'string') {
                // node 的编码参数规则：
                //   1) 只有**字符串**才被当作编码名 —— null / 123 / true / Symbol /
                //      {toString(){}} 一律按默认的 utf8 处理，不抛错、也不调用
                //      toString()（node: Buffer.from('ab', 123) === Buffer.from('ab')）
                //   2) 字符串的编码名大小写不敏感（'HEX' 与 'hex' 同义），但 native
                //      侧大小写敏感，所以这里统一小写化后再分发
                //   3) 不认识的字符串编码保留原样交回 native，由它决定（fibjs 扩展
                //      的 base32/base58/字符集编码依赖这一点）
                var codec = typeof byte_offset === 'string'
                    ? normalizeFromCodec(byte_offset)
                    : undefined;

                // node: latin1/binary/ascii 的 str→bytes 是"每个 code unit 取低 8 位"，
                // ucs2/utf16le 是"每个 code unit 两字节"。两者都能在 JS 层一次写完，
                // 不必经过 native 的 UTF-16 中转：
                //   from(latin1) 高位 1177 µs → 104 µs，from(ucs2) 339 µs → 67 µs
                // 同时修正 write 方向的 length 语义（node 按字节计，不写半个 code unit）。
                if (codec === 'latin1' || codec === 'binary' || codec === 'ascii') {
                    var len = bufferOrLength.length;
                    let offset = getPool(len);
                    if (offset === -1)
                        super(len);
                    else
                        super(allocPool, offset, len);
                    // 与 write() 同理：native 的 ascii 分支就是 WriteOneByte，
                    // 与 latin1 逐字节等价，1.6 µs vs JS 循环 103 µs
                    Buffer.native_write.call(this, bufferOrLength, 0, len, 'ascii');
                } else if (codec === 'ucs2' || codec === 'ucs-2'
                    || codec === 'utf16le' || codec === 'utf-16le') {
                    // ucs2 的 str→bytes 由 native_write 直连 V8 的 UTF-16 拷贝完成
                    // （64 KB 91 µs → ~5 µs）。这里只负责分配。
                    var len16 = bufferOrLength.length * 2;
                    if (len16 > 0x7fffffff)
                        len16 = 0x7fffffff;
                    let offset = getPool(len16);
                    if (offset === -1)
                        super(len16);
                    else
                        super(allocPool, offset, len16);
                    this.write(bufferOrLength, 0, len16, 'ucs2');
                } else if (codec === 'utf16be' || codec === 'utf-16be'
                    || codec === 'ucs2be' || codec === 'ucs-2be') {
                    // 大端 UTF-16：与 utf16le 完全相同的语义（每个 code unit
                    // 原样输出，孤代理项**不替换**），只是字节序相反。
                    //
                    // 为什么不用 native 的 utf_convert_s：那条路径会对输入做
                    // validate_utf16，孤代理项被判非法后替换成 U+FFFD —— 于是
                    //   Buffer.from('a\\ud800b','utf16be')  ->  0061 fffd fffd fffd 0062
                    // 而 utf16le 是原样保留（node 的 ucs2 也是原样保留）。
                    // 两个名字指的是同一个编码，必须给出同样的 code unit 序列。
                    var n16 = bufferOrLength.length;
                    var lenBE = n16 * 2;
                    if (lenBE > 0x7fffffff)
                        lenBE = 0x7fffffff;
                    let offBE = getPool(lenBE);
                    if (offBE === -1)
                        super(lenBE);
                    else
                        super(allocPool, offBE, lenBE);
                    for (var ib = 0; ib < n16; ib++) {
                        var cu = bufferOrLength.charCodeAt(ib);
                        this[ib * 2] = cu >>> 8;
                        this[ib * 2 + 1] = cu & 0xff;
                    }
                } else if (codec === 'utf32' || codec === 'utf-32'
                    || codec === 'utf32le' || codec === 'utf-32le'
                    || codec === 'utf32be' || codec === 'utf-32be'
                    || codec === 'ucs4' || codec === 'ucs-4'
                    || codec === 'ucs4le' || codec === 'ucs-4le'
                    || codec === 'ucs4be' || codec === 'ucs-4be') {
                    // UTF-32：每个**码点**扩成 4 字节（代理对合并成单个 astral
                    // 码点，孤代理项原样保留为它本身的值，与 utf16 家族一致）。
                    //
                    // 为什么不用 native 的 utf_convert：那条路径会把孤代理项判为
                    // 非法并展开成多个 U+FFFD，与 utf16le 的语义不一致。
                    var be4 = (codec === 'utf32be' || codec === 'utf-32be'
                        || codec === 'ucs4be' || codec === 'ucs-4be');
                    var n32 = bufferOrLength.length;
                    var len32 = Buffer.byteLength(bufferOrLength, codec);
                    if (len32 > 0x7fffffff)
                        len32 = 0x7fffffff;
                    let off32 = getPool(len32);
                    if (off32 === -1)
                        super(len32);
                    else
                        super(allocPool, off32, len32);
                    var o32 = 0;
                    for (var i32 = 0; i32 < n32; i32++) {
                        var lo32 = bufferOrLength.charCodeAt(i32);
                        var cp32 = lo32;
                        // 合法代理对 -> 合并成一个 astral 码点（占 1 个 4 字节槽）
                        if (lo32 >= 0xd800 && lo32 <= 0xdbff && i32 + 1 < n32) {
                            var nx = bufferOrLength.charCodeAt(i32 + 1);
                            if (nx >= 0xdc00 && nx <= 0xdfff) {
                                cp32 = ((lo32 - 0xd800) << 10) + (nx - 0xdc00) + 0x10000;
                                i32++;
                            }
                        }
                        if (be4) {
                            this[o32] = (cp32 >>> 24) & 0xff;
                            this[o32 + 1] = (cp32 >>> 16) & 0xff;
                            this[o32 + 2] = (cp32 >>> 8) & 0xff;
                            this[o32 + 3] = cp32 & 0xff;
                        } else {
                            this[o32] = cp32 & 0xff;
                            this[o32 + 1] = (cp32 >>> 8) & 0xff;
                            this[o32 + 2] = (cp32 >>> 16) & 0xff;
                            this[o32 + 3] = (cp32 >>> 24) & 0xff;
                        }
                        o32 += 4;
                    }
                } else if (codec === 'hex' || codec === 'base64' || codec === 'base64url') {
                    // native 的零绕行解码：String::ValueView 直接拿 V8 的串指针，
                    // simdutf 宽容模式一次解码，结果直接落在返回 Buffer 的
                    // backing store 里（不再经过 JS 临时数组或 exlib::string）。
                    //   from(hex)    220 µs -> 约 20 µs
                    //   from(base64)  82 µs -> 约 10 µs
                    //
                    // 注意：必须在 super() **之前** 完成，所以这里不能传 this。
                    const decoded = Buffer.native_decodeString(bufferOrLength, codec);
                    if (decoded !== undefined) {
                        super(decoded.buffer, decoded.byteOffset, decoded.byteLength);
                        return;
                    }
                    // 理论上到不了这里（只有 hex/base64 会进来）
                    super(encoding.decode(bufferOrLength, codec).buffer);
                } else if (codec === undefined || codec === 'utf8' || codec === 'utf-8') {
                    var byte_length = Buffer.byteLength(bufferOrLength, codec);

                    let offset = getPool(byte_length);
                    if (offset === -1)
                        super(byte_length);
                    else
                        super(allocPool, offset, byte_length);

                    this.write(bufferOrLength, codec);
                } else {
                    if (!Buffer.isEncoding(codec))
                        throw unknownEncoding(codec);
                    super(encoding.decode(bufferOrLength, codec).buffer);
                }
            }
            else if (isAnyArrayBuffer(bufferOrLength)) {
                // node: offset 原样交给 V8 的 TypedArray 构造（2.5→2、'1'→1、'x'/NaN→0、-1 抛 V8 原生 RangeError），
                // 但 length 先按 ToLength 规整（NaN/负数→0、2.5→2），再做 offset/length 越界检查
                const ab_len = bufferOrLength.byteLength;
                const off_num = Number(byte_offset === undefined ? 0 : byte_offset);

                if (off_num > ab_len)
                    throw bufferOutOfBounds('offset');

                if (byte_length === undefined) {
                    super(bufferOrLength, byte_offset);
                } else {
                    const len_num = toLength(byte_length);
                    if (len_num > ab_len - off_num)
                        throw bufferOutOfBounds('length');

                    super(bufferOrLength, byte_offset, len_num);
                }
            }
            else if (bufferOrLength === null || bufferOrLength === undefined
                || typeof bufferOrLength === 'function' || typeof bufferOrLength === 'boolean'
                || typeof bufferOrLength === 'symbol' || typeof bufferOrLength === 'bigint')
                throw makeInvalidArgType('The first argument must be of type string or an instance of Buffer, ArrayBuffer, or Array or an Array-like Object. Received '
                    + describeReceived(bufferOrLength));
            else
                // 对象：沿用 V8 的类数组转换（legacy `new Buffer(obj)` 一直如此；
                // 严格校验放在 Buffer.from 里，见下）
                super(bufferOrLength, byte_offset, byte_length);
        }
    }

    // node: 第一个参数必须是 string（其余类型抛 ERR_INVALID_ARG_TYPE）；实现在 native_write
    write(buf, offset, length, codec) {
        if (typeof buf !== 'string')
            throw makeInvalidArgType('argument must be a string');

        const len = this.length;

        // node 的参数分派（lib/buffer.js:1102）。**顺序很关键**：
        //
        //   1) offset === undefined  -> 整个 length / encoding 都被忽略，
        //      等价于 write(s)，写满 [0, length) 的 utf8。
        //      旧实现在这条分支里算 `length = this.length - offset`，而 offset
        //      此时还是 undefined —— NaN 一路传到校验，于是
        //        write('abc', undefined, 'hex')
        //      抛 RangeError（node 正常写 3 字节）。显式 undefined（很多调用方
        //      会传 `write(s, off, len, enc)` 而 off/len 来自可能缺省的变量）
        //      是最容易踩到的形态。
        //   2) length === undefined && typeof offset === 'string' -> 编码名
        //   3) 其余 -> 正常校验；length 缺省为剩余空间，编码名可以由 length
        //      位置给出
        if (offset === undefined) {
            offset = 0;
            length = len;
            codec = 'utf8';
        } else if (length === undefined && typeof offset === 'string') {
            codec = offset;
            offset = 0;
            length = len;
        } else {
            if (typeof offset !== 'number')
                throw invalidArgType('offset', 'of type number', offset);

            // node 的 offset / length 校验规则（lib/buffer.js 的 validateOffset）：
            //   offset  必须落在 [0, this.length]，越界抛 RangeError
            //   length  必须落在 [0, this.length]，**上限是整条缓冲的长度**，
            //           而不是"剩余空间"
            //   length > this.length - offset 时**静默钳制**到剩余空间，不抛错
            //     write('abc', 6, 8)  -> 写 2 字节（'ab'）
            //     write('abc', 1, 8)  -> 写 3 字节（'abc'）
            const remaining = len - offset;
            if (offset % 1 !== 0 || offset < 0 || offset > len)
                throw outOfRange('offset', `>= 0 and <= ${len}`, offset);

            if (length === undefined)
                length = remaining;
            else if (typeof length === 'string') {
                codec = length;
                length = remaining;
            } else {
                if (typeof length !== 'number')
                    throw invalidArgType('length', 'of type number', length);
                if (length % 1 !== 0 || length < 0 || length > len)
                    throw outOfRange('length', `>= 0 and <= ${len}`, length);
                if (length > remaining)
                    length = remaining;
            }
        }

        // node: 编码名先按 ToString 强制转换（`getEncodingOps` 的第一行就是
        // `encoding += ''`），而且**falsy 一律当 utf8**（`if (!encoding ||
        // encoding === 'utf8')`）：
        //   write('ab', 0, 2, null)              -> utf8
        //   write('ab', 0, 4, {toString(){return 'hex'}}) -> hex
        // 之前的实现只接受字符串，null / 对象会抛 "Unknown encoding"。
        // Symbol 在 `+ ''` 时抛 TypeError，与 node 一致。
        if (!codec)
            codec = 'utf8';
        else if (typeof codec !== 'string')
            codec = codec + '';

        if (!Buffer.isEncoding(codec))
            throw unknownEncoding(codec);

        // node: 编码名大小写不敏感，且 native_write 只认小写名。
        //
        // 之前只在 from/toString/byteLength 里做了 normalizeFromCodec，
        // write 直接把自己的参数原样转发给 native_write，于是 'HEX' / 'BASE64' /
        // 'UCS2' 会落到 commonDecode 的 ICU 兜底并抛
        //   Error: encoding: Unknown charset: 'HEX'.
        // 而 node 的 write('abcd',0,4,'HEX') 正常写入。这里补齐。
        codec = normalizeFromCodec(codec);

        const off = offset;

        // latin1/binary/ascii 的 str→bytes 是"每个 code unit 取低 8 位"。
        //
        // fibjs 的 native_write 里 ascii 分支已经是 WriteOneByte（Buffer.cpp:643-651），
        // 与 latin1 的语义**逐字节相同**（已对全部 65536 个 code unit 穷举验证，含
        // astral / 孤立代理项 / NUL / length 截断 / offset）。它 64 KB 只要 1.6 µs，
        // 而 JS 循环要 103 µs —— 因此这里直接转发给 native 的 ascii 分支。
        //
        // 注意：**解码方向不能这么做** —— native 的 ascii 解码会掩掉高位（0x80 → 0x00），
        // 而 latin1 要保留 U+0080..U+00FF，所以 toString 仍走 JS/utf8 分支。
        if (codec === 'latin1' || codec === 'binary' || codec === 'ascii') {
            let n = buf.length;
            if (length < n)
                n = length;
            if (n <= 0)
                return 0;
            return Buffer.native_write.call(this, buf, off, n, 'ascii');
        }

        // ucs2/utf16le 现在由 native_write 直连 V8 的 UTF-16 拷贝处理（is_big_endian 与
        // 未对齐都已覆盖），这里不再重复实现。
        //
        // 必须显式传规范化后的 offset/length —— 不能 `apply(this, arguments)`，
        // 否则上面刚补上的 `undefined -> 0 / 剩余长度` 归一会丢失。
        return Buffer.native_write.call(this, buf, off, length, codec);
    }

    fill(buf, offset, end, codec) {
        let this_byteLength = this.byteLength;

        if (typeof offset === 'string') {
            codec = offset;
            offset = 0;
            end = this_byteLength;
        } else if (typeof end === 'string') {
            codec = end;
            end = this_byteLength;
        }

        // node: offset/end 必须是 [0, length] 内的整数；offset 超界直接 no-op（连 end 都不再校验），
        // 负数/非整数抛 RangeError，end 超界抛 RangeError
        if (offset === undefined)
            offset = 0;
        else {
            if (typeof offset !== 'number')
                throw invalidArgType('offset', 'of type number', offset);
            if (offset % 1 !== 0 || offset < 0)
                throw outOfRange('offset', `>= 0 and <= ${this_byteLength}`, offset);
            if (offset > this_byteLength)
                return this;
        }

        if (end === undefined)
            end = this_byteLength;
        else {
            if (typeof end !== 'number')
                throw invalidArgType('end', 'of type number', end);
            if (end % 1 !== 0 || end < 0 || end > this_byteLength)
                throw outOfRange('end', `>= 0 and <= ${this_byteLength}`, end);
        }

        if (end <= offset)
            return this;

        if (typeof buf === 'number') {
            TypedArrayPrototypeFill(this, buf, offset, end);
            return this;
        }

        let buf_byteLength;
        if (typeof buf === 'string') {
            // node: fill 的编码名处理（lib/buffer.js 的 _fill）：
            //   null / undefined / ''  -> utf8        （`normalizeEncoding` 的
            //                                          `enc == null` 分支 + `!encoding`）
            //   非字符串（数字 / 布尔 / 普通对象）-> TypeError（`validateString`）
            //   其它字符串 -> 大小写不敏感；未知编码抛 ERR_UNKNOWN_ENCODING
            // fibjs 之前只接受字符串：write/from/byteLength 都接受 null 而 fill 抛
            // "Unknown encoding: null"，同一个编码参数在不同 API 上不一致。
            if (codec == null || codec === '')
                codec = 'utf8';
            else if (typeof codec !== 'string')
                throw invalidArgType('encoding', 'of type string', codec);
            else
                codec = normalizeFromCodec(codec);
            if (!Buffer.isEncoding(codec))
                throw unknownEncoding(codec);

            // **先把字符串编码成字节，再按字节循环重复** —— 这正是 node 的 fill
            // 做法（lib/buffer.js 里 `binding.fill` 用的是已编码的字节串）。
            //
            // 不能用 byteLength() 当源长度：它按 node 语义只回答"这个字符串按该
            // 编码最**多**占多少字节"，**不校验内容**：
            //   Buffer.byteLength('abZZ','hex')      === 2   （实际只解出 1 字节）
            //   Buffer.byteLength('QUJD$$','base64') === 3   （实际只解出 'ABC'）
            // 拿它当填充节奏会按"声称的长度"推进，而真正写入的字节更少，中间
            // 留下没被覆盖的洞：
            //   Buffer.alloc(4,0x2e).fill('abZZ','hex')  ->  ab2eab2e（node: abababab）
            //
            // 也不能把这件事交给 write()：write 的 length 以**字节**计，且对
            // utf16 家族会向下取整到整 code unit，而 fill 是纯字节级的循环重复：
            //   fill('\u00ff'.repeat(n), 'ucs2') 填 5 字节 -> ff00ff00ff
            //   （末字节是 ff，而不是碰巧保留的 0xff 之外的其它值）
            const decoded = Buffer.from(buf, codec);
            buf_byteLength = decoded.length;

            if (buf_byteLength === 0) {
                if (buf.length === 0) {
                    // node: 空字符串按 0 填充。
                    // 这里也是**死循环**的老位置：0 长度源 + while (offset < end)
                    // 时 offset 不自增
                    TypedArrayPrototypeFill(this, 0, offset, end);
                    return this;
                }
                // 非空源但一个字节都解不出来 -> 与 node 一致地报参数错误
                // （fill('a','hex') / fill('!!!!','base64')）
                throw invalidArgValue('value', buf);
            }

            // 后续统一走"字节数组重复填充"，不再区分来源是字符串还是 Buffer
            buf = decoded;
        } else if (ArrayBuffer.isView(buf)) {
            // node: **任何** ArrayBufferView 都按它的字节填充，不只是 Uint8Array。
            // 用 `buf instanceof Uint8Array` 判断会把 Uint16Array / Float32Array /
            // DataView 漏到下面的 Number() 分支 —— Number(view) 是 NaN，于是
            // 整段被填成 0（静默错误）：
            //   fill(new Uint16Array([0x6162]))  node: 626162616261  fibjs: 000000
            //   fill(new DataView(...))          node: 010201020102  fibjs: 000000
            // 必须同时尊重 byteOffset / byteLength（子视图只填它自己的区间）。
            buf_byteLength = buf.byteLength;
            if (buf_byteLength === 0)
                throw invalidArgValue('value', buf);
            if (!(buf instanceof Uint8Array))
                buf = new Uint8Array(buf.buffer, buf.byteOffset, buf.byteLength);
        } else {
            // node: 其余类型走 Number() 强制转换（null→0、true→1、[1,2]→NaN→0）
            TypedArrayPrototypeFill(this, Number(buf), offset, end);
            return this;
        }

        if (buf_byteLength >= end - offset) {
            if (buf_byteLength > end - offset)
                buf = new Uint8Array(buf.buffer, buf.byteOffset, end - offset);
            TypedArrayPrototypeSet(this, buf, offset);
        } else {
            TypedArrayPrototypeSet(this, buf, offset);
            let fill_offset = offset;
            offset += buf_byteLength;

            while (offset < end) {
                if (buf_byteLength > end - offset)
                    buf_byteLength = end - offset;
                if (buf_byteLength <= 0)
                    break;

                TypedArrayPrototypeCopyWithin(this, offset, fill_offset, fill_offset + buf_byteLength);
                offset += buf_byteLength;

                buf_byteLength *= 2;
            }
        }

        return this;
    }

    readBigUInt64LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 7];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 8);

        const lo = first +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 24;

        const hi = this[++offset] +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            last * 2 ** 24;

        return BigInt(lo) + (BigInt(hi) << 32n);
    }

    readBigUInt64BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 7];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 8);

        const hi = first * 2 ** 24 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            this[++offset];

        const lo = this[++offset] * 2 ** 24 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            last;

        return (BigInt(hi) << 32n) + BigInt(lo);
    }

    readBigInt64LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 7];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 8);

        const val = this[offset + 4] +
            this[offset + 5] * 2 ** 8 +
            this[offset + 6] * 2 ** 16 +
            (last << 24); // Overflow
        return (BigInt(val) << 32n) +
            BigInt(first +
                this[++offset] * 2 ** 8 +
                this[++offset] * 2 ** 16 +
                this[++offset] * 2 ** 24);
    }

    readBigInt64BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 7];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 8);

        const val = (first << 24) + // Overflow
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            this[++offset];
        return (BigInt(val) << 32n) +
            BigInt(this[++offset] * 2 ** 24 +
                this[++offset] * 2 ** 16 +
                this[++offset] * 2 ** 8 +
                last);
    }

    readUIntLE(offset, byteLength) {
        if (offset === undefined)
            throw invalidArgType('offset', 'of type number', offset);
        if (byteLength === 6)
            return this.readUInt48LE(offset);
        if (byteLength === 5)
            return this.readUInt40LE(offset);
        if (byteLength === 4)
            return this.readUInt32LE(offset);
        if (byteLength === 3)
            return this.readUInt24LE(offset);
        if (byteLength === 2)
            return this.readUInt16LE(offset);
        if (byteLength === 1)
            return this.readUInt8(offset);

        boundsError(byteLength, 6, 'byteLength');
    }

    readUInt48LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 5];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 6);

        return first +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 24 +
            (this[++offset] + last * 2 ** 8) * 2 ** 32;
    }

    readUInt40LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 4];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 5);

        return first +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 24 +
            last * 2 ** 32;
    }

    readUInt32LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 3];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 4);

        return first +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            last * 2 ** 24;
    }

    readUInt24LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 2];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 3);

        return first + this[++offset] * 2 ** 8 + last * 2 ** 16;
    }

    readUInt16LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 1];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 2);

        return first + last * 2 ** 8;
    }

    readUInt8(offset = 0) {
        validateNumber(offset, 'offset');
        const val = this[offset];
        if (val === undefined)
            boundsError(offset, this.length - 1);

        return val;
    }

    readUIntBE(offset, byteLength) {
        if (offset === undefined)
            throw invalidArgType('offset', 'of type number', offset);
        if (byteLength === 6)
            return this.readUInt48BE(offset);
        if (byteLength === 5)
            return this.readUInt40BE(offset);
        if (byteLength === 4)
            return this.readUInt32BE(offset);
        if (byteLength === 3)
            return this.readUInt24BE(offset);
        if (byteLength === 2)
            return this.readUInt16BE(offset);
        if (byteLength === 1)
            return this.readUInt8(offset);

        boundsError(byteLength, 6, 'byteLength');
    }

    readUInt48BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 5];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 6);

        return (first * 2 ** 8 + this[++offset]) * 2 ** 32 +
            this[++offset] * 2 ** 24 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            last;
    }

    readUInt40BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 4];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 5);

        return first * 2 ** 32 +
            this[++offset] * 2 ** 24 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            last;
    }

    readUInt32BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 3];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 4);

        return first * 2 ** 24 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            last;
    }

    readUInt24BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 2];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 3);

        return first * 2 ** 16 + this[++offset] * 2 ** 8 + last;
    }

    readUInt16BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 1];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 2);

        return first * 2 ** 8 + last;
    }

    readIntLE(offset, byteLength) {
        if (offset === undefined)
            throw invalidArgType('offset', 'of type number', offset);
        if (byteLength === 6)
            return this.readInt48LE(offset);
        if (byteLength === 5)
            return this.readInt40LE(offset);
        if (byteLength === 4)
            return this.readInt32LE(offset);
        if (byteLength === 3)
            return this.readInt24LE(offset);
        if (byteLength === 2)
            return this.readInt16LE(offset);
        if (byteLength === 1)
            return this.readInt8(offset);

        boundsError(byteLength, 6, 'byteLength');
    }

    readInt48LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 5];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 6);

        const val = this[offset + 4] + last * 2 ** 8;
        return (val | (val & 2 ** 15) * 0x1fffe) * 2 ** 32 +
            first +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 24;
    }

    readInt40LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 4];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 5);

        return (last | (last & 2 ** 7) * 0x1fffffe) * 2 ** 32 +
            first +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 24;
    }

    readInt32LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 3];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 4);

        return first +
            this[++offset] * 2 ** 8 +
            this[++offset] * 2 ** 16 +
            (last << 24); // Overflow
    }

    readInt24LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 2];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 3);

        const val = first + this[++offset] * 2 ** 8 + last * 2 ** 16;
        return val | (val & 2 ** 23) * 0x1fe;
    }

    readInt16LE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 1];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 2);

        const val = first + last * 2 ** 8;
        return val | (val & 2 ** 15) * 0x1fffe;
    }

    readInt8(offset = 0) {
        validateNumber(offset, 'offset');
        const val = this[offset];
        if (val === undefined)
            boundsError(offset, this.length - 1);

        return val | (val & 2 ** 7) * 0x1fffffe;
    }

    readIntBE(offset, byteLength) {
        if (offset === undefined)
            throw invalidArgType('offset', 'of type number', offset);
        if (byteLength === 6)
            return this.readInt48BE(offset);
        if (byteLength === 5)
            return this.readInt40BE(offset);
        if (byteLength === 4)
            return this.readInt32BE(offset);
        if (byteLength === 3)
            return this.readInt24BE(offset);
        if (byteLength === 2)
            return this.readInt16BE(offset);
        if (byteLength === 1)
            return this.readInt8(offset);

        boundsError(byteLength, 6, 'byteLength');
    }

    readInt48BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 5];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 6);

        const val = this[++offset] + first * 2 ** 8;
        return (val | (val & 2 ** 15) * 0x1fffe) * 2 ** 32 +
            this[++offset] * 2 ** 24 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            last;
    }

    readInt40BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 4];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 5);

        return (first | (first & 2 ** 7) * 0x1fffffe) * 2 ** 32 +
            this[++offset] * 2 ** 24 +
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            last;
    }

    readInt32BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 3];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 4);

        return (first << 24) + // Overflow
            this[++offset] * 2 ** 16 +
            this[++offset] * 2 ** 8 +
            last;
    }

    readInt24BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 2];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 3);

        const val = first * 2 ** 16 + this[++offset] * 2 ** 8 + last;
        return val | (val & 2 ** 23) * 0x1fe;
    }

    readInt16BE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 1];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 2);

        const val = first * 2 ** 8 + last;
        return val | (val & 2 ** 15) * 0x1fffe;
    }

    readFloatLE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 3];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 4);

        uInt8Float32Array[0] = first;
        uInt8Float32Array[1] = this[++offset];
        uInt8Float32Array[2] = this[++offset];
        uInt8Float32Array[3] = last;
        return float32Array[0];
    }

    readFloatBE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 3];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 4);

        uInt8Float32Array[3] = first;
        uInt8Float32Array[2] = this[++offset];
        uInt8Float32Array[1] = this[++offset];
        uInt8Float32Array[0] = last;
        return float32Array[0];
    }

    readDoubleBE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 7];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 8);

        uInt8Float64Array[7] = first;
        uInt8Float64Array[6] = this[++offset];
        uInt8Float64Array[5] = this[++offset];
        uInt8Float64Array[4] = this[++offset];
        uInt8Float64Array[3] = this[++offset];
        uInt8Float64Array[2] = this[++offset];
        uInt8Float64Array[1] = this[++offset];
        uInt8Float64Array[0] = last;
        return float64Array[0];
    }

    readDoubleLE(offset = 0) {
        validateNumber(offset, 'offset');
        const first = this[offset];
        const last = this[offset + 7];
        if (first === undefined || last === undefined)
            boundsError(offset, this.length - 8);

        uInt8Float64Array[0] = first;
        uInt8Float64Array[1] = this[++offset];
        uInt8Float64Array[2] = this[++offset];
        uInt8Float64Array[3] = this[++offset];
        uInt8Float64Array[4] = this[++offset];
        uInt8Float64Array[5] = this[++offset];
        uInt8Float64Array[6] = this[++offset];
        uInt8Float64Array[7] = last;
        return float64Array[0];
    }

    writeBigUInt64LE(value, offset = 0) {
        return writeBigU_Int64LE(this, value, offset, 0n, 0xffffffffffffffffn);
    }

    writeBigUInt64BE(value, offset = 0) {
        return writeBigU_Int64BE(this, value, offset, 0n, 0xffffffffffffffffn);
    }

    writeBigInt64LE(value, offset = 0) {
        return writeBigU_Int64LE(
            this, value, offset, -0x8000000000000000n, 0x7fffffffffffffffn);
    }

    writeBigInt64BE(value, offset = 0) {
        return writeBigU_Int64BE(
            this, value, offset, -0x8000000000000000n, 0x7fffffffffffffffn);
    }

    writeUIntLE(value, offset, byteLength) {
        if (byteLength === 6)
            return this.writeUInt48LE(value, offset);
        if (byteLength === 5)
            return this.writeUInt40LE(value, offset);
        if (byteLength === 4)
            return this.writeUInt32LE(value, offset);
        if (byteLength === 3)
            return this.writeUInt24LE(value, offset);
        if (byteLength === 2)
            return writeUInt16LE(this, value, offset);
        if (byteLength === 1)
            return writeUInt8(this, value, offset);

        boundsError(byteLength, 6, 'byteLength');
    }

    writeUInt48LE(value, offset = 0) {
        return writeU_Int48LE(this, value, offset, 0, 0xffffffffffff);
    }

    writeUInt40LE(value, offset = 0) {
        return writeU_Int40LE(this, value, offset, 0, 0xffffffffff);
    }

    writeUInt32LE(value, offset = 0) {
        return writeU_Int32LE(this, value, offset, 0, 0xffffffff);
    }

    writeUInt24LE(value, offset = 0) {
        return writeU_Int24LE(this, value, offset, 0, 0xffffff);
    }

    writeUInt16LE(value, offset = 0) {
        return writeU_Int16LE(this, value, offset, 0, 0xffff);
    }

    writeUInt8(value, offset = 0) {
        return writeU_Int8(this, value, offset, 0, 0xff);
    }

    writeUIntBE(value, offset, byteLength) {
        if (byteLength === 6)
            return this.writeUInt48BE(value, offset);
        if (byteLength === 5)
            return this.writeUInt40BE(value, offset);
        if (byteLength === 4)
            return this.writeUInt32BE(value, offset);
        if (byteLength === 3)
            return this.writeUInt24BE(value, offset);
        if (byteLength === 2)
            return this.writeUInt16BE(value, offset);
        if (byteLength === 1)
            return this.writeUInt8(value, offset);

        boundsError(byteLength, 6, 'byteLength');
    }

    writeUInt48BE(value, offset = 0) {
        return writeU_Int48BE(this, value, offset, 0, 0xffffffffffff);
    }

    writeUInt40BE(value, offset = 0) {
        return writeU_Int40BE(this, value, offset, 0, 0xffffffffff);
    }

    writeUInt32BE(value, offset = 0) {
        return writeU_Int32BE(this, value, offset, 0, 0xffffffff);
    }

    writeUInt24BE(value, offset = 0) {
        return writeU_Int24BE(this, value, offset, 0, 0xffffff);
    }

    writeUInt16BE(value, offset = 0) {
        return writeU_Int16BE(this, value, offset, 0, 0xffff);
    }

    writeIntLE(value, offset, byteLength) {
        if (byteLength === 6)
            return this.writeInt48LE(value, offset);
        if (byteLength === 5)
            return this.writeInt40LE(value, offset);
        if (byteLength === 4)
            return this.writeInt32LE(value, offset);
        if (byteLength === 3)
            return this.writeInt24LE(value, offset);
        if (byteLength === 2)
            return this.writeInt16LE(value, offset);
        if (byteLength === 1)
            return this.writeInt8(value, offset);

        boundsError(byteLength, 6, 'byteLength');
    }

    writeInt48LE(value, offset = 0) {
        return writeU_Int48LE(this, value, offset, -0x800000000000, 0x7fffffffffff);
    }

    writeInt40LE(value, offset = 0) {
        return writeU_Int40LE(this, value, offset, -0x8000000000, 0x7fffffffff);
    }

    writeInt32LE(value, offset = 0) {
        return writeU_Int32LE(this, value, offset, -0x80000000, 0x7fffffff);
    }

    writeInt24LE(value, offset = 0) {
        return writeU_Int24LE(this, value, offset, -0x800000, 0x7fffff);
    }

    writeInt16LE(value, offset = 0) {
        return writeU_Int16LE(this, value, offset, -0x8000, 0x7fff);
    }

    writeInt8(value, offset = 0) {
        return writeU_Int8(this, value, offset, -0x80, 0x7f);
    }

    writeIntBE(value, offset, byteLength) {
        if (byteLength === 6)
            return writeU_Int48BE(this, value, offset, -0x800000000000, 0x7fffffffffff);
        if (byteLength === 5)
            return writeU_Int40BE(this, value, offset, -0x8000000000, 0x7fffffffff);
        if (byteLength === 4)
            return writeU_Int32BE(this, value, offset, -0x80000000, 0x7fffffff);
        if (byteLength === 3)
            return writeU_Int24BE(this, value, offset, -0x800000, 0x7fffff);
        if (byteLength === 2)
            return writeU_Int16BE(this, value, offset, -0x8000, 0x7fff);
        if (byteLength === 1)
            return writeU_Int8(this, value, offset, -0x80, 0x7f);

        boundsError(byteLength, 6, 'byteLength');
    }


    writeInt48BE(value, offset = 0) {
        return writeU_Int48BE(this, value, offset, -0x800000000000, 0x7fffffffffff);
    }

    writeInt40BE(value, offset = 0) {
        return writeU_Int40BE(this, value, offset, -0x8000000000, 0x7fffffffff);
    }

    writeInt32BE(value, offset = 0) {
        return writeU_Int32BE(this, value, offset, -0x80000000, 0x7fffffff);
    }

    writeInt24BE(value, offset = 0) {
        return writeU_Int24BE(this, value, offset, -0x800000, 0x7fffff);
    }

    writeInt16BE(value, offset = 0) {
        return writeU_Int16BE(this, value, offset, -0x8000, 0x7fff);
    }

    writeFloatLE(val, offset = 0) {
        val = +val;
        checkBounds(this, offset, 3);

        float32Array[0] = val;
        this[offset++] = uInt8Float32Array[0];
        this[offset++] = uInt8Float32Array[1];
        this[offset++] = uInt8Float32Array[2];
        this[offset++] = uInt8Float32Array[3];
        return offset;
    }

    writeFloatBE(val, offset = 0) {
        val = +val;
        checkBounds(this, offset, 3);

        float32Array[0] = val;
        this[offset++] = uInt8Float32Array[3];
        this[offset++] = uInt8Float32Array[2];
        this[offset++] = uInt8Float32Array[1];
        this[offset++] = uInt8Float32Array[0];
        return offset;
    }

    writeDoubleLE(val, offset = 0) {
        val = +val;
        checkBounds(this, offset, 7);

        float64Array[0] = val;
        this[offset++] = uInt8Float64Array[0];
        this[offset++] = uInt8Float64Array[1];
        this[offset++] = uInt8Float64Array[2];
        this[offset++] = uInt8Float64Array[3];
        this[offset++] = uInt8Float64Array[4];
        this[offset++] = uInt8Float64Array[5];
        this[offset++] = uInt8Float64Array[6];
        this[offset++] = uInt8Float64Array[7];
        return offset;
    }

    writeDoubleBE(val, offset = 0) {
        val = +val;
        checkBounds(this, offset, 7);

        float64Array[0] = val;
        this[offset++] = uInt8Float64Array[7];
        this[offset++] = uInt8Float64Array[6];
        this[offset++] = uInt8Float64Array[5];
        this[offset++] = uInt8Float64Array[4];
        this[offset++] = uInt8Float64Array[3];
        this[offset++] = uInt8Float64Array[2];
        this[offset++] = uInt8Float64Array[1];
        this[offset++] = uInt8Float64Array[0];
        return offset;
    }

    // node: slice 与 subarray 同义（共享内存的视图 + 负数索引 + 钳制）；
    // 继承来的 %TypedArray%.prototype.slice 是**拷贝**，会让零拷贝代码静默失效
    slice(start, end) {
        return this.subarray(start, end);
    }

    copy(target, targetStart, sourceStart, sourceEnd) {
        if (!(target instanceof Uint8Array))
            throw invalidArgType('target', 'an instance of Buffer or Uint8Array', target);

        const target_len = target.length;
        const this_len = this.length;

        if (targetStart === undefined)
            targetStart = 0;
        else {
            if (typeof targetStart !== 'number')
                throw invalidArgType('targetStart', 'of type number', targetStart);
            if (targetStart < 0)
                throw outOfRange('targetStart', `>= 0 and <= ${target_len}`, targetStart);
            targetStart = Math.floor(targetStart);      // node: 非整数截断
        }

        if (sourceStart === undefined)
            sourceStart = 0;
        else {
            if (typeof sourceStart !== 'number')
                throw invalidArgType('sourceStart', 'of type number', sourceStart);
            if (sourceStart < 0 || sourceStart > this_len)
                throw outOfRange('sourceStart', `>= 0 and <= ${this_len}`, sourceStart);
            sourceStart = Math.floor(sourceStart);
        }

        if (sourceEnd === undefined)
            sourceEnd = this_len;
        else {
            if (typeof sourceEnd !== 'number')
                throw invalidArgType('sourceEnd', 'of type number', sourceEnd);
            if (sourceEnd < 0)
                throw outOfRange('sourceEnd', `>= 0 and <= ${this_len}`, sourceEnd);
            sourceEnd = Math.min(Math.floor(sourceEnd), this_len);   // node: 超界按源长度截断
        }

        if (targetStart >= target_len || sourceStart >= sourceEnd)
            return 0;

        let count = Math.min(sourceEnd - sourceStart, target_len - targetStart);

        const src = (sourceStart === 0 && count === this_len)
            ? this : new Uint8Array(this.buffer, this.byteOffset + sourceStart, count);

        TypedArrayPrototypeSet(target, src, targetStart);

        return count;
    }

    indexOf(value, byteOffset, encoding) {
        return indexOfImpl(this, value, byteOffset, encoding, false);
    }

    lastIndexOf(value, byteOffset, encoding) {
        return indexOfImpl(this, value, byteOffset, encoding, true);
    }

    includes(value, byteOffset, encoding) {
        return indexOfImpl(this, value, byteOffset, encoding, false) !== -1;
    }

    equals(otherBuffer) {
        if (!(otherBuffer instanceof Uint8Array))
            throw invalidArgType('otherBuffer', 'an instance of Buffer or Uint8Array', otherBuffer);
        return Buffer.native_equals.call(this, otherBuffer);
    }

    compare(target, targetStart, targetEnd, sourceStart, sourceEnd) {
        if (!(target instanceof Uint8Array))
            throw invalidArgType('target', 'an instance of Buffer or Uint8Array', target);

        if (targetStart === undefined && targetEnd === undefined
            && sourceStart === undefined && sourceEnd === undefined)
            return Buffer.compare(this, target);

        return Buffer.compare(subRange(this, sourceStart, sourceEnd, 'sourceStart', 'sourceEnd'),
            subRange(target, targetStart, targetEnd, 'targetStart', 'targetEnd'));
    }

    swap16() { return swapBytes(this, 2); }
    swap32() { return swapBytes(this, 4); }
    swap64() { return swapBytes(this, 8); }

    // node 的 inspect()：<Buffer 61 62> / 超过 50 字节时 "... N more bytes"
    inspect() {
        const max = 50;
        const len = this.byteLength;
        if (len === 0)
            return '<Buffer >';

        let out = '<Buffer';
        const count = Math.min(len, max);
        for (let i = 0; i < count; i++)
            out += ' ' + (this[i] < 16 ? '0' : '') + this[i].toString(16);

        if (len > max)
            out += ' ... ' + (len - max) + ' more byte' + (len - max > 1 ? 's' : '');

        return out + '>';
    }

    hex() {
        return encoding.hex.encode(this);
    }

    base32() {
        return encoding.base32.encode(this);
    }

    base58() {
        return encoding.base58.encode(this);
    }

    base64() {
        return encoding.base64.encode(this);
    }

    toArray() {
        return Array.from(this);
    }

    toJSON() {
        if (this.length > 0)
            return { type: 'Buffer', data: Array.from(this) };
        else
            return { type: 'Buffer', data: [] };
    }

    // node: toString 的区间是 [0, length] 内的整数（负数按 0 钳制，超界截断）
    toString(codec, start = 0, end) {
        const len = this.byteLength;

        // node: 编码名先 ToString（`getEncodingOps` 的 `encoding += ''`），只对
        // **undefined** 采用默认值。null 仍然是 "Unknown encoding: null"，
        // 而 `new String('hex')` / `{toString(){return 'hex'}}` 都可以用。
        if (codec === undefined)
            codec = 'utf8';
        else if (typeof codec !== 'string')
            codec = codec + '';

        if (!Buffer.isEncoding(codec))
            throw unknownEncoding(codec);

        // node: 编码名大小写不敏感（toString('HEX') 等同 toString('hex')）。
        codec = normalizeFromCodec(codec);

        start = start === undefined ? 0 : Math.trunc(start);
        if (!(start >= 0))
            start = 0;
        if (start > len)
            start = len;

        if (end === undefined)
            end = len;
        else {
            end = Math.trunc(end);
            if (!(end >= 0))
                end = 0;
            if (end > len)
                end = len;
        }

        if (end <= start)
            return '';

        // latin1/binary/ascii/ucs2/utf16* 走 native 的零绕行出口：字节直接挂成
        // one-byte / two-byte 外部字符串，不经过 exlib::string(UTF-8)。
        // 对标 Node 的 StringBytes::Encode(LATIN1/UCS2)（node/src/string_bytes.cc:547/604）。
        //
        // 这条路径覆盖了全部 5 个"高位/多字节"解码方向；只有 native 不认识的编码
        // 才返回 undefined 并落到下面的通用路径。
        const fast = Buffer.native_encodeString.call(this, codec, start, end);
        if (fast !== undefined)
            return fast;

        if (start !== 0 || end !== len)
            return encoding.encode(new Uint8Array(this.buffer, this.byteOffset + start, end - start), codec);

        return encoding.encode(this, codec);
    }

    // node: toLocaleString 与 toString 必须是同一个函数（见类定义之后的赋值）

    utf8Slice(start, end) { return encoding.encode(sliceView(this, start, end), 'utf8'); }
    latin1Slice(start, end) { return encoding.encode(sliceView(this, start, end), 'latin1'); }
    asciiSlice(start, end) { return encoding.encode(sliceView(this, start, end), 'ascii'); }
    hexSlice(start, end) { return encoding.encode(sliceView(this, start, end), 'hex'); }
    base64Slice(start, end) { return encoding.encode(sliceView(this, start, end), 'base64'); }
    base64urlSlice(start, end) { return encoding.encode(sliceView(this, start, end), 'base64url'); }
    ucs2Slice(start, end) { return encoding.encode(sliceView(this, start, end), 'ucs2'); }

    utf8Write(str, offset, length) { return this.write(str, offset, length, 'utf8'); }
    latin1Write(str, offset, length) { return this.write(str, offset, length, 'latin1'); }
    asciiWrite(str, offset, length) { return this.write(str, offset, length, 'ascii'); }
    hexWrite(str, offset, length) { return this.write(str, offset, length, 'hex'); }
    base64Write(str, offset, length) { return this.write(str, offset, length, 'base64'); }
    base64urlWrite(str, offset, length) { return this.write(str, offset, length, 'base64url'); }
    ucs2Write(str, offset, length) { return this.write(str, offset, length, 'ucs2'); }
}

// node: Buffer.poolSize 是可写的数据属性，驱动池大小（零长度一律不走池）
// 注意：createPool() 在模块加载时就会跑（那时 Buffer 还在 TDZ 里），所以用 try/catch 惰性读取
function currentPoolSize() {
    try {
        const ps = Buffer.poolSize;
        if (typeof ps === 'number' && ps > 0 && ps % 1 === 0 && ps <= MAX_LENGTH)
            return ps;
    } catch (e) { /* class Buffer 尚未初始化 */ }

    return defaultPoolSize;
}

// node: from() 只接受 string / Buffer|TypedArray / ArrayBuffer|SharedArrayBuffer / Array /
// 类数组对象 / Date（→ 字符串）/ 带 valueOf() 委托的对象；其余抛 TypeError ERR_INVALID_ARG_TYPE
function bufferSourceOf(value) {
    if (value === null || typeof value !== 'object')
        return value;

    if (value instanceof Uint8Array || value instanceof DataView || Array.isArray(value)
        || isAnyArrayBuffer(value) || value instanceof Date)
        return value;

    // String 对象（new String('ab')）必须走 valueOf 解包：它带 length 属性，
    // 会被后面的"类数组"判定当成数组，进而按 code unit 拷贝（'ab' -> 0000）。
    // node 的 Buffer.from(new String('ab'),'hex') 得到 'ab'。
    if (value instanceof String)
        return value.valueOf();

    if (typeof value.length !== 'number' && typeof value.valueOf === 'function') {
        const valueOf = value.valueOf();
        if (valueOf !== null && valueOf !== value && typeof valueOf !== 'object')
            return valueOf;
    }

    return value;
}

Buffer.from = function (bufferOrString, byte_offset, byte_length) {
    const value = bufferSourceOf(bufferOrString);

    // node 的 from() 是严格的（与 legacy `new Buffer(obj)` 不同）：
    // 数字/undefined/null/函数/布尔/symbol/bigint，以及非类数组的普通对象都抛 TypeError
    const FROM_MSG = 'The first argument must be of type string or an instance of Buffer, ArrayBuffer, or Array or an Array-like Object. Received ';

    if (value === null || value === undefined || typeof value === 'number'
        || typeof value === 'function' || typeof value === 'boolean'
        || typeof value === 'symbol' || typeof value === 'bigint')
        throw makeInvalidArgType(FROM_MSG + describeReceived(value));

    if (typeof value === 'object' && !(value instanceof Uint8Array) && !Array.isArray(value)
        && !isAnyArrayBuffer(value) && !(value instanceof DataView) && !(value instanceof Date)
        && typeof value.length !== 'number')     // 读 length 可能触发 getter 抛错（node 同样）
        throw makeInvalidArgType(FROM_MSG + describeReceived(value));

    return new Buffer(value, byte_offset, byte_length);
};

Buffer.alloc = function (size, fill, codec) {
    // node: alloc 必定零填充且不走池
    const buf = new Buffer(validateSize(size), NO_POOL);

    if (fill !== undefined && fill !== 0)
        buf.fill(fill, codec);

    return buf;
};

Buffer.allocUnsafe = function (size) {
    return new Buffer(validateSize(size));
};

Buffer.allocUnsafeSlow = function (size) {
    // node: allocUnsafeSlow 不走池
    return new Buffer(validateSize(size), NO_POOL);
};

Buffer.isBuffer = function (obj) {
    return obj instanceof Buffer;
};

// node: Buffer.from 的编码名大小写不敏感（'HEX'/'BASE64' 与 'hex'/'base64' 同义）。
// native 的 encoding.decode 对大小写敏感，所以在 JS 层统一小写化后再分发。
// 只规范化 native 认得的名字 —— 未知编码保留原样，让 native 抛同样的错。
//
// **别名集合必须与 native 完全一致**（encoding_conv::is_buffer_codec +
// encoding_conv_ucs 的 is_ucs2/is_ucs4/... 判定）。曾经漏掉 'ucs2le'/'ucs-2le'
// 与 'ucs-4'/'utf-32' 家族，导致同一个编码名在不同 API 上给出不同结果：
//   from('ab','UCS-4')      -> TypeError Unknown encoding
//   byteLength('ab','UCS-4') -> 8            （native 侧认得大写以外的写法）
//   from('ab','UTF-32')     -> BOM 前缀 + 8 字节（落到 ICU 的 UTF-32 解码器）
//   from('ab','utf-32')     -> 8 字节         （native 的 ucs4le）
function normalizeFromCodec(codec) {
    switch (codec.toLowerCase()) {
    // node 认得的标准编码
    case 'utf8': case 'utf-8':
    case 'utf16le': case 'utf-16le': case 'ucs2': case 'ucs-2':
    case 'latin1': case 'binary': case 'ascii':
    case 'base64': case 'base64url': case 'hex':
    // fibjs 扩展：Buffer.isEncoding 对它们的大写形式也返回 true，
    // 所以 from/toString/byteLength 必须同样接受，否则同一个编码名在不同
    // API 上表现不一致（'BASE32' 的 byteLength 曾按 utf8 算成 2）
    case 'base32': case 'base58':
    case 'ucs2le': case 'ucs-2le':
    case 'utf16': case 'utf-16':
    case 'utf16be': case 'utf-16be': case 'ucs2be': case 'ucs-2be':
    case 'utf32': case 'utf-32':
    case 'utf32le': case 'utf-32le':
    case 'utf32be': case 'utf-32be':
    case 'ucs4': case 'ucs-4':
    case 'ucs4le': case 'ucs-4le':
    case 'ucs4be': case 'ucs-4be':
        return codec.toLowerCase();
    }
    return codec;
}

// node 认得的标准编码（fibjs 扩展编码继续返回 true，见 plans 的「已接受差异」）
const NODE_ENCODINGS = {
    'utf8': true, 'utf-8': true,
    'utf16le': true, 'utf-16le': true,
    'ucs2': true, 'ucs-2': true,
    'latin1': true, 'binary': true, 'ascii': true,
    'base64': true, 'base64url': true, 'hex': true
};

// node: 编码名的规范化。node 对编码名做小写化（'BASE64' 与 'base64' 同义），
// 所以这里也小写化后再匹配。fibjs 的 native 侧对大小写是敏感的（'LATIN1' 会落到
// ICU 的 windows-1252），因此只有规范化命中的名字才走快路径，其余把**原始**名字
// 交回 native，行为与改动前一致。
function normalizeByteLengthCodec(codec) {
    if (typeof codec !== 'string' || codec.length === 0)
        return undefined;
    // 只覆盖 node 认识的名字。fibjs 扩展的 base32/base58 不在其中（node 抛
    // ERR_UNKNOWN_ENCODING，fibjs 保留支持），它们走 native 的真实换算分支。
    switch (codec.toLowerCase()) {
    case 'hex':
    case 'latin1':
    case 'binary':
    case 'ascii':
    case 'base64':
    case 'base64url':
    case 'ucs2':
    case 'ucs-2':
    case 'ucs2le':
    case 'ucs-2le':
    case 'utf16':
    case 'utf-16':
    case 'utf16le':
    case 'utf-16le':
    case 'utf16be':
    case 'utf-16be':
    case 'ucs2be':
    case 'ucs-2be':
    case 'utf32':
    case 'utf-32':
    case 'utf32le':
    case 'utf-32le':
    case 'utf32be':
    case 'utf-32be':
    case 'ucs4':
    case 'ucs-4':
    case 'ucs4le':
    case 'ucs-4le':
    case 'ucs4be':
    case 'ucs-4be':
        return codec.toLowerCase();
    }
    return undefined;
}

// node: byteLength 对 latin1/binary/ascii 是 UTF-16 code unit 数，
// base64/base64url 是 (去掉最多 2 个尾部 '=' 的长度) * 3 >>> 2 —— 都是 O(1)，
// 而且**不校验内容**（'!!!!' 得 3）。旧实现落到 native 的解码分支，
// 等于为了计数真的解码一遍：64 KB 的 latin1 要 589 µs（node 0.06 µs）。
Buffer.byteLength = function (string, codec) {
    // node: 只接受 string / Buffer|TypedArray / ArrayBuffer|SharedArrayBuffer / DataView
    if (typeof string === 'string') {
        // node: 编码名同样先 ToString（getEncodingOps 的 `encoding += ''`），
        // 但**falsy 视为默认**（`if (!encoding || encoding === 'utf8')`）。
        //   byteLength('ab', {toString(){return 'hex'}})  node: 1
        //   byteLength('ab', null)                        node: 2
        if (codec == null)
            codec = undefined;
        else if (typeof codec !== 'string')
            codec = codec + '';

        var norm = normalizeByteLengthCodec(codec);
        switch (norm) {
        case 'hex':
            // node: hex 的 byteLength 是 floor(len / 2)，**不校验内容**
            return string.length / 2 | 0;

        case 'latin1':
        case 'binary':
        case 'ascii':
            return string.length;

        case 'base64':
        case 'base64url': {
            var n = string.length;
            if (n > 0 && string.charCodeAt(n - 1) === 61) {
                n--;
                if (n > 0 && string.charCodeAt(n - 1) === 61)
                    n--;
            }
            return (n * 3) >>> 2;
        }

        case 'ucs2':
        case 'ucs-2':
        case 'utf16':
        case 'utf-16':
        case 'utf16le':
        case 'utf-16le':
        case 'utf16be':
        case 'utf-16be':
        case 'ucs2be':
        case 'ucs-2be':
            return string.length * 2;

        case 'utf32':
        case 'utf-32':
        case 'utf32le':
        case 'utf-32le':
        case 'utf32be':
        case 'utf-32be':
        case 'ucs4':
        case 'ucs-4':
        case 'ucs4le':
        case 'ucs-4le':
        case 'ucs4be':
        case 'ucs-4be':
            // 每个**码点** 4 字节：代理对合并成一个 astral 码点，所以不能简单
            // 用 length * 4（'a😀b' 有 4 个 code unit 但只有 3 个码点）。
            {
                var cnt = 0;
                for (var k32 = 0; k32 < string.length; k32++) {
                    var c32 = string.charCodeAt(k32);
                    if (c32 >= 0xd800 && c32 <= 0xdbff && k32 + 1 < string.length) {
                        var n32b = string.charCodeAt(k32 + 1);
                        if (n32b >= 0xdc00 && n32b <= 0xdfff)
                            k32++;
                    }
                    cnt++;
                }
                return cnt * 4;
            }
        }

        // 交给 native 的编码名要与 native 的大小写敏感度一致：
        //   - normalizeFromCodec 认得的（含 fibjs 扩展的 ucs4/utf32/base32…）
        //     一律传小写名，这样 'UCS4' 与 'ucs4' 得到相同结果
        //   - 其余（真正的未知编码）保留原始名字，让 native 决定
        // 注意不能直接用 norm：它只覆盖"能用 O(1) 公式算"的那几种
        // （ucs4/base32 等不在其中），传 undefined 会退回原始大小写。
        var nativeCodec = codec;
        if (typeof codec === 'string' && normalizeFromCodec(codec) !== codec)
            nativeCodec = normalizeFromCodec(codec);
        return Buffer.native_byteLength(string, nativeCodec);
    }

    if (string instanceof Uint8Array || isAnyArrayBuffer(string) || string instanceof DataView)
        return string.byteLength;

    throw invalidArgType('string', 'of type string or an instance of Buffer or ArrayBuffer', string);
};

Buffer.isEncoding = function (codec) {
    // node: 空串/含空白/非字符串一律 false（fibjs 旧实现会把 'utf8 '、'utf 8' 当合法）
    if (typeof codec !== 'string' || codec.length === 0)
        return false;
    if (/\s/.test(codec))
        return false;

    if (NODE_ENCODINGS[codec.toLowerCase()])
        return true;

    // fibjs 扩展（utf16be/utf32/base32/base58/字符集…）。
    // 这些名字在 from/toString 里会先经 normalizeFromCodec 小写化再交给 native，
    // 所以这里也要用小写名去问，否则会出现「from('BASE32') 能用但
    // isEncoding('BASE32') 说不能用」的矛盾。
    return encoding.isEncoding(normalizeFromCodec(codec));
};

Buffer.concat = function (list, length) {
    validateArray(list, 'list');

    if (list.length === 0)
        return new Buffer(0, NO_POOL);

    if (length === undefined) {
        length = 0;
        for (let i = 0; i < list.length; i++) {
            if (list[i].length) {
                length += list[i].length;
            }
        }
    } else {
        // node: totalLength 必须是 [0, kMaxLength] 内的整数
        if (typeof length !== 'number')
            throw invalidArgType('length', 'of type number', length);
        if (length !== length || length < 0 || length % 1 !== 0 || length > MAX_LENGTH)
            throw outOfRange('length', `>= 0 and <= ${MAX_LENGTH}`, length);
    }

    const buffer = Buffer.allocUnsafe(length);
    let pos = 0;
    for (let i = 0; i < list.length; i++) {
        let buf = list[i];
        if (!(buf instanceof Uint8Array))
            throw invalidArgType(`list[${i}]`, 'an instance of Buffer or Uint8Array', buf);

        let buf_byteLength = buf.byteLength;
        if (buf_byteLength > length - pos) {
            buf = new Uint8Array(buf.buffer, buf.byteOffset, length - pos);
            buf_byteLength = length - pos;
        }

        TypedArrayPrototypeSet(buffer, buf, pos);
        pos += buf_byteLength;
    }

    return buffer;
};

Buffer.compare = function (buf1, buf2) {
    // node: 只接受 Uint8Array（不再静默把字符串当 buffer 比较），且返回值规范化为 -1/0/1
    if (!(buf1 instanceof Uint8Array))
        throw invalidArgType('buf1', 'an instance of Buffer or Uint8Array', buf1);
    if (!(buf2 instanceof Uint8Array))
        throw invalidArgType('buf2', 'an instance of Buffer or Uint8Array', buf2);

    const r = Buffer.native_compare(buf1, buf2);
    return r < 0 ? -1 : (r > 0 ? 1 : 0);
};

// node: Buffer.copyBytesFrom(view[, offset[, length]])
Buffer.copyBytesFrom = function (view, offset, length) {
    if (!(view instanceof Uint8Array))
        throw invalidArgType('view', 'an instance of TypedArray', view);

    let o = offset === undefined ? 0 : Math.floor(offset);
    let l = length === undefined ? view.byteLength - o : Math.floor(length);

    if (o < 0 || o > view.byteLength)
        throw outOfRange('offset', `>= 0 and <= ${view.byteLength}`, offset);
    if (l < 0 || o + l > view.byteLength)
        throw outOfRange('length', `>= 0 and <= ${view.byteLength - o}`, length);

    return new Buffer(new Uint8Array(view.buffer, view.byteOffset + o, l));
};

// node: readUint8/… 系列是 readUInt8/… 的别名（同一函数引用）
// 注意：必须**不可枚举** —— fibjs 的 util.table 会遍历原型上的可枚举属性，普通赋值会让表格
// 多出 20 行方法名（V8 内建的 TypedArray 方法都是不可枚举的）
['readUint8', 'readUint16LE', 'readUint16BE', 'readUint32LE', 'readUint32BE', 'readUintLE', 'readUintBE',
    'writeUint8', 'writeUint16LE', 'writeUint16BE', 'writeUint32LE', 'writeUint32BE', 'writeUintLE', 'writeUintBE',
    'readBigUint64LE', 'readBigUint64BE', 'writeBigUint64LE', 'writeBigUint64BE'].forEach((name) => {
        const upper = name.replace('Uint', 'UInt');
        if (Buffer.prototype[name] === undefined && Buffer.prototype[upper] !== undefined)
            Object.defineProperty(Buffer.prototype, name, {
                value: Buffer.prototype[upper], writable: true, configurable: true
            });
    });

// node: 废弃但仍在用的 parent / offset 访问器
Object.defineProperty(Buffer.prototype, 'parent', {
    configurable: true,
    get() { return this.buffer; }
});

Object.defineProperty(Buffer.prototype, 'offset', {
    configurable: true,
    get() { return this.byteOffset; }
});

// node: toLocaleString 与 toString 引用同一个函数（不可枚举，理由同上）
Object.defineProperty(Buffer.prototype, 'toLocaleString', {
    value: Buffer.prototype.toString, writable: true, configurable: true
});

Buffer.Buffer = Buffer;
Buffer.poolSize = defaultPoolSize;

Buffer.constants = {
    MAX_LENGTH: MAX_LENGTH,
    MAX_STRING_LENGTH: 536870888
};

/* ====================================================================== */
/* R12: require('buffer') 模块对象（node 形态）                            */
/* ====================================================================== */

function SlowBuffer(size) {
    // node: SlowBuffer 不走池（保留 deprecated 形态，`new` 与直接调用都可用）
    return new Buffer(validateSize(size), NO_POOL);
}

function transcode(source, fromEnc, toEnc) {
    if (!(source instanceof Uint8Array))
        throw invalidArgType('source', 'an instance of Uint8Array', source);
    if (typeof fromEnc !== 'string')
        throw invalidArgType('fromEnc', 'of type string', fromEnc);
    if (typeof toEnc !== 'string')
        throw invalidArgType('toEnc', 'of type string', toEnc);

    const from = normalizeTranscodeEncoding(fromEnc.toLowerCase());
    const to = normalizeTranscodeEncoding(toEnc.toLowerCase());

    if (TRANSCODE_ENCODINGS.indexOf(from) < 0 || TRANSCODE_ENCODINGS.indexOf(to) < 0)
        throw invalidArgValue('fromEnc / toEnc', fromEnc + ' -> ' + toEnc);

    // fibjs: buf -> 字符串用 encode(buf, codec)，字符串 -> buf 用 Buffer.from(str, codec)
    return Buffer.from(encoding.encode(new Buffer(source), from), to);
}

function isAscii(buffer) {
    if (!(buffer instanceof Uint8Array))
        throw invalidArgType('buffer', 'an instance of Uint8Array', buffer);
    for (let i = 0; i < buffer.byteLength; i++)
        if (buffer[i] > 0x7f)
            return false;
    return true;
}

function isUtf8(buffer) {
    if (!(buffer instanceof Uint8Array))
        throw invalidArgType('buffer', 'an instance of Uint8Array', buffer);
    return isValidUtf8(buffer);
}

const INSPECT_MAX_BYTES = 50;
const K_STRING_MAX_LENGTH = 536870888;

const buffer_module = {
    Buffer: Buffer,
    SlowBuffer: SlowBuffer,
    constants: {
        MAX_LENGTH: MAX_LENGTH,
        MAX_STRING_LENGTH: K_STRING_MAX_LENGTH
    },
    kMaxLength: MAX_LENGTH,
    kStringMaxLength: K_STRING_MAX_LENGTH,
    INSPECT_MAX_BYTES: INSPECT_MAX_BYTES,
    transcode: transcode,
    atob: typeof atob === 'function' ? atob : undefined,
    btoa: typeof btoa === 'function' ? btoa : undefined,
    isUtf8: isUtf8,
    isAscii: isAscii,
    Blob: typeof Blob === 'function' ? Blob : undefined
};

// load_module 在本模块求值之后安装 Buffer.native_*，随后回调这里把它们改成不可枚举，
// 避免 Object.keys(Buffer) 多出一堆内部名字（node 的 Buffer 里没有这些）。
// 钩子自身也不可枚举，否则会出现在 require('buffer') 的模块面上。
Object.defineProperty(buffer_module, '__postInstall', {
    writable: true,
    configurable: true,
    value: function () {
        ['native_compare', 'native_equals', 'native_indexOf', 'native_lastIndexOf',
            'native_write', 'native_fill', 'native_byteLength', 'native_encodeString',
            'native_decodeString'].forEach((name) => {
            const value = Buffer[name];
            if (value === undefined)
                return;
            delete Buffer[name];
            Object.defineProperty(Buffer, name, { value: value, writable: true, configurable: true });
        });
    }
});

module.exports = buffer_module;
