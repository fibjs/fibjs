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

namespace fibjs {

v8::Local<v8::String> NewString(v8::Isolate* isolate, const char* data, ssize_t length = -1);
v8::Local<v8::String> NewString(v8::Isolate* isolate, exlib::string str);

class SandBox;
class JSFiber;
class HttpClient;
class LruCache;
class Stream_base;
class ValueHolder;
class X509Cert_base;

class Isolate : public exlib::linkitem {
public:
    class SnapshotJsScope {
    public:
        SnapshotJsScope(Isolate* cur = NULL);
        ~SnapshotJsScope();

    protected:
        Isolate* m_isolate;
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
    Isolate(exlib::string jsFilename);

public:
    static Isolate* current();
    void init();

    void RequestInterrupt(v8::InterruptCallback callback, void* data);

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
        v8::Local<v8::Function> func = v8::Function::New(m_isolate, callback, data);
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
        return v->ToString(this->context()).ToLocalChecked();
    }

    bool toBoolean(v8::Local<v8::Value> v)
    {
        return v->BooleanValue(this->context()).ToChecked();
    }

    int64_t toInteger(v8::Local<v8::Value> v)
    {
        return v->IntegerValue(this->context()).ToChecked();
    }

    int32_t toInt32Value(v8::Local<v8::Value> v)
    {
        return v->Int32Value(this->context()).ToChecked();
    }

    uint32_t toUint32Value(v8::Local<v8::Value> v)
    {
        return v->Uint32Value(this->context()).ToChecked();
    }

    double toNumber(v8::Local<v8::Value> v)
    {
        return v->NumberValue(this->context()).ToChecked();
    }

    void start_profiler();

    void Ref();
    void Unref(int32_t hr = 0);

public:
    int32_t m_id;
    int32_t m_hr;
    exlib::string m_fname;

    QuickArray<void*> m_classInfo;

    exlib::spinlock m_weakLock;
    exlib::List<exlib::linkitem> m_weak;

    v8::Isolate* m_isolate;
    v8::Global<v8::Context> m_context;
    v8::Global<v8::Object> m_env;

    v8::Global<v8::Object> m_AssertionError;

    obj_ptr<SandBox> m_topSandbox;
    obj_ptr<HttpClient> m_httpclient;

    obj_ptr<Stream_base> m_stdin;
    obj_ptr<Stream_base> m_stdout;
    obj_ptr<Stream_base> m_stderr;

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
    int32_t m_max_buffer_size;

    obj_ptr<X509Cert_base> m_ca;

public:
    void get_stdin(obj_ptr<Stream_base>& retVal);
    void get_stdout(obj_ptr<Stream_base>& retVal);
    void get_stderr(obj_ptr<Stream_base>& retVal);
};

} /* namespace fibjs */
