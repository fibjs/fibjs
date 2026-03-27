/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/**
 * @description 基础消息对象
 * 
 *  Message 对象兼容于 mq 各个模块，可用于构建自定义消息处理系统，创建方法：
 *  ```JavaScript
 *  var mq = require("mq");
 *  var m = new mq.Message();
 *  ```
 * 
 */
declare class Class_Message extends Class_EventEmitter {


    /**
     * @description 消息对象构造函数 
     */
    constructor();

    /**
     * @description 当前消息是否已经发送 
     */
    readonly sent: boolean;

    /**
     * @description 消息的基本内容 
     */
    value: string;

    /**
     * @description 消息的基本参数 
     */
    readonly params: any[];

    /**
     * @description 消息类型 
     */
    type: number;

    /**
     * @description 包含消息数据部分的流对象 
     */
    body: Class_Stream;

    /**
     * @description 查询消息的 body 是否已被消费 
     */
    readonly bodyUsed: boolean;

    /**
     * @description 从流内读取指定大小的数据，此方法为 body 相应方法的别名
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    read(bytes?: number): Class_Buffer;

    read(bytes?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 从流内读取指定大小的数据，此方法为 body 相应方法的别名
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readSync(bytes?: number): Class_Buffer;

    /**
     * @description 从流内读取指定大小的数据，此方法为 body 相应方法的别名
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description 从流内读取剩余的全部数据，此方法为 body 相应方法的别名
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readAll(): Class_Buffer;

    readAll(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 从流内读取剩余的全部数据，此方法为 body 相应方法的别名
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readAllSync(): Class_Buffer;

    /**
     * @description 从流内读取剩余的全部数据，此方法为 body 相应方法的别名
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readAllAsync(): Promise<Class_Buffer>;

    /**
     * @description 写入给定的数据，此方法为 body 相应方法的别名
     *      @param data 给定要写入的数据
     *      @return 返回实际写入的字节数
     *      
     */
    write(data: Class_Buffer): number;

    write(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 写入给定的数据，此方法为 body 相应方法的别名
     *      @param data 给定要写入的数据
     *      @return 返回实际写入的字节数
     *      
     */
    writeSync(data: Class_Buffer): number;

    /**
     * @description 写入给定的数据，此方法为 body 相应方法的别名
     *      @param data 给定要写入的数据
     *      @return 返回实际写入的字节数
     *      
     */
    writeAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description 写入给定的文本数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    text(data: string): string;

    text(data: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 写入给定的文本数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    textSync(data: string): string;

    /**
     * @description 写入给定的文本数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    textAsync(data: string): Promise<string>;

    /**
     * @description 以文本编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    text(): string;

    text(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 以文本编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    textSync(): string;

    /**
     * @description 以文本编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    textAsync(): Promise<string>;

    /**
     * @description 以二进制形式返回消息的数据部分
     *      @return 返回包含消息数据部分的 ArrayBuffer 对象
     *      
     */
    arrayBuffer(): ArrayBuffer;

    arrayBuffer(callback: (err: Error | undefined | null, retVal: ArrayBuffer)=>any): void;

    /**
     * @description 以二进制形式返回消息的数据部分
     *      @return 返回包含消息数据部分的 ArrayBuffer 对象
     *      
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description 以二进制形式返回消息的数据部分
     *      @return 返回包含消息数据部分的 ArrayBuffer 对象
     *      
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

    /**
     * @description 以 Blob 形式返回消息中的数据部分
     *      @param type Blob 的 MIME 类型，默认为空字符串
     *      @return 返回包含消息数据部分的 Blob 对象
     *      
     */
    blob(type?: string): Class_Blob;

    blob(type?: string, callback: (err: Error | undefined | null, retVal: Class_Blob)=>any): void;

    /**
     * @description 以 Blob 形式返回消息中的数据部分
     *      @param type Blob 的 MIME 类型，默认为空字符串
     *      @return 返回包含消息数据部分的 Blob 对象
     *      
     */
    blobSync(type?: string): Class_Blob;

    /**
     * @description 以 Blob 形式返回消息中的数据部分
     *      @param type Blob 的 MIME 类型，默认为空字符串
     *      @return 返回包含消息数据部分的 Blob 对象
     *      
     */
    blobAsync(type?: string): Promise<Class_Blob>;

    /**
     * @description 以 Buffer 形式返回消息中的数据部分
     *      @return 返回包含消息数据部分的 Buffer，若无数据则返回空 Buffer
     *      
     */
    bytes(): Class_Buffer;

    bytes(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 以 Buffer 形式返回消息中的数据部分
     *      @return 返回包含消息数据部分的 Buffer，若无数据则返回空 Buffer
     *      
     */
    bytesSync(): Class_Buffer;

    /**
     * @description 以 Buffer 形式返回消息中的数据部分
     *      @return 返回包含消息数据部分的 Buffer，若无数据则返回空 Buffer
     *      
     */
    bytesAsync(): Promise<Class_Buffer>;

    /**
     * @description 以 JSON 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    json(data: any): any;

    json(data: any, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 以 JSON 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    jsonSync(data: any): any;

    /**
     * @description 以 JSON 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    jsonAsync(data: any): Promise<any>;

    /**
     * @description 以 JSON 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    json(): any;

    json(callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 以 JSON 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    jsonSync(): any;

    /**
     * @description 以 JSON 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    jsonAsync(): Promise<any>;

    /**
     * @description 以 msgpack 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    pack(data: any): any;

    pack(data: any, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 以 msgpack 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    packSync(data: any): any;

    /**
     * @description 以 msgpack 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    packAsync(data: any): Promise<any>;

    /**
     * @description 以 msgpack 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    pack(): any;

    pack(callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 以 msgpack 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    packSync(): any;

    /**
     * @description 以 msgpack 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    packAsync(): Promise<any>;

    /**
     * @description 消息数据部分的长度 
     */
    readonly length: number;

    /**
     * @description 设置当前消息处理结束，Chain 处理器不再继续后面的事务
     *      @return 成功返回 0
     *      
     */
    end(): number;

    end(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 设置当前消息处理结束，Chain 处理器不再继续后面的事务
     *      @return 成功返回 0
     *      
     */
    endSync(): number;

    /**
     * @description 设置当前消息处理结束，Chain 处理器不再继续后面的事务
     *      @return 成功返回 0
     *      
     */
    endAsync(): Promise<number>;

    /**
     * @description 写入给定的数据并设置当前消息处理结束
     *      @param data 给定要写入的数据
     *      @return 成功返回 0
     *      
     */
    end(data: Class_Buffer): number;

    end(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 写入给定的数据并设置当前消息处理结束
     *      @param data 给定要写入的数据
     *      @return 成功返回 0
     *      
     */
    endSync(data: Class_Buffer): number;

    /**
     * @description 写入给定的数据并设置当前消息处理结束
     *      @param data 给定要写入的数据
     *      @return 成功返回 0
     *      
     */
    endAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description 写入给定的数据并设置当前消息处理结束
     *      @param data 给定要写入的数据
     *      @param encoding 指定编码方式，由于 data 是 Buffer 类型，此参数将被忽略
     *      @return 成功返回 0
     *      
     */
    end(data: Class_Buffer, encoding: string): number;

    end(data: Class_Buffer, encoding: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 写入给定的数据并设置当前消息处理结束
     *      @param data 给定要写入的数据
     *      @param encoding 指定编码方式，由于 data 是 Buffer 类型，此参数将被忽略
     *      @return 成功返回 0
     *      
     */
    endSync(data: Class_Buffer, encoding: string): number;

    /**
     * @description 写入给定的数据并设置当前消息处理结束
     *      @param data 给定要写入的数据
     *      @param encoding 指定编码方式，由于 data 是 Buffer 类型，此参数将被忽略
     *      @return 成功返回 0
     *      
     */
    endAsync(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description 写入给定的字符串数据并设置当前消息处理结束
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，默认为 "utf8"
     *      @return 成功返回 0
     *      
     */
    end(data: string, encoding?: string): number;

    end(data: string, encoding?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 写入给定的字符串数据并设置当前消息处理结束
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，默认为 "utf8"
     *      @return 成功返回 0
     *      
     */
    endSync(data: string, encoding?: string): number;

    /**
     * @description 写入给定的字符串数据并设置当前消息处理结束
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，默认为 "utf8"
     *      @return 成功返回 0
     *      
     */
    endAsync(data: string, encoding?: string): Promise<number>;

    /**
     * @description 查询当前消息是否结束
     *      @return 结束则返回 true
     *     
     */
    isEnded(): boolean;

    /**
     * @description 清除消息的内容 
     */
    clear(): void;

    /**
     * @description 发送格式化消息到给定的流对象
     *      @param stm 指定接收格式化消息的流对象
     *      @param options 指定发送选项
     *      
     */
    sendTo(stm: Class_Stream, options?: FIBJS.GeneralObject): void;

    sendTo(stm: Class_Stream, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 发送格式化消息到给定的流对象
     *      @param stm 指定接收格式化消息的流对象
     *      @param options 指定发送选项
     *      
     */
    sendToSync(stm: Class_Stream, options?: FIBJS.GeneralObject): void;

    /**
     * @description 发送格式化消息到给定的流对象
     *      @param stm 指定接收格式化消息的流对象
     *      @param options 指定发送选项
     *      
     */
    sendToAsync(stm: Class_Stream, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description 从给定的缓存流对象中读取格式化消息，并解析填充对象
     *      @param stm 指定读取格式化消息的流对象
     *      @param options 指定读取选项
     *      
     */
    readFrom(stm: Class_Stream, options?: FIBJS.GeneralObject): void;

    readFrom(stm: Class_Stream, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 从给定的缓存流对象中读取格式化消息，并解析填充对象
     *      @param stm 指定读取格式化消息的流对象
     *      @param options 指定读取选项
     *      
     */
    readFromSync(stm: Class_Stream, options?: FIBJS.GeneralObject): void;

    /**
     * @description 从给定的缓存流对象中读取格式化消息，并解析填充对象
     *      @param stm 指定读取格式化消息的流对象
     *      @param options 指定读取选项
     *      
     */
    readFromAsync(stm: Class_Stream, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description 查询消息 readFrom 时的流对象 
     */
    readonly stream: Class_Stream;

    /**
     * @description 查询和设置消息处理的最后错误 
     */
    lastError: string;

    /**
     * @description 复制当前消息对象
     *      @return 返回复制的消息对象
     *     
     */
    clone(): Class_Message;

    /**
     * @description 将消息的 body 流切换到流动读取模式
     *      @return 返回消息对象
     *      
     */
    resume(): Class_Message;

    /**
     * @description 暂停消息的 body 流的自动读取模式。此方法仅为兼容，调用后不会有实际效果
     *      @return 返回消息对象
     *      
     */
    pause(): Class_Message;

    /**
     * @description 将消息的 body 流数据管道传输到目标流
     *      @param destination 目标流对象
     *      @param options 管道选项，可选
     *      @return 返回目标流对象
     *      
     */
    pipe(destination: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description 移除消息的 body 流的所有管道目标。此方法仅为兼容，调用后不会有实际效果
     *      @param destination 要取消管道的特定可写目标
     *      
     */
    unpipe(destination?: Class_Stream): void;

    /**
     * @description 查询和绑定流数据事件，相当于 on("data", func);
     *      @param data 读取到的数据
     *      
     */
    on(event: "data", listener: ()=>void): this;

    /**
     * @description 查询和绑定流关闭事件，相当于 on("close", func); 
     */
    on(event: "close", listener: ()=>void): this;

    /**
     * @description 查询和绑定流错误事件，相当于 on("error", func);
     *      @param code 错误码
     *      
     */
    on(event: "error", listener: ()=>void): this;

}

