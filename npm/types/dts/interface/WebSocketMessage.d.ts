/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/**
 * @description The message object exchanged by WebSocket peers, a Message with frame metadata
 *
 *  A WebSocketMessage bundles one complete application message: the frame type
 *  (`type`, inherited from Message), the WebSocket flags `masked` and
 *  `compress`, the size limit `maxSize` and the payload held by the Message
 *  body. The `message` event of a WebSocket delivers an instance directly, and
 *  `data` reads the payload in its natural JavaScript form.
 *
 *  The class is reachable as `WebSocket.Message`: `new WebSocket.Message()`
 *  creates an empty message that can be filled with `write`, serialized with
 *  `sendTo` and parsed back with `readFrom`. Messages received from a socket
 *  are created by the protocol layer, so application code mostly reads `type`
 *  and `data`.
 *
 *  Concepts:
 *  - type: TEXT (1) or BINARY (2) for application messages. PING (9), PONG
 *    (10) and CLOSE (8) frames are handled by the WebSocket protocol layer and
 *    are not delivered to `message`; CONTINUE (0) fragments are re-assembled
 *    before delivery.
 *  - data: a String for TEXT messages and a Buffer for BINARY messages. The
 *    getter reads the body from the beginning on every access, so it can be
 *    called more than once.
 *  - masked: on a received frame it reports the mask bit of the wire frame,
 *    true for frames sent by a client and false for frames sent by a server;
 *    on an outgoing message it selects whether the frame is masked, as the
 *    protocol requires for each role.
 *  - compress: true when the message was compressed with permessage-deflate,
 *    negotiated by the `perMessageDeflate` option of the WebSocket constructor
 *    or of WebSocket.upgrade; it is forced to false for frames that never
 *    carry compressed payloads.
 *  - maxSize: the maximum accepted message size in bytes, 67108864 (64 MB) by
 *    default. A larger incoming message fails the connection: the `error`
 *    event reports code 1009 and `close` reports 1006.
 *  - Frame serialization: `sendTo` writes a message to a stream and `readFrom`
 *    parses one back, which makes it possible to produce or verify the wire
 *    format without a socket (see the examples).
 *
 *  Obtained from:
 *  - the `message` event of a WebSocket (`msg` argument);
 *  - `new WebSocket.Message(type, masked, compress, maxSize)` for protocol
 *    work such as tests or custom transports.
 *
 *  Example 1 — inspect the messages received by a server:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, {
 *      '/ws': WebSocket.upgrade((conn) => {
 *          conn.onmessage = (msg) => {
 *              console.log(msg.type, msg.compress, typeof msg.data); // 1 false string
 *              conn.send(msg.data);
 *          };
 *      })
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const sock = new WebSocket('ws://127.0.0.1:' + port + '/ws');
 *  sock.onopen = () => sock.send('inspect me');
 *  sock.onmessage = (msg) => {
 *      console.log(msg.type === WebSocket.TEXT, msg.data); // true inspect me
 *      sock.close();
 *  };
 *  sock.onclose = () => server.stop();
 *  ```
 *
 *  Example 2 — build, serialize and parse a message in memory:
 *  ```JavaScript
 *  const io = require('io');
 *
 *  const out = new io.MemoryStream();
 *  const msg = new WebSocket.Message(WebSocket.TEXT, true, false);
 *  msg.write('payload', () => {
 *      msg.sendTo(out, () => {
 *          out.rewind();
 *          const back = new WebSocket.Message();
 *          back.readFrom(out, () => {
 *              console.log(back.type === WebSocket.TEXT, back.data); // true payload
 *              console.log(back.masked, back.compress); // true false
 *          });
 *      });
 *  });
 *  ```
 *
 */
declare class Class_WebSocketMessage extends Class_Message {
    /**
     * @description Creates an empty message with the given frame metadata
     *
     *      The constructor initializes the frame header fields only; the payload is
     *      written afterwards with `write`. compress is forced to false for frame
     *      types other than TEXT and BINARY. Incoming messages are created by the
     *      protocol layer, so build messages by hand to serialize frames in memory
     *      with `sendTo` or to parse them with `readFrom`, as the class examples do.
     *
     *      @param type the frame type, WebSocket.BINARY by default
     *      @param masked whether the frame is masked, true by default (clients mask frames)
     *      @param compress whether the frame is compressed with permessage-deflate, false by default
     *      @param maxSize the maximum accepted message size in bytes, 67108864 (64 MB) by default
     *
     */
    constructor(type?: number, masked?: boolean, compress?: boolean, maxSize?: number);

    /**
     * @description Queries or sets the mask flag of the frame
     *
     *      On a received message it reports the mask bit of the wire frame: true
     *      for a frame sent by a client and false for a frame sent by a server. On
     *      an outgoing message it selects whether the payload is masked, following
     *      the protocol rule that clients mask and servers do not. Default: true.
     *
     */
    masked: boolean;

    /**
     * @description Queries or sets the compression flag of the frame
     *
     *      true when the payload is compressed with permessage-deflate; the
     *      extension must be negotiated by the connection (see the
     *      `perMessageDeflate` option of the WebSocket constructor and of
     *      WebSocket.upgrade), otherwise every message reports false. The
     *      constructor forces the flag to false for frame types other than TEXT
     *      and BINARY. Default: false.
     *
     */
    compress: boolean;

    /**
     * @description Queries or sets the maximum accepted message size in bytes
     *
     *      Default: 67108864 (64 MB), the same limit the WebSocket `maxPayload`
     *      option sets for a connection. A received message larger than the limit
     *      fails the connection: the `error` event reports code 1009 and `close`
     *      reports 1006. Setting a negative value throws.
     *
     */
    maxSize: number;

    /**
     * @description Reads the message payload: a String for TEXT messages, a Buffer for binary messages
     *
     *      The getter reads the body from the beginning on every access, so it can
     *      be called more than once, and returns null when the message has no
     *      payload. This property is a fibjs extension: the DOM MessageEvent.data
     *      may be a Blob or an ArrayBuffer instead, and it only exists on events.
     *
     *      Example — the same property in text and binary form:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, {
     *          '/ws': WebSocket.upgrade((conn) => {
     *              conn.onmessage = (msg) => conn.send(msg.data);
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const sock = new WebSocket('ws://127.0.0.1:' + port + '/ws');
     *      let step = 0;
     *      sock.onopen = () => sock.send(Buffer.from('bytes'));
     *      sock.onmessage = (msg) => {
     *          console.log(Buffer.isBuffer(msg.data), msg.data.toString()); // true bytes
     *          if (step++ === 0)
     *              sock.send('text'); // the next message comes back as a string
     *          else
     *              sock.close();
     *      };
     *      sock.onclose = () => server.stop();
     *      ```
     *
     */
    readonly data: any;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/**
 * The promise variant of the WebSocketMessage class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_WebSocketMessagePromise extends Class_MessagePromise {
    /**
     * @description Creates an empty message with the given frame metadata
     *
     *      The constructor initializes the frame header fields only; the payload is
     *      written afterwards with `write`. compress is forced to false for frame
     *      types other than TEXT and BINARY. Incoming messages are created by the
     *      protocol layer, so build messages by hand to serialize frames in memory
     *      with `sendTo` or to parse them with `readFrom`, as the class examples do.
     *
     *      @param type the frame type, WebSocket.BINARY by default
     *      @param masked whether the frame is masked, true by default (clients mask frames)
     *      @param compress whether the frame is compressed with permessage-deflate, false by default
     *      @param maxSize the maximum accepted message size in bytes, 67108864 (64 MB) by default
     *
     */
    constructor(type?: number, masked?: boolean, compress?: boolean, maxSize?: number);

    /**
     * @description Queries or sets the mask flag of the frame
     *
     *      On a received message it reports the mask bit of the wire frame: true
     *      for a frame sent by a client and false for a frame sent by a server. On
     *      an outgoing message it selects whether the payload is masked, following
     *      the protocol rule that clients mask and servers do not. Default: true.
     *
     */
    masked: boolean;

    /**
     * @description Queries or sets the compression flag of the frame
     *
     *      true when the payload is compressed with permessage-deflate; the
     *      extension must be negotiated by the connection (see the
     *      `perMessageDeflate` option of the WebSocket constructor and of
     *      WebSocket.upgrade), otherwise every message reports false. The
     *      constructor forces the flag to false for frame types other than TEXT
     *      and BINARY. Default: false.
     *
     */
    compress: boolean;

    /**
     * @description Queries or sets the maximum accepted message size in bytes
     *
     *      Default: 67108864 (64 MB), the same limit the WebSocket `maxPayload`
     *      option sets for a connection. A received message larger than the limit
     *      fails the connection: the `error` event reports code 1009 and `close`
     *      reports 1006. Setting a negative value throws.
     *
     */
    maxSize: number;

    /**
     * @description Reads the message payload: a String for TEXT messages, a Buffer for binary messages
     *
     *      The getter reads the body from the beginning on every access, so it can
     *      be called more than once, and returns null when the message has no
     *      payload. This property is a fibjs extension: the DOM MessageEvent.data
     *      may be a Blob or an ArrayBuffer instead, and it only exists on events.
     *
     *      Example — the same property in text and binary form:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, {
     *          '/ws': WebSocket.upgrade((conn) => {
     *              conn.onmessage = (msg) => conn.send(msg.data);
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const sock = new WebSocket('ws://127.0.0.1:' + port + '/ws');
     *      let step = 0;
     *      sock.onopen = () => sock.send(Buffer.from('bytes'));
     *      sock.onmessage = (msg) => {
     *          console.log(Buffer.isBuffer(msg.data), msg.data.toString()); // true bytes
     *          if (step++ === 0)
     *              sock.send('text'); // the next message comes back as a string
     *          else
     *              sock.close();
     *      };
     *      sock.onclose = () => server.stop();
     *      ```
     *
     */
    readonly data: any;

}


declare namespace Class_WebSocketMessage {
    const promises: FIBJS.GeneralObject;
}
