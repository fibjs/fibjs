/**
 * @author richardo2016@gmail.com
 * @email richardo2016
 * @create date 2021-04-11 19:01:26
 * @modify date 2021-04-11 19:01:26
 *
 * @desc fibjs's Isolate implementation
 */

#include "object.h"
#include "options.h"
#include "Fiber.h"
#include "ifs/profiler.h"
#include "ifs/global.h"
#include "X509Cert.h"
#include "HttpClient.h"
#include "SandBox.h"
#include "TTYStream.h"
#include "EventEmitter.h"
#include "v8/include/libplatform/libplatform.h"

using namespace v8;

namespace fibjs {

void init_process_ipc(Isolate* isolate);

exlib::LockedList<Isolate> s_isolates;
exlib::atomic s_iso_id;
exlib::atomic s_iso_ref;
extern v8::Platform* g_default_platform;

static int32_t syncRunMicrotasks(Isolate* isolate)
{
    JSFiber::EnterJsScope s(NULL, true);

    isolate->m_isolate->PerformMicrotaskCheckpoint();
    if (isolate->m_isolate->HasPendingBackgroundTasks())
        while (v8::platform::PumpMessageLoop(g_default_platform, isolate->m_isolate,
            isolate->m_isolate->HasPendingBackgroundTasks()
                ? v8::platform::MessageLoopBehavior::kWaitForWork
                : platform::MessageLoopBehavior::kDoNotWait))
            ;

    return 0;
}

void Isolate::RunMicrotasks()
{
    if (RunMicrotaskSize(m_isolate) > 0 || m_isolate->HasPendingBackgroundTasks())
        syncCall(this, syncRunMicrotasks, this);
}

Isolate::SnapshotJsScope::SnapshotJsScope(Isolate* cur)
    : m_isolate((cur ? cur : Isolate::current()))
{
    JSFiber* fb = JSFiber::current();
    V8FrameInfo _fi = save_fi(m_isolate->m_isolate);

    fb->m_c_entry_fp_ = _fi.entry_fp;
    fb->m_handler_ = _fi.handle;

    m_isolate->RunMicrotasks();
}

Isolate::SnapshotJsScope::~SnapshotJsScope()
{
}

static void fb_GCCallback(v8::Isolate* js_isolate, v8::GCType type, v8::GCCallbackFlags flags)
{
    Isolate* isolate = Isolate::current();
    exlib::linkitem* p;
    exlib::List<exlib::linkitem> freelist;

    while (true) {
        isolate->m_weakLock.lock();
        isolate->m_weak.getList(freelist);
        isolate->m_weakLock.unlock();

        if (freelist.empty())
            break;

        while ((p = freelist.getHead()) != 0)
            object_base::gc_delete(p);
    }
}

static void FiberProcIsolate(void* p)
{
    Isolate* isolate = (Isolate*)p;
    Runtime rtForThread(isolate);

    isolate->init();
    JSFiber::FiberProcRunJavascript(p);
}

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void* Allocate(size_t length)
    {
        void* data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void* AllocateUninitialized(size_t length)
    {
        return new uint8_t[length];
    }

    virtual void Free(void* data, size_t)
    {
        delete[] static_cast<uint8_t*>(data);
    }
};

Isolate::Isolate(exlib::string jsFilename, exlib::string jsCode)
    : m_id((int32_t)s_iso_id.inc())
    , m_hr(0)
    , m_ipc_mode(0)
    , m_test(NULL)
    , m_currentFibers(0)
    , m_idleFibers(0)
    , m_fid(1)
    , m_flake_tm(0)
    , m_flake_host(0)
    , m_flake_count(0)
    , m_console_colored(true)
    , m_loglevel(console_base::C_NOTSET)
    , m_defaultMaxListeners(10)
    , m_exitCode(0)
    , m_enable_FileSystem(true)
    , m_safe_buffer(false)
    , m_max_buffer_size(-1)
    , m_ca(new X509Cert())
{
    s_isolates.putTail(this);

    if (jsCode.empty())
        m_fname = jsFilename;
    else
        m_jsCode = jsCode;

    static v8::Isolate::CreateParams create_params;
    static ShellArrayBufferAllocator array_buffer_allocator;
    static v8::StartupData blob;

    if (blob.data == NULL) {
        v8::SnapshotCreator creator;
        v8::Isolate* isolate = creator.GetIsolate();
        {
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            creator.SetDefaultContext(context);
        }
        blob = creator.CreateBlob(v8::SnapshotCreator::FunctionCodeHandling::kClear);
    }

    create_params.snapshot_blob = &blob;
    create_params.array_buffer_allocator = &array_buffer_allocator;

    m_isolate = v8::Isolate::New(create_params);
    m_isolate->AddGCEpilogueCallback(fb_GCCallback, v8::kGCTypeMarkSweepCompact);

    m_currentFibers++;
    m_idleFibers++;

    m_ca->loadRootCerts();

    m_httpclient = new HttpClient();

    exlib::Service::CreateFiber(FiberProcIsolate, this, stack_size * 1024, "JSFiber");
}

Isolate* Isolate::current()
{
    Runtime* rt = Runtime::current();
    if (rt == NULL)
        return NULL;

    return rt->isolate();
}

static void _PromiseRejectCallback(v8::PromiseRejectMessage data)
{
    Runtime* rt = Runtime::current();
    Isolate* isolate = Isolate::current();
    v8::PromiseRejectEvent e = data.GetEvent();

    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Array> _promise_error;

    if (rt->m_promise_error.IsEmpty()) {
        _promise_error = v8::Array::New(isolate->m_isolate);
        rt->m_promise_error.Reset(isolate->m_isolate, _promise_error);
    } else
        _promise_error = rt->m_promise_error.Get(isolate->m_isolate);

    if (e == v8::kPromiseRejectWithNoHandler) {
        v8::Local<v8::Array> o = v8::Array::New(isolate->m_isolate);
        o->Set(_context, 0, data.GetPromise()).IsJust();
        o->Set(_context, 1, data.GetValue()).IsJust();
        _promise_error->Set(_context, rt->m_promise_error_no++, o).IsJust();
    } else if (e == v8::kPromiseHandlerAddedAfterReject) {
        v8::Local<v8::Promise> _promise = data.GetPromise();
        if (!_promise.IsEmpty()) {
            JSArray ks = _promise_error->GetPropertyNames(_context);
            int32_t len = ks->Length();

            for (int32_t i = 0; i < len; i++) {
                JSValue k = ks->Get(_context, i);
                JSValue v = _promise_error->Get(_context, k);

                v8::Local<v8::Array> o = v8::Local<v8::Array>::Cast(v);
                v = o->Get(_context, 0);

                bool e = _promise->Equals(_context, v).FromMaybe(false);
                if (e) {
                    _promise_error->Delete(_context, k).IsJust();
                }
            }
        }
    }
}

void Isolate::init()
{
    v8::Locker locker(m_isolate);
    v8::Isolate::Scope isolate_scope(m_isolate);
    v8::HandleScope handle_scope(m_isolate);

    v8::Local<v8::Context> _context = v8::Context::New(m_isolate);
    m_context.Reset(m_isolate, _context);

    v8::Context::Scope context_scope(_context);

    Fiber_base::class_info().getModule(this);

    JSFiber::EnterJsScope s;

    if (g_cov && m_id == 1)
        beginCoverage(m_isolate);

    _context->SetEmbedderData(1, v8::Object::New(m_isolate)->GetPrototype());

    static const char* skips[] = { "Master", "argv", "__filename", "__dirname", NULL };
    global_base::class_info().Attach(this, _context->Global(), skips);

    m_topSandbox = new SandBox();
    m_topSandbox->addBuiltinModules();

    const char* assertion_error = "class AssertionError extends Error {"
                                  "   constructor(options) {"
                                  "       var { actual, expected, message, operator } = options;"
                                  "       if (message) {"
                                  "           super(message);"
                                  "       } else {"
                                  "           if (actual && actual.stack && actual instanceof Error)"
                                  "               actual = `${actual.name}: ${actual.message}`;"
                                  "           if (expected && expected.stack && expected instanceof Error)"
                                  "               expected = `${expected.name}: ${expected.message}`;"
                                  "           super(`${JSON.stringify(actual).slice(0, 128)} ` +"
                                  "               `${operator} ${JSON.stringify(expected).slice(0, 128)}`);"
                                  "       }"
                                  "       this.generatedMessage = !message;"
                                  "       this.name = 'AssertionError [ERR_ASSERTION]';"
                                  "       this.code = 'ERR_ASSERTION';"
                                  "       this.actual = actual;"
                                  "       this.expected = expected;"
                                  "       this.operator = operator;"
                                  "   }"
                                  "}"
                                  "AssertionError;";

    v8::Local<v8::Script> script = v8::Script::Compile(_context, NewString(assertion_error)).FromMaybe(v8::Local<v8::Script>());
    v8::Local<v8::Value> result = script->Run(_context).FromMaybe(v8::Local<v8::Value>());
    v8::Local<v8::Object> AssertionError = result.As<v8::Object>();
    m_AssertionError.Reset(m_isolate, AssertionError);

    m_isolate->SetPromiseRejectCallback(_PromiseRejectCallback);

    init_process_ipc(this);
}

static result_t syncExit(Isolate* isolate)
{
    v8::HandleScope handle_scope(isolate->m_isolate);
    JSFiber::EnterJsScope s;
    JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));
    v8::Local<v8::Value> code = v8::Number::New(isolate->m_isolate, isolate->m_exitCode);
    bool r;

    t._emit("beforeExit", &code, 1, r);
    if (s_iso_ref == 1) {
        if (isolate->m_hr >= 0)
            process_base::exit();
        else
            process_base::exit(isolate->m_hr);
    }

    return 0;
}

void Isolate::Ref()
{
    s_iso_ref.inc();
}

void Isolate::Unref(int32_t hr)
{
    if (s_iso_ref.dec() == 0) {
        Isolate* isolate = s_isolates.head();
        isolate->m_hr = hr;
        syncCall(isolate, syncExit, isolate);
    }
}

void Isolate::start_profiler()
{
    if (g_prof) {
        char name[32];
        obj_ptr<Timer_base> tm;
        snprintf(name, sizeof(name), "fibjs-%08x.log", (uint32_t)(intptr_t)this);
        profiler_base::start(name, -1, g_prof_interval, tm);
    }
}
void InvokeApiInterruptCallbacks(v8::Isolate* isolate);
static result_t js_timer(Isolate* isolate)
{
    JSFiber::EnterJsScope s;
    isolate->m_has_timer = 0;
    InvokeApiInterruptCallbacks(isolate->m_isolate);
    return 0;
}

void Isolate::RequestInterrupt(v8::InterruptCallback callback, void* data)
{
    m_isolate->RequestInterrupt(callback, data);
    if (m_has_timer.CompareAndSwap(0, 1) == 0)
        syncCall(this, js_timer, this);
}

void Isolate::get_stdin(obj_ptr<Stream_base>& retVal)
{
    if (!m_stdin) {
        int32_t fd = _fileno(stdin);
        if (_is_ctx_atty(fd))
            m_stdin = new TTYInputStream(fd);
        else
            m_stdin = new UVStream(fd);
    }

    retVal = m_stdin;
}

void Isolate::get_stdout(obj_ptr<Stream_base>& retVal)
{
    if (!m_stdout) {
        int32_t fd = _fileno(stdout);
        if (_is_ctx_atty(fd))
            m_stdout = new TTYOutputStream(fd);
        else
            m_stdout = new UVStream(fd);
    }

    retVal = this->m_stdout;
}

void Isolate::get_stderr(obj_ptr<Stream_base>& retVal)
{
    if (!m_stderr) {
        int32_t fd = _fileno(stderr);
        if (_is_ctx_atty(fd))
            m_stderr = new TTYOutputStream(fd);
        else
            m_stderr = new UVStream(fd);
    }

    retVal = m_stderr;
}

} // namespace fibjs