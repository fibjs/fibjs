/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/**
 * @description http 请求消息对象 
 */
declare class Class_HttpRequest extends Class_HttpMessage {
    /**
     * @description HttpRequest 构造函数，创建一个新的 HttpRequest 对象 
     */
    constructor();

    /**
     * @description 获取响应消息对象
     */
    readonly response: Class_HttpResponse;

    /**
     * @description 查询和设置请求方法 
     */
    method: string;

    /**
     * @description 查询和设置请求地址 
     */
    address: string;

    /**
     * @description 查询和设置请求查询字符串 
     */
    queryString: string;

    /**
     * @description 获取包含消息 cookies 的容器
     */
    readonly cookies: Class_HttpCollection;

    /**
     * @description 获取包含消息 form 的容器
     */
    readonly form: Class_HttpCollection;

    /**
     * @description 获取包含消息 query 的容器
     */
    readonly query: Class_HttpCollection;

}

