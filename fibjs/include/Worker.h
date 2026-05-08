/*
 * Worker.h
 *
 *  Created on: Apr 18, 2017
 *      Author: lion
 */

#pragma once

#include "ifs/Worker.h"

namespace fibjs {

class Buffer_base;
class MessagePort;
class WorkerExitTimer;

class Worker : public Worker_base {
    friend class WorkerExitTimer;

public:
    Worker(Worker* worker)
        : m_isolate(NULL)
        , m_peer_worker(worker)
        , m_has_worker_data(false)
    {
    }

    Worker(exlib::string path, v8::Local<v8::Object> opts);
    Worker(exlib::string path, v8::Local<v8::Object> opts, bool isEval);

public:
    // Worker_base
    virtual result_t get_threadId(int32_t& retVal);
    virtual result_t terminate();
    virtual result_t postMessage(v8::Local<v8::Value> data);
    virtual result_t ref();
    virtual result_t unref();
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func);

public:
    void start();
    void onIsolateIdle(int32_t exitCode);
    result_t emitMessage(v8::Local<v8::Value> data);

private:
    void _main();
    void emitExit(int32_t exitCode);
    bool hasExitListeners();

public:
    Isolate* m_isolate;
    obj_ptr<Worker> m_peer_worker;
    obj_ptr<MessagePort> m_port;
    int32_t m_threadId;
    bool m_exited;
    bool m_has_worker_data;
    obj_ptr<Buffer_base> m_workerData;
};
}
