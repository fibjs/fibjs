/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DgramSocket.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The dgram module provides UDP datagram sockets: create a socket, bind it to a local port, send datagrams to a destination and receive each datagram as one message; useful for discovery and telemetry protocols, broadcast and multicast delivery and any service where message boundaries matter
 *
 *  Main capabilities:
 *
 *  - **Socket creation**: `createSocket` creates a `DgramSocket` of the `udp4` or `udp6` family from
 *    a family string or an options object; `Socket` is the class alias used for type checks;
 *  - **Datagram transfer**: `send` transmits one buffer or string as one datagram, and every
 *    received datagram is delivered to the `'message'` event;
 *  - **Local endpoint**: `bind` assigns the local port and address, `address` reads them back and
 *    `close` releases the handle;
 *  - **Broadcast and multicast**: `setBroadcast`, `addMembership`, `dropMembership` and
 *    `setMulticastTTL` control the corresponding socket options;
 *  - **Socket buffers**: `getRecvBufferSize`/`setRecvBufferSize` and
 *    `getSendBufferSize`/`setSendBufferSize` read and write the operating-system buffers;
 *  - **Lifetime**: `ref` and `unref` control whether a bound socket keeps the process alive.
 *
 *  Concepts:
 *
 *  - **Datagrams vs streams**: UDP keeps message boundaries — one send produces exactly one
 *    'message' event and the payload is never split or merged — but it is unreliable: datagrams
 *    may be lost, duplicated or reordered, and there is no connection, retransmission or flow
 *    control. Use net, tls or http when a reliable byte stream is needed.
 *  - **Size limits**: one udp4 datagram carries at most 65507 bytes of payload (65527 for udp6) and
 *    a larger payload throws EMSGSIZE. A datagram larger than the path MTU (about 1472 payload
 *    bytes on Ethernet) is fragmented by IP, and losing one fragment loses the whole datagram.
 *  - **Binding**: sockets are created unbound. `bind` assigns the local address and emits
 *    'listening'; the first `send` of an unbound socket binds it automatically to a random port on
 *    every local address, which is fine for clients but means a server must bind explicitly so that
 *    peers can be told the port in advance.
 *  - **Fiber and event duality**: bind and send follow the fibjs asynchronous conventions —
 *    without a callback they block the calling fiber and return the result, with a trailing
 *    callback they run asynchronously, and the Sync/Async aliases and the promises namespace exist
 *    as well. Receiving is event-only (there is no blocking recv): datagrams arrive at the
 *    'message' event, whose handler runs in its own fiber.
 *  - **Broadcast and multicast**: receiving broadcast traffic needs no option, but sending to a
 *    broadcast address requires `setBroadcast(true)`; multicast requires joining a group with
 *    `addMembership`, and `setMulticastTTL` limits how far multicast datagrams travel.
 *  - **Node.js differences**: `dgram.Socket` is a class reference and is not constructible —
 *    create instances with `createSocket` only; `send` accepts a missing address (defaulting to
 *    loopback) and has no AddressInfo destination form; there is no `socket.connect`, no `setTTL`,
 *    `setMulticastLoopback` or `setMulticastInterface` and no source-specific multicast methods;
 *    accepting a family string directly in `createSocket` is a fibjs extension.
 *
 *  Import:
 *  ```JavaScript
 *  const dgram = require('dgram');
 *  ```
 *
 *  Example 1 — a local round trip on an OS-assigned port:
 *  ```JavaScript
 *  const dgram = require('dgram');
 *  const coroutine = require('coroutine');
 *
 *  const server = dgram.createSocket('udp4');
 *  server.bind(0, '127.0.0.1');
 *  server.on('message', (msg, rinfo) => {
 *      server.send(msg, rinfo.port, rinfo.address); // echo the datagram back
 *  });
 *
 *  const client = dgram.createSocket('udp4');
 *  let reply = null;
 *  client.on('message', (msg) => { reply = msg.toString(); });
 *  const bytes = client.send('ping', server.address().port, '127.0.0.1');
 *
 *  let waited = 0;
 *  while (reply === null && waited < 1000) {
 *      coroutine.sleep(10);
 *      waited += 10;
 *  }
 *  console.log(bytes, reply); // 4 ping
 *
 *  client.close();
 *  server.close();
 *  ```
 *
 *  Example 2 — the event-driven form with a message handler installed at creation:
 *  ```JavaScript
 *  const dgram = require('dgram');
 *  const coroutine = require('coroutine');
 *
 *  const server = dgram.createSocket('udp4', (msg, rinfo) => {
 *      server.send(msg.toString().toUpperCase(), rinfo.port, rinfo.address);
 *  });
 *  server.bind(0, '127.0.0.1');
 *
 *  const client = dgram.createSocket('udp4');
 *  let reply = null;
 *  client.on('message', (msg) => { reply = msg.toString(); });
 *  client.send('hello', server.address().port, '127.0.0.1');
 *
 *  let waited = 0;
 *  while (reply === null && waited < 1000) {
 *      coroutine.sleep(10);
 *      waited += 10;
 *  }
 *  console.log(reply); // HELLO
 *
 *  client.close();
 *  server.close();
 *  ```
 *
 *  Example 3 — unknown socket types and an already used port fail with an error:
 *  ```JavaScript
 *  const dgram = require('dgram');
 *
 *  try {
 *      dgram.createSocket('udp5');
 *  } catch (err) {
 *      console.log(err.message); // dgram: unknown socket type: 'udp5'.
 *  }
 *
 *  const first = dgram.createSocket('udp4');
 *  first.bind(0, '127.0.0.1');
 *
 *  const second = dgram.createSocket('udp4');
 *  try {
 *      second.bind(first.address().port, '127.0.0.1');
 *  } catch (err) {
 *      console.log(err.code); // EADDRINUSE
 *  }
 *
 *  second.close();
 *  first.close();
 *  ```
 *
 */
declare module 'dgram' {
    /**
     * @description The alias of the DgramSocket class, see DgramSocket
     *
     *      It is a class reference, useful for `instanceof` checks; unlike Node.js it is not a
     *      constructor, so a socket is always created with createSocket.
     *
     */
    const Socket: typeof Class_DgramSocket;

    /**
     * @description Creates a UDP socket of the given family
     *
     *      opts is either the family string 'udp4' or 'udp6', or an options object. The options object
     *      accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          type: 'udp4', // 'udp4' or 'udp6'; required
     *          reuseAddr: false, // let several sockets share the address and port; default false
     *          ipv6Only: false, // for udp6, disable the dual-stack IPv4 mapping; default false
     *          recvBufferSize: 0, // receive buffer hint in bytes, applied on bind; default 0 = system
     *          sendBufferSize: 0 // send buffer hint in bytes, applied on bind; default 0 = system
     *      })
     *      ```
     *
     *      The socket is created unbound; call bind, or let the first send bind it. Passing a callback
     *      as the second argument is equivalent to adding a listener for the 'message' event, see the
     *      createSocket(opts, callback) overload. An unknown family throws an error with number 20004
     *      and the message `dgram: unknown socket type: '<value>'.`.
     *
     *      Example — create with options, bind and print the assigned address:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *
     *      const socket = dgram.createSocket({
     *          type: 'udp4',
     *          reuseAddr: true,
     *          recvBufferSize: 65536,
     *          sendBufferSize: 65536
     *      });
     *      socket.bind(0, '127.0.0.1');
     *
     *      console.log(socket.address().address); // 127.0.0.1
     *      console.log(socket.getRecvBufferSize() > 0, socket.getSendBufferSize() > 0); // true true
     *
     *      socket.close();
     *      ```
     *      @param opts the family string or the options object
     *      @return the created socket
     *
     */
    function createSocket(opts: FIBJS.GeneralObject | string): Class_DgramSocket;

    /**
     * @description Creates a UDP socket and registers a handler for the 'message' event
     *
     *      Equivalent to createSocket(opts) followed by `socket.on('message', callback)`: the callback
     *      receives the payload Buffer and the remote-info object, and this is the receive-only server
     *      form used when no other event needs a listener. The callback is installed before the socket
     *      is bound, so no datagram can be missed. The opts argument accepts the same family string and
     *      options object as the overload above.
     *
     *      Example — the callback receives every datagram:
     *      ```JavaScript
     *      const dgram = require('dgram');
     *      const coroutine = require('coroutine');
     *
     *      let payload = null;
     *      const socket = dgram.createSocket('udp4', (msg, rinfo) => {
     *          payload = msg.toString() + '@' + rinfo.address;
     *      });
     *      socket.bind(0, '127.0.0.1');
     *
     *      const peer = dgram.createSocket('udp4');
     *      peer.send('ping', socket.address().port, '127.0.0.1');
     *
     *      let waited = 0;
     *      while (payload === null && waited < 1000) {
     *          coroutine.sleep(10);
     *          waited += 10;
     *      }
     *      console.log(payload); // ping@127.0.0.1
     *
     *      peer.close();
     *      socket.close();
     *      ```
     *      @param opts the family string or the options object
     *      @param callback the function called with (msg, rinfo) for every received datagram
     *      @return the created socket
     *
     */
    function createSocket(opts: FIBJS.GeneralObject | string, callback: (msg: Class_Buffer, rinfo: FIBJS.GeneralObject)=>void): Class_DgramSocket;

}

