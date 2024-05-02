/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/RTCDataChannel.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
declare class Class_RTCPeerConnection extends Class_EventEmitter {
    constructor(options?: FIBJS.GeneralObject);

    createDataChannel(label: string, options?: FIBJS.GeneralObject): Class_RTCDataChannel;

    setLocalDescription(): void;

    setLocalDescription(callback: (err: Error | undefined | null)=>any): void;

    setLocalDescription(description: Class_RTCSessionDescription): void;

    setLocalDescription(description: Class_RTCSessionDescription, callback: (err: Error | undefined | null)=>any): void;

    setRemoteDescription(description: Class_RTCSessionDescription): void;

    setRemoteDescription(description: Class_RTCSessionDescription, callback: (err: Error | undefined | null)=>any): void;

    addIceCandidate(candidate: Class_RTCIceCandidate): void;

    addIceCandidate(candidate: Class_RTCIceCandidate, callback: (err: Error | undefined | null)=>any): void;

    createOffer(options?: FIBJS.GeneralObject): any;

    createOffer(options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: any)=>any): void;

    createAnswer(options?: FIBJS.GeneralObject): any;

    createAnswer(options?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: any)=>any): void;

    getStats(): FIBJS.GeneralObject;

    getStats(callback: (err: Error | undefined | null, retVal: FIBJS.GeneralObject)=>any): void;

    close(): void;

    readonly connectionState: string;

    readonly iceConnectionState: string;

    readonly iceGatheringState: string;

    readonly localDescription: FIBJS.GeneralObject;

    readonly remoteDescription: FIBJS.GeneralObject;

    readonly signalingState: string;

    onconnectionstatechange: (...args: any[])=>any;

    ondatachannel: (...args: any[])=>any;

    onicecandidate: (...args: any[])=>any;

    oniceconnectionstatechange: (...args: any[])=>any;

    onicegatheringstatechange: (...args: any[])=>any;

    onlocaldescription: (...args: any[])=>any;

    onsignalingstatechange: (...args: any[])=>any;

    ontrack: (...args: any[])=>any;

}

