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

void* t(void* p)
{
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);

    HandleScope handle_scope;

    Handle<Context> context = Context::New(NULL, g_global);
    Context::Scope context_scope(context);

    std::vector<char> buf;

    ReadFile("main.js", buf);

    TryCatch try_catch;
    Handle<Script> script = Script::Compile(String::New(&buf.front(), buf.size()), String::New("main.js"));
    if (script.IsEmpty())
    {
        ReportException(&try_catch);
        return false;
    }
    else
    {
        Handle<Value> result = script->Run();
        if (result.IsEmpty())
        {
            ReportException(&try_catch, true);
            return false;
        }
    }

    return NULL;
}


int main()
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

    isolate = Isolate::New();

    Locker lock(isolate);
    Isolate::Scope isolate_scope(isolate);

    HandleScope handle_scope;
    g_global = Persistent<ObjectTemplate>(ObjectTemplate::New());

    Unlocker unlocker(isolate);

    fiber::Service::CreateFiber(t, NULL)->join();

    return 0;
}
