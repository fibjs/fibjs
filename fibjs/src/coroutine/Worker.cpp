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
#include "file_path.h"
#include "path.h"
#include "ifs/worker_threads.h"
#include "ifs/process.h"
#include "ifs/v8.h"
#include "Buffer.h"
#include "MessagePort.h"
#include "SimpleObject.h"

namespace fibjs {

DECLARE_MODULE(worker_threads);

static bool starts_with(const exlib::string& value, const char* prefix)
{
    size_t prefix_len = strlen(prefix);
    return value.length() >= prefix_len && !strncmp(value.c_str(), prefix, prefix_len);
}

static result_t throw_worker_path_error(Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> err = v8::Exception::TypeError(isolate->NewString(
        "The worker script or module filename must be an absolute path or a relative path starting with './' or '../'."));
    err.As<v8::Object>()->Set(context, isolate->NewString("code"), isolate->NewString("ERR_WORKER_PATH")).IsJust();
    isolate->m_isolate->ThrowException(err);
    return CALL_E_JAVASCRIPT;
}

result_t worker_threads_base::get_isMainThread(bool& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = (isolate == NULL || isolate->m_parent_worker == NULL);
    return 0;
}

result_t worker_threads_base::get_threadId(int32_t& retVal)
{
    Isolate* isolate = Isolate::current();
    if (isolate == NULL || isolate->m_parent_worker == NULL)
        retVal = 0;
    else
        retVal = isolate->m_id;

    return 0;
}

result_t worker_threads_base::get_parentPort(obj_ptr<MessagePort_base>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (isolate->m_parent_port == NULL)
        return CALL_RETURN_NULL;

    retVal = isolate->m_parent_port;

    return 0;
}

result_t worker_threads_base::get_workerData(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (isolate->m_parent_worker == NULL)
        return CALL_RETURN_NULL;

    obj_ptr<Worker> worker = isolate->m_parent_worker.As<Worker>();
    if (!worker->m_has_worker_data || worker->m_workerData == NULL)
        return CALL_RETURN_NULL;

    return v8_base::deserialize(worker->m_workerData, retVal);
}

result_t worker_threads_base::receiveMessageOnPort(MessagePort_base* port, v8::Local<v8::Value>& retVal)
{
    MessagePort* messagePort = static_cast<MessagePort*>(port);
    if (messagePort == NULL)
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    if (!messagePort->receiveMessage(retVal)) {
        retVal = v8::Undefined(Isolate::current()->m_isolate);
        return 0;
    }

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> ctx = isolate->context();
    v8::Local<v8::Object> message = v8::Object::New(isolate->m_isolate);
    message->Set(ctx, isolate->NewString("message"), retVal).IsJust();
    retVal = message;

    return 0;
}

result_t Worker_base::_new(exlib::string path, v8::Local<v8::Object> opts,
    obj_ptr<Worker_base>& retVal, v8::Local<v8::Object> This)
{
    bool isEval = false;
    result_t hr = GetConfigValue(opts, "eval", isEval, false);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (!isEval) {
        bool isAbs = false;
        path_base::isAbsolute(path, isAbs);
        bool isRelativeWorkerPath = starts_with(path, "./") || starts_with(path, "../");
        bool isFileUrl = starts_with(path, "file://");

        if (!isAbs && !isRelativeWorkerPath && !isFileUrl)
            return throw_worker_path_error(Isolate::current(opts));

        hr = absolute_file_path_like(path, path);
        if (hr < 0)
            return hr;

        path_base::isAbsolute(path, isAbs);
        if (!isAbs)
            return throw_worker_path_error(Isolate::current(opts));
    }

    obj_ptr<Worker> worker = isEval ? new Worker(path, opts, true) : new Worker(path, opts);

    bool v;

    v = true;
    hr = GetConfigValue(opts, "file_system", v, false);
    if (hr >= 0)
        worker->m_isolate->m_enable_FileSystem = v;

    v = false;
    hr = GetConfigValue(opts, "safe_buffer", v, false);
    if (hr >= 0)
        worker->m_isolate->m_safe_buffer = v;

    v8::Local<v8::Value> data;
    hr = GetConfigValue(opts, "workerData", data, false);
    if (hr >= 0) {
        obj_ptr<Buffer_base> workerData;
        hr = v8_base::serialize(data, workerData);
        if (hr < 0)
            return hr;

        worker->m_peer_worker->m_workerData = workerData;
        worker->m_peer_worker->m_has_worker_data = true;
    }

    worker->wrap(This);
    worker->isolate_ref();
    worker->start();

    retVal = worker;
    return 0;
}

void Worker::start()
{
    m_isolate->sync([this]() -> int {
        _main();
        return 0;
    });
}

void Worker::_main()
{
    JSFiber::EnterJsScope s;

    m_isolate->start_profiler();
    m_peer_worker->wrap();
    _emit("online");

    m_isolate->m_topSandbox = new SandBox();
    m_isolate->m_topSandbox->addBuiltinModules();
    m_isolate->m_topSandbox->installBuffer();

    if (!m_isolate->m_jsCode.empty())
        s.m_hr = m_isolate->m_topSandbox->run_worker_source(m_isolate->m_fname, m_isolate->m_jsCode, m_peer_worker);
    else
        s.m_hr = m_isolate->m_topSandbox->run_worker(m_isolate->m_fname, m_peer_worker);
    if (s.m_hr < 0) {
        _emit("error", new NError(GetException(s.try_catch, s.m_hr, false, true)));

        emitExit(1);
        return;
    }

    return;
}

Worker::Worker(exlib::string path, v8::Local<v8::Object> opts)
{
    m_isolate = new Isolate(path);
    m_threadId = m_isolate->m_id;
    m_exited = false;
    m_has_worker_data = false;
    m_isolate->m_parent_worker = m_peer_worker = new Worker(this);
    m_peer_worker->holder(m_isolate);

    obj_ptr<MessagePort> parentPort = new MessagePort();
    obj_ptr<MessagePort> childPort = new MessagePort();
    parentPort->setPeer(childPort);
    childPort->setPeer(parentPort);
    parentPort->holder(Isolate::current());
    childPort->holder(m_isolate);

    parentPort->setCanDeliverInline(true);
    childPort->setCanDeliverInline(true);

    parentPort->setRawMessageMode(true);
    parentPort->setMessageTarget(this);
    childPort->setRawMessageMode(true);

    m_port = parentPort;
    m_peer_worker->m_port = childPort;
    m_isolate->m_parent_port = childPort;
}

Worker::Worker(exlib::string source, v8::Local<v8::Object> opts, bool isEval)
{
    exlib::string cwd;
    process_base::cwd(cwd);
    if (!cwd.empty() && cwd[cwd.length() - 1] != PATH_SLASH)
        cwd.append(1, PATH_SLASH);

    exlib::string workerPath = cwd + "[worker eval].js";
    m_isolate = new Isolate(workerPath, source);

    m_threadId = m_isolate->m_id;
    m_exited = false;
    m_has_worker_data = false;
    m_isolate->m_parent_worker = m_peer_worker = new Worker(this);
    m_peer_worker->holder(m_isolate);

    obj_ptr<MessagePort> parentPort = new MessagePort();
    obj_ptr<MessagePort> childPort = new MessagePort();
    parentPort->setPeer(childPort);
    childPort->setPeer(parentPort);
    parentPort->holder(Isolate::current());
    childPort->holder(m_isolate);

    parentPort->setCanDeliverInline(true);
    childPort->setCanDeliverInline(true);

    parentPort->setRawMessageMode(true);
    parentPort->setMessageTarget(this);
    childPort->setRawMessageMode(true);

    m_port = parentPort;
    m_peer_worker->m_port = childPort;
    m_isolate->m_parent_port = childPort;
}

result_t Worker::get_threadId(int32_t& retVal)
{
    retVal = m_threadId;
    return 0;
}

result_t Worker::terminate()
{
    if (m_exited)
        return 0;

    obj_ptr<MessagePort> mainPort = m_port;
    obj_ptr<MessagePort> peerPort = m_peer_worker ? m_peer_worker->m_port : nullptr;

    if (mainPort)
        mainPort->close();

    if (peerPort) {
        Isolate* peerIsolate = peerPort->get_holder();
        if (peerIsolate)
            peerIsolate->sync([peerPort]() -> int {
                JSFiber::EnterJsScope s;
                peerPort->close();
                return 0;
            });
    }

    emitExit(1);
    return 0;
}

result_t Worker::postMessage(v8::Local<v8::Value> data)
{
    if (m_port == NULL)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_port->postMessage(data);
}

result_t Worker::ref()
{
    isolate_ref();

    return 0;
}

result_t Worker::unref()
{
    isolate_unref();

    return 0;
}

result_t Worker::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
{
    if (type == "newListener" && m_port) {
        if (ev == "message") {
            if (m_port)
                m_port->start();
        }
    }

    return 0;
}

void Worker::onIsolateIdle(int32_t exitCode)
{
    if (m_exited)
        return;

    emitExit(exitCode);
}

result_t Worker::emitMessage(v8::Local<v8::Value> data)
{
    bool retVal;
    return _emit("message", &data, 1, retVal);
}

void Worker::emitExit(int32_t exitCode)
{
    if (m_exited)
        return;

    m_exited = true;
    obj_ptr<MessagePort> mainPort = m_port;
    obj_ptr<MessagePort> peerPort = m_peer_worker ? m_peer_worker->m_port : nullptr;

    if (mainPort)
        mainPort->close();

    if (peerPort) {
        Isolate* peerIsolate = peerPort->get_holder();
        if (peerIsolate)
            peerIsolate->sync([peerPort]() -> int {
                JSFiber::EnterJsScope s;
                peerPort->close();
                return 0;
            });
    }

    Variant exitValue = static_cast<int32_t>(exitCode);
    _emit("exit", &exitValue, 1);
    isolate_unref();
}

bool Worker::hasExitListeners()
{
    int32_t count = 0;
    listenerCount(holder()->NewString("exit"), count);
    return count > 0;
}
}
