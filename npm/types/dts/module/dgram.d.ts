/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DgramSocket.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description one of the dgram basic modules, mainly used to encapsulate UDP datagram sockets.
 *
 * Usage steps:
 *
 * 1. First, import the dgram module with the following statement.
 *
 * ```
 * var dgram = require('dgram');
 * ```
 *
 * 2. Create a UDP datagram socket instance.
 *
 * ```
 * var sock = dgram.createSocket('udp4');
 * ```
 *
 * 3. Register a data reception event message callback function for the UDP datagram socket.
 *
 * ```
 * sock.on('message', function (msg, rinfo) {
 *   // process received message
 * });
 * ```
 *
 * 4. Send UDP datagram messages to the specified destination address.
 *
 * ```
 * var msg = ...; // message to send
 * var port = ...; // destination port
 * var host = ...; // destination host
 * var bytes = sock.send(msg, 0, msg.length, port, host);
 * console.log('UDP message sent to ' + host + ':' + port);
 * ```
 *
 */
declare module 'dgram' {
    /**
     * @description the dgram.Socket object is an EventEmitter encapsulating datagram functionality. See DgramSocket
     *      dgram.Socket instances are created by dgram.createSocket(). Creating a dgram.Socket instance does not require the new keyword.
     *
     */
    const Socket: typeof Class_DgramSocket;

    /**
     * @description creates a dgram.Socket object
     *
     *      opts is the socket family, 'udp4' or 'udp6', or an options object:
     *      ```JavaScript
     *      {
     *          "type": "udp4" | "udp6",   // socket type
     *          "reuseAddr": true | false, // reuse address, default is false
     *          "ipv6Only": true | false, // only accept IPv6 packets, default is false
     *          "recvBufferSize": 1024,     // specify the size of the receive buffer
     *          "sendBufferSize": 1024      // specify the size of the send buffer
     *      }
     *      ```
     *      @param opts the socket family or the options object
     *      @return returns the created Socket object
     *
     */
    function createSocket(opts: FIBJS.GeneralObject | string): Class_DgramSocket;

    /**
     * @description creates a dgram.Socket object
     *
     *      opts is the socket family, 'udp4' or 'udp6', or an options object:
     *      ```JavaScript
     *      {
     *          "type": "udp4" | "udp6",   // socket type
     *          "reuseAddr": true | false, // reuse address, default is false
     *          "ipv6Only": true | false, // only accept IPv6 packets, default is false
     *          "recvBufferSize": 1024,     // specify the size of the receive buffer
     *          "sendBufferSize": 1024      // specify the size of the send buffer
     *      }
     *      ```
     *      @param opts the socket family or the options object
     *      @param callback adds a listener for the 'message' event.
     *      @return returns the created Socket object
     *
     */
    function createSocket(opts: FIBJS.GeneralObject | string, callback: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): Class_DgramSocket;

}

