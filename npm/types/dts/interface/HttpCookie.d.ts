/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description http Cookie 对象，用于添加和处理 cookie 
 */
declare class Class_HttpCookie extends Class_object {
    /**
     * @description HttpCookie 构造函数，创建一个新的 HttpCookie 对象
     * 
     *      opts 可以设置的选项如下：
     *      ```JavaScript
     *      {
     *          "name": "", // 指定创建的 cookie 名称
     *          "value": "", // 指定创建的 cookie 值
     *          "expires": Date, // 指定创建的 cookie 过期时间
     *          "domain": "", // 指定创建的 cookie 的域名范围
     *          "path": "", // 指定创建的 cookie 的路径范围
     *          "secure": false, // 指定创建的 cookie 是否仅通过 https 传递
     *          "httpOnly": false, // 指定创建的 cookie 仅允许 http 请求
     *      }
     *      ```
     *      @param opts 指定创建的 cookie 的属性
     *     
     */
    constructor(opts?: FIBJS.GeneralObject);

    /**
     * @description HttpCookie 构造函数，创建一个新的 HttpCookie 对象
     * 
     *      opts 可以设置的选项如下：
     *      ```JavaScript
     *      {
     *          "expires": Date, // 指定创建的 cookie 过期时间
     *          "domain": "", // 指定创建的 cookie 的域名范围
     *          "path": "", // 指定创建的 cookie 的路径范围
     *          "secure": false, // 指定创建的 cookie 是否仅通过 https 传递
     *          "httpOnly": false, // 指定创建的 cookie 仅允许 http 请求
     *      }
     *      ```
     *      @param name 指定创建的 cookie 名称
     *      @param value 指定创建的 cookie 值
     *      @param opts 指定创建的 cookie 的其它属性
     *     
     */
    constructor(name: string, value: string, opts?: FIBJS.GeneralObject);

    /**
     * @description 解析给定的字符串，填充 cookie 对象
     *      @param header 指定需要解析的 header 字符串
     *     
     */
    parse(header: string): void;

    /**
     * @description 检测给定的 url 是否匹配当前设置
     *      @param url 指定测试的 url
     *      @return 匹配成功返回 true
     *     
     */
    match(url: string): boolean;

    /**
     * @description 查询和设置 cookie 名称 
     */
    name: string;

    /**
     * @description 查询和设置 cookie 的值 
     */
    value: string;

    /**
     * @description 查询和设置 cookie 的域名范围 
     */
    domain: string;

    /**
     * @description 查询和设置 cookie 的路径范围 
     */
    path: string;

    /**
     * @description 查询和设置 cookie 的过期时间 
     */
    expires: typeof Date;

    /**
     * @description 查询和设置 cookie 是否仅允许 http 请求，缺省 false 
     */
    httpOnly: boolean;

    /**
     * @description 查询和设置 cookie 是否仅通过 https 传递，缺省 false 
     */
    secure: boolean;

}

