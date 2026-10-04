/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description the dgram.Socket object is an EventEmitter encapsulating datagram functionality.
 *
 * DgramSocket instances are created by dgram.createSocket(). Creating a dgram.Socket instance does not require the new keyword.
 *
 * Creation:
 * ```JavaScript
 * var dgram = require('dgram');
 * var sock = dgram.createSocket('udp4');
 * ```
 *
 */
declare class Class_DgramSocket extends Class_EventEmitter {
    /**
     * @description this method makes dgram.Socket listen for datagrams on the specified `port` and `addr`. A `listening` event is emitted when binding completes.
     *      @param port specifies the binding port; if `port` is not specified or is 0, the operating system will try to bind a random port
     *      @param addr specifies the binding address; if address is not specified, the operating system will try to listen on all addresses.
     *
     */
    bind(port?: number, addr?: string): void;

    bind(port?: number, addr?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the specified `port` and `addr`. A `listening` event is emitted when binding completes.
     *      @param port specifies the binding port; if `port` is not specified or is 0, the operating system will try to bind a random port
     *      @param addr specifies the binding address; if address is not specified, the operating system will try to listen on all addresses.
     *
     */
    bindSync(port?: number, addr?: string): void;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the specified `port` and `addr`. A `listening` event is emitted when binding completes.
     *      @param port specifies the binding port; if `port` is not specified or is 0, the operating system will try to bind a random port
     *      @param addr specifies the binding address; if address is not specified, the operating system will try to listen on all addresses.
     *
     */
    bindAsync(port?: number, addr?: string): Promise<void>;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the `port` and `address` specified by `opts`. A `listening` event is emitted when binding completes.
     *      @param opts specifies the binding parameters
     *
     */
    bind(opts: FIBJS.GeneralObject): void;

    bind(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the `port` and `address` specified by `opts`. A `listening` event is emitted when binding completes.
     *      @param opts specifies the binding parameters
     *
     */
    bindSync(opts: FIBJS.GeneralObject): void;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the `port` and `address` specified by `opts`. A `listening` event is emitted when binding completes.
     *      @param opts specifies the binding parameters
     *
     */
    bindAsync(opts: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, port: number, address?: string): number;

    send(msg: Class_Buffer | string, port: number, address?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, port: number, address?: string): number;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, port: number, address?: string): Promise<number>;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param offset starts sending from the specified offset
     *      @param length sends the specified length
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): number;

    send(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param offset starts sending from the specified offset
     *      @param length sends the specified length
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): number;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param offset starts sending from the specified offset
     *      @param length sends the specified length
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): Promise<number>;

    /**
     * @description returns an object containing the socket address information. For UDP sockets, the object will contain the address, family and port properties.
     *      @return returns the object bound address
     *
     */
    address(): {
        family: string;
        address: string;
        port: number;
    };

    /**
     * @description closes the current socket
     */
    close(): void;

    /**
     * @description closes the current socket
     *      @param callback the callback function after closing completes, which is equivalent to adding a listener for the `close` event
     *
     */
    close(callback: ()=>void): void;

    /**
     * @description queries the socket receive buffer size
     *      @return returns the query result
     *
     */
    getRecvBufferSize(): number;

    /**
     * @description queries the socket send buffer size
     *      @return returns the query result
     *
     */
    getSendBufferSize(): number;

    /**
     * @description joins the multicast group at the given multicastAddress and multicastInterface using the IP_ADD_MEMBERSHIP socket option. If the multicastInterface parameter is not specified, the operating system will choose an interface and add membership to it. To add membership to every available interface, call addMembership multiple times, once per interface.
     *      @param multicastAddress specifies the multicast group address to join
     *      @param multicastInterface specifies the multicast group interface to join
     *
     */
    addMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description leaves the multicast group at multicastAddress using the IP_DROP_MEMBERSHIP socket option. The kernel calls this method automatically when the socket is closed or the process terminates, so most applications never need to call it.
     *      @param multicastAddress specifies the multicast group address to drop
     *      @param multicastInterface specifies the multicast group interface to drop
     *
     */
    dropMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description sets the IP_MULTICAST_TTL socket option
     *      @param ttl specifies the ttl to set; the ttl parameter can be between 0 and 255. The default value on most systems is 1.
     *
     */
    setMulticastTTL(ttl: number): void;

    /**
     * @description sets the socket receive buffer size
     *      @param size specifies the size to set
     *
     */
    setRecvBufferSize(size: number): void;

    /**
     * @description sets the socket send buffer size
     *      @param size specifies the size to set
     *
     */
    setSendBufferSize(size: number): void;

    /**
     * @description sets or clears the SO_BROADCAST socket option
     *      @param flag when set to true, UDP packets will be sent to the broadcast address of a local interface
     *
     */
    setBroadcast(flag: boolean): void;

    /**
     * @description the `close` event is emitted after a `socket` is closed with `close()`. Once this event is emitted, no new `message` events will be emitted on this `socket`
     */
    on(event: "close", listener: ()=>void): this;

    once(event: "close", listener: ()=>void): this;

    off(event: "close", listener: ()=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    /**
     * @description the `close` event is emitted after a `socket` is closed with `close()`. Once this event is emitted, no new `message` events will be emitted on this `socket`
     */
    onclose: (()=>void) | null;

    /**
     * @description the `error` event is emitted when any error occurs
     */
    on(event: "error", listener: ()=>void): this;

    once(event: "error", listener: ()=>void): this;

    off(event: "error", listener: ()=>void): this;

    addListener(event: "error", listener: ()=>void): this;

    removeListener(event: "error", listener: ()=>void): this;

    addEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: ()=>void): this;

    /**
     * @description the `error` event is emitted when any error occurs
     */
    onerror: (()=>void) | null;

    /**
     * @description the `listening` event is emitted when a `socket` starts listening for datagrams. This event is emitted immediately after the UDP socket is created
     */
    on(event: "listening", listener: ()=>void): this;

    once(event: "listening", listener: ()=>void): this;

    off(event: "listening", listener: ()=>void): this;

    addListener(event: "listening", listener: ()=>void): this;

    removeListener(event: "listening", listener: ()=>void): this;

    addEventListener(event: "listening", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "listening", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "listening", listener: ()=>void): this;

    prependOnceListener(event: "listening", listener: ()=>void): this;

    /**
     * @description the `listening` event is emitted when a `socket` starts listening for datagrams. This event is emitted immediately after the UDP socket is created
     */
    onlistening: (()=>void) | null;

    /**
     * @description the `message` event is emitted when a new datagram is received by the `socket`. `msg` and `rinfo` are passed as parameters to the handler of this event.
     *      @param msg the received datagram
     *      @param rinfo an object containing the remote information of the received datagram. The object contains `address`, `port` and `family` properties, representing the remote address, port and protocol family respectively.
     *
     */
    on(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): this;

    once(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): this;

    off(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): this;

    addListener(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "message", listener: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): this;

    /**
     * @description the `message` event is emitted when a new datagram is received by the `socket`. `msg` and `rinfo` are passed as parameters to the handler of this event.
     *      @param msg the received datagram
     *      @param rinfo an object containing the remote information of the received datagram. The object contains `address`, `port` and `family` properties, representing the remote address, port and protocol family respectively.
     *
     */
    onmessage: ((msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description keeps the fibjs process from exiting, preventing the fibjs process from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_DgramSocket;

    /**
     * @description allows the fibjs process to exit, allowing the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_DgramSocket;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the DgramSocket class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_DgramSocketPromise extends Class_EventEmitter {
    /**
     * @description this method makes dgram.Socket listen for datagrams on the specified `port` and `addr`. A `listening` event is emitted when binding completes.
     *      @param port specifies the binding port; if `port` is not specified or is 0, the operating system will try to bind a random port
     *      @param addr specifies the binding address; if address is not specified, the operating system will try to listen on all addresses.
     *
     */
    bind(port?: number, addr?: string): Promise<void>;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the specified `port` and `addr`. A `listening` event is emitted when binding completes.
     *      @param port specifies the binding port; if `port` is not specified or is 0, the operating system will try to bind a random port
     *      @param addr specifies the binding address; if address is not specified, the operating system will try to listen on all addresses.
     *
     */
    bindSync(port?: number, addr?: string): void;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the specified `port` and `addr`. A `listening` event is emitted when binding completes.
     *      @param port specifies the binding port; if `port` is not specified or is 0, the operating system will try to bind a random port
     *      @param addr specifies the binding address; if address is not specified, the operating system will try to listen on all addresses.
     *
     */
    bindAsync(port?: number, addr?: string): Promise<void>;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the `port` and `address` specified by `opts`. A `listening` event is emitted when binding completes.
     *      @param opts specifies the binding parameters
     *
     */
    bind(opts: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the `port` and `address` specified by `opts`. A `listening` event is emitted when binding completes.
     *      @param opts specifies the binding parameters
     *
     */
    bindSync(opts: FIBJS.GeneralObject): void;

    /**
     * @description this method makes dgram.Socket listen for datagrams on the `port` and `address` specified by `opts`. A `listening` event is emitted when binding completes.
     *      @param opts specifies the binding parameters
     *
     */
    bindAsync(opts: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, port: number, address?: string): Promise<number>;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, port: number, address?: string): number;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, port: number, address?: string): Promise<number>;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param offset starts sending from the specified offset
     *      @param length sends the specified length
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): Promise<number>;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param offset starts sending from the specified offset
     *      @param length sends the specified length
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): number;

    /**
     * @description sends a datagram on the socket; a string msg is encoded as utf8
     *      @param msg specifies the data to send, a string is encoded as utf8
     *      @param offset starts sending from the specified offset
     *      @param length sends the specified length
     *      @param port specifies the destination port to send to
     *      @param address specifies the destination address to send to, a string is encoded as utf8
     *      @return returns the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): Promise<number>;

    /**
     * @description returns an object containing the socket address information. For UDP sockets, the object will contain the address, family and port properties.
     *      @return returns the object bound address
     *
     */
    address(): {
        family: string;
        address: string;
        port: number;
    };

    /**
     * @description closes the current socket
     */
    close(): void;

    /**
     * @description closes the current socket
     *      @param callback the callback function after closing completes, which is equivalent to adding a listener for the `close` event
     *
     */
    close(callback: ()=>void): void;

    /**
     * @description queries the socket receive buffer size
     *      @return returns the query result
     *
     */
    getRecvBufferSize(): number;

    /**
     * @description queries the socket send buffer size
     *      @return returns the query result
     *
     */
    getSendBufferSize(): number;

    /**
     * @description joins the multicast group at the given multicastAddress and multicastInterface using the IP_ADD_MEMBERSHIP socket option. If the multicastInterface parameter is not specified, the operating system will choose an interface and add membership to it. To add membership to every available interface, call addMembership multiple times, once per interface.
     *      @param multicastAddress specifies the multicast group address to join
     *      @param multicastInterface specifies the multicast group interface to join
     *
     */
    addMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description leaves the multicast group at multicastAddress using the IP_DROP_MEMBERSHIP socket option. The kernel calls this method automatically when the socket is closed or the process terminates, so most applications never need to call it.
     *      @param multicastAddress specifies the multicast group address to drop
     *      @param multicastInterface specifies the multicast group interface to drop
     *
     */
    dropMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description sets the IP_MULTICAST_TTL socket option
     *      @param ttl specifies the ttl to set; the ttl parameter can be between 0 and 255. The default value on most systems is 1.
     *
     */
    setMulticastTTL(ttl: number): void;

    /**
     * @description sets the socket receive buffer size
     *      @param size specifies the size to set
     *
     */
    setRecvBufferSize(size: number): void;

    /**
     * @description sets the socket send buffer size
     *      @param size specifies the size to set
     *
     */
    setSendBufferSize(size: number): void;

    /**
     * @description sets or clears the SO_BROADCAST socket option
     *      @param flag when set to true, UDP packets will be sent to the broadcast address of a local interface
     *
     */
    setBroadcast(flag: boolean): void;

    /**
     * @description the `close` event is emitted after a `socket` is closed with `close()`. Once this event is emitted, no new `message` events will be emitted on this `socket`
     */
    onclose: (()=>void) | null;

    /**
     * @description the `error` event is emitted when any error occurs
     */
    onerror: (()=>void) | null;

    /**
     * @description the `listening` event is emitted when a `socket` starts listening for datagrams. This event is emitted immediately after the UDP socket is created
     */
    onlistening: (()=>void) | null;

    /**
     * @description the `message` event is emitted when a new datagram is received by the `socket`. `msg` and `rinfo` are passed as parameters to the handler of this event.
     *      @param msg the received datagram
     *      @param rinfo an object containing the remote information of the received datagram. The object contains `address`, `port` and `family` properties, representing the remote address, port and protocol family respectively.
     *
     */
    onmessage: ((msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description keeps the fibjs process from exiting, preventing the fibjs process from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_DgramSocket;

    /**
     * @description allows the fibjs process to exit, allowing the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_DgramSocket;

}


declare namespace Class_DgramSocket {
    const promises: FIBJS.GeneralObject;
}
