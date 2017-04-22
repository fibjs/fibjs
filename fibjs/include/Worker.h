/*
 * Worker.h
 *
 *  Created on: Apr 18, 2017
 *      Author: lion
 */

#ifndef WORKER_H_
#define WORKER_H_

#include "ifs/Worker.h"

namespace fibjs {

class Worker : public Worker_base {
public:
    Worker(Worker* worker)
        : m_worker(worker)
    {
    }

    Worker(exlib::string path, v8::Local<v8::Object> opts);

    EVENT_SUPPORT();

public:
    // Worker_base
    virtual result_t postMessage(exlib::string data);
    virtual result_t postMessage(Buffer_base* data);
    virtual result_t get_onopen(v8::Local<v8::Function>& retVal);
    virtual result_t set_onopen(v8::Local<v8::Function> newVal);
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal);
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal);
    virtual result_t get_onclose(v8::Local<v8::Function>& retVal);
    virtual result_t set_onclose(v8::Local<v8::Function> newVal);
    virtual result_t get_onerror(v8::Local<v8::Function>& retVal);
    virtual result_t set_onerror(v8::Local<v8::Function> newVal);

private:
    static result_t worker_fiber(Worker* worker);
    result_t _main();

private:
    Isolate* m_isolate;
    obj_ptr<Worker> m_worker;
};
}

#endif /* WORKER_H_ */
