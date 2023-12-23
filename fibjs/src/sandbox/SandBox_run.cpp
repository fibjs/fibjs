/*
 * SandBox_run.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "Buffer.h"
#include "path.h"
#include "parse.h"
#include "options.h"
#include "ifs/global.h"
#include "ifs/encoding.h"

namespace fibjs {

extern std::vector<char*> s_argv;
static std::vector<exlib::string> script_argv;

result_t SandBox::run_main(exlib::string fname, v8::Local<v8::Array> argv)
{
    result_t hr;
    obj_ptr<Buffer_base> bin;
    int32_t step = 0;

    while (true) {
        if (fname.c_str()[0] == '-' && fname.c_str()[1] == '-') {
            int32_t i;
            exlib::string tmp("opt_tools/");
            tmp += fname.c_str() + 2;

            for (i = 0; opt_tools[i].name && qstrcmp(opt_tools[i].name, tmp.c_str()); i++)
                ;
            opt_tools[i].getDate(bin);

            break;
        }

        bool isAbs;
        exlib::string rname;

        path_base::isAbsolute(fname, isAbs);
        if (!isAbs) {
            rname = fname;
            os_resolve(fname);
        } else
            path_base::normalize(fname, fname);

        hr = resolveFile(fname, bin, NULL);
        if (hr >= 0)
            break;

        if (isAbs)
            return hr;

        fname = "node_modules/.bin/" + rname;
        os_resolve(fname);

        hr = resolveFile(fname, bin, NULL);
        if (hr >= 0)
            break;

        if (step > 0)
            return CALL_E_FILE_NOT_FOUND;

        v8::Local<v8::Value> v;
        exlib::string buf;
        Isolate* isolate = holder();
        v8::Local<v8::Context> context = isolate->context();

        hr = loadFile("package.json", bin);
        if (hr < 0)
            return CALL_E_FILE_NOT_FOUND;

        bin->toString(buf);
        hr = json_base::decode(buf, v);
        if (hr < 0)
            return hr;

        if (v.IsEmpty() || !v->IsObject())
            return CHECK_ERROR(Runtime::setError("SandBox: Invalid package.json"));

        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
        v8::Local<v8::Value> scripts = o->Get(context, isolate->NewString("scripts", 7)).FromMaybe(v8::Local<v8::Value>());
        if (IsEmpty(scripts) || !scripts->IsObject())
            return CALL_E_FILE_NOT_FOUND;

        o = v8::Local<v8::Object>::Cast(scripts);

        v8::Local<v8::Value> cmd = o->Get(context, isolate->NewString(rname)).FromMaybe(v8::Local<v8::Value>());
        if (IsEmpty(cmd) || !cmd->IsString())
            return CALL_E_FILE_NOT_FOUND;

        exlib::string cmd_str = isolate->toString(cmd);
        fname.clear();

        _parser p(cmd_str.c_str(), (int32_t)cmd_str.length());

        p.skipSpace();

        char ch = p.get();
        if (ch == '\'' || ch == '\"') {
            p.skip();
            p.getString(fname, ch);
            p.skip();
        } else
            p.getWord(fname);

        script_argv.clear();

        while (true) {
            exlib::string arg;

            p.skipSpace();

            char ch = p.get();
            if (ch == '\'' || ch == '\"') {
                p.skip();
                p.getString(arg, ch);
                p.skip();
            } else
                p.getWord(arg);

            if (arg.empty())
                break;

            script_argv.push_back(arg);
        }

        s_argv.resize(2);
        for (size_t i = 0; i < script_argv.size(); i++)
            s_argv.push_back((char*)script_argv[i].c_str());

        step++;
    }

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr < 0)
        return hr;

    Context context(this, fname);

    std::vector<ExtLoader::arg> extarg(1);
    extarg[0] = ExtLoader::arg("__argv", argv);

    return l->run_script(&context, bin, fname, extarg, true);
}

result_t SandBox::run_worker(exlib::string fname, Worker_base* master)
{
    result_t hr;
    bool isAbs;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file name."));
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin;
    hr = resolveFile(fname, bin, NULL);
    if (hr < 0)
        return hr;

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr < 0)
        return hr;

    Context context(this, fname);

    std::vector<ExtLoader::arg> extarg(1);
    extarg[0] = ExtLoader::arg("Master", master->wrap());

    return l->run_script(&context, bin, fname, extarg, false);
}

result_t SandBox::run(exlib::string fname)
{
    Scope _scope(this);

    result_t hr;
    bool isAbs;

    path_base::isAbsolute(fname, isAbs);
    if (!isAbs)
        return CHECK_ERROR(Runtime::setError("SandBox: Invalid file name."));
    path_base::normalize(fname, fname);

    obj_ptr<Buffer_base> bin;
    hr = resolveFile(fname, bin, NULL);
    if (hr < 0)
        return hr;

    obj_ptr<ExtLoader> l;
    hr = get_loader(fname, l);
    if (hr < 0)
        return hr;

    Context context(this, fname);
    std::vector<ExtLoader::arg> extarg;

    return l->run_script(&context, bin, fname, extarg, false);
}
}
