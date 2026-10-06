/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/TLSSocket.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description A TLS server: a fiber-per-connection TCP server whose listener receives an encrypted TLSSocket
 *
 *  TLSServer combines net.TcpServer with a TLSHandler: it owns the secure context and the listener,
 *  binds the port, performs the handshake of every accepted connection and then invokes the
 *  listener with the resulting TLSSocket. It is logically equivalent to:
 *  ```JavaScript
 *  // fragment: logical equivalent of the class
 *  const tls = require('tls');
 *  const net = require('net');
 *
 *  const server = new net.TcpServer(port, new tls.Handler(ctx, (conn) => {
 *      // conn is a TLSSocket
 *  }));
 *  server.start();
 *  ```
 *
 *  Concepts:
 *
 *  - **Context**: the TLS configuration comes either from a ready SecureContext or from an options
 *    object passed to tls.createSecureContext with isServer true; the constructor options object
 *    additionally reads `address` and `port` for the bind. setSecureContext() replaces the
 *    configuration used by the connections accepted afterwards.
 *  - **Lifecycle**: the constructors with a port bind immediately and start() begins accepting,
 *    while the forms without a port only store the listener and need listen(). stop()/close()
 *    closes the listening socket, emits 'close' immediately and leaves the accepted connections in
 *    their handler fibers untouched.
 *  - **Handshake and errors**: every connection performs the handshake before the listener runs, so
 *    the listener always receives a connected TLSSocket. A failed handshake is logged by the server
 *    and the raw connection is closed; it is not thrown to the caller, and the remaining
 *    connections continue to be served.
 *  - **Events**: inherited from net.TcpServer - 'listening' after start()/listen(), 'connection'
 *    with the raw accepted Socket before the handshake, 'error' with a message string and 'close'
 *    on stop(). Node.js tls.Server reports the established session through 'secureConnection'
 *    instead and passes an Error to 'error'.
 *  - **Node.js differences**: the class is exported as tls.Server (not tls.TLSServer), the
 *    constructor may bind the port, SNI is configured on the context (setSNIContext) instead of
 *    server.addContext(), and there is no getTicketKeys/setTicketKeys, maxConnections or unref.
 *
 *  Obtained from:
 *  - `tls.createServer(context|options, listener)` — the factory form, no port bound;
 *  - `new tls.Server(context|options, [addr,] port, listener)` — binds the port immediately;
 *  - `new tls.Server(context|options, listener)` — deferred, call listen() to bind.
 *
 *  Example 1 — createServer() plus listen() on an OS-assigned port:
 *  ```JavaScript
 *  const tls = require('tls');
 *  const crypto = require('crypto');
 *
 *  const pk = crypto.generateKeyPair('ec', { namedCurve: 'secp256r1' });
 *  const cert = crypto.createCertificateRequest({
 *      key: pk.privateKey,
 *      subject: { CN: 'localhost' }
 *  }).issue({
 *      key: pk.privateKey,
 *      issuer: { CN: 'localhost' },
 *      validFrom: new Date(Date.now() - 1000),
 *      days: 1
 *  });
 *
 *  // createServer() only stores the options and the listener: listen() binds the
 *  // port, and port 0 asks the operating system for a free one
 *  const server = tls.createServer({ key: pk.privateKey, cert }, (conn) => {
 *      conn.write(conn.read());
 *      conn.close();
 *  });
 *  server.listen(0, '127.0.0.1');
 *  console.log(server.address().port > 0); // true
 *
 *  const client = tls.connect(server.address().port, 'localhost', { ca: cert.pem });
 *  client.write('hello server');
 *  console.log(client.read().toString()); // hello server
 *
 *  client.close();
 *  server.stop();
 *  ```
 *
 *  Example 2 — the deferred class form with lifecycle events:
 *  ```JavaScript
 *  const tls = require('tls');
 *  const crypto = require('crypto');
 *
 *  const pk = crypto.generateKeyPair('ec', { namedCurve: 'secp256r1' });
 *  const cert = crypto.createCertificateRequest({
 *      key: pk.privateKey,
 *      subject: { CN: 'localhost' }
 *  }).issue({
 *      key: pk.privateKey,
 *      issuer: { CN: 'localhost' },
 *      validFrom: new Date(Date.now() - 1000),
 *      days: 1
 *  });
 *  const ctx = tls.createSecureContext({ key: pk.privateKey, cert }, true);
 *
 *  // the deferred class form: no port is bound until listen(), so lifecycle
 *  // events can be registered first
 *  const server = new tls.Server(ctx, (conn) => {
 *      conn.write(conn.read());
 *      conn.close();
 *  });
 *  server.on('listening', () => console.log('listening'));
 *  server.on('connection', () => console.log('connection'));
 *  server.on('close', () => console.log('closed'));
 *  server.listen(0, '127.0.0.1');
 *
 *  const client = tls.connect(server.address().port, 'localhost', { ca: cert.pem });
 *  client.write('events');
 *  console.log(client.read().toString()); // events
 *
 *  client.close();
 *  server.stop(); // closed
 *  ```
 *
 */
declare class Class_TLSServer extends Class_TcpServer {
    /**
     * @description Creates a TLS server and binds a port on all local addresses
     *
     *      The port is bound during construction and start() begins accepting; port 0 asks the
     *      operating system for a free one, read it from address() or socket.localPort afterwards. The
     *      listener forms are described on the class page; a listener function receives one TLSSocket
     *      per accepted connection in its own fiber.
     *      @param context specifies the secure context used to create TLSServer
     *      @param port specifies the listening port
     *      @param listener the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates a TLS server bound to the given address and port
     *
     *      addr is an IP literal such as '127.0.0.1'; an empty string listens on all local addresses.
     *      The port is bound during construction and start() begins accepting.
     *      @param context specifies the secure context used to create TLSServer
     *      @param addr specifies the listening address
     *      @param port specifies the listening port
     *      @param listener the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates a TLS server from an options object, binding when it carries a port
     *
     *      The TLS keys are passed to tls.createSecureContext with isServer true, so cert and key are
     *      usually required for a usable server; in addition `address` (default all local addresses)
     *      and `port` are read for the bind. With a port the server binds immediately and start()
     *      begins accepting; without one it only stores the listener and listen() must be called. The
     *      listener forms are described on the class page.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param listener the connection handler
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates a TLS server without binding a port; listen() must be called to start
     *
     *      The deferred form stores the context and the listener, so lifecycle events can be registered
     *      before listen(port[, addr[, backlog]]) binds and starts. address() throws until the server
     *      is bound.
     *      @param context specifies the secure context used to create TLSServer
     *      @param listener the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description The SecureContext used by the server
     *
     *      The same object as the context of the internal handler; it applies to the connections
     *      accepted from now on, so replacing it through setSecureContext() affects the next
     *      connections only.
     *
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description Replaces the SecureContext used for the connections accepted afterwards
     *
     *      Connections that already started their handshake keep the context they began with; the new
     *      context applies to the next accepted connection, which makes certificate rotation possible
     *      without restarting the listener.
     *
     *      Example — rotating the certificate between two connections:
     *      ```JavaScript
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      // two certificates with different subjects, served in turn
     *      function selfSigned(cn) {
     *          const pk = crypto.generateKeyPair('ec', { namedCurve: 'secp256r1' });
     *          const cert = crypto.createCertificateRequest({
     *              key: pk.privateKey,
     *              subject: { CN: cn }
     *          }).issue({
     *              key: pk.privateKey,
     *              issuer: { CN: cn },
     *              validFrom: new Date(Date.now() - 1000),
     *              days: 1
     *          });
     *          return { pk, cert };
     *      }
     *      const first = selfSigned('localhost');
     *      const second = selfSigned('other.local');
     *
     *      const server = tls.createServer({ key: first.pk.privateKey, cert: first.cert }, (conn) => {
     *          conn.write(conn.secureContext.cert.subject);
     *          conn.close();
     *      });
     *      server.listen(0, '127.0.0.1');
     *
     *      // setSecureContext() replaces the context used by the connections accepted
     *      // afterwards; existing connections keep the context they started with
     *      let client = tls.connect(server.address().port, '127.0.0.1', { requestCert: false });
     *      console.log(client.read().toString()); // CN=localhost
     *
     *      server.setSecureContext({ key: second.pk.privateKey, cert: second.cert });
     *      client = tls.connect(server.address().port, '127.0.0.1', { requestCert: false });
     *      console.log(client.read().toString()); // CN=other.local
     *
     *      client.close();
     *      server.stop();
     *      ```
     *      @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description Replaces the SecureContext from a fresh options object
     *
     *      The options are passed to tls.createSecureContext with isServer true, validated immediately
     *      and then used for the connections accepted afterwards; equivalent to building a context
     *      with createSecureContext and passing it to the other overload.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TcpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/TLSSocket.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the TLSServer class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TLSServerPromise extends Class_TcpServerPromise {
    /**
     * @description Creates a TLS server and binds a port on all local addresses
     *
     *      The port is bound during construction and start() begins accepting; port 0 asks the
     *      operating system for a free one, read it from address() or socket.localPort afterwards. The
     *      listener forms are described on the class page; a listener function receives one TLSSocket
     *      per accepted connection in its own fiber.
     *      @param context specifies the secure context used to create TLSServer
     *      @param port specifies the listening port
     *      @param listener the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates a TLS server bound to the given address and port
     *
     *      addr is an IP literal such as '127.0.0.1'; an empty string listens on all local addresses.
     *      The port is bound during construction and start() begins accepting.
     *      @param context specifies the secure context used to create TLSServer
     *      @param addr specifies the listening address
     *      @param port specifies the listening port
     *      @param listener the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates a TLS server from an options object, binding when it carries a port
     *
     *      The TLS keys are passed to tls.createSecureContext with isServer true, so cert and key are
     *      usually required for a usable server; in addition `address` (default all local addresses)
     *      and `port` are read for the bind. With a port the server binds immediately and start()
     *      begins accepting; without one it only stores the listener and listen() must be called. The
     *      listener forms are described on the class page.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param listener the connection handler
     *
     */
    constructor(options: FIBJS.GeneralObject, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates a TLS server without binding a port; listen() must be called to start
     *
     *      The deferred form stores the context and the listener, so lifecycle events can be registered
     *      before listen(port[, addr[, backlog]]) binds and starts. address() throws until the server
     *      is bound.
     *      @param context specifies the secure context used to create TLSServer
     *      @param listener the connection handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, listener: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((socket: Class_TLSSocket | Class_TLSSocketPromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description The SecureContext used by the server
     *
     *      The same object as the context of the internal handler; it applies to the connections
     *      accepted from now on, so replacing it through setSecureContext() affects the next
     *      connections only.
     *
     */
    readonly secureContext: Class_SecureContextPromise;

    /**
     * @description Replaces the SecureContext used for the connections accepted afterwards
     *
     *      Connections that already started their handshake keep the context they began with; the new
     *      context applies to the next accepted connection, which makes certificate rotation possible
     *      without restarting the listener.
     *
     *      Example — rotating the certificate between two connections:
     *      ```JavaScript
     *      const tls = require('tls');
     *      const crypto = require('crypto');
     *
     *      // two certificates with different subjects, served in turn
     *      function selfSigned(cn) {
     *          const pk = crypto.generateKeyPair('ec', { namedCurve: 'secp256r1' });
     *          const cert = crypto.createCertificateRequest({
     *              key: pk.privateKey,
     *              subject: { CN: cn }
     *          }).issue({
     *              key: pk.privateKey,
     *              issuer: { CN: cn },
     *              validFrom: new Date(Date.now() - 1000),
     *              days: 1
     *          });
     *          return { pk, cert };
     *      }
     *      const first = selfSigned('localhost');
     *      const second = selfSigned('other.local');
     *
     *      const server = tls.createServer({ key: first.pk.privateKey, cert: first.cert }, (conn) => {
     *          conn.write(conn.secureContext.cert.subject);
     *          conn.close();
     *      });
     *      server.listen(0, '127.0.0.1');
     *
     *      // setSecureContext() replaces the context used by the connections accepted
     *      // afterwards; existing connections keep the context they started with
     *      let client = tls.connect(server.address().port, '127.0.0.1', { requestCert: false });
     *      console.log(client.read().toString()); // CN=localhost
     *
     *      server.setSecureContext({ key: second.pk.privateKey, cert: second.cert });
     *      client = tls.connect(server.address().port, '127.0.0.1', { requestCert: false });
     *      console.log(client.read().toString()); // CN=other.local
     *
     *      client.close();
     *      server.stop();
     *      ```
     *      @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description Replaces the SecureContext from a fresh options object
     *
     *      The options are passed to tls.createSecureContext with isServer true, validated immediately
     *      and then used for the connections accepted afterwards; equivalent to building a context
     *      with createSecureContext and passing it to the other overload.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


declare namespace Class_TLSServer {
    const promises: FIBJS.GeneralObject;
}
