/*
 * SandBox_cmd.cpp
 *
 *  Created on: Mar 30, 2014
 *      Author: lion
 */

#include "object.h"
#include "version.h"
#include "SandBox.h"
#include "ifs/process.h"
#include "ifs/console.h"
#include "ifs/util.h"
#include "BufferedStream.h"
#include "console.h"
#include "parse.h"

namespace fibjs {

void asyncLog(int32_t priority, exlib::string msg);

void output(int32_t priority, exlib::string msg)
{
    if (priority == console_base::C_ERROR)
        msg = logger::error() + msg + COLOR_RESET;

    if (priority != console_base::C_PRINT)
        msg += '\n';

    asyncLog(console_base::C_PRINT, msg);
}

result_t SandBox::repl(exlib::string src)
{
    Context context(this, src.empty() ? "[repl]" : "[eval]");
    return context.repl(src);
}

extern std_logger* s_std;
exlib::string appname("fibjs");

result_t SandBox::Context::repl(exlib::string src)
{
    result_t hr = 0;
    exlib::string buf(src);
    v8::Local<v8::Value> v;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::String> strFname;
    obj_ptr<BufferedStream_base> bs;

    strFname = isolate->NewString(m_id);
    if (src.empty()) {
        exlib::string str_ver("Welcome to " + appname + " ");

        str_ver += fibjs_version;
        str_ver += '.';
        output(console_base::C_INFO, str_ver);
        output(console_base::C_INFO, "Type \".help\" for more information.");
    }

    while (true) {
        if (!buf.empty()) {
            TryCatch try_catch;

            v8::ScriptOrigin origin(isolate->m_isolate, strFname);
            v8::Local<v8::Context> context = isolate->m_isolate->GetCurrentContext();
            v8::Local<v8::Script> script = v8::Script::Compile(context, isolate->NewString(buf), &origin).FromMaybe(v8::Local<v8::Script>());

            if (script.IsEmpty()) {
                if (isolate->toString(try_catch.Exception()) != "SyntaxError: Unexpected end of input" || !src.empty()) {
                    buf.clear();
                    ReportException(try_catch, 0, true);
                }
            } else {
                buf.clear();

                v = script->Run(context).FromMaybe(v8::Local<v8::Value>());
                if (v.IsEmpty())
                    ReportException(try_catch, 0, true);
            }
        }

        if (!v.IsEmpty() && !v->IsUndefined())
            console_base::dir(v, v8::Local<v8::Object>());
        v.Clear();

        if (!src.empty())
            break;

        exlib::string line;
        hr = console_base::ac_readLine(buf.empty() ? "> " : " ... ", line);
        if (hr < 0)
            break;

        if (line.empty())
            continue;

        {
            _parser p(line);
            exlib::string cmd_word;

            p.skipSpace();
            p.getWord(cmd_word);

            if (cmd_word == ".help") {
                exlib::string help_str = ".exit     Exit the repl\n"
                                         ".help     Show repl options\n"
                                         ".info     Show fibjs build information";

                output(console_base::C_INFO, help_str);
                continue;
            }

            if (cmd_word == ".exit")
                break;

            if (cmd_word == ".info") {
                v8::Local<v8::Object> o;

                process_base::get_versions(o);
                console_base::dir(o, v8::Local<v8::Object>());
                continue;
            }
        }

        buf += line;
        buf.append("\n", 1);
    }

    return hr;
}
}
