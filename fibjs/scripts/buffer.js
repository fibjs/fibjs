var encoding = require('encoding');
var {
    Uint8Array,
    ArrayBuffer,
    TypedArrayPrototypeFill,
    TypedArrayPrototypeCopyWithin,
    TypedArrayPrototypeSet
} = require('./internal/primordials.js');

const poolSize = 8 * 1024;
let poolOffset, allocPool;

function createPool() {
    allocPool = new ArrayBuffer(poolSize);
    poolOffset = 0;
}
createPool();

function getPool(length) {
    if (length >= poolSize >>> 1)
        return -1;

    if (poolOffset + length > poolSize)
        createPool();

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
        throw new Error('value not a Array');

    if (value.length < minLength) {
        throw new Error(`value.length must be longer than ${minLength}`);
    }
}

function validateInteger(value, name, min = NumberMIN_SAFE_INTEGER, max = NumberMAX_SAFE_INTEGER) {
    if (typeof value !== 'number')
        throw new Error('value not a number');
    if (value < min || value > max)
        throw new Error(`>= ${min} && <= ${max}`);
}

function validateNumber(value, name, min = undefined, max) {
    if (typeof value !== 'number')
        throw new Error('value not a number');

    if ((min != null && value < min) || (max != null && value > max) ||
        ((min != null || max != null) && Number.isNaN(value))) {
        throw new Error(`${min != null ? `>= ${min}` : ''}${min != null && max != null ? ' && ' : ''}${max != null ? `<= ${max}` : ''}`);
    }
}

function boundsError(value, length, type) {
    if (Math.floor(value) !== value) {
        validateNumber(value, type);
        throw new Error('offset not an integer');
    }

    if (length < 0)
        throw new Error('length < 0');

    throw new Error(`offset >= ${type ? 1 : 0} and <= ${length}`);
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
        throw new Error(range);
    }
    checkBounds(buf, offset, byteLength);
}

function writeBigU_Int64LE(buf, value, offset, min, max) {
    checkInt(value, min, max, buf, offset, 7);

    if (typeof value !== 'bigint')
        value = BigInt(value);

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
    checkInt(value, min, max, buf, offset, 7);

    if (typeof value !== 'bigint')
        value = BigInt(value);

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
        throw new ERR_OUT_OF_RANGE('value', `>= ${min} and <= ${max}`, value);
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
            let offset = getPool(bufferOrLength);
            if (offset === -1)
                super(bufferOrLength);
            else
                super(allocPool, offset, bufferOrLength);
        }
        else if (typeof bufferOrLength === 'string') {
            super(encoding.decode(bufferOrLength, byte_offset || 'utf8').buffer);
        }
        else if (bufferOrLength instanceof Uint8Array || Array.isArray(bufferOrLength))
            super(bufferOrLength);
        else
            super(bufferOrLength, byte_offset, byte_length);
    }

    write(buf, offset, length, codec) {
        if (typeof offset === 'string') {
            codec = offset;
            offset = undefined;
            length = undefined;
        } else if (typeof length === 'string') {
            codec = length;
            length = undefined;
        }

        if (typeof buf === 'string')
            buf = encoding.decode(buf, codec || 'utf8');
        else if (!(buf instanceof Uint8Array))
            throw new Error('buf must be a string or Uint8Array');

        let this_byteLength = this.byteLength;
        if (offset === undefined)
            offset = 0;

        if (offset > this_byteLength)
            throw new Error('offset out of bounds');

        let buf_byteLength = buf.byteLength;
        if (length === undefined || length > buf_byteLength)
            length = buf_byteLength;

        if (length + offset > this_byteLength)
            length = this_byteLength - offset;

        if (length !== buf_byteLength)
            buf = new Uint8Array(buf.buffer, buf.byteOffset, length);

        TypedArrayPrototypeSet(this, buf, offset);

        return length;
    }

    fill(buf, offset, end, codec) {
        if (typeof offset === 'string') {
            codec = offset;
            offset = undefined;
            end = undefined;
        } else if (typeof end === 'string') {
            codec = end;
            end = undefined;
        }

        if (typeof buf === 'string')
            return Buffer.native_fill.call(this, buf, offset, end, codec);

        let this_byteLength = this.byteLength;
        if (offset === undefined)
            offset = 0;

        if (offset > this_byteLength)
            throw new Error('offset out of bounds');

        if (end === undefined)
            end = this_byteLength;

        if (end > this_byteLength)
            throw new Error('end out of bounds');

        if (end < offset)
            throw new Error('end < offset');

        if (typeof buf === 'number') {
            TypedArrayPrototypeFill(this, buf, offset, end);
            return this;
        }

        let buf_byteLength = buf.byteLength;
        if (offset < end) {
            if (buf_byteLength >= end - offset) {
                if (buf_byteLength > end - offset)
                    buf = new Uint8Array(buf.buffer, buf.byteOffset, end - offset);
                TypedArrayPrototypeSet(this, buf, offset);
            } else {
                let fill_offset = offset;

                TypedArrayPrototypeSet(this, buf, offset);
                offset += buf_byteLength;

                while (offset < end) {
                    if (buf_byteLength > end - offset)
                        buf_byteLength = end - offset;

                    TypedArrayPrototypeCopyWithin(this, offset, fill_offset, fill_offset + buf_byteLength);
                    offset += buf_byteLength;

                    buf_byteLength *= 2;
                }
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
            throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
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
            throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
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
            throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
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
            throw new ERR_INVALID_ARG_TYPE('offset', 'number', offset);
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

    copy(target, targetStart, sourceStart, sourceEnd) {
        if (!target instanceof Uint8Array)
            throw new Error('not an Uint8Array');

        if (targetStart === undefined)
            targetStart = 0;

        if (sourceStart === undefined)
            sourceStart = 0;

        if (sourceEnd === undefined)
            sourceEnd = this.length;

        if (targetStart >= target.length || sourceStart >= sourceEnd)
            return 0;

        if (sourceStart > this.length)
            throw new Error(`sourceStart <= ${this.length}`);
        if (targetStart + sourceEnd - sourceStart > target.length)
            sourceEnd = sourceStart + target.length - targetStart;

        let buf = this;
        if (sourceStart != 0 || sourceEnd != buf.byteLength)
            buf = new Uint8Array(buf.buffer, buf.byteOffset + sourceStart, sourceEnd - sourceStart);

        TypedArrayPrototypeSet(target, buf, targetStart);

        return sourceEnd - sourceStart;

    }

    indexOf(val, byteOffset) {
        if (typeof val === 'number')
            return super.indexOf(val, byteOffset);

        if (typeof val === 'string')
            val = encoding.decode(val);

        let arrLength = this.length;
        let valLength = val.length;

        if (byteOffset === undefined)
            byteOffset = 0;

        let foundIndex = -1;
        for (let i = byteOffset; i < arrLength; i++) {
            if (this[i] === val[foundIndex === -1 ? 0 : i - foundIndex]) {
                if (foundIndex === -1)
                    foundIndex = i;
                if (i - foundIndex + 1 === valLength)
                    return foundIndex;
            } else {
                if (foundIndex !== -1)
                    i -= i - foundIndex;
                foundIndex = -1;
            }
        }

        return -1;
    }

    equals(otherBuffer) {
        return Buffer.compare(this, otherBuffer) === 0;
    }

    compare(otherBuffer) {
        return Buffer.compare(this, otherBuffer);
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

    toString(codec = "utf8", start = 0, end) {
        if (end === undefined)
            end = this.byteLength;

        if (start != 0 || end != this.byteLength)
            return encoding.encode(this.slice(start, end), codec || 'utf8');

        return encoding.encode(this, codec || 'utf8');
    }
}

Buffer.from = function (bufferOrString, byte_offset, byte_length) {
    if (typeof bufferOrLength === 'string')
        return encoding.decode(bufferOrLength, byte_offset || 'utf8');
    else
        return new Buffer(bufferOrString, byte_offset, byte_length);
};

Buffer.alloc = function (size, fill, codec) {
    var buf = new Buffer(size);
    if (fill !== undefined && fill !== 0)
        buf.fill(fill, codec);

    return buf;
};

Buffer.allocUnsafe = function (size) {
    return new Buffer(size);
};

Buffer.allocUnsafeSlow = function (size) {
    return new Buffer(size);
};

Buffer.isBuffer = function (obj) {
    return obj instanceof Buffer;
};

Buffer.isEncoding = function (codec) {
    return encoding.isEncoding(codec);
};

Buffer.byteLength = function (string, codec) {
    if (string instanceof Uint8Array || string instanceof ArrayBuffer)
        return string.byteLength;
    return encoding.decode(string, codec || 'utf8').byteLength;
}

Buffer.concat = function (list, length) {
    validateArray(list, 'list');

    if (list.length === 0)
        return new Buffer();

    if (length === undefined) {
        length = 0;
        for (let i = 0; i < list.length; i++) {
            if (list[i].length) {
                length += list[i].length;
            }
        }
    }

    const buffer = Buffer.allocUnsafe(length);
    let pos = 0;
    for (let i = 0; i < list.length; i++) {
        let buf = list[i];
        if (!buf instanceof Uint8Array)
            throw new Error('not a Uint8Array');

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
    if (typeof buf1 === 'string')
        buf1 = encoding.decode(buf1);
    if (typeof buf2 === 'string')
        buf2 = encoding.decode(buf2);

    if (!(buf1 instanceof Uint8Array))
        throw new Error('not a Uint8Array');
    if (!(buf2 instanceof Uint8Array))
        throw new Error('not a Uint8Array');

    const len = Math.min(buf1.byteLength, buf2.byteLength);
    for (let i = 0; i < len; i++) {
        if (buf1[i] !== buf2[i])
            return buf1[i] - buf2[i];
    }

    return buf1.byteLength - buf2.byteLength;
};

Buffer.Buffer = Buffer;
module.exports = Buffer;
