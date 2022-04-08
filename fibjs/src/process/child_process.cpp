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
#include "MemoryStream.h"
#include "encoding.h"
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
            ChildProcess_base* cp = (ChildProcess_base*)(object_base*)ac->m_ctxo;

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
        }

        void start()
        {
            if (m_cnt == 0) {
                m_retVal = new ExecFileType();
                m_ac->post(0);
                delete this;
                return;
            }

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
                exlib::string b;
                exlib::string s;

                buf->toString(b);
                if (commonEncode(codec, b, s) < 0)
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

                m_ac->post(0);
                delete this;
            }

            return 0;
        }

    private:
        exlib::string m_codec;
        obj_ptr<ExecFileType>& m_retVal;
        AsyncEvent* m_ac;
        exlib::atomic m_cnt;

        obj_ptr<Stream_base> m_stdout;
        obj_ptr<MemoryStream> m_bufout;
        int64_t m_szout;

        obj_ptr<Stream_base> m_stderr;
        obj_ptr<MemoryStream> m_buferr;
        int64_t m_szerr;
    };

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current();
        exlib::string cmd;
        v8::Local<v8::Value> opts_;
        v8::Local<v8::Object> opts;
        obj_ptr<ChildProcess_base> cp;

        util_base::clone(options, opts_);

        opts = v8::Local<v8::Object>::Cast(opts_);
        opts->Delete(opts->CreationContext(), isolate->NewString("stdio"));

        exlib::string codec("utf8");
        GetConfigValue(isolate->m_isolate, opts, "encoding", codec);

        result_t hr = spawn(command, args, opts, cp);
        if (hr < 0)
            return hr;

        ac->m_ctxo = cp;
        ac->m_ctx.resize(1);
        ac->m_ctx[0] = codec;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    (new ReadStdout(retVal, ac))->start();
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
    obj_ptr<ExecFileType>& _retVal = *((obj_ptr<ExecFileType>*)&retVal);

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();
        obj_ptr<NArray> _args;
        exlib::string cmd;
        v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate);
        size_t i;

        util_base::parseArgs(command, _args);
        if (_args->m_array.size()) {
            cmd = _args->m_array[0].string();
            for (i = 1; i < _args->m_array.size(); i++)
                args->Set(context, (int32_t)i - 1, _args->m_array[i]);
        }

        return execFile(cmd, args, options, _retVal, ac);
    }

    return execFile(command, v8::Local<v8::Array>(), options, _retVal, ac);
}

result_t child_process_base::fork(exlib::string module, v8::Local<v8::Array> args, v8::Local<v8::Object> options, obj_ptr<ChildProcess_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    exlib::string exePath;
    v8::Local<v8::Array> args1 = v8::Array::New(isolate->m_isolate);

    process_base::get_execPath(exePath);
    args1->Set(context, 0, isolate->NewString(module));
    if (!args.IsEmpty()) {
        int32_t len = args->Length();
        for (int32_t i = 0; i < len; i++)
            args1->Set(context, i + 1, JSValue(args->Get(context, i)));
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
    class WaitExitCode : public AsyncEvent {
    public:
        WaitExitCode(ChildProcess_base* cp, int32_t& retVal, AsyncEvent* ac)
            : m_this(cp)
            , m_retVal(retVal)
            , m_ac(ac)
        {
            setAsync();
        }

        virtual int32_t post(int32_t v)
        {
            m_this->get_exitCode(m_retVal);

            m_ac->post(v);
            delete this;

            return 0;
        }

    private:
        obj_ptr<ChildProcess_base> m_this;
        int32_t& m_retVal;
        AsyncEvent* m_ac;
    };

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();
        exlib::string cmd;
        v8::Local<v8::Value> opts_;
        v8::Local<v8::Object> opts;
        obj_ptr<ChildProcess_base> cp;

        util_base::clone(options, opts_);

        opts = v8::Local<v8::Object>::Cast(opts_);
        opts->Set(context, isolate->NewString("stdio"), isolate->NewString("inherit"));

        result_t hr = spawn(command, args, opts, cp);
        if (hr < 0)
            return hr;

        ac->m_ctxo = cp;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<ChildProcess_base> cp = (ChildProcess*)(object_base*)ac->m_ctxo;
    AsyncEvent* _ac = new WaitExitCode(cp, retVal, ac);
    result_t hr = cp->join(_ac);
    if (hr != CALL_E_PENDDING) {
        cp->get_exitCode(retVal);
        delete _ac;
    }

    return hr;
}

result_t child_process_base::run(exlib::string command, v8::Local<v8::Object> options,
    int32_t& retVal, AsyncEvent* ac)
{
    return run(command, v8::Local<v8::Array>(), options, retVal, ac);
}
}