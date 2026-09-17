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
});