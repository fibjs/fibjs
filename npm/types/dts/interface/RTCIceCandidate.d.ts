/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description WebRTC ICE 候选参数对象 
 */
declare class Class_RTCIceCandidate extends Class_object {
    /**
     * @description 构造函数
     * 
     *      description 初始化参数，支持以下字段：
     *         - candidate: 候选项字符串
     *         - sdpMid: 媒体流标识
     * 
     *       @param description 初始化参数
     *     
     */
    constructor(description?: FIBJS.GeneralObject);

    /**
     * @description 返回候选项字符串 
     */
    readonly candidate: string;

    /**
     * @description 返回媒体流标识 
     */
    readonly sdpMid: string;

    /**
     * @description 返回优先级 
     */
    readonly priority: number;

    /**
     * @description 返回传输协议 
     */
    readonly transport: string;

    /**
     * @description 返回地址 
     */
    readonly address: string;

    /**
     * @description 返回端口 
     */
    readonly port: number;

    /**
     * @description 返回类型 
     */
    readonly type: string;

}

