/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description WebRTC session description object
 */
declare class Class_RTCSessionDescription extends Class_object {
    /**
     * @description constructor
     *
     *      description is the initialization parameter, supporting the following fields:
     *         - type: description type
     *         - sdp: description string
     *
     *       @param description initialization parameter
     *
     */
    constructor(description?: FIBJS.GeneralObject);

    /**
     * @description returns the description type
     */
    readonly type: string;

    /**
     * @description returns the description string
     */
    readonly sdp: string;

}

