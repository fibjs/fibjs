/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description WebRTC ICE candidate parameter object
 */
declare class Class_RTCIceCandidate extends Class_object {
    /**
     * @description constructor
     *
     *      description is the initialization parameter, supporting the following fields:
     *         - candidate: candidate string
     *         - sdpMid: media stream identification
     *
     *       @param description initialization parameter
     *
     */
    constructor(description?: FIBJS.GeneralObject);

    /**
     * @description returns the candidate string
     */
    readonly candidate: string;

    /**
     * @description returns the media stream identification
     */
    readonly sdpMid: string;

    /**
     * @description returns the priority
     */
    readonly priority: number;

    /**
     * @description returns the transport protocol
     */
    readonly transport: string;

    /**
     * @description returns the address
     */
    readonly address: string;

    /**
     * @description returns the port
     */
    readonly port: number;

    /**
     * @description returns the type
     */
    readonly type: string;

}

