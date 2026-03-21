// Simple HTTP/2 test server for fibjs client testing
// Usage: node test/http2_server_helper.mjs <port> <keyFile> <certFile>

import http2 from 'node:http2';
import fs from 'node:fs';

const port = parseInt(process.argv[2]) || 8443;
const keyFile = process.argv[3];
const certFile = process.argv[4];

const key = fs.readFileSync(keyFile);
const cert = fs.readFileSync(certFile);

const server = http2.createSecureServer({ key, cert });

server.on('stream', (stream, headers) => {
    const method = headers[':method'];
    const path = headers[':path'];

    if (path === '/hello') {
        stream.respond({ ':status': 200, 'content-type': 'text/plain' });
        stream.end('Hello HTTP/2');
    } else if (path === '/echo-headers') {
        // Echo back received headers as JSON
        stream.respond({ ':status': 200, 'content-type': 'application/json' });
        stream.end(JSON.stringify(headers));
    } else if (path === '/echo-body') {
        // Echo back request body
        let body = '';
        stream.on('data', (chunk) => { body += chunk; });
        stream.on('end', () => {
            stream.respond({ ':status': 200, 'content-type': 'text/plain' });
            stream.end(body);
        });
    } else if (path === '/large') {
        stream.respond({ ':status': 200, 'content-type': 'text/plain' });
        stream.end('x'.repeat(1024 * 100)); // 100KB
    } else if (path === '/empty') {
        stream.respond({ ':status': 200, 'content-type': 'text/plain' });
        stream.end('');
    } else if (path === '/multi-headers') {
        stream.respond({
            ':status': 200,
            'x-custom-a': 'val-a',
            'x-custom-b': 'val-b',
            'content-type': 'text/plain'
        });
        stream.end('multi-headers-ok');
    } else if (path.startsWith('/delay/')) {
        const ms = parseInt(path.split('/')[2]) || 100;
        setTimeout(() => {
            stream.respond({ ':status': 200, 'content-type': 'text/plain' });
            stream.end(`delayed:${ms}`);
        }, ms);
    } else if (path === '/post-json') {
        let body = '';
        stream.on('data', (chunk) => { body += chunk; });
        stream.on('end', () => {
            let parsed;
            try { parsed = JSON.parse(body); } catch (e) { parsed = null; }
            stream.respond({ ':status': parsed ? 200 : 400, 'content-type': 'application/json' });
            stream.end(JSON.stringify({ received: parsed, size: body.length }));
        });
    } else if (path === '/large-1m') {
        stream.respond({ ':status': 200, 'content-type': 'application/octet-stream' });
        // Send 1MB in chunks
        const chunk = Buffer.alloc(64 * 1024, 0x41); // 64KB of 'A'
        for (let i = 0; i < 16; i++)
            stream.write(chunk);
        stream.end();
    } else if (path === '/rst-stream') {
        stream.respond({ ':status': 200 });
        stream.close(http2.constants.NGHTTP2_CANCEL);
    } else if (path === '/method') {
        // Echo back the HTTP method
        stream.respond({ ':status': 200, 'content-type': 'text/plain' });
        stream.end(method);
    } else if (path === '/no-content') {
        stream.respond({ ':status': 204 });
        stream.end();
    } else if (path === '/binary-echo') {
        // Echo back binary body preserving raw bytes
        const chunks = [];
        stream.on('data', (chunk) => { chunks.push(chunk); });
        stream.on('end', () => {
            const body = Buffer.concat(chunks);
            stream.respond({ ':status': 200, 'content-type': 'application/octet-stream', 'x-body-length': String(body.length) });
            stream.end(body);
        });
    } else if (path === '/frame-boundary') {
        // Return exactly 16384 bytes (one H2 max frame size)
        stream.respond({ ':status': 200, 'content-type': 'application/octet-stream' });
        stream.end(Buffer.alloc(16384, 0x42));
    } else if (path === '/status/301') {
        stream.respond({ ':status': 301, 'location': `/hello` });
        stream.end('');
    } else if (path === '/status/404') {
        stream.respond({ ':status': 404 });
        stream.end('Not Found');
    } else if (path === '/status/500') {
        stream.respond({ ':status': 500 });
        stream.end('Server Error');
    } else {
        stream.respond({ ':status': 200 });
        stream.end(`ok:${path}`);
    }
});

server.listen(port, () => {
    // Signal ready to parent process
    console.log(`H2_READY:${port}`);
});
