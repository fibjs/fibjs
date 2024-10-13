/*
 * child_process.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#undef stdout
#undef stderr
#include "ifs/child_process.h"
#include "ChildProcess.h"
#include "AsyncUV.h"
#include <uv/include/uv.h>
#include "MemoryStream.h"
#include "encoding.h"
#include "StringBuffer.h"
#include "ifs/process.h"

namespace fibjs {

DECLARE_MODULE(child_process);

result_t child_process_base::spawn(exlib::string command, v8::Local<v8::Array> args,
    v8::Local<v8::Object> options, obj_ptr<ChildProcess_base>& retVal)
{
    obj_ptr<ChildProcess> cp = new ChildProcess();
    retVal = cp;

    return cp->spawn(command, args, options, false);
}

result_t child_process_base::spawn(exlib::string command, v8::Local<v8::Object> options,
    obj_ptr<ChildProcess_base>& retVal)
{
    return spawn(command, v8::Local<v8::Array>(), options, retVal);
}

result_t child_process_base::execFile(exlib::string command, v8::Local<v8::Array> args,
    v8::Local<v8::Object> options, obj_ptr<ExecFileType>& retVal, AsyncEvent* ac)
{
    class ReadStdout : public AsyncEvent {
    public:
        ReadStdout(obj_ptr<ExecFileType>& retVal, AsyncEvent* ac)
            : m_codec(ac->m_ctx[0].string())
            , m_retVal(retVal)
            , m_ac(ac)
        {
            setAsync();
            m_cp = ac->m_ctxo.As<ChildProcess_base>();

            m_cp->get_stdout(m_stdout);
            if (m_stdout) {
                m_cnt.inc();
                m_bufout = new MemoryStream();
            }

            m_cp->get_stderr(m_stderr);
            if (m_stderr) {
                m_cnt.inc();
                m_buferr = new MemoryStream();
            }

            m_cnt.inc();
            m_cp->join(m_status, this);

            if (m_stdout)
                m_stdout->copyTo(m_bufout, -1, m_szout, this);

            if (m_stderr)
                m_stderr->copyTo(m_buferr, -1, m_szerr, this);
        }

        static Variant getBuffer(obj_ptr<MemoryStream>& stream, exlib::string codec)
        {
            Variant v;

            if (!stream) {
                v.setNull();
                return v;
            }

            obj_ptr<Buffer_base> buf;

            stream->rewind();
            result_t hr = stream->cc_readAll(buf);
            if (hr == CALL_RETURN_NULL)
                v.setNull();
            else if (codec == "buffer")
                v = buf;
            else {
                exlib::string s;

                if (buf->toString(codec, 0, s) < 0)
                    v = buf;
                else
                    v = s;
            }

            return v;
        }

        virtual int32_t post(int32_t v)
        {
            if (m_cnt.dec() == 0) {
                m_retVal = new ExecFileType();

                m_retVal->stdout = getBuffer(m_bufout, m_codec);
                m_retVal->stderr = getBuffer(m_buferr, m_codec);
                m_cp->get_exitCode(m_retVal->exitCode);

                m_ac->post(0);
                delete this;
            }

            return 0;
        }

    private:
        exlib::string m_codec;
        obj_ptr<ExecFileType>& m_retVal;
        AsyncEvent* m_ac;

        ChildProcess_base* m_cp;

        exlib::atomic m_cnt;

        obj_ptr<Stream_base> m_stdout;
        obj_ptr<MemoryStream> m_bufout;
        int64_t m_szout;

        obj_ptr<Stream_base> m_stderr;
        obj_ptr<MemoryStream> m_buferr;
        int64_t m_szerr;

        int32_t m_status;
    };

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);
        exlib::string cmd;
        v8::Local<v8::Value> opts_;
        v8::Local<v8::Object> opts;
        obj_ptr<ChildProcess_base> cp;

        util_base::clone(options, opts_);

        opts = opts_.As<v8::Object>();

        exlib::string codec("utf8");
        GetConfigValue(isolate, opts, "encoding", codec);

        result_t hr = spawn(command, args, opts, cp);
        if (hr < 0)
            return hr;

        ac->m_ctxo = cp;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = codec;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    new ReadStdout(retVal, ac);
    return CALL_E_PENDDING;
}

result_t child_process_base::execFile(exlib::string command, v8::Local<v8::Object> options,
    obj_ptr<ExecFileType>& retVal, AsyncEvent* ac)
{
    return execFile(command, v8::Local<v8::Array>(), options, retVal, ac);
}

result_t child_process_base::exec(exlib::string command, v8::Local<v8::Object> options,
    obj_ptr<ExecType>& retVal, AsyncEvent* ac)
{
#ifdef _WIN32
    const char* shell = "cmd.exe";
#elif defined(__ANDROID__)
    const char* shell = "/system/bin/sh";
#else
    const char* shell = "/bin/sh";
#endif

    obj_ptr<ExecFileType>& _retVal = *((obj_ptr<ExecFileType>*)&retVal);

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate);
        v8::Local<v8::Object> opts;

#ifdef _WIN32
        static bool init = false;
        static bool is_cmd_exe = false;

        if (!init) {
            size_t sz = 4096;
            char* env_shell = (char*)malloc(sz);

            if (uv_os_getenv("ComSpec", env_shell, &sz) == 0)
                shell = env_shell;
            else
                free(env_shell);

            int32_t len = strlen(shell);
            if (len >= 7 && stricmp(shell + len - 7, "cmd.exe") == 0) {
                if (len == 7 || isPathSlash(shell[len - 8]))
                    is_cmd_exe = true;
            } else if (len >= 3 && stricmp(shell + len - 3, "cmd") == 0) {
                if (len == 3 || isPathSlash(shell[len - 4]))
                    is_cmd_exe = true;
            }

            init = true;
        }

        if (is_cmd_exe) {
            args->Set(context, 0, isolate->NewString("/d")).IsJust();
            args->Set(context, 1, isolate->NewString("/s")).IsJust();
            args->Set(context, 2, isolate->NewString("/c")).IsJust();
            args->Set(context, 3, isolate->NewString(command)).IsJust();
        } else {
            args->Set(context, 0, isolate->NewString("/c")).IsJust();
            args->Set(context, 1, isolate->NewString(command)).IsJust();
        }

        v8::Local<v8::String> windowsVerbatimArguments = isolate->NewString("windowsVerbatimArguments");
        if (!options->Has(context, windowsVerbatimArguments).FromMaybe(false)) {
            v8::Local<v8::Value> opts_;
            util_base::clone(options, opts_);

            opts = opts_.As<v8::Object>();
            opts->Set(context, windowsVerbatimArguments, v8::True(isolate->m_isolate)).IsJust();
        } else
            opts = options;
#else
        args->Set(context, 0, isolate->NewString("-c")).IsJust();
        args->Set(context, 1, isolate->NewString(command)).IsJust();

        opts = options;
#endif

        return execFile(shell, args, opts, _retVal, ac);
    }

    return execFile(shell, v8::Local<v8::Array>(), options, _retVal, ac);
}

result_t child_process_base::sh(v8::Local<v8::Array> strings, OptArgs args, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(strings);
        v8::Local<v8::Context> context = isolate->context();
        StringBuffer sbuf;
        int32_t argc = strings->Length();
        size_t strcnt = strings->Length();

        for (int32_t i = 0; i < strcnt; i++) {
            v8::Local<v8::Value> v = strings->Get(context, i).FromMaybe(v8::Local<v8::Value>());
            v8::String::Utf8Value str(isolate->m_isolate, v);
            sbuf.append(*str, str.length());

            if (i < argc - 1) {
                v8::Local<v8::Value> v1 = args[i];

                if (v1->IsArray()) {
                    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v1);
                    int32_t len = arr->Length();

                    for (int32_t j = 0; j < len; j++) {
                        v8::Local<v8::Value> v2 = arr->Get(context, j).FromMaybe(v8::Local<v8::Value>());
                        v8::String::Utf8Value str2(isolate->m_isolate, v2);
                        if (*str2)
                            sbuf.append(*str2, str2.length());

                        if (j < len - 1)
                            sbuf.append(' ');
                    }
                } else {
                    v8::String::Utf8Value str1(isolate->m_isolate, v1);
                    if (*str1)
                        sbuf.append(*str1, str1.length());
                }
            }
        }

        exlib::string cmd = sbuf.str();
        v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

        ac->m_ctx.resize(2);
        ac->m_ctx[1] = cmd;

        return exec(cmd, opts, *(obj_ptr<ExecType>*)nullptr, ac);
    }

    class AsyncShell : public AsyncEvent {
    public:
        AsyncShell(exlib::string& retVal, AsyncEvent* ac)
            : m_retVal(retVal)
            , m_ac(ac)
        {
            m_ctx.resize(1);
            m_ctx[0] = ac->m_ctx[0];
            m_ctxo = ac->m_ctxo;

            setAsync();
        }

        static exlib::string process_output(Variant& v)
        {
            exlib::string s = v.string();
            const char* c_s = s.c_str();
            size_t sz = s.length();

#ifdef _WIN32
            if (sz > 1 && c_s[sz - 2] == '\r' && c_s[sz - 1] == '\n')
                s.resize(sz - 2);
#else
            if (sz > 0 && c_s[sz - 1] == '\n')
                s.resize(sz - 1);
#endif

            return s;
        }

        virtual int32_t post(int32_t v)
        {
            if (m_exec_retVal->exitCode) {
                m_ac->post(Runtime::setError(process_output(m_exec_retVal->stderr)));
            } else {
                m_retVal = process_output(m_exec_retVal->stdout);
                m_ac->post(v);
            }
            delete this;
            return 0;
        }

    public:
        obj_ptr<ExecType> m_exec_retVal;

    private:
        exlib::string& m_retVal;
        AsyncEvent* m_ac;
    };

    AsyncShell* as = new AsyncShell(retVal, ac);
    exlib::string cmd = ac->m_ctx[1].string();

    return exec(cmd, v8::Local<v8::Object>(), as->m_exec_retVal, as);
}

result_t child_process_base::spawnSync(exlib::string command, v8::Local<v8::Array> args,
    v8::Local<v8::Object> options, obj_ptr<SpawnSyncType>& retVal, AsyncEvent* ac)
{
    class ReadStdout : public AsyncEvent {
    public:
        ReadStdout(obj_ptr<SpawnSyncType>& retVal, AsyncEvent* ac)
            : m_codec(ac->m_ctx[0].string())
            , m_retVal(retVal)
            , m_ac(ac)
        {
            setAsync();
            ChildProcess_base* cp = ac->m_ctxo.As<ChildProcess_base>();

            cp->get_stdout(m_stdout);
            if (m_stdout) {
                m_cnt.inc();
                m_bufout = new MemoryStream();
            }

            cp->get_stderr(m_stderr);
            if (m_stderr) {
                m_cnt.inc();
                m_buferr = new MemoryStream();
            }

            m_cnt.inc();
            cp->join(m_status, this);

            if (m_stdout)
                m_stdout->copyTo(m_bufout, -1, m_szout, this);

            if (m_stderr)
                m_stderr->copyTo(m_buferr, -1, m_szerr, this);
        }

        static Variant getBuffer(obj_ptr<MemoryStream>& stream, exlib::string codec)
        {
            Variant v;

            if (!stream) {
                v.setNull();
                return v;
            }

            obj_ptr<Buffer_base> buf;

            stream->rewind();
            result_t hr = stream->cc_readAll(buf);
            if (hr == CALL_RETURN_NULL)
                v.setNull();
            else if (codec == "buffer")
                v = buf;
            else {
                exlib::string s;

                if (buf->toString(codec, 0, s) < 0)
                    v = buf;
                else
                    v = s;
            }

            return v;
        }

        virtual int32_t post(int32_t v)
        {
            if (m_cnt.dec() == 0) {
                m_retVal = new SpawnSyncType();

                ChildProcess_base* cp = m_ac->m_ctxo.As<ChildProcess_base>();

                cp->get_pid(m_retVal->pid);
                cp->get_exitCode(m_retVal->status);

                m_retVal->stdout = getBuffer(m_bufout, m_codec);
                m_retVal->stderr = getBuffer(m_buferr, m_codec);

                m_retVal->output = new NArray();
                m_retVal->output->append(Variant().setNull());
                m_retVal->output->append(m_retVal->stdout);
                m_retVal->output->append(m_retVal->stderr);

                m_ac->post(0);
                delete this;
            }

            return 0;
        }

    private:
        exlib::string m_codec;
        obj_ptr<SpawnSyncType>& m_retVal;
        AsyncEvent* m_ac;
        exlib::atomic m_cnt;

        obj_ptr<Stream_base> m_stdout;
        obj_ptr<MemoryStream> m_bufout;
        int64_t m_szout;

        obj_ptr<Stream_base> m_stderr;
        obj_ptr<MemoryStream> m_buferr;
        int64_t m_szerr;

        int32_t m_status;
    };

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);
        exlib::string cmd;
        v8::Local<v8::Value> opts_;
        v8::Local<v8::Object> opts;
        obj_ptr<ChildProcess_base> cp;

        util_base::clone(options, opts_);

        opts = opts_.As<v8::Object>();

        exlib::string codec("buffer");
        GetConfigValue(isolate, opts, "encoding", codec);

        result_t hr = spawn(command, args, opts, cp);
        if (hr < 0) {
            retVal = new SpawnSyncType();

            retVal->pid = 0;
            retVal->status = 0;

            retVal->stdout.setNull();
            retVal->stderr.setNull();

            retVal->output = new NArray();
            retVal->output->append(Variant().setNull());
            retVal->output->append(retVal->stdout);
            retVal->output->append(retVal->stderr);

            retVal->error = v8::Exception::Error(
                isolate->NewString(getResultMessage(hr)));

            return 0;
        }

        ac->m_ctxo = cp;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = codec;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    new ReadStdout(retVal, ac);
    return CALL_E_PENDDING;
}

result_t child_process_base::spawnSync(exlib::string command, v8::Local<v8::Object> options,
    obj_ptr<SpawnSyncType>& retVal, AsyncEvent* ac)
{
    return spawnSync(command, v8::Local<v8::Array>(), options, retVal, ac);
}

result_t child_process_base::fork(exlib::string module, v8::Local<v8::Array> args, v8::Local<v8::Object> options, obj_ptr<ChildProcess_base>& retVal)
{
    Isolate* isolate = Isolate::current(options);
    v8::Local<v8::Context> context = isolate->context();
    exlib::string exePath;
    v8::Local<v8::Array> args1 = v8::Array::New(isolate->m_isolate);

    process_base::get_execPath(exePath);
    args1->Set(context, 0, isolate->NewString(module)).IsJust();
    if (!args.IsEmpty()) {
        int32_t len = args->Length();
        for (int32_t i = 0; i < len; i++)
            args1->Set(context, i + 1, JSValue(args->Get(context, i))).IsJust();
    }

    obj_ptr<ChildProcess> cp = new ChildProcess();
    retVal = cp;

    return cp->spawn(exePath, args1, options, true);
}

result_t child_process_base::fork(exlib::string module, v8::Local<v8::Object> options, obj_ptr<ChildProcess_base>& retVal)
{
    return fork(module, v8::Local<v8::Array>(), options, retVal);
}

result_t child_process_base::run(exlib::string command, v8::Local<v8::Array> args,
    v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);
        v8::Local<v8::Context> context = isolate->context();
        exlib::string cmd;
        v8::Local<v8::Value> opts_;
        v8::Local<v8::Object> opts;
        obj_ptr<ChildProcess_base> cp;

        util_base::clone(options, opts_);

        opts = opts_.As<v8::Object>();
        opts->Set(context, isolate->NewString("stdio"), isolate->NewString("inherit")).IsJust();

        result_t hr = spawn(command, args, opts, cp);
        if (hr < 0)
            return hr;

        ac->m_ctxo = cp;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<ChildProcess_base> cp = ac->m_ctxo.As<ChildProcess_base>();
    return cp->join(retVal, ac);
}

result_t child_process_base::run(exlib::string command, v8::Local<v8::Object> options,
    int32_t& retVal, AsyncEvent* ac)
{
    return run(command, v8::Local<v8::Array>(), options, retVal, ac);
}
}