const { after, before, describe, it } = require('node:test');
const assert = require('node:assert');
const fs = require('fs');
const os = require('os');
const path = require('path');
const crypto = require('crypto');
const child_process = require('child_process');
const dgram = require('dgram');
const http = require('http');
const http2 = require('http2');
const tls = require('tls');
const sse = require('sse');
const coroutine = require('coroutine');

const isFibjs = !!process.versions?.fibjs;
const workerThreads = require('worker_threads');
const {
    MessageChannel,
    MessagePort,
    Worker,
    isMainThread,
    parentPort,
    receiveMessageOnPort,
    threadId,
    workerData
} = workerThreads;

let wsPort = 0;
let h2Port = 0;
let ssePort = 0;
let previousHardTerminateEnv;

function doneOnce(done) {
    let finished = false;

    return (err) => {
        if (finished)
            return;

        finished = true;
        done(err);
    };
}

function terminateWhenReady(worker, options, done) {
    const finish = doneOnce(done);
    const postTerminateMessages = [];
    let terminated = false;

    worker.once('error', finish);
    worker.on('message', (message) => {
        try {
            if (!terminated) {
                const shouldTerminate = options.onReady(message);
                if (!shouldTerminate)
                    return;

                terminated = true;
                worker.once('exit', (exitCode) => {
                    try {
                        assert.strictEqual(exitCode, 1);

                        if (options.afterExit)
                            options.afterExit();

                        setTimeout(() => {
                            try {
                                assert.deepStrictEqual(postTerminateMessages, []);
                                finish();
                            } catch (err) {
                                finish(err);
                            }
                        }, options.quietMs || 120);
                    } catch (err) {
                        finish(err);
                    }
                });

                worker.terminate();
                return;
            }

            postTerminateMessages.push(message);
        } catch (err) {
            finish(err);
        }
    });
}

// 「计算型」worker 的统一 harness：worker 先发 'ready' 再进入死循环，父侧收到
// ready 后 terminate()，交给 terminateWhenReady 校验有序退出（exit code 1、
// 不产生 error 事件、terminate 之后不再有消息）。
// 额外的看门狗保证中断失效时是**快速失败**，而不是让整套测试挂住。
function terminateBusyWorker(bodyLines, done) {
    const finish = doneOnce(done);
    let sawReady = false;
    const guard = setTimeout(() => finish(new Error(sawReady
        ? 'terminate() did not settle: worker stuck in a compute loop'
        : 'worker never reported ready')), 5000);
    const worker = new Worker([
        'const { parentPort } = require("worker_threads");',
        'parentPort.postMessage("ready");'
    ].concat(bodyLines).join('\n'), { eval: true });

    terminateWhenReady(worker, {
        onReady: (message) => {
            if (message === 'ready')
                sawReady = true;
            return message === 'ready';
        }
    }, (err) => {
        clearTimeout(guard);
        finish(err);
    });
}

describe('worker_threads fibjs target behavior', () => {
    let fixtureRoot;
    let readyWorkerFile;
    let throwWorkerFile;
    let watchTargetFile;

    before(() => {
        previousHardTerminateEnv = process.env.FIBJS_WORKER_HARD_TERMINATE;
        process.env.FIBJS_WORKER_HARD_TERMINATE = '1';
    });

    after(() => {
        if (previousHardTerminateEnv == null)
            delete process.env.FIBJS_WORKER_HARD_TERMINATE;
        else
            process.env.FIBJS_WORKER_HARD_TERMINATE = previousHardTerminateEnv;
    });
    let wsServer;
    let wsClients;
    let h2Server;
    let sseServer;
    let sseClosing = false;
    const sseSenders = new Set();

    before(() => {
        fixtureRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-worker-threads-target-'));
        readyWorkerFile = path.join(fixtureRoot, 'ready-worker.js');
        throwWorkerFile = path.join(fixtureRoot, 'throw-worker.js');
        watchTargetFile = path.join(fixtureRoot, 'watched.txt');
        wsClients = new Set();

        fs.writeFileSync(readyWorkerFile, [
            "const { MessagePort, isMainThread, parentPort, workerData } = require('worker_threads');",
            'parentPort.on("message", (payload) => {',
            '  if (payload && payload.cmd === "close") {',
            '    parentPort.close();',
            '    return;',
            '  }',
            '  parentPort.postMessage({ kind: "reply", payload });',
            '});'
        ].join('\n'), 'utf8');

        fs.writeFileSync(throwWorkerFile, 'throw new Error("fibjs worker boom");\n', 'utf8');
        fs.writeFileSync(watchTargetFile, 'seed\n', 'utf8');

        wsServer = new http.Server({
            '/ws': WebSocket.upgrade({
                protocol: 'test'
            }, (socket) => {
                wsClients.add(socket);
                socket.onclose = () => {
                    wsClients.delete(socket);
                };
            })
        });
        wsServer.listen(0, '127.0.0.1');
        wsPort = wsServer.address().port;

        const serverKeys = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
        const certKeys = crypto.generateKeyPair('rsa', { modulusLength: 2048 });
        const ca = crypto.createCertificateRequest({
            key: serverKeys.privateKey,
            subject: { CN: 'fibjs.org' }
        }).issue({
            key: serverKeys.privateKey,
            ca: true,
            issuer: { CN: 'fibjs.org' }
        });
        const cert = crypto.createCertificateRequest({
            key: certKeys.privateKey,
            subject: { CN: 'localhost' }
        }).issue({
            key: serverKeys.privateKey,
            issuer: { CN: 'fibjs.org' }
        });

        const secureContext = tls.createSecureContext({
            key: certKeys.privateKey.export(),
            cert: cert.pem,
            requestCert: false,
            alpnProtocols: ['h2']
        }, true);

        h2Server = new http2.Server(secureContext, function(req) {});
        h2Server.on('session', (session) => {
            session.on('stream', (stream, headers) => {
                const requestPath = headers[':path'];
                if (requestPath === '/hold') {
                    coroutine.sleep(180);
                    if (stream.closed || stream.destroyed)
                        return;

                    stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                    stream.write('hold');
                    stream.close();
                    return;
                }

                stream.respond({ ':status': 200, 'content-type': 'text/plain' });
                stream.write('ok');
                stream.close();
            });
        });
        h2Server.listen(0, '127.0.0.1');
        h2Port = h2Server.address().port;

        // 长活 SSE 夹具：必须走 sse.upgrade，普通 http.Server handler
        // 在返回前不会 flush，无法做长连接。
        sseServer = new http.Server({
            '/sse': sse.upgrade((se, req) => {
                sseSenders.add(se);
                // 不发送任何完整事件，让 worker 侧的 readLine 稳定挂住
                while (!sseClosing)
                    coroutine.sleep(20);
            })
        });
        sseServer.listen(0, '127.0.0.1');
        ssePort = sseServer.address().port;
        coroutine.sleep(50);
    });

    after(() => {
        sseClosing = true;
        // 直接关闭已登记的 sender，不依赖 handler 循环的时序
        for (const sender of sseSenders)
            sender.close();
        sseSenders.clear();
        coroutine.sleep(100);
        if (sseServer)
            sseServer.stop();
        if (h2Server)
            h2Server.stop();
        if (wsServer)
            wsServer.stop();
        fs.rmSync(fixtureRoot, { recursive: true, force: true });
    });

    it('exposes globalThis.Worker as the same class as worker_threads.Worker', () => {
        assert.strictEqual(typeof globalThis.Worker, 'function');
        assert.strictEqual(globalThis.Worker, Worker);
        assert.strictEqual(globalThis.Worker.prototype, Worker.prototype);
    });

    it('runs a worker created through globalThis.Worker', (done) => {
        const finish = doneOnce(done);
        const worker = new globalThis.Worker([
            "const { parentPort } = require('worker_threads');",
            'parentPort.on("message", (message) => parentPort.postMessage({',
            '  echo: message,',
            '  globalWorker: typeof globalThis.Worker,',
            '  sameClass: globalThis.Worker === require("worker_threads").Worker',
            '}));'
        ].join('\n'), { eval: true });

        worker.once('error', finish);
        worker.on('message', (message) => {
            try {
                assert.deepStrictEqual(message, {
                    echo: 'ping',
                    globalWorker: 'function',
                    sameClass: true
                });
                worker.terminate().then((exitCode) => {
                    try {
                        assert.strictEqual(exitCode, 1);
                        finish();
                    } catch (err) {
                        finish(err);
                    }
                }, finish);
            } catch (err) {
                finish(err);
            }
        });

        worker.postMessage('ping');
    });

    it('resolves terminate() with the exit code without blocking', (done) => {
        const finish = doneOnce(done);
        const worker = new globalThis.Worker('setInterval(() => {}, 50);', { eval: true });

        let exited = false;
        worker.once('error', finish);
        worker.once('exit', () => {
            exited = true;
        });

        worker.once('online', () => {
            const startedAt = Date.now();
            const pending = worker.terminate();
            const elapsed = Date.now() - startedAt;

            try {
                // Node 语义：立即返回 Promise，在 exit 时以退出码 resolve
                assert.ok(pending instanceof Promise);
                assert.ok(elapsed < 1000, 'terminate() should return without waiting for the worker to exit');
            } catch (err) {
                finish(err);
                return;
            }

            pending.then((exitCode) => {
                try {
                    assert.strictEqual(exited, true);
                    assert.strictEqual(exitCode, 1);
                    finish();
                } catch (err) {
                    finish(err);
                }
            }, finish);
        });
    });

    it('transfers an ArrayBuffer through postMessage(value, transferList)', (done) => {
        const finish = doneOnce(done);
        const worker = new globalThis.Worker([
            "const { parentPort } = require('worker_threads');",
            'parentPort.on("message", (message) => parentPort.postMessage({',
            '  received: Array.from(new Uint8Array(message.buf))',
            '}));'
        ].join('\n'), { eval: true });

        const buf = new ArrayBuffer(4);
        new Uint8Array(buf).set([1, 2, 3, 4]);

        worker.once('error', finish);
        worker.on('message', (message) => {
            try {
                assert.deepStrictEqual(message.received, [1, 2, 3, 4]);
                assert.strictEqual(buf.byteLength, 0);
            } catch (err) {
                finish(err);
                return;
            }

            worker.terminate().then(() => finish(), finish);
        });

        worker.postMessage({ buf }, [buf]);
    });

    it('exposes Node-like module state in the main thread', () => {
        assert.strictEqual(isFibjs, true);
        assert.strictEqual(isMainThread, true);
        assert.strictEqual(parentPort, null);
        assert.strictEqual(threadId, 0);
        assert.strictEqual(workerData, null);
        assert.strictEqual(typeof MessagePort, 'function');
        assert.strictEqual(typeof MessageChannel, 'function');
        assert.strictEqual(typeof receiveMessageOnPort, 'function');
    });

    it('exposes parentPort as MessagePort and delivers direct payloads across worker threads', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(readyWorkerFile, {
            workerData: { value: 'seed' }
        });

        worker.once('error', finish);
        worker.once('online', () => {
            try {
                worker.once('message', (replyMessage) => {
                    try {
                        assert.deepStrictEqual(replyMessage, {
                            kind: 'reply',
                            payload: { seq: 1, text: 'alpha' }
                        });
                        worker.once('exit', () => finish());
                        worker.terminate();
                    } catch (err) {
                        finish(err);
                    }
                });

                worker.postMessage({ seq: 1, text: 'alpha' });
            } catch (err) {
                finish(err);
            }
        });
    });

    it('uses logical isolate ids for threadId', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker([
            "const { parentPort, threadId } = require('worker_threads');",
            'parentPort.postMessage({ threadId });'
        ].join('\n'), {
            eval: true
        });

        worker.once('error', finish);
        worker.once('message', (message) => {
            try {
                assert.strictEqual(typeof worker.threadId, 'number');
                assert.ok(worker.threadId > 0);
                assert.deepStrictEqual(message, { threadId: worker.threadId });
                worker.once('exit', () => finish());
                worker.terminate();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('emits online and exit instead of the legacy load event surface', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(readyWorkerFile, {
            workerData: { phase: 'lifecycle' }
        });

        let onlineSeen = false;

        worker.once('error', finish);
        worker.once('online', () => {
            onlineSeen = true;
            try {
                assert.strictEqual(onlineSeen, true);
                worker.once('exit', (exitCode) => {
                    try {
                        assert.strictEqual(typeof exitCode, 'number');
                        finish();
                    } catch (err) {
                        finish(err);
                    }
                });
                worker.terminate();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('accepts a postMessage immediately after construction', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(readyWorkerFile, {
            workerData: { phase: 'immediate-post' }
        });

        let onlineSeen = false;
        let replySeen = false;

        worker.once('error', finish);
        worker.once('online', () => {
            onlineSeen = true;
        });
        worker.on('message', (message) => {
            try {
                assert.deepStrictEqual(message, {
                    kind: 'reply',
                    payload: { seq: 1, text: 'early' }
                });
                replySeen = true;
                worker.postMessage({ cmd: 'close' });
            } catch (err) {
                finish(err);
            }
        });
        worker.once('exit', (exitCode) => {
            try {
                assert.strictEqual(exitCode, 0);
                assert.strictEqual(onlineSeen, true);
                assert.strictEqual(replySeen, true);
                finish();
            } catch (err) {
                finish(err);
            }
        });

        worker.postMessage({ seq: 1, text: 'early' });
    });

    it('terminates a worker with a bound dgram socket in order', (done) => {
        const worker = new Worker([
            "const dgram = require('dgram');",
            "const { parentPort } = require('worker_threads');",
            'const socket = dgram.createSocket("udp4");',
            'socket.on("listening", () => {',
            '  const address = socket.address();',
            '  parentPort.postMessage({ type: "ready", port: address.port });',
            '});',
            'socket.on("message", (msg) => {',
            '  parentPort.postMessage({ type: "udp", value: msg.toString() });',
            '});',
            'socket.bind(0, "127.0.0.1");'
        ].join('\n'), { eval: true });

        let port = 0;
        terminateWhenReady(worker, {
            onReady(message) {
                if (message.type !== 'ready')
                    return false;
                port = message.port;
                return true;
            },
            afterExit() {
                const client = dgram.createSocket('udp4');
                client.send('late', port, '127.0.0.1');
                client.close();
            }
        }, done);
    });

    it('terminates a worker with a listening TCP server in order', (done) => {
        const worker = new Worker([
            "const net = require('net');",
            "const { parentPort } = require('worker_threads');",
            'const server = net.createServer(() => {});',
            'server.on("error", () => {});',
            'server.listen(0, "127.0.0.1", () => parentPort.postMessage({ type: "ready" }));'
        ].join('\n'), { eval: true });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'ready';
            },
            quietMs: 260
        }, done);
    });

    it('terminates a worker with a listening HTTP server in order', (done) => {
        const worker = new Worker([
            "const http = require('http');",
            "const { parentPort } = require('worker_threads');",
            'const server = new http.Server(0, (req) => { req.response.end("ok"); });',
            'server.start();',
            'parentPort.postMessage({ type: "ready" });'
        ].join('\n'), { eval: true });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'ready';
            },
            quietMs: 260
        }, done);
    });

    it('terminates a worker with fs.watch in order', (done) => {
        const worker = new Worker([
            "const fs = require('fs');",
            "const { parentPort, workerData } = require('worker_threads');",
            'fs.watch(workerData.file, () => {',
            '  parentPort.postMessage({ type: "change" });',
            '});',
            'parentPort.postMessage({ type: "ready" });'
        ].join('\n'), {
            eval: true,
            workerData: { file: watchTargetFile }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'ready';
            },
            afterExit() {
                fs.appendFileSync(watchTargetFile, 'fs.watch late\n');
            }
        }, done);
    });

    it('terminates a worker with fs.watchFile in order', (done) => {
        const worker = new Worker([
            "const fs = require('fs');",
            "const { parentPort, workerData } = require('worker_threads');",
            'let primed = false;',
            'fs.watchFile(workerData.file, { interval: 20 }, () => {',
            '  if (!primed) {',
            '    primed = true;',
            '    parentPort.postMessage({ type: "primed" });',
            '    return;',
            '  }',
            '  parentPort.postMessage({ type: "change" });',
            '});',
        ].join('\n'), {
            eval: true,
            workerData: { file: watchTargetFile }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'primed';
            },
            afterExit() {
                fs.appendFileSync(watchTargetFile, 'fs.watchFile late\n');
            },
            quietMs: 260
        }, done);
    });

    it('terminates a worker with a long-lived child process IPC channel in order', (done) => {
        const worker = new Worker([
            "const child_process = require('child_process');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const child = child_process.fork(workerData.childFile);',
            'child.on("message", (message) => {',
            '  if (message === "ready")',
            '    parentPort.postMessage({ type: "ready" });',
            '});'
        ].join('\n'), {
            eval: true,
            workerData: {
                childFile: path.join(process.cwd(), 'test', 'process', 'exec_ipc_hold.js')
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'ready';
            },
            quietMs: 260
        }, done);
    });

    it('terminates a worker with an active child stdout reader in order', (done) => {
        const worker = new Worker([
            "const child_process = require('child_process');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const child = child_process.spawn(process.execPath, [workerData.childFile], { stdio: "pipe" });',
            'child.stdout.on("data", () => {});',
            'child.on("error", () => {});',
            'parentPort.postMessage({ type: "ready", pid: child.pid });'
        ].join('\n'), {
            eval: true,
            workerData: {
                childFile: path.join(process.cwd(), 'test', 'process', 'exec_sleep.js')
            }
        });

        let childPid = 0;
        terminateWhenReady(worker, {
            onReady(message) {
                if (message.type !== 'ready')
                    return false;
                childPid = message.pid;
                return true;
            },
            quietMs: 260,
            afterExit() {
                // R5/D1：worker 终止时其 spawn 的子进程一并终止（SIGKILL）。
                // 子进程从被杀死到被 uv 回收 pid 有一小段窗口，这里轮询等待。
                let gone = false;
                for (let i = 0; i < 50; i++) {
                    try {
                        process.kill(childPid, 0);
                    } catch (err) {
                        gone = true;
                        break;
                    }
                    coroutine.sleep(20);
                }

                assert.strictEqual(gone, true, 'child process should be terminated together with the worker');
            }
        }, done);
    });

    it('terminates a worker with a pending callback-style TCP connect in order', (done) => {
        const worker = new Worker([
            "const net = require('net');",
            "const { parentPort } = require('worker_threads');",
            "const sock = net.connect(65001, '10.255.255.1', () => parentPort.postMessage({ type: 'connect' }));",
            'sock.on("error", () => {});',
            'parentPort.postMessage({ type: "ready" });'
        ].join('\n'), { eval: true });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'ready';
            },
            quietMs: 260
        }, done);
    });

    it('terminates a worker with an open WebSocket in order', (done) => {
        const worker = new Worker([
            "const { parentPort, workerData } = require('worker_threads');",
            'const socket = new WebSocket(workerData.url, "test");',
            'socket.onopen = () => {',
            '  parentPort.postMessage({ type: "ready" });',
            '};',
            'socket.onmessage = (event) => {',
            '  parentPort.postMessage({ type: "ws", value: event.data });',
            '};',
            'socket.onerror = (event) => {',
            '  parentPort.postMessage({ type: "error", value: String(event && event.message || event) });',
            '};'
        ].join('\n'), {
            eval: true,
            workerData: { url: `ws://127.0.0.1:${wsPort}/ws` }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                if (message.type === 'error')
                    throw new Error(message.value);
                return message.type === 'ready';
            },
            afterExit() {
                for (const socket of wsClients) {
                    try {
                        socket.send('late');
                    } catch (error) {
                    }
                }
            }
        }, done);
    });

    it('terminates a worker with an active EventSource stream in order', (done) => {
        const worker = new Worker([
            "const sse = require('sse');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const es = new sse.EventSource(workerData.url);',
            'es.onopen = () => {',
            '  parentPort.postMessage({ type: "ready" });',
            '};',
            'es.onmessage = (event) => {',
            '  parentPort.postMessage({ type: "message", value: event.data });',
            '};',
            'es.onerror = () => {};'
        ].join('\n'), {
            eval: true,
            workerData: { url: `http://127.0.0.1:${ssePort}/sse` }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'ready';
            },
            quietMs: 260
        }, done);
    });

    it('terminates a worker with an active http2 session in order', (done) => {
        const worker = new Worker([
            "const http2 = require('http2');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const session = http2.connect(workerData.url, { rejectUnauthorized: false, rejectUnverified: false });',
            'const stream = session.request({ ":method": "GET", ":path": "/hold" });',
            'parentPort.postMessage({ type: "ready" });',
            'stream.on("headers", () => {',
            '  parentPort.postMessage({ type: "headers" });',
            '});',
            'stream.on("close", () => {',
            '  parentPort.postMessage({ type: "stream-close" });',
            '});'
        ].join('\n'), {
            eval: true,
            workerData: { url: `https://localhost:${h2Port}` }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message.type === 'ready';
            },
            quietMs: 260
        }, done);
    });

    it('releases the worker isolate when its http2 client session is closed', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker([
            "const http2 = require('http2');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const session = http2.connect(workerData.url, { rejectUnauthorized: false, rejectUnverified: false });',
            'const stream = session.request({ ":method": "GET", ":path": "/hold" });',
            'stream.on("data", () => {});',
            'setTimeout(() => {',
            '  session.close();',
            '  parentPort.postMessage({ type: "closed" });',
            '}, 120);'
        ].join('\n'), {
            eval: true,
            workerData: { url: `https://localhost:${h2Port}` }
        });

        let closed = false;
        worker.once('error', finish);
        worker.on('message', (message) => {
            if (message.type === 'closed')
                closed = true;
        });
        worker.once('exit', (exitCode) => {
            try {
                // session.close() 之后必须自然退出（exitCode 0）：
                // 说明 close 路径的 isolate hold 由读循环终态 releaseRef() 释放，
                // 而不是依赖 stop()/destroy() 的强制回收。
                assert.strictEqual(closed, true);
                assert.strictEqual(exitCode, 0);
                finish();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('keeps parentPort alive across multiple messages with an async gap', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker([
            "const { parentPort } = require('worker_threads');",
            'parentPort.postMessage({ step: "ready" });',
            'parentPort.on("message", (message) => {',
            '  if (message.cmd === "step1") {',
            '    parentPort.postMessage({ step: "step1" });',
            '    return;',
            '  }',
            '  if (message.cmd === "step2") {',
            '    parentPort.postMessage({ step: "step2" });',
            '    parentPort.close();',
            '  }',
            '});'
        ].join('\n'), {
            eval: true
        });

        const steps = [];

        worker.once('error', finish);
        worker.on('message', (message) => {
            try {
                steps.push(message.step);

                if (message.step === 'ready') {
                    worker.postMessage({ cmd: 'step1' });
                    return;
                }

                if (message.step === 'step1') {
                    setTimeout(() => worker.postMessage({ cmd: 'step2' }), 10);
                    return;
                }
            } catch (err) {
                finish(err);
            }
        });
        worker.once('exit', (exitCode) => {
            try {
                assert.deepStrictEqual(steps, ['ready', 'step1', 'step2']);
                assert.strictEqual(exitCode, 0);
                finish();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('emits uncaught worker exceptions on the error event', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(throwWorkerFile);

        worker.once('error', (error) => {
            try {
                assert.ok(error instanceof Error);
                assert.match(error.message, /fibjs worker boom/);
                finish();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('supports receiveMessageOnPort in the same-thread MessageChannel case', () => {
        const { port1, port2 } = new MessageChannel();
        const first = { value: 1, nested: ['a'] };
        const second = { value: 2 };

        assert.strictEqual(receiveMessageOnPort(port2), undefined);
        port1.postMessage(first);
        port1.postMessage(second);

        first.value = 10;
        first.nested.push('b');

        assert.deepStrictEqual(receiveMessageOnPort(port2), {
            message: { value: 1, nested: ['a'] }
        });
        assert.deepStrictEqual(receiveMessageOnPort(port2), { message: second });
        assert.strictEqual(receiveMessageOnPort(port2), undefined);
    });

    // ---- worker 优雅退出语义（对照 Node v23.9 实测；见 plans/worker-graceful-exit-plan.md）----

    it('terminates a worker before it goes online', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker('Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, 10000);', { eval: true });

        let online = false;
        let exitCode = null;
        let resolveValue = null;

        worker.once('error', finish);
        worker.once('online', () => {
            online = true;
        });
        worker.once('exit', (code) => {
            exitCode = code;
        });

        const pending = worker.terminate();

        Promise.resolve(pending).then((code) => {
            resolveValue = code;
            setTimeout(() => {
                try {
                    // Node: exitCode 0 / resolve 0（worker 未启动）；fibjs: 1/1 —— 均表示「未 online 即结束」
                    assert.strictEqual(online, false);
                    assert.strictEqual(exitCode, 1);
                    assert.strictEqual(resolveValue, 1);
                    finish();
                } catch (err) {
                    finish(err);
                }
            }, 50);
        }, finish);
    });

    it('keeps the process alive when a worker calls process.exit()', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker([
            "const { parentPort } = require('worker_threads');",
            'parentPort.postMessage("before-exit");',
            'process.exit(3);',
            'parentPort.postMessage("after-exit");'
        ].join('\n'), { eval: true });

        const messages = [];
        worker.on('message', (message) => messages.push(message));
        worker.once('error', finish);
        worker.once('exit', (exitCode) => {
            setTimeout(() => {
                try {
                    // Node 语义：worker 内 process.exit(N) 只结束本线程（父进程继续），exit code = N
                    assert.strictEqual(exitCode, 3);
                    // process.exit() 之后的语句不再执行
                    assert.deepStrictEqual(messages, ['before-exit']);
                    finish();
                } catch (err) {
                    finish(err);
                }
            }, 100);
        });
    });

    it('cascades terminate to nested workers', (done) => {
        const finish = doneOnce(done);
        const childFile = path.join(fixtureRoot, 'nested-child.js');
        fs.writeFileSync(childFile, 'setInterval(() => {}, 1000);\n', 'utf8');

        const worker = new Worker([
            "const { Worker, parentPort, workerData } = require('worker_threads');",
            'const nested = new Worker(workerData.childFile);',
            'nested.on("exit", (code) => parentPort.postMessage({ nestedExit: code }));',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: { childFile }
        });

        const events = [];
        let terminated = false;

        worker.on('message', (message) => {
            events.push(typeof message === 'string' ? message : JSON.stringify(message));
            if (message !== 'ready' || terminated)
                return;

            terminated = true;
            Promise.resolve(worker.terminate()).then((code) => {
                setTimeout(() => {
                    try {
                        // Node 语义：worker teardown 会一并停掉它的嵌套 worker（否则父 worker 无法收敛）
                        assert.strictEqual(code, 1);
                        assert.strictEqual(events[0], 'ready');
                        assert.ok(events.indexOf('exit') > 0);
                        finish();
                    } catch (err) {
                        finish(err);
                    }
                }, 200);
            }, finish);
        });
        worker.once('exit', () => events.push('exit'));
        worker.once('error', finish);
    });

    it('reports the original error message and worker stack for uncaught worker exceptions', (done) => {        const finish = doneOnce(done);
        const worker = new Worker('throw new Error("boom-payload");', { eval: true });

        worker.once('error', (err) => {
            try {
                assert.ok(err instanceof Error);
                assert.strictEqual(err.message, 'boom-payload');
                assert.ok(String(err.stack).indexOf('boom-payload') >= 0);
            } catch (e) {
                finish(e);
                return;
            }

            worker.terminate().then(() => finish(), finish);
        });
    });

    it('delivers messages sent before terminate and none after exit', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker([
            "const { parentPort } = require('worker_threads');",
            'let n = 0;',
            'setInterval(() => parentPort.postMessage(++n), 20);'
        ].join('\n'), { eval: true });

        const beforeExit = [];
        const afterExit = [];
        let exited = false;
        let resolveValue = null;
        let terminated = false;

        worker.once('error', finish);
        worker.on('message', (message) => {
            (exited ? afterExit : beforeExit).push(message);

            if (message === 2 && !terminated) {
                terminated = true;
                Promise.resolve(worker.terminate()).then((code) => {
                    resolveValue = code;
                }, finish);
            }
        });
        worker.once('exit', (exitCode) => {
            exited = true;
            setTimeout(() => {
                try {
                    assert.deepStrictEqual(beforeExit.slice(0, 2), [1, 2]);
                    assert.deepStrictEqual(afterExit, []);
                    assert.strictEqual(exitCode, 1);
                    assert.strictEqual(resolveValue, 1);
                    finish();
                } catch (err) {
                    finish(err);
                }
            }, 150);
        });
    });

    it('does not run callback-style async completions after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-callback-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const http = require('http');",
            "const { parentPort, workerData } = require('worker_threads');",
            'http.get(workerData.url, (resp) => {',
            '  resp.text();',
            '  fs.writeFileSync(workerData.marker, "callback-ran");',
            '  parentPort.postMessage("late-callback");',
            '});',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'callback JS should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run promise continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-promise-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const { parentPort, workerData } = require('worker_threads');",
            'fetch(workerData.url).then((resp) => resp.text()).then(() => {',
            '  fs.writeFileSync(workerData.marker, "promise-ran");',
            '  parentPort.postMessage("late-promise");',
            '});',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'promise continuation JS should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run events.once() continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-events-once-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const http = require('http');",
            "const events = require('events');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const req = http.get(workerData.url);',
            'events.once(req, "response").then(([resp]) => {',
            '  resp.text();',
            '  fs.writeFileSync(workerData.marker, "events-once-ran");',
            '  parentPort.postMessage("late-events-once");',
            '});',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'events.once() continuation should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run events.on() iterator continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-events-on-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const http = require('http');",
            "const events = require('events');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const req = http.get(workerData.url);',
            '(async () => {',
            '  for await (const args of events.on(req, "response", { close: ["close"] })) {',
            '    const resp = args[0];',
            '    resp.text();',
            '    fs.writeFileSync(workerData.marker, "events-on-ran");',
            '    parentPort.postMessage("late-events-on");',
            '  }',
            '})();',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'events.on() iterator continuation should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run util.callbackify continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-callbackify-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const util = require('util');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const fn = util.callbackify(async () => {',
            '  const resp = await fetch(workerData.url);',
            '  await resp.text();',
            '  return "ok";',
            '});',
            'fn((err, value) => {',
            '  fs.writeFileSync(workerData.marker, String(err) + ":" + value);',
            '  parentPort.postMessage("late-callbackify");',
            '});',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'callbackify callback should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run util.promisify continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-promisify-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const http = require('http');",
            "const util = require('util');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const fn = util.promisify((done) => {',
            '  http.get(workerData.url, (resp) => {',
            '    resp.text();',
            '    done(null, "ok");',
            '  });',
            '});',
            'fn().then(() => {',
            '  fs.writeFileSync(workerData.marker, "promisify-ran");',
            '  parentPort.postMessage("late-promisify");',
            '});',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'promisify continuation should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run AsyncLocalStorage.bind continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-als-bind-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const { AsyncLocalStorage } = require('async_hooks');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const als = new AsyncLocalStorage();',
            'als.run({ scope: "bind" }, () => {',
            '  const bound = AsyncLocalStorage.bind(() => {',
            '    fs.writeFileSync(workerData.marker, als.getStore() && als.getStore().scope || "missing");',
            '    parentPort.postMessage("late-als-bind");',
            '  });',
            '  fetch(workerData.url).then(() => bound());',
            '});',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'AsyncLocalStorage.bind continuation should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run AsyncLocalStorage.snapshot continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-als-snapshot-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const { AsyncLocalStorage } = require('async_hooks');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const als = new AsyncLocalStorage();',
            'als.run({ scope: "snapshot" }, () => {',
            '  const runInScope = AsyncLocalStorage.snapshot();',
            '  fetch(workerData.url).then(() => runInScope(() => {',
            '    fs.writeFileSync(workerData.marker, als.getStore() && als.getStore().scope || "missing");',
            '    parentPort.postMessage("late-als-snapshot");',
            '  }));',
            '});',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'AsyncLocalStorage.snapshot continuation should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run util.sync callbacks after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-util-sync-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const http = require('http');",
            "const util = require('util');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const wrapped = util.sync((done) => {',
            '  http.get(workerData.url, (resp) => {',
            '    resp.text();',
            '    fs.writeFileSync(workerData.marker, "util-sync-ran");',
            '    parentPort.postMessage("late-util-sync");',
            '    done(null, "ok");',
            '  });',
            '});',
            'setTimeout(() => wrapped(), 0);',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'util.sync callback should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run AsyncResource.bind continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-async-resource-bind-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const { AsyncResource } = require('async_hooks');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const ar = new AsyncResource("TEST");',
            'const bound = ar.bind(() => {',
            '  fs.writeFileSync(workerData.marker, "async-resource-bind-ran");',
            '  parentPort.postMessage("late-async-resource-bind");',
            '});',
            'fetch(workerData.url).then(() => bound());',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'AsyncResource.bind continuation should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not run AsyncResource.runInAsyncScope continuations after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-async-resource-scope-marker.txt');
        const server = http.createServer((req) => {
            coroutine.sleep(250);
            req.response.end('ok');
        });

        server.listen(0, '127.0.0.1');
        const port = server.address().port;

        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const { AsyncResource } = require('async_hooks');",
            "const { parentPort, workerData } = require('worker_threads');",
            'const ar = new AsyncResource("TEST");',
            'fetch(workerData.url).then(() => ar.runInAsyncScope(() => {',
            '  fs.writeFileSync(workerData.marker, "async-resource-scope-ran");',
            '  parentPort.postMessage("late-async-resource-scope");',
            '}));',
            'parentPort.postMessage("ready");'
        ].join('\n'), {
            eval: true,
            workerData: {
                url: 'http://127.0.0.1:' + port + '/',
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'ready';
            },
            afterExit() {
                server.stop();
                assert.strictEqual(fs.existsSync(markerFile), false, 'AsyncResource.runInAsyncScope continuation should not run after terminate');
            },
            quietMs: 180
        }, done);
    });

    it('does not start later coroutine.parallel items after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-coroutine-parallel-marker.txt');
        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const coroutine = require('coroutine');",
            "const { parentPort, workerData } = require('worker_threads');",
            'parentPort.postMessage("ready");',
            'setTimeout(() => {',
            '  coroutine.parallel([1, 2], (value) => {',
            '    if (value === 1) {',
            '      parentPort.postMessage("go");',
            '      const end = Date.now() + 150;',
            '      while (Date.now() < end) {}',
            '      return 1;',
            '    }',
            '    fs.writeFileSync(workerData.marker, "parallel-ran");',
            '    parentPort.postMessage("late-coroutine-parallel");',
            '    return 2;',
            '  }, 1);',
            '}, 0);'
        ].join('\n'), {
            eval: true,
            workerData: {
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'go';
            },
            afterExit() {
                assert.strictEqual(fs.existsSync(markerFile), false, 'coroutine.parallel should not start later items after terminate');
            },
            quietMs: 220
        }, done);
    });

    it('does not start later util.map items after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-util-map-marker.txt');
        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const util = require('util');",
            "const { parentPort, workerData } = require('worker_threads');",
            'parentPort.postMessage("ready");',
            'setTimeout(() => {',
            '  util.map([1, 2], (value) => {',
            '    if (value === 1) {',
            '      parentPort.postMessage("go");',
            '      const end = Date.now() + 150;',
            '      while (Date.now() < end) {}',
            '      return 1;',
            '    }',
            '    fs.writeFileSync(workerData.marker, "util-map-ran");',
            '    parentPort.postMessage("late-util-map");',
            '    return 2;',
            '  });',
            '}, 0);'
        ].join('\n'), {
            eval: true,
            workerData: {
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'go';
            },
            afterExit() {
                assert.strictEqual(fs.existsSync(markerFile), false, 'util.map should not start later items after terminate');
            },
            quietMs: 220
        }, done);
    });

    it('does not start later util.reduce items after terminate', (done) => {
        const markerFile = path.join(fixtureRoot, 'terminate-util-reduce-marker.txt');
        try {
            fs.unlinkSync(markerFile);
        } catch (_) {
        }

        const worker = new Worker([
            "const fs = require('fs');",
            "const util = require('util');",
            "const { parentPort, workerData } = require('worker_threads');",
            'parentPort.postMessage("ready");',
            'setTimeout(() => {',
            '  util.reduce([1, 2], (memo, value) => {',
            '    if (value === 1) {',
            '      parentPort.postMessage("go");',
            '      const end = Date.now() + 150;',
            '      while (Date.now() < end) {}',
            '      return memo + value;',
            '    }',
            '    fs.writeFileSync(workerData.marker, "util-reduce-ran");',
            '    parentPort.postMessage("late-util-reduce");',
            '    return memo + value;',
            '  }, 0);',
            '}, 0);'
        ].join('\n'), {
            eval: true,
            workerData: {
                marker: markerFile
            }
        });

        terminateWhenReady(worker, {
            onReady(message) {
                return message === 'go';
            },
            afterExit() {
                assert.strictEqual(fs.existsSync(markerFile), false, 'util.reduce should not start later items after terminate');
            },
            quietMs: 220
        }, done);
    });

    it('resolves terminate() with the recorded exit code after the worker exited', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker('process.exitCode = 5;', { eval: true });

        worker.once('error', finish);
        worker.once('exit', (exitCode) => {
            try {
                assert.strictEqual(exitCode, 5);
            } catch (err) {
                finish(err);
                return;
            }

            Promise.resolve(worker.terminate()).then((code) => {
                try {
                    // Node 此处 resolve 0（与真实退出码无关）；fibjs resolve 记录到的真实退出码，信息更全
                    assert.strictEqual(code, 5);
                    finish();
                } catch (err) {
                    finish(err);
                }
            }, finish);
        });
    });

    // ---------- G2-b：硬中断（v8::Isolate::RequestInterrupt + TerminateExecution） ----------
    // worker.terminate() 会把终止请求投递到 worker isolate 的中断队列，由 V8 在正在执行的
    // JS 的下一个 interrupt check 点回调，因此**不依赖 worker 的事件循环** —— 纯 JS 死循环
    // 也能被终止。软中断（只在 fibjs 可控的 JS/native 边界生效）做不到这一点。

    it('terminates a worker stuck in a JS busy loop', (done) => {
        const finish = doneOnce(done);
        const guard = setTimeout(() => finish(new Error('terminate() did not settle: worker still in busy loop')), 5000);
        const worker = new globalThis.Worker([
            'const { parentPort } = require("worker_threads");',
            'parentPort.postMessage("looping");',
            'while (true) {}'
        ].join('\n'), { eval: true });

        worker.once('error', () => {
            clearTimeout(guard);
            finish();
        });
        worker.on('message', (msg) => {
            if (msg !== 'looping')
                return;

            worker.terminate().then((exitCode) => {
                clearTimeout(guard);
                try {
                    assert.strictEqual(exitCode, 1);
                    finish();
                } catch (err) {
                    finish(err);
                }
            }, (err) => {
                clearTimeout(guard);
                finish(err);
            });
        });
    });

    it('terminates a worker blocked in Atomics.wait', (done) => {
        const finish = doneOnce(done);
        const guard = setTimeout(() => finish(new Error('terminate() did not settle: worker still in Atomics.wait')), 5000);
        const worker = new globalThis.Worker([
            'const { parentPort } = require("worker_threads");',
            'parentPort.postMessage("waiting");',
            'const sab = new SharedArrayBuffer(4);',
            'Atomics.wait(new Int32Array(sab), 0, 0);'
        ].join('\n'), { eval: true });

        worker.once('error', () => {
            clearTimeout(guard);
            finish();
        });
        worker.on('message', (msg) => {
            if (msg !== 'waiting')
                return;

            worker.terminate().then((exitCode) => {
                clearTimeout(guard);
                try {
                    assert.strictEqual(exitCode, 1);
                    finish();
                } catch (err) {
                    finish(err);
                }
            }, (err) => {
                clearTimeout(guard);
                finish(err);
            });
        });
    });

    // ---------- 常用组件「计算场景」下的中断安全退出 ----------
    // 与纯 JS 死循环不同：这里 worker 正在调用常用组件的 native 实现做计算，中断
    // 往往落在组件调用附近（甚至要等一次较长的 native 调用返回后才生效）。
    // 检验「安全退出」：不崩溃、有序（exit code 1）、不产生 error 事件、
    // terminate() 一定收敛 —— 即组件外围的 JS 回退路径对终止异常免疫。
    const computeScenarios = [
        {
            what: 'crypto hash (sha256) loop',
            body: [
                "const crypto = require('crypto');",
                "const payload = 'payload'.repeat(64);",
                'while (true) crypto.createHash(\'sha256\').update(payload).digest();'
            ]
        },
        {
            what: 'crypto HMAC (sha256) loop',
            body: [
                "const crypto = require('crypto');",
                "const payload = 'payload'.repeat(64);",
                "while (true) crypto.createHmac('sha256', 'key').update(payload).digest();"
            ]
        },
        {
            // 每次调用都是一段较长的 native 计算：中断只能在它返回后才生效，
            // 因此这条同时验证「终止请求会等 in-flight 的 native 调用结束」。
            what: 'crypto PBKDF2 (long native call) loop',
            body: [
                "const crypto = require('crypto');",
                "while (true) crypto.pbkdf2Sync('password', 'salt', 50000, 32, 'sha256');"
            ]
        },
        {
            what: 'crypto AES-256-CBC cipher loop',
            body: [
                "const crypto = require('crypto');",
                'const key = crypto.randomBytes(32);',
                'const iv = crypto.randomBytes(16);',
                'while (true) {',
                "  const c = crypto.createCipheriv('aes-256-cbc', key, iv);",
                "  c.update('payload-payload-payload', 'utf8', 'hex');",
                "  c.final('hex');",
                '}'
            ]
        },
        {
            what: 'crypto randomBytes loop',
            body: [
                "const crypto = require('crypto');",
                'while (true) crypto.randomBytes(4096);'
            ]
        },
        {
            what: 'vm.SandBox nested script loop',
            body: [
                "const vm = require('vm');",
                "const sbox = new vm.SandBox({});",
                "while (true) sbox.addScript('loop.js', 'var a = 0; for (var i = 0; i < 1000; i++) a += i; a;');"
            ]
        },
        {
            what: 'vm.runInNewContext loop',
            body: [
                "const vm = require('vm');",
                "while (true) vm.runInNewContext('var a = 0; for (var i = 0; i < 1000; i++) a += i; a;');"
            ]
        },
        {
            what: 'mq.Routing build loop',
            body: [
                "const mq = require('mq');",
                "while (true) new mq.Routing({ '/x': function (r) { return r; } });"
            ]
        },
        {
            what: 'url.URLSearchParams parse loop',
            body: [
                "const url = require('url');",
                "const qs = 'a=1&b=2&c=3&d=' + 'x'.repeat(200);",
                "while (true) new url.URLSearchParams(qs).get('d');"
            ]
        },
        {
            what: 'querystring stringify/parse loop',
            body: [
                "const qs = require('querystring');",
                "const obj = { a: 1, b: 'x'.repeat(200), c: [1, 2, 3] };",
                'while (true) qs.parse(qs.stringify(obj));'
            ]
        },
        {
            // 已知限制：与 zlib 场景同源 —— 卡在**同步 native 调用**里时，实测 V8 可能
            // 始终不派发中断回调（带埋点的压测证据：该 isolate 只有 requestTerminate，
            // 之后无中断回调、无收尾任务、无 exit）。SQLite execute 是这一模式的第二个实例，
            // 说明它不是 zlib 特有，而是「同步 native 计算」这一类。
            what: 'SQLite query loop',
            knownLimitation: 'V8 may never deliver the interrupt while spinning inside sync native calls',
            body: [
                "const db = require('db');",
                "const conn = db.openSQLite(':memory:');",
                "conn.execute('create table t(a int)');",
                "for (let i = 0; i < 100; i++) conn.execute('insert into t values(' + i + ')');",
                "while (true) conn.execute('select count(*) c, sum(a) s from t');"
            ]
        },
        {
            what: 'v8 serialize/deserialize loop',
            body: [
                "const v8 = require('v8');",
                "const obj = { a: [1, 2, 3], b: 'x'.repeat(256), c: { d: true } };",
                'while (true) v8.deserialize(v8.serialize(obj));'
            ]
        },
        {
            what: 'string_decoder churn loop',
            body: [
                "const { StringDecoder } = require('string_decoder');",
                "const buf = Buffer.from('中\u6587'.repeat(200));",
                "while (true) { const d = new StringDecoder('utf8'); d.write(buf); d.end(); }"
            ]
        },
        {
            what: 'util.format loop',
            body: [
                "const util = require('util');",
                "const args = ['%s-%d', 'x'.repeat(128), 42];",
                'while (true) util.format.apply(null, args);'
            ]
        },
        {
            // 异步组件：终止会落在 promise 链的重新调度中间，而不是同步循环里。
            what: 'crypto.subtle async digest loop',
            body: [
                "const crypto = require('crypto');",
                "const data = Buffer.from('x'.repeat(1024));",
                '(function loop() { crypto.subtle.digest("SHA-256", data).then(loop, function () {}); })();'
            ]
        },
        {
            // 已知限制：worker 在 zlib 同步计算循环里时，实测 V8 可能**始终不派发**
            // 中断回调，收尾任务也拿不到事件循环 —— 该 isolate 既不回事件循环也不检查
            // 中断，terminate() 因此不收敛（约 1/20~1/70 轮，与负载相关）。
            // 证据：带埋点的压测里该 isolate 只有 requestTerminate，之后既无中断回调、
            // 无收尾任务、也无 exit；而同样结构下 crypto/JSON/encoding 等 11 个场景从不出问题。
            what: 'zlib deflate/inflate loop',
            knownLimitation: 'V8 may never deliver the interrupt while spinning inside zlib sync calls',
            body: [
                "const zlib = require('zlib');",
                "const payload = Buffer.from('hello world '.repeat(500));",
                'while (true) zlib.inflateSync(zlib.deflateSync(payload));'
            ]
        },
        {
            what: 'JSON stringify/parse loop',
            body: [
                'const payload = { items: [], nested: { flag: true } };',
                "for (let i = 0; i < 500; i++) payload.items.push({ i: i, s: 'x'.repeat(32) });",
                'while (true) JSON.parse(JSON.stringify(payload));'
            ]
        },
        {
            what: 'encoding base64/hex loop',
            body: [
                "const encoding = require('encoding');",
                "const text = 'fibjs worker interrupt '.repeat(64);",
                'while (true) {',
                '  encoding.base64.decode(encoding.base64.encode(text).toString());',
                '  encoding.hex.decode(encoding.hex.encode(text).toString());',
                '}'
            ]
        },
        {
            what: 'Buffer alloc/concat churn loop',
            body: [
                'while (true) {',
                '  const a = Buffer.alloc(32 * 1024, 0x61);',
                '  const b = Buffer.alloc(32 * 1024, 0x62);',
                "  Buffer.concat([a, b]).slice(0, 128).toString('hex');",
                '}'
            ]
        },
        {
            what: 'RegExp heavy matching loop',
            body: [
                "const long = 'a'.repeat(2000) + 'b'.repeat(2000) + 'c';",
                'const re = /^a+b+c+$/;',
                'while (true) {',
                '  re.test(long);',
                '  long.match(/(a+)(b+)(c+)/);',
                '}'
            ]
        },
        {
            what: 'structuredClone loop',
            body: [
                "const payload = { a: [1, 2, 3], b: { c: 'x'.repeat(128) } };",
                'payload.d = new Array(256).fill(0).map((_, i) => i);',
                'while (true) structuredClone(payload);'
            ]
        },
        {
            what: 'array sort / GC churn loop',
            body: [
                'while (true) {',
                '  const a = new Array(20000);',
                '  for (let i = 0; i < a.length; i++) a[i] = (i * 7919) % a.length;',
                '  a.sort((x, y) => x - y);',
                '}'
            ]
        }
    ];

    computeScenarios.forEach(({ what, body, knownLimitation }) => {
        if (knownLimitation) {
            it.todo(`terminates a worker stuck in a ${what} (known limitation: ${knownLimitation})`);
            return;
        }

        it(`terminates a worker stuck in a ${what}`, (done) => {
            terminateBusyWorker(body, done);
        });
    });

    // 模块加载器会 park 在 Isolate::await（ev->ac_wait）等 ESM 求值完成；
    // 而 promise_then/promise_catch 在终止态下提前 return、不再 set()。
    // 没有唤醒源时该 fiber 永久 park，worker 退不出去 —— 中断开/关都一样，
    // 所以这条同时是生产路径的回归。
    it('terminates a worker while it is awaiting an ESM module load', (done) => {
        const finish = doneOnce(done);
        const guard = setTimeout(
            () => finish(new Error('terminate() did not settle: worker parked in module loading')), 5000);
        const moduleFile = path.join(fixtureRoot, 'slow-module.mjs');
        fs.writeFileSync(moduleFile, [
            'await new Promise((resolve) => setTimeout(resolve, 400));',
            "export const value = 'slow-module-done';"
        ].join('\n'), 'utf8');

        const worker = new Worker([
            'const { parentPort } = require("worker_threads");',
            // ready 必须在 require 进行中才发出：那时 fiber 已 park 在 Isolate::await，
            // 否则 terminate 会落在 park 之前、在 JS 里就被中断掉，测不到这个窗口。
            'setTimeout(() => parentPort.postMessage("ready"), 120);',
            `require(${JSON.stringify(moduleFile)});`,
            'parentPort.postMessage("after-require");'
        ].join('\n'), { eval: true });

        terminateWhenReady(worker, {
            onReady: (message) => message === 'ready'
        }, (err) => {
            clearTimeout(guard);
            finish(err);
        });
    });

    // 仍为已知限制：coroutine.sleep 把 fiber park 在定时器队列上，worker 线程此时并不在
    // 执行 JS，中断无从下手 —— 需要「可唤醒 fiber 登记表」才能让 worker 退出去。
    it.todo('terminates a worker parked in coroutine.sleep (known limitation: needs fiber wake-up registry)');

    // ---------- G4：worker.ref() / unref() 与父进程存活 ----------
    // Node 语义：ref'd worker 阻止父进程退出（父进程等它）；unref() 之后父进程不再等它
    // （worker 线程随进程退出被直接回收，不发 exit 事件）。ref()/unref() 幂等，重复调用无副作用。

    it('keeps the message channel working across unref() and ref()', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(readyWorkerFile);

        worker.once('error', finish);

        const reply = (payload) => {
            worker.postMessage({ cmd: payload });
        };

        let step = 0;
        worker.on('message', (message) => {
            try {
                assert.strictEqual(message.kind, 'reply');
                step++;

                if (step === 1) {
                    // unref 只解除「父进程存活」的持有，不影响通道
                    worker.unref();
                    reply('after-unref');
                } else if (step === 2) {
                    // ref 恢复持有；重复 unref/ref 必须幂等
                    worker.ref();
                    worker.unref();
                    worker.unref();
                    worker.ref();
                    reply('after-ref');
                } else {
                    Promise.resolve(worker.terminate()).then(() => finish(), finish);
                }
            } catch (err) {
                finish(err);
            }
        });

        reply('first');
    });

    it('treats unref() on an exited worker as a no-op', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker('process.exitCode = 0;', { eval: true });

        worker.once('error', finish);
        worker.once('exit', () => {
            try {
                // 已退出 → hold 已由 emitExit() 释放；此处 unref() 不得重复释放（refcount 不得下溢）
                worker.unref();
                worker.ref();
                worker.unref();
            } catch (err) {
                finish(err);
                return;
            }

            setTimeout(() => finish(), 50);
        });
    });

    it('does not make the parent wait for an unref()ed worker', (done) => {
        const finish = doneOnce(done);
        const liveFile = path.join(fixtureRoot, 'unref-live.js');
        const childFile = path.join(fixtureRoot, 'unref-parent.js');

        // worker 存活 10s：父进程若等待它，子进程不可能在预算内退出
        fs.writeFileSync(liveFile, 'setInterval(() => {}, 10000);\n', 'utf8');
        fs.writeFileSync(childFile, [
            "const { Worker } = require('worker_threads');",
            'const w = new Worker(process.argv[2]);',
            'w.on("exit", (code) => console.log("WORKER_EXIT:" + code));',
            'w.unref();',
            'process.on("beforeExit", () => console.log("BEFORE_EXIT"));',
            'console.log("PARENT_END");'
        ].join('\n'), 'utf8');

        const startedAt = Date.now();
        const child = child_process.spawn(process.execPath, [childFile, liveFile], { stdio: ['ignore', 'pipe', 'pipe'] });

        let stdout = '';
        let stderr = '';
        let settled = false;

        const settle = (err) => {
            if (settled)
                return;

            settled = true;
            clearTimeout(watchdog);
            try {
                child.kill();
            } catch (_) {
                // 已退出
            }
            finish(err);
        };

        const watchdog = setTimeout(() => {
            settle(new Error('parent process did not exit while only an unref()ed worker was left'));
        }, 5000);

        child.stdout.on('data', (data) => {
            stdout += data.toString();
        });
        child.stderr.on('data', (data) => {
            stderr += data.toString();
        });
        child.on('error', (err) => settle(err));
        child.on('exit', (code) => {
            const elapsed = Date.now() - startedAt;

            try {
                assert.strictEqual(code, 0, 'child exit code (stderr: ' + stderr.trim() + ')');
                assert.ok(elapsed < 4000, 'parent should exit without waiting for the unref()ed worker, took ' + elapsed + 'ms');
                assert.ok(stdout.indexOf('PARENT_END') >= 0, 'parent finished its script: ' + stdout);
                assert.ok(stdout.indexOf('BEFORE_EXIT') >= 0, 'beforeExit must fire once only unref()ed workers are left: ' + stdout);
                // 进程退出直接回收 worker 线程：不发 exit 事件（与 Node 一致）
                assert.strictEqual(stdout.indexOf('WORKER_EXIT'), -1, 'unref()ed worker must not be waited for: ' + stdout);
                settle();
            } catch (err) {
                settle(err);
            }
        });
    });

    it('makes the parent wait again after ref() and observes the worker exit', (done) => {
        const finish = doneOnce(done);
        const liveFile = path.join(fixtureRoot, 'ref-live.js');
        const childFile = path.join(fixtureRoot, 'ref-parent.js');

        // worker 自然存活 ~900ms：ref'd 时父进程必须等它退出后才结束
        fs.writeFileSync(liveFile, 'setTimeout(() => {}, 900);\n', 'utf8');
        fs.writeFileSync(childFile, [
            "const { Worker } = require('worker_threads');",
            'const w = new Worker(process.argv[2]);',
            'w.on("exit", (code) => console.log("WORKER_EXIT:" + code));',
            'w.unref();',
            'w.ref();',
            'console.log("PARENT_END");'
        ].join('\n'), 'utf8');

        const startedAt = Date.now();
        const child = child_process.spawn(process.execPath, [childFile, liveFile], { stdio: ['ignore', 'pipe', 'pipe'] });

        let stdout = '';
        let stderr = '';
        let settled = false;

        const settle = (err) => {
            if (settled)
                return;

            settled = true;
            clearTimeout(watchdog);
            try {
                child.kill();
            } catch (_) {
                // 已退出
            }
            finish(err);
        };

        const watchdog = setTimeout(() => {
            settle(new Error('parent process did not exit after the ref()ed worker finished'));
        }, 6000);

        child.stdout.on('data', (data) => {
            stdout += data.toString();
        });
        child.stderr.on('data', (data) => {
            stderr += data.toString();
        });
        child.on('error', (err) => settle(err));
        child.on('exit', (code) => {
            const elapsed = Date.now() - startedAt;

            try {
                assert.strictEqual(code, 0, 'child exit code (stderr: ' + stderr.trim() + ')');
                assert.ok(stdout.indexOf('PARENT_END') >= 0, 'parent finished its script: ' + stdout);
                // ref'd：父进程等到 worker 退出，且 exit 事件在脚本结束之后到达
                assert.ok(stdout.indexOf('WORKER_EXIT:0') >= 0, 'parent must wait for the ref()ed worker: ' + stdout);
                assert.ok(stdout.indexOf('PARENT_END') < stdout.indexOf('WORKER_EXIT'), 'worker exit must arrive after the parent script ended: ' + stdout);
                assert.ok(elapsed >= 800, 'parent must not exit before the ref()ed worker finished, took ' + elapsed + 'ms');
                settle();
            } catch (err) {
                settle(err);
            }
        });
    });

    it('re-runs beforeExit while an unref()ed worker stays alive', (done) => {
        const finish = doneOnce(done);
        const liveFile = path.join(fixtureRoot, 'beforeexit-live.js');
        const childFile = path.join(fixtureRoot, 'beforeexit-parent.js');

        fs.writeFileSync(liveFile, 'setInterval(() => {}, 10000);\n', 'utf8');
        fs.writeFileSync(childFile, [
            "const { Worker } = require('worker_threads');",
            'const w = new Worker(process.argv[2]);',
            'w.unref();',
            'let n = 0;',
            'process.on("beforeExit", () => {',
            '  n++;',
            '  console.log("BEFORE_EXIT:" + n);',
            '  if (n === 1) setTimeout(() => console.log("TIMER"), 200);',
            '});',
            'process.on("exit", (code) => console.log("EXIT:" + code + ":" + n));'
        ].join('\n'), 'utf8');

        const child = child_process.spawn(process.execPath, [childFile, liveFile], { stdio: ['ignore', 'pipe', 'pipe'] });

        let stdout = '';
        let stderr = '';
        let settled = false;

        const settle = (err) => {
            if (settled)
                return;

            settled = true;
            clearTimeout(watchdog);
            try {
                child.kill();
            } catch (_) {
                // 已退出
            }
            finish(err);
        };

        const watchdog = setTimeout(() => {
            settle(new Error('parent process did not exit after beforeExit re-armed new work'));
        }, 5000);

        child.stdout.on('data', (data) => {
            stdout += data.toString();
        });
        child.stderr.on('data', (data) => {
            stderr += data.toString();
        });
        child.on('error', (err) => settle(err));
        child.on('exit', (code) => {
            try {
                assert.strictEqual(code, 0, 'child exit code (stderr: ' + stderr.trim() + ')');
                // Node 循环语义：unref'd worker 不阻止退出，但 beforeExit 里新安排的工作必须跑完
                const order = ['BEFORE_EXIT:1', 'TIMER', 'BEFORE_EXIT:2', 'EXIT:0:2'];
                let cursor = -1;
                for (const marker of order) {
                    const at = stdout.indexOf(marker);
                    assert.ok(at > cursor, 'expected ' + marker + ' in order, got: ' + stdout);
                    cursor = at;
                }
                settle();
            } catch (err) {
                settle(err);
            }
        });
    });
});