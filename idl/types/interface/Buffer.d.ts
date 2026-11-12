/**
 * @description Binary data buffer object, used for io read/write data handling
 *
 *  Buffer is a global base class, and can be created with `Buffer.alloc(...)` / `Buffer.from(...)`:
 *  ```JavaScript
 *  var buf = Buffer.from("abc");
 *  ```
 *
 *  Buffer is Uint8Array: `Buffer.from('x') instanceof Uint8Array` is true, and all members of
 *  typed arrays (`subarray` / `map` / `filter` / `entries` / the iteration protocol ...) are
 *  inherited directly, so a Buffer can be passed to any interface that accepts a Uint8Array.
 *
 *  Buffer provides static methods such as `Buffer.alloc`, `Buffer.from` and `Buffer.concat`, as
 *  well as instance methods for reading, writing, searching, slicing and encoding conversion.
 *
 *  Notes: like node, `slice` / `subarray` return a view that **shares memory** with the original
 *  object; use `Buffer.from(buf)` when a copy is needed.
 *
 *  `require('buffer')` returns a node-style module object (`Buffer`/`SlowBuffer`/`constants`/
 *  `kMaxLength`/`transcode`/`atob`/`btoa`/`isUtf8`/`isAscii`), and the global `Buffer` is still the class itself.
 *
 *  Supported encodings include: node standard encodings ("utf8", "utf16le"/"ucs2", "latin1",
 *  "ascii", "base64", "base64url", "hex"), as well as fibjs extensions ("base32", "base58",
 *  "utf16be"/"utf32" and all charsets supported by the iconv module).
 */

declare class Class_Buffer<TArrayBuffer extends ArrayBufferLike = ArrayBufferLike> extends Uint8Array<TArrayBuffer> {
    // ---------------------------------------------------------------- Static members

    /**
     * @description Self-reference, equal to Buffer itself
     *
     */
    static Buffer: typeof Class_Buffer;

    /**
     * @description Creates a buffer object
     *      @param value the data source: array / ArrayBuffer / SharedArrayBuffer / Uint8Array / string / object with valueOf / iterable
     *      @param byteOffset the starting offset; valid only when the source is an ArrayBuffer / SharedArrayBuffer / Uint8Array; default 0
     *      @param length the length; valid only when the source is an ArrayBuffer / SharedArrayBuffer / Uint8Array; defaults to the remaining data in the source
     *      @return the buffer object created
     *
     */
    static from(value: ArrayLike<number> | ArrayBufferLike, byteOffset?: number, length?: number): Class_Buffer<ArrayBuffer>;
    static from(value: Class_Buffer | Uint8Array, byteOffset?: number, length?: number): Class_Buffer<ArrayBuffer>;
    static from(str: string, encoding?: string): Class_Buffer<ArrayBuffer>;
    static from(obj: { valueOf(): string | object } | { [Symbol.toPrimitive](hint: 'string'): string }, encoding?: string): Class_Buffer<ArrayBuffer>;
    static from(elements: Iterable<number>): Class_Buffer<ArrayBuffer>;
    static from<T>(arrayLike: ArrayLike<T>, mapfn: (v: T, k: number) => number, thisArg?: any): Class_Buffer<ArrayBuffer>;

    /**
     * @description Allocates a buffer object of the specified size and fills it with the specified content
     *      @param size the number of bytes to allocate
     *      @param fill the content to fill with: number / string / Uint8Array, default 0
     *      @param encoding the encoding when fill is a string, default utf8
     *      @return the buffer object created
     *
     */
    static alloc(size: number, fill?: string | number | Uint8Array, encoding?: string): Class_Buffer<ArrayBuffer>;

    /**
     * @description Allocates a buffer object of the specified size without initializing its content; it may contain sensitive data
     *      @param size the number of bytes to allocate
     *      @return the buffer object created
     *
     */
    static allocUnsafe(size: number): Class_Buffer<ArrayBuffer>;

    /**
     * @description Allocates a buffer object of the specified size without initializing its content and without using the memory pool
     *      @param size the number of bytes to allocate
     *      @return the buffer object created
     *
     */
    static allocUnsafeSlow(size: number): Class_Buffer<ArrayBuffer>;

    /**
     * @description Concatenates a list of buffer objects
     *      @param list the list of buffer objects to concatenate
     *      @param totalLength the length to concatenate to, default the sum of the lengths of the list
     *      @return the concatenated buffer object
     *
     */
    static concat(list: readonly Uint8Array[], totalLength?: number): Class_Buffer<ArrayBuffer>;

    /**
     * @description Checks whether the given object is a buffer object
     *      @param obj the object to check
     *      @return true if it is a buffer object
     *
     */
    static isBuffer(obj: any): obj is Class_Buffer;

    /**
     * @description Checks whether the specified encoding is supported
     *      @param encoding the encoding to check
     *      @return true if supported
     *
     */
    static isEncoding(encoding: string): boolean;

    /**
     * @description Computes the byte length of a string or view in the specified encoding
     *      @param value the string or view to compute
     *      @param encoding the encoding format, default utf8
     *      @return the byte length
     *
     */
    static byteLength(value: string | ArrayBuffer | SharedArrayBuffer | ArrayBufferView, encoding?: string): number;

    /**
     * @description Compares the contents of two buffer objects
     *      @param buf1 the buffer object to compare
     *      @param buf2 the buffer object to compare
     *      @return 0 if equal, -1 if buf1 is less than buf2, 1 if buf1 is greater than buf2
     *
     */
    static compare(buf1: Uint8Array, buf2: Uint8Array): number;

    // ---------------------------------------------------------------- Instance members

    /**
     * @description Gets the size of the buffer object
     */
    readonly length: number;

    /**
     * @description Writes the specified string to the buffer object; the string is utf-8 by default, and only part of the data is written if it would exceed the bounds
     *      @param str the string to write
     *      @param offset the starting position, default 0
     *      @param length the number of bytes to write, defaulting to the byte length of the string to write
     *      @param encoding the encoding format; allowed values: "hex", "base32", "base58", "base64", "utf8", or any charset supported by the iconv module
     *      @return the number of bytes written
     *
     */
    write(str: string, offset?: number, length?: number, encoding?: string): number;
    write(str: string, offset?: number, encoding?: string): number;
    write(str: string, encoding?: string): number;

    /**
     * @description Decodes the buffer object to a string, utf-8 by default
     *      @param encoding the encoding format; allowed values: "hex", "base32", "base58", "base64", "utf8", or any charset supported by the iconv module
     *      @param start the starting position to decode from, default 0
     *      @param end the end position to decode to, default the length of the buffer object
     *      @return the decoded string
     *
     */
    toString(encoding?: string, start?: number, end?: number): string;

    /**
     * @description Returns the node-style JSON representation: `{ type: 'Buffer', data: [byte array] }`
     *      @return the JSON object
     *
     */
    toJSON(): { type: 'Buffer'; data: number[] };

    /**
     * @description Compares whether the current object equals the given object, comparing byte content only
     *      @param otherBuffer the target object to compare
     *      @return the comparison result
     *
     */
    equals(otherBuffer: Uint8Array): boolean;

    /**
     * @description Compares the content with the target buffer object
     *      @param target the target buffer object
     *      @param targetStart the comparison start position in the target buffer object, default 0
     *      @param targetEnd the comparison end position in the target buffer object, default the length of the target buffer object
     *      @param sourceStart the comparison start position in the source buffer object, default 0
     *      @param sourceEnd the comparison end position in the source buffer object, default the length of the source buffer object
     *      @return 0 if equal, -1 if less than the target, 1 if greater than the target
     *
     */
    compare(target: Uint8Array, targetStart?: number, targetEnd?: number, sourceStart?: number, sourceEnd?: number): number;

    /**
     * @description Copies data from a region of the source buffer object to a region of the target buffer object
     *      @param target the target buffer object
     *      @param targetStart the byte position where copying starts in the target buffer object, default 0
     *      @param sourceStart the starting byte position in the source buffer object, default 0
     *      @param sourceEnd the ending byte position in the source buffer object, default the source data length
     *      @return the number of bytes copied
     *
     */
    copy(target: Uint8Array, targetStart?: number, sourceStart?: number, sourceEnd?: number): number;

    /**
     * @description Fills the Buffer object with the specified content; fills the whole buffer when offset and end are not specified
     *      @param value the data to fill with: number / string / Uint8Array
     *      @param offset the starting position, default 0
     *      @param end the end position, default the length of the buffer object
     *      @param encoding the encoding when value is a string, default utf8
     *      @return the current Buffer object
     *
     */
    fill(value: string | number | Uint8Array, offset?: number, end?: number, encoding?: string): this;

    /**
     * @description Searches the buffer object for the first occurrence of the specified content
     *      @param value the content to search for: number / string / Uint8Array
     *      @param byteOffset the starting position, default 0
     *      @param encoding the encoding when value is a string, default utf8
     *      @return the position, or -1 if not found
     *
     */
    indexOf(value: string | number | Uint8Array, byteOffset?: number, encoding?: string): number;

    /**
     * @description Searches the buffer object backwards for the first occurrence of the specified content
     *      @param value the content to search for: number / string / Uint8Array
     *      @param byteOffset the starting position, default the end of the buffer object
     *      @param encoding the encoding when value is a string, default utf8
     *      @return the position, or -1 if not found
     *
     */
    lastIndexOf(value: string | number | Uint8Array, byteOffset?: number, encoding?: string): number;

    /**
     * @description Checks whether the buffer object contains the specified content
     *      @param value the content to search for: number / string / Uint8Array
     *      @param byteOffset the starting position, default 0
     *      @param encoding the encoding when value is a string, default utf8
     *      @return true if contained, otherwise false
     *
     */
    includes(value: string | number | Uint8Array, byteOffset?: number, encoding?: string): boolean;

    /**
     * @description Reads an 8-bit unsigned integer from the buffer object
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 1
     *      @return the integer value read
     *
     */
    readUInt8(offset?: number): number;

    /**
     * @description Reads a 16-bit unsigned integer from the buffer object, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return the integer value read
     *
     */
    readUInt16LE(offset?: number): number;

    /**
     * @description Reads a 16-bit unsigned integer from the buffer object, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return the integer value read
     *
     */
    readUInt16BE(offset?: number): number;

    /**
     * @description Reads a 32-bit unsigned integer from the buffer object, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return the integer value read
     *
     */
    readUInt32LE(offset?: number): number;

    /**
     * @description Reads a 32-bit unsigned integer from the buffer object, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return the integer value read
     *
     */
    readUInt32BE(offset?: number): number;

    /**
     * @description Reads an unsigned integer from the buffer object, up to 48 bits, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to read, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return the integer value read
     *
     */
    readUIntLE(offset?: number, byteLength?: number): number;

    /**
     * @description Reads an unsigned integer from the buffer object, up to 48 bits, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to read, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return the integer value read
     *
     */
    readUIntBE(offset?: number, byteLength?: number): number;

    /**
     * @description Reads an 8-bit signed integer from the buffer object
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 1
     *      @return the integer value read
     *
     */
    readInt8(offset?: number): number;

    /**
     * @description Reads a 16-bit signed integer from the buffer object, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return the integer value read
     *
     */
    readInt16LE(offset?: number): number;

    /**
     * @description Reads a 16-bit signed integer from the buffer object, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return the integer value read
     *
     */
    readInt16BE(offset?: number): number;

    /**
     * @description Reads a 32-bit signed integer from the buffer object, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return the integer value read
     *
     */
    readInt32LE(offset?: number): number;

    /**
     * @description Reads a 32-bit signed integer from the buffer object, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return the integer value read
     *
     */
    readInt32BE(offset?: number): number;

    /**
     * @description Reads a 64-bit signed integer from the buffer object, in little-endian byte order, returned as a number
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the integer value read
     *
     */
    readInt64LE(offset?: number): number;

    /**
     * @description Reads a 64-bit signed integer from the buffer object, in big-endian byte order, returned as a number
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the integer value read
     *
     */
    readInt64BE(offset?: number): number;

    /**
     * @description Reads a 64-bit signed integer from the buffer object, in little-endian byte order, returned as a bigint
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the integer value read
     *
     */
    readBigInt64LE(offset?: number): bigint;

    /**
     * @description Reads a 64-bit signed integer from the buffer object, in big-endian byte order, returned as a bigint
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the integer value read
     *
     */
    readBigInt64BE(offset?: number): bigint;

    /**
     * @description Reads a 64-bit unsigned integer from the buffer object, in little-endian byte order, returned as a bigint
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the integer value read
     *
     */
    readBigUInt64LE(offset?: number): bigint;

    /**
     * @description Reads a 64-bit unsigned integer from the buffer object, in big-endian byte order, returned as a bigint
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the integer value read
     *
     */
    readBigUInt64BE(offset?: number): bigint;

    /**
     * @description Reads a signed integer from the buffer object, up to 48 bits, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to read, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return the integer value read
     *
     */
    readIntLE(offset?: number, byteLength?: number): number;

    /**
     * @description Reads a signed integer from the buffer object, up to 48 bits, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to read, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return the integer value read
     *
     */
    readIntBE(offset?: number, byteLength?: number): number;

    /**
     * @description Reads a floating-point number from the buffer object, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return the floating-point number read
     *
     */
    readFloatLE(offset?: number): number;

    /**
     * @description Reads a floating-point number from the buffer object, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return the floating-point number read
     *
     */
    readFloatBE(offset?: number): number;

    /**
     * @description Reads a double-precision floating-point number from the buffer object, in little-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the double-precision floating-point number read
     *
     */
    readDoubleLE(offset?: number): number;

    /**
     * @description Reads a double-precision floating-point number from the buffer object, in big-endian byte order
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return the double-precision floating-point number read
     *
     */
    readDoubleBE(offset?: number): number;

    /**
     * @description Writes an 8-bit unsigned integer to the buffer object
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 1
     *      @return offset plus the number of bytes written
     *
     */
    writeUInt8(value: number, offset?: number): number;

    /**
     * @description Writes a 16-bit unsigned integer to the buffer object, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return offset plus the number of bytes written
     *
     */
    writeUInt16LE(value: number, offset?: number): number;

    /**
     * @description Writes a 16-bit unsigned integer to the buffer object, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return offset plus the number of bytes written
     *
     */
    writeUInt16BE(value: number, offset?: number): number;

    /**
     * @description Writes a 32-bit unsigned integer to the buffer object, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return offset plus the number of bytes written
     *
     */
    writeUInt32LE(value: number, offset?: number): number;

    /**
     * @description Writes a 32-bit unsigned integer to the buffer object, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return offset plus the number of bytes written
     *
     */
    writeUInt32BE(value: number, offset?: number): number;

    /**
     * @description Writes an unsigned integer to the buffer object, up to 48 bits, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to write, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return offset plus the number of bytes written
     *
     */
    writeUIntLE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description Writes an unsigned integer to the buffer object, up to 48 bits, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to write, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return offset plus the number of bytes written
     *
     */
    writeUIntBE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description Writes an 8-bit signed integer to the buffer object
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 1
     *      @return offset plus the number of bytes written
     *
     */
    writeInt8(value: number, offset?: number): number;

    /**
     * @description Writes a 16-bit signed integer to the buffer object, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return offset plus the number of bytes written
     *
     */
    writeInt16LE(value: number, offset?: number): number;

    /**
     * @description Writes a 16-bit signed integer to the buffer object, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 2
     *      @return offset plus the number of bytes written
     *
     */
    writeInt16BE(value: number, offset?: number): number;

    /**
     * @description Writes a 32-bit signed integer to the buffer object, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return offset plus the number of bytes written
     *
     */
    writeInt32LE(value: number, offset?: number): number;

    /**
     * @description Writes a 32-bit signed integer to the buffer object, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return offset plus the number of bytes written
     *
     */
    writeInt32BE(value: number, offset?: number): number;

    /**
     * @description Writes a 64-bit signed integer to the buffer object, in little-endian byte order; value is a number
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeInt64LE(value: number, offset?: number): number;

    /**
     * @description Writes a 64-bit signed integer to the buffer object, in big-endian byte order; value is a number
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeInt64BE(value: number, offset?: number): number;

    /**
     * @description Writes a 64-bit signed integer to the buffer object, in little-endian byte order; value is a bigint
     *      @param value the integer to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeBigInt64LE(value: bigint, offset?: number): number;

    /**
     * @description Writes a 64-bit signed integer to the buffer object, in big-endian byte order; value is a bigint
     *      @param value the integer to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeBigInt64BE(value: bigint, offset?: number): number;

    /**
     * @description Writes a 64-bit unsigned integer to the buffer object, in little-endian byte order; value is a bigint
     *      @param value the integer to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeBigUInt64LE(value: bigint, offset?: number): number;

    /**
     * @description Writes a 64-bit unsigned integer to the buffer object, in big-endian byte order; value is a bigint
     *      @param value the integer to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeBigUInt64BE(value: bigint, offset?: number): number;

    /**
     * @description Writes a signed integer to the buffer object, up to 48 bits, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to write, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return offset plus the number of bytes written
     *
     */
    writeIntLE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description Writes a signed integer to the buffer object, up to 48 bits, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - byteLength
     *      @param byteLength the number of bytes to write, default 6 bytes, must satisfy 1 <= byteLength <= 6
     *      @return offset plus the number of bytes written
     *
     */
    writeIntBE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description Writes a floating-point number to the buffer object, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return offset plus the number of bytes written
     *
     */
    writeFloatLE(value: number, offset?: number): number;

    /**
     * @description Writes a floating-point number to the buffer object, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 4
     *      @return offset plus the number of bytes written
     *
     */
    writeFloatBE(value: number, offset?: number): number;

    /**
     * @description Writes a double-precision floating-point number to the buffer object, in little-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeDoubleLE(value: number, offset?: number): number;

    /**
     * @description Writes a double-precision floating-point number to the buffer object, in big-endian byte order
     *      @param value the value to write
     *      @param offset the starting position, default 0, must satisfy 0 <= offset <= length - 8
     *      @return offset plus the number of bytes written
     *
     */
    writeDoubleBE(value: number, offset?: number): number;

    /**
     * @description Swaps the byte order of the buffer object in 16-bit units, modifying in place
     *      @return the current Buffer object
     *
     */
    swap16(): this;

    /**
     * @description Swaps the byte order of the buffer object in 32-bit units, modifying in place
     *      @return the current Buffer object
     *
     */
    swap32(): this;

    /**
     * @description Swaps the byte order of the buffer object in 64-bit units, modifying in place
     *      @return the current Buffer object
     *
     */
    swap64(): this;

    /**
     * @description Returns a new buffer object containing the data in the specified range; the new buffer object shares memory with the original one, so modifications affect each other
     *      @param start the start of the range, default the beginning
     *      @param end the end of the range, default the length of the buffer object
     *      @return the new buffer object
     *
     */
    slice(start?: number, end?: number): Class_Buffer<ArrayBuffer>;

    /**
     * @description Returns a new buffer object containing the data in the specified range; the new buffer object shares memory with the original one, so modifications affect each other; equivalent to slice
     *      @param start the start of the range, default the beginning
     *      @param end the end of the range, default the length of the buffer object
     *      @return the new buffer object
     *
     */
    subarray(start?: number, end?: number): Class_Buffer<TArrayBuffer>;

    /**
     * @description Copies data from a region of the source buffer object to a region of the current buffer object
     *      @param src the source buffer object
     *      @param start the starting position to write to in the current buffer object, default 0
     *      @return the number of bytes written
     *
     */
    set(src: Class_Buffer | Uint8Array, start?: number): number;

    /**
     * @description Returns a new array containing all the data of the buffer object, equivalent to `Array.from(buf)`
     *      @return the array containing the object data
     *
     */
    toArray(): number[];

    /**
     * @description Returns a hex-encoded string
     *      @return the encoded string
     *
     */
    hex(): string;

    /**
     * @description Returns a base32-encoded string
     *      @return the encoded string
     *
     */
    base32(): string;

    /**
     * @description Returns a base58-encoded string
     *      @return the encoded string
     *
     */
    base58(): string;

    /**
     * @description Returns a base64-encoded string
     *      @return the encoded string
     *
     */
    base64(): string;
}
