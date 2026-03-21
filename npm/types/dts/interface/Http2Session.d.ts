/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Http2Stream.d.ts" />
/**
 * @description Http2Session 表示活动的 HTTP/2 会话，管理连接和所有流
 * 
 * Http2Session 实例不应由用户直接构造。服务端在接收到新的 HTTP/2 连接时创建。客户端使用 http2.connect() 创建会话。
 * 
 */
declare class Class_Http2Session extends Class_EventEmitter {
    /**
     * @description 查询此会话的远端设置 
     */
    readonly remoteSettings: FIBJS.GeneralObject;

    /**
     * @description 查询此会话的本地设置 
     */
    readonly localSettings: FIBJS.GeneralObject;

    /**
     * @description 查询会话是否已销毁 
     */
    readonly destroyed: boolean;

    /**
     * @description 查询会话是否已关闭 
     */
    readonly closed: boolean;

    /**
     * @description 查询此会话协商的 ALPN 协议 
     */
    readonly alpnProtocol: string;

    /**
     * @description 查询此会话的底层 TLSSocket 
     */
    readonly socket: Class_Stream;

    /**
     * @description 发起新的 HTTP/2 流以发送请求（仅客户端）
     *      @param headers 包含请求头的对象，必须包含 :method 和 :path 伪头部
     *      @param options 可选的流创建选项
     *      @return 返回新请求的 Http2Stream 对象
     *     
     */
    request(headers: FIBJS.GeneralObject, options?: FIBJS.GeneralObject): Class_Http2Stream;

    /**
     * @description 向远端发送 GOAWAY 帧并优雅关闭会话
     *      @param code HTTP/2 错误码，默认为 NGHTTP2_NO_ERROR (0)
     *      @param lastStreamId 本地处理的最后一个流 ID，默认为 0
     *     
     */
    goaway(code?: number, lastStreamId?: number): void;

    /**
     * @description 向远端发送 PING 帧
     *      @return 返回往返时间（毫秒）
     *     
     */
    ping(): number;

    ping(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 向远端发送 PING 帧
     *      @return 返回往返时间（毫秒）
     *     
     */
    pingSync(): number;

    /**
     * @description 向远端发送 PING 帧
     *      @return 返回往返时间（毫秒）
     *     
     */
    pingAsync(): Promise<number>;

    /**
     * @description 更新此会话的本地设置
     *      @param settings 包含要更新的设置的对象
     *     
     */
    settings(settings: FIBJS.GeneralObject): void;

    /**
     * @description 优雅关闭会话，允许现有流完成 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 优雅关闭会话，允许现有流完成 
     */
    closeSync(): void;

    /**
     * @description 优雅关闭会话，允许现有流完成 
     */
    closeAsync(): Promise<void>;

    /**
     * @description 立即销毁会话，中止所有流 
     */
    destroy(): void;

    /**
     * @description 当创建新流时触发（服务端）
     *      @param stream 新创建的 Http2Stream
     *      @param headers 请求头对象
     *     
     */
    on(event: "stream", listener: ()=>void): this;

    /**
     * @description 当会话收到 GOAWAY 帧时触发 
     */
    on(event: "goaway", listener: ()=>void): this;

    /**
     * @description 当会话发生错误时触发
     *      @param err 错误对象
     *     
     */
    on(event: "error", listener: ()=>void): this;

}

