/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description A UDP datagram socket: an EventEmitter endpoint that binds a local port, sends one datagram at a time to a destination, and delivers every received datagram through the 'message' event
 *
 *  DgramSocket is the concrete socket behind dgram.createSocket. It plays two roles:
 *  - **receiver**: bind to a local port (or let the first send bind automatically) and handle every
 *    datagram in the 'message' event;
 *  - **sender**: send one datagram at a time with the synchronous, callback or promise form of send.
 *
 *  DgramSocket inherits on/once/off/emit and the listener bookkeeping of EventEmitter and adds the
 *  UDP operations: bind, send, address, close, the buffer-size and multicast accessors and the
 *  ref/unref pair. See the dgram module for the UDP model, the size limits and the
 *  broadcast/multicast rules.
 *
 *  Concepts:
 *
 *  - **Message boundaries**: each send produces exactly one 'message' event and the payload is never
 *    split or merged; delivery, ordering and duplication are not guaranteed.
 *  - **Binding**: the socket is created unbound; bind assigns the local address and emits
 *    'listening' during the call, so the synchronous form returns after the event has been handled
 *    (Node.js emits it on a later tick). send on an unbound socket binds it first to a random port
 *    on 0.0.0.0 or ::.
 *  - **Call forms**: bind and send follow the asynchronous conventions of the module — without a
 *    callback they block the calling fiber and return the result, with a trailing callback they run
 *    asynchronously, and the Sync/Async aliases exist as well. Receiving is event-only: datagrams
 *    arrive at the 'message' event, whose handler runs in its own fiber and may block or call the
 *    socket without stalling the sender.
 *  - **Remote information**: a message handler receives the payload and an object with the sender
 *    address, family, port and payload size.
 *  - **Lifetime**: close releases the handle and emits 'close'; the address and buffer-size getters
 *    then fail with EBADF, so do not reuse the socket. A bound socket keeps the process alive until
 *    close or unref; ref restores the keep-alive.
 *  - **Node.js differences**: create instances with dgram.createSocket only (new dgram.Socket() is
 *    not constructible); there is no recv method; bind(opts) reads only the required port and
 *    address keys; errors are thrown rather than emitted through 'error'; and there are no
 *    setTTL/setMulticastLoopback/setMulticastInterface, source-specific membership or connect
 *    methods.
 *
 *  Obtained from:
 *  - `dgram.createSocket('udp4' | 'udp6' | options[, callback])` — the only creation path.
 *
 *  Example 1 — a request/response round trip with an explicit local port:
 *  ```JavaScript
 *  const dgram = require('dgram');
 *  const coroutine = require('coroutine');
 *
 *  const server = dgram.createSocket('udp4');
 *  server.bind(0, '127.0.0.1');
 *  server.on('message', (msg, rinfo) => {
 *      console.log(rinfo.address, rinfo.size); // 127.0.0.1 4
 *      server.send('pong', rinfo.port, rinfo.address);
 *  });
 *
 *  const client = dgram.createSocket('udp4');
 *  let reply = null;
 *  client.on('message', (msg) => { reply = msg.toString(); });
 *  client.send('ping', server.address().port, '127.0.0.1');
 *
 *  let waited = 0;
 *  while (reply === null && waited < 1000) {
 *      coroutine.sleep(10);
 *      waited += 10;
 *  }
 *  console.log(reply); // pong
 *
 *  client.close();
 *  server.close();
 *  ```
 *
 *  Example 2 — the options form, address() and the socket buffer sizes:
 *  ```JavaScript
 *  const dgram = require('dgram');
 *
 *  const socket = dgram.createSocket({
 *      type: 'udp4',
 *      reuseAddr: true,
 *      recvBufferSize: 65536,
 *      sendBufferSize: 65536
 *  });
 *  socket.bind({ port: 0, address: '127.0.0.1' });
 *
 *  const info = socket.address();
 *  console.log(info.family, info.address, info.port > 0); // IPv4 127.0.0.1 true
 *  console.log(socket.getRecvBufferSize() > 0, socket.getSendBufferSize() > 0); // true true
 *
 *  socket.close();
 *  ```
 *
 *  Example 3 — an event-driven receiver handling several datagrams:
 *  ```JavaScript
 *  const dgram = require('dgram');
 *  const coroutine = require('coroutine');
 *
 *  const received = [];
 *  const socket = dgram.createSocket('udp4', (msg) => {
 *      received.push(msg.toString());
 *  });
 *  socket.bind(0, '127.0.0.1');
 *
 *  const sender = dgram.createSocket('udp4');
 *  sender.send('one', socket.address().port, '127.0.0.1');
 *  sender.send('two', socket.address().port, '127.0.0.1');
 *  sender.send('three', socket.address().port, '127.0.0.1');
 *
 *  let waited = 0;
 *  while (received.length < 3 && waited < 1000) {
 *      coroutine.sleep(10);
 *      waited += 10;
 *  }
 *  console.log(received.length, received.indexOf('one') >= 0); // 3 true
 *
 *  sender.close();
 *  socket.close();
 *  ```
 *
 *  Example 4 — joining a multicast group (needs a multicast-capable interface):
 *  ```JavaScript
 *  // requires: network
 *  const dgram = require('dgram');
 *  const coroutine = require('coroutine');
 *
 *  const group = '225.0.0.100';
 *  const port = 41234;
 *
 *  let got = null;
 *  const member = dgram.createSocket({ type: 'udp4', reuseAddr: true });
 *  member.bind(port);
 *  member.addMembership(group);
 *  member.setMulticastTTL(1);
 *  member.on('message', (msg) => { got = msg.toString(); });
 *
 *  const sender = dgram.createSocket('udp4');
 *  sender.send('multicast', port, group);
 *
 *  let waited = 0;
 *  while (got === null && waited < 1000) {
 *      coroutine.sleep(10);
 *      waited += 10;
 *  }
 *  console.log(got); // multicast
 *
 *  sender.close();
 *  member.close();
 *  ```
 *
 */
declare class Class_DgramSocket extends Class_EventEmitter {
    /**
     * @description Binds the socket to a local port and address and starts delivering received datagrams to the 'message' event
     *
     *      port defaults to 0, which asks the operating system for a free port (read it back with
     *      address); addr defaults to "", which binds every local address (0.0.0.0 for udp4, :: for
     *      udp6). The recvBufferSize and sendBufferSize options passed to createSocket are applied
     *      here. The same operation is also available as bind(opts) with an options object.
     *
     *      The 'listening' event is emitted during bind — in the synchronous form it has already been
     *      handled when the call returns — while Node.js emits it on a later tick. Binding a socket
     *      that is already bound throws error 20009, and binding a port in use throws EADDRINUSE.
     *      Node.js additionally accepts exclusive/fd options; fibjs does not.
     *
     *      Example — bind to an ephemeral port and report the assigned address:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *
     *      const socket = dgram.createSocket('udp4');
     *      let info = null;
     *      socket.on('listening', () => {
     *          info = socket.address();
     *      });
     *
     *      socket.bind(0, '127.0.0.1');
     *      console.log(info.family, info.address, info.port > 0); // IPv4 127.0.0.1 true
     *
     *      socket.close();
     *      ```
     *      @param port the local port, 0 to let the operating system choose
     *      @param addr the local address, empty to bind every local address
     *
     */
    bind(port?: number, addr?: string): void;

    bind(port?: number, addr?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Binds the socket to a local port and address and starts delivering received datagrams to the 'message' event
     *
     *      port defaults to 0, which asks the operating system for a free port (read it back with
     *      address); addr defaults to "", which binds every local address (0.0.0.0 for udp4, :: for
     *      udp6). The recvBufferSize and sendBufferSize options passed to createSocket are applied
     *      here. The same operation is also available as bind(opts) with an options object.
     *
     *      The 'listening' event is emitted during bind — in the synchronous form it has already been
     *      handled when the call returns — while Node.js emits it on a later tick. Binding a socket
     *      that is already bound throws error 20009, and binding a port in use throws EADDRINUSE.
     *      Node.js additionally accepts exclusive/fd options; fibjs does not.
     *
     *      Example — bind to an ephemeral port and report the assigned address:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *
     *      const socket = dgram.createSocket('udp4');
     *      let info = null;
     *      socket.on('listening', () => {
     *          info = socket.address();
     *      });
     *
     *      socket.bind(0, '127.0.0.1');
     *      console.log(info.family, info.address, info.port > 0); // IPv4 127.0.0.1 true
     *
     *      socket.close();
     *      ```
     *      @param port the local port, 0 to let the operating system choose
     *      @param addr the local address, empty to bind every local address
     *
     */
    bindSync(port?: number, addr?: string): void;

    /**
     * @description Binds the socket to a local port and address and starts delivering received datagrams to the 'message' event
     *
     *      port defaults to 0, which asks the operating system for a free port (read it back with
     *      address); addr defaults to "", which binds every local address (0.0.0.0 for udp4, :: for
     *      udp6). The recvBufferSize and sendBufferSize options passed to createSocket are applied
     *      here. The same operation is also available as bind(opts) with an options object.
     *
     *      The 'listening' event is emitted during bind — in the synchronous form it has already been
     *      handled when the call returns — while Node.js emits it on a later tick. Binding a socket
     *      that is already bound throws error 20009, and binding a port in use throws EADDRINUSE.
     *      Node.js additionally accepts exclusive/fd options; fibjs does not.
     *
     *      Example — bind to an ephemeral port and report the assigned address:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *
     *      const socket = dgram.createSocket('udp4');
     *      let info = null;
     *      socket.on('listening', () => {
     *          info = socket.address();
     *      });
     *
     *      socket.bind(0, '127.0.0.1');
     *      console.log(info.family, info.address, info.port > 0); // IPv4 127.0.0.1 true
     *
     *      socket.close();
     *      ```
     *      @param port the local port, 0 to let the operating system choose
     *      @param addr the local address, empty to bind every local address
     *
     */
    bindAsync(port?: number, addr?: string): Promise<void>;

    /**
     * @description Binds the socket with an options object; both `port` and `address` are required
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          port: 0, // the local port, 0 to let the operating system choose
     *          address: "127.0.0.1" // the local address to bind; required even when port is 0
     *      })
     *      ```
     *
     *      Unlike Node.js, exclusive, fd and the remaining bind options are not read, and a missing
     *      key throws error 20002. The 'listening' event and the error behavior are the same as in the
     *      port/address form above.
     *      @param opts the binding options
     *
     */
    bind(opts: FIBJS.GeneralObject): void;

    bind(opts: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Binds the socket with an options object; both `port` and `address` are required
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          port: 0, // the local port, 0 to let the operating system choose
     *          address: "127.0.0.1" // the local address to bind; required even when port is 0
     *      })
     *      ```
     *
     *      Unlike Node.js, exclusive, fd and the remaining bind options are not read, and a missing
     *      key throws error 20002. The 'listening' event and the error behavior are the same as in the
     *      port/address form above.
     *      @param opts the binding options
     *
     */
    bindSync(opts: FIBJS.GeneralObject): void;

    /**
     * @description Binds the socket with an options object; both `port` and `address` are required
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          port: 0, // the local port, 0 to let the operating system choose
     *          address: "127.0.0.1" // the local address to bind; required even when port is 0
     *      })
     *      ```
     *
     *      Unlike Node.js, exclusive, fd and the remaining bind options are not read, and a missing
     *      key throws error 20002. The 'listening' event and the error behavior are the same as in the
     *      port/address form above.
     *      @param opts the binding options
     *
     */
    bindAsync(opts: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Sends one datagram to the given destination and returns the number of bytes sent
     *
     *      msg may be a Buffer or a string, which is encoded as utf8; the whole payload becomes one
     *      datagram. address defaults to the loopback address of the socket family (127.0.0.1 for
     *      udp4, ::1 for udp6), unlike Node.js which requires it; a host name is accepted and resolved
     *      through the system resolver, which throws ENOTFOUND or EAI_AGAIN for unknown names.
     *
     *      An unbound socket is bound automatically first (emitting 'listening') to a random port on
     *      every local address. A payload larger than 65507 bytes for udp4 (65527 for udp6) throws
     *      EMSGSIZE, and sending to a broadcast address without setBroadcast(true) throws EACCES. The
     *      trailing callback form receives (err, bytes); sendSync/sendAsync and the promises namespace
     *      are generated as well.
     *
     *      The byte-range form send(msg, offset, length, port, address) sends the slice
     *      [offset, offset + length) of the utf8-encoded payload and throws error 20004 for a negative
     *      offset or a non-positive length.
     *
     *      Example — send the last word of a larger payload:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const server = dgram.createSocket('udp4');
     *      let got = null;
     *      server.on('message', (msg) => { got = msg.toString(); });
     *      server.bind(0, '127.0.0.1');
     *
     *      const client = dgram.createSocket('udp4');
     *      const sent = client.send('hello world', 6, 5, server.address().port, '127.0.0.1');
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(sent, got); // 5 world
     *
     *      client.close();
     *      server.close();
     *      ```
     *      @param msg the datagram payload
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, port: number, address?: string): number;

    send(msg: Class_Buffer | string, port: number, address?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Sends one datagram to the given destination and returns the number of bytes sent
     *
     *      msg may be a Buffer or a string, which is encoded as utf8; the whole payload becomes one
     *      datagram. address defaults to the loopback address of the socket family (127.0.0.1 for
     *      udp4, ::1 for udp6), unlike Node.js which requires it; a host name is accepted and resolved
     *      through the system resolver, which throws ENOTFOUND or EAI_AGAIN for unknown names.
     *
     *      An unbound socket is bound automatically first (emitting 'listening') to a random port on
     *      every local address. A payload larger than 65507 bytes for udp4 (65527 for udp6) throws
     *      EMSGSIZE, and sending to a broadcast address without setBroadcast(true) throws EACCES. The
     *      trailing callback form receives (err, bytes); sendSync/sendAsync and the promises namespace
     *      are generated as well.
     *
     *      The byte-range form send(msg, offset, length, port, address) sends the slice
     *      [offset, offset + length) of the utf8-encoded payload and throws error 20004 for a negative
     *      offset or a non-positive length.
     *
     *      Example — send the last word of a larger payload:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const server = dgram.createSocket('udp4');
     *      let got = null;
     *      server.on('message', (msg) => { got = msg.toString(); });
     *      server.bind(0, '127.0.0.1');
     *
     *      const client = dgram.createSocket('udp4');
     *      const sent = client.send('hello world', 6, 5, server.address().port, '127.0.0.1');
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(sent, got); // 5 world
     *
     *      client.close();
     *      server.close();
     *      ```
     *      @param msg the datagram payload
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, port: number, address?: string): number;

    /**
     * @description Sends one datagram to the given destination and returns the number of bytes sent
     *
     *      msg may be a Buffer or a string, which is encoded as utf8; the whole payload becomes one
     *      datagram. address defaults to the loopback address of the socket family (127.0.0.1 for
     *      udp4, ::1 for udp6), unlike Node.js which requires it; a host name is accepted and resolved
     *      through the system resolver, which throws ENOTFOUND or EAI_AGAIN for unknown names.
     *
     *      An unbound socket is bound automatically first (emitting 'listening') to a random port on
     *      every local address. A payload larger than 65507 bytes for udp4 (65527 for udp6) throws
     *      EMSGSIZE, and sending to a broadcast address without setBroadcast(true) throws EACCES. The
     *      trailing callback form receives (err, bytes); sendSync/sendAsync and the promises namespace
     *      are generated as well.
     *
     *      The byte-range form send(msg, offset, length, port, address) sends the slice
     *      [offset, offset + length) of the utf8-encoded payload and throws error 20004 for a negative
     *      offset or a non-positive length.
     *
     *      Example — send the last word of a larger payload:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const server = dgram.createSocket('udp4');
     *      let got = null;
     *      server.on('message', (msg) => { got = msg.toString(); });
     *      server.bind(0, '127.0.0.1');
     *
     *      const client = dgram.createSocket('udp4');
     *      const sent = client.send('hello world', 6, 5, server.address().port, '127.0.0.1');
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(sent, got); // 5 world
     *
     *      client.close();
     *      server.close();
     *      ```
     *      @param msg the datagram payload
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, port: number, address?: string): Promise<number>;

    /**
     * @description Sends a byte range of the payload as one datagram
     *
     *      offset and length are byte counts; a string msg is encoded as utf8 before slicing. offset
     *      must be non-negative and length positive, otherwise error 20004 is thrown. The destination
     *      and the error behavior are the same as in the three-argument form above.
     *      @param msg the datagram payload
     *      @param offset the first byte to send
     *      @param length the number of bytes to send
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): number;

    send(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Sends a byte range of the payload as one datagram
     *
     *      offset and length are byte counts; a string msg is encoded as utf8 before slicing. offset
     *      must be non-negative and length positive, otherwise error 20004 is thrown. The destination
     *      and the error behavior are the same as in the three-argument form above.
     *      @param msg the datagram payload
     *      @param offset the first byte to send
     *      @param length the number of bytes to send
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): number;

    /**
     * @description Sends a byte range of the payload as one datagram
     *
     *      offset and length are byte counts; a string msg is encoded as utf8 before slicing. offset
     *      must be non-negative and length positive, otherwise error 20004 is thrown. The destination
     *      and the error behavior are the same as in the three-argument form above.
     *      @param msg the datagram payload
     *      @param offset the first byte to send
     *      @param length the number of bytes to send
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): Promise<number>;

    /**
     * @description Returns the local address the socket is bound to, as an object with the family, address and port
     *
     *      family is 'IPv4' or 'IPv6'. A bound socket is required: before bind (and after close) the
     *      underlying handle is not available and the call throws EBADF, comparable to the
     *      ERR_SOCKET_DGRAM_NOT_RUNNING error of Node.js. A socket bound implicitly by send can be
     *      queried too.
     *      @return the bound address information
     *
     */
    address(): {
        family: string;
        address: string;
        port: number;
    };

    /**
     * @description Closes the socket and releases the underlying handle
     *
     *      The 'close' event is emitted when the handle has been released; no further 'message' events
     *      are delivered. A second close throws error 20009 ('dgram: socket is already closing.'),
     *      whereas Node.js throws ERR_SOCKET_DGRAM_NOT_RUNNING. Do not use the socket after close:
     *      address and the buffer-size getters fail with EBADF. The close(Function() callback)
     *      overload is equivalent to close() with a listener on the 'close' event.
     *
     */
    close(): void;

    /**
     * @description Closes the socket and calls back when the handle has been released
     *
     *      The callback is registered as a 'close' listener, so it runs asynchronously after the close
     *      completes; it receives no arguments and errors are not reported to it.
     *
     *      Example — wait for the close callback:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const socket = dgram.createSocket('udp4');
     *      socket.bind(0, '127.0.0.1');
     *
     *      socket.close(() => {
     *          console.log('closed'); // closed
     *      });
     *      coroutine.sleep(50);
     *      ```
     *      @param callback the function called on the 'close' event
     *
     */
    close(callback: ()=>void): void;

    /**
     * @description Returns the operating-system receive buffer size of the socket in bytes
     *
     *      The value is the real size reported by the operating system and may differ from the size
     *      passed to setRecvBufferSize or to the recvBufferSize option (Linux, for example, may round
     *      or double the request). The socket must be bound, otherwise the call throws EBADF.
     *      @return the receive buffer size in bytes
     *
     */
    getRecvBufferSize(): number;

    /**
     * @description Returns the operating-system send buffer size of the socket in bytes
     *
     *      The value is the real size reported by the operating system and may differ from the size
     *      passed to setSendBufferSize or to the sendBufferSize option. The socket must be bound,
     *      otherwise the call throws EBADF.
     *      @return the send buffer size in bytes
     *
     */
    getSendBufferSize(): number;

    /**
     * @description Joins a multicast group on the given interface (IP_ADD_MEMBERSHIP)
     *
     *      The socket must be bound before joining. multicastInterface selects the local interface by
     *      its address; when it is empty the operating system picks one, and addMembership can be
     *      called once per interface to join on several of them. Membership is released by
     *      dropMembership or automatically when the socket is closed or the process exits, so most
     *      programs never call dropMembership explicitly. An invalid address throws EINVAL. Node.js
     *      additionally offers source-specific membership and interface/TTL helpers, fibjs does not.
     *
     *      Example — join a group and receive a datagram sent to it (needs a multicast-capable
     *      interface):
     *      ```JavaScript
     *      // requires: network
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const group = '225.0.0.100';
     *      const port = 41234;
     *
     *      let got = null;
     *      const member = dgram.createSocket({ type: 'udp4', reuseAddr: true });
     *      member.bind(port);
     *      member.addMembership(group);
     *      member.setMulticastTTL(1);
     *      member.on('message', (msg) => { got = msg.toString(); });
     *
     *      const sender = dgram.createSocket('udp4');
     *      sender.send('multicast', port, group);
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(got); // multicast
     *
     *      sender.close();
     *      member.close();
     *      ```
     *      @param multicastAddress the multicast group address to join
     *      @param multicastInterface the local interface address, empty to let the system choose
     *
     */
    addMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description Leaves the multicast group joined with addMembership (IP_DROP_MEMBERSHIP)
     *
     *      multicastInterface must match the interface used when the group was joined on one specific
     *      interface. Closing the socket or terminating the process removes all memberships, so
     *      calling this is rarely necessary. An invalid address throws EINVAL.
     *      @param multicastAddress the multicast group address to leave
     *      @param multicastInterface the local interface address, empty for the system choice
     *
     */
    dropMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description Sets the hop limit of outgoing multicast datagrams (IP_MULTICAST_TTL)
     *
     *      ttl is 0 to 255 and defaults to 1, so a multicast datagram stays on the local network; a
     *      value outside the range throws EINVAL. The option affects multicast destinations only, not
     *      the unicast time-to-live, and Node.js exposes the same setter.
     *      @param ttl the multicast hop limit, 0 to 255
     *
     */
    setMulticastTTL(ttl: number): void;

    /**
     * @description Sets the operating-system receive buffer size in bytes
     *
     *      The requested size is a hint: the operating system may round or clamp it, so read the
     *      effective value back with getRecvBufferSize. Passing recvBufferSize to createSocket applies
     *      the same setting while binding.
     *      @param size the requested receive buffer size in bytes
     *
     */
    setRecvBufferSize(size: number): void;

    /**
     * @description Sets the operating-system send buffer size in bytes
     *
     *      The requested size is a hint: the operating system may round or clamp it, so read the
     *      effective value back with getSendBufferSize. Passing sendBufferSize to createSocket applies
     *      the same setting while binding.
     *      @param size the requested send buffer size in bytes
     *
     */
    setSendBufferSize(size: number): void;

    /**
     * @description Enables or disables sending to the broadcast address (SO_BROADCAST)
     *
     *      Broadcast is disabled by default, and a send to an address such as 255.255.255.255 then
     *      throws EACCES; call setBroadcast(true) first. When the host has no route for the broadcast
     *      address the send fails with EHOSTUNREACH or ENETUNREACH instead. Receiving broadcast
     *      traffic needs no option, and Node.js exposes the same setter.
     *      @param flag true to allow broadcast sends
     *
     */
    setBroadcast(flag: boolean): void;

    /**
     * @description Emitted after the socket has been closed; no 'message' event follows it
     *
     *      The event carries no arguments and is delivered asynchronously after close; the underlying
     *      handle is released, so address and the buffer-size getters fail with EBADF afterwards.
     *
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
     * @description Emitted after the socket has been closed; no 'message' event follows it
     *
     *      The event carries no arguments and is delivered asynchronously after close; the underlying
     *      handle is released, so address and the buffer-size getters fail with EBADF afterwards.
     *
     */
    onclose: (()=>void) | null;

    /**
     * @description EventEmitter error event; the current implementation throws instead of emitting it
     *
     *      bind and send report failures by throwing (or through the callback), so DgramSocket itself
     *      never emits 'error'. As in Node.js, emitting 'error' without a listener throws.
     *
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
     * @description EventEmitter error event; the current implementation throws instead of emitting it
     *
     *      bind and send report failures by throwing (or through the callback), so DgramSocket itself
     *      never emits 'error'. As in Node.js, emitting 'error' without a listener throws.
     *
     */
    onerror: (()=>void) | null;

    /**
     * @description Emitted when bind completes and the socket can receive datagrams
     *
     *      The event is emitted during bind, so in the synchronous form it has already been delivered
     *      when bind returns; binding implicitly inside send emits it too. Node.js emits 'listening' on
     *      a later tick.
     *
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
     * @description Emitted when bind completes and the socket can receive datagrams
     *
     *      The event is emitted during bind, so in the synchronous form it has already been delivered
     *      when bind returns; binding implicitly inside send emits it too. Node.js emits 'listening' on
     *      a later tick.
     *
     */
    onlistening: (()=>void) | null;

    /**
     * @description Emitted for every received datagram
     *
     *      msg is a Buffer with exactly the bytes of one datagram. rinfo is an object with `address`
     *      (the sender address), `family` ('IPv4' or 'IPv6'), `port` (the sender port) and `size` (the
     *      payload length in bytes). The handler runs in its own fiber, so it may block or send without
     *      stalling other sockets.
     *      @param msg the received datagram
     *      @param rinfo the remote information of the sender
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
     * @description Emitted for every received datagram
     *
     *      msg is a Buffer with exactly the bytes of one datagram. rinfo is an object with `address`
     *      (the sender address), `family` ('IPv4' or 'IPv6'), `port` (the sender port) and `size` (the
     *      payload length in bytes). The handler runs in its own fiber, so it may block or send without
     *      stalling other sockets.
     *      @param msg the received datagram
     *      @param rinfo the remote information of the sender
     *
     */
    onmessage: ((msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Keeps the fibjs process alive while the socket is bound (the default)
     *
     *      A bound socket holds a reference that prevents the process from exiting; ref restores that
     *      reference after unref. Node.js has the same pair.
     *      @return the socket itself
     *
     */
    ref(): Class_DgramSocket;

    /**
     * @description Allows the fibjs process to exit while the socket is bound
     *
     *      unref removes the keep-alive reference, so a program whose only remaining work is receiving
     *      datagrams can exit; processing continues while other references keep the loop alive.
     *      Node.js has the same method.
     *      @return the socket itself
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
     * @description Binds the socket to a local port and address and starts delivering received datagrams to the 'message' event
     *
     *      port defaults to 0, which asks the operating system for a free port (read it back with
     *      address); addr defaults to "", which binds every local address (0.0.0.0 for udp4, :: for
     *      udp6). The recvBufferSize and sendBufferSize options passed to createSocket are applied
     *      here. The same operation is also available as bind(opts) with an options object.
     *
     *      The 'listening' event is emitted during bind — in the synchronous form it has already been
     *      handled when the call returns — while Node.js emits it on a later tick. Binding a socket
     *      that is already bound throws error 20009, and binding a port in use throws EADDRINUSE.
     *      Node.js additionally accepts exclusive/fd options; fibjs does not.
     *
     *      Example — bind to an ephemeral port and report the assigned address:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *
     *      const socket = dgram.createSocket('udp4');
     *      let info = null;
     *      socket.on('listening', () => {
     *          info = socket.address();
     *      });
     *
     *      socket.bind(0, '127.0.0.1');
     *      console.log(info.family, info.address, info.port > 0); // IPv4 127.0.0.1 true
     *
     *      socket.close();
     *      ```
     *      @param port the local port, 0 to let the operating system choose
     *      @param addr the local address, empty to bind every local address
     *
     */
    bind(port?: number, addr?: string): Promise<void>;

    /**
     * @description Binds the socket to a local port and address and starts delivering received datagrams to the 'message' event
     *
     *      port defaults to 0, which asks the operating system for a free port (read it back with
     *      address); addr defaults to "", which binds every local address (0.0.0.0 for udp4, :: for
     *      udp6). The recvBufferSize and sendBufferSize options passed to createSocket are applied
     *      here. The same operation is also available as bind(opts) with an options object.
     *
     *      The 'listening' event is emitted during bind — in the synchronous form it has already been
     *      handled when the call returns — while Node.js emits it on a later tick. Binding a socket
     *      that is already bound throws error 20009, and binding a port in use throws EADDRINUSE.
     *      Node.js additionally accepts exclusive/fd options; fibjs does not.
     *
     *      Example — bind to an ephemeral port and report the assigned address:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *
     *      const socket = dgram.createSocket('udp4');
     *      let info = null;
     *      socket.on('listening', () => {
     *          info = socket.address();
     *      });
     *
     *      socket.bind(0, '127.0.0.1');
     *      console.log(info.family, info.address, info.port > 0); // IPv4 127.0.0.1 true
     *
     *      socket.close();
     *      ```
     *      @param port the local port, 0 to let the operating system choose
     *      @param addr the local address, empty to bind every local address
     *
     */
    bindSync(port?: number, addr?: string): void;

    /**
     * @description Binds the socket to a local port and address and starts delivering received datagrams to the 'message' event
     *
     *      port defaults to 0, which asks the operating system for a free port (read it back with
     *      address); addr defaults to "", which binds every local address (0.0.0.0 for udp4, :: for
     *      udp6). The recvBufferSize and sendBufferSize options passed to createSocket are applied
     *      here. The same operation is also available as bind(opts) with an options object.
     *
     *      The 'listening' event is emitted during bind — in the synchronous form it has already been
     *      handled when the call returns — while Node.js emits it on a later tick. Binding a socket
     *      that is already bound throws error 20009, and binding a port in use throws EADDRINUSE.
     *      Node.js additionally accepts exclusive/fd options; fibjs does not.
     *
     *      Example — bind to an ephemeral port and report the assigned address:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *
     *      const socket = dgram.createSocket('udp4');
     *      let info = null;
     *      socket.on('listening', () => {
     *          info = socket.address();
     *      });
     *
     *      socket.bind(0, '127.0.0.1');
     *      console.log(info.family, info.address, info.port > 0); // IPv4 127.0.0.1 true
     *
     *      socket.close();
     *      ```
     *      @param port the local port, 0 to let the operating system choose
     *      @param addr the local address, empty to bind every local address
     *
     */
    bindAsync(port?: number, addr?: string): Promise<void>;

    /**
     * @description Binds the socket with an options object; both `port` and `address` are required
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          port: 0, // the local port, 0 to let the operating system choose
     *          address: "127.0.0.1" // the local address to bind; required even when port is 0
     *      })
     *      ```
     *
     *      Unlike Node.js, exclusive, fd and the remaining bind options are not read, and a missing
     *      key throws error 20002. The 'listening' event and the error behavior are the same as in the
     *      port/address form above.
     *      @param opts the binding options
     *
     */
    bind(opts: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Binds the socket with an options object; both `port` and `address` are required
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          port: 0, // the local port, 0 to let the operating system choose
     *          address: "127.0.0.1" // the local address to bind; required even when port is 0
     *      })
     *      ```
     *
     *      Unlike Node.js, exclusive, fd and the remaining bind options are not read, and a missing
     *      key throws error 20002. The 'listening' event and the error behavior are the same as in the
     *      port/address form above.
     *      @param opts the binding options
     *
     */
    bindSync(opts: FIBJS.GeneralObject): void;

    /**
     * @description Binds the socket with an options object; both `port` and `address` are required
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          port: 0, // the local port, 0 to let the operating system choose
     *          address: "127.0.0.1" // the local address to bind; required even when port is 0
     *      })
     *      ```
     *
     *      Unlike Node.js, exclusive, fd and the remaining bind options are not read, and a missing
     *      key throws error 20002. The 'listening' event and the error behavior are the same as in the
     *      port/address form above.
     *      @param opts the binding options
     *
     */
    bindAsync(opts: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Sends one datagram to the given destination and returns the number of bytes sent
     *
     *      msg may be a Buffer or a string, which is encoded as utf8; the whole payload becomes one
     *      datagram. address defaults to the loopback address of the socket family (127.0.0.1 for
     *      udp4, ::1 for udp6), unlike Node.js which requires it; a host name is accepted and resolved
     *      through the system resolver, which throws ENOTFOUND or EAI_AGAIN for unknown names.
     *
     *      An unbound socket is bound automatically first (emitting 'listening') to a random port on
     *      every local address. A payload larger than 65507 bytes for udp4 (65527 for udp6) throws
     *      EMSGSIZE, and sending to a broadcast address without setBroadcast(true) throws EACCES. The
     *      trailing callback form receives (err, bytes); sendSync/sendAsync and the promises namespace
     *      are generated as well.
     *
     *      The byte-range form send(msg, offset, length, port, address) sends the slice
     *      [offset, offset + length) of the utf8-encoded payload and throws error 20004 for a negative
     *      offset or a non-positive length.
     *
     *      Example — send the last word of a larger payload:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const server = dgram.createSocket('udp4');
     *      let got = null;
     *      server.on('message', (msg) => { got = msg.toString(); });
     *      server.bind(0, '127.0.0.1');
     *
     *      const client = dgram.createSocket('udp4');
     *      const sent = client.send('hello world', 6, 5, server.address().port, '127.0.0.1');
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(sent, got); // 5 world
     *
     *      client.close();
     *      server.close();
     *      ```
     *      @param msg the datagram payload
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, port: number, address?: string): Promise<number>;

    /**
     * @description Sends one datagram to the given destination and returns the number of bytes sent
     *
     *      msg may be a Buffer or a string, which is encoded as utf8; the whole payload becomes one
     *      datagram. address defaults to the loopback address of the socket family (127.0.0.1 for
     *      udp4, ::1 for udp6), unlike Node.js which requires it; a host name is accepted and resolved
     *      through the system resolver, which throws ENOTFOUND or EAI_AGAIN for unknown names.
     *
     *      An unbound socket is bound automatically first (emitting 'listening') to a random port on
     *      every local address. A payload larger than 65507 bytes for udp4 (65527 for udp6) throws
     *      EMSGSIZE, and sending to a broadcast address without setBroadcast(true) throws EACCES. The
     *      trailing callback form receives (err, bytes); sendSync/sendAsync and the promises namespace
     *      are generated as well.
     *
     *      The byte-range form send(msg, offset, length, port, address) sends the slice
     *      [offset, offset + length) of the utf8-encoded payload and throws error 20004 for a negative
     *      offset or a non-positive length.
     *
     *      Example — send the last word of a larger payload:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const server = dgram.createSocket('udp4');
     *      let got = null;
     *      server.on('message', (msg) => { got = msg.toString(); });
     *      server.bind(0, '127.0.0.1');
     *
     *      const client = dgram.createSocket('udp4');
     *      const sent = client.send('hello world', 6, 5, server.address().port, '127.0.0.1');
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(sent, got); // 5 world
     *
     *      client.close();
     *      server.close();
     *      ```
     *      @param msg the datagram payload
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, port: number, address?: string): number;

    /**
     * @description Sends one datagram to the given destination and returns the number of bytes sent
     *
     *      msg may be a Buffer or a string, which is encoded as utf8; the whole payload becomes one
     *      datagram. address defaults to the loopback address of the socket family (127.0.0.1 for
     *      udp4, ::1 for udp6), unlike Node.js which requires it; a host name is accepted and resolved
     *      through the system resolver, which throws ENOTFOUND or EAI_AGAIN for unknown names.
     *
     *      An unbound socket is bound automatically first (emitting 'listening') to a random port on
     *      every local address. A payload larger than 65507 bytes for udp4 (65527 for udp6) throws
     *      EMSGSIZE, and sending to a broadcast address without setBroadcast(true) throws EACCES. The
     *      trailing callback form receives (err, bytes); sendSync/sendAsync and the promises namespace
     *      are generated as well.
     *
     *      The byte-range form send(msg, offset, length, port, address) sends the slice
     *      [offset, offset + length) of the utf8-encoded payload and throws error 20004 for a negative
     *      offset or a non-positive length.
     *
     *      Example — send the last word of a larger payload:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const server = dgram.createSocket('udp4');
     *      let got = null;
     *      server.on('message', (msg) => { got = msg.toString(); });
     *      server.bind(0, '127.0.0.1');
     *
     *      const client = dgram.createSocket('udp4');
     *      const sent = client.send('hello world', 6, 5, server.address().port, '127.0.0.1');
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(sent, got); // 5 world
     *
     *      client.close();
     *      server.close();
     *      ```
     *      @param msg the datagram payload
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, port: number, address?: string): Promise<number>;

    /**
     * @description Sends a byte range of the payload as one datagram
     *
     *      offset and length are byte counts; a string msg is encoded as utf8 before slicing. offset
     *      must be non-negative and length positive, otherwise error 20004 is thrown. The destination
     *      and the error behavior are the same as in the three-argument form above.
     *      @param msg the datagram payload
     *      @param offset the first byte to send
     *      @param length the number of bytes to send
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    send(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): Promise<number>;

    /**
     * @description Sends a byte range of the payload as one datagram
     *
     *      offset and length are byte counts; a string msg is encoded as utf8 before slicing. offset
     *      must be non-negative and length positive, otherwise error 20004 is thrown. The destination
     *      and the error behavior are the same as in the three-argument form above.
     *      @param msg the datagram payload
     *      @param offset the first byte to send
     *      @param length the number of bytes to send
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendSync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): number;

    /**
     * @description Sends a byte range of the payload as one datagram
     *
     *      offset and length are byte counts; a string msg is encoded as utf8 before slicing. offset
     *      must be non-negative and length positive, otherwise error 20004 is thrown. The destination
     *      and the error behavior are the same as in the three-argument form above.
     *      @param msg the datagram payload
     *      @param offset the first byte to send
     *      @param length the number of bytes to send
     *      @param port the destination port
     *      @param address the destination address or host name, empty for the loopback address
     *      @return the number of bytes sent
     *
     */
    sendAsync(msg: Class_Buffer | string, offset: number, length: number, port: number, address?: string): Promise<number>;

    /**
     * @description Returns the local address the socket is bound to, as an object with the family, address and port
     *
     *      family is 'IPv4' or 'IPv6'. A bound socket is required: before bind (and after close) the
     *      underlying handle is not available and the call throws EBADF, comparable to the
     *      ERR_SOCKET_DGRAM_NOT_RUNNING error of Node.js. A socket bound implicitly by send can be
     *      queried too.
     *      @return the bound address information
     *
     */
    address(): {
        family: string;
        address: string;
        port: number;
    };

    /**
     * @description Closes the socket and releases the underlying handle
     *
     *      The 'close' event is emitted when the handle has been released; no further 'message' events
     *      are delivered. A second close throws error 20009 ('dgram: socket is already closing.'),
     *      whereas Node.js throws ERR_SOCKET_DGRAM_NOT_RUNNING. Do not use the socket after close:
     *      address and the buffer-size getters fail with EBADF. The close(Function() callback)
     *      overload is equivalent to close() with a listener on the 'close' event.
     *
     */
    close(): void;

    /**
     * @description Closes the socket and calls back when the handle has been released
     *
     *      The callback is registered as a 'close' listener, so it runs asynchronously after the close
     *      completes; it receives no arguments and errors are not reported to it.
     *
     *      Example — wait for the close callback:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const socket = dgram.createSocket('udp4');
     *      socket.bind(0, '127.0.0.1');
     *
     *      socket.close(() => {
     *          console.log('closed'); // closed
     *      });
     *      coroutine.sleep(50);
     *      ```
     *      @param callback the function called on the 'close' event
     *
     */
    close(callback: ()=>void): void;

    /**
     * @description Returns the operating-system receive buffer size of the socket in bytes
     *
     *      The value is the real size reported by the operating system and may differ from the size
     *      passed to setRecvBufferSize or to the recvBufferSize option (Linux, for example, may round
     *      or double the request). The socket must be bound, otherwise the call throws EBADF.
     *      @return the receive buffer size in bytes
     *
     */
    getRecvBufferSize(): number;

    /**
     * @description Returns the operating-system send buffer size of the socket in bytes
     *
     *      The value is the real size reported by the operating system and may differ from the size
     *      passed to setSendBufferSize or to the sendBufferSize option. The socket must be bound,
     *      otherwise the call throws EBADF.
     *      @return the send buffer size in bytes
     *
     */
    getSendBufferSize(): number;

    /**
     * @description Joins a multicast group on the given interface (IP_ADD_MEMBERSHIP)
     *
     *      The socket must be bound before joining. multicastInterface selects the local interface by
     *      its address; when it is empty the operating system picks one, and addMembership can be
     *      called once per interface to join on several of them. Membership is released by
     *      dropMembership or automatically when the socket is closed or the process exits, so most
     *      programs never call dropMembership explicitly. An invalid address throws EINVAL. Node.js
     *      additionally offers source-specific membership and interface/TTL helpers, fibjs does not.
     *
     *      Example — join a group and receive a datagram sent to it (needs a multicast-capable
     *      interface):
     *      ```JavaScript
     *      // requires: network
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      const group = '225.0.0.100';
     *      const port = 41234;
     *
     *      let got = null;
     *      const member = dgram.createSocket({ type: 'udp4', reuseAddr: true });
     *      member.bind(port);
     *      member.addMembership(group);
     *      member.setMulticastTTL(1);
     *      member.on('message', (msg) => { got = msg.toString(); });
     *
     *      const sender = dgram.createSocket('udp4');
     *      sender.send('multicast', port, group);
     *
     *      let waited = 0;
     *      while (got === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(got); // multicast
     *
     *      sender.close();
     *      member.close();
     *      ```
     *      @param multicastAddress the multicast group address to join
     *      @param multicastInterface the local interface address, empty to let the system choose
     *
     */
    addMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description Leaves the multicast group joined with addMembership (IP_DROP_MEMBERSHIP)
     *
     *      multicastInterface must match the interface used when the group was joined on one specific
     *      interface. Closing the socket or terminating the process removes all memberships, so
     *      calling this is rarely necessary. An invalid address throws EINVAL.
     *      @param multicastAddress the multicast group address to leave
     *      @param multicastInterface the local interface address, empty for the system choice
     *
     */
    dropMembership(multicastAddress: string, multicastInterface?: string): void;

    /**
     * @description Sets the hop limit of outgoing multicast datagrams (IP_MULTICAST_TTL)
     *
     *      ttl is 0 to 255 and defaults to 1, so a multicast datagram stays on the local network; a
     *      value outside the range throws EINVAL. The option affects multicast destinations only, not
     *      the unicast time-to-live, and Node.js exposes the same setter.
     *      @param ttl the multicast hop limit, 0 to 255
     *
     */
    setMulticastTTL(ttl: number): void;

    /**
     * @description Sets the operating-system receive buffer size in bytes
     *
     *      The requested size is a hint: the operating system may round or clamp it, so read the
     *      effective value back with getRecvBufferSize. Passing recvBufferSize to createSocket applies
     *      the same setting while binding.
     *      @param size the requested receive buffer size in bytes
     *
     */
    setRecvBufferSize(size: number): void;

    /**
     * @description Sets the operating-system send buffer size in bytes
     *
     *      The requested size is a hint: the operating system may round or clamp it, so read the
     *      effective value back with getSendBufferSize. Passing sendBufferSize to createSocket applies
     *      the same setting while binding.
     *      @param size the requested send buffer size in bytes
     *
     */
    setSendBufferSize(size: number): void;

    /**
     * @description Enables or disables sending to the broadcast address (SO_BROADCAST)
     *
     *      Broadcast is disabled by default, and a send to an address such as 255.255.255.255 then
     *      throws EACCES; call setBroadcast(true) first. When the host has no route for the broadcast
     *      address the send fails with EHOSTUNREACH or ENETUNREACH instead. Receiving broadcast
     *      traffic needs no option, and Node.js exposes the same setter.
     *      @param flag true to allow broadcast sends
     *
     */
    setBroadcast(flag: boolean): void;

    /**
     * @description Emitted after the socket has been closed; no 'message' event follows it
     *
     *      The event carries no arguments and is delivered asynchronously after close; the underlying
     *      handle is released, so address and the buffer-size getters fail with EBADF afterwards.
     *
     */
    onclose: (()=>void) | null;

    /**
     * @description EventEmitter error event; the current implementation throws instead of emitting it
     *
     *      bind and send report failures by throwing (or through the callback), so DgramSocket itself
     *      never emits 'error'. As in Node.js, emitting 'error' without a listener throws.
     *
     */
    onerror: (()=>void) | null;

    /**
     * @description Emitted when bind completes and the socket can receive datagrams
     *
     *      The event is emitted during bind, so in the synchronous form it has already been delivered
     *      when bind returns; binding implicitly inside send emits it too. Node.js emits 'listening' on
     *      a later tick.
     *
     */
    onlistening: (()=>void) | null;

    /**
     * @description Emitted for every received datagram
     *
     *      msg is a Buffer with exactly the bytes of one datagram. rinfo is an object with `address`
     *      (the sender address), `family` ('IPv4' or 'IPv6'), `port` (the sender port) and `size` (the
     *      payload length in bytes). The handler runs in its own fiber, so it may block or send without
     *      stalling other sockets.
     *      @param msg the received datagram
     *      @param rinfo the remote information of the sender
     *
     */
    onmessage: ((msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Keeps the fibjs process alive while the socket is bound (the default)
     *
     *      A bound socket holds a reference that prevents the process from exiting; ref restores that
     *      reference after unref. Node.js has the same pair.
     *      @return the socket itself
     *
     */
    ref(): Class_DgramSocket;

    /**
     * @description Allows the fibjs process to exit while the socket is bound
     *
     *      unref removes the keep-alive reference, so a program whose only remaining work is receiving
     *      datagrams can exit; processing continues while other references keep the loop alive.
     *      Node.js has the same method.
     *      @return the socket itself
     *
     */
    unref(): Class_DgramSocket;

}


declare namespace Class_DgramSocket {
    const promises: FIBJS.GeneralObject;
}
