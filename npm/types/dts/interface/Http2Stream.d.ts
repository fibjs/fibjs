/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description one HTTP/2 stream: an independent request/response carried by a session, a duplex Stream
 *
 *  Http2Stream is a duplex Stream (read/write/close plus the inherited stream events),
 *  multiplexed with other streams on the same Http2Session. A client creates one with
 *  Http2Session.request; a server receives one with the `stream` event of the session.
 *  Data written is sent in DATA frames and data received is returned by read()/readAll();
 *  the header block received on the stream is exposed through `headers` and the
 *  `headers`/`trailers` events.
 *
 *  Concepts:
 *
 *  - **Request and response sides**: on a client stream, `headers` and the `headers`
 *    event carry the response headers once they arrive; on a server stream they carry the
 *    request headers. A server answers with respond() (a missing `:status` defaults to
 *    200), writes the body and ends with close(); a client reads the body with
 *    read()/readAll() or the `data` event inherited from Stream.
 *  - **States and ids**: `id` is odd for client-initiated streams and even for
 *    server-initiated ones. `closed` becomes true when both sides have ended (END_STREAM)
 *    or after close()/RST_STREAM; `destroyed` marks a stream killed by destroy()/reset.
 *    After a normal end, read() returns null.
 *  - **Headers and trailers**: response headers are sent by respond(), informational 1xx
 *    headers by additionalHeaders() and trailing headers by sendTrailers() before close();
 *    the receiving side gets them through the `headers`/`trailers` events as plain objects.
 *  - **Errors and resets**: rstStream(code) sends RST_STREAM with an error code from
 *    http2_constants; the peer's read() then throws, while a stream killed by a session
 *    destroy() reads null. The remote error payload of a reset is not populated in the
 *    current implementation (the thrown error is `[0] Success`).
 *  - **Node.js differences**: there is no `pushStream()`, no writable `end` overload set
 *    with callbacks and no `headersSent`/`sentHeaders` properties; the request-body
 *    limitation described in Http2Session.request applies to client streams.
 *
 *  Obtained from:
 *  - `session.request(headers, options)` — the client stream of a request;
 *  - the `stream` event of an Http2Session — the server stream of an accepted request.
 *
 *  Example 1 — a client stream reads the status, the headers and the body:
 *  ```JavaScript
 *  const http2 = require('http2');
 *  const tls = require('tls');
 *  const crypto = require('crypto');
 *
 *  const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const ca = crypto.createCertificateRequest({
 *      key: caKey.privateKey, subject: { CN: 'fibjs.org' }
 *  }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
 *  const crt = crypto.createCertificateRequest({
 *      key: srvKey.privateKey, subject: { CN: 'localhost' }
 *  }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
 *  const ctx = tls.createSecureContext({
 *      key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
 *  }, true);
 *
 *  const server = new http2.Server(ctx, 0, function () { });
 *  server.on('session', (session) => {
 *      session.on('stream', (stream, headers) => {
 *          stream.respond({ ':status': 200, 'content-type': 'text/plain' });
 *          stream.write('chunk-1;chunk-2');
 *          stream.close();
 *      });
 *  });
 *  server.start();
 *
 *  const session = http2.connect('https://localhost:' + server.socket.localPort, {
 *      rejectUnauthorized: false, rejectUnverified: false
 *  });
 *  const stream = session.request({ ':method': 'GET', ':path': '/data' });
 *  console.log(stream.readAll().toString()); // chunk-1;chunk-2
 *  console.log(stream.headers[':status']); // 200
 *  console.log(stream.closed, stream.destroyed); // true false
 *
 *  session.close();
 *  server.stop();
 *  ```
 *
 *  Example 2 — a server stream sends headers, body and trailers:
 *  ```JavaScript
 *  const http2 = require('http2');
 *  const tls = require('tls');
 *  const crypto = require('crypto');
 *
 *  const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const ca = crypto.createCertificateRequest({
 *      key: caKey.privateKey, subject: { CN: 'fibjs.org' }
 *  }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
 *  const crt = crypto.createCertificateRequest({
 *      key: srvKey.privateKey, subject: { CN: 'localhost' }
 *  }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
 *  const ctx = tls.createSecureContext({
 *      key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
 *  }, true);
 *
 *  const server = new http2.Server(ctx, 0, function () { });
 *  server.on('session', (session) => {
 *      session.on('stream', (stream, headers) => {
 *          stream.respond({ ':status': 200 });
 *          stream.write('body for ' + headers[':path']);
 *          stream.sendTrailers({ 'x-checksum': 'abc' });
 *          stream.close();
 *      });
 *  });
 *  server.start();
 *
 *  const session = http2.connect('https://localhost:' + server.socket.localPort, {
 *      rejectUnauthorized: false, rejectUnverified: false
 *  });
 *  const stream = session.request({ ':method': 'GET', ':path': '/trail' });
 *  const trailers = [];
 *  stream.on('trailers', (headers) => trailers.push(headers));
 *  console.log(stream.readAll().toString()); // body for /trail
 *  console.log(trailers[0]['x-checksum']); // abc
 *
 *  session.close();
 *  server.stop();
 *  ```
 *
 */
declare class Class_Http2Stream extends Class_Stream {
    /**
     * @description queries the numeric stream identifier
     *
     *      Client-initiated streams use odd ids, server-initiated streams even ids; the id is
     *      assigned at creation and never changes. It is accepted as the `lastStreamId`
     *      argument of Http2Session.goaway and appears in GOAWAY/RST_STREAM frames.
     *
     */
    readonly id: number;

    /**
     * @description queries whether the stream is closed
     *
     *      True when both directions have ended (END_STREAM from both peers), when close() was
     *      called, or when the stream was reset/destroyed. A closed stream refuses writes and
     *      its read() returns null (it throws instead when the stream was reset with an error).
     *
     */
    readonly closed: boolean;

    /**
     * @description queries whether the stream was forcibly destroyed
     *
     *      True after the session is destroyed or the stream was reset; a destroyed stream is
     *      also closed and reading it returns null or throws. A stream that ended normally is
     *      closed but not destroyed.
     *
     */
    readonly destroyed: boolean;

    /**
     * @description queries the headers received on this stream
     *
     *      A plain object whose keys are the header names: on a client stream the response
     *      headers (available after the first read() or the `headers` event), on a server
     *      stream the request headers (also passed to the session `stream` event). Pseudo-
     *      headers keep their colon prefix (`:status`, `:method`, ...). The value is null
     *      before the header block arrives.
     *
     */
    readonly headers: FIBJS.GeneralObject;

    /**
     * @description sends the response headers to the peer (server stream)
     *
     *      headers is a plain object with header name-value pairs; when `:status` is missing it
     *      is sent as 200. respond() must be called before the first write() and only once per
     *      stream: a second call, or a call on a closed/destroyed stream, throws
     *      `Http2Stream: stream is closed.`. After respond() the body is written with
     *      write()/end() and finished with close(); additionalHeaders() sends informational 1xx
     *      headers before respond().
     *
     *      Example — answer every request and echo the path:
     *      ```JavaScript
     *      const http2 = require('http2');
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const ca = crypto.createCertificateRequest({
     *          key: caKey.privateKey, subject: { CN: 'fibjs.org' }
     *      }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
     *      const crt = crypto.createCertificateRequest({
     *          key: srvKey.privateKey, subject: { CN: 'localhost' }
     *      }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
     *      const ctx = tls.createSecureContext({
     *          key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
     *      }, true);
     *
     *      const server = new http2.Server(ctx, 0, function () { });
     *      server.on('session', (session) => {
     *          session.on('stream', (stream, headers) => {
     *              stream.respond({ ':status': 200, 'x-echo': headers[':path'] });
     *              stream.write('ok:' + headers[':path']);
     *              stream.close();
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/echo' });
     *      console.log(stream.read().toString()); // ok:/echo
     *      console.log(stream.headers['x-echo']); // /echo
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param headers an object containing header name-value pairs
     *
     */
    respond(headers?: FIBJS.GeneralObject): void;

    /**
     * @description sends informational (1xx) headers to the peer (server stream)
     *
     *      Must be called before respond(); the fields are submitted as an informational HEADERS
     *      frame, typically `:status: 103` together with Link headers. On the client side the
     *      block is delivered as a `headers` event; the following final response arrives as a
     *      `trailers` event in the current implementation, because the receiving stack treats
     *      the second header block as a trailer, so do not rely on `stream.headers` for the
     *      final status after using additionalHeaders().
     *
     *      @param headers an object containing header name-value pairs
     *
     */
    additionalHeaders(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends trailing headers at the end of the stream (server stream)
     *
     *      Call after respond()/write() and before (or instead of) close(); the trailers are
     *      sent in a HEADERS frame after the body, and the receiving side emits the `trailers`
     *      event with them. Throws when the stream is closed or destroyed.
     *
     *      Example — attach a checksum trailer to a response:
     *      ```JavaScript
     *      const http2 = require('http2');
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const ca = crypto.createCertificateRequest({
     *          key: caKey.privateKey, subject: { CN: 'fibjs.org' }
     *      }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
     *      const crt = crypto.createCertificateRequest({
     *          key: srvKey.privateKey, subject: { CN: 'localhost' }
     *      }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
     *      const ctx = tls.createSecureContext({
     *          key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
     *      }, true);
     *
     *      const server = new http2.Server(ctx, 0, function () { });
     *      server.on('session', (session) => {
     *          session.on('stream', (stream, headers) => {
     *              stream.respond({ ':status': 200, 'content-type': 'text/plain' });
     *              stream.write('hello');
     *              stream.sendTrailers({ 'x-checksum': 'abc123' });
     *              stream.close();
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/' });
     *      const trailers = [];
     *      stream.on('trailers', (headers) => trailers.push(headers));
     *      console.log(stream.readAll().toString()); // hello
     *      console.log(trailers[0]['x-checksum']); // abc123
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param headers an object containing trailing header name-value pairs
     *
     */
    sendTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends an RST_STREAM frame to cancel the stream
     *
     *      code is an HTTP/2 error code (NGHTTP2_NO_ERROR, NGHTTP2_CANCEL, ...; see
     *      http2_constants). The stream ends immediately: the peer's read() throws (the current
     *      implementation leaves the remote error payload empty, `[0] Success`), and a local
     *      read() still waiting for data throws `Http2Stream: stream reset` (number 20024).
     *      Throws when the stream is already destroyed.
     *
     *      Example — cancel a stream instead of answering it:
     *      ```JavaScript
     *      const http2 = require('http2');
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const ca = crypto.createCertificateRequest({
     *          key: caKey.privateKey, subject: { CN: 'fibjs.org' }
     *      }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
     *      const crt = crypto.createCertificateRequest({
     *          key: srvKey.privateKey, subject: { CN: 'localhost' }
     *      }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
     *      const ctx = tls.createSecureContext({
     *          key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
     *      }, true);
     *
     *      const server = new http2.Server(ctx, 0, function () { });
     *      server.on('session', (session) => {
     *          session.on('stream', (stream, headers) => {
     *              stream.respond({ ':status': 200 });
     *              stream.rstStream(http2.constants.NGHTTP2_CANCEL);
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/cancel' });
     *      try {
     *          stream.read();
     *      } catch (e) {
     *          console.log('read failed after RST_STREAM');
     *      }
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param code RST_STREAM error code, default is NGHTTP2_NO_ERROR (0)
     *
     */
    rstStream(code?: number): void;

    /**
     * @description emitted when a header block is received
     *
     *      On a client stream the argument contains the response headers once per stream,
     *      before the body; on a server stream it contains the request headers (also passed to
     *      the session `stream` event). Informational 1xx blocks are delivered here too when
     *      additionalHeaders() was used.
     *
     *      @param headers response headers object
     *
     */
    on(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    once(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    off(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "headers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    /**
     * @description emitted when a header block is received
     *
     *      On a client stream the argument contains the response headers once per stream,
     *      before the body; on a server stream it contains the request headers (also passed to
     *      the session `stream` event). Informational 1xx blocks are delivered here too when
     *      additionalHeaders() was used.
     *
     *      @param headers response headers object
     *
     */
    onheaders: ((headers: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description emitted when a trailing header block is received
     *
     *      The argument contains the trailer fields sent with sendTrailers(). Note that when a
     *      server used additionalHeaders(), the final response block is delivered here as well
     *      (see additionalHeaders).
     *
     *      @param headers trailing headers object
     *
     */
    on(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    once(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    off(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "trailers", listener: (headers: FIBJS.GeneralObject)=>void): this;

    /**
     * @description emitted when a trailing header block is received
     *
     *      The argument contains the trailer fields sent with sendTrailers(). Note that when a
     *      server used additionalHeaders(), the final response block is delivered here as well
     *      (see additionalHeaders).
     *
     *      @param headers trailing headers object
     *
     */
    ontrailers: ((headers: FIBJS.GeneralObject)=>void) | null;

    on(event: "data", listener: (data: Class_Buffer)=>void): this;

    on(event: "close", listener: ()=>void): this;

    on(event: "error", listener: (code: number)=>void): this;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(event: "data", listener: (data: Class_Buffer)=>void): this;

    once(event: "close", listener: ()=>void): this;

    once(event: "error", listener: (code: number)=>void): this;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(event: "data", listener: (data: Class_Buffer)=>void): this;

    off(event: "close", listener: ()=>void): this;

    off(event: "error", listener: (code: number)=>void): this;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    addListener(event: "error", listener: (code: number)=>void): this;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    removeListener(event: "error", listener: (code: number)=>void): this;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependListener(event: "error", listener: (code: number)=>void): this;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: (code: number)=>void): this;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the Http2Stream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_Http2StreamPromise extends Class_StreamPromise {
    /**
     * @description queries the numeric stream identifier
     *
     *      Client-initiated streams use odd ids, server-initiated streams even ids; the id is
     *      assigned at creation and never changes. It is accepted as the `lastStreamId`
     *      argument of Http2Session.goaway and appears in GOAWAY/RST_STREAM frames.
     *
     */
    readonly id: number;

    /**
     * @description queries whether the stream is closed
     *
     *      True when both directions have ended (END_STREAM from both peers), when close() was
     *      called, or when the stream was reset/destroyed. A closed stream refuses writes and
     *      its read() returns null (it throws instead when the stream was reset with an error).
     *
     */
    readonly closed: boolean;

    /**
     * @description queries whether the stream was forcibly destroyed
     *
     *      True after the session is destroyed or the stream was reset; a destroyed stream is
     *      also closed and reading it returns null or throws. A stream that ended normally is
     *      closed but not destroyed.
     *
     */
    readonly destroyed: boolean;

    /**
     * @description queries the headers received on this stream
     *
     *      A plain object whose keys are the header names: on a client stream the response
     *      headers (available after the first read() or the `headers` event), on a server
     *      stream the request headers (also passed to the session `stream` event). Pseudo-
     *      headers keep their colon prefix (`:status`, `:method`, ...). The value is null
     *      before the header block arrives.
     *
     */
    readonly headers: FIBJS.GeneralObject;

    /**
     * @description sends the response headers to the peer (server stream)
     *
     *      headers is a plain object with header name-value pairs; when `:status` is missing it
     *      is sent as 200. respond() must be called before the first write() and only once per
     *      stream: a second call, or a call on a closed/destroyed stream, throws
     *      `Http2Stream: stream is closed.`. After respond() the body is written with
     *      write()/end() and finished with close(); additionalHeaders() sends informational 1xx
     *      headers before respond().
     *
     *      Example — answer every request and echo the path:
     *      ```JavaScript
     *      const http2 = require('http2');
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const ca = crypto.createCertificateRequest({
     *          key: caKey.privateKey, subject: { CN: 'fibjs.org' }
     *      }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
     *      const crt = crypto.createCertificateRequest({
     *          key: srvKey.privateKey, subject: { CN: 'localhost' }
     *      }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
     *      const ctx = tls.createSecureContext({
     *          key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
     *      }, true);
     *
     *      const server = new http2.Server(ctx, 0, function () { });
     *      server.on('session', (session) => {
     *          session.on('stream', (stream, headers) => {
     *              stream.respond({ ':status': 200, 'x-echo': headers[':path'] });
     *              stream.write('ok:' + headers[':path']);
     *              stream.close();
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/echo' });
     *      console.log(stream.read().toString()); // ok:/echo
     *      console.log(stream.headers['x-echo']); // /echo
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param headers an object containing header name-value pairs
     *
     */
    respond(headers?: FIBJS.GeneralObject): void;

    /**
     * @description sends informational (1xx) headers to the peer (server stream)
     *
     *      Must be called before respond(); the fields are submitted as an informational HEADERS
     *      frame, typically `:status: 103` together with Link headers. On the client side the
     *      block is delivered as a `headers` event; the following final response arrives as a
     *      `trailers` event in the current implementation, because the receiving stack treats
     *      the second header block as a trailer, so do not rely on `stream.headers` for the
     *      final status after using additionalHeaders().
     *
     *      @param headers an object containing header name-value pairs
     *
     */
    additionalHeaders(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends trailing headers at the end of the stream (server stream)
     *
     *      Call after respond()/write() and before (or instead of) close(); the trailers are
     *      sent in a HEADERS frame after the body, and the receiving side emits the `trailers`
     *      event with them. Throws when the stream is closed or destroyed.
     *
     *      Example — attach a checksum trailer to a response:
     *      ```JavaScript
     *      const http2 = require('http2');
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const ca = crypto.createCertificateRequest({
     *          key: caKey.privateKey, subject: { CN: 'fibjs.org' }
     *      }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
     *      const crt = crypto.createCertificateRequest({
     *          key: srvKey.privateKey, subject: { CN: 'localhost' }
     *      }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
     *      const ctx = tls.createSecureContext({
     *          key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
     *      }, true);
     *
     *      const server = new http2.Server(ctx, 0, function () { });
     *      server.on('session', (session) => {
     *          session.on('stream', (stream, headers) => {
     *              stream.respond({ ':status': 200, 'content-type': 'text/plain' });
     *              stream.write('hello');
     *              stream.sendTrailers({ 'x-checksum': 'abc123' });
     *              stream.close();
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/' });
     *      const trailers = [];
     *      stream.on('trailers', (headers) => trailers.push(headers));
     *      console.log(stream.readAll().toString()); // hello
     *      console.log(trailers[0]['x-checksum']); // abc123
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param headers an object containing trailing header name-value pairs
     *
     */
    sendTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description sends an RST_STREAM frame to cancel the stream
     *
     *      code is an HTTP/2 error code (NGHTTP2_NO_ERROR, NGHTTP2_CANCEL, ...; see
     *      http2_constants). The stream ends immediately: the peer's read() throws (the current
     *      implementation leaves the remote error payload empty, `[0] Success`), and a local
     *      read() still waiting for data throws `Http2Stream: stream reset` (number 20024).
     *      Throws when the stream is already destroyed.
     *
     *      Example — cancel a stream instead of answering it:
     *      ```JavaScript
     *      const http2 = require('http2');
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const ca = crypto.createCertificateRequest({
     *          key: caKey.privateKey, subject: { CN: 'fibjs.org' }
     *      }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
     *      const crt = crypto.createCertificateRequest({
     *          key: srvKey.privateKey, subject: { CN: 'localhost' }
     *      }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
     *      const ctx = tls.createSecureContext({
     *          key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false, alpnProtocols: ['h2']
     *      }, true);
     *
     *      const server = new http2.Server(ctx, 0, function () { });
     *      server.on('session', (session) => {
     *          session.on('stream', (stream, headers) => {
     *              stream.respond({ ':status': 200 });
     *              stream.rstStream(http2.constants.NGHTTP2_CANCEL);
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/cancel' });
     *      try {
     *          stream.read();
     *      } catch (e) {
     *          console.log('read failed after RST_STREAM');
     *      }
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param code RST_STREAM error code, default is NGHTTP2_NO_ERROR (0)
     *
     */
    rstStream(code?: number): void;

    /**
     * @description emitted when a header block is received
     *
     *      On a client stream the argument contains the response headers once per stream,
     *      before the body; on a server stream it contains the request headers (also passed to
     *      the session `stream` event). Informational 1xx blocks are delivered here too when
     *      additionalHeaders() was used.
     *
     *      @param headers response headers object
     *
     */
    onheaders: ((headers: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description emitted when a trailing header block is received
     *
     *      The argument contains the trailer fields sent with sendTrailers(). Note that when a
     *      server used additionalHeaders(), the final response block is delivered here as well
     *      (see additionalHeaders).
     *
     *      @param headers trailing headers object
     *
     */
    ontrailers: ((headers: FIBJS.GeneralObject)=>void) | null;

}


declare namespace Class_Http2Stream {
    const promises: FIBJS.GeneralObject;
}
