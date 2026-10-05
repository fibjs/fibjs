/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/RTCDataChannel.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
/**
 * @description RTCPeerConnection defines the methods and properties of a WebRTC connection
 *
 * RTCPeerConnection is the core object of WebRTC connections, used to create WebRTC connections, manage connection states, and send and receive media data, etc.
 *
 * The RTCPeerConnection object is created as follows:
 * ```JavaScript
 * const rtc = require('rtc');
 * var pc = new rtc.RTCPeerConnection();
 * ```
 *
 */
declare class Class_RTCPeerConnection extends Class_EventEmitter {
    /**
     * @description constructs a new WebRTC connection object and initializes the basic parameters
     *
     *      The options parameter is an object containing the following properties:
     *         - certificateType: certificate type, optional values are 'rsa', 'ecdsa', default is 'ecdsa'
     *         - iceTransportPolicy: ICE transport policy, optional values are 'all', 'relay', default is 'all'
     *         - iceServers: list of ICE servers used for NAT traversal, in the format [{urls: 'stun:stun.l.google.com:19302'}]
     *         - maxMessageSize: maximum message size, used to specify the maximum message size of the data channel
     *         - enableIceUdpMux: whether to enable ICE UDP multiplexing
     *         - disableFingerprintVerification: whether to disable fingerprint verification
     *         - bindAddress: binding address, used to specify the local IP address
     *         - port: local port number, used to specify the local port
     *         - iceUfrag: ICE username
     *         - icePwd: ICE password
     *         - certPem: certificate in PEM format
     *         - keyPem: private key in PEM format
     *         - keyPass: private key passphrase
     *
     *      @param options initialization parameters
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description creates a new channel linked to a remote peer
     *
     *      Creates a new channel linked to a remote peer through which any type of data can be transmitted. This is useful for reverse channel content such as images, file transfers, text chat, game update packets, etc.
     *
     *      The options parameter is an object containing the following properties:
     *         - ordered: whether the order of packets is guaranteed, default is true
     *         - maxPacketLifeTime: maximum packet lifetime, default is 0
     *         - maxRetransmits: maximum number of packet retransmits, default is 0
     *         - protocol: channel protocol, default is ''
     *         - negotiated: whether it is a negotiated channel, default is false
     *         - id: channel ID, default is 0
     *
     *      @param label channel name
     *      @param options channel parameters
     *      @return returns the created channel object
     *
     */
    createDataChannel(label: string, options?: FIBJS.GeneralObject): Class_RTCDataChannel;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     */
    setLocalDescription(): Promise<void>;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     */
    setLocalDescriptionSync(): void;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     */
    setLocalDescriptionAsync(): Promise<void>;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setLocalDescription(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setLocalDescriptionSync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): void;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setLocalDescriptionAsync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description changes the remote description associated with the connection
     *
     *      This method specifies the properties of the remote end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setRemoteDescription(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description changes the remote description associated with the connection
     *
     *      This method specifies the properties of the remote end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setRemoteDescriptionSync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): void;

    /**
     * @description changes the remote description associated with the connection
     *
     *      This method specifies the properties of the remote end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setRemoteDescriptionAsync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description adds an ICE candidate
     *
     *      This method adds an ICE candidate to the remote end of the connection. The method takes a single parameter (ICE candidate) and returns a Promise that is fulfilled once the candidate is changed asynchronously.
     *
     * candidate may be an RTCIceCandidate object, or an options object the RTCIceCandidate constructor accepts (candidate, sdpMid, sdpMLineIndex).
     *      @param candidate the ICE candidate
     *
     */
    addIceCandidate(candidate: Class_RTCIceCandidate | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description adds an ICE candidate
     *
     *      This method adds an ICE candidate to the remote end of the connection. The method takes a single parameter (ICE candidate) and returns a Promise that is fulfilled once the candidate is changed asynchronously.
     *
     * candidate may be an RTCIceCandidate object, or an options object the RTCIceCandidate constructor accepts (candidate, sdpMid, sdpMLineIndex).
     *      @param candidate the ICE candidate
     *
     */
    addIceCandidateSync(candidate: Class_RTCIceCandidate | FIBJS.GeneralObject): void;

    /**
     * @description adds an ICE candidate
     *
     *      This method adds an ICE candidate to the remote end of the connection. The method takes a single parameter (ICE candidate) and returns a Promise that is fulfilled once the candidate is changed asynchronously.
     *
     * candidate may be an RTCIceCandidate object, or an options object the RTCIceCandidate constructor accepts (candidate, sdpMid, sdpMLineIndex).
     *      @param candidate the ICE candidate
     *
     */
    addIceCandidateAsync(candidate: Class_RTCIceCandidate | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description creates an Offer description
     *
     *      This method creates an Offer description used to initiate a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createOffer(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description creates an Offer description
     *
     *      This method creates an Offer description used to initiate a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createOfferSync(options?: FIBJS.GeneralObject): any;

    /**
     * @description creates an Offer description
     *
     *      This method creates an Offer description used to initiate a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createOfferAsync(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description creates an Answer description
     *
     *      This method creates an Answer description used to answer a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createAnswer(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description creates an Answer description
     *
     *      This method creates an Answer description used to answer a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createAnswerSync(options?: FIBJS.GeneralObject): any;

    /**
     * @description creates an Answer description
     *
     *      This method creates an Answer description used to answer a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createAnswerAsync(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description gets the statistics of the connection
     *
     *      This method gets the statistics of the connection and returns a Promise that is fulfilled once the statistics are ready.
     *
     *      @return returns the statistics
     *
     */
    getStats(): Promise<FIBJS.GeneralObject>;

    /**
     * @description gets the statistics of the connection
     *
     *      This method gets the statistics of the connection and returns a Promise that is fulfilled once the statistics are ready.
     *
     *      @return returns the statistics
     *
     */
    getStatsSync(): FIBJS.GeneralObject;

    /**
     * @description gets the statistics of the connection
     *
     *      This method gets the statistics of the connection and returns a Promise that is fulfilled once the statistics are ready.
     *
     *      @return returns the statistics
     *
     */
    getStatsAsync(): Promise<FIBJS.GeneralObject>;

    /**
     * @description closes the connection; this method closes the connection and releases all resources
     */
    close(): void;

    /**
     * @description gets the connection state, returns a connection state string, possible values are: 'new', 'connecting', 'connected', 'disconnected', 'failed', 'closed'
     */
    readonly connectionState: string;

    /**
     * @description gets the ICE connection state, returns an ICE connection state string, possible values are: 'new', 'checking', 'connected', 'completed', 'failed', 'disconnected', 'closed'
     */
    readonly iceConnectionState: string;

    /**
     * @description gets the ICE gathering state, returns an ICE gathering state string, possible values are: 'new', 'gathering', 'complete'
     */
    readonly iceGatheringState: string;

    /**
     * @description gets the local description, returns the local description object
     */
    readonly localDescription: FIBJS.GeneralObject;

    /**
     * @description gets the remote description, returns the remote description object
     */
    readonly remoteDescription: FIBJS.GeneralObject;

    /**
     * @description gets the remote fingerprint, returns the remote fingerprint object
     */
    readonly remoteFingerprint: FIBJS.GeneralObject;

    /**
     * @description gets the signaling state, returns a signaling state string, possible values are: 'stable', 'have-local-offer', 'have-remote-offer', 'have-local-pranswer', 'have-remote-pranswer', 'closed'
     */
    readonly signalingState: string;

    /**
     * @description connection state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    on(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "connectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description connection state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    onconnectionstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description data channel event
     *      @param ev the event object, carrying the data channel in its channel property
     *
     */
    on(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "datachannel", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description data channel event
     *      @param ev the event object, carrying the data channel in its channel property
     *
     */
    ondatachannel: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description ICE candidate event
     *      @param ev the event object, carrying the candidate
     *
     */
    on(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "icecandidate", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description ICE candidate event
     *      @param ev the event object, carrying the candidate
     *
     */
    onicecandidate: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description ICE connection state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    on(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "iceconnectionstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description ICE connection state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    oniceconnectionstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description ICE gathering state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    on(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "icegatheringstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description ICE gathering state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    onicegatheringstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description local description change event
     *      @param ev the event object, carrying the local description
     *
     */
    on(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "localdescription", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description local description change event
     *      @param ev the event object, carrying the local description
     *
     */
    onlocaldescription: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description signaling state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    on(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "signalingstatechange", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description signaling state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    onsignalingstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description media track event
     */
    on(event: "track", listener: ()=>void): this;

    once(event: "track", listener: ()=>void): this;

    off(event: "track", listener: ()=>void): this;

    addListener(event: "track", listener: ()=>void): this;

    removeListener(event: "track", listener: ()=>void): this;

    addEventListener(event: "track", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "track", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "track", listener: ()=>void): this;

    prependOnceListener(event: "track", listener: ()=>void): this;

    /**
     * @description media track event
     */
    ontrack: (()=>void) | null;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/RTCDataChannel.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
/**
 * The promise variant of the RTCPeerConnection class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_RTCPeerConnectionPromise extends Class_EventEmitter {
    /**
     * @description constructs a new WebRTC connection object and initializes the basic parameters
     *
     *      The options parameter is an object containing the following properties:
     *         - certificateType: certificate type, optional values are 'rsa', 'ecdsa', default is 'ecdsa'
     *         - iceTransportPolicy: ICE transport policy, optional values are 'all', 'relay', default is 'all'
     *         - iceServers: list of ICE servers used for NAT traversal, in the format [{urls: 'stun:stun.l.google.com:19302'}]
     *         - maxMessageSize: maximum message size, used to specify the maximum message size of the data channel
     *         - enableIceUdpMux: whether to enable ICE UDP multiplexing
     *         - disableFingerprintVerification: whether to disable fingerprint verification
     *         - bindAddress: binding address, used to specify the local IP address
     *         - port: local port number, used to specify the local port
     *         - iceUfrag: ICE username
     *         - icePwd: ICE password
     *         - certPem: certificate in PEM format
     *         - keyPem: private key in PEM format
     *         - keyPass: private key passphrase
     *
     *      @param options initialization parameters
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description creates a new channel linked to a remote peer
     *
     *      Creates a new channel linked to a remote peer through which any type of data can be transmitted. This is useful for reverse channel content such as images, file transfers, text chat, game update packets, etc.
     *
     *      The options parameter is an object containing the following properties:
     *         - ordered: whether the order of packets is guaranteed, default is true
     *         - maxPacketLifeTime: maximum packet lifetime, default is 0
     *         - maxRetransmits: maximum number of packet retransmits, default is 0
     *         - protocol: channel protocol, default is ''
     *         - negotiated: whether it is a negotiated channel, default is false
     *         - id: channel ID, default is 0
     *
     *      @param label channel name
     *      @param options channel parameters
     *      @return returns the created channel object
     *
     */
    createDataChannel(label: string, options?: FIBJS.GeneralObject): Class_RTCDataChannel;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     */
    setLocalDescription(): Promise<void>;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     */
    setLocalDescriptionSync(): void;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     */
    setLocalDescriptionAsync(): Promise<void>;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setLocalDescription(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setLocalDescriptionSync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): void;

    /**
     * @description changes the local description associated with the connection
     *
     *      This method specifies the properties of the local end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setLocalDescriptionAsync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description changes the remote description associated with the connection
     *
     *      This method specifies the properties of the remote end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setRemoteDescription(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description changes the remote description associated with the connection
     *
     *      This method specifies the properties of the remote end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setRemoteDescriptionSync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): void;

    /**
     * @description changes the remote description associated with the connection
     *
     *      This method specifies the properties of the remote end of the connection, including media formats. The method takes a single parameter (session description) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     * description may be an RTCSessionDescription object, or an options object the RTCSessionDescription constructor accepts (type, sdp).
     *      @param description the session description
     *
     */
    setRemoteDescriptionAsync(description: Class_RTCSessionDescription | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description adds an ICE candidate
     *
     *      This method adds an ICE candidate to the remote end of the connection. The method takes a single parameter (ICE candidate) and returns a Promise that is fulfilled once the candidate is changed asynchronously.
     *
     * candidate may be an RTCIceCandidate object, or an options object the RTCIceCandidate constructor accepts (candidate, sdpMid, sdpMLineIndex).
     *      @param candidate the ICE candidate
     *
     */
    addIceCandidate(candidate: Class_RTCIceCandidate | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description adds an ICE candidate
     *
     *      This method adds an ICE candidate to the remote end of the connection. The method takes a single parameter (ICE candidate) and returns a Promise that is fulfilled once the candidate is changed asynchronously.
     *
     * candidate may be an RTCIceCandidate object, or an options object the RTCIceCandidate constructor accepts (candidate, sdpMid, sdpMLineIndex).
     *      @param candidate the ICE candidate
     *
     */
    addIceCandidateSync(candidate: Class_RTCIceCandidate | FIBJS.GeneralObject): void;

    /**
     * @description adds an ICE candidate
     *
     *      This method adds an ICE candidate to the remote end of the connection. The method takes a single parameter (ICE candidate) and returns a Promise that is fulfilled once the candidate is changed asynchronously.
     *
     * candidate may be an RTCIceCandidate object, or an options object the RTCIceCandidate constructor accepts (candidate, sdpMid, sdpMLineIndex).
     *      @param candidate the ICE candidate
     *
     */
    addIceCandidateAsync(candidate: Class_RTCIceCandidate | FIBJS.GeneralObject): Promise<void>;

    /**
     * @description creates an Offer description
     *
     *      This method creates an Offer description used to initiate a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createOffer(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description creates an Offer description
     *
     *      This method creates an Offer description used to initiate a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createOfferSync(options?: FIBJS.GeneralObject): any;

    /**
     * @description creates an Offer description
     *
     *      This method creates an Offer description used to initiate a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createOfferAsync(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description creates an Answer description
     *
     *      This method creates an Answer description used to answer a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createAnswer(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description creates an Answer description
     *
     *      This method creates an Answer description used to answer a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createAnswerSync(options?: FIBJS.GeneralObject): any;

    /**
     * @description creates an Answer description
     *
     *      This method creates an Answer description used to answer a connection. The method takes an optional parameter (options object) and returns a Promise that is fulfilled once the description is changed asynchronously.
     *
     *      @param options options object, not yet supported, only for compatibility
     *      @return returns the description object
     *
     */
    createAnswerAsync(options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description gets the statistics of the connection
     *
     *      This method gets the statistics of the connection and returns a Promise that is fulfilled once the statistics are ready.
     *
     *      @return returns the statistics
     *
     */
    getStats(): Promise<FIBJS.GeneralObject>;

    /**
     * @description gets the statistics of the connection
     *
     *      This method gets the statistics of the connection and returns a Promise that is fulfilled once the statistics are ready.
     *
     *      @return returns the statistics
     *
     */
    getStatsSync(): FIBJS.GeneralObject;

    /**
     * @description gets the statistics of the connection
     *
     *      This method gets the statistics of the connection and returns a Promise that is fulfilled once the statistics are ready.
     *
     *      @return returns the statistics
     *
     */
    getStatsAsync(): Promise<FIBJS.GeneralObject>;

    /**
     * @description closes the connection; this method closes the connection and releases all resources
     */
    close(): void;

    /**
     * @description gets the connection state, returns a connection state string, possible values are: 'new', 'connecting', 'connected', 'disconnected', 'failed', 'closed'
     */
    readonly connectionState: string;

    /**
     * @description gets the ICE connection state, returns an ICE connection state string, possible values are: 'new', 'checking', 'connected', 'completed', 'failed', 'disconnected', 'closed'
     */
    readonly iceConnectionState: string;

    /**
     * @description gets the ICE gathering state, returns an ICE gathering state string, possible values are: 'new', 'gathering', 'complete'
     */
    readonly iceGatheringState: string;

    /**
     * @description gets the local description, returns the local description object
     */
    readonly localDescription: FIBJS.GeneralObject;

    /**
     * @description gets the remote description, returns the remote description object
     */
    readonly remoteDescription: FIBJS.GeneralObject;

    /**
     * @description gets the remote fingerprint, returns the remote fingerprint object
     */
    readonly remoteFingerprint: FIBJS.GeneralObject;

    /**
     * @description gets the signaling state, returns a signaling state string, possible values are: 'stable', 'have-local-offer', 'have-remote-offer', 'have-local-pranswer', 'have-remote-pranswer', 'closed'
     */
    readonly signalingState: string;

    /**
     * @description connection state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    onconnectionstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description data channel event
     *      @param ev the event object, carrying the data channel in its channel property
     *
     */
    ondatachannel: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description ICE candidate event
     *      @param ev the event object, carrying the candidate
     *
     */
    onicecandidate: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description ICE connection state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    oniceconnectionstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description ICE gathering state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    onicegatheringstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description local description change event
     *      @param ev the event object, carrying the local description
     *
     */
    onlocaldescription: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description signaling state change event
     *      @param ev the event object, carrying the new state in its state property
     *
     */
    onsignalingstatechange: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description media track event
     */
    ontrack: (()=>void) | null;

}


declare namespace Class_RTCPeerConnection {
    const promises: FIBJS.GeneralObject;
}
