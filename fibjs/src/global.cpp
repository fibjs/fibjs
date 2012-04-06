#include "ifs/global.h"
#include "ifs/Fiber.h"
#include "ifs/Function.h"
#include "ifs/os.h"
#include "ifs/fs.h"

#ifdef _WIN32
#include <mmsystem.h>
inline int64_t Ticks()
{
    return timeGetTime();  // Convert to microseconds.
}

#else
#include <sys/time.h>

inline int64_t Ticks()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
        return 0;
    return (tv.tv_sec * 1000ll) + tv.tv_usec / 1000;
}

#endif

namespace fibjs
{

extern v8::Isolate* isolate;

result_t global_base::get_console(obj_ptr<console_base>& retVal)
{
    static obj_ptr<console_base> s_console;

    if(s_console == NULL)
        s_console = new console_base();

    retVal = s_console;

    return 0;
}

result_t global_base::print(const char* fmt, const v8::Arguments& args)
{
    return console_base::log(fmt, args);
}

void initGlobal(v8::Handle<v8::Object> global)
{
    fibjs::global_base::class_info().Attach(global);

    global->Set(v8::String::New("Buffer"), fibjs::Buffer_base::class_info().FunctionTemplate()->GetFunction());

    v8::Local<v8::Object> proto = global->Get(v8::String::New("Function"))->ToObject()
                          ->GetPrototype()->ToObject();

    fibjs::Function_base::class_info().Attach(proto);
}

result_t global_base::run(const char* fname)
{
    std::string buf;

    result_t hr = fs_base::readFile(fname, buf);
    if(hr < 0)
        return hr;

    v8::HandleScope handle_scope;

    v8::Persistent<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);

    initGlobal(context->Global());

    v8::TryCatch try_catch;
    v8::Handle<v8::Value> result;
    v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(buf.c_str(), (int)buf.length()), v8::String::New(fname));
    if (!script.IsEmpty())
        result = script->Run();

    if (try_catch.HasCaught())
        ReportException(&try_catch, !script.IsEmpty());

    context.Dispose();

    return 0;
}

v8::Persistent<v8::Object> s_Modules;

void initMdule()
{
	v8::HandleScope handle_scope;

	s_Modules = v8::Persistent<v8::Object>::New(v8::Object::New());

	s_Modules->Set(v8::String::New("fs"), fs_base::class_info().CreateInstance());
	s_Modules->Set(v8::String::New("os"), os_base::class_info().CreateInstance());
}

result_t global_base::require(const char* mod, v8::Handle<v8::Value>& retVal)
{
	retVal = s_Modules->Get(v8::String::New(mod));
	return 0;
}

extern AsyncQueue s_acSleep;

result_t global_base::sleep(int32_t ms)
{
    if(ms > 0)
    {
        void* args[] = {&ms};
        AsyncCall ac(args);
        s_acSleep.put(&ac);

        v8::Unlocker unlocker(isolate);
        ac.weak.wait();
    }else
    {
        v8::Unlocker unlocker(isolate);
        exlib::Fiber::yield();
    }

    return 0;
}

result_t global_base::GC()
{
    v8::V8::LowMemoryNotification();
    return 0;
}

}
