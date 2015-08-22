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
#include "BufferedStream.h"
#include "console.h"
#include "parse.h"

namespace fibjs
{

bool repl_command(std::string &line)
{
    _parser p(line);
    std::string cmd;

    p.skipSpace();
    p.getWord(cmd);

    if (!qstrcmp(cmd.c_str(), ".help"))
    {
        asyncLog(console_base::_INFO,
                 ".exit   Exit the repl\n"
                 ".help   Show repl options\n"
                 ".info   Show fibjs build information"
                );
        return true;
    }

    if (!qstrcmp(cmd.c_str(), ".exit"))
        return false;

    if (!qstrcmp(cmd.c_str(), ".info"))
    {
        v8::Local<v8::Object> o;

        util_base::buildInfo(o);
        console_base::dir(o);
        return true;
    }

    asyncLog(console_base::_ERROR, cmd + ": command not found.");
    return true;
}

result_t SandBox::repl(Stream_base* out)
{
    Context context(this, "repl");
    return Context::repl(out);
}

extern stream_logger* s_stream;
extern std_logger* s_std;

result_t SandBox::Context::repl(Stream_base* out)
{
    result_t hr = 0;
    std::string buf;
    v8::Local<v8::Value> v, v1;
    Isolate* isolate = Isolate::now();
    v8::Local<v8::String> strFname = v8::String::NewFromUtf8(isolate->m_isolate, "repl",
                                     v8::String::kNormalString, 4);
    obj_ptr<BufferedStream_base> bs;
    stream_logger* logger = NULL;

    if (out)
    {
        if ((logger = s_stream) != NULL)
        {
            s_stream = NULL;
            logger->close();
        }

        s_stream = logger = new stream_logger(out);
        bs = new BufferedStream(out);
    }

    while (true)
    {
        if (!v.IsEmpty() && !v->IsUndefined())
            console_base::dir(v);

        v = v1;

        std::string line;
        if (out)
        {
            asyncLog(console_base::_PRINT, buf.empty() ? "> " : " ... ");
            hr = bs->ac_readLine(-1, line);
            if (hr >= 0)
                s_std->log(console_base::_INFO, line);
        } else
            hr = console_base::ac_readLine(buf.empty() ? "> " : " ... ", line);

        if (hr < 0)
            break;

        if (line.empty())
            continue;

        if (line[0] == '.')
        {
            if (!repl_command(line))
                break;
            continue;
        }

        buf += line;
        buf.append("\n", 1);

        {
            v8::Local<v8::Script> script;
            TryCatch try_catch;

            script = v8::Script::Compile(
                         v8::String::NewFromUtf8(isolate->m_isolate, buf.c_str(),
                                                 v8::String::kNormalString, (int32_t) buf.length()),
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

    if (out)
    {
        if (logger == s_stream)
            s_stream = NULL;

        delete logger;
    }

    return hr;
}

}
