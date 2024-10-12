/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 二进制数据缓存对象，用于 io 读写的数据处理
 * 
 *  Buffer 对象为全局基础类，在任何时候都可以直接以 new Buffer(...) 创建：
 *  ```JavaScript
 *  var buf = new Buffer();
 *  ```
 *  
 */
declare class Class_Buffer extends Class_object {
    /**
     * @description 二进制数据缓存对象，用于 io 读写的数据处理 
     */
    static Buffer: Class_Buffer;

    /**
     * @description 缓存对象构造函数
     *      @param datas 初始化数据数组
     *      
     */
    constructor(datas: any[]);

    /**
     * @description 缓存对象构造函数
     *      @param datas 初始化数据数组
     *      @param byteOffset 指定数据起始位置，起始为 0
     *      @param length 指定数据长度，起始位 -1，表示剩余所有数据
     *      
     */
    constructor(datas: ArrayBuffer, byteOffset?: number, length?: number);

    /**
     * @description 缓存对象构造函数
     *      @param datas 初始化数据数组
     *      @param byteOffset 指定数据起始位置，起始为 0
     *      @param length 指定数据长度，起始位 -1，表示剩余所有数据
     *      
     */
    constructor(datas: Uint8Array, byteOffset?: number, length?: number);

    /**
     * @description 缓存对象构造函数
     *      @param str 初始化字符串，字符串将以 utf-8 格式写入，缺省则创建一个空对象
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      
     */
    constructor(str: string, codec?: string);

    /**
     * @description 缓存对象构造函数
     *      @param size 初始化缓冲区大小
     *      
     */
    constructor(size?: number);

    /**
     * @description 分配一个指定长度的新缓存区。如果大小为0，将创建一个零长度的缓存区。
     *      @param size 缓冲区的所需长度
     *      @param fill 预先填充新缓冲区的值，可使用 string/buffer/integer 值类型。 默认值：0
     *      @return 填充好的新 Buffer 对象
     *     
     */
    static alloc(size: number, fill?: number): Class_Buffer;

    /**
     * @description 分配一个指定长度的新缓存区。如果大小为0，将创建一个零长度的缓存区。
     *      @param size 缓冲区的所需长度
     *      @param fill 预先填充新缓冲区的值，可使用 string/buffer/integer 值类型。 默认值：0
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 填充好的新 Buffer 对象
     *     
     */
    static alloc(size: number, fill?: string, codec?: string): Class_Buffer;

    /**
     * @description 分配一个指定长度的新缓存区。如果大小为0，将创建一个零长度的缓存区。
     *      @param size 缓冲区的所需长度
     *      @param fill 预先填充新缓冲区的值，可使用 string/buffer/integer 值类型。 默认值：0
     *      @return 填充好的新 Buffer 对象
     *     
     */
    static alloc(size: number, fill: Class_Buffer): Class_Buffer;

    /**
     * @description 分配一个指定长度的新缓存区。如果大小为0，将创建一个零长度的缓存区。
     *      @param size 缓冲区的所需长度
     *      @return 指定尺寸的新 Buffer 对象
     *     
     */
    static allocUnsafe(size: number): Class_Buffer;

    /**
     * @description 分配一个指定长度的新缓存区。如果大小为0，将创建一个零长度的缓存区。
     *      @param size 缓冲区的所需长度
     *      @return 指定尺寸的新 Buffer 对象
     *     
     */
    static allocUnsafeSlow(size: number): Class_Buffer;

    /**
     * @description 通过给定的数组创建 Buffer 对象
     *      @param datas 初始化数据数组
     *      @return 返回 Buffer 实例
     *      
     */
    static from(datas: any[]): Class_Buffer;

    /**
     * @description 通过其他 Buffer 创建 Buffer 对象
     *      @param buffer 给定 Buffer 类型变量用于创建 Buffer 对象
     *      @param byteOffset 指定数据起始位置，起始为 0
     *      @param length 指定数据长度，起始位 -1，表示剩余所有数据
     *      @return 返回 Buffer 实例
     *      
     */
    static from(buffer: Class_Buffer, byteOffset?: number, length?: number): Class_Buffer;

    /**
     * @description 通过其他 Buffer 创建 Buffer 对象
     *      @param datas 初始化数据数组
     *      @param byteOffset 指定数据起始位置，起始为 0
     *      @param length 指定数据长度，起始位 -1，表示剩余所有数据
     *      @return 返回 Buffer 实例
     *      
     */
    static from(datas: ArrayBuffer, byteOffset?: number, length?: number): Class_Buffer;

    /**
     * @description 通过其他 Buffer 创建 Buffer 对象
     *      @param datas 初始化数据数组
     *      @param byteOffset 指定数据起始位置，起始为 0
     *      @param length 指定数据长度，起始位 -1，表示剩余所有数据
     *      @return 返回 Buffer 实例
     *      
     */
    static from(datas: Uint8Array, byteOffset?: number, length?: number): Class_Buffer;

    /**
     * @description 通过字符串创建 Buffer 对象
     *      @param str 初始化字符串，字符串将以 utf-8 格式写入，缺省则创建一个空对象
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回 Buffer 实例
     *      
     */
    static from(str: string, codec?: string): Class_Buffer;

    /**
     * @description 拼接多个缓存区中的数据
     *      @param buflist 待拼接的Buffer数组
     *      @param cutLength 截取多少个Buffer对象
     *      @return 拼接后产生的新 Buffer 对象
     *     
     */
    static concat(buflist: any[], cutLength?: number): Class_Buffer;

    /**
     * @description 检测给定的变量是否是 Buffer 对象
     *      @param v 给定需要检测的变量
     *      @return 传入对象是否 Buffer 对象
     *     
     */
    static isBuffer(v: any): boolean;

    /**
     * @description 检测编码格式是否被支持
     *      @param codec 待检测的编码格式
     *      @return 是否支持
     *      
     */
    static isEncoding(codec: string): boolean;

    /**
     * @description 返回字符串的实际字节长度
     *      @param str 待取字节的字符串，如果str为 ArrayBuffer/TypedArray/DataView/Buffer 对象，则返回它们的实际长度
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回实际字节长度
     *      
     */
    static byteLength(str: string, codec?: string): number;

    /**
     * @description 返回字符串的实际字节长度
     *      @param str 待取字节的字符串，如果str为 ArrayBuffer/TypedArray/DataView/Buffer 对象，则返回它们的实际长度
     *      @return 返回实际字节长度
     *      
     */
    static byteLength(str: ArrayBuffer): number;

    /**
     * @description 返回字符串的实际字节长度
     *      @param str 待取字节的字符串，如果 str 为 ArrayBuffer/TypedArray/DataView/Buffer 对象，则返回它们的实际长度
     *      @return 返回实际字节长度
     *      
     */
    static byteLength(str: Uint8Array): number;

    /**
     * @description 返回字符串的实际字节长度
     *      @param str 待取字节的字符串，如果str为 ArrayBuffer/TypedArray/DataView/Buffer 对象，则返回它们的实际长度
     *      @return 返回实际字节长度
     *      
     */
    static byteLength(str: Class_Buffer): number;

    /**
     * @description 比较 buf1 和 buf2, 往往用于 Buffer 实例之间的排序. 该方式等价于 buf1.compare(buf2).
     *     @param buf1 待比较的 buf
     *     @param buf2 待比较的 buf
     *     @return 返回比较字节长度
     *      
     */
    static compare(buf1: Class_Buffer, buf2: Class_Buffer): number;

    /**
     * @description 获取缓存对象的尺寸 
     */
    readonly length: number;

    /**
     * @description 向缓存对象写入指定字符串，字符串默认为utf-8，越界时只写入部分数据
     *      @param str 待写入的字符串
     *      @param offset 写入起始位置
     *      @param length 写入长度（单位字节，默认值-1），未指定时为待写入字符串的长度
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 写入的数据字节长度
     *      
     */
    write(str: string, offset?: number, length?: number, codec?: string): number;

    /**
     * @description 向缓存对象写入指定字符串，字符串默认为utf-8，越界时只写入部分数据
     *      @param str 待写入的字符串
     *      @param offset 写入起始位置
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 写入的数据字节长度
     *      
     */
    write(str: string, offset?: number, codec?: string): number;

    /**
     * @description 向缓存对象写入指定字符串，字符串默认为utf-8，越界时只写入部分数据
     *      @param str 待写入的字符串
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 写入的数据字节长度
     *      
     */
    write(str: string, codec?: string): number;

    /**
     * @description 为 Buffer 对象填充指定内容数据
     *      @param v 需要填充的数据，如果未指定 offset 和 end，将填充满整个 buffer
     *      @param offset 填充起始位置
     *      @param end 填充终止位置
     *      @return 返回当前 Buffer 对象
     *      
     */
    fill(v: number, offset?: number, end?: number): Class_Buffer;

    /**
     * @description 为 Buffer 对象填充指定内容数据
     *      @param v 需要填充的数据，如果未指定 offset 和 end，将填充满整个 buffer
     *      @param offset 填充起始位置
     *      @param end 填充终止位置
     *      @return 返回当前 Buffer 对象
     *      
     */
    fill(v: Class_Buffer, offset?: number, end?: number): Class_Buffer;

    /**
     * @description 为 Buffer 对象填充指定内容数据
     *      @param v 需要填充的数据，如果未指定 offset 和 end，将填充满整个 buffer
     *      @param offset 填充起始位置
     *      @param end 填充终止位置
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回当前 Buffer 对象
     *      
     */
    fill(v: string, offset?: number, end?: number, codec?: string): Class_Buffer;

    /**
     * @description 为 Buffer 对象填充指定内容数据
     *      @param v 需要填充的数据，如果未指定 offset 和 end，将填充满整个 buffer
     *      @param offset 填充起始位置
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回当前 Buffer 对象
     *      
     */
    fill(v: string, offset: number, codec: string): Class_Buffer;

    /**
     * @description 为 Buffer 对象填充指定内容数据
     *      @param v 需要填充的数据，如果未指定 offset 和 end，将填充满整个 buffer
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回当前 Buffer 对象
     *      
     */
    fill(v: string, codec: string): Class_Buffer;

    /**
     * @description 从源缓存对象区域拷贝数据到目标缓存对象区域
     *      @param targetBuffer 目标缓存对象
     *      @param targetStart 目标缓存对象开始拷贝字节位置，缺省为 0
     *      @param sourceStart 源缓存对象开始字节位置, 缺省为 0
     *      @param sourceEnd 源缓存对象结束字节位置, 缺省为 -1,表示源数据长度
     *      @return 拷贝的数据字节长度
     *      
     */
    copy(targetBuffer: Class_Buffer, targetStart?: number, sourceStart?: number, sourceEnd?: number): number;

    /**
     * @description 从源缓存对象区域拷贝数据到目标缓存对象区域
     *      @param src 目标缓存对象
     *      @param start 源缓存对象开始字节位置
     *      @return 拷贝的数据字节长度
     *      
     */
    set(src: Class_Buffer, start: number): number;

    /**
     * @description 从缓存对象读取一个 8 位无符号整型数值
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readUInt8(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 16 位无符号整型数值，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readUInt16LE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 16 位无符号整型数值，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readUInt16BE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 32 位无符号整型数值，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readUInt32LE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 32 位无符号整型数值，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readUInt32BE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个无符号整型数值，最大支持 48 位，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @param byteLength 指定读取的字节数，缺省 6 个字节
     *      @return 返回读取的整型数值
     *      
     */
    readUIntLE(offset?: number, byteLength?: number): number;

    /**
     * @description 从缓存对象读取一个无符号整型数值，最大支持 48 位，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @param byteLength 指定读取的字节数，缺省 6 个字节
     *      @return 返回读取的整型数值
     *      
     */
    readUIntBE(offset?: number, byteLength?: number): number;

    /**
     * @description 从缓存对象读取一个 64 位整型数值，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readInt64LE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 64 位整型数值，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readInt64BE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 8 位整型数值
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readInt8(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 16 位整型数值，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readInt16LE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 16 位整型数值，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readInt16BE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 32 位整型数值，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readInt32LE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个 32 位整型数值，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的整型数值
     *      
     */
    readInt32BE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个整型数值，最大支持 48 位，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @param byteLength 指定读取的字节数，缺省 6 个字节
     *      @return 返回读取的整型数值
     *      
     */
    readIntLE(offset?: number, byteLength?: number): number;

    /**
     * @description 从缓存对象读取一个整型数值，最大支持 48 位，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @param byteLength 指定读取的字节数，缺省 6 个字节
     *      @return 返回读取的整型数值
     *      
     */
    readIntBE(offset?: number, byteLength?: number): number;

    /**
     * @description 从缓存对象读取一个浮点数，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的浮点数
     *      
     */
    readFloatLE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个浮点数，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的浮点数
     *      
     */
    readFloatBE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个双精度浮点数，以低字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的双精度浮点数
     *      
     */
    readDoubleLE(offset?: number): number;

    /**
     * @description 从缓存对象读取一个双精度浮点数，以高字节序的存储方式
     *      @param offset 指定读取的起始位置，缺省为 0
     *      @return 返回读取的双精度浮点数
     *      
     */
    readDoubleBE(offset?: number): number;

    /**
     * @description 向缓存对象写入一个 8 位无符号整型数值
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeUInt8(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 16 位无符号整型数值，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeUInt16LE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 16 位无符号整型数值，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeUInt16BE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 32 位无符号整型数值，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeUInt32LE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 32 位无符号整型数值，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeUInt32BE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个无符号整型数值，最大支持 48 位，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @param byteLength 指定写入的字节数，缺省 6 个字节
     *      @return offset 加上写入的字节数
     *      
     */
    writeUIntLE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description 向缓存对象写入一个无符号整型数值，最大支持 48 位，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @param byteLength 指定写入的字节数，缺省 6 个字节
     *      @return offset 加上写入的字节数
     *      
     */
    writeUIntBE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description 向缓存对象写入一个 8 位整型数值
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeInt8(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 16 位整型数值，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeInt16LE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 16 位整型数值，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeInt16BE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 32 位整型数值，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeInt32LE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 32 位整型数值，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeInt32BE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 64 位整型数值，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeInt64LE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个 64 位整型数值，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeInt64BE(value: number, offset?: number): number;

    /**
     * @description 向缓存对象写入一个整型数值，最大支持 48 位，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @param byteLength 指定写入的字节数，缺省 6 个字节
     *      @return offset 加上写入的字节数
     *      
     */
    writeIntLE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description 向缓存对象写入一个整型数值，最大支持 48 位，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @param byteLength 指定写入的字节数，缺省 6 个字节
     *      @return offset 加上写入的字节数
     *      
     */
    writeIntBE(value: number, offset?: number, byteLength?: number): number;

    /**
     * @description 向缓存对象写入一个浮点数，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeFloatLE(value: number, offset: number): number;

    /**
     * @description 向缓存对象写入一个浮点数，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeFloatBE(value: number, offset: number): number;

    /**
     * @description 向缓存对象写入一个双精度浮点数，以低字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeDoubleLE(value: number, offset: number): number;

    /**
     * @description 向缓存对象写入一个双精度浮点数，以高字节序的存储方式
     *      @param value 指定写入的数值
     *      @param offset 指定写入的起始位置
     *      @return offset 加上写入的字节数
     *      
     */
    writeDoubleBE(value: number, offset: number): number;

    /**
     * @description 返回某个指定数据在 Buffer 中首次出现的位置
     *      @param v 待查找数据，如果未指定 offset，默认从起始位开始
     *      @param offset 起始查找位置
     *      @return 返回查找到的位置，未找到返回 -1
     *      
     */
    indexOf(v: number, offset?: number): number;

    /**
     * @description 返回某个指定数据在 Buffer 中首次出现的位置
     *      @param v 待查找数据，如果未指定 offset，默认从起始位开始
     *      @param offset 起始查找位置
     *      @return 返回查找到的位置，未找到返回 -1
     *      
     */
    indexOf(v: Class_Buffer, offset?: number): number;

    /**
     * @description 返回某个指定数据在 Buffer 中首次出现的位置
     *      @param v 待查找数据，如果未指定 offset，默认从起始位开始
     *      @param offset 起始查找位置
     *      @return 返回查找到的位置，未找到返回 -1
     *      
     */
    indexOf(v: string, offset?: number): number;

    /**
     * @description 返回某个指定数据在 Buffer 中最后出现的位置
     *      @param v 待查找数据，如果未指定 offset，默认从起始位开始
     *      @param offset 起始查找位置
     *      @return 返回查找到的位置，未找到返回 -1
     *      
     */
    lastIndexOf(v: number, offset?: number): number;

    /**
     * @description 返回某个指定数据在 Buffer 中最后出现的位置
     *      @param v 待查找数据，如果未指定 offset，默认从起始位开始
     *      @param offset 起始查找位置
     *      @return 返回查找到的位置，未找到返回 -1
     *      
     */
    lastIndexOf(v: Class_Buffer, offset?: number): number;

    /**
     * @description 返回某个指定数据在 Buffer 中最后出现的位置
     *      @param v 待查找数据，如果未指定 offset，默认从起始位开始
     *      @param offset 起始查找位置
     *      @return 返回查找到的位置，未找到返回 -1
     *      
     */
    lastIndexOf(v: string, offset?: number): number;

    /**
     * @description 返回一个新缓存对象，包含指定起始到缓存结尾的数据
     *      @param start 指定范围的起始，缺省从头开始
     *      @return 返回新的缓存对象
     *      
     */
    slice(start?: number): Class_Buffer;

    /**
     * @description 返回一个新缓存对象，包含指定范围的数据，若范围超出缓存，则只返回有效部分数据
     *      @param start 指定范围的起始
     *      @param end 指定范围的结束
     *      @return 返回新的缓存对象
     *      
     */
    slice(start: number, end: number): Class_Buffer;

    /**
     * @description 比较当前对象与给定的对象是否相等
     *      @param expected 制定比较的目标对象
     *      @return 返回对象比较的结果
     *     
     */
    equals(expected: Class_object): boolean;

    /**
     * @description 比较缓存区的内容
     *      @param buf 待比较缓存对象
     *      @return 内容比较结果
     *      
     */
    compare(buf: Class_Buffer): number;

    /**
     * @description 返回二进制数据的编码字符串
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @param offset 读取起始位置
     *      @param end 读取终止位置
     *      @return 返回对象的字符串表示
     *     
     */
    toString(codec: string, offset?: number, end?: number): string;

    /**
     * @description 返回二进制数据的编码字符串
     *      @param codec 指定编码格式，允许值为："hex", "base32", "base58", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @param offset 读取起始位置
     *      @return 返回对象的字符串表示
     *     
     */
    toString(codec: string, offset?: number): string;

    /**
     * @description 返回二进制数据的 utf8 编码字符串
     *      @return 返回对象的字符串表示
     *     
     */
    toString(): string;

    /**
     * @description 返回全部二进制数据的数组
     *      @return 返回包含对象数据的数组
     *     
     */
    toArray(): any[];

    /**
     * @description 使用 16 进制编码缓存对象内容
     *      @return 返回编码字符串
     *      
     */
    hex(): string;

    /**
     * @description 使用 base32 编码缓存对象内容
     *      @return 返回编码字符串
     *      
     */
    base32(): string;

    /**
     * @description 使用 base58 编码缓存对象内容
     *      @return 返回编码字符串
     *      
     */
    base58(): string;

    /**
     * @description 使用 base64 编码缓存对象内容
     *      @return 返回编码字符串
     *      
     */
    base64(): string;

}

