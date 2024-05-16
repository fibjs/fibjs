/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/RTCPeerConnection.d.ts" />
/// <reference path="../interface/RTCSessionDescription.d.ts" />
/// <reference path="../interface/RTCIceCandidate.d.ts" />
/**
 * @description WebRTC 网络实时通信模块 
 */
declare module 'rtc' {
    const RTCPeerConnection: typeof Class_RTCPeerConnection;

    const RTCSessionDescription: typeof Class_RTCSessionDescription;

    const RTCIceCandidate: typeof Class_RTCIceCandidate;

    function bind(bind_address: string, local_port: number, cb: (...args: any[])=>any): void;

    function bind(local_port: number, cb: (...args: any[])=>any): void;

}

