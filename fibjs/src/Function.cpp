#include "ifs/Function.h"
#include <vector>

namespace fibjs
{

class fiber_data
{
public:
    fiber_data() : m_next(NULL)
    {
    }

    ~fiber_data()
    {
        size_t i;

        for(i = 0; i < argv.size(); i ++)
            argv[i].Dispose();

        m_func.Dispose();
    }
public:
    v8::Persistent<v8::Value> m_func;
    std::vector< v8::Persistent<v8::Value> > argv;
    fiber_data* m_next;
};

fiber::List<fiber_data> g_jobs;
fiber::Semaphore g_job_sem;

extern v8::Isolate* isolate;

extern void ReportException(v8::TryCatch* try_catch, bool rt = false);

void* t(void* p)
{
    v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);

    v8::HandleScope handle_scope;
    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);

    while(1)
    {
        {
            v8::Unlocker unlocker();
            g_job_sem.wait();
        }

        v8::HandleScope handle_scope;

        fiber_data* fb = g_jobs.get();
        size_t i;

        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(fb->m_func);
        std::vector< v8::Handle<v8::Value> > argv;

        argv.resize(fb->argv.size());
        for(i = 0; i < fb->argv.size(); i ++)
            argv[i] = fb->argv[i];

        v8::TryCatch try_catch;
        func->Call(func, argv.size(), argv.size() ? &argv[0] : NULL);
        if (try_catch.HasCaught())
            ReportException(&try_catch, true);

        delete fb;
    }

    return NULL;
}

result_t Function_base::start(const v8::Arguments& args)
{
    if (!args.This()->IsFunction())
        return CALL_E_NOTINSTANCE;

    fiber_data* fb = new fiber_data();
    int i;

    fb->argv.resize(args.Length());
    for(i = 0; i < args.Length(); i ++)
        fb->argv[i] = v8::Persistent<v8::Value>::New(args[i]);
    fb->m_func = v8::Persistent<v8::Value>::New(args.This());

    if(g_job_sem.blocked() == 0)
        fiber::Service::CreateFiber(t);

    g_jobs.put(fb);
    g_job_sem.post();

    return 0;
}

}
