#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/time.h>

#include <v8/v8.h>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>

#include <fiber.h>


using namespace v8;

Isolate* isolate;
Persistent<ObjectTemplate> g_global;

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
    {
        root.error(ToCString(exception));
    }
    else
    {
        if(rt)
        {
            v8::String::Utf8Value stack_trace(try_catch->StackTrace());
            if (stack_trace.length() > 0)
                root.error(ToCString(stack_trace));
        }
        else
        {
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

        String::AsciiValue str(args[i]);
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
        m_func.Dispose();
        m_func.Clear();
    }
public:
    Persistent<Value> m_func;
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
        {
            Unlocker unlocker(isolate);
            g_job_sem.wait();
        }

        HandleScope handle_scope;

        fiber_data* fb = g_jobs.get();

        Handle<Function> func = Handle<Function>::Cast(fb->m_func);
        delete fb;

        TryCatch try_catch;
        Handle<Value> result = func->Call(func, 0, NULL);
        if (result.IsEmpty())
            ReportException(&try_catch, true);
    }

    return NULL;
}

Handle<Value> fb_start(const Arguments& args)
{
    if (args.Length() != 1 || !args[0]->IsFunction())
        return ThrowException(String::New("Invalid arguments. Use 'new Fiber(name{string}, func{function() : void})'"));

    fiber_data* fb = new fiber_data();

    fb->m_func = Persistent<Value>::New(args[0]);

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

Handle<Value> runScript(const char* name)
{
    HandleScope handle_scope;

    Persistent<Context> context = Context::New(NULL, g_global);
    Context::Scope context_scope(context);

    std::vector<char> buf;

    ReadFile(name, buf);

    TryCatch try_catch;
    Handle<Value> result = Undefined();
    Handle<Script> script = Script::Compile(String::New(&buf.front(), buf.size()), String::New(name));
    if (script.IsEmpty())
        ReportException(&try_catch);
    else
    {
        result = script->Run();
        if (result.IsEmpty())
            ReportException(&try_catch, true);
    }

    context.Dispose();

    return result;
}

Handle<Value> Run(const Arguments& args)
{
    if (args.Length() != 1 || !args[0]->IsString())
        return ThrowException(String::New("Invalid arguments. Use 'new Fiber(name{string}, func{function() : void})'"));

    String::AsciiValue str(args[0]);

    return runScript(*str);
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

    v8::V8::Initialize();

    isolate = Isolate::New();

    Locker lock(isolate);
    Isolate::Scope isolate_scope(isolate);

    HandleScope handle_scope;
    g_global = Persistent<ObjectTemplate>(ObjectTemplate::New());
    g_global->Set(String::New("print"), FunctionTemplate::New(Print));
    g_global->Set(String::New("run"), FunctionTemplate::New(Run));
    g_global->Set(String::New("start"), FunctionTemplate::New(fb_start));
    g_global->Set(String::New("yield"), FunctionTemplate::New(fb_yield));

    if(argc == 2)
        runScript(argv[1]);
    else runScript("main.js");

    return 0;
}
