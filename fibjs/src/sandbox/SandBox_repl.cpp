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

void output(int32_t priority, exlib::string msg)
{
    if (priority == console_base::C_ERROR)
        msg = logger::error() + msg + COLOR_RESET;

    if (priority != console_base::C_PRINT)
        msg += '\n';

    outLog(console_base::C_PRINT, msg);
}

result_t SandBox::repl(exlib::string src)
{
    Context context(this, src.empty() ? "[repl]" : "[eval]");

    // For -e mode, use ESM evaluation to support import syntax
    if (!src.empty()) {
        v8::Local<v8::Value> v;

        // Inject __dirname and __filename to global for -e mode
        // to provide compatibility with CommonJS-style scripts
        Isolate* isolate = holder();
        v8::Local<v8::Context> _context = isolate->context();
        v8::Local<v8::Object> global = _context->Global();
        
        exlib::string cwd;
        process_base::cwd(cwd);
        
        // __filename should be an absolute path, use cwd + '/[eval]'
        exlib::string filename = cwd;
        if (!filename.empty() && filename[filename.length() - 1] != PATH_SLASH)
            filename += PATH_SLASH;
        filename += context.m_id;
        
        global->Set(_context, isolate->NewString("__dirname"), isolate->NewString(cwd)).IsJust();
        global->Set(_context, isolate->NewString("__filename"), isolate->NewString(filename)).IsJust();

        result_t hr = context.m_sb->evalModule(src, context.m_id, v);

        if (!v.IsEmpty() && !v->IsUndefined())
            console_base::dir(v, v8::Local<v8::Object>());

        return hr;
    }

    return context.repl();
}

extern std_logger* s_std;
exlib::string appname("fibjs");

result_t SandBox::Context::repl()
{
    result_t hr = 0;
    exlib::string buf;
    v8::Local<v8::Value> v;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::String> strFname;
    obj_ptr<BufferedStream_base> bs;

    strFname = isolate->NewString(m_id);

    exlib::string str_ver("Welcome to " + appname + " ");
    str_ver += fibjs_version;
    str_ver += '.';
    output(console_base::C_INFO, str_ver);
    output(console_base::C_INFO, "Type \".help\" for more information.");

    while (true) {
        if (!buf.empty()) {
            TryCatch try_catch;

            v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
            pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, m_sb->m_id));

            v8::ScriptOrigin origin(strFname, 0, 0, false, -1, v8::Local<v8::Value>(), false, false, false, pargs);
            v8::Local<v8::Context> context = isolate->context();
            v8::Local<v8::Script> script = v8::Script::Compile(context, isolate->NewString(buf), &origin)
                                               .FromMaybe(v8::Local<v8::Script>());

            if (script.IsEmpty()) {
                if (isolate->toString(try_catch.Exception()) != "SyntaxError: Unexpected end of input") {
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
                exlib::string help_str = ".break    Sometimes you get stuck, this gets you out\n"
                                         ".clear    Clear repl output information\n"
                                         ".exit     Exit the repl\n"
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

            if (cmd_word == ".break") {
                buf.clear();
                continue;
            }

            if (cmd_word == ".clear") {
                output(console_base::C_PRINT, "\x1b"
                                              "c");
                continue;
            }
        }

        buf += line;
        buf.append("\n", 1);
    }

    return hr;
}
}
