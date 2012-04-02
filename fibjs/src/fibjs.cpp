#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <v8/v8.h>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/OstreamAppender.hh>

#include <fiber.h>

#include "ifs/Buffer.h"
#include "ifs/global.h"
#include "ifs/Function.h"

using namespace v8;

namespace fibjs
{

Isolate* isolate;

inline const char* ToCString(const v8::String::Utf8Value& value)
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
        if(lineNumber == 1)
            strError << ':' << lineNumber << ':' << (message->GetStartColumn() - 12);
        else if(lineNumber > 1)
            strError << ':' << lineNumber << ':' << (message->GetStartColumn() + 1);

        root.error(strError.str());
    }
}

static int ReadFile(const char* name, std::vector<char>& buf)
{
    FILE* file = fopen(name, "rb");
    if (file == NULL)
        return -1;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    buf.resize(size + 13);
    memcpy(&buf[0], "\"use1strict\";", 13);

    for (int i = 0; i < size;)
    {
        int read = static_cast<int>(fread(&buf[i + 13], 1, size - i, file));
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

    global->Set(String::New("run"), FunctionTemplate::New(Run)->GetFunction());

    global->Set(String::New("ReadFile"), FunctionTemplate::New(ReadFile)->GetFunction());
    global->Set(String::New("WriteFile"), FunctionTemplate::New(WriteFile)->GetFunction());

    global_base::class_info().Attach(global);

    global->Set(String::New("Buffer"), Buffer_base::class_info().getTemplate()->GetFunction());

    Local<Object> proto = global->Get(String::New("Function"))->ToObject()
                          ->GetPrototype()->ToObject();

    Function_base::class_info().Attach(proto);
}

class MyAppender : public log4cpp::LayoutAppender
{
public:
    MyAppender() : LayoutAppender("console")
    {
        puts("MyAppender");
    }

    void close()
    {
    }

protected:
    void _append(const log4cpp::LoggingEvent& event)
    {
        if(event.priority > log4cpp::Priority::NOTICE)
            std::cerr << event.message << std::endl;
        else std::cout << event.message << std::endl;
    }
};

extern "C" int main(int argc, char* argv[])
{
    try
    {
        log4cpp::PropertyConfigurator::configure("log.ini");
    }
    catch (log4cpp::ConfigureFailure e)
    {
        log4cpp::Category& root = log4cpp::Category::getRoot();
        root.addAppender(new MyAppender());
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

}
