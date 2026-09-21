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

// Mark an object as not cloneable. No-op in fibjs: fibjs uses V8's
// ValueSerializer for postMessage, which does not honor Node.js's
// transfer_mode_private_symbol. The API is provided for compatibility
// with packages (e.g. undici) that call it in Web API constructors.
result_t worker_threads_base::markAsUncloneable(v8::Local<v8::Value> object)
{
    return 0;
}

// Mark an object as not transferable. No-op in fibjs.
result_t worker_threads_base::markAsUntransferable(v8::Local<v8::Value> object)
{
    return 0;
}

// Check if an object is marked as not transferable. Always returns false
// in fibjs since markAsUntransferable is a no-op.
result_t worker_threads_base::isMarkedAsUntransferable(v8::Local<v8::Value> object, bool& retVal)
{
    retVal = false;
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

    // 启动前就已被 terminate（或已进入终止流程）：不执行脚本、不发 online，
    // 直接按 terminated 语义结束（与 Node 一致：terminate 在 online 前的 worker 不会 online）。
    if (m_isolate->is_terminating()) {
        emitExit(1);
        return;
    }

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
        // 终止已生效且 JS 已解栈回到这里：清掉粘性终止标志，避免毒化收尾期 JS。
        // 父侧收尾任务可能在本线程还没观察到中断时就先跑过了（见 requestTerminate
        // 里的说明），那种情况下标志一直留到现在，必须在这里清。
        if (m_isolate->is_terminating())
            m_isolate->m_isolate->CancelTerminateExecution();

        // isolate 已处于终止中（worker.terminate() 中断了 JS）时，中断异常不作为错误上报，
        // 与 Node.js 一致：terminate 不产生 error 事件；退出码仍按 terminated 的 1 处理。
        if (!m_isolate->is_terminating()) {
            // 与 Node 对齐：error 载荷是「原始错误」语义 —— message 为异常自身的 message，
            // 位置/调用栈放在 stack；而不是把整段格式化报告塞进 message。
            v8::Local<v8::Context> ctx = m_isolate->context();
            v8::Local<v8::Value> ex = s.try_catch.Exception();
            exlib::string message, stack;

            if (!ex.IsEmpty() && ex->IsObject()) {
                v8::Local<v8::Object> o = ex.As<v8::Object>();
                v8::Local<v8::Value> v;

                if (o->Get(ctx, m_isolate->NewString("message")).ToLocal(&v) && !v->IsUndefined())
                    message = m_isolate->toString(v);
                if (o->Get(ctx, m_isolate->NewString("stack")).ToLocal(&v) && !v->IsUndefined())
                    stack = m_isolate->toString(v);
            }

            if (message.empty())
                message = GetException(s.try_catch, s.m_hr, false, false);

            obj_ptr<NError> err = new NError(message);
            if (!stack.empty())
                err->setStack(stack);

            _emit("error", err);
        }

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
    m_terminating = false;
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
    m_terminating = false;
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

bool Worker::canEmit(exlib::string ev)
{
    return !(ev == "online" && m_isolate && m_isolate->is_terminating());
}

// V8 中断回调：在**目标 isolate 自己的线程**上、正在执行的 JS 的下一个 interrupt
// check 点被调用。它不依赖目标 isolate 的事件循环，所以能打断纯 JS 死循环
// —— 这正是下面 sync() 收尾任务做不到的（busy loop 下那个任务队列永远不被消费）。
static void worker_terminate_interrupt(v8::Isolate* isolate, void* data)
{
    isolate->TerminateExecution();
}

// 硬中断开关。必须用 uv_os_getenv 而不是 ::getenv 读取：process.env 的赋值走
// uv_os_setenv，Windows 上它调的是 SetEnvironmentVariableW（只更新进程的 Win32 环境块），
// 而 CRT 的 getenv 读的是自己那份环境表副本（_environ），两者在 Windows 上互不同步，
// 于是「运行时 process.env.FIBJS_WORKER_HARD_TERMINATE = '1'」在 Windows 上读不到，
// 开关静默失效（表现：worker.terminate() 打不断死循环，用例超时）。
// uv_os_getenv 在 unix 上就等价于 getenv，两侧行为一致。
static bool worker_hard_terminate_enabled()
{
    char value[64];
    size_t size = sizeof(value);

    if (uv_os_getenv("FIBJS_WORKER_HARD_TERMINATE", value, &size) != 0)
        return false;

    return size > 0 && value[0] != '0';
}

// 终止 worker（同步发起，不等待退出）
//
// 硬中断（G2-b，可开关）：worker.terminate() 把终止请求投递到 worker isolate 的 V8 中断队列，
// 由 V8 在正在执行的 JS 的下一个 interrupt check 点回调 —— **不依赖 worker 的事件循环**，
// 所以纯 JS 死循环、Atomics.wait 这类场景也能被终止；软中断（只在 fibjs 可控的
// JS/native 边界生效）做不到这一点（实测 terminate() 永不返回）。
//
// 代价与约束：TerminateExecution 是 V8 的**粘性**标志，会溢出到中断点之后的 JS。
// 因此有两处必须遵守：
//   1) 清标志要排在**任何 V8 调用之前** —— 见下面收尾任务里的 CancelTerminateExecution()
//      （JSFiber::EnterJsScope 的构造内部会 wrap() 创建 V8 对象，标志未清时会拿到空句柄）。
//   2) 被打断路径上的代码不得把「可能为空的句柄」交给 V8。已修：Buffer::load_module()
//      （require 被中断时 _buffer 是空句柄却传给了 Object::Set，崩溃栈为
//      Object::Set → PropertySetterCallback → Handle<Object>::operator*）。
//
// 生产环境暂时默认关闭，仅测试环境显式打开（FIBJS_WORKER_HARD_TERMINATE=1）。
void Worker::requestTerminate()
{
    if (m_exited || m_terminating)
        return;

    m_terminating = true;
    m_isolate->m_exitCode = 1;
    m_isolate->m_terminating = true;

    // 粘性终止标志由收尾任务「借用后还原」/ _main 的解栈路径负责处理。
    if (worker_hard_terminate_enabled() && m_isolate->m_isolate)
        m_isolate->m_isolate->RequestInterrupt(worker_terminate_interrupt, nullptr);

    obj_ptr<MessagePort> mainPort = m_port;
    obj_ptr<MessagePort> peerPort = m_peer_worker ? m_peer_worker->m_port : nullptr;

    if (mainPort)
        mainPort->close();

    Isolate* peerIsolate = peerPort ? peerPort->get_holder() : nullptr;
    if (!peerIsolate)
        return;

    // 收尾任务（在 worker 的 JS 线程上执行）：停 holder、关端口。
    //
    // 顺序很关键：CancelTerminateExecution() 必须排在**任何 V8 调用之前** ——
    // JSFiber::EnterJsScope 的构造会 wrap() 创建 V8 对象，粘性终止标志未清时会
    // 拿到空句柄而崩溃。
    // 但它只能是**借用**，不能是“清掉”：若终止请求还没被 JS 消费（JS 可能正卡在
    // 会让出事件循环的 sync 型 native 调用里，实测 zlib.deflateSync 走 AsyncCall
    // 就是如此，timer 能在它返回前触发），清完不还原就等于取消了这次终止 ——
    // JS 会接着跑，terminate() 永不收敛。故先记下原状态，做完 V8 收尾立即还原。
    peerIsolate->sync([peerIsolate, peerPort]() -> int {
        v8::Isolate* v8_isolate = peerIsolate->m_isolate;
        bool was_terminating = v8_isolate->IsExecutionTerminating();
        v8_isolate->CancelTerminateExecution();

        {
            JSFiber::EnterJsScope s;

            peerIsolate->stopHoldingObjects();
            peerPort->close();
        }

        // 还原：若 JS 还没真正观察到终止，让它在下个检查点被终止；
        // 此时置上的粘性标志由 _main 的解栈路径负责清掉。
        if (was_terminating)
            v8_isolate->TerminateExecution();

        return 0;
    });
}

// worker isolate 内部（process.exit()）触发的自我退出：只结束本 worker 线程。
// 复用 terminate 的收敛语义，但全部在**当前（worker）线程**上直接执行：与 requestTerminate 同理，
// 不能靠投递任务 —— JS 正在执行时那个队列不会被消费。
// 注意：不置父侧对象的 m_terminating，退出码保持传入值（Node：worker 内 process.exit(N) → exit code N）。
void Worker::exitFromInside(int32_t code)
{
    Isolate* isolate = holder();

    isolate->m_exitCode = code;
    isolate->m_terminating = true;

    // 关闭本 isolate 的 parentPort（两端都关，父侧收尾路径与 terminate 一致）
    if (m_port)
        m_port->close();
    if (m_peer_worker && m_peer_worker->m_port)
        m_peer_worker->m_port->close();

    // 停掉本 isolate 的 holding 对象
    isolate->stopHoldingObjects();

    // 中断本 isolate 的 JS：process.exit() 之后的语句不再执行。
    // 用**软中断**（每 fiber 的 m_termed + 既有的 SnapshotJsScope 钩子，与 TimeoutScope 同一套机制），
    // 只在 fibjs 可控的 JS/native 边界上生效；不用硬 TerminateExecution —— 后者会把
    // V8 粘性终止标志溢出中断点，毒化收尾期 JS（实测为 worker 线程空 handle 崩溃）。
    //
    // 先投递「让出后取消」：软中断在边界上升级为 V8 终止后，这里把标志清掉，
    // 避免它跨过中断点影响收尾期的 JS。
    isolate->sync([isolate]() -> int {
        isolate->m_isolate->CancelTerminateExecution();
        return 0;
    });

    JSFiber* fb = JSFiber::current();
    if (fb)
        fb->m_termed = true;
}

// 供 process.cpp 调用的轻量入口（避免其依赖 Worker 的内部布局）
void worker_exit_from_inside(Isolate* isolate, int32_t code)
{
    obj_ptr<Worker> worker = isolate->m_parent_worker.As<Worker>();
    if (worker)
        worker->exitFromInside(code);
}

// 与 Node.js 对齐：terminate() 返回 Promise，在 worker 退出（exit 事件）时以退出码 resolve。
// 终止动作仍然是在本次调用中同步发起的（不等下一 tick），等待退出码则是异步完成的。
result_t Worker::terminate(int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_exited) {
        // 已经退出：立即以记录到的退出码完成（与 Node 一致）
        retVal = m_exitCode;
        return 0;
    }

    m_terminate_lock.lock();
    m_terminate_waiters.push_back({ ac, &retVal });
    m_terminate_lock.unlock();

    requestTerminate();

    return CALL_E_PENDDING;
}

result_t Worker::postMessage(v8::Local<v8::Value> data)
{
    if (m_port == NULL)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_port->postMessage(data);
}

result_t Worker::postMessage(v8::Local<v8::Value> data, v8::Local<v8::Array> transfer)
{
    if (m_port == NULL)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_port->postMessage(data, transfer);
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

    Isolate* isolate = m_isolate;
    if (m_terminating)
        exitCode = 1;
    emitExit(exitCode);

    if (isolate && isolate->m_jsService) {
        exlib::Service* svc = isolate->m_jsService;
        isolate->m_jsService = NULL;
        isolate->m_jsAffinity = false;

        svc->shutdown();
        svc->join();
        svc->Unref();
    }
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
    m_exitCode = exitCode;

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

    // Node 语义：terminate() 的 Promise 在 exit 之后以退出码 resolve。
    // 先取走等待者，避免在 post 回溯到 JS 时重复完成。
    std::vector<TerminateWaiter> waiters;
    m_terminate_lock.lock();
    waiters.swap(m_terminate_waiters);
    m_terminate_lock.unlock();

    for (auto& waiter : waiters) {
        *waiter.retVal = exitCode;
        waiter.ac->post(0);
    }
}

bool Worker::hasExitListeners()
{
    int32_t count = 0;
    listenerCount(holder()->NewString("exit"), count);
    return count > 0;
}
}
