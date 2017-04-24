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
    obj_ptr<Worker> worker = new Worker(path, opts);
    worker->wrap(This);
    worker->start();

    retVal = worker;
    return 0;
}

void Worker::start()
{
    syncCall(m_isolate, worker_fiber, this);
}

result_t Worker::_main()
{
    JSFiber::scope s;

    _emit("open", NULL, 0);
    return m_isolate->m_topSandbox->run_worker(m_isolate->m_fname, m_worker);
}

result_t Worker::worker_fiber(Worker* worker)
{
    return worker->_main();
}

Worker::Worker(exlib::string path, v8::Local<v8::Object> opts)
{
    m_worker = new Worker(this);
    m_isolate = new Isolate(path);
}

result_t Worker::postMessage(exlib::string data)
{
    return 0;
}

result_t Worker::postMessage(Buffer_base* data)
{
    return 0;
}
}
