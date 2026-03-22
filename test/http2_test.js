var { describe, it, before, after } = require('node:test');
var assert = require('assert');
var test_util = require('./test_util');

var http2 = require('http2');
var tls = require('tls');
var crypto = require('crypto');
var child_process = require('child_process');
var coroutine = require('coroutine');
var path = require('path');
var fs = require('fs');
var os = require('os');

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

// Write certs to temp files for Node.js helper server
var tmpDir = os.tmpdir() + '/fibjs_h2test_' + process.pid;
try { fs.mkdir(tmpDir); } catch (e) { }
var keyFile = tmpDir + '/key.pem';
var certFile = tmpDir + '/cert.pem';
fs.writeFile(keyFile, pk1.privateKey.export());
fs.writeFile(certFile, crt.pem);

// Common connect options for client (skip cert verification for test)
var connectOpts = { rejectUnauthorized: false, rejectUnverified: false };

describe('http2', () => {
    var h2_port = 9880 + base_port;
    var serverProc;

    before(() => {
        // Start Node.js HTTP/2 test server
        var helperScript = path.join(__dirname, 'http2_server_helper.mjs');
        serverProc = child_process.spawn('node', [helperScript, h2_port, keyFile, certFile]);

        // Wait for server ready signal by reading stdout directly
        var ready = false;
        var buf = '';
        var deadline = new Date().getTime() + 5000;
        while (!ready && new Date().getTime() < deadline) {
            var chunk = serverProc.stdout.read();
            if (chunk) {
                buf += chunk.toString();
                if (buf.indexOf('H2_READY:') >= 0)
                    ready = true;
            } else {
                coroutine.sleep(50);
            }
        }

        if (!ready) {
            // Clean up the server process before throwing
            try { serverProc.kill(); } catch (e) { }
            try { serverProc.join(); } catch (e) { }
            serverProc = null;
            throw new Error('HTTP/2 test server failed to start');
        }
    });

    after(() => {
        if (serverProc) {
            serverProc.kill();
            serverProc.join();
            serverProc = null;
        }
        // Cleanup temp files
        try { fs.unlink(keyFile); } catch (e) { }
        try { fs.unlink(certFile); } catch (e) { }
        try { fs.rmdir(tmpDir); } catch (e) { }
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

            var responseHeaders;
            var stream = session.request({
                ':method': 'GET',
                ':path': '/hello'
            });

            stream.on('headers', (headers) => {
                responseHeaders = headers;
            });

            stream.read();

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

            var responseHeaders;
            stream.on('headers', (headers) => {
                responseHeaders = headers;
            });

            var buf = stream.read();
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
            var resp = hc.get(`https://localhost:${h2_port}/hello`);
            assert.strictEqual(resp.statusCode, 200);
            var body = resp.body.read(-1);
            assert.strictEqual(body.toString(), 'Hello HTTP/2');
        });

        it('should reuse h2 session for subsequent requests', () => {
            var resp1 = hc.get(`https://localhost:${h2_port}/a`);
            assert.strictEqual(resp1.statusCode, 200);
            assert.strictEqual(resp1.body.read(-1).toString(), 'ok:/a');

            var resp2 = hc.get(`https://localhost:${h2_port}/b`);
            assert.strictEqual(resp2.statusCode, 200);
            assert.strictEqual(resp2.body.read(-1).toString(), 'ok:/b');
        });

        it('should receive response headers', () => {
            var resp = hc.get(`https://localhost:${h2_port}/hello`);
            assert.strictEqual(resp.statusCode, 200);
            assert.ok(resp.firstHeader('content-type'));
            assert.strictEqual(resp.firstHeader('content-type'), 'text/plain');
        });

        it('should handle different status codes', () => {
            var resp = hc.get(`https://localhost:${h2_port}/status/404`);
            assert.strictEqual(resp.statusCode, 404);
        });

        it('should handle large response body', () => {
            var resp = hc.get(`https://localhost:${h2_port}/large`);
            assert.strictEqual(resp.statusCode, 200);
            var totalLen = 0;
            var chunk;
            while ((chunk = resp.body.read(-1)) !== null) {
                totalLen += chunk.length;
            }
            assert.strictEqual(totalLen, 1024 * 100);
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

            var responseHeaders;
            var stream = session.request({
                ':method': 'GET',
                ':path': '/multi-headers'
            });

            stream.on('headers', (headers) => {
                responseHeaders = headers;
            });

            stream.read();

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

            var responseHeaders;
            var stream = session.request({
                ':method': 'GET',
                ':path': '/status/500'
            });

            stream.on('headers', (headers) => {
                responseHeaders = headers;
            });

            stream.read();
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
                var resp = hc.get(`https://localhost:${h2_port}/stress/${i}`);
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
                                var resp = hc.get(`https://localhost:${h2_port}/mt/${idx}/${j}`);
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
            var resp = hc.post(`https://localhost:${h2_port}/echo-body`, {
                body: 'Hello POST'
            });
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'Hello POST');
        });

        it('should send JSON POST body', () => {
            var hc = new http.Client(connectOpts);
            var payload = JSON.stringify({ key: 'value', num: 42 });
            var resp = hc.post(`https://localhost:${h2_port}/post-json`, {
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
            var resp = hc.post(`https://localhost:${h2_port}/echo-body`, {
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
            var resp = hc.post(`https://localhost:${h2_port}/echo-body`, {
                body: ''
            });
            assert.strictEqual(resp.statusCode, 200);
            var body = resp.body.read(-1);
            assert.ok(body === null || body.toString() === '');
        });

        it('should handle PUT method with body', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.put(`https://localhost:${h2_port}/echo-body`, {
                body: 'PUT data'
            });
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'PUT data');
        });

        it('should handle POST without body property', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.post(`https://localhost:${h2_port}/method`);
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'POST');
        });

        it('should handle DELETE method', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.del(`https://localhost:${h2_port}/method`);
            assert.strictEqual(resp.statusCode, 200);
            assert.strictEqual(resp.body.read(-1).toString(), 'DELETE');
        });

        it('should handle PATCH method with body', () => {
            var hc = new http.Client(connectOpts);
            var resp = hc.patch(`https://localhost:${h2_port}/echo-body`, {
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
            var resp = hc.post(`https://localhost:${h2_port}/binary-echo`, {
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
            var resp = hc.post(`https://localhost:${h2_port}/echo-body`, {
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

            var responseHeaders;
            var stream = session.request({ ':method': 'GET', ':path': '/no-content' });
            stream.on('headers', (headers) => { responseHeaders = headers; });
            var buf = stream.read();
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
            var resp = hc.get(`https://localhost:${h2_port}/no-content`);
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
            var stream = session.request({ ':method': 'GET', ':path': '/delay/500' });

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
});
