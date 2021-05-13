/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description http 响应消息对象 
 */
declare class Class_HttpResponse extends Class_HttpMessage {
    /**
     * @description HttpResponse 构造函数，创建一个新的 HttpResponse 对象 
     */
    constructor();

    /**
     * @description 查询和设置响应消息的返回状态 
     */
    statusCode: number;

    /**
     * @description 查询和设置响应消息的返回消息 
     */
    statusMessage: string;

    /**
     * @description 设置响应消息的返回状态，返回消息，并添加响应头
     *      @param statusCode 指定响应消息的返回状态
     *      @param statusMessage 指定响应消息的返回消息
     *      @param headers 指定响应消息添加的响应头
     *      
     */
    writeHead(statusCode: number, statusMessage: string, headers?: FIBJS.GeneralObject): void;

    /**
     * @description 设置响应消息的返回状态，返回消息，并添加响应头
     *      @param statusCode 指定响应消息的返回状态
     *      @param headers 指定响应消息添加的响应头
     *      
     */
    writeHead(statusCode: number, headers?: FIBJS.GeneralObject): void;

    /**
     * @description 返回当前消息的 HttpCookie 对象列表 
     */
    readonly cookies: any[];

    /**
     * @description 向 cookies 添加一个 HttpCookie 对象
     *      @param cookie 指定要添加的 HttpCookie 对象
     *      
     */
    addCookie(cookie: Class_HttpCookie): void;

    /**
     * @description 发送重定向到客户端
     *      @param url 重定向的地址
     *      
     */
    redirect(url: string): void;

    /**
     * @description 发送重定向到客户端
     *      @param statusCode 指定响应消息的返回状态，接受的状态为：301, 302, 307
     *      @param url 重定向的地址
     *      
     */
    redirect(statusCode: number, url: string): void;

    /**
     * @description 仅发送格式化 http 头到给定的流对象
     *      @param stm 指定接收格式化消息的流对象
     *      
     */
    sendHeader(stm: Class_Stream): void;

    sendHeader(stm: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

}

