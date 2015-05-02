/*
 * SandBox_cmd.cpp
 *
 *  Created on: Mar 30, 2014
 *      Author: lion
 */

#include "SandBox.h"

#include "ifs/process.h"
#include "ifs/console.h"
#include "ifs/util.h"

namespace fibjs
{

void repl_command(std::string &line)
{
    if (!qstrcmp(line.c_str(), ".help"))
    {
        asyncLog(console_base::_INFO,
                 ".exit   Exit the repl\n"
                 ".help   Show repl options\n"
                 ".info   Show fibjs build information"
                );
        return;
    }

    if (!qstrcmp(line.c_str(), ".exit"))
        process_base::exit(0);

    if (!qstrcmp(line.c_str(), ".info"))
    {
        v8::Local<v8::Object> o;

        util_base::buildInfo(o);
        console_base::dir(o);
        return;
    }
}

result_t SandBox::repl()
{
    Context context(this, "repl");
    return Context::repl();
}

result_t SandBox::Context::repl()
{
    result_t hr;
    std::string buf;
    v8::Local<v8::Value> v, v1;
    Isolate &isolate = Isolate::now();
    v8::Local<v8::String> strFname = v8::String::NewFromUtf8(isolate.isolate, "repl",
                                     v8::String::kNormalString, 4);

    while (true)
    {
        if (!v.IsEmpty() && !v->IsUndefined())
            console_base::dir(v);

        v = v1;

        std::string line;
        hr = console_base::ac_readLine(buf.empty() ? "> " : " ... ", line);
        if (hr < 0)
            return hr;

        if (line.empty())
            continue;

        if (line[0] == '.')
        {
            repl_command(line);
            continue;
        }

        buf += line;
        buf.append("\n", 1);

        {
            v8::Local<v8::Script> script;
            v8::TryCatch try_catch;

            script = v8::Script::Compile(
                         v8::String::NewFromUtf8(isolate.isolate, buf.c_str(),
                                                 v8::String::kNormalString, (int) buf.length()),
                         strFname);
            if (script.IsEmpty())
            {
                v8::String::Utf8Value exception(try_catch.Exception());
                if (*exception && qstrcmp(*exception, "SyntaxError: Unexpected end of input"))
                {
                    buf.clear();
                    ReportException(try_catch, 0);
                }
                continue;
            }

            buf.clear();

            v = script->Run();
            if (v.IsEmpty())
                ReportException(try_catch, 0);
        }
    }

    return 0;
}

}
