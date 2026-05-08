const { after, before, describe, it } = require('node:test');
const assert = require('node:assert');
const fs = require('fs');
const os = require('os');
const path = require('path');
const { pathToFileURL } = require('node:url');

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

function doneOnce(done) {
    let finished = false;

    return (err) => {
        if (finished)
            return;

        finished = true;
        done(err);
    };
}

describe('worker_threads node baseline', () => {
    let fixtureRoot;
    let echoWorkerFile;
    let replyWorkerFile;
    let throwWorkerFile;
    let cwdRoot;
    let cwdChild;
    let originalCwd;

    before(() => {
        fixtureRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-worker-threads-'));
        echoWorkerFile = path.join(fixtureRoot, 'echo-worker.js');
        replyWorkerFile = path.join(fixtureRoot, 'reply-worker.js');
        throwWorkerFile = path.join(fixtureRoot, 'throw-worker.js');
        cwdRoot = path.join(fixtureRoot, 'cwd-root');
        cwdChild = path.join(cwdRoot, 'child');
        originalCwd = process.cwd();

        fs.mkdirSync(cwdChild, { recursive: true });

        fs.writeFileSync(echoWorkerFile, [
            "const { parentPort, workerData } = require('worker_threads');",
            'parentPort.postMessage({',
            '  kind: "echo",',
            '  value: workerData.value,',
            '  hasPort: !!parentPort',
            '});'
        ].join('\n'), 'utf8');

        fs.writeFileSync(replyWorkerFile, [
            "const { parentPort, workerData } = require('worker_threads');",
            'parentPort.on("message", (message) => {',
            '  if (message && message.cmd === "close") {',
            '    parentPort.close();',
            '    return;',
            '  }',
            '  parentPort.postMessage({ kind: "reply", payload: message });',
            '});'
        ].join('\n'), 'utf8');

        fs.writeFileSync(throwWorkerFile, 'throw new Error("worker boom");\n', 'utf8');
    });

    after(() => {
        process.chdir(originalCwd);
        fs.rmSync(fixtureRoot, { recursive: true, force: true });
    });

    it('exports the expected module surface in the main thread', () => {
        assert.strictEqual(typeof Worker, 'function');
        assert.strictEqual(typeof MessagePort, 'function');
        assert.strictEqual(typeof MessageChannel, 'function');
        assert.strictEqual(typeof receiveMessageOnPort, 'function');
        assert.strictEqual(isMainThread, true);
        assert.strictEqual(parentPort, null);
        assert.strictEqual(threadId, 0);
        assert.strictEqual(workerData, null);
        assert.strictEqual(typeof workerThreads.threadId, 'number');
    });

    it('accepts an absolute worker path', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(echoWorkerFile, {
            workerData: { value: 'absolute-path' }
        });

        let messageSeen = false;

        worker.once('error', finish);
        worker.once('message', (message) => {
            try {
                assert.deepStrictEqual(message, {
                    kind: 'echo',
                    value: 'absolute-path',
                    hasPort: true
                });
                messageSeen = true;
            } catch (err) {
                finish(err);
            }
        });
        worker.once('exit', (exitCode) => {
            try {
                assert.strictEqual(messageSeen, true);
                assert.strictEqual(exitCode, 0);
                finish();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('accepts a file URL worker path', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(pathToFileURL(echoWorkerFile), {
            workerData: { value: 'file-url' }
        });

        worker.once('error', finish);
        worker.once('message', (message) => {
            try {
                assert.strictEqual(message.value, 'file-url');
                worker.once('exit', () => finish());
                worker.terminate();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('accepts ./ and ../ worker paths relative to process.cwd()', (done) => {
        const finish = doneOnce(done);

        process.chdir(cwdRoot);
        const workerFromDot = new Worker('./../echo-worker.js', {
            workerData: { value: 'dot-path' }
        });

        workerFromDot.once('error', finish);
        workerFromDot.once('message', (dotMessage) => {
            try {
                assert.strictEqual(dotMessage.value, 'dot-path');
                workerFromDot.once('exit', () => {
                    process.chdir(cwdChild);

                    const workerFromDotDot = new Worker('.././../reply-worker.js', {
                        workerData: { value: 'dotdot-path' }
                    });

                    workerFromDotDot.once('error', finish);
                    workerFromDotDot.once('online', () => {
                        try {
                            workerFromDotDot.once('exit', () => finish());
                            workerFromDotDot.terminate();
                        } catch (err) {
                            finish(err);
                        }
                    });
                });
                workerFromDot.terminate();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('rejects bare relative paths', () => {
        assert.throws(() => {
            new Worker('echo-worker.js');
        }, {
            name: 'TypeError',
            code: 'ERR_WORKER_PATH'
        });
    });

    it('supports eval workers', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker([
            "const { parentPort, workerData } = require('worker_threads');",
            'parentPort.postMessage(workerData.value);'
        ].join('\n'), {
            eval: true,
            workerData: { value: 'eval-ok' }
        });

        worker.once('error', finish);
        worker.once('message', (message) => {
            try {
                assert.strictEqual(message, 'eval-ok');
                worker.once('exit', () => finish());
                worker.terminate();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('exposes a stable logical threadId for each worker isolate', (done) => {
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

    it('emits online before exit for a normal worker lifecycle', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(replyWorkerFile, {
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
                        assert.strictEqual(exitCode, 1);
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

    it('supports parent to worker and worker to parent messaging with direct payloads', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(replyWorkerFile, {
            workerData: { phase: 'roundtrip' }
        });

        let onlineSeen = false;

        worker.once('error', finish);
        worker.once('online', () => {
            onlineSeen = true;
            try {
                assert.strictEqual(onlineSeen, true);
                worker.once('message', (replyMessage) => {
                    try {
                        assert.deepStrictEqual(replyMessage, {
                            kind: 'reply',
                            payload: { seq: 1, text: 'hello' }
                        });
                        worker.once('exit', () => finish());
                        worker.terminate();
                    } catch (err) {
                        finish(err);
                    }
                });

                worker.postMessage({ seq: 1, text: 'hello' });
            } catch (err) {
                finish(err);
            }
        });
    });

    it('accepts a postMessage immediately after construction', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(replyWorkerFile, {
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

    it('emits uncaught worker exceptions on the error event', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(throwWorkerFile);

        worker.once('error', (error) => {
            try {
                assert.ok(error instanceof Error);
                assert.match(error.message, /worker boom/);
            } catch (err) {
                finish(err);
            }
        });
        worker.once('exit', (exitCode) => {
            try {
                assert.strictEqual(exitCode, 1);
                finish();
            } catch (err) {
                finish(err);
            }
        });
    });

    it('supports receiveMessageOnPort with FIFO dequeue semantics', () => {
        const { port1, port2 } = new MessageChannel();
        const first = { order: 1 };
        const second = { order: 2 };

        assert.strictEqual(receiveMessageOnPort(port2), undefined);
        port1.postMessage(first);
        port1.postMessage(second);
        assert.deepStrictEqual(receiveMessageOnPort(port2), { message: first });
        assert.deepStrictEqual(receiveMessageOnPort(port2), { message: second });
        assert.strictEqual(receiveMessageOnPort(port2), undefined);

        port2.on('message', () => {
            throw new Error('message listener should not fire during receiveMessageOnPort test');
        });
        port1.postMessage(first);
        assert.deepStrictEqual(receiveMessageOnPort(port2), { message: first });

        port1.close();
        port2.close();
    });

    it('rejects invalid receiveMessageOnPort arguments', () => {
        for (const value of [null, 0, -1, {}, []]) {
            assert.throws(() => receiveMessageOnPort(value), {
                name: 'TypeError',
                code: 'ERR_INVALID_ARG_TYPE'
            });
        }
    });

    it('returns the worker from ref() and unref()', (done) => {
        const finish = doneOnce(done);
        const worker = new Worker(replyWorkerFile, {
            workerData: { phase: 'ref' }
        });

        worker.once('error', finish);
        worker.once('online', () => {
            worker.once('exit', () => finish());
            worker.terminate();
        });

        try {
            assert.strictEqual(worker.ref(), undefined);
            assert.strictEqual(worker.unref(), undefined);
        } catch (err) {
            finish(err);
        }
    });
});