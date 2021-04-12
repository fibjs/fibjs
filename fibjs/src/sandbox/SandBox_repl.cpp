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

bool repl_command(exlib::string& line)
{
    _parser p(line);
    exlib::string cmd_word;

    p.skipSpace();
    p.getWord(cmd_word);

    if (cmd_word == ".help") {
        exlib::string help_str = ".exit     Exit the repl\n"
                                 ".help     Show repl options\n"
                                 ".info     Show fibjs build information";

        Isolate* isolate = Isolate::current();

        output(console_base::C_INFO, help_str);
        return true;
    }

    if (cmd_word == ".exit")
        return false;

    if (cmd_word == ".info") {
        v8::Local<v8::Object> o;

        process_base::get_versions(o);
        console_base::dir(o);
        return true;
    }

    Isolate* isolate = Isolate::current();

    output(console_base::C_ERROR, cmd_word + ": command not found.");
    return true;
}

result_t SandBox::repl()
{
    Context context(this, "repl");
    return Context::repl();
}

extern std_logger* s_std;
exlib::string appname("fibjs");

result_t SandBox::Context::repl()
{
    result_t hr = 0;
    exlib::string buf;
    v8::Local<v8::Value> v, v1;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::String> strFname = isolate->NewString("repl", 4);
    obj_ptr<BufferedStream_base> bs;

    exlib::string str_ver("Welcome to " + appname + " ");

    str_ver += fibjs_version;
    str_ver += '.';
    output(console_base::C_INFO, str_ver);
    output(console_base::C_INFO, "Type \".help\" for more information.");

    while (true) {
        if (!v.IsEmpty() && !v->IsUndefined())
            console_base::dir(v);

        v = v1;

        exlib::string line;
        hr = console_base::ac_readLine(buf.empty() ? "> " : " ... ", line);
        if (hr < 0)
            break;

        if (line.empty())
            continue;

        if (line[0] == '.') {
            if (!repl_command(line))
                break;
            continue;
        }

        buf += line;
        buf.append("\n", 1);

        {
            v8::Local<v8::Script> script;
            TryCatch try_catch;

            v8::ScriptOrigin origin(strFname);
            v8::Local<v8::Context> context = isolate->m_isolate->GetCurrentContext();
            v8::MaybeLocal<v8::Script> lscript = v8::Script::Compile(context, isolate->NewString(buf), &origin);

            if (lscript.IsEmpty() || (script = lscript.ToLocalChecked()).IsEmpty()) {
                v8::String::Utf8Value exception(isolate->m_isolate, try_catch.Exception());
                if (*exception && qstrcmp(ToCString(exception), "SyntaxError: Unexpected end of input")) {
                    buf.clear();
                    ReportException(try_catch, 0, true);
                }
                continue;
            }

            buf.clear();

            v8::MaybeLocal<v8::Value> lv = script->Run(context);
            if (lv.IsEmpty() || (v = lv.ToLocalChecked()).IsEmpty()) {
                ReportException(try_catch, 0, true);
                continue;
            }
        }
    }

    return hr;
}
}
