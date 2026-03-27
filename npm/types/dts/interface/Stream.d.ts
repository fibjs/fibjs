/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/StreamReader.d.ts" />
/**
 * @description 流操作对象，用于二进制数据流读写
 * 
 * Stream 为基础对象，用于为流处理定义标准借口，不能独立创建 
 */
declare class Class_Stream extends Class_EventEmitter {
    /**
     * @description 查询 Stream 对应的文件描述符值, 由子类实现 
     */
    readonly fd: number;

    /**
     * @description 从流内读取指定大小的数据
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的数据。若设置了编码则返回字符串，否则返回 Buffer。若无数据可读，或者连接中断，则返回 null
     *      
     */
    read(bytes?: number): any;

    read(bytes?: number, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 从流内读取指定大小的数据
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的数据。若设置了编码则返回字符串，否则返回 Buffer。若无数据可读，或者连接中断，则返回 null
     *      
     */
    readSync(bytes?: number): any;

    /**
     * @description 从流内读取指定大小的数据
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的数据。若设置了编码则返回字符串，否则返回 Buffer。若无数据可读，或者连接中断，则返回 null
     *      
     */
    readAsync(bytes?: number): Promise<any>;

    /**
     * @description 从流内读取指定大小的数据，以 Buffer 形式返回
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的 Buffer 数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readBuffer(bytes?: number): Class_Buffer;

    readBuffer(bytes?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 从流内读取指定大小的数据，以 Buffer 形式返回
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的 Buffer 数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readBufferSync(bytes?: number): Class_Buffer;

    /**
     * @description 从流内读取指定大小的数据，以 Buffer 形式返回
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的 Buffer 数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readBufferAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description 设置流的编码方式。设置后 read() 将返回字符串而非 Buffer 对象
     *      @param encoding 要使用的编码，如 'utf8'、'ascii'、'hex' 等。传入 null 恢复为 Buffer 模式
     *      @return 返回当前流对象
     *      
     */
    setEncoding(encoding: string): Class_Stream;

    /**
     * @description 将给定的二进制数据写入流
     *      @param data 给定要写入的 Buffer 数据
     *      
     */
    writeBuffer(data: Class_Buffer): void;

    writeBuffer(data: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 将给定的二进制数据写入流
     *      @param data 给定要写入的 Buffer 数据
     *      
     */
    writeBufferSync(data: Class_Buffer): void;

    /**
     * @description 将给定的二进制数据写入流
     *      @param data 给定要写入的 Buffer 数据
     *      
     */
    writeBufferAsync(data: Class_Buffer): Promise<void>;

    /**
     * @description 将给定的数据写入流
     *      @param data 给定要写入的数据
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    write(data: Class_Buffer): boolean;

    write(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 将给定的数据写入流
     *      @param data 给定要写入的数据
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    writeSync(data: Class_Buffer): boolean;

    /**
     * @description 将给定的数据写入流
     *      @param data 给定要写入的数据
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    writeAsync(data: Class_Buffer): Promise<boolean>;

    /**
     * @description 将给定的数据写入流
     *      @param data 给定要写入的数据
     *      @param encoding 指定的编码方式，因为 data 为 Buffer 类型，此参数将被忽略
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    write(data: Class_Buffer, encoding: string): boolean;

    write(data: Class_Buffer, encoding: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 将给定的数据写入流
     *      @param data 给定要写入的数据
     *      @param encoding 指定的编码方式，因为 data 为 Buffer 类型，此参数将被忽略
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    writeSync(data: Class_Buffer, encoding: string): boolean;

    /**
     * @description 将给定的数据写入流
     *      @param data 给定要写入的数据
     *      @param encoding 指定的编码方式，因为 data 为 Buffer 类型，此参数将被忽略
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    writeAsync(data: Class_Buffer, encoding: string): Promise<boolean>;

    /**
     * @description 将给定的字符串写入流
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，缺省为 "utf8"
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    write(data: string, encoding?: string): boolean;

    write(data: string, encoding?: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 将给定的字符串写入流
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，缺省为 "utf8"
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    writeSync(data: string, encoding?: string): boolean;

    /**
     * @description 将给定的字符串写入流
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，缺省为 "utf8"
     *      @return 如果流希望调用代码在继续写入其他数据之前等待 'drain' 事件，则返回 true；否则返回 false
     *      
     */
    writeAsync(data: string, encoding?: string): Promise<boolean>;

    /**
     * @description 将流切换到流动读取模式。在 fibjs 下，切换到流动读取模式是不可逆的，不能再切换回非流动读取模式。
     *      @return 返回当前流对象
     *      
     */
    resume(): Class_Stream;

    /**
     * @description 暂停流的自动读取模式。此方法仅为兼容，目前调用此方法不会有任何效果
     *      @return 返回当前流对象
     *      
     */
    pause(): Class_Stream;

    /**
     * @description 将流数据管道传输到目标流。数据通过事件驱动方式从源流传输到目标流，支持背压控制
     *      @param destination 目标流对象
     *      @param options 管道选项，可选
     *      @return 返回目标流对象，支持链式调用
     *      
     */
    pipe(destination: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description 移除所有管道目标，或仅移除指定的目标。此方法仅为兼容，目前调用此方法不会有任何效果
     *      @param destination 要取消管道的特定可写目标
     *      
     */
    unpipe(destination?: Class_Stream): void;

    /**
     * @description 结束流操作，可选择性地写入最后的数据
     *      @return 返回一个异步对象
     *      
     */
    end(): number;

    end(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 结束流操作，可选择性地写入最后的数据
     *      @return 返回一个异步对象
     *      
     */
    endSync(): number;

    /**
     * @description 结束流操作，可选择性地写入最后的数据
     *      @return 返回一个异步对象
     *      
     */
    endAsync(): Promise<number>;

    /**
     * @description 将给定的文件缓冲区写入流并结束流操作
     *      @param data 给定要写入的文件缓冲区数据
     *      @return 返回一个异步对象
     *      
     */
    end(data: Class_Buffer): number;

    end(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将给定的文件缓冲区写入流并结束流操作
     *      @param data 给定要写入的文件缓冲区数据
     *      @return 返回一个异步对象
     *      
     */
    endSync(data: Class_Buffer): number;

    /**
     * @description 将给定的文件缓冲区写入流并结束流操作
     *      @param data 给定要写入的文件缓冲区数据
     *      @return 返回一个异步对象
     *      
     */
    endAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description 将给定的文件缓冲区写入流并结束流操作
     *      @param data 给定要写入的文件缓冲区数据
     *      @param encoding 指定的编码方式，因为 data 为 Buffer 类型，此参数将被忽略
     *      @return 返回一个异步对象
     *      
     */
    end(data: Class_Buffer, encoding: string): number;

    end(data: Class_Buffer, encoding: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将给定的文件缓冲区写入流并结束流操作
     *      @param data 给定要写入的文件缓冲区数据
     *      @param encoding 指定的编码方式，因为 data 为 Buffer 类型，此参数将被忽略
     *      @return 返回一个异步对象
     *      
     */
    endSync(data: Class_Buffer, encoding: string): number;

    /**
     * @description 将给定的文件缓冲区写入流并结束流操作
     *      @param data 给定要写入的文件缓冲区数据
     *      @param encoding 指定的编码方式，因为 data 为 Buffer 类型，此参数将被忽略
     *      @return 返回一个异步对象
     *      
     */
    endAsync(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description 将给定的字符串写入流并结束流操作
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，缺省为 "utf8"
     *      @return 返回一个异步对象
     *      
     */
    end(data: string, encoding?: string): number;

    end(data: string, encoding?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将给定的字符串写入流并结束流操作
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，缺省为 "utf8"
     *      @return 返回一个异步对象
     *      
     */
    endSync(data: string, encoding?: string): number;

    /**
     * @description 将给定的字符串写入流并结束流操作
     *      @param data 给定要写入的字符串数据
     *      @param encoding 指定字符串的编码方式，缺省为 "utf8"
     *      @return 返回一个异步对象
     *      
     */
    endAsync(data: string, encoding?: string): Promise<number>;

    /**
     * @description 将文件缓冲区内容写入物理设备 
     */
    flush(): void;

    flush(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 将文件缓冲区内容写入物理设备 
     */
    flushSync(): void;

    /**
     * @description 将文件缓冲区内容写入物理设备 
     */
    flushAsync(): Promise<void>;

    /**
     * @description 关闭当前流对象 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 关闭当前流对象 
     */
    closeSync(): void;

    /**
     * @description 关闭当前流对象 
     */
    closeAsync(): Promise<void>;

    /**
     * @description 复制流数据到目标流中
     *      @param stm 目标流对象
     *      @param bytes 复制的字节数
     *      @return 返回复制的字节数
     *      
     */
    copyTo(stm: Class_Stream, bytes?: number): number;

    copyTo(stm: Class_Stream, bytes?: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 复制流数据到目标流中
     *      @param stm 目标流对象
     *      @param bytes 复制的字节数
     *      @return 返回复制的字节数
     *      
     */
    copyToSync(stm: Class_Stream, bytes?: number): number;

    /**
     * @description 复制流数据到目标流中
     *      @param stm 目标流对象
     *      @param bytes 复制的字节数
     *      @return 返回复制的字节数
     *      
     */
    copyToAsync(stm: Class_Stream, bytes?: number): Promise<number>;

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

    /**
     * @description 获取流的读取器，兼容 WHATWG ReadableStreamDefaultReader 接口
     *      @return 返回 StreamReader 对象
     *     
     */
    getReader(): Class_StreamReader;

    /**
     * @description 维持 fibjs 进程不退出，在对象绑定期间阻止 fibjs 进程退出
     *      @return 返回当前对象
     *     
     */
    ref(): Class_Stream;

    /**
     * @description 允许 fibjs 进程退出，在对象绑定期间允许 fibjs 进程退出
     *      @return 返回当前对象
     *     
     */
    unref(): Class_Stream;

}

