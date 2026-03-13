const { describe, it, xdescribe, xit, before, after } = require('node:test');
const assert = require('node:assert');
const http = require('node:http');
const { once } = require('node:events');

// fibjs throws TypeError for abort/timeout; Node.js throws DOMException (AbortError/TimeoutError)
const isFibjs = !!process.versions?.fibjs;
const abortErrorName = isFibjs ? 'TypeError' : 'AbortError';
const timeoutErrorName = isFibjs ? 'TypeError' : 'TimeoutError';

// Helper: start a server with a given handler, returns { server, baseUrl, port }
async function startServer(handler) {
    const server = http.createServer(handler);
    server.listen(0);
    await once(server, 'listening');
    const port = server.address().port;
    return { server, port, baseUrl: `http://127.0.0.1:${port}` };
}

describe("web fetch", () => {
    // ─────────────────────────────────────────────────────────────────────────
    // 1. Global API surface
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - global API", () => {
        it("fetch exists as a function on globalThis", () => {
            assert.strictEqual(typeof globalThis.fetch, 'function');
        });

        it("Request exists on globalThis", () => {
            assert.strictEqual(typeof globalThis.Request, 'function');
        });

        it("Response exists on globalThis", () => {
            assert.strictEqual(typeof globalThis.Response, 'function');
        });

        it("Headers exists on globalThis", () => {
            assert.strictEqual(typeof globalThis.Headers, 'function');
        });

        it("FormData exists on globalThis", () => {
            assert.strictEqual(typeof globalThis.FormData, 'function');
        });
    });

    // ─────────────────────────────────────────────────────────────────────────
    // 2. Basic GET request
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - basic GET", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                res.writeHead(200, { 'Content-Type': 'text/plain' });
                res.end('Hello world');
            });
        });

        after(() => ctx.server.close());

        it("returns a Response instance", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.ok(resp instanceof Response);
        });

        it("status is 200", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.strictEqual(resp.status, 200);
        });

        it("statusText is OK", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.strictEqual(resp.statusText, 'OK');
        });

        it("response.ok is true for 2xx", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.strictEqual(resp.ok, true);
        });

        it("body text matches", async () => {
            const resp = await fetch(ctx.baseUrl);
            const text = await resp.text();
            assert.strictEqual(text, 'Hello world');
        });

        it("response.url matches request url", async () => {
            const resp = await fetch(ctx.baseUrl + '/path');
            assert.ok(resp.url.includes('/path'));
        });
    });

    // ─────────────────────────────────────────────────────────────────────────
    // 3. HTTP methods
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - HTTP methods", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ method: req.method, url: req.url }));
            });
        });

        after(() => ctx.server.close());

        for (const method of ['GET', 'POST', 'PUT', 'DELETE', 'PATCH']) {
            it(`${method} request`, async () => {
                const opts = { method };
                if (method !== 'GET') opts.body = 'test';
                const resp = await fetch(ctx.baseUrl + '/test', opts);
                const data = await resp.json();
                assert.strictEqual(data.method, method);
                assert.strictEqual(data.url, '/test');
            });
        }

        it("HEAD request returns no body", async () => {
            const resp = await fetch(ctx.baseUrl + '/test', { method: 'HEAD' });
            assert.strictEqual(resp.status, 200);
            const text = await resp.text();
            assert.strictEqual(text, '');
        });

        it("OPTIONS request", async () => {
            const resp = await fetch(ctx.baseUrl + '/test', { method: 'OPTIONS' });
            const data = await resp.json();
            assert.strictEqual(data.method, 'OPTIONS');
        });
    });

    // ─────────────────────────────────────────────────────────────────────────
    // 4. Request headers
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - request headers", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                // Echo back all request headers as JSON
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify(req.headers));
            });
        });

        after(() => ctx.server.close());

        it("sends custom headers", async () => {
            const resp = await fetch(ctx.baseUrl, {
                headers: { 'X-Custom': 'hello' }
            });
            const hdrs = await resp.json();
            assert.strictEqual(hdrs['x-custom'], 'hello');
        });

        it("sends multiple custom headers", async () => {
            const resp = await fetch(ctx.baseUrl, {
                headers: {
                    'X-First': 'one',
                    'X-Second': 'two'
                }
            });
            const hdrs = await resp.json();
            assert.strictEqual(hdrs['x-first'], 'one');
            assert.strictEqual(hdrs['x-second'], 'two');
        });

        it("sends headers via Headers object", async () => {
            const h = new Headers();
            h.set('X-Via-Headers', 'yes');
            const resp = await fetch(ctx.baseUrl, { headers: h });
            const hdrs = await resp.json();
            assert.strictEqual(hdrs['x-via-headers'], 'yes');
        });

        it("sends headers via array pairs", async () => {
            const resp = await fetch(ctx.baseUrl, {
                headers: [['X-Array', 'value1']]
            });
            const hdrs = await resp.json();
            assert.strictEqual(hdrs['x-array'], 'value1');
        });

        it("host header is set automatically", async () => {
            const resp = await fetch(ctx.baseUrl);
            const hdrs = await resp.json();
            assert.ok(hdrs['host']);
            assert.ok(hdrs['host'].includes(String(ctx.port)));
        });

        it("user-agent header is set", async () => {
            const resp = await fetch(ctx.baseUrl);
            const hdrs = await resp.json();
            assert.ok(hdrs['user-agent'] || hdrs['User-Agent']);
        });

        it("override user-agent", async () => {
            const resp = await fetch(ctx.baseUrl, {
                headers: { 'User-Agent': 'custom-agent/1.0' }
            });
            const hdrs = await resp.json();
            assert.strictEqual(hdrs['user-agent'], 'custom-agent/1.0');
        });
    });

    // ─────────────────────────────────────────────────────────────────────────
    // 5. Response headers
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - response headers", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                res.writeHead(200, {
                    'Content-Type': 'text/plain',
                    'X-Response-Id': '12345',
                    'X-Multi': 'a',
                    'Set-Cookie': ['cookie1=v1', 'cookie2=v2']
                });
                res.end('ok');
            });
        });

        after(() => ctx.server.close());

        it("response.headers is a Headers instance", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.ok(resp.headers instanceof Headers);
        });

        it("headers.get() returns header value", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.strictEqual(resp.headers.get('x-response-id'), '12345');
        });

        it("headers.get() is case insensitive", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.strictEqual(resp.headers.get('X-Response-Id'), '12345');
            assert.strictEqual(resp.headers.get('x-response-id'), '12345');
        });

        it("headers.has() works", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.strictEqual(resp.headers.has('x-response-id'), true);
            assert.strictEqual(resp.headers.has('x-nonexistent'), false);
        });

        it("content-type header present", async () => {
            const resp = await fetch(ctx.baseUrl);
            assert.strictEqual(resp.headers.get('content-type'), 'text/plain');
        });

        it("headers are iterable", async () => {
            const resp = await fetch(ctx.baseUrl);
            const entries = [...resp.headers];
            assert.ok(entries.length > 0);
            assert.ok(entries.some(([k]) => k === 'content-type'));
        });

        it("headers.keys() and headers.values()", async () => {
            const resp = await fetch(ctx.baseUrl);
            const keys = [...resp.headers.keys()];
            assert.ok(keys.includes('content-type'));
            const values = [...resp.headers.values()];
            assert.ok(values.length > 0);
        });

        it("headers.forEach()", async () => {
            const resp = await fetch(ctx.baseUrl);
            const collected = {};
            resp.headers.forEach((value, key) => {
                collected[key] = value;
            });
            assert.strictEqual(collected['x-response-id'], '12345');
        });

        it("set-cookie via getSetCookie()", async () => {
            const resp = await fetch(ctx.baseUrl);
            const cookies = resp.headers.getSetCookie();
            assert.ok(Array.isArray(cookies));
            assert.ok(cookies.length >= 2);
            assert.ok(cookies.some(c => c.includes('cookie1=v1')));
            assert.ok(cookies.some(c => c.includes('cookie2=v2')));
        });
    });
    // ─────────────────────────────────────────────────────────────────────────
    // 7. Response body consumption methods
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - response body methods", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                if (req.url === '/text') {
                    res.writeHead(200, { 'Content-Type': 'text/plain' });
                    res.end('plain text response');
                } else if (req.url === '/json') {
                    res.writeHead(200, { 'Content-Type': 'application/json' });
                    res.end(JSON.stringify({ key: 'value', num: 123 }));
                } else if (req.url === '/binary') {
                    res.writeHead(200, { 'Content-Type': 'application/octet-stream' });
                    res.end(Buffer.from([0x00, 0x01, 0x02, 0xFF]));
                } else if (req.url === '/blob-type') {
                    res.writeHead(200, { 'Content-Type': 'image/png' });
                    res.end(Buffer.from([0x89, 0x50, 0x4E, 0x47]));
                } else if (req.url === '/empty') {
                    res.writeHead(200, { 'Content-Type': 'text/plain' });
                    res.end('');
                } else {
                    res.writeHead(200);
                    res.end('default');
                }
            });
        });

        after(() => ctx.server.close());

        it("response.text()", async () => {
            const resp = await fetch(ctx.baseUrl + '/text');
            const text = await resp.text();
            assert.strictEqual(text, 'plain text response');
        });

        it("response.json()", async () => {
            const resp = await fetch(ctx.baseUrl + '/json');
            const data = await resp.json();
            assert.deepStrictEqual(data, { key: 'value', num: 123 });
        });

        it("response.arrayBuffer()", async () => {
            const resp = await fetch(ctx.baseUrl + '/binary');
            const buf = await resp.arrayBuffer();
            assert.ok(buf instanceof ArrayBuffer);
            const u8 = new Uint8Array(buf);
            assert.strictEqual(u8[0], 0x00);
            assert.strictEqual(u8[3], 0xFF);
            assert.strictEqual(u8.length, 4);
        });

        it("response.blob()", async () => {
            const resp = await fetch(ctx.baseUrl + '/blob-type');
            const blob = await resp.blob();
            assert.ok(blob instanceof Blob);
            assert.strictEqual(blob.size, 4);
            assert.strictEqual(blob.type, 'image/png');
        });

        it("response.bytes()", async () => {
            const resp = await fetch(ctx.baseUrl + '/binary');
            const bytes = await resp.bytes();
            assert.ok(bytes instanceof Uint8Array);
            assert.strictEqual(bytes.length, 4);
            assert.strictEqual(bytes[0], 0x00);
            assert.strictEqual(bytes[3], 0xFF);
        });

        it("empty body text()", async () => {
            const resp = await fetch(ctx.baseUrl + '/empty');
            const text = await resp.text();
            assert.strictEqual(text, '');
        });

        it("body can only be consumed once", async () => {
            const resp = await fetch(ctx.baseUrl + '/text');
            await resp.text();
            await assert.rejects(() => resp.text(), {
                name: 'TypeError'
            });
        });

        it("response.bodyUsed tracks consumption", async () => {
            const resp = await fetch(ctx.baseUrl + '/text');
            assert.strictEqual(resp.bodyUsed, false);
            await resp.text();
            assert.strictEqual(resp.bodyUsed, true);
        });

        it("response.clone() allows multiple reads", async () => {
            const resp = await fetch(ctx.baseUrl + '/json');
            const clone = resp.clone();
            const data1 = await resp.json();
            const data2 = await clone.json();
            assert.deepStrictEqual(data1, data2);
        });

        it("clone is independent from original", async () => {
            const resp = await fetch(ctx.baseUrl + '/text');
            const clone = resp.clone();
            await resp.text();
            assert.strictEqual(resp.bodyUsed, true);
            assert.strictEqual(clone.bodyUsed, false);
            const text = await clone.text();
            assert.strictEqual(text, 'plain text response');
        });
    });

    // ─────────────────────────────────────────────────────────────────────────
    // 8. Response static methods
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - Response static methods", () => {
        it("Response.json() creates JSON response", async () => {
            const resp = Response.json({ hello: 'world' });
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.headers.get('content-type'), 'application/json');
            const data = await resp.json();
            assert.deepStrictEqual(data, { hello: 'world' });
        });

        it("Response.json() with status", async () => {
            const resp = Response.json({ error: 'not found' }, { status: 404 });
            assert.strictEqual(resp.status, 404);
        });

        it("Response.redirect() creates redirect response", () => {
            const resp = Response.redirect('http://example.com', 302);
            assert.strictEqual(resp.status, 302);
            assert.ok(resp.headers.get('location').startsWith('http://example.com'));
            assert.strictEqual(resp.redirected, false); // it's a new response, not redirected
        });

        it("Response.redirect() defaults to 302", () => {
            const resp = Response.redirect('http://example.com');
            assert.strictEqual(resp.status, 302);
        });

        it("Response.error() creates error response", () => {
            const resp = Response.error();
            assert.strictEqual(resp.type, 'error');
            assert.strictEqual(resp.status, 0);
        });

        it("new Response() with string body", async () => {
            const resp = new Response('hello');
            const text = await resp.text();
            assert.strictEqual(text, 'hello');
        });

        it("new Response() with null body", async () => {
            const resp = new Response(null, { status: 204 });
            assert.strictEqual(resp.status, 204);
        });

        it("new Response() with Blob body", async () => {
            const blob = new Blob(['blob content'], { type: 'text/plain' });
            const resp = new Response(blob);
            const text = await resp.text();
            assert.strictEqual(text, 'blob content');
        });

        it("new Response() with custom headers and status", async () => {
            const resp = new Response('body', {
                status: 201,
                statusText: 'Created',
                headers: { 'X-Custom': 'test' }
            });
            assert.strictEqual(resp.status, 201);
            assert.strictEqual(resp.statusText, 'Created');
            assert.strictEqual(resp.headers.get('x-custom'), 'test');
        });
    });
    // ─────────────────────────────────────────────────────────────────────────
    // 11. Status codes
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - status codes", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                const code = parseInt(req.url.slice(1), 10);
                if (code === 204) {
                    res.writeHead(204);
                    res.end();
                } else if (code === 304) {
                    res.writeHead(304);
                    res.end();
                } else if (code >= 100 && code < 600) {
                    res.writeHead(code, { 'Content-Type': 'text/plain' });
                    res.end(`status ${code}`);
                } else {
                    res.writeHead(200);
                    res.end('ok');
                }
            });
        });

        after(() => ctx.server.close());

        it("200 OK — ok is true", async () => {
            const resp = await fetch(ctx.baseUrl + '/200');
            assert.strictEqual(resp.ok, true);
            assert.strictEqual(resp.status, 200);
        });

        it("201 Created", async () => {
            const resp = await fetch(ctx.baseUrl + '/201');
            assert.strictEqual(resp.status, 201);
            assert.strictEqual(resp.ok, true);
        });

        it("204 No Content", async () => {
            const resp = await fetch(ctx.baseUrl + '/204');
            assert.strictEqual(resp.status, 204);
            assert.strictEqual(resp.ok, true);
            const text = await resp.text();
            assert.strictEqual(text, '');
        });

        it("304 Not Modified", async () => {
            const resp = await fetch(ctx.baseUrl + '/304');
            assert.strictEqual(resp.status, 304);
            assert.strictEqual(resp.ok, false);
        });

        it("400 Bad Request — ok is false", async () => {
            const resp = await fetch(ctx.baseUrl + '/400');
            assert.strictEqual(resp.ok, false);
            assert.strictEqual(resp.status, 400);
        });

        it("401 Unauthorized", async () => {
            const resp = await fetch(ctx.baseUrl + '/401');
            assert.strictEqual(resp.status, 401);
            assert.strictEqual(resp.ok, false);
        });

        it("403 Forbidden", async () => {
            const resp = await fetch(ctx.baseUrl + '/403');
            assert.strictEqual(resp.status, 403);
            assert.strictEqual(resp.ok, false);
        });

        it("404 Not Found", async () => {
            const resp = await fetch(ctx.baseUrl + '/404');
            assert.strictEqual(resp.status, 404);
            assert.strictEqual(resp.ok, false);
        });

        it("500 Internal Server Error", async () => {
            const resp = await fetch(ctx.baseUrl + '/500');
            assert.strictEqual(resp.status, 500);
            assert.strictEqual(resp.ok, false);
        });

        it("body still readable on error status", async () => {
            const resp = await fetch(ctx.baseUrl + '/500');
            const text = await resp.text();
            assert.strictEqual(text, 'status 500');
        });
    });
    // ─────────────────────────────────────────────────────────────────────────
    // 14. Query string & URL handling
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - URL & query string", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ url: req.url }));
            });
        });

        after(() => ctx.server.close());

        it("query string is sent", async () => {
            const resp = await fetch(ctx.baseUrl + '/path?key=value&a=1');
            const data = await resp.json();
            assert.strictEqual(data.url, '/path?key=value&a=1');
        });

        it("URL object is accepted", async () => {
            const url = new URL('/url-obj', ctx.baseUrl);
            url.searchParams.set('q', 'test');
            const resp = await fetch(url);
            const data = await resp.json();
            assert.ok(data.url.includes('/url-obj'));
            assert.ok(data.url.includes('q=test'));
        });

        it("encoded query params", async () => {
            const resp = await fetch(ctx.baseUrl + '/search?q=' + encodeURIComponent('hello world'));
            const data = await resp.json();
            assert.ok(data.url.includes('hello%20world') || data.url.includes('hello+world'));
        });

        it("path with special characters", async () => {
            const resp = await fetch(ctx.baseUrl + '/path/with%20space');
            const data = await resp.json();
            assert.ok(data.url.includes('/path/with'));
        });
    });

    // ─────────────────────────────────────────────────────────────────────────
    // 15. Chunked transfer / streaming
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - chunked & streaming", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                if (req.url === '/chunked') {
                    res.writeHead(200, { 'Content-Type': 'text/plain' });
                    res.write('chunk1');
                    res.write('chunk2');
                    res.write('chunk3');
                    res.end();
                } else if (req.url === '/stream-read') {
                    res.writeHead(200, { 'Content-Type': 'text/plain' });
                    res.write('part1-');
                    res.write('part2-');
                    res.write('part3');
                    res.end();
                } else {
                    res.writeHead(200);
                    res.end('default');
                }
            });
        });

        after(() => ctx.server.close());

        it("reads chunked response via text()", async () => {
            const resp = await fetch(ctx.baseUrl + '/chunked');
            const text = await resp.text();
            assert.strictEqual(text, 'chunk1chunk2chunk3');
        });


        it("response.body is null for HEAD request", async () => {
            const resp = await fetch(ctx.baseUrl + '/chunked', { method: 'HEAD' });
            assert.strictEqual(resp.body, null);
        });
    });
    // ─────────────────────────────────────────────────────────────────────────
    // 17. Keep-alive & connection behavior
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - multiple requests", () => {
        let ctx;
        let requestCount = 0;

        before(async () => {
            requestCount = 0;
            ctx = await startServer((req, res) => {
                requestCount++;
                res.writeHead(200, { 'Content-Type': 'text/plain' });
                res.end(`request #${requestCount}`);
            });
        });

        after(() => ctx.server.close());

        it("sequential requests to same server", async () => {
            const resp1 = await fetch(ctx.baseUrl);
            assert.strictEqual(await resp1.text(), 'request #1');

            const resp2 = await fetch(ctx.baseUrl);
            assert.strictEqual(await resp2.text(), 'request #2');

            const resp3 = await fetch(ctx.baseUrl);
            assert.strictEqual(await resp3.text(), 'request #3');
        });

        it("parallel requests", async () => {
            const promises = Array.from({ length: 5 }, () => fetch(ctx.baseUrl));
            const responses = await Promise.all(promises);
            assert.strictEqual(responses.length, 5);
            for (const resp of responses) {
                assert.strictEqual(resp.status, 200);
                const text = await resp.text();
                assert.ok(text.startsWith('request #'));
            }
        });
    });

    // ─────────────────────────────────────────────────────────────────────────
    // 18. Headers API standalone
    // ─────────────────────────────────────────────────────────────────────────
    describe("fetch - Headers API standalone", () => {
        it("construct from object", () => {
            const h = new Headers({ 'Content-Type': 'text/html', 'X-Custom': 'val' });
            assert.strictEqual(h.get('content-type'), 'text/html');
            assert.strictEqual(h.get('x-custom'), 'val');
        });

        it("construct from array pairs", () => {
            const h = new Headers([['X-A', '1'], ['X-B', '2']]);
            assert.strictEqual(h.get('x-a'), '1');
            assert.strictEqual(h.get('x-b'), '2');
        });

        it("construct from existing Headers", () => {
            const h1 = new Headers({ 'X-Test': 'original' });
            const h2 = new Headers(h1);
            assert.strictEqual(h2.get('x-test'), 'original');
        });

        it("append adds values", () => {
            const h = new Headers();
            h.append('X-Multi', 'a');
            h.append('X-Multi', 'b');
            const val = h.get('x-multi');
            assert.ok(val.includes('a'));
            assert.ok(val.includes('b'));
        });

        it("set replaces value", () => {
            const h = new Headers();
            h.set('X-Key', 'first');
            h.set('X-Key', 'second');
            assert.strictEqual(h.get('x-key'), 'second');
        });

        it("delete removes header", () => {
            const h = new Headers({ 'X-Del': 'val' });
            assert.strictEqual(h.has('x-del'), true);
            h.delete('X-Del');
            assert.strictEqual(h.has('x-del'), false);
        });

        it("entries() returns iterator", () => {
            const h = new Headers({ 'A': '1', 'B': '2' });
            const entries = [...h.entries()];
            assert.ok(entries.length >= 2);
        });

        it("headers are sorted in iteration", () => {
            const h = new Headers({ 'Z-Last': '1', 'A-First': '2', 'M-Mid': '3' });
            const keys = [...h.keys()];
            const sorted = [...keys].sort();
            assert.deepStrictEqual(keys, sorted);
        });

    });


    // ─────────────────────────────────────────────────────────────────────────────
    // Stage C: Redirect tracking and redirect options
    // ─────────────────────────────────────────────────────────────────────────────
    describe("fetch - redirect", () => {
        let ctx;

        before(async () => {
            ctx = await startServer((req, res) => {
                const url = req.url;
                // /redirect/301 → /target  (permanent redirect)
                // /redirect/302 → /target  (temporary redirect)
                // /redirect/303 → /target  (see other: POST→GET)
                // /redirect/307 → /target  (temporary redirect, preserve method)
                // /redirect/308 → /target  (permanent redirect, preserve method)
                // /redirect/chain/1 → /redirect/chain/2 → /target
                // /redirect/abs  → absolute URL to /target
                // /target        → echoes "METHOD PATH"
                if (url === '/target') {
                    res.writeHead(200, { 'Content-Type': 'text/plain' });
                    res.end(`${req.method} ${req.url}`);
                } else if (url.startsWith('/redirect/chain/')) {
                    const step = parseInt(url.split('/').pop(), 10);
                    if (step < 3) {
                        res.writeHead(302, { Location: `/redirect/chain/${step + 1}` });
                    } else {
                        res.writeHead(302, { Location: '/target' });
                    }
                    res.end();
                } else if (url === '/redirect/abs') {
                    res.writeHead(302, { Location: `${ctx.baseUrl}/target` });
                    res.end();
                } else {
                    const code = parseInt(url.split('/').pop(), 10);
                    res.writeHead(code, { Location: '/target' });
                    res.end();
                }
            });
        });
        after(() => ctx && ctx.server.close());

        it("follows 301 redirect", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/301');
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.redirected, true);
        });

        it("follows 302 redirect", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/302');
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.redirected, true);
        });

        it("follows 303 redirect (POST becomes GET)", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/303', { method: 'POST', body: 'data' });
            const text = await resp.text();
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.redirected, true);
            assert.ok(text.startsWith('GET'), `expected GET method, got: ${text}`);
        });

        it("follows 307 redirect (preserves method)", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/307', { method: 'POST', body: 'data' });
            const text = await resp.text();
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.redirected, true);
            assert.ok(text.startsWith('POST'), `expected POST method, got: ${text}`);
        });

        it("follows 308 redirect (preserves method)", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/308', { method: 'PUT', body: 'data' });
            const text = await resp.text();
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.redirected, true);
            assert.ok(text.startsWith('PUT'), `expected PUT method, got: ${text}`);
        });

        it("follows redirect chain", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/chain/1');
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.redirected, true);
        });

        it("follows absolute redirect URL", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/abs');
            assert.strictEqual(resp.status, 200);
            assert.strictEqual(resp.redirected, true);
        });

        it("redirect: manual returns redirect response", async () => {
            const resp = await fetch(ctx.baseUrl + '/redirect/302', { redirect: 'manual' });
            assert.strictEqual(resp.status, 302);
            assert.strictEqual(resp.redirected, false);
        });

        it("redirect: error throws on redirect", async () => {
            await assert.rejects(
                () => fetch(ctx.baseUrl + '/redirect/302', { redirect: 'error' }),
                { name: 'TypeError' }
            );
        });
    });

    // ─────────────────────────────────────────────────────────────────────────────
    describe("fetch - error cases", () => {
        it("connection refused throws TypeError", async () => {
            await assert.rejects(
                () => fetch('http://127.0.0.1:1/'),
                { name: 'TypeError' }
            );
        });

        it("invalid URL throws TypeError", async () => {
            await assert.rejects(
                () => fetch('not-a-valid-url'),
                { name: 'TypeError' }
            );
        });

        describe("redirect loop throws", () => {
            let ctx;
            before(async () => {
                ctx = await startServer((req, res) => {
                    res.writeHead(302, { Location: req.url });
                    res.end();
                });
            });
            after(() => ctx && ctx.server.close());

            it("redirect loop throws TypeError", async () => {
                await assert.rejects(
                    () => fetch(ctx.baseUrl + '/loop'),
                    { name: 'TypeError' }
                );
            });
        });

        // ─────────────────────────────────────────────────────────────────────────
        // B. Request constructor API
        // ─────────────────────────────────────────────────────────────────────────
        describe("fetch - Request API", () => {
            it("new Request() with string URL", () => {
                const req = new Request('http://example.com/path');
                assert.strictEqual(req.method, 'GET');
                assert.ok(req.url.includes('example.com/path'));
            });

            it("new Request() with method", () => {
                const req = new Request('http://example.com', { method: 'POST' });
                assert.strictEqual(req.method, 'POST');
            });

            it("new Request() with headers", () => {
                const req = new Request('http://example.com', {
                    headers: { 'X-Custom': 'test' }
                });
                assert.strictEqual(req.headers.get('x-custom'), 'test');
            });

            it("new Request() with body", async () => {
                const req = new Request('http://example.com', {
                    method: 'POST',
                    body: 'request body'
                });
                const text = await req.text();
                assert.strictEqual(text, 'request body');
            });

            it("Request clone()", async () => {
                const req = new Request('http://example.com', {
                    method: 'POST',
                    body: 'clone me',
                    headers: { 'X-Test': 'val' }
                });
                const clone = req.clone();
                assert.strictEqual(clone.method, 'POST');
                assert.strictEqual(clone.headers.get('x-test'), 'val');
                const text = await clone.text();
                assert.strictEqual(text, 'clone me');
            });

            it("new Request() from existing Request", () => {
                const req1 = new Request('http://example.com', {
                    method: 'PUT',
                    headers: { 'X-From': 'original' }
                });
                const req2 = new Request(req1);
                assert.strictEqual(req2.method, 'PUT');
                assert.strictEqual(req2.headers.get('x-from'), 'original');
            });

            it("new Request() from existing Request with overrides", () => {
                const req1 = new Request('http://example.com', { method: 'PUT' });
                const req2 = new Request(req1, { method: 'DELETE' });
                assert.strictEqual(req2.method, 'DELETE');
            });

            it("GET with body throws TypeError", () => {
                assert.throws(
                    () => new Request('http://localhost', { method: 'GET', body: 'x' }),
                    { name: 'TypeError' }
                );
            });

            it("HEAD with body throws TypeError", () => {
                assert.throws(
                    () => new Request('http://localhost', { method: 'HEAD', body: 'x' }),
                    { name: 'TypeError' }
                );
            });

            it("fetch accepts Request object", async () => {
                const ctx = await startServer((req, res) => {
                    res.writeHead(200, { 'Content-Type': 'text/plain' });
                    res.end(`${req.method} ${req.url}`);
                });
                try {
                    const request = new Request(ctx.baseUrl + '/via-request', { method: 'POST', body: 'hi' });
                    const resp = await fetch(request);
                    const text = await resp.text();
                    assert.strictEqual(text, 'POST /via-request');
                } finally {
                    ctx.server.close();
                }
            });
        });
    });

    // ─────────────────────────────────────────────────────────────────────────────
    // Stage D: AbortSignal
    // ─────────────────────────────────────────────────────────────────────────────
    describe("fetch - AbortSignal", () => {
        it("already aborted signal rejects immediately", async () => {
            const controller = new AbortController();
            controller.abort();
            await assert.rejects(
                () => fetch('http://127.0.0.1:1/', { signal: controller.signal }),
                { name: abortErrorName }
            );
        });

        it("AbortController aborts fetch", async () => {
            let wakeUp;
            const ctx = await startServer(async (req, res) => {
                // Block until woken up from finally
                await new Promise(resolve => { wakeUp = resolve; });
            });
            try {
                const controller = new AbortController();
                const fetchPromise = fetch(ctx.baseUrl + '/hang', { signal: controller.signal });
                // abort after a short delay
                setTimeout(() => controller.abort(), 20);
                await assert.rejects(() => fetchPromise, { name: abortErrorName });
            } finally {
                wakeUp?.();
                ctx.server.closeAllConnections?.();
                ctx.server.close();
            }
        });

        it("AbortSignal.timeout() rejects slow request", async () => {
            let wakeUp;
            const ctx = await startServer(async (req, res) => {
                // Block until woken up from finally
                await new Promise(resolve => { wakeUp = resolve; });
            });
            try {
                await assert.rejects(
                    () => fetch(ctx.baseUrl + '/slow', { signal: AbortSignal.timeout(30) }),
                    { name: timeoutErrorName }
                );
            } finally {
                wakeUp?.();
                ctx.server.closeAllConnections?.();
                ctx.server.close();
            }
        });

        it("AbortSignal.timeout() does not reject fast request", async () => {
            const ctx = await startServer((req, res) => {
                res.writeHead(200, { 'Content-Type': 'text/plain' });
                res.end('fast');
            });
            try {
                const resp = await fetch(ctx.baseUrl + '/fast', { signal: AbortSignal.timeout(500) });
                const text = await resp.text();
                assert.strictEqual(text, 'fast');
            } finally {
                ctx.server.close();
            }
        });
    });
});
