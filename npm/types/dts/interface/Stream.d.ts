/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
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
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    read(bytes?: number): Class_Buffer;

    read(bytes?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 将给定的数据写入流
     *      @param data 给定要写入的数据
     *      @return 返回实际写入的字节数
     *      
     */
    write(data: Class_Buffer): number;

    write(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

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
     * @description 将文件缓冲区内容写入物理设备 
     */
    flush(): void;

    flush(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 关闭当前流对象 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

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

