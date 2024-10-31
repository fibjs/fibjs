/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/RTCPeerConnection.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
/**
 * @description WebRTC 网络实时通信模块 
 * 
 * `rtc` 模块是一个用于实现 WebRTC 网络实时通信的模块。它提供了一系列功能和接口，帮助开发者创建和管理 WebRTC 连接，发送和接收实时数据。该模块包含以下主要组件：
 * 
 * 1. **RTCPeerConnection**：核心对象，用于创建和管理 WebRTC 连接，处理连接状态，发送和接收媒体数据。
 * 2. **RTCSessionDescription**：会话描述对象，用于描述 WebRTC 连接的媒体格式和其他属性。
 * 3. **RTCIceCandidate**：ICE 候选参数对象，用于 NAT 穿透和连接建立。
 * 4. **RTCDataChannel**：数据通道接口，用于在 WebRTC 连接中传输任意类型的数据。
 * 
 * 此外，`rtc` 模块还提供了全局 SCTP 参数设置、侦听服务绑定和解除绑定等功能，确保开发者能够灵活地配置和管理 WebRTC 连接。通过这些接口和方法，开发者可以轻松实现实时音视频通信、文件传输、文本聊天等功能。
 * 
 * ### 示例
 * 
 * 以下是如何使用 `rtc` 模块创建一个简单的 WebRTC 连接的示例：
 * 
 * ```javascript
 * const rtc = require('rtc');
 * 
 * // 创建 RTCPeerConnection 对象
 * var pc = new rtc.RTCPeerConnection({
 *     iceServers: [{urls: 'stun:stun.l.google.com:19302'}]
 * });
 * 
 * // 创建数据通道
 * var dataChannel = pc.createDataChannel('myDataChannel');
 * 
 * // 设置本地描述
 * pc.setLocalDescription(new rtc.RTCSessionDescription({
 *     type: 'offer',
 *     sdp: 'v=0...'
 * })).then(() => {
 *     console.log('Local description set');
 * });
 * 
 * // 设置远程描述
 * pc.setRemoteDescription(new rtc.RTCSessionDescription({
 *     type: 'answer',
 *     sdp: 'v=0...'
 * })).then(() => {
 *     console.log('Remote description set');
 * });
 * 
 * // 添加 ICE 候选项
 * pc.addIceCandidate(new rtc.RTCIceCandidate({
 *     candidate: 'candidate:842163049 1 udp 1677729535 192.168.1.2 3478 typ srflx raddr 0.0.0.0 rport 0 generation 0 ufrag abc network-id 1',
 *     sdpMid: '0'
 * })).then(() => {
 *     console.log('ICE candidate added');
 * });
 * 
 * // 监听连接状态变化
 * pc.onconnectionstatechange = function(event) {
 *     console.log('Connection state change: ', pc.connectionState);
 * };
 * 
 * // 监听数据通道事件
 * pc.ondatachannel = function(event) {
 *     var receiveChannel = event.channel;
 *     receiveChannel.onmessage = function(event) {
 *         console.log('Received message: ', event.data);
 *     };
 * };
 * 
 * // 监听 ICE 候选项事件
 * pc.onicecandidate = function(event) {
 *     if (event.candidate) {
 *         console.log('New ICE candidate: ', event.candidate);
 *     }
 * };
 * 
 * // 监听 ICE 连接状态变化
 * pc.oniceconnectionstatechange = function(event) {
 *     console.log('ICE connection state change: ', pc.iceConnectionState);
 * };
 * 
 * // 监听信令状态变化
 * pc.onsignalingstatechange = function(event) {
 *     console.log('Signaling state change: ', pc.signalingState);
 * };
 * 
 * // 监听媒体轨道事件
 * pc.ontrack = function(event) {
 *     console.log('New track: ', event.track);
 * };
 * ```
 * 
 * 通过上述示例代码，开发者可以创建一个基本的 WebRTC 连接，并处理连接状态、数据通道、ICE 候选项等事件。
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
    function listen(bind_address: string, local_port: number, cb: (...args: any[])=>any): void;

    /**
     * @description 在指定端口上绑定一个 WebRTC 侦听服务
     * 
     *     bind 方法用于在指定端口上绑定一个 WebRTC 侦听服务，用于响应未握手的 WebRTC 连接请求。
     * 
     *      @param local_port 本地端口
     *      @param cb 回调函数
     *     
     */
    function listen(local_port: number, cb: (...args: any[])=>any): void;

    /**
     * @description 解除 WebRTC 侦听服务绑定
     *      @param bind_address 绑定地址
     *      @param local_port 本地端口
     *      
     */
    function stopListen(bind_address: string, local_port: number): void;

    /**
     * @description 解除 WebRTC 侦听服务绑定
     *      @param local_port 本地端口
     *      
     */
    function stopListen(local_port: number): void;

    /**
     * @@description 设置 WebRTC 全局 SCTP 参数
     * 
     *      setSctpSettings 方法用于设置 WebRTC 全局 SCTP 参数，新设置的参数会立即生效。已经存在的连接不受影响。支持以下参数：
     *         - `recvBufferSize` - 接收缓冲区大小，以字节为单位（默认值: 1MiB）
     *         - `sendBufferSize` - 发送缓冲区大小，以字节为单位（默认值: 1MiB）
     *         - `maxChunksOnQueue` - 队列中最大数据块数量（默认值: 10K）
     *         - `initialCongestionWindow` - 初始拥塞窗口大小，以 MTU（最大传输单元）为单位（默认值: 10 MTUs）
     *         - `maxBurst` - 最大突发传输量，以 MTU 为单位（默认值: 10 MTUs）
     *         - `congestionControlModule` - 拥塞控制模块，0: RFC2581（默认），1: HSTCP，2: H-TCP，3: RTCC
     *         - `delayedSackTimeMs` - 延迟确认时间，以毫秒为单位（默认值: 20ms）
     *         - `minRetransmitTimeoutMs` - 最小重传超时时间，以毫秒为单位（默认值: 200ms）
     *         - `maxRetransmitTimeoutMs` - 最大重传超时时间，以毫秒为单位（默认值: 10s）
     *         - `initialRetransmitTimeoutMs` - 初始重传超时时间，以毫秒为单位（默认值: 1s）
     *         - `maxRetransmitAttempts` - 最大重传尝试次数（默认值: 5）
     *         - `heartbeatIntervalMs` - 心跳间隔时间，以毫秒为单位（默认值: 10s）
     * 
     *      @param settings SCTP 参数
     *     
     */
    function setSctpSettings(settings: FIBJS.GeneralObject): void;

}

