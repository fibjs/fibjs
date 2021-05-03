/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description Smtp对象
 * 
 *  
 */
declare class Class_Smtp extends Class_object {
    /**
     * @description Smtp 对象构造函数 
     */
    constructor();

    /**
     * @description 建立到指定的服务器
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port
     *      
     */
    connect(): void;

    /**
     * @description 发送指定命令，并返回响应，服务器报错则抛出错误
     *      @param cmd 命令名
     *      @param arg 参数
     *      @return 如果成功，返回服务器响应
     *      
     */
    command(): string;

    /**
     * @description 发送 HELO 命令，服务器报错则抛出错误
     *      @param hostname 主机名，缺省为“localhost”
     *      
     */
    hello(): void;

    /**
     * @description 用指定的用户及密码登录服务器，服务器报错则抛出错误
     *      @param username 用户名
     *      @param password 密码
     *      
     */
    login(): void;

    /**
     * @description 指定发件人信箱，服务器报错则抛出错误
     *      @param address 发件人信箱
     *      
     */
    from(): void;

    /**
     * @description 指定收件人信箱，服务器报错则抛出错误
     *      @param address 收件人信箱
     *      
     */
    to(): void;

    /**
     * @description 发送文本到收件人，服务器报错则抛出错误
     *      @param txt 要发送的文本
     *      
     */
    data(): void;

    /**
     * @description 退出并关闭连接，服务器报错则抛出错误 
     */
    quit(): void;

    /**
     * @description 查询和设置超时时间 单位毫秒
     */
    timeout: number;

    /**
     * @description 查询 Smtp 对象当前连接的 Socket 
     */
    readonly socket: Class_Stream;

}

