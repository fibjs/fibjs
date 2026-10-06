/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/// <reference path="../interface/Headers.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/**
 * @description HTTP message base object: the protocol metadata shared by HttpRequest and HttpResponse
 *
 *  HttpMessage is the abstract base class of the two HTTP payload types, HttpRequest and
 *  HttpResponse. It extends Message with the HTTP protocol metadata: the protocol version, the
 *  header collection, keep-alive and upgrade flags, the size limits used by the parser and the
 *  trailer collection; the body reading/writing API and the routing metadata are inherited from
 *  Message.
 *
 *  The class cannot be instantiated and is not reachable as a global: a server receives an
 *  HttpRequest, handlers build the reply through `request.response` (an HttpResponse), and the
 *  client functions return an HttpResponse. Use those concrete classes for member examples and
 *  for real code.
 *
 *  Concepts:
 *
 *  - **Headers**: every HTTP message carries a case-insensitive Headers collection exposed as
 *    `headers`. Multi-value keys such as Set-Cookie keep their values in order; hasHeader,
 *    firstHeader, allHeader, appendHeader, setHeader, removeHeader, getHeader and getHeaders are
 *    the member forms of the same collection and follow the Node.js names. appendHeader adds
 *    values, setHeader replaces every value of a key (an object argument only touches the keys it
 *    contains, a Headers argument replaces the whole set), removeHeader deletes the key.
 *  - **Protocol and persistence**: `protocol` is the HTTP version string; setting a version above
 *    1.0 turns `keepAlive` on and 1.0 turns it off, and appending or parsing a Connection:
 *    keep-alive / Connection: Upgrade header updates keepAlive / upgrade accordingly.
 *  - **Limits**: maxHeadersCount (default 128) and maxHeaderSize (default 8192 bytes for one
 *    header line) bound the parsed header block; maxChunkSize (default 2 MB) bounds one chunk of
 *    a chunked body and maxBodySize (default 64 MB) bounds the whole body, both in megabytes
 *    (-1 disables the body limit, 0 reads no body). The http module exposes module-level
 *    counterparts for the client side.
 *  - **Wire format**: sendTo writes the start line, the headers and (unless header_only) the body
 *    to a stream; readFrom parses a message from a BufferedStream. The members are inherited from
 *    Message; the base implementation throws, so they are only usable on Request/Response.
 *  - **Trailers**: trailer headers are held in `trailers` and queued with addTrailers; they are
 *    sent after a chunked body and parsed back into `trailers` on the receiving side.
 *  - **Socket and stream**: for a message read from the network, `socket` is the underlying
 *    device stream and `stream` (inherited from Message) is the buffered stream the parser read
 *    from; both are null for messages built in memory and are reset by clear().
 *  - **formData**: parses the body according to Content-Type; only multipart/form-data (with a
 *    boundary) and application/x-www-form-urlencoded are accepted, any other type raises a
 *    TypeError, matching the Fetch Body mixin (MDN).
 *  - **Node.js differences**: Node has no shared base class exporting these members;
 *    http.IncomingMessage carries headers/method/url and http.ServerResponse carries
 *    headersSent/setHeader/getHeaders/addTrailers. Node headers are plain objects with array
 *    values, while fibjs exposes the WHATWG-style Headers collection plus the firstHeader and
 *    allHeader helpers.
 *
 *  Obtained from:
 *  - `new http.Request()` — a request message, as received by a server or built for a client;
 *  - `new http.Response()` — a response message, as built by a server or received from a client;
 *  - the handler of an http.Server — `req` is an HttpRequest and `req.response` an HttpResponse;
 *  - `http.getSync(...)`, `http.requestSync(...)`, `http.fetch(...)` and the other client
 *    functions — they return an HttpResponse.
 *
 *  Example 1 — parse an HTTP request from an in-memory stream:
 *  ```JavaScript
 *  const http = require('http');
 *  const io = require('io');
 *
 *  const ms = new io.MemoryStream();
 *  const bs = new io.BufferedStream(ms);
 *  bs.EOL = '\r\n';
 *  bs.writeText('POST /submit HTTP/1.1\r\nHost: example.com\r\nContent-Length: 7\r\n\r\npayload');
 *  ms.rewind();
 *
 *  const req = new http.Request();
 *  req.readFrom(bs);
 *
 *  console.log(req.method, req.address, req.protocol); // POST /submit HTTP/1.1
 *  console.log(req.firstHeader('Host'), req.length, req.text()); // example.com 7 payload
 *  console.log(req.stream !== null, req.socket !== null); // true true
 *  ```
 *
 *  Example 2 — build and serialize an HTTP response:
 *  ```JavaScript
 *  const http = require('http');
 *  const io = require('io');
 *
 *  const res = new http.Response();
 *  res.statusCode = 200;
 *  res.setHeader('Content-Type', 'text/plain');
 *  res.write('ok');
 *
 *  const wire = new io.MemoryStream();
 *  res.sendTo(wire);
 *  console.log(res.headersSent); // true
 *
 *  wire.rewind();
 *  console.log(wire.readAll().toString());
 *  // HTTP/1.1 200 OK with Content-Type, Connection and Content-Length, then the body 'ok'
 *  ```
 *
 *  Example 3 — a Request/Response round trip through a server:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, (req) => {
 *      req.response.setHeader('X-Path', req.address);
 *      req.response.write('hello');
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const resp = http.getSync('http://127.0.0.1:' + port + '/world');
 *  console.log(resp.statusCode, resp.firstHeader('X-Path')); // 200 /world
 *  console.log(resp.keepAlive, resp.protocol); // true HTTP/1.1
 *  console.log(resp.text()); // hello
 *
 *  server.stop();
 *  ```
 *
 *  Notes:
 *
 *  - The parser normalizes parsed header names to lowercase, but hasHeader/firstHeader and the
 *    Headers methods are case-insensitive either way; the wire format written by sendTo uses the
 *    stored casing.
 *  - content_length and header_only belong to the sendTo/readFrom options, not to the message
 *    state; the limit properties are read by the parser and the writer only.
 *
 */
declare class Class_HttpMessage extends Class_Message {
    /**
     * @description protocol version information, the allowed format is: HTTP/#.#
     *
     *      Default HTTP/1.1. The setter validates the form and throws error 20024 for anything else;
     *      a version above 1.0 enables keepAlive and 1.0 disables it. A parsed message sets the
     *      version from its start line. Node.js exposes the same value as httpVersion on
     *      IncomingMessage and ServerResponse.
     *
     *      Example — the version drives keep-alive:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      console.log(req.protocol, req.keepAlive); // HTTP/1.1 true
     *
     *      req.protocol = 'HTTP/1.0';
     *      console.log(req.protocol, req.keepAlive); // HTTP/1.0 false
     *
     *      req.appendHeader('Connection', 'keep-alive');
     *      console.log(req.keepAlive); // true
     *      ```
     *
     */
    protocol: string;

    /**
     * @description container holding the http headers of the message, read-only property
     *
     *      The live Headers collection of the message: the property itself cannot be replaced, but
     *      the collection can be modified with its own methods (set, append, delete, ...) and the
     *      change is reflected in the wire output. Parsed messages store keys lowercase, lookup is
     *      case-insensitive either way. Node.js uses a plain object plus rawHeaders instead.
     *
     */
    readonly headers: Class_Headers;

    /**
     * @description queries and sets whether to keep the connection alive
     *
     *      Default true. Setting protocol above 1.0 turns it on and 1.0 turns it off, and appending
     *      or parsing a Connection header updates it ('close' clears, 'keep-alive' and 'upgrade' set).
     *      On output the value selects the Connection header written by sendTo. Node's IncomingMessage
     *      has no such property, persistence belongs to the Agent.
     *
     */
    keepAlive: boolean;

    /**
     * @description queries and sets whether the protocol is upgraded
     *
     *      Default false. Set when a Connection: Upgrade header is appended or parsed; the WebSocket
     *      handshake relies on it. Node.js reports the same condition through the 'upgrade' event.
     *
     */
    upgrade: boolean;

    /**
     * @description queries and sets the maximum number of request headers, default is 128
     *
     *      The parser fails with error 20024 when a message carries more header lines than this; it
     *      is used when reading requests. A negative value throws a RangeError (20006). Node.js has
     *      an http.Server.maxHeadersCount property instead of a per-message one.
     *
     */
    maxHeadersCount: number;

    /**
     * @description queries and sets the maximum request header length, default is 8192
     *
     *      The maximum size in bytes of one header line accepted by the parser; a longer line fails
     *      with error 20024. A negative value throws a RangeError (20006). Node.js supports the
     *      process-wide --max-http-header-size setting instead.
     *
     */
    maxHeaderSize: number;

    /**
     * @description queries and sets the maximum chunk size in MB, default is 2
     *
     *      Bounds a single chunk of a chunked body while it is parsed: a larger chunk fails with
     *      error 20024 ("HttpMessage: chunk is too huge."). 0 is accepted. Node.js has no equivalent
     *      per-message setting.
     *
     */
    maxChunkSize: number;

    /**
     * @description queries and sets the maximum body size in MB, default is 64
     *
     *      While parsing, a Content-Length or chunked body larger than this fails with error 20024
     *      ("HttpMessage: body is too huge."); -1 disables the limit and 0 reads no body at all. Set
     *      it before readFrom. Node.js has no per-message body limit.
     *
     */
    maxBodySize: number;

    /**
     * @description queries the source socket of the current object
     *
     *      The underlying device stream the message was read from; null when the message was built in
     *      memory and reset by clear(). It differs from stream (inherited from Message), the buffered
     *      wrapper the parser consumed. Node.js exposes socket on IncomingMessage only.
     *
     */
    readonly socket: Class_Stream;

    /**
     * @description checks whether a header of the specified key exists
     *
     *      Lookup is case-insensitive; returns false when the key was never set. See the headers
     *      property for the collection itself.
     *      @param name specifies the key to check
     *      @return returns whether the key exists
     *
     */
    hasHeader(name: string): boolean;

    /**
     * @description queries the first header of the specified key
     *
     *      Returns the first value when the key was appended more than once; a missing key returns
     *      null (not undefined). Use getHeader for the Node.js-style undefined result and allHeader
     *      for every value.
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or null if it does not exist
     *
     */
    firstHeader(name: string): string;

    /**
     * @description queries all headers of the specified key
     *
     *      Returns the values in append order as an array; a missing key returns an empty array. With
     *      an empty name the whole header set is returned as an object whose multi-value keys are
     *      arrays and whose single-value keys are strings, the shape getHeaders() returns.
     *      @param name specifies the key to query; passing an empty string returns the result of all keys
     *      @return returns an array of all values corresponding to the key, or an empty array if the data does not exist
     *
     */
    allHeader(name?: string): FIBJS.GeneralObject;

    /**
     * @description appends a header; appending data does not modify the headers of an existing key
     *
     *      Appends every entry of the object; existing values of the same key are kept, so a key can
     *      end up with several values (Set-Cookie and friends). The overloads cover an object map, a
     *      Headers collection, a name with an array of values and a name with a single value; a
     *      Connection entry updates keepAlive/upgrade as it is appended.
     *
     *      Example — append keeps duplicates, setHeader replaces them:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      req.appendHeader('X-Tag', 'a');
     *      req.appendHeader('X-Tag', 'b');
     *      req.appendHeader({ 'X-Tag': 'c' });
     *
     *      console.log(req.allHeader('X-Tag').join(',')); // a,b,c
     *      console.log(req.firstHeader('X-Tag')); // a
     *
     *      const extra = new http.Headers();
     *      extra.append('X-Tag', 'd');
     *      extra.append('Content-Type', 'text/plain');
     *      req.appendHeader(extra);
     *      console.log(req.allHeader('X-Tag').join(',')); // a,b,c,d
     *      ```
     *      @param map specifies the key-value data dictionary to append
     *
     */
    appendHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description appends headers; appending data does not modify the headers of an existing key
     *
     *      Appends every value of the given collection in order, duplicates included; useful to merge
     *      another message's headers into this one.
     *      @param headers specifies the Headers object to append
     *
     */
    appendHeader(headers: Class_Headers): void;

    /**
     * @description appends a group of headers with the specified name; appending data does not modify the headers of an existing key
     *
     *      Every element of the array is appended as a separate value of the key; non-string scalars
     *      are rendered as strings.
     *      @param name specifies the key to append
     *      @param values specifies the group of data to append
     *
     */
    appendHeader(name: string, values: any[]): void;

    /**
     * @description appends a header; appending data does not modify the headers of an existing key
     *
     *      Appends one value; a non-string scalar (number, boolean, Date) is rendered as its string
     *      form. The same effect is available through headers.append(name, value).
     *      @param name specifies the key to append
     *      @param value specifies the data to append
     *
     */
    appendHeader(name: string, value: any): void;

    /**
     * @description sets a header; setting data modifies the first value of the key and clears the remaining headers with the same key
     *
     *      Sets each key of the object, replacing all of its existing values; keys not present in the
     *      object are left untouched. The setHeader(Headers) overload replaces the whole header set
     *      instead. See appendHeader for the appending counterpart.
     *
     *      Example — replace, set and remove headers:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response();
     *      res.setHeader('X-Mode', 'a');
     *      res.appendHeader('X-Mode', 'b');
     *      console.log(res.allHeader('X-Mode').join(',')); // a,b
     *
     *      res.setHeader('X-Mode', 'c');
     *      console.log(res.allHeader('X-Mode').join(',')); // c
     *
     *      res.setHeader({ 'X-Extra': '1' });
     *      res.removeHeader('X-Mode');
     *      console.log(res.hasHeader('X-Mode'), res.getHeader('X-Mode')); // false undefined
     *      console.log(res.firstHeader('X-Extra')); // 1
     *      ```
     *      @param map specifies the key-value data dictionary to set
     *
     */
    setHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description sets headers; setting data modifies the value of the key and clears the remaining headers with the same key
     *
     *      Clears every existing header first and copies the given collection, so the result contains
     *      exactly its entries (duplicates preserved).
     *      @param headers specifies the Headers object to set
     *
     */
    setHeader(headers: Class_Headers): void;

    /**
     * @description sets a group of headers with the specified name; setting data modifies the value of the key and clears the remaining headers with the same key
     *
     *      Replaces all values of the key with the given array.
     *      @param name specifies the key to set
     *      @param values specifies the group of data to set
     *
     */
    setHeader(name: string, values: any[]): void;

    /**
     * @description sets a header; setting data modifies the first value of the key and clears the remaining headers with the same key
     *
     *      Replaces all values of the key with the single value.
     *      @param name specifies the key to set
     *      @param value specifies the data to set
     *
     */
    setHeader(name: string, value: any): void;

    /**
     * @description deletes all headers of the specified key
     *
     *      Case-insensitive; a missing key is ignored. Returns no data, like Node's
     *      response.removeHeader.
     *      @param name specifies the key to delete
     *
     */
    removeHeader(name: string): void;

    /**
     * @description queries the first header of the specified key
     *
     *      Returns the first value as a string, or undefined when the key does not exist
     *      (Node.js-style). firstHeader returns null instead and allHeader returns every value.
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or undefined if it does not exist
     *
     */
    getHeader(name: string): any;

    /**
     * @description queries all headers
     *
     *      Returns one object with every header: a key with one value maps to the string, a key with
     *      several values maps to an array; the same shape as allHeader(""). Node's
     *      response.getHeaders() returns arrays for every key instead.
     *      @return returns the key-value pairs of all headers
     *
     */
    getHeaders(): FIBJS.GeneralObject;

    /**
     * @description queries whether the headers have been sent
     *
     *      False for a message built in memory; sendTo and send set it to true when the start line
     *      and headers were written to a stream. On a server response it is true once the reply left
     *      the handler pipeline. Node's ServerResponse.headersSent has the same meaning.
     *
     */
    readonly headersSent: boolean;

    /**
     * @description container holding the http trailer headers of the message, read-only property
     *
     *      A live Headers collection filled with the trailer section of a parsed chunked body and
     *      used to queue outgoing trailers together with addTrailers; empty for messages without
     *      trailers and reset by clear().
     *
     */
    readonly trailers: Class_Headers;

    /**
     * @description adds trailer headers, which will be sent after the body
     *
     *      Appends (not replaces) the entries of the object to the trailers collection; the trailer
     *      section is written after a chunked body. Node's response.addTrailers has the same shape.
     *
     *      Example — queue a trailer and read it back:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response();
     *      res.write('body');
     *      res.addTrailers({ 'X-Checksum': 'abc123' });
     *
     *      console.log(res.trailers.get('X-Checksum')); // abc123
     *      ```
     *      @param headers specifies the trailer headers to add
     *
     */
    addTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; any other type throws a TypeError 20024
     *      ("the Content-Type is not a form type"), a multipart type without a boundary throws "the
     *      multipart Content-Type is missing a boundary" and an empty body throws "the body is
     *      empty". The body is consumed (bodyUsed is set), a buffered body can still be read again;
     *      this mirrors the Fetch Body mixin (MDN).
     *
     *      Example — parse an urlencoded response body:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response('name=fibjs&mode=fast', {
     *          headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
     *      });
     *
     *      const form = res.formData();
     *      console.log(form.get('name'), form.get('mode')); // fibjs fast
     *      console.log(res.bodyUsed); // true
     *      ```
     *      @return returns the parsed FormData object
     *
     */
    formData(): Class_FormData;

    formData(callback: (err: Error | undefined | null, retVal: Class_FormData)=>any): void;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; any other type throws a TypeError 20024
     *      ("the Content-Type is not a form type"), a multipart type without a boundary throws "the
     *      multipart Content-Type is missing a boundary" and an empty body throws "the body is
     *      empty". The body is consumed (bodyUsed is set), a buffered body can still be read again;
     *      this mirrors the Fetch Body mixin (MDN).
     *
     *      Example — parse an urlencoded response body:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response('name=fibjs&mode=fast', {
     *          headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
     *      });
     *
     *      const form = res.formData();
     *      console.log(form.get('name'), form.get('mode')); // fibjs fast
     *      console.log(res.bodyUsed); // true
     *      ```
     *      @return returns the parsed FormData object
     *
     */
    formDataSync(): Class_FormData;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; any other type throws a TypeError 20024
     *      ("the Content-Type is not a form type"), a multipart type without a boundary throws "the
     *      multipart Content-Type is missing a boundary" and an empty body throws "the body is
     *      empty". The body is consumed (bodyUsed is set), a buffered body can still be read again;
     *      this mirrors the Fetch Body mixin (MDN).
     *
     *      Example — parse an urlencoded response body:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response('name=fibjs&mode=fast', {
     *          headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
     *      });
     *
     *      const form = res.formData();
     *      console.log(form.get('name'), form.get('mode')); // fibjs fast
     *      console.log(res.bodyUsed); // true
     *      ```
     *      @return returns the parsed FormData object
     *
     */
    formDataAsync(): Promise<Class_FormData>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/// <reference path="../interface/Headers.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/**
 * The promise variant of the HttpMessage class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpMessagePromise extends Class_MessagePromise {
    /**
     * @description protocol version information, the allowed format is: HTTP/#.#
     *
     *      Default HTTP/1.1. The setter validates the form and throws error 20024 for anything else;
     *      a version above 1.0 enables keepAlive and 1.0 disables it. A parsed message sets the
     *      version from its start line. Node.js exposes the same value as httpVersion on
     *      IncomingMessage and ServerResponse.
     *
     *      Example — the version drives keep-alive:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      console.log(req.protocol, req.keepAlive); // HTTP/1.1 true
     *
     *      req.protocol = 'HTTP/1.0';
     *      console.log(req.protocol, req.keepAlive); // HTTP/1.0 false
     *
     *      req.appendHeader('Connection', 'keep-alive');
     *      console.log(req.keepAlive); // true
     *      ```
     *
     */
    protocol: string;

    /**
     * @description container holding the http headers of the message, read-only property
     *
     *      The live Headers collection of the message: the property itself cannot be replaced, but
     *      the collection can be modified with its own methods (set, append, delete, ...) and the
     *      change is reflected in the wire output. Parsed messages store keys lowercase, lookup is
     *      case-insensitive either way. Node.js uses a plain object plus rawHeaders instead.
     *
     */
    readonly headers: Class_Headers;

    /**
     * @description queries and sets whether to keep the connection alive
     *
     *      Default true. Setting protocol above 1.0 turns it on and 1.0 turns it off, and appending
     *      or parsing a Connection header updates it ('close' clears, 'keep-alive' and 'upgrade' set).
     *      On output the value selects the Connection header written by sendTo. Node's IncomingMessage
     *      has no such property, persistence belongs to the Agent.
     *
     */
    keepAlive: boolean;

    /**
     * @description queries and sets whether the protocol is upgraded
     *
     *      Default false. Set when a Connection: Upgrade header is appended or parsed; the WebSocket
     *      handshake relies on it. Node.js reports the same condition through the 'upgrade' event.
     *
     */
    upgrade: boolean;

    /**
     * @description queries and sets the maximum number of request headers, default is 128
     *
     *      The parser fails with error 20024 when a message carries more header lines than this; it
     *      is used when reading requests. A negative value throws a RangeError (20006). Node.js has
     *      an http.Server.maxHeadersCount property instead of a per-message one.
     *
     */
    maxHeadersCount: number;

    /**
     * @description queries and sets the maximum request header length, default is 8192
     *
     *      The maximum size in bytes of one header line accepted by the parser; a longer line fails
     *      with error 20024. A negative value throws a RangeError (20006). Node.js supports the
     *      process-wide --max-http-header-size setting instead.
     *
     */
    maxHeaderSize: number;

    /**
     * @description queries and sets the maximum chunk size in MB, default is 2
     *
     *      Bounds a single chunk of a chunked body while it is parsed: a larger chunk fails with
     *      error 20024 ("HttpMessage: chunk is too huge."). 0 is accepted. Node.js has no equivalent
     *      per-message setting.
     *
     */
    maxChunkSize: number;

    /**
     * @description queries and sets the maximum body size in MB, default is 64
     *
     *      While parsing, a Content-Length or chunked body larger than this fails with error 20024
     *      ("HttpMessage: body is too huge."); -1 disables the limit and 0 reads no body at all. Set
     *      it before readFrom. Node.js has no per-message body limit.
     *
     */
    maxBodySize: number;

    /**
     * @description queries the source socket of the current object
     *
     *      The underlying device stream the message was read from; null when the message was built in
     *      memory and reset by clear(). It differs from stream (inherited from Message), the buffered
     *      wrapper the parser consumed. Node.js exposes socket on IncomingMessage only.
     *
     */
    readonly socket: Class_StreamPromise;

    /**
     * @description checks whether a header of the specified key exists
     *
     *      Lookup is case-insensitive; returns false when the key was never set. See the headers
     *      property for the collection itself.
     *      @param name specifies the key to check
     *      @return returns whether the key exists
     *
     */
    hasHeader(name: string): boolean;

    /**
     * @description queries the first header of the specified key
     *
     *      Returns the first value when the key was appended more than once; a missing key returns
     *      null (not undefined). Use getHeader for the Node.js-style undefined result and allHeader
     *      for every value.
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or null if it does not exist
     *
     */
    firstHeader(name: string): string;

    /**
     * @description queries all headers of the specified key
     *
     *      Returns the values in append order as an array; a missing key returns an empty array. With
     *      an empty name the whole header set is returned as an object whose multi-value keys are
     *      arrays and whose single-value keys are strings, the shape getHeaders() returns.
     *      @param name specifies the key to query; passing an empty string returns the result of all keys
     *      @return returns an array of all values corresponding to the key, or an empty array if the data does not exist
     *
     */
    allHeader(name?: string): FIBJS.GeneralObject;

    /**
     * @description appends a header; appending data does not modify the headers of an existing key
     *
     *      Appends every entry of the object; existing values of the same key are kept, so a key can
     *      end up with several values (Set-Cookie and friends). The overloads cover an object map, a
     *      Headers collection, a name with an array of values and a name with a single value; a
     *      Connection entry updates keepAlive/upgrade as it is appended.
     *
     *      Example — append keeps duplicates, setHeader replaces them:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      req.appendHeader('X-Tag', 'a');
     *      req.appendHeader('X-Tag', 'b');
     *      req.appendHeader({ 'X-Tag': 'c' });
     *
     *      console.log(req.allHeader('X-Tag').join(',')); // a,b,c
     *      console.log(req.firstHeader('X-Tag')); // a
     *
     *      const extra = new http.Headers();
     *      extra.append('X-Tag', 'd');
     *      extra.append('Content-Type', 'text/plain');
     *      req.appendHeader(extra);
     *      console.log(req.allHeader('X-Tag').join(',')); // a,b,c,d
     *      ```
     *      @param map specifies the key-value data dictionary to append
     *
     */
    appendHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description appends headers; appending data does not modify the headers of an existing key
     *
     *      Appends every value of the given collection in order, duplicates included; useful to merge
     *      another message's headers into this one.
     *      @param headers specifies the Headers object to append
     *
     */
    appendHeader(headers: Class_Headers): void;

    /**
     * @description appends a group of headers with the specified name; appending data does not modify the headers of an existing key
     *
     *      Every element of the array is appended as a separate value of the key; non-string scalars
     *      are rendered as strings.
     *      @param name specifies the key to append
     *      @param values specifies the group of data to append
     *
     */
    appendHeader(name: string, values: any[]): void;

    /**
     * @description appends a header; appending data does not modify the headers of an existing key
     *
     *      Appends one value; a non-string scalar (number, boolean, Date) is rendered as its string
     *      form. The same effect is available through headers.append(name, value).
     *      @param name specifies the key to append
     *      @param value specifies the data to append
     *
     */
    appendHeader(name: string, value: any): void;

    /**
     * @description sets a header; setting data modifies the first value of the key and clears the remaining headers with the same key
     *
     *      Sets each key of the object, replacing all of its existing values; keys not present in the
     *      object are left untouched. The setHeader(Headers) overload replaces the whole header set
     *      instead. See appendHeader for the appending counterpart.
     *
     *      Example — replace, set and remove headers:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response();
     *      res.setHeader('X-Mode', 'a');
     *      res.appendHeader('X-Mode', 'b');
     *      console.log(res.allHeader('X-Mode').join(',')); // a,b
     *
     *      res.setHeader('X-Mode', 'c');
     *      console.log(res.allHeader('X-Mode').join(',')); // c
     *
     *      res.setHeader({ 'X-Extra': '1' });
     *      res.removeHeader('X-Mode');
     *      console.log(res.hasHeader('X-Mode'), res.getHeader('X-Mode')); // false undefined
     *      console.log(res.firstHeader('X-Extra')); // 1
     *      ```
     *      @param map specifies the key-value data dictionary to set
     *
     */
    setHeader(map: FIBJS.GeneralObject): void;

    /**
     * @description sets headers; setting data modifies the value of the key and clears the remaining headers with the same key
     *
     *      Clears every existing header first and copies the given collection, so the result contains
     *      exactly its entries (duplicates preserved).
     *      @param headers specifies the Headers object to set
     *
     */
    setHeader(headers: Class_Headers): void;

    /**
     * @description sets a group of headers with the specified name; setting data modifies the value of the key and clears the remaining headers with the same key
     *
     *      Replaces all values of the key with the given array.
     *      @param name specifies the key to set
     *      @param values specifies the group of data to set
     *
     */
    setHeader(name: string, values: any[]): void;

    /**
     * @description sets a header; setting data modifies the first value of the key and clears the remaining headers with the same key
     *
     *      Replaces all values of the key with the single value.
     *      @param name specifies the key to set
     *      @param value specifies the data to set
     *
     */
    setHeader(name: string, value: any): void;

    /**
     * @description deletes all headers of the specified key
     *
     *      Case-insensitive; a missing key is ignored. Returns no data, like Node's
     *      response.removeHeader.
     *      @param name specifies the key to delete
     *
     */
    removeHeader(name: string): void;

    /**
     * @description queries the first header of the specified key
     *
     *      Returns the first value as a string, or undefined when the key does not exist
     *      (Node.js-style). firstHeader returns null instead and allHeader returns every value.
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or undefined if it does not exist
     *
     */
    getHeader(name: string): any;

    /**
     * @description queries all headers
     *
     *      Returns one object with every header: a key with one value maps to the string, a key with
     *      several values maps to an array; the same shape as allHeader(""). Node's
     *      response.getHeaders() returns arrays for every key instead.
     *      @return returns the key-value pairs of all headers
     *
     */
    getHeaders(): FIBJS.GeneralObject;

    /**
     * @description queries whether the headers have been sent
     *
     *      False for a message built in memory; sendTo and send set it to true when the start line
     *      and headers were written to a stream. On a server response it is true once the reply left
     *      the handler pipeline. Node's ServerResponse.headersSent has the same meaning.
     *
     */
    readonly headersSent: boolean;

    /**
     * @description container holding the http trailer headers of the message, read-only property
     *
     *      A live Headers collection filled with the trailer section of a parsed chunked body and
     *      used to queue outgoing trailers together with addTrailers; empty for messages without
     *      trailers and reset by clear().
     *
     */
    readonly trailers: Class_Headers;

    /**
     * @description adds trailer headers, which will be sent after the body
     *
     *      Appends (not replaces) the entries of the object to the trailers collection; the trailer
     *      section is written after a chunked body. Node's response.addTrailers has the same shape.
     *
     *      Example — queue a trailer and read it back:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response();
     *      res.write('body');
     *      res.addTrailers({ 'X-Checksum': 'abc123' });
     *
     *      console.log(res.trailers.get('X-Checksum')); // abc123
     *      ```
     *      @param headers specifies the trailer headers to add
     *
     */
    addTrailers(headers: FIBJS.GeneralObject): void;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; any other type throws a TypeError 20024
     *      ("the Content-Type is not a form type"), a multipart type without a boundary throws "the
     *      multipart Content-Type is missing a boundary" and an empty body throws "the body is
     *      empty". The body is consumed (bodyUsed is set), a buffered body can still be read again;
     *      this mirrors the Fetch Body mixin (MDN).
     *
     *      Example — parse an urlencoded response body:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response('name=fibjs&mode=fast', {
     *          headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
     *      });
     *
     *      const form = res.formData();
     *      console.log(form.get('name'), form.get('mode')); // fibjs fast
     *      console.log(res.bodyUsed); // true
     *      ```
     *      @return returns the parsed FormData object
     *
     */
    formData(): Promise<Class_FormData>;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; any other type throws a TypeError 20024
     *      ("the Content-Type is not a form type"), a multipart type without a boundary throws "the
     *      multipart Content-Type is missing a boundary" and an empty body throws "the body is
     *      empty". The body is consumed (bodyUsed is set), a buffered body can still be read again;
     *      this mirrors the Fetch Body mixin (MDN).
     *
     *      Example — parse an urlencoded response body:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response('name=fibjs&mode=fast', {
     *          headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
     *      });
     *
     *      const form = res.formData();
     *      console.log(form.get('name'), form.get('mode')); // fibjs fast
     *      console.log(res.bodyUsed); // true
     *      ```
     *      @return returns the parsed FormData object
     *
     */
    formDataSync(): Class_FormData;

    /**
     * @description parses the message body into FormData according to Content-Type
     *
     *      Only multipart/form-data (with a boundary parameter in Content-Type) and
     *      application/x-www-form-urlencoded are supported; any other type throws a TypeError 20024
     *      ("the Content-Type is not a form type"), a multipart type without a boundary throws "the
     *      multipart Content-Type is missing a boundary" and an empty body throws "the body is
     *      empty". The body is consumed (bodyUsed is set), a buffered body can still be read again;
     *      this mirrors the Fetch Body mixin (MDN).
     *
     *      Example — parse an urlencoded response body:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const res = new http.Response('name=fibjs&mode=fast', {
     *          headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
     *      });
     *
     *      const form = res.formData();
     *      console.log(form.get('name'), form.get('mode')); // fibjs fast
     *      console.log(res.bodyUsed); // true
     *      ```
     *      @return returns the parsed FormData object
     *
     */
    formDataAsync(): Promise<Class_FormData>;

}


declare namespace Class_HttpMessage {
    const promises: FIBJS.GeneralObject;
}
