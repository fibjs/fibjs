/*
 * Worker.h
 *
 *  Created on: Apr 18, 2017
 *      Author: lion
 */

#ifndef WORKER_H_
#define WORKER_H_

#include "ifs/Worker.h"
#include "Event.h"

namespace fibjs {

class Worker : public Worker_base {
public:
    Worker(Worker* worker)
        : m_isolate(NULL)
        , m_worker(worker)
    {
    }

    Worker(exlib::string path, v8::Local<v8::Object> opts);

    EVENT_SUPPORT();

public:
    // Worker_base
    virtual result_t postMessage(v8::Local<v8::Value> data);

public:
    EVENT_FUNC(message);

public:
    void start();

private:
    static result_t worker_fiber(Worker* worker);
    result_t _main();

private:
    Isolate* m_isolate;
    obj_ptr<Worker> m_worker;
    obj_ptr<Event_base> m_event;
};
}

#endif /* WORKER_H_ */
