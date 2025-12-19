/// <reference path="../_import/_fibjs.d.ts" />
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
    connect(url: string): void;

    connect(url: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 建立到指定的服务器
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port
     *      
     */
    connectSync(url: string): void;

    /**
     * @description 建立到指定的服务器
     *      @param url 指定连接的协议，可以是：tcp://host:port 或者 ssl://host:port
     *      
     */
    connectAsync(url: string): Promise<void>;

    /**
     * @description 发送指定命令，并返回响应，服务器报错则抛出错误
     *      @param cmd 命令名
     *      @param arg 参数
     *      @return 如果成功，返回服务器响应
     *      
     */
    command(cmd: string, arg: string): string;

    command(cmd: string, arg: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 发送指定命令，并返回响应，服务器报错则抛出错误
     *      @param cmd 命令名
     *      @param arg 参数
     *      @return 如果成功，返回服务器响应
     *      
     */
    commandSync(cmd: string, arg: string): string;

    /**
     * @description 发送指定命令，并返回响应，服务器报错则抛出错误
     *      @param cmd 命令名
     *      @param arg 参数
     *      @return 如果成功，返回服务器响应
     *      
     */
    commandAsync(cmd: string, arg: string): Promise<string>;

    /**
     * @description 发送 HELO 命令，服务器报错则抛出错误
     *      @param hostname 主机名，缺省为“localhost”
     *      
     */
    hello(hostname?: string): void;

    hello(hostname?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 发送 HELO 命令，服务器报错则抛出错误
     *      @param hostname 主机名，缺省为“localhost”
     *      
     */
    helloSync(hostname?: string): void;

    /**
     * @description 发送 HELO 命令，服务器报错则抛出错误
     *      @param hostname 主机名，缺省为“localhost”
     *      
     */
    helloAsync(hostname?: string): Promise<void>;

    /**
     * @description 用指定的用户及密码登录服务器，服务器报错则抛出错误
     *      @param username 用户名
     *      @param password 密码
     *      
     */
    login(username: string, password: string): void;

    login(username: string, password: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 用指定的用户及密码登录服务器，服务器报错则抛出错误
     *      @param username 用户名
     *      @param password 密码
     *      
     */
    loginSync(username: string, password: string): void;

    /**
     * @description 用指定的用户及密码登录服务器，服务器报错则抛出错误
     *      @param username 用户名
     *      @param password 密码
     *      
     */
    loginAsync(username: string, password: string): Promise<void>;

    /**
     * @description 指定发件人信箱，服务器报错则抛出错误
     *      @param address 发件人信箱
     *      
     */
    from(address: string): void;

    from(address: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 指定发件人信箱，服务器报错则抛出错误
     *      @param address 发件人信箱
     *      
     */
    fromSync(address: string): void;

    /**
     * @description 指定发件人信箱，服务器报错则抛出错误
     *      @param address 发件人信箱
     *      
     */
    fromAsync(address: string): Promise<void>;

    /**
     * @description 指定收件人信箱，服务器报错则抛出错误
     *      @param address 收件人信箱
     *      
     */
    to(address: string): void;

    to(address: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 指定收件人信箱，服务器报错则抛出错误
     *      @param address 收件人信箱
     *      
     */
    toSync(address: string): void;

    /**
     * @description 指定收件人信箱，服务器报错则抛出错误
     *      @param address 收件人信箱
     *      
     */
    toAsync(address: string): Promise<void>;

    /**
     * @description 发送文本到收件人，服务器报错则抛出错误
     *      @param txt 要发送的文本
     *      
     */
    data(txt: string): void;

    data(txt: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 发送文本到收件人，服务器报错则抛出错误
     *      @param txt 要发送的文本
     *      
     */
    dataSync(txt: string): void;

    /**
     * @description 发送文本到收件人，服务器报错则抛出错误
     *      @param txt 要发送的文本
     *      
     */
    dataAsync(txt: string): Promise<void>;

    /**
     * @description 退出并关闭连接，服务器报错则抛出错误 
     */
    quit(): void;

    quit(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 退出并关闭连接，服务器报错则抛出错误 
     */
    quitSync(): void;

    /**
     * @description 退出并关闭连接，服务器报错则抛出错误 
     */
    quitAsync(): Promise<void>;

    /**
     * @description 查询和设置超时时间 单位毫秒
     */
    timeout: number;

    /**
     * @description 查询 Smtp 对象当前连接的 Socket 
     */
    readonly socket: Class_Stream;

}

