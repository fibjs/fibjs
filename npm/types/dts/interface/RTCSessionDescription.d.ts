/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description WebRTC 会话描述对象 
 */
declare class Class_RTCSessionDescription extends Class_object {
    /**
     * @description 构造函数
     * 
     *      description 初始化参数，支持以下字段：
     *         - type: 描述类型
     *         - sdp: 描述字符串
     * 
     *       @param description 初始化参数
     *     
     */
    constructor(description?: FIBJS.GeneralObject);

    /**
     * @description 返回描述类型 
     */
    readonly type: string;

    /**
     * @description 返回描述字符串 
     */
    readonly sdp: string;

}

