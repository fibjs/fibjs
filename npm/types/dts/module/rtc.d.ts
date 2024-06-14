/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/RTCPeerConnection.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
/**
 * @description WebRTC 网络实时通信模块 
 */
declare module 'rtc' {
    /**
     * @description WebRTC 连接对象，参见 RTCPeerConnection 
     */
    const RTCPeerConnection: typeof Class_RTCPeerConnection;

    /**
     * @description WebRTC 会话描述对象，参见 RTCSessionDescription 
     */
    const RTCSessionDescription: typeof Class_RTCSessionDescription;

    /**
     * @description WebRTC ICE 候选参数对象，参见 RTCIceCandidate 
     */
    const RTCIceCandidate: typeof Class_RTCIceCandidate;

    /**
     * @description 在指定地址和端口上绑定一个 WebRTC 侦听服务
     * 
     *     bind 方法用于在指定地址和端口上绑定一个 WebRTC 侦听服务，用于响应未握手的 WebRTC 连接请求。
     * 
     *      @param bind_address 绑定地址
     *      @param local_port 本地端口
     *      @param cb 回调函数
     *     
     */
    function bind(bind_address: string, local_port: number, cb: (...args: any[])=>any): void;

    /**
     * @description 在指定端口上绑定一个 WebRTC 侦听服务
     * 
     *     bind 方法用于在指定端口上绑定一个 WebRTC 侦听服务，用于响应未握手的 WebRTC 连接请求。
     * 
     *      @param local_port 本地端口
     *      @param cb 回调函数
     *     
     */
    function bind(local_port: number, cb: (...args: any[])=>any): void;

    /**
     * @description 解除 WebRTC 侦听服务绑定 
     */
    function unbind(): void;

}

