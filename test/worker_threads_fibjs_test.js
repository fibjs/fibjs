const { after, before, describe, it } = require('node:test');
const assert = require('node:assert');
const fs = require('fs');
const os = require('os');
const path = require('path');

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

function doneOnce(done) {
    let finished = false;

    return (err) => {
        if (finished)
            return;

        finished = true;
        done(err);
    };
}

describe('worker_threads fibjs target behavior', () => {
    let fixtureRoot;
    let readyWorkerFile;
    let throwWorkerFile;

    before(() => {
        fixtureRoot = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-worker-threads-target-'));
        readyWorkerFile = path.join(fixtureRoot, 'ready-worker.js');
        throwWorkerFile = path.join(fixtureRoot, 'throw-worker.js');

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
    });

    after(() => {
        fs.rmSync(fixtureRoot, { recursive: true, force: true });
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
});