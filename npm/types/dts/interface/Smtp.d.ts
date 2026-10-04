/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * @description Smtp object
 *
 *
 */
declare class Class_Smtp extends Class_object {
    /**
     * @description Smtp object constructor
     */
    constructor();

    /**
     * @description Establishes a connection to the specified server
     *      @param url the connection protocol, which can be: tcp://host:port or ssl://host:port
     *
     */
    connect(url: string): void;

    connect(url: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Establishes a connection to the specified server
     *      @param url the connection protocol, which can be: tcp://host:port or ssl://host:port
     *
     */
    connectSync(url: string): void;

    /**
     * @description Establishes a connection to the specified server
     *      @param url the connection protocol, which can be: tcp://host:port or ssl://host:port
     *
     */
    connectAsync(url: string): Promise<void>;

    /**
     * @description Sends the specified command and returns the response; throws an error if the server reports an error
     *      @param cmd command name
     *      @param arg parameter
     *      @return returns the server response on success
     *
     */
    command(cmd: string, arg: string): string;

    command(cmd: string, arg: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Sends the specified command and returns the response; throws an error if the server reports an error
     *      @param cmd command name
     *      @param arg parameter
     *      @return returns the server response on success
     *
     */
    commandSync(cmd: string, arg: string): string;

    /**
     * @description Sends the specified command and returns the response; throws an error if the server reports an error
     *      @param cmd command name
     *      @param arg parameter
     *      @return returns the server response on success
     *
     */
    commandAsync(cmd: string, arg: string): Promise<string>;

    /**
     * @description Sends the HELO command; throws an error if the server reports an error
     *      @param hostname host name, default is "localhost"
     *
     */
    hello(hostname?: string): void;

    hello(hostname?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sends the HELO command; throws an error if the server reports an error
     *      @param hostname host name, default is "localhost"
     *
     */
    helloSync(hostname?: string): void;

    /**
     * @description Sends the HELO command; throws an error if the server reports an error
     *      @param hostname host name, default is "localhost"
     *
     */
    helloAsync(hostname?: string): Promise<void>;

    /**
     * @description Logs in to the server with the specified user and password; throws an error if the server reports an error
     *      @param username user name
     *      @param password password
     *
     */
    login(username: string, password: string): void;

    login(username: string, password: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Logs in to the server with the specified user and password; throws an error if the server reports an error
     *      @param username user name
     *      @param password password
     *
     */
    loginSync(username: string, password: string): void;

    /**
     * @description Logs in to the server with the specified user and password; throws an error if the server reports an error
     *      @param username user name
     *      @param password password
     *
     */
    loginAsync(username: string, password: string): Promise<void>;

    /**
     * @description Specifies the sender mailbox; throws an error if the server reports an error
     *      @param address sender mailbox
     *
     */
    from(address: string): void;

    from(address: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Specifies the sender mailbox; throws an error if the server reports an error
     *      @param address sender mailbox
     *
     */
    fromSync(address: string): void;

    /**
     * @description Specifies the sender mailbox; throws an error if the server reports an error
     *      @param address sender mailbox
     *
     */
    fromAsync(address: string): Promise<void>;

    /**
     * @description Specifies the recipient mailbox; throws an error if the server reports an error
     *      @param address recipient mailbox
     *
     */
    to(address: string): void;

    to(address: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Specifies the recipient mailbox; throws an error if the server reports an error
     *      @param address recipient mailbox
     *
     */
    toSync(address: string): void;

    /**
     * @description Specifies the recipient mailbox; throws an error if the server reports an error
     *      @param address recipient mailbox
     *
     */
    toAsync(address: string): Promise<void>;

    /**
     * @description Sends text to the recipient; throws an error if the server reports an error
     *      @param txt the text to send
     *
     */
    data(txt: string): void;

    data(txt: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sends text to the recipient; throws an error if the server reports an error
     *      @param txt the text to send
     *
     */
    dataSync(txt: string): void;

    /**
     * @description Sends text to the recipient; throws an error if the server reports an error
     *      @param txt the text to send
     *
     */
    dataAsync(txt: string): Promise<void>;

    /**
     * @description Quits and closes the connection; throws an error if the server reports an error
     */
    quit(): void;

    quit(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Quits and closes the connection; throws an error if the server reports an error
     */
    quitSync(): void;

    /**
     * @description Quits and closes the connection; throws an error if the server reports an error
     */
    quitAsync(): Promise<void>;

    /**
     * @description Queries and sets the timeout in milliseconds
     */
    timeout: number;

    /**
     * @description Queries the Socket currently connected to the Smtp object
     */
    readonly socket: Class_Stream;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the Smtp class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_SmtpPromise extends Class_object {
    /**
     * @description Smtp object constructor
     */
    constructor();

    /**
     * @description Establishes a connection to the specified server
     *      @param url the connection protocol, which can be: tcp://host:port or ssl://host:port
     *
     */
    connect(url: string): Promise<void>;

    /**
     * @description Establishes a connection to the specified server
     *      @param url the connection protocol, which can be: tcp://host:port or ssl://host:port
     *
     */
    connectSync(url: string): void;

    /**
     * @description Establishes a connection to the specified server
     *      @param url the connection protocol, which can be: tcp://host:port or ssl://host:port
     *
     */
    connectAsync(url: string): Promise<void>;

    /**
     * @description Sends the specified command and returns the response; throws an error if the server reports an error
     *      @param cmd command name
     *      @param arg parameter
     *      @return returns the server response on success
     *
     */
    command(cmd: string, arg: string): Promise<string>;

    /**
     * @description Sends the specified command and returns the response; throws an error if the server reports an error
     *      @param cmd command name
     *      @param arg parameter
     *      @return returns the server response on success
     *
     */
    commandSync(cmd: string, arg: string): string;

    /**
     * @description Sends the specified command and returns the response; throws an error if the server reports an error
     *      @param cmd command name
     *      @param arg parameter
     *      @return returns the server response on success
     *
     */
    commandAsync(cmd: string, arg: string): Promise<string>;

    /**
     * @description Sends the HELO command; throws an error if the server reports an error
     *      @param hostname host name, default is "localhost"
     *
     */
    hello(hostname?: string): Promise<void>;

    /**
     * @description Sends the HELO command; throws an error if the server reports an error
     *      @param hostname host name, default is "localhost"
     *
     */
    helloSync(hostname?: string): void;

    /**
     * @description Sends the HELO command; throws an error if the server reports an error
     *      @param hostname host name, default is "localhost"
     *
     */
    helloAsync(hostname?: string): Promise<void>;

    /**
     * @description Logs in to the server with the specified user and password; throws an error if the server reports an error
     *      @param username user name
     *      @param password password
     *
     */
    login(username: string, password: string): Promise<void>;

    /**
     * @description Logs in to the server with the specified user and password; throws an error if the server reports an error
     *      @param username user name
     *      @param password password
     *
     */
    loginSync(username: string, password: string): void;

    /**
     * @description Logs in to the server with the specified user and password; throws an error if the server reports an error
     *      @param username user name
     *      @param password password
     *
     */
    loginAsync(username: string, password: string): Promise<void>;

    /**
     * @description Specifies the sender mailbox; throws an error if the server reports an error
     *      @param address sender mailbox
     *
     */
    from(address: string): Promise<void>;

    /**
     * @description Specifies the sender mailbox; throws an error if the server reports an error
     *      @param address sender mailbox
     *
     */
    fromSync(address: string): void;

    /**
     * @description Specifies the sender mailbox; throws an error if the server reports an error
     *      @param address sender mailbox
     *
     */
    fromAsync(address: string): Promise<void>;

    /**
     * @description Specifies the recipient mailbox; throws an error if the server reports an error
     *      @param address recipient mailbox
     *
     */
    to(address: string): Promise<void>;

    /**
     * @description Specifies the recipient mailbox; throws an error if the server reports an error
     *      @param address recipient mailbox
     *
     */
    toSync(address: string): void;

    /**
     * @description Specifies the recipient mailbox; throws an error if the server reports an error
     *      @param address recipient mailbox
     *
     */
    toAsync(address: string): Promise<void>;

    /**
     * @description Sends text to the recipient; throws an error if the server reports an error
     *      @param txt the text to send
     *
     */
    data(txt: string): Promise<void>;

    /**
     * @description Sends text to the recipient; throws an error if the server reports an error
     *      @param txt the text to send
     *
     */
    dataSync(txt: string): void;

    /**
     * @description Sends text to the recipient; throws an error if the server reports an error
     *      @param txt the text to send
     *
     */
    dataAsync(txt: string): Promise<void>;

    /**
     * @description Quits and closes the connection; throws an error if the server reports an error
     */
    quit(): Promise<void>;

    /**
     * @description Quits and closes the connection; throws an error if the server reports an error
     */
    quitSync(): void;

    /**
     * @description Quits and closes the connection; throws an error if the server reports an error
     */
    quitAsync(): Promise<void>;

    /**
     * @description Queries and sets the timeout in milliseconds
     */
    timeout: number;

    /**
     * @description Queries the Socket currently connected to the Smtp object
     */
    readonly socket: Class_StreamPromise;

}


declare namespace Class_Smtp {
    const promises: FIBJS.GeneralObject;
}
