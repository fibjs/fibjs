/*
 * Worker.h
 *
 *  Created on: Apr 18, 2017
 *      Author: lion
 */

#pragma once

#include "ifs/Worker.h"
#include "Event.h"
#include "WorkerMessage.h"

namespace fibjs {

class Worker : public Worker_base {
public:
    Worker(Worker* worker)
        : m_isolate(NULL)
        , m_peer_worker(worker)
    {
    }

    Worker(exlib::string path, v8::Local<v8::Object> opts);

    EVENT_SUPPORT();

public:
    // Worker_base
    virtual result_t postMessage(v8::Local<v8::Value> data);

public:
    EVENT_FUNC(load);
    EVENT_FUNC(message);
    EVENT_FUNC(error);

public:
    void start();

private:
    void _main();

public:
    Isolate* m_isolate;
    obj_ptr<Worker> m_peer_worker;
    obj_ptr<WorkerMessage> m_workerData;
};
}
