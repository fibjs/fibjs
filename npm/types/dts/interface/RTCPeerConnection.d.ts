/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/RTCDataChannel.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
/**
 * @description RTCPeerConnection 定义 WebRTC 连接的方法和属性 
 * 
 * RTCPeerConnection 是 WebRTC 连接的核心对象，用于创建 WebRTC 连接，管理连接状态，发送和接收媒体数据等。
 * 
 * RTCPeerConnection 对象的创建方式如下：
 * ```JavaScript
 * const rtc = require('rtc');
 * var pc = new rtc.RTCPeerConnection();
 * ```
 * 
 */
declare class Class_RTCPeerConnection extends Class_EventEmitter {
    /**
     * @description 构造一个新的 WebRTC 连接对象，并初始化基础参数
     * 
     *      options 参数是一个对象，包含以下属性：
     *         - certificateType: 证书类型，可选值为 'rsa'、'ecdsa'，默认为 'ecdsa'
     *         - iceTransportPolicy: ICE 传输策略，可选值为 'all'、'relay'，默认为 'all'
     *         - iceServers: ICE 服务器列表，用于 NAT 穿透，格式为 [{urls: 'stun:stun.l.google.com:19302'}]
     *         - maxMessageSize: 最大消息大小，用于指定数据通道的最大消息大小
     *         - enableIceUdpMux: 是否启用 ICE UDP 多路复用
     *         - disableFingerprintVerification: 是否禁用指纹验证
     *         - bindAddress: 绑定地址，用于指定本地 IP 地址
     *         - port: 本地端口号，用于指定本地端口
     *         - iceUfrag: ICE 用户名
     *         - icePwd: ICE 密码
     *         - certPem: 证书 PEM 格式
     *         - keyPem: 私钥 PEM 格式        
     * 
     *      @param options 初始化参数
     *     
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description 创建一个与远程对等点链接的新通道
     * 
     *      创建一个与远程对等点链接的新通道，可以通过该通道传输任何类型的数据。这对于反向通道内容非常有用，例如图像、文件传输、文本聊天、游戏更新数据包等。
     * 
     *      options 参数是一个对象，包含以下属性：
     *         - ordered: 是否保证数据包的顺序，默认为 true
     *         - maxPacketLifeTime: 数据包的最大生命周期，默认为 0
     *         - maxRetransmits: 数据包的最大重传次数，默认为 0
     *         - protocol: 通道协议，默认为 ''
     *         - negotiated: 是否为协商通道，默认为 false
     *         - id: 通道 ID，默认为 0
     * 
     *      @param label 通道名称
     *      @param options 通道参数
     *      @return 返回创建的通道对象
     *     
     */
    createDataChannel(label: string, options?: FIBJS.GeneralObject): Class_RTCDataChannel;

    /**
     * @description 更改与连接关联的本地描述
     *     
     *      此方法指定连接本地端的属性，包括媒体格式。该方法采用单个参数（会话描述），并返回一个 Promise ，一旦描述发生异步更改，该值就会得到满足。
     *     
     */
    setLocalDescription(): void;

    setLocalDescription(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 更改与连接关联的本地描述
     *     
     *      此方法指定连接本地端的属性，包括媒体格式。该方法采用单个参数（会话描述），并返回一个 Promise ，一旦描述发生异步更改，该值就会得到满足。
     * 
     *      @param description 会话描述
     *     
     */
    setLocalDescription(description: Class_RTCSessionDescription): void;

    setLocalDescription(description: Class_RTCSessionDescription, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 更改与连接关联的远程描述
     *     
     *      此方法指定连接远程端的属性，包括媒体格式。该方法采用单个参数（会话描述），并返回一个 Promise ，一旦描述发生异步更改，该值就会得到满足。
     * 
     *      @param description 会话描述
     *     
     */
    setRemoteDescription(description: Class_RTCSessionDescription): void;

    setRemoteDescription(description: Class_RTCSessionDescription, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 添加 ICE 候选项
     *     
     *      此方法将 ICE 候选项添加到连接的远程端。该方法采用单个参数（ICE 候选项），并返回一个 Promise ，一旦候选项发生异步更改，该值就会得到满足。
     * 
     *      @param candidate ICE 候选项
     *     
     */
    addIceCandidate(candidate: Class_RTCIceCandidate): void;

    addIceCandidate(candidate: Class_RTCIceCandidate, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 创建一个 Offer 描述
     *     
     *      此方法创建一个 Offer 描述，用于发起连接。该方法采用一个可选参数（选项对象），并返回一个 Promise ，一旦描述发生异步更改，该值就会得到满足。
     * 
     *      @param options 选项对象，目前暂未支持，仅用于兼容
     *      @return 返回描述对象
     *     
     */
    createOffer(options?: FIBJS.GeneralObject): any;

    createOffer(options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 创建一个 Answer 描述
     *     
     *      此方法创建一个 Answer 描述，用于应答连接。该方法采用一个可选参数（选项对象），并返回一个 Promise ，一旦描述发生异步更改，该值就会得到满足。
     * 
     *      @param options 选项对象，目前暂未支持，仅用于兼容
     *      @return 返回描述对象
     *     
     */
    createAnswer(options?: FIBJS.GeneralObject): any;

    createAnswer(options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 获取连接的统计信息
     *     
     *      此方法获取连接的统计信息，返回一个 Promise ，一旦统计信息准备好，该值就会得到满足。
     * 
     *      @return 返回统计信息
     *     
     */
    getStats(): FIBJS.GeneralObject;

    getStats(callback: (err: Error | undefined | null, retVal: FIBJS.GeneralObject)=>any): void;

    /**
     * @description 关闭连接，此方法关闭连接，释放所有资源 
     */
    close(): void;

    /**
     * @description 获取连接状态，返回连接状态字符串，可能的值有：'new'、'connecting'、'connected'、'disconnected'、'failed'、'closed' 
     */
    readonly connectionState: string;

    /**
     * @description 获取 ICE 连接状态，返回 ICE 连接状态字符串，可能的值有：'new'、'checking'、'connected'、'completed'、'failed'、'disconnected'、'closed' 
     */
    readonly iceConnectionState: string;

    /**
     * @description 获取 ICE 收集状态，返回 ICE 收集状态字符串，可能的值有：'new'、'gathering'、'complete' 
     */
    readonly iceGatheringState: string;

    /**
     * @description 获取本地描述，返回本地描述对象 
     */
    readonly localDescription: FIBJS.GeneralObject;

    /**
     * @description 获取远程描述，返回远程描述对象 
     */
    readonly remoteDescription: FIBJS.GeneralObject;

    /**
     * @description 获取信令状态，返回信令状态字符串，可能的值有：'stable'、'have-local-offer'、'have-remote-offer'、'have-local-pranswer'、'have-remote-pranswer'、'closed' 
     */
    readonly signalingState: string;

    /**
     * @description 连接状态变更事件 
     */
    onconnectionstatechange: (...args: any[])=>any;

    /**
     * @description 数据通道事件 
     */
    ondatachannel: (...args: any[])=>any;

    /**
     * @description ICE 候选项事件 
     */
    onicecandidate: (...args: any[])=>any;

    /**
     * @description ICE 连接状态变更事件 
     */
    oniceconnectionstatechange: (...args: any[])=>any;

    /**
     * @description ICE 收集状态变更事件 
     */
    onicegatheringstatechange: (...args: any[])=>any;

    /**
     * @description 本地描述变更事件 
     */
    onlocaldescription: (...args: any[])=>any;

    /**
     * @description 信令状态变更事件 
     */
    onsignalingstatechange: (...args: any[])=>any;

    /**
     * @description 媒体轨道事件 
     */
    ontrack: (...args: any[])=>any;

}

