/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description http 基础消息对象  
 */
declare class Class_HttpMessage extends Class_Message {
    /**
     * @description 协议版本信息，允许的格式为：HTTP/#.# 
     */
    protocol: string;

    /**
     * @description 包含消息中 http 消息头的容器，只读属性 
     */
    readonly headers: Class_HttpCollection;

    /**
     * @description 查询和设定是否保持连接 
     */
    keepAlive: boolean;

    /**
     * @description 查询和设定是否是升级协议 
     */
    upgrade: boolean;

    /**
     * @description 查询和设置最大请求头个数，缺省为 128 
     */
    maxHeadersCount: number;

    /**
     * @description 查询和设置 body 最大尺寸，以 MB 为单位，缺省为 64 
     */
    maxBodySize: number;

    /**
     * @description 查询当前对象的来源 socket 
     */
    readonly socket: Class_Stream;

    /**
     * @description 检查是否存在指定键值的消息头
     *      @param name 指定要检查的键值
     *      @return 返回键值是否存在
     *      
     */
    hasHeader(name: string): boolean;

    /**
     * @description 查询指定键值的第一个消息头
     *      @param name 指定要查询的键值
     *      @return 返回键值所对应的值，若不存在，则返回 undefined
     *      
     */
    firstHeader(name: string): string;

    /**
     * @description 查询指定键值的全部消息头
     *      @param name 指定要查询的键值，传递空字符串返回全部键值的结果
     *      @return 返回键值所对应全部值的数组，若数据不存在，则返回 null
     *      
     */
    allHeader(name?: string): FIBJS.GeneralObject;

    /**
     * @description 添加一个消息头，添加数据并不修改已存在的键值的消息头
     *      @param map 指定要添加的键值数据字典
     *      
     */
    addHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description 添加指定名称的一组消息头，添加数据并不修改已存在的键值的消息头
     *      @param name 指定要添加的键值
     *      @param values 指定要添加的一组数据
     *      
     */
    addHeader(name: string, values: any[]): void;

    /**
     * @description 添加一个消息头，添加数据并不修改已存在的键值的消息头
     *      @param name 指定要添加的键值
     *      @param value 指定要添加的数据
     *      
     */
    addHeader(name: string, value: string): void;

    /**
     * @description 设定一个消息头，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余消息头
     *      @param map 指定要设定的键值数据字典
     *      
     */
    setHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description 设定指定名称的一组消息头，设定数据将修改键值所对应的数值，并清除相同键值的其余消息头
     *      @param name 指定要设定的键值
     *      @param values 指定要设定的一组数据
     *      
     */
    setHeader(name: string, values: any[]): void;

    /**
     * @description 设定一个消息头，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余消息头
     *      @param name 指定要设定的键值
     *      @param value 指定要设定的数据
     *      
     */
    setHeader(name: string, value: string): void;

    /**
     * @description 删除指定键值的全部消息头
     *      @param name 指定要删除的键值
     *      
     */
    removeHeader(name: string): void;

}

