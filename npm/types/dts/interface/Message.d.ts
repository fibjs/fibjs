/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
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
declare class Class_Message extends Class_object {


    /**
     * @description 消息对象构造函数 
     */
    constructor();

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
     * @description 查询消息的数据，此属性会根据 content-type 返回不同数据，为 text 时，返回文本，为 json 时返回 json，其它时候返回二进制 
     */
    readonly data: any;

    /**
     * @description 包含消息数据部分的流对象 
     */
    body: Class_SeekableStream;

    /**
     * @description 从流内读取指定大小的数据，此方法为 body 相应方法的别名
     *      @param bytes 指定要读取的数据量，缺省为读取随机大小的数据块，读出的数据尺寸取决于设备
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    read(bytes?: number): Class_Buffer;

    read(bytes?: number, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 从流内读取剩余的全部数据，此方法为 body 相应方法的别名
     *      @return 返回从流内读取的数据，若无数据可读，或者连接中断，则返回 null
     *      
     */
    readAll(): Class_Buffer;

    readAll(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 写入给定的数据，此方法为 body 相应方法的别名
     *      @param data 给定要写入的数据
     *      
     */
    write(data: Class_Buffer): void;

    write(data: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 以 JSON 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    json(data: any): any;

    /**
     * @description 以 JSON 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    json(): any;

    /**
     * @description 以 msgpack 编码写入给定的数据
     *      @param data 给定要写入的数据
     *      @return 此方法不会返回数据
     *      
     */
    pack(data: any): any;

    /**
     * @description 以 msgpack 编码解析消息中的数据
     *      @return 返回解析的结果
     *      
     */
    pack(): any;

    /**
     * @description 消息数据部分的长度 
     */
    readonly length: number;

    /**
     * @description 设置当前消息处理结束，Chain 处理器不再继续后面的事务 
     */
    end(): void;

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
     *      
     */
    sendTo(stm: Class_Stream): void;

    sendTo(stm: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 从给定的缓存流对象中读取格式化消息，并解析填充对象
     *      @param stm 指定读取格式化消息的流对象
     *      
     */
    readFrom(stm: Class_Stream): void;

    readFrom(stm: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询消息 readFrom 时的流对象 
     */
    readonly stream: Class_Stream;

    /**
     * @description 查询和设置消息处理的最后错误 
     */
    lastError: string;

}

