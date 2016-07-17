/*
 * SandBox_cmd.cpp
 *
 *  Created on: Mar 30, 2014
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/process.h"
#include "ifs/console.h"
#include "ifs/util.h"
#include "BufferedStream.h"
#include "console.h"
#include "parse.h"

namespace fibjs
{

void output(int32_t priority, exlib::string msg)
{
    if (priority == console_base::_ERROR)
        msg = logger::error() + msg + COLOR_RESET;

    if (priority != console_base::_PRINT)
        msg += '\n';

    asyncLog(console_base::_PRINT, msg);
}

bool repl_command(exlib::string &line, v8::Local<v8::Array> cmds)
{
    _parser p(line);
    exlib::string cmd_word;
    result_t hr;
    int32_t len = cmds->Length();
    int32_t i;

    p.skipSpace();
    p.getWord(cmd_word);

    if (!qstrcmp(cmd_word.c_str(), ".help"))
    {
        exlib::string help_str = ".exit     Exit the repl\n"
                                 ".help     Show repl options\n"
                                 ".info     Show fibjs build information";

        Isolate* isolate = Isolate::current();

        for (i = 0; i < len; i ++)
        {
            v8::Local<v8::Value> v = cmds->Get(i);
            v8::Local<v8::Object> o;
            exlib::string cmd;
            exlib::string help;

            hr = GetArgumentValue(v, o, true);
            if (hr >= 0)
            {
                hr = GetConfigValue(isolate->m_isolate, o, "cmd", cmd, true);
                if (hr >= 0)
                    hr = GetConfigValue(isolate->m_isolate, o, "help", help, true);
            }

            if (hr < 0)
            {
                output(console_base::_ERROR, "Invalid cmds argument.");
                return false;
            }

            cmd.append(10, ' ');
            help_str = help_str + "\n" + cmd.substr(0, 10) + help;
        }

        output(console_base::_INFO, help_str);
        return true;
    }

    if (!qstrcmp(cmd_word.c_str(), ".exit"))
        return false;

    if (!qstrcmp(cmd_word.c_str(), ".info"))
    {
        v8::Local<v8::Object> o;

        util_base::buildInfo(o);
        console_base::dir(o);
        return true;
    }

    Isolate* isolate = Isolate::current();

    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> v = cmds->Get(i);
        v8::Local<v8::Object> o;
        exlib::string cmd;
        v8::Local<v8::Function> exec;

        hr = GetArgumentValue(v, o, true);
        if (hr >= 0)
        {
            hr = GetConfigValue(isolate->m_isolate, o, "cmd", cmd, true);
            if (hr >= 0)
                hr = GetConfigValue(isolate->m_isolate, o, "exec", exec, true);
        }

        if (hr < 0)
        {
            output(console_base::_ERROR, "Invalid cmds argument.");
            return false;
        }

        if (!qstrcmp(cmd_word.c_str(), cmd.c_str()))
        {
            v8::Local<v8::Array> argv = v8::Array::New(isolate->m_isolate);
            int32_t n = 0;

            while (!cmd_word.empty())
            {
                argv->Set(n ++, GetReturnValue(isolate->m_isolate, cmd_word));
                p.skipSpace();
                p.getWord(cmd_word);
            }

            TryCatch try_catch;
            v = argv;
            v = exec->Call(o, 1, &v);
            if (v.IsEmpty())
                ReportException(try_catch, 0);
            return true;
        }
    }

    output(console_base::_ERROR, cmd_word + ": command not found.");
    return true;
}

result_t SandBox::repl(v8::Local<v8::Array> cmds, Stream_base* out)
{
    Context context(this, "repl");
    return Context::repl(cmds, out);
}

extern stream_logger* s_stream;
extern std_logger* s_std;

result_t SandBox::Context::repl(v8::Local<v8::Array> cmds, Stream_base* out)
{
    result_t hr = 0;
    exlib::string buf;
    v8::Local<v8::Value> v, v1;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::String> strFname = isolate->NewFromUtf8("repl", 4);
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

    output(console_base::_INFO, "Type \".help\" for more information.");

    while (true)
    {
        if (!v.IsEmpty() && !v->IsUndefined())
            console_base::dir(v);

        v = v1;

        exlib::string line;
        if (out)
        {
            output(console_base::_PRINT, buf.empty() ? "> " : " ... ");
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
            if (!repl_command(line, cmds))
                break;
            continue;
        }

        buf += line;
        buf.append("\n", 1);

        {
            v8::Local<v8::Script> script;
            TryCatch try_catch;

            script = v8::Script::Compile(isolate->NewFromUtf8(buf), strFname);
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
