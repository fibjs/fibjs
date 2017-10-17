/*
 * Isolate.h
 *
 *  Created on: May 2, 2015
 *      Author: lion
 */

#ifndef ISOLATE_H_
#define ISOLATE_H_

#include <exlib/include/list.h>
#include <exlib/include/service.h>
#include "QuickArray.h"
#include "utf8.h"

namespace fibjs {

inline v8::Local<v8::String> NewString(v8::Isolate* isolate, const char* data, int32_t length = -1)
{
    exlib::wstring wstr = utf8to16String(data, length);

    return v8::String::NewFromTwoByte(isolate, (const uint16_t*)wstr.c_str(),
        v8::String::kNormalString, (int32_t)wstr.length());
}

inline v8::Local<v8::String> NewString(v8::Isolate* isolate, exlib::string str)
{
    return NewString(isolate, str.c_str(), (int32_t)str.length());
}

class SandBox;
class JSFiber;
class LruCache;
class File_base;

class Isolate : public exlib::linkitem {
public:
    class rt_base {
    public:
        rt_base(Isolate* cur = NULL);
        ~rt_base();

    protected:
        Isolate* m_isolate;
    };

    class rt : public rt_base {
    public:
        rt(Isolate* cur = NULL)
            : rt_base(cur)
            , unlocker(m_isolate->m_isolate)
        {
        }

    private:
        v8::Unlocker unlocker;
    };

public:
    Isolate(exlib::string fname);

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
        func->SetName(NewString(funcName));
        return func;
    }

    v8::Local<v8::Context> context()
    {
        return m_isolate->GetCurrentContext();
    }

    bool IsInUse()
    {
        return m_in_use != 0;
    }

public:
    int32_t m_id;
    exlib::string m_fname;

    QuickArray<void*> m_classInfo;

    exlib::spinlock m_weakLock;
    exlib::List<exlib::linkitem> m_weak;

    v8::Isolate* m_isolate;
    v8::Global<v8::Context> m_context;
    v8::Global<v8::Object> m_env;

    obj_ptr<LruCache> m_script_cache;
    obj_ptr<SandBox> m_topSandbox;
    obj_ptr<obj_base> m_httpclient;

    obj_ptr<File_base> m_stdin;
    obj_ptr<File_base> m_stdout;
    obj_ptr<File_base> m_stderr;

    exlib::List<exlib::linkitem> m_fibers;

    void* m_test;

    exlib::Semaphore m_sem;
    exlib::LockedList<exlib::linkitem> m_jobs;
    int32_t m_currentFibers;
    int32_t m_idleFibers;

    exlib::atomic m_pendding;
    exlib::atomic m_in_use;

    int64_t m_fid;

    int64_t m_flake_tm;
    int32_t m_flake_host;
    int32_t m_flake_count;

    int32_t m_loglevel;

    int32_t m_defaultMaxListeners;

    int32_t m_exitCode;
};

} /* namespace fibjs */
#endif /* ISOLATE_H_ */
