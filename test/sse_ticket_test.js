'use strict';

// SSE 票据（ticket）一次性回投回归用例。
//
// 背景：`sse.upgrade` 的 accept 状态把 `next(CALL_RETURN_NULL)` —— 一张**自删除**的
// AsyncState 票据 —— 交给 EventSource，由 `EventSource::close()` 负责回投一次，
// 之后请求才收尾。Windows core dump（worker_threads_fibjs_test.js 收尾时）显示这张票
// 被**回投了两次**：第二次的虚调用落到了完全不相干的函数（dump 里是被复用内存里的
// XmlNodeList::removeChild），即 use-after-free。
//
// 本文件复现该时序：
//   1. 裸客户端发 SSE 请求但**不读取**响应，随后强关（接收缓冲有未读数据 ⇒ RST）；
//   2. 让 handler 循环退出（对齐 worker 套件 after 里 `sseClosing = true` 的时序）；
//   3. 调 `sender.close()`：服务端写终止分块 "0\r\n\r\n" 会失败，走 error()/done 收尾。
//
// 检测手段：
//   * 普通构建：close() 不得抛异常、进程不得崩；
//   * 门禁构建（configure 时 -DFIBJS_ASYNC_STATE_CHECK=1）：票据的重复/迟到回投会被
//     AsyncState 相位锁捕获，打印调用栈后 abort —— 这是定位"第二个回投者"的确定性手段。

const { describe, it, after } = require('node:test');
const assert = require('node:assert');
const net = require('net');
const http = require('http');
const sse = require('sse');
const coroutine = require('coroutine');
const test_util = require('./test_util');

const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const REQ = 'GET /sse HTTP/1.1\r\nHost: localhost\r\nAccept: text/event-stream\r\n\r\n';

// 起一个 sse.upgrade 服务；handler 登记 sender 后循环等待，直到 closeLoop 置位
const startServer = () => {
    const state = {
        senders: [],
        closeLoop: false,
        server: null,
        port: 0
    };

    state.server = new http.Server({
        '/sse': sse.upgrade((se, req) => {
            state.senders.push(se);
            while (!state.closeLoop)
                coroutine.sleep(10);
        })
    });
    state.server.listen(0, '127.0.0.1');
    state.port = state.server.address().port;

    return state;
};

// 强关客户端：不读取响应 ⇒ 接收缓冲有未读数据 ⇒ RST（服务端下一次写会失败）
const rudeClient = (port) => {
    const sock = new net.Socket();
    sock.connect(port, '127.0.0.1');
    sock.write(REQ);
    return sock;
};

const stopServer = async (state) => {
    state.closeLoop = true;
    await sleep(60);
    try {
        state.server.stop();
    } catch (e) {
        // ignore
    }
};

// 一轮：建连 → 强关 → （可选）先退出 handler 循环 → close() 每个 sender
const round = async (state, { exitLoopFirst }) => {
    const sock = rudeClient(state.port);

    // 等 accept 回调把 sender 交出来
    const deadline = Date.now() + 2000;
    while (state.senders.length === 0 && Date.now() < deadline)
        await sleep(10);
    assert.ok(state.senders.length > 0, 'sse accept handler should register a sender');

    // 等握手响应写出（客户端不读 ⇒ 留在接收缓冲里）
    await sleep(80);
    sock.close();
    await sleep(80);

    if (exitLoopFirst) {
        // 对齐 worker 套件：先让 handler 循环结束，再关闭 sender
        state.closeLoop = true;
        await sleep(60);
    }

    for (const se of state.senders) {
        let threw = null;
        try {
            se.close();
        } catch (e) {
            threw = e;
        }
        assert.strictEqual(threw, null,
            'sender.close() must not throw when the peer is already gone: ' + (threw && threw.message));

        // 幂等：第二次 close 必须是 no-op（不得再回投票据）
        se.close();
    }
    state.senders = [];

    await sleep(80);
};

describe('sse ticket close', () => {
    let state;

    it('closes a sender whose peer was killed, handler loop still running', async () => {
        state = startServer();
        await round(state, { exitLoopFirst: false });
        await stopServer(state);
    });

    it('closes a sender whose peer was killed after the handler loop exited', async () => {
        state = startServer();
        await round(state, { exitLoopFirst: true });
        await stopServer(state);
    });

    it('survives repeated peer-kill + close cycles', async () => {
        state = startServer();
        for (let i = 0; i < 20; i++)
            await round(state, { exitLoopFirst: (i % 2) === 1 });
        await stopServer(state);
    });

    it('finalizes the pending request when the sender is dropped without close()', async () => {
        // accept 回调不保存 sender、不 close ⇒ JS 侧不再持有 EventSource；
        // GC 后由 ~EventSource() 收票（apost）⇒ 挂着的请求必须收尾。
        // 若析构不收票，客户端的 body 读取会永远挂住 —— 这正是本用例的判定点。
        const state = { accepted: 0 };
        state.server = new http.Server({
            '/sse': sse.upgrade((se, req) => {
                state.accepted++;
            })
        });
        state.server.listen(0, '127.0.0.1');
        const port = state.server.address().port;

        let settled = null;
        let bodyError = null;
        const resp = http.get('http://127.0.0.1:' + port + '/sse');
        resp.readAll((err) => {
            settled = true;
            bodyError = err || null;
        });

        await sleep(150);
        assert.strictEqual(state.accepted, 1, 'sse accept handler should run');

        for (let i = 0; i < 6 && settled === null; i++) {
            test_util.gc();
            await sleep(150);
        }

        assert.strictEqual(settled, true,
            'pending request must be finalized after the sender was collected (body read hung)');
        assert.strictEqual(bodyError, null,
            'the request should be finalized normally, got: ' + (bodyError && bodyError.message));

        await stopServer(state);
    });

    after(async () => {
        if (state)
            await stopServer(state);
    });
});
