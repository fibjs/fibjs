var { describe, it, before, after } = require('node:test');
var assert = require('assert');
var test_util = require('./test_util');

var http2 = require('http2');
var tls = require('tls');
var crypto = require('crypto');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

// -- Generate TLS certificates --
var pk = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
var pk1 = crypto.generateKeyPair('rsa', { modulusLength: 2048 });

var ca = crypto.createCertificateRequest({
    key: pk.privateKey,
    subject: { CN: "fibjs.org" }
}).issue({
    key: pk.privateKey,
    ca: true,
    issuer: { CN: "fibjs.org" }
});

var crt = crypto.createCertificateRequest({
    key: pk1.privateKey,
    subject: { CN: "localhost" }
}).issue({
    key: pk.privateKey,
    issuer: { CN: "fibjs.org" }
});

// Common connect options for client (skip cert verification for test)
var connectOpts = { rejectUnauthorized: false, rejectUnverified: false };

describe('http2', () => {
    var h2_port = 9880 + base_port;
    var svr;
    var sessionSeq = 0;
    var sessionIdMap = new Map();

    before(() => {
        var ctx = tls.createSecureContext({
            key: pk1.privateKey.export(),
            cert: crt.pem,
            requestCert: false,
            alpnProtocols: ['h2']
        }, true);

        svr = new http2.Server(ctx, h2_port, function (req) { });

        svr.on('session', function (session) {
            sessionSeq += 1;
            sessionIdMap.set(session, sessionSeq);
            session.on('stream', function (stream, headers) {
                handleStream(session, stream, headers);
            });

            session.on('close', function () {
                sessionIdMap.delete(session);
            });
        });

        function handleStream(session, stream, headers) {
                var method = headers[':method'];
                var path = headers[':path'];
                var sessionId = sessionIdMap.get(session) || 0;

                if (path === '/hello') {
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write('Hello HTTP/2');
                    stream.close();
                } else if (path === '/session-id') {
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write(String(sessionId));
                    stream.close();
                } else if (path === '/session-seq') {
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write(String(sessionSeq));
                    stream.close();
                } else if (path === '/echo-headers') {
                    stream.respond({ ':status': 200, 'content-type': 'application/json' });
                    stream.write(JSON.stringify(headers));
                    stream.close();
                } else if (path === '/echo-body') {
                    var chunks = [];
                    var chunk;
                    while ((chunk = stream.read()) !== null)
                        chunks.push(chunk);
                    var body = chunks.length > 0 ? Buffer.concat(chunks) : null;
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    if (body)
                        stream.write(body);
                    stream.close();
                } else if (path === '/large') {
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write('x'.repeat(1024 * 100));
                    stream.close();
                } else if (path === '/empty') {
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.close();
                } else if (path === '/multi-headers') {
                    stream.respond({
                        ':status': 200,
                        'x-custom-a': 'val-a',
                        'x-custom-b': 'val-b',
                        'content-type': 'text/plain'
                    });
                    stream.write('multi-headers-ok');
                    stream.close();
                } else if (path.startsWith('/delay/')) {
                    var ms = parseInt(path.split('/')[2]) || 100;
                    coroutine.sleep(ms);
                    if (stream.closed || stream.destroyed)
                        return;
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write('delayed:' + ms);
                    stream.close();
                } else if (path === '/post-json') {
                    var body = stream.read();
                    var bodyStr = body ? body.toString() : '';
                    var parsed;
                    try { parsed = JSON.parse(bodyStr); } catch (e) { parsed = null; }
                    stream.respond({ ':status': parsed ? 200 : 400, 'content-type': 'application/json' });
                    stream.write(JSON.stringify({ received: parsed, size: bodyStr.length }));
                    stream.close();
                } else if (path === '/large-1m') {
                    stream.respond({ ':status': 200, 'content-type': 'application/octet-stream' });
                    var chunk = Buffer.alloc(64 * 1024, 0x41);
                    for (var i = 0; i < 16; i++)
                        stream.write(chunk);
                    stream.close();
                } else if (path === '/rst-stream') {
                    stream.respond({ ':status': 200 });
                    stream.rstStream(http2.constants.NGHTTP2_CANCEL);
                } else if (path === '/method') {
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write(method);
                    stream.close();
                } else if (path === '/no-content') {
                    stream.respond({ ':status': 204 });
                    stream.close();
                } else if (path === '/binary-echo') {
                    var chunks = [];
                    var chunk;
                    while ((chunk = stream.read()) !== null)
                        chunks.push(chunk);
                    var body = chunks.length > 0 ? Buffer.concat(chunks) : null;
                    var bodyLen = body ? body.length : 0;
                    stream.respond({ ':status': 200, 'content-type': 'application/octet-stream', 'x-body-length': String(bodyLen) });
                    if (body)
                        stream.write(body);
                    stream.close();
                } else if (path === '/frame-boundary') {
                    stream.respond({ ':status': 200, 'content-type': 'application/octet-stream' });
                    stream.write(Buffer.alloc(16384, 0x42));
                    stream.close();
                } else if (path === '/status/301') {
                    stream.respond({ ':status': 301, 'location': '/hello' });
                    stream.close();
                } else if (path === '/status/404') {
                    stream.respond({ ':status': 404 });
                    stream.write('Not Found');
                    stream.close();
                } else if (path === '/status/500') {
                    stream.respond({ ':status': 500 });
                    stream.write('Server Error');
                    stream.close();
                } else if (path === '/large-4m') {
                    stream.respond({ ':status': 200, 'content-type': 'application/octet-stream' });
                    var chunk = Buffer.alloc(64 * 1024, 0x43);
                    for (var i = 0; i < 64; i++)
                        stream.write(chunk);
                    stream.close();
                } else if (path === '/goaway-after-headers') {
                    // Respond, then send GOAWAY - stream should still be readable
                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write('goaway-data');
                    stream.close();
                    // Send GOAWAY after responding
                    session.goaway(0, stream.id);
                } else {
                    stream.respond({ ':status': 200 });
                    stream.write('ok:' + path);
                    stream.close();
                }
        }

        svr.start();
        coroutine.sleep(50);
    });

    after(() => {
        if (svr) {
            svr.stop();
            svr = null;
        }
    });

    describe('getDefaultSettings', () => {
        it('should return correct default settings', () => {
            var defaults = http2.getDefaultSettings();
            assert.strictEqual(typeof defaults.headerTableSize, 'number');
            assert.strictEqual(typeof defaults.enablePush, 'boolean');
            assert.strictEqual(typeof defaults.initialWindowSize, 'number');
            assert.strictEqual(typeof defaults.maxFrameSize, 'number');
            assert.strictEqual(typeof defaults.maxConcurrentStreams, 'number');
            assert.strictEqual(typeof defaults.maxHeaderListSize, 'number');

            assert.strictEqual(defaults.headerTableSize, 4096);
            assert.strictEqual(defaults.initialWindowSize, 65535);
            assert.strictEqual(defaults.maxFrameSize, 16384);
        });
    });

    describe('connect', () => {
        it('should connect to an HTTP/2 server', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            assert.ok(session);
            assert.strictEqual(session.closed, false);
            assert.strictEqual(session.destroyed, false);

            session.close();
        });

        it('should have alpnProtocol = h2', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            assert.strictEqual(session.alpnProtocol, 'h2');

            session.close();
        });

        it('should have socket property', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            assert.ok(session.socket);

            session.close();
        });

        it('should have settings properties', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var local = session.localSettings;
            assert.ok(local);
            assert.strictEqual(typeof local.headerTableSize, 'number');
            assert.strictEqual(typeof local.maxConcurrentStreams, 'number');

            var remote = session.remoteSettings;
            assert.ok(remote);
            assert.strictEqual(typeof remote.headerTableSize, 'number');

            session.close();
        });
    });

    describe('request - basic GET', () => {
        it('should send GET and receive response', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/hello'
            });

            assert.ok(stream);
            assert.strictEqual(typeof stream.id, 'number');
            assert.ok(stream.id > 0);

            var buf = stream.read();
            assert.ok(buf);
            assert.strictEqual(buf.toString(), 'Hello HTTP/2');

            session.close();
        });

        it('should receive response headers via event', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/hello'
            });

            stream.read();

            var responseHeaders = stream.headers;
            assert.ok(responseHeaders);
            assert.strictEqual(responseHeaders[':status'], '200');
            assert.strictEqual(responseHeaders['content-type'], 'text/plain');

            session.close();
        });

        it('should handle different paths', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/status/404'
            });

            var buf = stream.read();
            var responseHeaders = stream.headers;
            assert.ok(responseHeaders);
            assert.strictEqual(responseHeaders[':status'], '404');

            session.close();
        });
    });

    describe('request - multiplexing', () => {
        it('should support multiple concurrent streams', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var paths = ['/a', '/b', '/c'];
            var streams = paths.map(p =>
                session.request({ ':method': 'GET', ':path': p })
            );

            var results = streams.map(s => s.read().toString());

            assert.strictEqual(results[0], 'ok:/a');
            assert.strictEqual(results[1], 'ok:/b');
            assert.strictEqual(results[2], 'ok:/c');

            session.close();
        });
    });

    describe('stream properties', () => {
        it('should have id, closed, destroyed', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/hello'
            });

            assert.strictEqual(typeof stream.id, 'number');
            assert.ok(stream.id > 0);
            assert.strictEqual(stream.closed, false);
            assert.strictEqual(stream.destroyed, false);

            stream.read();

            session.close();
        });
    });

    describe('session lifecycle', () => {
        it('should set closed after close()', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            assert.strictEqual(session.closed, false);
            session.close();
            assert.strictEqual(session.closed, true);
        });

        it('should set destroyed after destroy()', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            assert.strictEqual(session.destroyed, false);
            session.destroy();
            assert.strictEqual(session.destroyed, true);
        });
    });

    describe('auto-upgrade via HttpClient', () => {
        var http = require('http');
        var hc;

        before(() => {
            hc = new http.Client(connectOpts);
        });

        it('should auto-negotiate h2 and get response', () => {
            var resp = hc.getSync(`https://localhost:${h2_port}/hello`);
            assert.strictEqual(resp.statusCode, 200);
            var body = resp.body.read(-1);
            assert.strictEqual(body.toString(), 'Hello HTTP/2');
        });

        it('should reuse h2 session for subsequent requests', () => {
            var resp1 = hc.getSync(`https://localhost:${h2_port}/a`);
            assert.strictEqual(resp1.statusCode, 200);
            assert.strictEqual(resp1.body.read(-1).toString(), 'ok:/a');

            var resp2 = hc.getSync(`https://localhost:${h2_port}/b`);
            assert.strictEqual(resp2.statusCode, 200);
            assert.strictEqual(resp2.body.read(-1).toString(), 'ok:/b');
        });

        it('should receive response headers', () => {
            var resp = hc.getSync(`https://localhost:${h2_port}/hello`);
            assert.strictEqual(resp.statusCode, 200);
            assert.ok(resp.firstHeader('content-type'));
            assert.strictEqual(resp.firstHeader('content-type'), 'text/plain');
        });

        it('should handle different status codes', () => {
            var resp = hc.getSync(`https://localhost:${h2_port}/status/404`);
            assert.strictEqual(resp.statusCode, 404);
        });

        it('should handle large response body', () => {
            var resp = hc.getSync(`https://localhost:${h2_port}/large`);
            assert.strictEqual(resp.statusCode, 200);
            var totalLen = 0;
            var chunk;
            while ((chunk = resp.body.read(-1)) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 1024 * 100);
        });

        it('should abort in-flight HttpClient request via signal', () => {
            var controller = new AbortController();
            coroutine.start(() => {
                coroutine.sleep(100);
                controller.abort();
            });

            var t1 = Date.now();
            assert.throws(() => {
                hc.getSync(`https://localhost:${h2_port}/delay/500`, {
                    signal: controller.signal
                });
            }, /AbortError/);
            var t2 = Date.now();

            assert.ok(t2 - t1 < 500);
        });
    });

    describe('HttpClient session sharing and isolation', () => {
        var http = require('http');

        function getTextBody(resp) {
            var body = resp.body.read(-1);
            return body ? body.toString() : '';
        }

        it('should share H2 session across agents with same transport identity', () => {
            var sharedCtx = tls.createSecureContext({
                rejectUnauthorized: false,
                rejectUnverified: false
            }, false);
            var opts = Object.assign({}, connectOpts, {
                secureContext: sharedCtx
            });

            var hc1 = new http.Client(opts);
            var hc2 = new http.Client(opts);

            var sid1 = getTextBody(hc1.getSync(`https://localhost:${h2_port}/session-id`));
            var sid2 = getTextBody(hc2.getSync(`https://localhost:${h2_port}/session-id`));

            assert.ok(sid1);
            assert.strictEqual(sid1, sid2);
        });

        it('should isolate H2 session when transport identity differs', () => {
            var customAlpnCtx = tls.createSecureContext({
                alpnProtocols: ['h2'],
                rejectUnauthorized: false,
                rejectUnverified: false
            }, false);

            var hcCustom = new http.Client(Object.assign({}, connectOpts, {
                secureContext: customAlpnCtx
            }));
            var hcAuto = new http.Client(connectOpts);

            var sidCustom = getTextBody(hcCustom.getSync(`https://localhost:${h2_port}/session-id`));
            var sidAuto = getTextBody(hcAuto.getSync(`https://localhost:${h2_port}/session-id`));

            assert.ok(sidCustom);
            assert.ok(sidAuto);
            assert.notStrictEqual(sidCustom, sidAuto);
        });

        it('should keep one shared session under concurrent cross-agent requests with same identity', () => {
            var sharedCtx = tls.createSecureContext({
                rejectUnauthorized: false,
                rejectUnverified: false
            }, false);
            var opts = Object.assign({}, connectOpts, {
                secureContext: sharedCtx
            });

            var fibers = [];
            var sids = [];
            var errors = [];

            for (var i = 0; i < 8; i++) {
                fibers.push(coroutine.start(() => {
                    try {
                        var hc = new http.Client(opts);
                        var sid = getTextBody(hc.getSync(`https://localhost:${h2_port}/session-id`));
                        sids.push(sid);
                    } catch (e) {
                        errors.push(e);
                    }
                }));
            }

            fibers.forEach(f => f.join());

            assert.strictEqual(errors.length, 0, errors[0] && errors[0].message);
            assert.strictEqual(sids.length, 8);

            var uniq = new Set(sids);
            assert.strictEqual(uniq.size, 1);
        });
    });

    describe('edge cases', () => {
        it('should handle empty response body', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/empty'
            });

            var buf = stream.read();
            assert.ok(buf === null || buf.toString() === '');

            session.close();
        });

        it('should receive multiple custom headers', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/multi-headers'
            });

            stream.read();

            var responseHeaders = stream.headers;
            assert.strictEqual(responseHeaders['x-custom-a'], 'val-a');
            assert.strictEqual(responseHeaders['x-custom-b'], 'val-b');

            session.close();
        });

        it('should handle echo-headers', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/echo-headers',
                'x-test-header': 'test-value-123'
            });

            var buf = stream.read();
            var echoed = JSON.parse(buf.toString());
            assert.strictEqual(echoed[':method'], 'GET');
            assert.strictEqual(echoed[':path'], '/echo-headers');
            assert.strictEqual(echoed['x-test-header'], 'test-value-123');

            session.close();
        });

        it('should handle status 500', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/status/500'
            });

            stream.read();
            var responseHeaders = stream.headers;
            assert.strictEqual(responseHeaders[':status'], '500');

            session.close();
        });

        it('should handle 1MB response body', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/large-1m'
            });

            var totalLen = 0;
            var chunk;
            while ((chunk = stream.read()) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 1024 * 1024);

            session.close();
        });

        it('should handle many sequential requests on same session', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            for (var i = 0; i < 50; i++) {
                var stream = session.request({
                    ':method': 'GET',
                    ':path': '/seq/' + i
                });

                var buf = stream.read();
                assert.strictEqual(buf.toString(), 'ok:/seq/' + i);
            }

            session.close();
        });
    });

    describe('concurrency', () => {
        it('should handle rapid session create/close cycles', () => {
            for (var i = 0; i < 10; i++) {
                var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

                var stream = session.request({
                    ':method': 'GET',
                    ':path': '/cycle/' + i
                });

                var buf = stream.read();
                assert.strictEqual(buf.toString(), 'ok:/cycle/' + i);

                session.close();
            }
        });

        it('should handle HttpClient sequential stress', () => {
            var http = require('http');
            var hc = new http.Client(connectOpts);

            for (var i = 0; i < 30; i++) {
                var resp = hc.getSync(`https://localhost:${h2_port}/stress/${i}`);
                assert.strictEqual(resp.statusCode, 200);
                assert.strictEqual(resp.body.read(-1).toString(), `ok:/stress/${i}`);
            }
        });

    });

    describe('stability', () => {
        it('should handle concurrent fibers hitting different HttpClients', () => {
            var http = require('http');
            var fibers = [];
            var errors = [];

            for (var i = 0; i < 5; i++) {
                (function (idx) {
                    fibers.push(coroutine.start(() => {
                        try {
                            var hc = new http.Client(connectOpts);
                            for (var j = 0; j < 10; j++) {
                                var resp = hc.getSync(`https://localhost:${h2_port}/mt/${idx}/${j}`);
                                assert.strictEqual(resp.statusCode, 200);
                                assert.strictEqual(resp.body.read(-1).toString(), `ok:/mt/${idx}/${j}`);
                            }
                        } catch (e) {
                            errors.push(e);
                        }
                    }));
                })(i);
            }

            fibers.forEach(f => f.join());
            assert.strictEqual(errors.length, 0, errors[0] && errors[0].message);
        });

        it('should handle session ping', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var rtt = session.ping();
            assert.strictEqual(typeof rtt, 'number');
            assert.ok(rtt >= 0);

            session.close();
        });
    });

    describe('POST body via HttpClient', () => {
        var http = require('http');

        it('should send POST body and receive echo', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.postSync(`https://localhost:${h2_port}/echo-body`, {
                body: 'Hello POST'
            });
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'Hello POST');
        });

        it('should send JSON POST body', () => {
            var hc = new http.Client(connectOpts);
            var payload = JSON.stringify({ key: 'value', num: 42 });
            var resp = hc.postSync(`https://localhost:${h2_port}/post-json`, {
                headers: { 'content-type': 'application/json' },
                body: payload
            });
            assert.strictEqual(resp.statusCode, 200);
            var result = JSON.parse(resp.body.read(-1).toString());
            assert.deepStrictEqual(result.received, { key: 'value', num: 42 });
            assert.strictEqual(result.size, payload.length);
        });

        it('should send large POST body', () => {
            var hc = new http.Client(connectOpts);
            var largeBody = 'X'.repeat(128 * 1024); // 128KB
            var resp = hc.postSync(`https://localhost:${h2_port}/echo-body`, {
                body: largeBody
            });
            assert.strictEqual(resp.statusCode, 200);
            var totalLen = 0;
            var chunk;
            while ((chunk = resp.body.read(-1)) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 128 * 1024);
        });

        it('should handle POST with empty body', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.postSync(`https://localhost:${h2_port}/echo-body`, {
                body: ''
            });
            assert.strictEqual(resp.statusCode, 200);
            var body = resp.body.read(-1);
            assert.ok(body === null || body.toString() === '');
        });

        it('should handle PUT method with body', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.putSync(`https://localhost:${h2_port}/echo-body`, {
                body: 'PUT data'
            });
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'PUT data');
        });

        it('should handle POST without body property', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.postSync(`https://localhost:${h2_port}/method`);
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'POST');
        });

        it('should handle DELETE method', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.delSync(`https://localhost:${h2_port}/method`);
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'DELETE');
        });

        it('should handle PATCH method with body', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.patchSync(`https://localhost:${h2_port}/echo-body`, {
                body: 'PATCH data'
            });
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'PATCH data');
        });

        it('should send binary POST body', () => {
            var hc = new http.Client(connectOpts);
            // Create binary data with null bytes and high bytes
            var binData = Buffer.alloc(256);
            for (var i = 0; i < 256; i++)
                binData[i] = i;
            var resp = hc.postSync(`https://localhost:${h2_port}/binary-echo`, {
                headers: { 'content-type': 'application/octet-stream' },
                body: binData
            });
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.firstHeader('x-body-length'), '256');
            var respBody = resp.body.read(-1);
            assert.strictEqual(respBody.length, 256);
            for (var i = 0; i < 256; i++)
                assert.strictEqual(respBody[i], i);
        });

        it('should handle POST body at H2 initial window size boundary', () => {
            var hc = new http.Client(connectOpts);
            // 65535 = default initial window size
            var body = 'W'.repeat(65535);
            var resp = hc.postSync(`https://localhost:${h2_port}/echo-body`, {
                body: body
            });
            assert.strictEqual(resp.statusCode, 200);
            var totalLen = 0;
            var chunk;
            while ((chunk = resp.body.read(-1)) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 65535);
        });
    });

    describe('session methods', () => {
        it('should handle goaway from client', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            // Make a request first to establish communication
            var stream = session.request({ ':method': 'GET', ':path': '/hello' });
            var buf = stream.read();
            assert.strictEqual(buf.toString(), 'Hello HTTP/2');

            // Send goaway, session should be closeable
            session.goaway(0, 0);
            session.close();
            assert.strictEqual(session.closed, true);
        });

        it('should handle custom settings', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            // Submit custom settings - should not throw
            session.settings({ maxConcurrentStreams: 50 });

            // Session should still work after settings change
            var stream = session.request({ ':method': 'GET', ':path': '/hello' });
            var buf = stream.read();
            assert.strictEqual(buf.toString(), 'Hello HTTP/2');

            session.close();
        });
    });

    describe('pseudo-header auto-fill', () => {
        it('should auto-fill :method to GET when not provided', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            // Only provide :path — :method, :scheme, :authority should be auto-filled
            var stream = session.request({ ':path': '/method' });
            var buf = stream.read();
            assert.strictEqual(buf.toString(), 'GET');

            session.close();
        });

        it('should auto-fill :path to / when not provided', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            // Only provide :method — :path should default to /
            var stream = session.request({ ':method': 'GET' });
            var buf = stream.read();
            assert.strictEqual(buf.toString(), 'ok:/');

            session.close();
        });

        it('should accept explicit pseudo-headers without duplication', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({
                ':method': 'GET',
                ':path': '/echo-headers',
                ':scheme': 'https',
                ':authority': `localhost:${h2_port}`
            });

            var buf = stream.read();
            var echoed = JSON.parse(buf.toString());
            assert.strictEqual(echoed[':method'], 'GET');
            assert.strictEqual(echoed[':path'], '/echo-headers');
            assert.strictEqual(echoed[':scheme'], 'https');

            session.close();
        });
    });

    describe('response edge cases', () => {
        it('should handle 204 No Content', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({ ':method': 'GET', ':path': '/no-content' });
            var buf = stream.read();
            var responseHeaders = stream.headers;
            assert.strictEqual(responseHeaders[':status'], '204');
            assert.ok(buf === null || buf.length === 0);

            session.close();
        });

        it('should handle response body at frame boundary (16384)', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({ ':method': 'GET', ':path': '/frame-boundary' });
            var totalLen = 0;
            var chunk;
            while ((chunk = stream.read()) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 16384);

            session.close();
        });

        it('should handle 204 No Content via HttpClient', () => {
            var http = require('http');
            var hc = new http.Client(connectOpts);
            var resp = hc.getSync(`https://localhost:${h2_port}/no-content`);
            assert.strictEqual(resp.statusCode, 204);
        });

        it('should read null after stream EOF', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({ ':method': 'GET', ':path': '/hello' });

            // First read: actual data
            var buf = stream.read();
            assert.ok(buf !== null);
            assert.strictEqual(buf.toString(), 'Hello HTTP/2');

            // Second read: should return null (EOF)
            var buf2 = stream.read();
            assert.strictEqual(buf2, null);

            session.close();
        });
    });

    describe('session lifecycle edge cases', () => {
        it('should handle destroy() with pending stream', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            // Start a request to delayed endpoint
            var stream = session.request({ ':method': 'GET', ':path': '/delay/100' });

            // Destroy session immediately before response
            session.destroy();
            assert.strictEqual(session.destroyed, true);

            // Stream read should return null (session gone)
            var buf = stream.read();
            assert.strictEqual(buf, null);
        });

        it('should handle close() then request fails', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);
            session.close();

            assert.throws(() => {
                session.request({ ':method': 'GET', ':path': '/hello' });
            });
        });

        it('should handle multiple close() calls', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);
            session.close();
            session.close(); // should not throw
            assert.strictEqual(session.closed, true);
        });

        it('should handle multiple destroy() calls', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);
            session.destroy();
            session.destroy(); // should not throw
            assert.strictEqual(session.destroyed, true);
        });

        it('should handle new session after previous one closed', () => {
            // Create+close a session, then create another to same host
            var session1 = http2.connect(`https://localhost:${h2_port}`, connectOpts);
            var stream1 = session1.request({ ':method': 'GET', ':path': '/hello' });
            assert.strictEqual(stream1.read().toString(), 'Hello HTTP/2');
            session1.close();

            // New session should work independently
            var session2 = http2.connect(`https://localhost:${h2_port}`, connectOpts);
            var stream2 = session2.request({ ':method': 'GET', ':path': '/hello' });
            assert.strictEqual(stream2.read().toString(), 'Hello HTTP/2');
            session2.close();
        });
    });

    describe('concurrent session creation via HttpClient', () => {
        it('should handle concurrent fibers on same HttpClient', () => {
            var http = require('http');
            var hc = new http.Client(connectOpts);
            var fibers = [];
            var errors = [];
            var done = new Array(10).fill(false);

            for (var i = 0; i < 10; i++) {
                (function (idx) {
                    fibers.push(coroutine.start(() => {
                        try {
                            var resp = hc.getSync(`https://localhost:${h2_port}/conc/${idx}`);
                            assert.strictEqual(resp.statusCode, 200);
                            assert.strictEqual(resp.body.read(-1).toString(), `ok:/conc/${idx}`);
                            done[idx] = true;
                        } catch (e) {
                            errors.push(e);
                            done[idx] = 'error: ' + e.message;
                        }
                    }));
                })(i);
            }

            fibers.forEach(f => f.join());
            assert.strictEqual(errors.length, 0, errors[0] && errors[0].message);
        });
    });

    describe('large response and window size', () => {
        it('should handle 4MB response body', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({ ':method': 'GET', ':path': '/large-4m' });
            var totalLen = 0;
            var chunk;
            while ((chunk = stream.read()) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 4 * 1024 * 1024);

            session.close();
        });

        it('should handle 4MB response via HttpClient', () => {
            var http = require('http');
            var hc = new http.Client(connectOpts);
            var resp = hc.getSync(`https://localhost:${h2_port}/large-4m`);
            assert.strictEqual(resp.statusCode, 200);
            var totalLen = 0;
            var chunk;
            while ((chunk = resp.body.read(-1)) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 4 * 1024 * 1024);
        });
    });

    describe('GOAWAY handling', () => {
        it('should still read data after server sends GOAWAY', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({ ':method': 'GET', ':path': '/goaway-after-headers' });
            var buf = stream.read();
            assert.ok(buf !== null);
            assert.strictEqual(buf.toString(), 'goaway-data');

            session.close();
        });
    });

    describe('stream error vs normal close', () => {
        it('should throw on RST_STREAM with error code', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({ ':method': 'GET', ':path': '/rst-stream' });

            // Server sends RST_STREAM with CANCEL - read should throw
            assert.throws(() => {
                stream.read();
            });

            session.close();
        });

        it('should return null on normal session destroy', () => {
            var session = http2.connect(`https://localhost:${h2_port}`, connectOpts);

            var stream = session.request({ ':method': 'GET', ':path': '/delay/100' });
            session.destroy();

            // Normal destroy (error_code=0) - read returns null
            var buf = stream.read();
            assert.strictEqual(buf, null);
        });
    });

    describe('concurrent POST via HttpClient', () => {
        it('should handle concurrent POST requests', () => {
            var http = require('http');
            var hc = new http.Client(connectOpts);
            var fibers = [];
            var errors = [];

            for (var i = 0; i < 5; i++) {
                (function (idx) {
                    fibers.push(coroutine.start(() => {
                        try {
                            var body = 'post-data-' + idx;
                            var resp = hc.postSync(`https://localhost:${h2_port}/echo-body`, {
                                body: body
                            });
                            assert.strictEqual(resp.statusCode, 200);
                            assert.strictEqual(resp.body.read(-1).toString(), body);
                        } catch (e) {
                            errors.push(e);
                        }
                    }));
                })(i);
            }

            fibers.forEach(f => f.join());
            assert.strictEqual(errors.length, 0, errors[0] && errors[0].message);
        });
    });
});
