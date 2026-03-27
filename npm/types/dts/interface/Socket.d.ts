/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 网络套接口对象
 * 
 *  Socket 属于 net 模块，创建方法
 *  ```JavaScript
 *  var s = new net.Socket();
 *  ```
 *  
 */
declare class Class_Socket extends Class_Stream {
    /**
     * @description Socket 构造函数，创建一个新的 Socket 对象
     *      @param family 指定地址集，缺省为 AF_INET，ipv4
     *      
     */
    constructor(family: number);

    /**
     * @description 查询当前 Socket 对象的地址集 
     */
    readonly family: number;

    /**
     * @description 查询当前连接的对方地址 
     */
    readonly remoteAddress: string;

    /**
     * @description 查询当前连接的对方端口 
     */
    readonly remotePort: number;

    /**
     * @description 查询当前连接的本地地址 
     */
    readonly localAddress: string;

    /**
     * @description 查询当前连接的本地端口 
     */
    readonly localPort: number;

    /**
     * @description 查询和设置超时时间 单位毫秒
     */
    timeout: number;

    /**
     * @description 建立一个 tcp 连接
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(port: number, host?: string, timeout?: number): Class_Stream;

    connect(port: number, host?: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 建立一个 tcp 连接
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接的 Socket 对象
     *      
     */
    connectSync(port: number, host?: string, timeout?: number): Class_Stream;

    /**
     * @description 建立一个 tcp 连接
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接的 Socket 对象
     *      
     */
    connectAsync(port: number, host?: string, timeout?: number): Promise<Class_Stream>;

    /**
     * @description 建立一个 unix socket 或 Windows pipe 连接
     *      @param path 指定 unix socket 或 Windows pipe 路径
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(path: string, timeout?: number): Class_Stream;

    connect(path: string, timeout?: number, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 建立一个 unix socket 或 Windows pipe 连接
     *      @param path 指定 unix socket 或 Windows pipe 路径
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接的 Socket 对象
     *      
     */
    connectSync(path: string, timeout?: number): Class_Stream;

    /**
     * @description 建立一个 unix socket 或 Windows pipe 连接
     *      @param path 指定 unix socket 或 Windows pipe 路径
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @return 返回连接的 Socket 对象
     *      
     */
    connectAsync(path: string, timeout?: number): Promise<Class_Stream>;

    /**
     * @description 建立一个连接
     * 
     *      options 参数可以包含以下属性：
     *       - port: 指定对方端口
     *       - host: 指定对方地址或主机名
     *       - timeout: 指定超时时间，单位是毫秒，默认为 0
     * 
     *      @param options 指定连接选项对象，可以包含以下属性：
     *      @return 返回连接的 Socket 对象
     *     
     */
    connect(options: FIBJS.GeneralObject): Class_Stream;

    connect(options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description 建立一个连接
     * 
     *      options 参数可以包含以下属性：
     *       - port: 指定对方端口
     *       - host: 指定对方地址或主机名
     *       - timeout: 指定超时时间，单位是毫秒，默认为 0
     * 
     *      @param options 指定连接选项对象，可以包含以下属性：
     *      @return 返回连接的 Socket 对象
     *     
     */
    connectSync(options: FIBJS.GeneralObject): Class_Stream;

    /**
     * @description 建立一个连接
     * 
     *      options 参数可以包含以下属性：
     *       - port: 指定对方端口
     *       - host: 指定对方地址或主机名
     *       - timeout: 指定超时时间，单位是毫秒，默认为 0
     * 
     *      @param options 指定连接选项对象，可以包含以下属性：
     *      @return 返回连接的 Socket 对象
     *     
     */
    connectAsync(options: FIBJS.GeneralObject): Promise<Class_Stream>;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param port 指定对方端口
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(port: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(port: number, host: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param port 指定对方端口
     *      @param host 指定对方地址或主机名，缺省为 localhost
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(port: number, host: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param path 指定 unix socket 或 Windows pipe 路径
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(path: string, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param path 指定 unix socket 或 Windows pipe 路径
     *      @param timeout 指定超时时间，单位是毫秒，默认为 0
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(path: string, timeout: number, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 建立一个连接，并在连接建立后触发 connect 事件
     *      @param options 指定连接选项对象，可以包含以下属性：
     *      @param connectListener 指定 once 的 connect 事件监听器
     *      @return 返回连接的 Socket 对象
     *      
     */
    connect(options: FIBJS.GeneralObject, connectListener: (...args: any[])=>any): Class_Stream;

    /**
     * @description 将当前 Socket 绑定至本地所有地址的指定端口
     *      @param port 指定绑定的端口
     *      @param allowIPv4 指定是否接受 ipv4 连接，缺省为 true。本参数在 ipv6 时有效，并依赖于操作系统
     *      
     */
    bind(port: number, allowIPv4?: boolean): void;

    /**
     * @description 将当前 Socket 绑定至指定地址的指定端口
     *      @param addr 指定绑定的地址，也可以指向 unix socket 和 Windows pipe 路径
     *      @param port 指定绑定的端口，绑定 unix socket 和 Windows pipe 时，忽略此参数
     *      @param allowIPv4 指定是否接受 ipv4 连接，缺省为 true。本参数在 ipv6 时有效，并依赖于操作系统
     *      
     */
    bind(addr: string, port?: number, allowIPv4?: boolean): void;

    /**
     * @description 开始监听连接请求
     *      @param backlog 指定请求队列长度，超出的请求将被拒绝，缺省为 120
     *      
     */
    listen(backlog?: number): void;

    /**
     * @description 等待并接受一个连接
     *      @return 返回接收到得连接对象
     *      
     */
    accept(): Class_Socket;

    accept(callback: (err: Error | undefined | null, retVal: Class_Socket)=>any): void;

    /**
     * @description 等待并接受一个连接
     *      @return 返回接收到得连接对象
     *      
     */
    acceptSync(): Class_Socket;

    /**
     * @description 等待并接受一个连接
     *      @return 返回接收到得连接对象
     *      
     */
    acceptAsync(): Promise<Class_Socket>;

    /**
     * @description 启用或禁用 TCP 保持活动机制
     *      @param enable 指定是否启用保持活动机制，缺省为 false
     *      @param initialDelay 指定初始延迟时间，单位为秒，缺省为 0
     *      
     */
    setKeepAlive(enable?: boolean, initialDelay?: number): void;

    /**
     * @description 启用或禁用 Nagle 算法
     *      @param noDelay 指定是否禁用 Nagle 算法，缺省为 true
     *      
     */
    setNoDelay(noDelay?: boolean): void;

    /**
     * @description 从连接读取指定大小的数据，不同于 read 方法，recv 并不保证读完要求的数据，而是在读取到数据后立即返回
     *      @param bytes 指定要读取的数据量，缺省读取任意尺寸的数据
     *      @return 返回从连接读取的数据
     *      
     */
    recv(bytes?: number): Class_Buffer;

    recv(bytes?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 从连接读取指定大小的数据，不同于 read 方法，recv 并不保证读完要求的数据，而是在读取到数据后立即返回
     *      @param bytes 指定要读取的数据量，缺省读取任意尺寸的数据
     *      @return 返回从连接读取的数据
     *      
     */
    recvSync(bytes?: number): Class_Buffer;

    /**
     * @description 从连接读取指定大小的数据，不同于 read 方法，recv 并不保证读完要求的数据，而是在读取到数据后立即返回
     *      @param bytes 指定要读取的数据量，缺省读取任意尺寸的数据
     *      @return 返回从连接读取的数据
     *      
     */
    recvAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description 将给定的数据写入连接，此方法等效于 write 方法
     *      @param data 给定要写入的数据
     *      @return 返回实际写入的字节数
     *      
     */
    send(data: Class_Buffer): number;

    send(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将给定的数据写入连接，此方法等效于 write 方法
     *      @param data 给定要写入的数据
     *      @return 返回实际写入的字节数
     *      
     */
    sendSync(data: Class_Buffer): number;

    /**
     * @description 将给定的数据写入连接，此方法等效于 write 方法
     *      @param data 给定要写入的数据
     *      @return 返回实际写入的字节数
     *      
     */
    sendAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description 中止当前 socket 上所有正在进行的操作
     *      
     *      此方法会取消所有正在等待的异步操作（connect, recv, send 等），
     *      被取消的操作会返回错误。socket 本身不会被关闭，可以继续使用。
     *      
     */
    abort(): void;

}

