/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
declare class Class_RTCIceCandidate extends Class_object {
    constructor(description?: FIBJS.GeneralObject);

    readonly candidate: string;

    readonly sdpMid: string;

    readonly priority: number;

    readonly transport: string;

    readonly address: string;

    readonly port: number;

    readonly type: string;

}

