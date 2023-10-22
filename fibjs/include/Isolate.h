/*
 * Isolate.h
 *
 *  Created on: May 2, 2015
 *      Author: lion
 */

#pragma once

#include <exlib/include/list.h>
#include <exlib/include/service.h>
#include "QuickArray.h"
#include "utf8.h"
#include <unordered_map>

struct uv_loop_s;

namespace fibjs {

v8::Local<v8::String> NewString(v8::Isolate* isolate, const char* data, ssize_t length = -1);
v8::Local<v8::String> NewString(v8::Isolate* isolate, exlib::string str);
exlib::string ToString(v8::Isolate* isolate, v8::Local<v8::Value> v);

inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate,
    const char* data, int length = -1)
{
    return v8::String::NewFromOneByte(isolate,
        reinterpret_cast<const uint8_t*>(data),
        v8::NewStringType::kNormal, length)
        .ToLocalChecked();
}

class SandBox;
class JSFiber;
class HttpClient;
class LruCache;
class Stream_base;
class ValueHolder;
class X509Cert_base;
class Worker_base;

class Isolate : public exlib::linkitem {
public:
    class SnapshotJsScope {
    public:
        SnapshotJsScope(Isolate* cur = NULL);
        ~SnapshotJsScope();

    public:
        bool is_terminating();

    protected:
        Isolate* m_isolate;
        JSFiber* m_fb;
    };

    class LeaveJsScope : public SnapshotJsScope {
    public:
        LeaveJsScope(Isolate* cur = NULL)
            : SnapshotJsScope(cur)
            , unlocker(m_isolate->m_isolate)
        {
        }

    private:
        v8::Unlocker unlocker;
    };

public:
    Isolate(exlib::string jsFilename, exlib::string jsCode = "");

public:
    void init();

    static Isolate* main();

    static Isolate* current();

    static Isolate* current(v8::Isolate* v8_isolate)
    {
        return (Isolate*)v8_isolate->GetData(0);
    }

    static Isolate* current(v8::Local<v8::Context> context)
    {
        return current(context->GetIsolate());
    }

    template <typename T>
    static Isolate* current(v8::Local<T> object)
    {
        return current(object->GetCreationContextChecked());
    }

    template <typename T>
    static Isolate* current(const v8::FunctionCallbackInfo<T>& args)
    {
        return current(args.GetIsolate());
    }

    template <typename T>
    static Isolate* current(const v8::PropertyCallbackInfo<T>& args)
    {
        return current(args.GetIsolate());
    }

    template <typename T>
    static Isolate* GetCurrent(T v)
    {
        return current(v);
    }

    inline bool can_call_into_js() const
    {
        return true;
    }

    void RequestInterrupt(v8::InterruptCallback callback, void* data);
    void RunMicrotasks();

    v8::Local<v8::String> NewString(const char* data, int length = -1)
    {
        return fibjs::NewString(m_isolate, data, length);
    }

    v8::Local<v8::String> NewString(exlib::string str)
    {
        return fibjs::NewString(m_isolate, str);
    }

    v8::Local<v8::Function> NewFunction(const char* funcName, v8::FunctionCallback callback,
        v8::Local<v8::Value> data = v8::Local<v8::Value>())
    {
        v8::Local<v8::Function> func = v8::Function::New(context(), callback, data).FromMaybe(v8::Local<v8::Function>());
        if (!func.IsEmpty())
            func->SetName(NewString(funcName));
        return func;
    }

    v8::Local<v8::Context> context()
    {
        return m_isolate->GetCurrentContext();
    }

    v8::Local<v8::String> toLocalString(v8::Local<v8::Value> v)
    {
        return v->ToString(context()).FromMaybe(v8::Local<v8::String>());
    }

    exlib::string toString(v8::Local<v8::Value> v)
    {
        return ToString(m_isolate, v);
    }

    bool toBoolean(v8::Local<v8::Value> v)
    {
        if (v->IsBooleanObject())
            return v8::Local<v8::BooleanObject>::Cast(v)->ValueOf();
        return v->BooleanValue(m_isolate);
    }

    int64_t toInteger(v8::Local<v8::Value> v)
    {
        return v->IntegerValue(context()).FromMaybe(0);
    }

    int32_t toInt32Value(v8::Local<v8::Value> v)
    {
        return v->Int32Value(context()).FromMaybe(0);
    }

    uint32_t toUint32Value(v8::Local<v8::Value> v)
    {
        return v->Uint32Value(context()).FromMaybe(0);
    }

    double toNumber(v8::Local<v8::Value> v)
    {
        return v->NumberValue(context()).FromMaybe(0);
    }

    v8::Local<v8::Value> WaitPromise(v8::Local<v8::Value> promise);

    template <typename Fn>
    void SetImmediate(Fn&& cb, int32_t flags = 1)
    {
        syncCall(
            this, [cb](Isolate* isolate) {
                cb(isolate);
                return 0;
            },
            this);
    }

    void ThrowError(const char* errmsg)
    {
        ThrowError(v8::Exception::Error, errmsg);
    }

    void ThrowTypeError(const char* errmsg)
    {
        ThrowError(v8::Exception::TypeError, errmsg);
    }

    void ThrowRangeError(const char* errmsg)
    {
        ThrowError(v8::Exception::RangeError, errmsg);
    }

    void ThrowError(
        v8::Local<v8::Value> (*fun)(v8::Local<v8::String>),
        const char* errmsg)
    {
        v8::HandleScope handle_scope(m_isolate);
        m_isolate->ThrowException(fun(OneByteString(m_isolate, errmsg)));
    }

    v8::Isolate* isolate() const
    {
        return m_isolate;
    }

    typedef void (*CleanupCallback)(void*);
    void AddCleanupHook(CleanupCallback fn, void* arg)
    {
    }

    void RemoveCleanupHook(CleanupCallback fn, void* arg)
    {
    }

    static uv_loop_s* event_loop();

    void start_profiler();
    void init_global_template();

    typedef v8::Platform* (*platform_creator)();

    void Ref();
    void Unref(int32_t hr = 0);

public:
    int32_t m_id;
    int32_t m_hr;
    exlib::string m_fname;
    exlib::string m_jsCode;

    QuickArray<void*> m_classInfo;

    exlib::spinlock m_weakLock;
    exlib::List<exlib::linkitem> m_weak;

    v8::Isolate* m_isolate;
    v8::Global<v8::Context> m_context;
    v8::Global<v8::Object> m_env;

    v8::Global<v8::Object> m_AssertionError;

    v8::Global<v8::ObjectTemplate> m_global_template;

    obj_ptr<SandBox> m_topSandbox;
    std::unordered_map<uint32_t, SandBox*> m_sandboxes;
    uint32_t m_sandboxId = 0;

    bool m_intask = false;

    obj_ptr<HttpClient> m_httpclient;

    obj_ptr<Stream_base> m_stdio[3];

    obj_ptr<Stream_base>& m_stdin = m_stdio[0];
    obj_ptr<Stream_base>& m_stdout = m_stdio[1];
    obj_ptr<Stream_base>& m_stderr = m_stdio[2];

    obj_ptr<Stream_base> m_channel;
    int32_t m_ipc_mode;

    obj_ptr<Worker_base> m_worker;

    exlib::List<exlib::linkitem> m_fibers;

    void* m_test;

    exlib::Semaphore m_sem;
    exlib::LockedList<exlib::linkitem> m_jobs;
    int32_t m_currentFibers;
    int32_t m_idleFibers;

    exlib::atomic m_has_timer;

    int64_t m_fid;

    int64_t m_flake_tm;
    int32_t m_flake_host;
    int32_t m_flake_count;

    bool m_console_colored;
    int32_t m_loglevel;

    int32_t m_defaultMaxListeners;

    int32_t m_exitCode;

    bool m_enable_FileSystem;
    bool m_safe_buffer;

    obj_ptr<X509Cert_base> m_ca;

public:
    void get_stdin(obj_ptr<Stream_base>& retVal);
    void get_stdout(obj_ptr<Stream_base>& retVal);
    void get_stderr(obj_ptr<Stream_base>& retVal);
};

} /* namespace fibjs */
