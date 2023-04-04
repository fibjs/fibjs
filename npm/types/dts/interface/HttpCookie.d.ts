/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description HttpCookie 是 HTTP 协议封装的 cookie 对象，它提供了获取、设置 cookie 的各个属性，同时也支持多个 cookie 的组织与处理，是 http.Request 和 http.Response 两个对象都支持的一个重要属性
 * 
 * HttpCookie 对象有一些重要的属性，其中最重要的是 name，value 和 expires。其中，name 和 value 属性是必须设定的，name 属性为 cookie 的名称，value 属性为 cookie 的值。
 * 
 * ```JavaScript
 * const http = require('http');
 * 
 * const cookie = new http.Cookie('my_cookie_name', 'my_cookie_value');
 * console.log(cookie.name, cookie.value); // my_cookie_name my_cookie_value
 * ```
 * 
 * 在上述的示例代码中，我们创建了一个名为 "my_cookie_name" 值为 "my_cookie_value" 的 cookie 对象，并使用 console.log 打印 cookie 对象的 name 和 value 属性的值。
 * 
 * 除此之外，HttpCookie 对象还有 expires、domain、path、secure、httpOnly 等属性，可以指定 cookie 过期时间、访问路径、可访问域名、安全等级，cookie 仅允许 http 请求等等。
 * 
 * ```JavaScript
 * const http = require('http');
 * 
 * const now = new Date();
 * const date = new Date(
 *   now.getFullYear(),
 *   now.getMonth(),
 *   now.getDate() + 1
 * );
 * const options = {
 *   name: 'my_cookie_name',
 *   value: 'my_cookie_value',
 *   expires: date,
 *   domain: 'localhost',
 *   path: '/',
 *   secure: true,
 *   httpOnly: true
 * };
 * const cookie = new http.Cookie(options);
 * console.log(cookie);
 * ```
 * 
 * 在上述示例代码中，我们创建了一个 cookie 对象，并指定了 cookie 的各种属性。我们先实例化了一个 Date 类型的变量 now，然后新建了一个 date 变量，它的值为明天这个时间点。options 是一个 JS 对象，其中包含了很多关键的属性，比如过期时间 expires、cookie 名称和值 name 和 value、cookie 生效域名 domain、cookie 生效路径 path，另有 cookie 安全选项 secure 和 http-only 两个属性。
 * 
 * 在实际开发场景中，我们需要移除一个 cookie，可以将其过期时间设置为一个过去的时间，或者直接从 cookies 属性中删除。如：
 * 
 * ```JavaScript
 * const http = require('http');
 * 
 * const cookie = new http.Cookie('my_cookie_name', 'my_cookie_value');
 * delete cookie.server; //移除指定 httponly cookie
 * cookie.expires = -1; // 将过期时间改为已过期时间，浏览器会自动删除该 cookie
 * ``` 
 * 
 * 在上述示例代码中，我们使用了 delete 语句，移除了 cookie 对象的 server 属性（HTTP only cookie）。次外，为了移除最近添加的 cookie，我们将 expires 属性的值设置为 -1。
 *  
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

