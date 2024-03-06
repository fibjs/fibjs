/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
declare class Class_TLSSocket extends Class_Stream {
    constructor();

    constructor(context: Class_SecureContext);

    connect(socket: Class_Stream): void;

    connect(socket: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    accept(socket: Class_Stream): void;

    accept(socket: Class_Stream, callback: (err: Error | undefined | null)=>any): void;

    getProtocol(): string;

    getX509Certificate(): Class_X509Certificate;

    getPeerX509Certificate(): Class_X509Certificate;

    readonly secureContext: Class_SecureContext;

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

}

