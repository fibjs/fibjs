/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description RTCDataChannel接口定义了一个双向的数据通道 
 */
declare class Class_RTCDataChannel extends Class_EventEmitter {
    /**
     * @description 发送二进制数据，该方法用于发送数据到远程端
     * 
     *      @param data 要发送的二进制数据
     *     
     */
    send(data: Class_Buffer): void;

    /**
     * @description 发送文本数据，该方法用于发送数据到远程端
     * 
     *      @param data 要发送的文本数据
     *     
     */
    send(data: string): void;

    /**
     * @description 关闭通道，该方法用于关闭通道 
     */
    close(): void;

    /**
     * @description 返回唯一标识 RTCDataChannel 的 ID 号 
     */
    readonly id: number;

    /**
     * @description 返回一个字符串，其中包含描述数据通道的名称 
     */
    readonly label: string;

    /**
     * @description 返回包含正在使用的子协议名称的字符串 
     */
    readonly protocol: string;

    /**
     * @description 返回当前排队通过数据通道发送的数据的字节数 
     */
    readonly bufferedAmount: number;

    /**
     * @description 通道打开事件，当通道打开时触发 
     */
    onopen: (...args: any[])=>any;

    /**
     * @description 通道消息事件，当接收到消息时触发 
     */
    onmessage: (...args: any[])=>any;

    /**
     * @description 通道关闭事件，当通道关闭时触发 
     */
    onclose: (...args: any[])=>any;

    /**
     * @description 通道错误事件，当通道发生错误时触发 
     */
    onerror: (...args: any[])=>any;

    /**
     * @description 通道缓冲区低事件，当通道缓冲区低时触发 
     */
    onbufferedamountlow: (...args: any[])=>any;

}

