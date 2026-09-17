/*
 * Worker.h
 *
 *  Created on: Apr 18, 2017
 *      Author: lion
 */

#pragma once

#include "ifs/Worker.h"
#include "MessagePort.h"
#include <vector>

namespace fibjs {

class Buffer_base;
class MessagePort;
class WorkerExitTimer;

// worker isolate 内部（process.exit()）的自我退出入口：
// 只结束该 worker 线程（exit 事件带该 code），不退出整个进程（Node 语义）。
// 单独声明为自由函数，使 process.cpp 不必依赖 Worker 的内部成员布局。
void worker_exit_from_inside(Isolate* isolate, int32_t code);

class Worker : public Worker_base {
    friend class WorkerExitTimer;

public:
    Worker(Worker* worker)
        : m_isolate(NULL)
        , m_peer_worker(worker)
        , m_exited(false)
        , m_terminating(false)
        , m_has_worker_data(false)
    {
    }

    Worker(exlib::string path, v8::Local<v8::Object> opts);
    Worker(exlib::string path, v8::Local<v8::Object> opts, bool isEval);

public:
    // object_base：isolate 终止时的中断。本对象是 worker 的父侧句柄；
    // 当它所在的 isolate（例如父 worker）被终止时，级联终止自己托管的 worker（Node 语义：
    // worker teardown 会一并停掉它的嵌套 worker）。
    // 释放点：worker 退出后的 emitExit() → isolate_unref()。
    virtual result_t stop()
    {
        requestTerminate();
        return 0;
    }

    // Node 语义：terminate() 早于 online 上报时，不得再上报 online。
    // online 是在 worker 线程上发出、经 JSTrigger::AsyncEmitter 异步投递到父 isolate
    // 的循环的；投递窗口内父侧可能已经调用 terminate()（此时**子** isolate 的终止标志
    // 已置位）。因此在投递真正执行前用子 isolate 的状态收口。
    // 只拦 online —— exit / error / message 必须照常上报。
    virtual bool canEmit(exlib::string ev);

    // Worker_base
    virtual result_t get_threadId(int32_t& retVal);
    virtual result_t terminate(int32_t& retVal, AsyncEvent* ac);
    virtual result_t postMessage(v8::Local<v8::Value> data);
    virtual result_t postMessage(v8::Local<v8::Value> data, v8::Local<v8::Array> transfer);
    virtual result_t ref();
    virtual result_t unref();
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func);

public:
    void start();
    void onIsolateIdle(int32_t exitCode);
    result_t emitMessage(v8::Local<v8::Value> data);

    // worker isolate 内部（process.exit()）触发的自我退出：
    // Node 语义 —— 只结束该 worker 线程（exit 事件带该 code），不退出整个进程。
    // 必须在 worker isolate 自己的线程上调用。
    void exitFromInside(int32_t code);

private:
    void _main();
    void requestTerminate();
    void emitExit(int32_t exitCode);
    bool hasExitListeners();

    // Node 语义：terminate() 返回 Promise，在 worker 退出时以退出码 resolve。
    // retVal 是调用方（AsyncCall 实例）持有的值槽，仅在本次调用完成前有效。
    struct TerminateWaiter {
        AsyncEvent* ac;
        int32_t* retVal;
    };

public:
    Isolate* m_isolate;
    obj_ptr<Worker> m_peer_worker;
    obj_ptr<MessagePort> m_port;
    int32_t m_threadId;
    int32_t m_exitCode = 0;
    bool m_exited;
    bool m_terminating;
    bool m_has_worker_data;
    obj_ptr<Buffer_base> m_workerData;

private:
    exlib::spinlock m_terminate_lock;
    std::vector<TerminateWaiter> m_terminate_waiters;
};
}
