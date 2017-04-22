/*
 * Worker.cpp
 *
 *  Created on: Apr 18, 2017
 *      Author: lion
 */

#include "object.h"
#include "Worker.h"
#include "SandBox.h"
#include "Fiber.h"

namespace fibjs {

result_t Worker_base::_new(exlib::string path, v8::Local<v8::Object> opts,
    obj_ptr<Worker_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Worker(path, opts);
    return 0;
}

result_t Worker::_main()
{
    JSFiber::scope s;

    v8::Local<v8::Array> argv = v8::Array::New(m_isolate->m_isolate);
    return m_isolate->m_topSandbox->run(
        m_isolate->m_fname, argv, true);
}

result_t Worker::worker_fiber(Worker* worker)
{
    return worker->_main();
}

Worker::Worker(exlib::string path, v8::Local<v8::Object> opts)
{
    m_worker = new Worker(this);
    m_isolate = new Isolate(path);
    Ref();
    syncCall(m_isolate, worker_fiber, this);
}

result_t Worker::postMessage(exlib::string data)
{
    return 0;
}

result_t Worker::postMessage(Buffer_base* data)
{
    return 0;
}

result_t Worker::get_onopen(v8::Local<v8::Function>& retVal)
{
    return 0;
}

result_t Worker::set_onopen(v8::Local<v8::Function> newVal)
{
    return 0;
}

result_t Worker::get_onmessage(v8::Local<v8::Function>& retVal)
{
    return 0;
}

result_t Worker::set_onmessage(v8::Local<v8::Function> newVal)
{
    return 0;
}

result_t Worker::get_onclose(v8::Local<v8::Function>& retVal)
{
    return 0;
}

result_t Worker::set_onclose(v8::Local<v8::Function> newVal)
{
    return 0;
}

result_t Worker::get_onerror(v8::Local<v8::Function>& retVal)
{
    return 0;
}

result_t Worker::set_onerror(v8::Local<v8::Function> newVal)
{
    return 0;
}
}
