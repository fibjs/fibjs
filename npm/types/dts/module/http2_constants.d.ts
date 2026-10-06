/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The http2_constants module collects the HTTP/2 protocol constants used by the http2
 *  module: SETTINGS parameter ids and defaults, nghttp2 error codes, frame flags, stream
 *  states, padding strategies, HTTP status codes, and the standard pseudo-header and header
 *  names
 *
 *  The module mirrors the constant surface of the nghttp2 library bundled with fibjs and is
 *  reached through the `constants` property of the http2 module; it is not requireable on its
 *  own. The exported names and values are identical to Node.js's `http2.constants` (240
 *  entries, verified against Node.js v25.9.0):
 *
 *  - **SETTINGS**: `NGHTTP2_SETTINGS_*` ids (header table size, push, concurrent streams,
 *    initial window size, frame size, header list size, CONNECT protocol) and the matching
 *    `DEFAULT_SETTINGS_*` values assumed before a peer overrides them;
 *  - **Error codes**: the `NGHTTP2_*` codes carried by RST_STREAM and GOAWAY frames, from
 *    `NGHTTP2_NO_ERROR` (0) to `NGHTTP2_HTTP_1_1_REQUIRED` (13); `NGHTTP2_ERR_FRAME_SIZE_ERROR`
 *    is an internal negative nghttp2 return value, not a wire code;
 *  - **Frame flags**: the `NGHTTP2_FLAG_*` bit mask (ACK, END_STREAM, END_HEADERS, PADDED,
 *    PRIORITY) and `NGHTTP2_DEFAULT_WEIGHT`;
 *  - **Streams**: `NGHTTP2_STREAM_STATE_*` state numbers;
 *  - **Padding and session types**: `PADDING_STRATEGY_*` for outgoing DATA frames and
 *    `NGHTTP2_SESSION_SERVER` / `NGHTTP2_SESSION_CLIENT`;
 *  - **Limits**: `MIN_MAX_FRAME_SIZE`, `MAX_MAX_FRAME_SIZE` and `MAX_INITIAL_WINDOW_SIZE`;
 *  - **HTTP vocabulary**: `HTTP_STATUS_*`, `HTTP2_HEADER_*` (the pseudo-headers `:method`,
 *    `:path`, `:scheme`, `:authority`, `:status` and `:protocol` start with a colon) and
 *    `HTTP2_METHOD_*`.
 *
 *  Concepts:
 *
 *  - **SETTINGS negotiation**: each peer sends a SETTINGS frame when the connection starts; the
 *    id selects the parameter and the `DEFAULT_SETTINGS_*` values apply until the peer sends
 *    its own. `http2.getDefaultSettings()` maps the same defaults to parameter names, except
 *    that it reports `maxConcurrentStreams` 100, while the constant (and Node.js) use
 *    4294967295 for "unlimited".
 *  - **Error codes**: RST_STREAM and GOAWAY carry an `NGHTTP2_*` code; 0 is NO_ERROR and the
 *    other defined values are 1..13. They are distinct from the internal negative
 *    `NGHTTP2_ERR_*` values, which surface as ordinary errors.
 *  - **Flags are a bit mask**: END_STREAM and ACK share the value 1 because they belong to
 *    different frame types; combine flags with the bitwise-or operator.
 *  - **Pseudo-headers**: HTTP/2 header names are lowercase and pseudo-headers come before the
 *    regular headers; `HTTP2_HEADER_*` provides the conventional spellings.
 *
 *  Import:
 *  ```JavaScript
 *  const constants = require('http2').constants;
 *  ```
 *
 *  Example 1 — the constants in a local server round trip:
 *  ```JavaScript
 *  const http2 = require('http2');
 *  const tls = require('tls');
 *  const crypto = require('crypto');
 *  const constants = http2.constants;
 *
 *  // a self-signed certificate chain for localhost (do not use in production)
 *  const caKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const srvKey = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
 *  const ca = crypto.createCertificateRequest({
 *      key: caKey.privateKey, subject: { CN: 'fibjs.org' }
 *  }).issue({ key: caKey.privateKey, ca: true, issuer: { CN: 'fibjs.org' } });
 *  const crt = crypto.createCertificateRequest({
 *      key: srvKey.privateKey, subject: { CN: 'localhost' }
 *  }).issue({ key: caKey.privateKey, issuer: { CN: 'fibjs.org' } });
 *  const ctx = tls.createSecureContext({
 *      key: srvKey.privateKey.export(), cert: crt.pem, requestCert: false,
 *      alpnProtocols: ['h2']
 *  }, true);
 *
 *  const server = new http2.Server(ctx, 0, function () { });
 *  server.on('session', (session) => {
 *      session.on('stream', (stream, headers) => {
 *          stream.respond({
 *              [constants.HTTP2_HEADER_STATUS]: constants.HTTP_STATUS_OK,
 *              [constants.HTTP2_HEADER_CONTENT_TYPE]: 'text/plain'
 *          });
 *          stream.write(constants.HTTP2_METHOD_GET + ' ' +
 *              headers[constants.HTTP2_HEADER_PATH]);
 *          stream.close();
 *      });
 *  });
 *  server.start();
 *
 *  const session = http2.connect('https://localhost:' + server.socket.localPort, {
 *      rejectUnauthorized: false, rejectUnverified: false
 *  });
 *  const stream = session.request({
 *      [constants.HTTP2_HEADER_METHOD]: constants.HTTP2_METHOD_GET,
 *      [constants.HTTP2_HEADER_PATH]: '/constants'
 *  });
 *  console.log(stream.readAll().toString()); // GET /constants
 *  console.log(stream.headers[constants.HTTP2_HEADER_STATUS]); // 200
 *
 *  session.close();
 *  server.stop();
 *  ```
 *
 *  Example 2 — SETTINGS ids and protocol defaults:
 *  ```JavaScript
 *  const http2 = require('http2');
 *  const constants = http2.constants;
 *
 *  // SETTINGS parameter ids (RFC 9113 section 6.5.2).
 *  console.log(constants.NGHTTP2_SETTINGS_HEADER_TABLE_SIZE);       // 1
 *  console.log(constants.NGHTTP2_SETTINGS_ENABLE_PUSH);             // 2
 *  console.log(constants.NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);  // 3
 *  console.log(constants.NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE);     // 4
 *  console.log(constants.NGHTTP2_SETTINGS_MAX_FRAME_SIZE);          // 5
 *  console.log(constants.NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE);    // 6
 *  console.log(constants.NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL); // 8
 *
 *  // Values assumed before the peer sends its own SETTINGS frame.
 *  console.log(constants.DEFAULT_SETTINGS_HEADER_TABLE_SIZE);       // 4096
 *  console.log(constants.DEFAULT_SETTINGS_ENABLE_PUSH);             // 1
 *  console.log(constants.DEFAULT_SETTINGS_MAX_CONCURRENT_STREAMS);  // 4294967295
 *  console.log(constants.DEFAULT_SETTINGS_INITIAL_WINDOW_SIZE);     // 65535
 *  console.log(constants.DEFAULT_SETTINGS_MAX_FRAME_SIZE);          // 16384
 *  console.log(constants.DEFAULT_SETTINGS_MAX_HEADER_LIST_SIZE);    // 65535
 *  console.log(constants.DEFAULT_SETTINGS_ENABLE_CONNECT_PROTOCOL); // 0
 *
 *  // getDefaultSettings() maps the same defaults to parameter names.
 *  const settings = http2.getDefaultSettings();
 *  console.log(settings.headerTableSize ===
 *      constants.DEFAULT_SETTINGS_HEADER_TABLE_SIZE);               // true
 *  console.log(settings.initialWindowSize ===
 *      constants.DEFAULT_SETTINGS_INITIAL_WINDOW_SIZE);             // true
 *  console.log(settings.maxFrameSize ===
 *      constants.DEFAULT_SETTINGS_MAX_FRAME_SIZE);                  // true
 *  console.log(settings.maxHeaderListSize ===
 *      constants.DEFAULT_SETTINGS_MAX_HEADER_LIST_SIZE);            // true
 *  ```
 *
 *  Example 3 — error codes, flags and stream states:
 *  ```JavaScript
 *  const constants = require('http2').constants;
 *
 *  // Error codes carried by RST_STREAM and GOAWAY frames.
 *  console.log(constants.NGHTTP2_NO_ERROR, constants.NGHTTP2_PROTOCOL_ERROR,
 *      constants.NGHTTP2_FLOW_CONTROL_ERROR, constants.NGHTTP2_STREAM_CLOSED,
 *      constants.NGHTTP2_FRAME_SIZE_ERROR, constants.NGHTTP2_REFUSED_STREAM,
 *      constants.NGHTTP2_CANCEL, constants.NGHTTP2_ENHANCE_YOUR_CALM); // 0 1 3 5 6 7 8 11
 *
 *  // Frame flags are a bit mask; END_STREAM and ACK share 1 on different frame types.
 *  console.log(constants.NGHTTP2_FLAG_END_STREAM |
 *      constants.NGHTTP2_FLAG_END_HEADERS);                         // 5
 *  console.log(constants.NGHTTP2_FLAG_NONE, constants.NGHTTP2_FLAG_PADDED,
 *      constants.NGHTTP2_FLAG_PRIORITY);                            // 0 8 32
 *
 *  // Stream state numbers, frame limits and padding strategies.
 *  console.log(constants.NGHTTP2_STREAM_STATE_IDLE, constants.NGHTTP2_STREAM_STATE_OPEN,
 *      constants.NGHTTP2_STREAM_STATE_CLOSED);                      // 1 2 7
 *  console.log(constants.MIN_MAX_FRAME_SIZE, constants.MAX_MAX_FRAME_SIZE,
 *      constants.MAX_INITIAL_WINDOW_SIZE);                          // 16384 16777215 2147483647
 *  console.log(constants.PADDING_STRATEGY_NONE, constants.PADDING_STRATEGY_ALIGNED,
 *      constants.PADDING_STRATEGY_CALLBACK,
 *      constants.PADDING_STRATEGY_MAX);                             // 0 1 1 2
 *  ```
 *
 */
declare module 'http2_constants' {
    /**
     * @description whether the CONNECT protocol extension is enabled by default
     */
    export const DEFAULT_SETTINGS_ENABLE_CONNECT_PROTOCOL: 0;

    /**
     * @description whether server push is enabled by default
     */
    export const DEFAULT_SETTINGS_ENABLE_PUSH: 1;

    /**
     * @description default header table size (bytes)
     */
    export const DEFAULT_SETTINGS_HEADER_TABLE_SIZE: 4096;

    /**
     * @description default initial stream window size (bytes)
     */
    export const DEFAULT_SETTINGS_INITIAL_WINDOW_SIZE: 65535;

    /**
     * @description default maximum number of concurrent streams (unlimited)
     */
    export const DEFAULT_SETTINGS_MAX_CONCURRENT_STREAMS: 4294967295;

    /**
     * @description default maximum frame size (bytes)
     */
    export const DEFAULT_SETTINGS_MAX_FRAME_SIZE: 16384;

    /**
     * @description default maximum header list size (bytes)
     */
    export const DEFAULT_SETTINGS_MAX_HEADER_LIST_SIZE: 65535;

    /**
     * @description HTTP status code: Continue
     */
    export const HTTP_STATUS_CONTINUE: 100;

    /**
     * @description HTTP status code: Switching Protocols
     */
    export const HTTP_STATUS_SWITCHING_PROTOCOLS: 101;

    /**
     * @description HTTP status code: Processing
     */
    export const HTTP_STATUS_PROCESSING: 102;

    /**
     * @description HTTP status code: Early Hints
     */
    export const HTTP_STATUS_EARLY_HINTS: 103;

    /**
     * @description HTTP status code: OK
     */
    export const HTTP_STATUS_OK: 200;

    /**
     * @description HTTP status code: Created
     */
    export const HTTP_STATUS_CREATED: 201;

    /**
     * @description HTTP status code: Accepted
     */
    export const HTTP_STATUS_ACCEPTED: 202;

    /**
     * @description HTTP status code: Non-Authoritative Information
     */
    export const HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION: 203;

    /**
     * @description HTTP status code: No Content
     */
    export const HTTP_STATUS_NO_CONTENT: 204;

    /**
     * @description HTTP status code: Reset Content
     */
    export const HTTP_STATUS_RESET_CONTENT: 205;

    /**
     * @description HTTP status code: Partial Content
     */
    export const HTTP_STATUS_PARTIAL_CONTENT: 206;

    /**
     * @description HTTP status code: Multi-Status
     */
    export const HTTP_STATUS_MULTI_STATUS: 207;

    /**
     * @description HTTP status code: Already Reported
     */
    export const HTTP_STATUS_ALREADY_REPORTED: 208;

    /**
     * @description HTTP status code: IM Used
     */
    export const HTTP_STATUS_IM_USED: 226;

    /**
     * @description HTTP status code: Multiple Choices
     */
    export const HTTP_STATUS_MULTIPLE_CHOICES: 300;

    /**
     * @description HTTP status code: Moved Permanently
     */
    export const HTTP_STATUS_MOVED_PERMANENTLY: 301;

    /**
     * @description HTTP status code: Found
     */
    export const HTTP_STATUS_FOUND: 302;

    /**
     * @description HTTP status code: See Other
     */
    export const HTTP_STATUS_SEE_OTHER: 303;

    /**
     * @description HTTP status code: Not Modified
     */
    export const HTTP_STATUS_NOT_MODIFIED: 304;

    /**
     * @description HTTP status code: Use Proxy
     */
    export const HTTP_STATUS_USE_PROXY: 305;

    /**
     * @description HTTP status code: Temporary Redirect
     */
    export const HTTP_STATUS_TEMPORARY_REDIRECT: 307;

    /**
     * @description HTTP status code: Permanent Redirect
     */
    export const HTTP_STATUS_PERMANENT_REDIRECT: 308;

    /**
     * @description HTTP status code: Bad Request
     */
    export const HTTP_STATUS_BAD_REQUEST: 400;

    /**
     * @description HTTP status code: Unauthorized
     */
    export const HTTP_STATUS_UNAUTHORIZED: 401;

    /**
     * @description HTTP status code: Payment Required
     */
    export const HTTP_STATUS_PAYMENT_REQUIRED: 402;

    /**
     * @description HTTP status code: Forbidden
     */
    export const HTTP_STATUS_FORBIDDEN: 403;

    /**
     * @description HTTP status code: Not Found
     */
    export const HTTP_STATUS_NOT_FOUND: 404;

    /**
     * @description HTTP status code: Method Not Allowed
     */
    export const HTTP_STATUS_METHOD_NOT_ALLOWED: 405;

    /**
     * @description HTTP status code: Not Acceptable
     */
    export const HTTP_STATUS_NOT_ACCEPTABLE: 406;

    /**
     * @description HTTP status code: Proxy Authentication Required
     */
    export const HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED: 407;

    /**
     * @description HTTP status code: Request Timeout
     */
    export const HTTP_STATUS_REQUEST_TIMEOUT: 408;

    /**
     * @description HTTP status code: Conflict
     */
    export const HTTP_STATUS_CONFLICT: 409;

    /**
     * @description HTTP status code: Gone
     */
    export const HTTP_STATUS_GONE: 410;

    /**
     * @description HTTP status code: Length Required
     */
    export const HTTP_STATUS_LENGTH_REQUIRED: 411;

    /**
     * @description HTTP status code: Precondition Failed
     */
    export const HTTP_STATUS_PRECONDITION_FAILED: 412;

    /**
     * @description HTTP status code: Payload Too Large
     */
    export const HTTP_STATUS_PAYLOAD_TOO_LARGE: 413;

    /**
     * @description HTTP status code: URI Too Long
     */
    export const HTTP_STATUS_URI_TOO_LONG: 414;

    /**
     * @description HTTP status code: Unsupported Media Type
     */
    export const HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE: 415;

    /**
     * @description HTTP status code: Range Not Satisfiable
     */
    export const HTTP_STATUS_RANGE_NOT_SATISFIABLE: 416;

    /**
     * @description HTTP status code: Expectation Failed
     */
    export const HTTP_STATUS_EXPECTATION_FAILED: 417;

    /**
     * @description HTTP status code: I'm a Teapot
     */
    export const HTTP_STATUS_TEAPOT: 418;

    /**
     * @description HTTP status code: Misdirected Request
     */
    export const HTTP_STATUS_MISDIRECTED_REQUEST: 421;

    /**
     * @description HTTP status code: Unprocessable Entity
     */
    export const HTTP_STATUS_UNPROCESSABLE_ENTITY: 422;

    /**
     * @description HTTP status code: Locked
     */
    export const HTTP_STATUS_LOCKED: 423;

    /**
     * @description HTTP status code: Failed Dependency
     */
    export const HTTP_STATUS_FAILED_DEPENDENCY: 424;

    /**
     * @description HTTP status code: Too Early
     */
    export const HTTP_STATUS_TOO_EARLY: 425;

    /**
     * @description HTTP status code: Upgrade Required
     */
    export const HTTP_STATUS_UPGRADE_REQUIRED: 426;

    /**
     * @description HTTP status code: Precondition Required
     */
    export const HTTP_STATUS_PRECONDITION_REQUIRED: 428;

    /**
     * @description HTTP status code: Too Many Requests
     */
    export const HTTP_STATUS_TOO_MANY_REQUESTS: 429;

    /**
     * @description HTTP status code: Request Header Fields Too Large
     */
    export const HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE: 431;

    /**
     * @description HTTP status code: Unavailable For Legal Reasons
     */
    export const HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS: 451;

    /**
     * @description HTTP status code: Internal Server Error
     */
    export const HTTP_STATUS_INTERNAL_SERVER_ERROR: 500;

    /**
     * @description HTTP status code: Not Implemented
     */
    export const HTTP_STATUS_NOT_IMPLEMENTED: 501;

    /**
     * @description HTTP status code: Bad Gateway
     */
    export const HTTP_STATUS_BAD_GATEWAY: 502;

    /**
     * @description HTTP status code: Service Unavailable
     */
    export const HTTP_STATUS_SERVICE_UNAVAILABLE: 503;

    /**
     * @description HTTP status code: Gateway Timeout
     */
    export const HTTP_STATUS_GATEWAY_TIMEOUT: 504;

    /**
     * @description HTTP status code: HTTP Version Not Supported
     */
    export const HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED: 505;

    /**
     * @description HTTP status code: Variant Also Negotiates
     */
    export const HTTP_STATUS_VARIANT_ALSO_NEGOTIATES: 506;

    /**
     * @description HTTP status code: Insufficient Storage
     */
    export const HTTP_STATUS_INSUFFICIENT_STORAGE: 507;

    /**
     * @description HTTP status code: Loop Detected
     */
    export const HTTP_STATUS_LOOP_DETECTED: 508;

    /**
     * @description HTTP status code: Bandwidth Limit Exceeded
     */
    export const HTTP_STATUS_BANDWIDTH_LIMIT_EXCEEDED: 509;

    /**
     * @description HTTP status code: Not Extended
     */
    export const HTTP_STATUS_NOT_EXTENDED: 510;

    /**
     * @description HTTP status code: Network Authentication Required
     */
    export const HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED: 511;

    /**
     * @description HTTP/2 pseudo-header: :authority
     */
    export const HTTP2_HEADER_AUTHORITY: ":authority";

    /**
     * @description HTTP/2 header: accept
     */
    export const HTTP2_HEADER_ACCEPT: "accept";

    /**
     * @description HTTP/2 header: accept-charset
     */
    export const HTTP2_HEADER_ACCEPT_CHARSET: "accept-charset";

    /**
     * @description HTTP/2 header: accept-encoding
     */
    export const HTTP2_HEADER_ACCEPT_ENCODING: "accept-encoding";

    /**
     * @description HTTP/2 header: accept-language
     */
    export const HTTP2_HEADER_ACCEPT_LANGUAGE: "accept-language";

    /**
     * @description HTTP/2 header: accept-ranges
     */
    export const HTTP2_HEADER_ACCEPT_RANGES: "accept-ranges";

    /**
     * @description HTTP/2 header: access-control-allow-credentials
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_CREDENTIALS: "access-control-allow-credentials";

    /**
     * @description HTTP/2 header: access-control-allow-headers
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_HEADERS: "access-control-allow-headers";

    /**
     * @description HTTP/2 header: access-control-allow-methods
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_METHODS: "access-control-allow-methods";

    /**
     * @description HTTP/2 header: access-control-allow-origin
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN: "access-control-allow-origin";

    /**
     * @description HTTP/2 header: access-control-expose-headers
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_EXPOSE_HEADERS: "access-control-expose-headers";

    /**
     * @description HTTP/2 header: access-control-max-age
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_MAX_AGE: "access-control-max-age";

    /**
     * @description HTTP/2 header: access-control-request-headers
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_REQUEST_HEADERS: "access-control-request-headers";

    /**
     * @description HTTP/2 header: access-control-request-method
     */
    export const HTTP2_HEADER_ACCESS_CONTROL_REQUEST_METHOD: "access-control-request-method";

    /**
     * @description HTTP/2 header: age
     */
    export const HTTP2_HEADER_AGE: "age";

    /**
     * @description HTTP/2 header: allow
     */
    export const HTTP2_HEADER_ALLOW: "allow";

    /**
     * @description HTTP/2 header: alt-svc
     */
    export const HTTP2_HEADER_ALT_SVC: "alt-svc";

    /**
     * @description HTTP/2 header: authorization
     */
    export const HTTP2_HEADER_AUTHORIZATION: "authorization";

    /**
     * @description HTTP/2 header: cache-control
     */
    export const HTTP2_HEADER_CACHE_CONTROL: "cache-control";

    /**
     * @description HTTP/2 header: connection
     */
    export const HTTP2_HEADER_CONNECTION: "connection";

    /**
     * @description HTTP/2 header: content-disposition
     */
    export const HTTP2_HEADER_CONTENT_DISPOSITION: "content-disposition";

    /**
     * @description HTTP/2 header: content-encoding
     */
    export const HTTP2_HEADER_CONTENT_ENCODING: "content-encoding";

    /**
     * @description HTTP/2 header: content-language
     */
    export const HTTP2_HEADER_CONTENT_LANGUAGE: "content-language";

    /**
     * @description HTTP/2 header: content-length
     */
    export const HTTP2_HEADER_CONTENT_LENGTH: "content-length";

    /**
     * @description HTTP/2 header: content-location
     */
    export const HTTP2_HEADER_CONTENT_LOCATION: "content-location";

    /**
     * @description HTTP/2 header: content-md5
     */
    export const HTTP2_HEADER_CONTENT_MD5: "content-md5";

    /**
     * @description HTTP/2 header: content-range
     */
    export const HTTP2_HEADER_CONTENT_RANGE: "content-range";

    /**
     * @description HTTP/2 header: content-security-policy
     */
    export const HTTP2_HEADER_CONTENT_SECURITY_POLICY: "content-security-policy";

    /**
     * @description HTTP/2 header: content-type
     */
    export const HTTP2_HEADER_CONTENT_TYPE: "content-type";

    /**
     * @description HTTP/2 header: cookie
     */
    export const HTTP2_HEADER_COOKIE: "cookie";

    /**
     * @description HTTP/2 header: date
     */
    export const HTTP2_HEADER_DATE: "date";

    /**
     * @description HTTP/2 header: dnt
     */
    export const HTTP2_HEADER_DNT: "dnt";

    /**
     * @description HTTP/2 header: early-data
     */
    export const HTTP2_HEADER_EARLY_DATA: "early-data";

    /**
     * @description HTTP/2 header: etag
     */
    export const HTTP2_HEADER_ETAG: "etag";

    /**
     * @description HTTP/2 pseudo-header: :method
     */
    export const HTTP2_HEADER_METHOD: ":method";

    /**
     * @description HTTP/2 header: expect
     */
    export const HTTP2_HEADER_EXPECT: "expect";

    /**
     * @description HTTP/2 header: expect-ct
     */
    export const HTTP2_HEADER_EXPECT_CT: "expect-ct";

    /**
     * @description HTTP/2 header: expires
     */
    export const HTTP2_HEADER_EXPIRES: "expires";

    /**
     * @description HTTP/2 header: forwarded
     */
    export const HTTP2_HEADER_FORWARDED: "forwarded";

    /**
     * @description HTTP/2 header: from
     */
    export const HTTP2_HEADER_FROM: "from";

    /**
     * @description HTTP/2 header: host
     */
    export const HTTP2_HEADER_HOST: "host";

    /**
     * @description HTTP/2 header: http2-settings
     */
    export const HTTP2_HEADER_HTTP2_SETTINGS: "http2-settings";

    /**
     * @description HTTP/2 header: if-match
     */
    export const HTTP2_HEADER_IF_MATCH: "if-match";

    /**
     * @description HTTP/2 header: if-modified-since
     */
    export const HTTP2_HEADER_IF_MODIFIED_SINCE: "if-modified-since";

    /**
     * @description HTTP/2 header: if-none-match
     */
    export const HTTP2_HEADER_IF_NONE_MATCH: "if-none-match";

    /**
     * @description HTTP/2 header: if-range
     */
    export const HTTP2_HEADER_IF_RANGE: "if-range";

    /**
     * @description HTTP/2 header: if-unmodified-since
     */
    export const HTTP2_HEADER_IF_UNMODIFIED_SINCE: "if-unmodified-since";

    /**
     * @description HTTP/2 header: keep-alive
     */
    export const HTTP2_HEADER_KEEP_ALIVE: "keep-alive";

    /**
     * @description HTTP/2 header: last-modified
     */
    export const HTTP2_HEADER_LAST_MODIFIED: "last-modified";

    /**
     * @description HTTP/2 header: link
     */
    export const HTTP2_HEADER_LINK: "link";

    /**
     * @description HTTP/2 header: location
     */
    export const HTTP2_HEADER_LOCATION: "location";

    /**
     * @description HTTP/2 header: max-forwards
     */
    export const HTTP2_HEADER_MAX_FORWARDS: "max-forwards";

    /**
     * @description HTTP/2 header: origin
     */
    export const HTTP2_HEADER_ORIGIN: "origin";

    /**
     * @description HTTP/2 pseudo-header: :path
     */
    export const HTTP2_HEADER_PATH: ":path";

    /**
     * @description HTTP/2 header: prefer
     */
    export const HTTP2_HEADER_PREFER: "prefer";

    /**
     * @description HTTP/2 header: priority
     */
    export const HTTP2_HEADER_PRIORITY: "priority";

    /**
     * @description HTTP/2 pseudo-header: :protocol
     */
    export const HTTP2_HEADER_PROTOCOL: ":protocol";

    /**
     * @description HTTP/2 header: proxy-authenticate
     */
    export const HTTP2_HEADER_PROXY_AUTHENTICATE: "proxy-authenticate";

    /**
     * @description HTTP/2 header: proxy-authorization
     */
    export const HTTP2_HEADER_PROXY_AUTHORIZATION: "proxy-authorization";

    /**
     * @description HTTP/2 header: proxy-connection
     */
    export const HTTP2_HEADER_PROXY_CONNECTION: "proxy-connection";

    /**
     * @description HTTP/2 header: purpose
     */
    export const HTTP2_HEADER_PURPOSE: "purpose";

    /**
     * @description HTTP/2 header: range
     */
    export const HTTP2_HEADER_RANGE: "range";

    /**
     * @description HTTP/2 header: referer
     */
    export const HTTP2_HEADER_REFERER: "referer";

    /**
     * @description HTTP/2 header: refresh
     */
    export const HTTP2_HEADER_REFRESH: "refresh";

    /**
     * @description HTTP/2 header: retry-after
     */
    export const HTTP2_HEADER_RETRY_AFTER: "retry-after";

    /**
     * @description HTTP/2 pseudo-header: :scheme
     */
    export const HTTP2_HEADER_SCHEME: ":scheme";

    /**
     * @description HTTP/2 header: server
     */
    export const HTTP2_HEADER_SERVER: "server";

    /**
     * @description HTTP/2 header: set-cookie
     */
    export const HTTP2_HEADER_SET_COOKIE: "set-cookie";

    /**
     * @description HTTP/2 pseudo-header: :status
     */
    export const HTTP2_HEADER_STATUS: ":status";

    /**
     * @description HTTP/2 header: strict-transport-security
     */
    export const HTTP2_HEADER_STRICT_TRANSPORT_SECURITY: "strict-transport-security";

    /**
     * @description HTTP/2 header: te
     */
    export const HTTP2_HEADER_TE: "te";

    /**
     * @description HTTP/2 header: timing-allow-origin
     */
    export const HTTP2_HEADER_TIMING_ALLOW_ORIGIN: "timing-allow-origin";

    /**
     * @description HTTP/2 header: tk
     */
    export const HTTP2_HEADER_TK: "tk";

    /**
     * @description HTTP/2 header: trailer
     */
    export const HTTP2_HEADER_TRAILER: "trailer";

    /**
     * @description HTTP/2 header: transfer-encoding
     */
    export const HTTP2_HEADER_TRANSFER_ENCODING: "transfer-encoding";

    /**
     * @description HTTP/2 header: upgrade
     */
    export const HTTP2_HEADER_UPGRADE: "upgrade";

    /**
     * @description HTTP/2 header: upgrade-insecure-requests
     */
    export const HTTP2_HEADER_UPGRADE_INSECURE_REQUESTS: "upgrade-insecure-requests";

    /**
     * @description HTTP/2 header: user-agent
     */
    export const HTTP2_HEADER_USER_AGENT: "user-agent";

    /**
     * @description HTTP/2 header: vary
     */
    export const HTTP2_HEADER_VARY: "vary";

    /**
     * @description HTTP/2 header: via
     */
    export const HTTP2_HEADER_VIA: "via";

    /**
     * @description HTTP/2 header: warning
     */
    export const HTTP2_HEADER_WARNING: "warning";

    /**
     * @description HTTP/2 header: www-authenticate
     */
    export const HTTP2_HEADER_WWW_AUTHENTICATE: "www-authenticate";

    /**
     * @description HTTP/2 header: x-content-type-options
     */
    export const HTTP2_HEADER_X_CONTENT_TYPE_OPTIONS: "x-content-type-options";

    /**
     * @description HTTP/2 header: x-forwarded-for
     */
    export const HTTP2_HEADER_X_FORWARDED_FOR: "x-forwarded-for";

    /**
     * @description HTTP/2 header: x-frame-options
     */
    export const HTTP2_HEADER_X_FRAME_OPTIONS: "x-frame-options";

    /**
     * @description HTTP/2 header: x-xss-protection
     */
    export const HTTP2_HEADER_X_XSS_PROTECTION: "x-xss-protection";

    /**
     * @description HTTP/2 method: ACL
     */
    export const HTTP2_METHOD_ACL: "ACL";

    /**
     * @description HTTP/2 method: BASELINE-CONTROL
     */
    export const HTTP2_METHOD_BASELINE_CONTROL: "BASELINE-CONTROL";

    /**
     * @description HTTP/2 method: BIND
     */
    export const HTTP2_METHOD_BIND: "BIND";

    /**
     * @description HTTP/2 method: CHECKIN
     */
    export const HTTP2_METHOD_CHECKIN: "CHECKIN";

    /**
     * @description HTTP/2 method: CHECKOUT
     */
    export const HTTP2_METHOD_CHECKOUT: "CHECKOUT";

    /**
     * @description HTTP/2 method: CONNECT
     */
    export const HTTP2_METHOD_CONNECT: "CONNECT";

    /**
     * @description HTTP/2 method: COPY
     */
    export const HTTP2_METHOD_COPY: "COPY";

    /**
     * @description HTTP/2 method: DELETE
     */
    export const HTTP2_METHOD_DELETE: "DELETE";

    /**
     * @description HTTP/2 method: GET
     */
    export const HTTP2_METHOD_GET: "GET";

    /**
     * @description HTTP/2 method: HEAD
     */
    export const HTTP2_METHOD_HEAD: "HEAD";

    /**
     * @description HTTP/2 method: LABEL
     */
    export const HTTP2_METHOD_LABEL: "LABEL";

    /**
     * @description HTTP/2 method: LINK
     */
    export const HTTP2_METHOD_LINK: "LINK";

    /**
     * @description HTTP/2 method: LOCK
     */
    export const HTTP2_METHOD_LOCK: "LOCK";

    /**
     * @description HTTP/2 method: MERGE
     */
    export const HTTP2_METHOD_MERGE: "MERGE";

    /**
     * @description HTTP/2 method: MKACTIVITY
     */
    export const HTTP2_METHOD_MKACTIVITY: "MKACTIVITY";

    /**
     * @description HTTP/2 method: MKCALENDAR
     */
    export const HTTP2_METHOD_MKCALENDAR: "MKCALENDAR";

    /**
     * @description HTTP/2 method: MKCOL
     */
    export const HTTP2_METHOD_MKCOL: "MKCOL";

    /**
     * @description HTTP/2 method: MKREDIRECTREF
     */
    export const HTTP2_METHOD_MKREDIRECTREF: "MKREDIRECTREF";

    /**
     * @description HTTP/2 method: MKWORKSPACE
     */
    export const HTTP2_METHOD_MKWORKSPACE: "MKWORKSPACE";

    /**
     * @description HTTP/2 method: MOVE
     */
    export const HTTP2_METHOD_MOVE: "MOVE";

    /**
     * @description HTTP/2 method: OPTIONS
     */
    export const HTTP2_METHOD_OPTIONS: "OPTIONS";

    /**
     * @description HTTP/2 method: ORDERPATCH
     */
    export const HTTP2_METHOD_ORDERPATCH: "ORDERPATCH";

    /**
     * @description HTTP/2 method: PATCH
     */
    export const HTTP2_METHOD_PATCH: "PATCH";

    /**
     * @description HTTP/2 method: POST
     */
    export const HTTP2_METHOD_POST: "POST";

    /**
     * @description HTTP/2 method: PRI
     */
    export const HTTP2_METHOD_PRI: "PRI";

    /**
     * @description HTTP/2 method: PROPFIND
     */
    export const HTTP2_METHOD_PROPFIND: "PROPFIND";

    /**
     * @description HTTP/2 method: PROPPATCH
     */
    export const HTTP2_METHOD_PROPPATCH: "PROPPATCH";

    /**
     * @description HTTP/2 method: PUT
     */
    export const HTTP2_METHOD_PUT: "PUT";

    /**
     * @description HTTP/2 method: REBIND
     */
    export const HTTP2_METHOD_REBIND: "REBIND";

    /**
     * @description HTTP/2 method: REPORT
     */
    export const HTTP2_METHOD_REPORT: "REPORT";

    /**
     * @description HTTP/2 method: SEARCH
     */
    export const HTTP2_METHOD_SEARCH: "SEARCH";

    /**
     * @description HTTP/2 method: TRACE
     */
    export const HTTP2_METHOD_TRACE: "TRACE";

    /**
     * @description HTTP/2 method: UNBIND
     */
    export const HTTP2_METHOD_UNBIND: "UNBIND";

    /**
     * @description HTTP/2 method: UNCHECKOUT
     */
    export const HTTP2_METHOD_UNCHECKOUT: "UNCHECKOUT";

    /**
     * @description HTTP/2 method: UNLINK
     */
    export const HTTP2_METHOD_UNLINK: "UNLINK";

    /**
     * @description HTTP/2 method: UNLOCK
     */
    export const HTTP2_METHOD_UNLOCK: "UNLOCK";

    /**
     * @description HTTP/2 method: UPDATE
     */
    export const HTTP2_METHOD_UPDATE: "UPDATE";

    /**
     * @description HTTP/2 method: UPDATEREDIRECTREF
     */
    export const HTTP2_METHOD_UPDATEREDIRECTREF: "UPDATEREDIRECTREF";

    /**
     * @description HTTP/2 method: VERSION-CONTROL
     */
    export const HTTP2_METHOD_VERSION_CONTROL: "VERSION-CONTROL";

    /**
     * @description maximum value of the initial window size
     */
    export const MAX_INITIAL_WINDOW_SIZE: 2147483647;

    /**
     * @description maximum frame size value
     */
    export const MAX_MAX_FRAME_SIZE: 16777215;

    /**
     * @description minimum frame size value
     */
    export const MIN_MAX_FRAME_SIZE: 16384;

    /**
     * @description NGHTTP2 error: no error
     */
    export const NGHTTP2_NO_ERROR: 0;

    /**
     * @description NGHTTP2 error: protocol error
     */
    export const NGHTTP2_PROTOCOL_ERROR: 1;

    /**
     * @description NGHTTP2 error: internal error
     */
    export const NGHTTP2_INTERNAL_ERROR: 2;

    /**
     * @description NGHTTP2 error: flow control error
     */
    export const NGHTTP2_FLOW_CONTROL_ERROR: 3;

    /**
     * @description NGHTTP2 error: stream closed
     */
    export const NGHTTP2_STREAM_CLOSED: 5;

    /**
     * @description NGHTTP2 error: frame size error
     */
    export const NGHTTP2_FRAME_SIZE_ERROR: 6;

    /**
     * @description NGHTTP2 error: stream refused
     */
    export const NGHTTP2_REFUSED_STREAM: 7;

    /**
     * @description NGHTTP2 error: cancel
     */
    export const NGHTTP2_CANCEL: 8;

    /**
     * @description NGHTTP2 error: compression error
     */
    export const NGHTTP2_COMPRESSION_ERROR: 9;

    /**
     * @description NGHTTP2 error: connect error
     */
    export const NGHTTP2_CONNECT_ERROR: 10;

    /**
     * @description NGHTTP2 error: enhance your calm
     */
    export const NGHTTP2_ENHANCE_YOUR_CALM: 11;

    /**
     * @description NGHTTP2 error: inadequate security
     */
    export const NGHTTP2_INADEQUATE_SECURITY: 12;

    /**
     * @description NGHTTP2 error: HTTP/1.1 required
     */
    export const NGHTTP2_HTTP_1_1_REQUIRED: 13;

    /**
     * @description NGHTTP2 internal error: frame size error
     */
    export const NGHTTP2_ERR_FRAME_SIZE_ERROR: -522;

    /**
     * @description NGHTTP2 Flag: no flag
     */
    export const NGHTTP2_FLAG_NONE: 0;

    /**
     * @description NGHTTP2 Flag: ACK
     */
    export const NGHTTP2_FLAG_ACK: 1;

    /**
     * @description NGHTTP2 Flag: END_STREAM
     */
    export const NGHTTP2_FLAG_END_STREAM: 1;

    /**
     * @description NGHTTP2 Flag: END_HEADERS
     */
    export const NGHTTP2_FLAG_END_HEADERS: 4;

    /**
     * @description NGHTTP2 Flag: PADDED
     */
    export const NGHTTP2_FLAG_PADDED: 8;

    /**
     * @description NGHTTP2 Flag: PRIORITY
     */
    export const NGHTTP2_FLAG_PRIORITY: 32;

    /**
     * @description NGHTTP2 default weight
     */
    export const NGHTTP2_DEFAULT_WEIGHT: 16;

    /**
     * @description NGHTTP2 session type: server
     */
    export const NGHTTP2_SESSION_SERVER: 0;

    /**
     * @description NGHTTP2 session type: client
     */
    export const NGHTTP2_SESSION_CLIENT: 1;

    /**
     * @description NGHTTP2 setting: header table size
     */
    export const NGHTTP2_SETTINGS_HEADER_TABLE_SIZE: 1;

    /**
     * @description NGHTTP2 setting: whether push is enabled
     */
    export const NGHTTP2_SETTINGS_ENABLE_PUSH: 2;

    /**
     * @description NGHTTP2 setting: maximum concurrent streams
     */
    export const NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS: 3;

    /**
     * @description NGHTTP2 setting: initial window size
     */
    export const NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE: 4;

    /**
     * @description nghttp2 error code: SETTINGS not acknowledged in time (not a SETTINGS id)
     */
    export const NGHTTP2_SETTINGS_TIMEOUT: 4;

    /**
     * @description NGHTTP2 setting: maximum frame size
     */
    export const NGHTTP2_SETTINGS_MAX_FRAME_SIZE: 5;

    /**
     * @description NGHTTP2 setting: maximum header list size
     */
    export const NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE: 6;

    /**
     * @description NGHTTP2 setting: enable CONNECT protocol extension
     */
    export const NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL: 8;

    /**
     * @description NGHTTP2 stream state: idle
     */
    export const NGHTTP2_STREAM_STATE_IDLE: 1;

    /**
     * @description NGHTTP2 stream state: open
     */
    export const NGHTTP2_STREAM_STATE_OPEN: 2;

    /**
     * @description NGHTTP2 stream state: reserved local
     */
    export const NGHTTP2_STREAM_STATE_RESERVED_LOCAL: 3;

    /**
     * @description NGHTTP2 stream state: reserved remote
     */
    export const NGHTTP2_STREAM_STATE_RESERVED_REMOTE: 4;

    /**
     * @description NGHTTP2 stream state: half closed local
     */
    export const NGHTTP2_STREAM_STATE_HALF_CLOSED_LOCAL: 5;

    /**
     * @description NGHTTP2 stream state: half closed remote
     */
    export const NGHTTP2_STREAM_STATE_HALF_CLOSED_REMOTE: 6;

    /**
     * @description NGHTTP2 stream state: closed
     */
    export const NGHTTP2_STREAM_STATE_CLOSED: 7;

    /**
     * @description Padding strategy: none
     */
    export const PADDING_STRATEGY_NONE: 0;

    /**
     * @description Padding strategy: aligned
     */
    export const PADDING_STRATEGY_ALIGNED: 1;

    /**
     * @description Padding strategy: callback (same as ALIGNED)
     */
    export const PADDING_STRATEGY_CALLBACK: 1;

    /**
     * @description Padding strategy: maximum padding
     */
    export const PADDING_STRATEGY_MAX: 2;

}

