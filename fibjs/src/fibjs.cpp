#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <v8/v8.h>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>

#include <fiber.h>

#include "ifs/console.h"
#include "ifs/os.h"
#include "ifs/file.h"
#include "ifs/fs.h"


using namespace v8;
using namespace fibjs;

Isolate* isolate;

const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::TryCatch* try_catch, bool rt = false)
{
    log4cpp::Category& root = log4cpp::Category::getRoot();
    HandleScope handle_scope;

    v8::String::Utf8Value exception(try_catch->Exception());

    Handle<Message> message = try_catch->Message();
    if (message.IsEmpty())
        root.error(ToCString(exception));
    else
    {
        if(rt)
        {
            v8::String::Utf8Value stack_trace(try_catch->StackTrace());
            if (stack_trace.length() > 0)
            {
                root.error(ToCString(stack_trace));
                return;
            }
        }

        std::stringstream strError;

        v8::String::Utf8Value filename(message->GetScriptResourceName());
        strError << ToCString(exception) << "\n    at ";
        strError << ToCString(filename);
        int lineNumber = message->GetLineNumber();
        if(lineNumber > 0)
            strError << ':' << lineNumber << ':' << (message->GetStartColumn() + 1);

        root.error(strError.str());
    }
}

Handle<Value> Print(const Arguments& args)
{
    HandleScope handle_scope;

    bool first = true;
    for (int i = 0; i < args.Length(); i++)
    {
        if (first)
            first = false;
        else
            printf(" ");

        String::Utf8Value str(args[i]);
        printf("%s",  *str);
    }
    printf("\n");

    return Undefined();
}

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
    Persistent<Value> m_func;
    std::vector< Persistent<Value> > argv;
    fiber_data* m_next;
};

fiber::List<fiber_data> g_jobs;
fiber::Semaphore g_job_sem;

void* t(void* p)
{
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);

//    HandleScope handle_scope;
//    Handle<Context> context = Context::New();
//    Context::Scope context_scope(context);

    while(1)
    {
        V8::AdjustAmountOfExternalAllocatedMemory(128);
        {
            Unlocker unlocker(isolate);
            g_job_sem.wait();
        }

        HandleScope handle_scope;

        fiber_data* fb = g_jobs.get();
        size_t i;

        Handle<Function> func = Handle<Function>::Cast(fb->m_func);
        std::vector< Handle<Value> > argv;

        argv.resize(fb->argv.size());
        for(i = 0; i < fb->argv.size(); i ++)
            argv[i] = fb->argv[i];

        TryCatch try_catch;
        func->Call(func, argv.size(), argv.size() ? &argv[0] : NULL);
        if (try_catch.HasCaught())
            ReportException(&try_catch, true);

        delete fb;
        V8::AdjustAmountOfExternalAllocatedMemory(-128);
    }

    return NULL;
}

Handle<Value> fb_start(const Arguments& args)
{
    if (!args.This()->IsFunction())
        return ThrowException(String::New("Invalid arguments. Use function.start(...);'"));

    fiber_data* fb = new fiber_data();
    int i;

    fb->argv.resize(args.Length());
    for(i = 0; i < args.Length(); i ++)
        fb->argv[i] = Persistent<Value>::New(args[i]);
    fb->m_func = Persistent<Value>::New(args.This());

    if(g_job_sem.blocked() == 0)
        fiber::Service::CreateFiber(t);

    g_jobs.put(fb);
    g_job_sem.post();

    return Undefined();
}

Handle<Value> fb_yield(const Arguments& args)
{
    Unlocker unlocker(isolate);
    fiber::Fiber::yield();
    return Undefined();
}

static int ReadFile(const char* name, std::vector<char>& buf)
{
    FILE* file = fopen(name, "rb");
    if (file == NULL)
        return -1;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    buf.resize(size);

    for (int i = 0; i < size;)
    {
        int read = static_cast<int>(fread(&buf[i], 1, size - i, file));
        i += read;
    }
    fclose(file);

    return 0;
}

void initGlobal(Persistent<Context>& context);

Handle<Value> Run(const Arguments& args)
{
    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(String::New("Invalid arguments. Use 'new Fiber(name{string}, func{function() : void})'"));

    String::Utf8Value str(args[0]);
    const char* name = *str;

    HandleScope handle_scope;

    Persistent<Context> context = Context::New();
    Context::Scope context_scope(context);

    initGlobal(context);

    std::vector<char> buf;

    ReadFile(name, buf);

    TryCatch try_catch;
    Handle<Value> result;
    Handle<Script> script = Script::Compile(String::New(&buf.front(), buf.size()), String::New(name));
    if (!script.IsEmpty())
        result = script->Run();

    if (try_catch.HasCaught())
    {
        ReportException(&try_catch, !script.IsEmpty());

        std::string strMessage(name);

        strMessage += script.IsEmpty() ? ": Script Compile Error." : ": Script Runtime Error.";
        result = ThrowException(String::New(strMessage.c_str()));
    }

    context.Dispose();

    return result;
}

Handle<Value> runScript(const char* name)
{
    HandleScope handle_scope;

    Handle<ObjectTemplate> global_templ = ObjectTemplate::New();
    global_templ->Set(String::New("print"), FunctionTemplate::New(Print));

    Persistent<Context> context = Context::New(NULL, global_templ);
    Context::Scope context_scope(context);

    initGlobal(context);

    std::vector<char> buf;

    ReadFile(name, buf);

    TryCatch try_catch;
    Handle<Value> result;
    Handle<Script> script = Script::Compile(String::New(&buf.front(), buf.size()), String::New(name));
    if (!script.IsEmpty())
        result = script->Run();

    if (try_catch.HasCaught())
        ReportException(&try_catch, !script.IsEmpty());

    context.Dispose();

    return result;
}

Handle<Value> ReadFile(const Arguments& args)
{
    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(String::New("Invalid arguments. Use 'new Fiber(name{string}, func{function() : void})'"));

    String::Utf8Value name(args[0]);

    FILE* file = fopen(*name, "rb");
    if (file == NULL)
        return Undefined();

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    std::vector<char> buf;

    buf.resize(size);

    for (int i = 0; i < size;)
    {
        int read = static_cast<int>(fread(&buf[i], 1, size - i, file));
        i += read;
    }
    fclose(file);

    return String::New(&buf[0], size);
}


Handle<Value> WriteFile(const Arguments& args)
{
    if (args.Length() != 2 || !args[0]->IsString())
        return ThrowException(String::New("Invalid arguments. Use 'new Fiber(name{string}, func{function() : void})'"));

    String::Utf8Value name(args[0]);
    String::Utf8Value data(args[1]);

    FILE* file = fopen(*name, "wb+");
    if (file == NULL)
        return Undefined();

    int size = strlen(*data);

    fwrite(*data, 1, size, file);
    fclose(file);

    return Int32::New(size);
}

void initGlobal(Persistent<Context>& context)
{
    HandleScope handle_scope;

    Local<Object> global = context->Global();

    global->Set(String::New("print"), FunctionTemplate::New(Print)->GetFunction());
    global->Set(String::New("run"), FunctionTemplate::New(Run)->GetFunction());
    global->Set(String::New("yield"), FunctionTemplate::New(fb_yield)->GetFunction());

    global->Set(String::New("ReadFile"), FunctionTemplate::New(ReadFile)->GetFunction());
    global->Set(String::New("WriteFile"), FunctionTemplate::New(WriteFile)->GetFunction());

    global->Set(v8::String::New("console"), console_base::info().CreateInstance());
    global->Set(v8::String::New("os"), os_base::info().CreateInstance());
    global->Set(v8::String::New("fs"), fs_base::info().CreateInstance());

    Local<Object> fun = global->Get(String::New("Function"))->ToObject();
    Local<Object> proto = fun->GetPrototype()->ToObject();

    proto->Set(String::New("start"), FunctionTemplate::New(fb_start)->GetFunction());
}

int main(int argc, char* argv[])
{
    try
    {
        log4cpp::PropertyConfigurator::configure("log.ini");
    }
    catch (log4cpp::ConfigureFailure e)
    {
        log4cpp::Category& root = log4cpp::Category::getRoot();

        log4cpp::Appender* appender = new log4cpp::OstreamAppender("console", &std::cout);
        appender->setLayout(new log4cpp::BasicLayout());
        root.addAppender(appender);
        root.setPriority(log4cpp::Priority::DEBUG);

        root.warn(e.what());
    }
/*
    v8::ResourceConstraints rc;
    rc.set_max_young_space_size(2048); //KB
    rc.set_max_old_space_size(10); //MB
    rc.set_max_executable_size(10); //MB
    rc.set_stack_limit(reinterpret_cast<uint32_t*>((char*)&rc- 1024 * 400));

    SetResourceConstraints(&rc);
*/
    V8::Initialize();

    isolate = Isolate::GetCurrent();
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);

    if(argc == 2)
        runScript(argv[1]);
    else runScript("main.js");

    return 0;
}
