/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Http2Server.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Http2Session.d.ts" />
/// <reference path="../interface/Http2Stream.d.ts" />
/// <reference path="../module/http2_constants.d.ts" />
/**
 * The promise variant of the http2 module: async members return a Promise as their primary form.
 */
declare module 'http2/promises' {
    /**
     * @description the Http2Server constructor, see Http2Server
     *
     *      `http2.Server` is the constructor of the Http2Server class: `new http2.Server(...)`
     *      accepts the Http2Server constructor forms (a SecureContext or the
     *      tls.createSecureContext options, an optional address and port, and a handler). The
     *      class itself is documented under the name Http2Server.
     *
     */
    const Server: typeof Class_Http2Server;

    /**
     * @description creates an Http2Server from TLS options or a SecureContext
     *
     *      options may be a SecureContext object or the options used to create one with
     *      tls.createSecureContext (key, cert, ca, requestCert, alpnProtocols, ...). The
     *      returned server is not bound to a port: call listen(port[, addr]) before start(),
     *      or use one of the Http2Server constructor forms that take a port.
     *
     *      hdlr accepts the same forms as http.createServer (a Handler, an array of handlers, a
     *      function, a routing map, or a path/address string), but the current HTTP/2
     *      implementation does not invoke it. Requests are delivered as `stream` events on the
     *      Http2Session emitted by the server `session` event; register
     *      `session.on('stream', ...)` there.
     *
     *      Example — a server created from TLS options and bound with listen():
     *
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
     *      const options = {
     *          key: srvKey.privateKey.export(), cert: crt.pem,
     *          requestCert: false, alpnProtocols: ['h2']
     *      };
     *
     *      const server = http2.createServer(options, function () { });
     *      server.on('session', (session) => {
     *          session.on('stream', (stream, headers) => {
     *              stream.respond({ ':status': 200 });
     *              stream.write('created');
     *              stream.close();
     *          });
     *      });
     *      server.listen(0, '127.0.0.1');
     *
     *      const session = http2.connect('https://127.0.0.1:' + server.address().port, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/ping' });
     *      console.log(stream.read().toString()); // created
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param options the TLS options or the SecureContext object
     *      @param hdlr the request handler, accepted for compatibility
     *      @return returns an Http2Server object; call listen() to bind and serve
     *
     */
    function createServer(options: FIBJS.GeneralObject | Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_HttpRequest | Class_HttpRequestPromise, res: Class_HttpResponse | Class_HttpResponsePromise)=>any) | FIBJS.GeneralObject | string): Class_Http2Server;

    /**
     * @description creates an HTTP/2 client session to the specified target
     *
     *      authority must be an `https://host[:port]` URL; any other scheme throws
     *      `http2.connect: authority must use https:// scheme.`. options are the
     *      tls.createSecureContext options (ca, cert, key, rejectUnauthorized, ...). `h2` is
     *      added to alpnProtocols automatically when the option is absent; the caller's options
     *      object is cloned, not modified.
     *
     *      The call blocks the current fiber through the TCP connect, the TLS handshake (ALPN
     *      `h2`) and the first SETTINGS frame, then returns the connected Http2Session. Use
     *      session.request() to create streams and session.close()/destroy() to end the
     *      session.
     *
     *      Example — connect and inspect the negotiated session:
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
     *              stream.write('ok');
     *              stream.close();
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/' });
     *      console.log(stream.read().toString()); // ok
     *      console.log(session.alpnProtocol); // h2
     *      console.log(session.remoteSettings.maxConcurrentStreams); // 100
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param authority URL of the server to connect
     *      @param options connection options
     *      @return returns the connected Http2Session
     *
     */
    function connect(authority: string, options?: FIBJS.GeneralObject): Promise<Class_Http2SessionPromise>;

    /**
     * @description creates an HTTP/2 client session to the specified target
     *
     *      authority must be an `https://host[:port]` URL; any other scheme throws
     *      `http2.connect: authority must use https:// scheme.`. options are the
     *      tls.createSecureContext options (ca, cert, key, rejectUnauthorized, ...). `h2` is
     *      added to alpnProtocols automatically when the option is absent; the caller's options
     *      object is cloned, not modified.
     *
     *      The call blocks the current fiber through the TCP connect, the TLS handshake (ALPN
     *      `h2`) and the first SETTINGS frame, then returns the connected Http2Session. Use
     *      session.request() to create streams and session.close()/destroy() to end the
     *      session.
     *
     *      Example — connect and inspect the negotiated session:
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
     *              stream.write('ok');
     *              stream.close();
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/' });
     *      console.log(stream.read().toString()); // ok
     *      console.log(session.alpnProtocol); // h2
     *      console.log(session.remoteSettings.maxConcurrentStreams); // 100
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param authority URL of the server to connect
     *      @param options connection options
     *      @return returns the connected Http2Session
     *
     */
    function connectSync(authority: string, options?: FIBJS.GeneralObject): Class_Http2Session;

    /**
     * @description creates an HTTP/2 client session to the specified target
     *
     *      authority must be an `https://host[:port]` URL; any other scheme throws
     *      `http2.connect: authority must use https:// scheme.`. options are the
     *      tls.createSecureContext options (ca, cert, key, rejectUnauthorized, ...). `h2` is
     *      added to alpnProtocols automatically when the option is absent; the caller's options
     *      object is cloned, not modified.
     *
     *      The call blocks the current fiber through the TCP connect, the TLS handshake (ALPN
     *      `h2`) and the first SETTINGS frame, then returns the connected Http2Session. Use
     *      session.request() to create streams and session.close()/destroy() to end the
     *      session.
     *
     *      Example — connect and inspect the negotiated session:
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
     *              stream.write('ok');
     *              stream.close();
     *          });
     *      });
     *      server.start();
     *
     *      const session = http2.connect('https://localhost:' + server.socket.localPort, {
     *          rejectUnauthorized: false, rejectUnverified: false
     *      });
     *      const stream = session.request({ ':method': 'GET', ':path': '/' });
     *      console.log(stream.read().toString()); // ok
     *      console.log(session.alpnProtocol); // h2
     *      console.log(session.remoteSettings.maxConcurrentStreams); // 100
     *
     *      session.close();
     *      server.stop();
     *      ```
     *
     *      @param authority URL of the server to connect
     *      @param options connection options
     *      @return returns the connected Http2Session
     *
     */
    function connectAsync(authority: string, options?: FIBJS.GeneralObject): Promise<Class_Http2SessionPromise>;

    /**
     * @description returns the protocol default settings in a new object
     *
     *      The object contains headerTableSize, enablePush, maxConcurrentStreams,
     *      initialWindowSize, maxFrameSize and maxHeaderListSize with the protocol defaults
     *      (4096, true, 100, 65535, 16384 and 65535). These are the defaults of a new session;
     *      a live session advertises maxConcurrentStreams 100 but an initialWindowSize of
     *      1 MiB in its own SETTINGS frame, and the negotiated values are reported by
     *      Http2Session.localSettings/remoteSettings.
     *
     *      Example — the default settings of the protocol:
     *      ```JavaScript
     *      const http2 = require('http2');
     *
     *      const defaults = http2.getDefaultSettings();
     *      console.log(defaults.maxConcurrentStreams, defaults.initialWindowSize); // 100 65535
     *      ```
     *      @return returns an object containing the default settings
     *
     */
    function getDefaultSettings(): {
        headerTableSize: number;
        enablePush: boolean;
        maxConcurrentStreams: number;
        initialWindowSize: number;
        maxFrameSize: number;
        maxHeaderListSize: number;
    };

    /**
     * @description the Http2Stream class, see Http2Stream
     *
     *      Http2Stream instances are not constructed directly: a client obtains one from
     *      Http2Session.request and a server receives them with the `stream` event of an
     *      Http2Session. The class and its members are documented in Http2Stream.
     *
     */
    const Http2Stream: typeof Class_Http2Stream;

    /**
     * @description the Http2Session class, see Http2Session
     *
     *      Http2Session instances are not constructed directly: a client obtains one from
     *      http2.connect and a server receives one with the `session` event of Http2Server.
     *      The class and its members are documented in Http2Session.
     *
     */
    const Http2Session: typeof Class_Http2Session;

    /**
     * @description the nghttp2 constants of the http2 module, see http2_constants
     *
     *      The object exposes the HTTP/2 error codes used by RST_STREAM/GOAWAY (NGHTTP2_NO_ERROR,
     *      NGHTTP2_CANCEL, ...), the SETTINGS identifiers (NGHTTP2_SETTINGS_*) and the frame
     *      flags, all documented in http2_constants.
     *
     */
    const constants: typeof import ('http2_constants');

}


declare module "http2" {
    const promises: typeof import("http2/promises");
}
