/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description Http2Stream 是表示 Http2Session 中单个 HTTP/2 流的对象
 * 
 * 每个 Http2Stream 实例是一个双工流。写入流的数据作为 DATA 帧发送，接收到的数据作为来自远端的 DATA 帧提供。
 * 
 * Http2Stream 实例由 Http2Session 创建，不应直接构造。
 * 
 */
declare class Class_Http2Stream extends Class_Stream {
    /**
     * @description 查询此 Http2Stream 实例的数字流标识符 
     */
    readonly id: number;

    /**
     * @description 查询此 Http2Stream 实例是否已关闭 
     */
    readonly closed: boolean;

    /**
     * @description 查询此 Http2Stream 实例是否已销毁 
     */
    readonly destroyed: boolean;

    /**
     * @description 查询此流接收到的头部对象 
     */
    readonly headers: FIBJS.GeneralObject;

    /**
     * @description 向远端发送响应头
     *      @param headers 包含头部名值对的对象
     *     
     */
    respond(headers?: FIBJS.GeneralObject): void;

    /**
     * @description 向远端发送额外的信息性 (1xx) 头部
     *      @param headers 包含头部名值对的对象
     *     
     */
    additionalHeaders(headers: FIBJS.GeneralObject): void;

    /**
     * @description 向远端发送尾部头，标志流的结束
     *      @param headers 包含尾部头名值对的对象
     *     
     */
    sendTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description 向远端发送 RST_STREAM 帧，关闭流
     *      @param code RST_STREAM 错误码，默认为 NGHTTP2_NO_ERROR (0)
     *     
     */
    rstStream(code?: number): void;

    /**
     * @description 当接收到响应头时触发（客户端）
     *      @param headers 响应头对象
     *     
     */
    on(event: "headers", listener: ()=>void): this;

    /**
     * @description 当接收到尾部头时触发
     *      @param headers 尾部头对象
     *     
     */
    on(event: "trailers", listener: ()=>void): this;

}

