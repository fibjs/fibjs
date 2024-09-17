/*
 * Worker.cpp
 *
 *  Created on: Apr 18, 2017
 *      Author: lion
 */

#include "object.h"
#include "Worker.h"
#include "WorkerMessage.h"
#include "SandBox.h"
#include "Fiber.h"
#include "EventInfo.h"
#include "path.h"
#include "ifs/worker_threads.h"

namespace fibjs {

DECLARE_MODULE(worker_threads);

result_t worker_threads_base::get_isMainThread(bool& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->m_id == 1;
    return 0;
}

result_t worker_threads_base::get_parentPort(obj_ptr<Worker_base>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (isolate->m_parent_worker == NULL)
        return CALL_RETURN_NULL;

    retVal = isolate->m_parent_worker;

    return 0;
}

result_t worker_threads_base::get_workerData(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (isolate->m_parent_worker == NULL)
        return CALL_RETURN_NULL;

    obj_ptr<Worker> worker = isolate->m_parent_worker.As<Worker>();
    if (worker->m_workerData == NULL)
        return CALL_RETURN_NULL;

    return worker->m_workerData->get_data(retVal);
}

result_t Worker_base::_new(exlib::string path, v8::Local<v8::Object> opts,
    obj_ptr<Worker_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current();
    bool isAbs = false;
    path_base::isAbsolute(path, isAbs);
    if (!isAbs)
        return CHECK_ERROR(Runtime::setError("Worker: only accept absolute path."));

    obj_ptr<Worker> worker = new Worker(path, opts);

    bool v;
    result_t hr;

    v = true;
    hr = GetConfigValue(isolate, opts, "file_system", v, false);
    if (hr >= 0)
        worker->m_isolate->m_enable_FileSystem = v;

    v = false;
    hr = GetConfigValue(isolate, opts, "safe_buffer", v, false);
    if (hr >= 0)
        worker->m_isolate->m_safe_buffer = v;

    v8::Local<v8::Value> data;
    hr = GetConfigValue(isolate, opts, "workerData", data, false);
    if (hr >= 0) {
        obj_ptr<WorkerMessage> wm = new WorkerMessage(data);
        hr = wm->unbind();
        if (hr < 0)
            return hr;

        worker->m_peer_worker->m_workerData = wm;
    }

    worker->wrap(This);
    worker->start();

    retVal = worker;
    return 0;
}

void Worker::start()
{
    syncCall(m_isolate, [](Worker* worker) -> int {
        worker->_main();
        return 0;
    },  this);
}

void Worker::_main()
{
    JSFiber::EnterJsScope s;

    m_isolate->start_profiler();
    m_peer_worker->wrap();

    m_isolate->m_topSandbox = new SandBox();
    m_isolate->m_topSandbox->addBuiltinModules();

    s.m_hr = m_isolate->m_topSandbox->run_worker(m_isolate->m_fname, m_peer_worker);
    if (s.m_hr < 0)
        _emit("error", new EventInfo(this, "error", s.m_hr, GetException(s.try_catch, s.m_hr, false, true)));
    else
        _emit("load");
}

Worker::Worker(exlib::string path, v8::Local<v8::Object> opts)
{
    m_isolate = new Isolate(path);
    m_isolate->m_parent_worker = m_peer_worker = new Worker(this);
    m_peer_worker->holder(m_isolate);
}

result_t Worker::postMessage(v8::Local<v8::Value> data)
{
    obj_ptr<WorkerMessage> wm = new WorkerMessage(data);
    result_t hr = wm->unbind();
    if (hr < 0)
        return hr;

    m_peer_worker->_emit("message", wm);

    return 0;
}
}
