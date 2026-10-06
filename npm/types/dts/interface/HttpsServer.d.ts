/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description The HTTPS server: an HttpServer whose connections are terminated by an embedded TLSServer
 *
 *  HttpsServer extends HttpServer and adds the SecureContext used for the TLS handshake;
 *  everything else (handler forms, routing, limits, automatic headers, CORS and
 *  compression) behaves exactly like the plain server, because the embedded TLSServer
 *  decodes the stream and passes it to the same HttpHandler. Use it, or
 *  `http.createServer(options, hdlr)`, whenever the server terminates TLS itself; TLSServer
 *  with an http.Handler builds the same composition manually.
 *
 *  Concepts:
 *
 *  - **TLS termination**: the SecureContext holds the certificate chain, the private key,
 *    the trusted CAs, the protocol versions and the verification flags (see the tls
 *    module). The options constructor and createServer build the context in server mode. A
 *    context created without isServer (`tls.createSecureContext(opts)`) has requestCert
 *    true and a server built from it asks every client for a certificate, which ordinary
 *    clients do not send (the handshake fails with "peer did not return a certificate");
 *    use `tls.createSecureContext(opts, true)` or pass the TLS options object to the
 *    server.
 *  - **Construction**: context plus port, context plus address plus port, a TLS options
 *    object plus handler (its address and port keys are read too) and context plus handler
 *    without a port (listen() to bind). setSecureContext() replaces the context at runtime;
 *    connections already open are not renegotiated and later handshakes use the new
 *    context.
 *  - **Client trust**: a self-signed or private-CA certificate is not in the default
 *    Mozilla store, so the client must trust it explicitly with `new http.Client({ ca })`
 *    or a SecureContext. The module-level http functions do not accept TLS keys, so a `ca`
 *    in their options is ignored (Node's https.get does accept it).
 *  - **Node.js differences**: https.Server extends tls.Server and is created by
 *    https.createServer(options, listener); fibjs HttpsServer extends HttpServer, and
 *    `http.createServer` returns one when the first argument is a SecureContext or contains
 *    TLS material, while `createServer({}, hdlr)` returns a plain HttpServer. ALPN, SNI and
 *    session resumption come from the SecureContext (see tls).
 *
 *  Obtained from:
 *  - `new http.HttpsServer(options, hdlr)` / `new http.HttpsServer(context, port, hdlr)` —
 *    the server, bound when a port is given;
 *  - `new http.HttpsServer(context, hdlr)` — no port, call listen() to bind;
 *  - `http.createServer(options, hdlr)` — the same object through the http module;
 *  - `http.HttpsServer` is the module alias of this class; `require('https')` returns the
 *    same http module.
 *
 *  Example 1 — a self-signed server and a trusting client:
 *  ```JavaScript
 *  const http = require('http');
 *  const crypto = require('crypto');
 *
 *  const pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const cert = crypto.createCertificateRequest({
 *      key: pk.privateKey,
 *      subject: { CN: 'localhost' }
 *  }).issue({ key: pk.privateKey, ca: true, issuer: { CN: 'localhost' } });
 *
 *  const server = new http.HttpsServer({ cert, key: pk.privateKey, port: 0 }, (req) => {
 *      req.response.write('secure');
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const client = new http.Client({ ca: cert });
 *  const res = client.getSync('https://localhost:' + port + '/');
 *  console.log(res.statusCode, res.text()); // 200 secure
 *
 *  server.stop();
 *  ```
 *
 *  Example 2 — a server-mode SecureContext with the no-port constructor:
 *  ```JavaScript
 *  const http = require('http');
 *  const crypto = require('crypto');
 *  const tls = require('tls');
 *
 *  const pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const cert = crypto.createCertificateRequest({
 *      key: pk.privateKey,
 *      subject: { CN: 'localhost' }
 *  }).issue({ key: pk.privateKey, ca: true, issuer: { CN: 'localhost' } });
 *
 *  const ctx = tls.createSecureContext({ cert, key: pk.privateKey }, true);
 *  const server = new http.HttpsServer(ctx, (req) => { req.response.write('from context'); });
 *  server.listen(0);
 *  const port = server.socket.localPort;
 *
 *  const client = new http.Client({ ca: cert });
 *  console.log(client.getSync('https://localhost:' + port + '/').text()); // from context
 *
 *  server.stop();
 *  ```
 *
 *  Example 3 — http.createServer() with TLS options, and an untrusted client:
 *  ```JavaScript
 *  const http = require('http');
 *  const crypto = require('crypto');
 *
 *  const pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const cert = crypto.createCertificateRequest({
 *      key: pk.privateKey,
 *      subject: { CN: 'localhost' }
 *  }).issue({ key: pk.privateKey, ca: true, issuer: { CN: 'localhost' } });
 *
 *  const server = http.createServer({ cert, key: pk.privateKey }, (req) => {
 *      req.response.write('created');
 *  });
 *  console.log(server.constructor.name); // HttpsServer
 *
 *  server.listen(0);
 *  const port = server.socket.localPort;
 *
 *  let rejected = false;
 *  try {
 *      new http.Client().getSync('https://localhost:' + port + '/');
 *  } catch (e) {
 *      rejected = true; // the self-signed certificate is not trusted
 *  }
 *  console.log(rejected); // true
 *
 *  const client = new http.Client({ ca: cert });
 *  console.log(client.getSync('https://localhost:' + port + '/').text()); // created
 *
 *  server.stop();
 *  ```
 *
 */
declare class Class_HttpsServer extends Class_HttpServer {
    /**
     * @description Creates an HTTPS server bound to a port from a ready SecureContext
     *
     *      The context should have been built in server mode
     *      (`tls.createSecureContext(opts, true)`); a client-mode context has requestCert on
     *      and makes the server ask clients for a certificate. Port 0 selects a free port
     *      (socket.localPort). The handler forms and the lifecycle are those of
     *      HttpServer; see its documentation for the handler contract.
     *      @param context the SecureContext secure context
     *      @param port specifies the port on which the http server listens
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates an HTTPS server bound to an address and a port from a ready SecureContext
     *
     *      addr selects the local interface ("" means all of them, as in HttpServer); the
     *      context should be in server mode. The other constructor forms and the handler
     *      contract are documented on the first constructor and on HttpServer.
     *      @param context the SecureContext secure context
     *      @param addr the listening address; "" listens on all local addresses
     *      @param port specifies the port on which the http server listens
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates an HTTPS server from TLS options, binding immediately when a port is given
     *
     *      options is passed to tls.createSecureContext in server mode; in addition the keys
     *      address (optional, "" listens on all addresses) and port (optional) are read by the
     *      server. When port is omitted, listen() must be called to bind; invalid certificate
     *      material throws error 20024 here. The handler contract is the one of HttpServer.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param hdlr the request handler
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates an HTTPS server without binding a port; listen() must be called to start
     *
     *      Stores the ready SecureContext and the handler and creates the listener; call
     *      listen(port, addr) to bind and begin serving. Do not call start() after listen()
     *      (error 20009). The handler contract is the one of HttpServer.
     *      @param context the SecureContext secure context
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description The SecureContext used for the TLS handshake, read-only
     *
     *      The context given to the constructor or built from the options; pass it to a
     *      tls.connect client as secureContext to reuse the same trust settings. It is replaced
     *      by setSecureContext().
     *
     *      Example — the property returns the context the server was built with:
     *      ```JavaScript
     *      const http = require('http');
     *      const crypto = require('crypto');
     *      const tls = require('tls');
     *
     *      const pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const cert = crypto.createCertificateRequest({
     *          key: pk.privateKey,
     *          subject: { CN: 'localhost' }
     *      }).issue({ key: pk.privateKey, ca: true, issuer: { CN: 'localhost' } });
     *
     *      const ctx = tls.createSecureContext({ cert, key: pk.privateKey }, true);
     *      const server = new http.HttpsServer(ctx, (req) => { req.response.write('ctx'); });
     *      console.log(server.secureContext === ctx); // true
     *      server.stop();
     *      ```
     *
     */
    readonly secureContext: Class_SecureContext;

    /**
     * @description Replaces the SecureContext used for new TLS handshakes
     *
     *      Connections that are already open keep the old context; later handshakes use the
     *      new one, which is the supported way to rotate certificates without restarting the
     *      listener. Passing a ready context is equivalent to replacing the certificate chain
     *      and key directly.
     *      @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description Replaces the SecureContext, building it from TLS options
     *
     *      Creates a server-mode context from the options accepted by tls.createSecureContext
     *      (cert, key, ca, passphrase, minVersion, ...) and installs it, so new handshakes use
     *      the new material without a restart. Invalid material throws error 20024.
     *
     *      Example — install a context built from TLS options:
     *      ```JavaScript
     *      const http = require('http');
     *      const crypto = require('crypto');
     *      const tls = require('tls');
     *
     *      const pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const cert = crypto.createCertificateRequest({
     *          key: pk.privateKey,
     *          subject: { CN: 'localhost' }
     *      }).issue({ key: pk.privateKey, ca: true, issuer: { CN: 'localhost' } });
     *
     *      const ctx = tls.createSecureContext({ cert, key: pk.privateKey }, true);
     *      const server = new http.HttpsServer(ctx,
     *          (req) => { req.response.write('rotated'); });
     *      server.listen(0);
     *      const port = server.socket.localPort;
     *
     *      server.setSecureContext({ cert: cert, key: pk.privateKey });
     *      const client = new http.Client({ ca: cert });
     *      console.log(client.getSync('https://localhost:' + port + '/').text()); // rotated
     *
     *      server.stop();
     *      ```
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the HttpsServer class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpsServerPromise extends Class_HttpServerPromise {
    /**
     * @description Creates an HTTPS server bound to a port from a ready SecureContext
     *
     *      The context should have been built in server mode
     *      (`tls.createSecureContext(opts, true)`); a client-mode context has requestCert on
     *      and makes the server ask clients for a certificate. Port 0 selects a free port
     *      (socket.localPort). The handler forms and the lifecycle are those of
     *      HttpServer; see its documentation for the handler contract.
     *      @param context the SecureContext secure context
     *      @param port specifies the port on which the http server listens
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates an HTTPS server bound to an address and a port from a ready SecureContext
     *
     *      addr selects the local interface ("" means all of them, as in HttpServer); the
     *      context should be in server mode. The other constructor forms and the handler
     *      contract are documented on the first constructor and on HttpServer.
     *      @param context the SecureContext secure context
     *      @param addr the listening address; "" listens on all local addresses
     *      @param port specifies the port on which the http server listens
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, addr: string, port: number, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates an HTTPS server from TLS options, binding immediately when a port is given
     *
     *      options is passed to tls.createSecureContext in server mode; in addition the keys
     *      address (optional, "" listens on all addresses) and port (optional) are read by the
     *      server. When port is omitted, listen() must be called to bind; invalid certificate
     *      material throws error 20024 here. The handler contract is the one of HttpServer.
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *      @param hdlr the request handler
     *
     */
    constructor(options: FIBJS.GeneralObject, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description Creates an HTTPS server without binding a port; listen() must be called to start
     *
     *      Stores the ready SecureContext and the handler and creates the listener; call
     *      listen(port, addr) to bind and begin serving. Do not call start() after listen()
     *      (error 20009). The handler contract is the one of HttpServer.
     *      @param context the SecureContext secure context
     *      @param hdlr the request handler
     *
     */
    constructor(context: Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string);

    /**
     * @description The SecureContext used for the TLS handshake, read-only
     *
     *      The context given to the constructor or built from the options; pass it to a
     *      tls.connect client as secureContext to reuse the same trust settings. It is replaced
     *      by setSecureContext().
     *
     *      Example — the property returns the context the server was built with:
     *      ```JavaScript
     *      const http = require('http');
     *      const crypto = require('crypto');
     *      const tls = require('tls');
     *
     *      const pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const cert = crypto.createCertificateRequest({
     *          key: pk.privateKey,
     *          subject: { CN: 'localhost' }
     *      }).issue({ key: pk.privateKey, ca: true, issuer: { CN: 'localhost' } });
     *
     *      const ctx = tls.createSecureContext({ cert, key: pk.privateKey }, true);
     *      const server = new http.HttpsServer(ctx, (req) => { req.response.write('ctx'); });
     *      console.log(server.secureContext === ctx); // true
     *      server.stop();
     *      ```
     *
     */
    readonly secureContext: Class_SecureContextPromise;

    /**
     * @description Replaces the SecureContext used for new TLS handshakes
     *
     *      Connections that are already open keep the old context; later handshakes use the
     *      new one, which is the supported way to rotate certificates without restarting the
     *      listener. Passing a ready context is equivalent to replacing the certificate chain
     *      and key directly.
     *      @param context specifies the new SecureContext
     *
     */
    setSecureContext(context: Class_SecureContext | Class_SecureContextPromise): void;

    /**
     * @description Replaces the SecureContext, building it from TLS options
     *
     *      Creates a server-mode context from the options accepted by tls.createSecureContext
     *      (cert, key, ca, passphrase, minVersion, ...) and installs it, so new handshakes use
     *      the new material without a restart. Invalid material throws error 20024.
     *
     *      Example — install a context built from TLS options:
     *      ```JavaScript
     *      const http = require('http');
     *      const crypto = require('crypto');
     *      const tls = require('tls');
     *
     *      const pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
     *      const cert = crypto.createCertificateRequest({
     *          key: pk.privateKey,
     *          subject: { CN: 'localhost' }
     *      }).issue({ key: pk.privateKey, ca: true, issuer: { CN: 'localhost' } });
     *
     *      const ctx = tls.createSecureContext({ cert, key: pk.privateKey }, true);
     *      const server = new http.HttpsServer(ctx,
     *          (req) => { req.response.write('rotated'); });
     *      server.listen(0);
     *      const port = server.socket.localPort;
     *
     *      server.setSecureContext({ cert: cert, key: pk.privateKey });
     *      const client = new http.Client({ ca: cert });
     *      console.log(client.getSync('https://localhost:' + port + '/').text()); // rotated
     *
     *      server.stop();
     *      ```
     *      @param options the options needed to create a secure context with tls.createSecureContext
     *
     */
    setSecureContext(options: FIBJS.GeneralObject): void;

}


declare namespace Class_HttpsServer {
    const promises: FIBJS.GeneralObject;
}
