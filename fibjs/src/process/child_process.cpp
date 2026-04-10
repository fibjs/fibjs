/*
 * child_process.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "ChildProcess.h"
#include "MemoryStream.h"
#include "encoding.h"
#include "ifs/process.h"

namespace fibjs {

DECLARE_MODULE(child_process);

// Helper function to throw execSync/execFileSync error when child process exits with non-zero code
static void throwExecSyncError(const exlib::string& command, int32_t exitCode,
    const Variant& stdout_val, const Variant& stderr_val, v8::Local<v8::Object> options)
{
    Isolate* isolate = Isolate::current(options);
    v8::Local<v8::Context> context = isolate->context();

    exlib::string error_msg = "Command failed: " + command;
    v8::Local<v8::Object> error_obj = v8::Exception::Error(
        isolate->NewString(error_msg))
                                          ->ToObject(context)
                                          .ToLocalChecked();

    // Set error properties to match Node.js behavior
    error_obj->Set(context, isolate->NewString("status"),
                 v8::Integer::New(isolate->m_isolate, exitCode))
        .IsJust();
    error_obj->Set(context, isolate->NewString("signal"), v8::Null(isolate->m_isolate)).IsJust();
    error_obj->Set(context, isolate->NewString("stdout"), stdout_val).IsJust();
    error_obj->Set(context, isolate->NewString("stderr"), stderr_val).IsJust();

    // Create output array [null, stdout, stderr]
    v8::Local<v8::Array> output = v8::Array::New(isolate->m_isolate, 3);
    output->Set(context, 0, v8::Null(isolate->m_isolate)).IsJust();
    output->Set(context, 1, stdout_val).IsJust();
    output->Set(context, 2, stderr_val).IsJust();
    error_obj->Set(context, isolate->NewString("output"), output).IsJust();

    // Throw the error
    isolate->m_isolate->ThrowException(error_obj);
}

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
        // Helper to write input to stdin then close it
        class WriteStdin : public AsyncEvent {
        public:
            WriteStdin(obj_ptr<Stream_base> stdin_stream, obj_ptr<Buffer_base> buf, ReadStdout* parent)
                : m_stdin(stdin_stream)
                , m_buf(buf)
                , m_parent(parent)
                , m_phase(0)
            {
                setAsync();
                m_stdin->writeBuffer(m_buf, this);
            }

            virtual int32_t post(int32_t v)
            {
                if (m_phase == 0) {
                    m_phase = 1;
                    m_stdin->close(this);
                } else {
                    m_parent->post(0);
                    delete this;
                }
                return 0;
            }

        private:
            obj_ptr<Stream_base> m_stdin;
            obj_ptr<Buffer_base> m_buf;
            ReadStdout* m_parent;
            int32_t m_phase;
        };

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

            // Write input to stdin if provided
            if (ac->m_ctx.size() > 1) {
                obj_ptr<Buffer_base> input_buf = (Buffer_base*)ac->m_ctx[1].object();
                if (input_buf) {
                    obj_ptr<Stream_base> stdin_stream;
                    m_cp->get_stdin(stdin_stream);
                    if (stdin_stream) {
                        m_cnt.inc();
                        new WriteStdin(stdin_stream, input_buf, this);
                    }
                }
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
        exlib::string cmd;
        v8::Local<v8::Value> opts_;
        v8::Local<v8::Object> opts;
        obj_ptr<ChildProcess_base> cp;

        util_base::clone(options, opts_);

        opts = opts_.As<v8::Object>();

        exlib::string codec("utf8");
        GetConfigValue(opts, "encoding", codec);

        // Extract input option (string or Buffer)
        obj_ptr<Buffer_base> input_buf;
        if (!opts.IsEmpty()) {
            Isolate* isolate = Isolate::current(opts);
            JSValue input_val = opts->Get(isolate->context(), isolate->NewString("input"));
            if (!input_val.IsEmpty() && !input_val->IsUndefined() && !input_val->IsNull()) {
                if (input_val->IsString()) {
                    exlib::string input_str;
                    GetArgumentValue(isolate, input_val, input_str);
                    Buffer_base::_new(input_str, "utf8", input_buf);
                } else {
                    GetArgumentValue(isolate, input_val, input_buf);
                }
            }
        }

        result_t hr = spawn(command, args, opts, cp);
        if (hr < 0)
            return hr;

        ac->m_ctxo = cp;
        ac->m_ctx.resize(input_buf ? 2 : 1);
        ac->m_ctx[0] = codec;
        if (input_buf)
            ac->m_ctx[1] = input_buf;

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
        if (!options.IsEmpty() && !options->Has(context, windowsVerbatimArguments).FromMaybe(false)) {
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

result_t ChildProcess::async_spawn(exlib::string command, v8::Local<v8::Array> args,
    v8::Local<v8::Object> options, obj_ptr<child_process_base::SpawnSyncType>& retVal, AsyncEvent* ac)
{
    class ReadStdout : public AsyncEvent {
    public:
        // Helper to write input to stdin then close it
        class WriteStdin : public AsyncEvent {
        public:
            WriteStdin(obj_ptr<Stream_base> stdin_stream, obj_ptr<Buffer_base> buf, ReadStdout* parent)
                : m_stdin(stdin_stream)
                , m_buf(buf)
                , m_parent(parent)
                , m_phase(0)
            {
                setAsync();
                m_stdin->writeBuffer(m_buf, this);
            }

            virtual int32_t post(int32_t v)
            {
                if (m_phase == 0) {
                    m_phase = 1;
                    m_stdin->close(this);
                } else {
                    m_parent->post(0);
                    delete this;
                }
                return 0;
            }

        private:
            obj_ptr<Stream_base> m_stdin;
            obj_ptr<Buffer_base> m_buf;
            ReadStdout* m_parent;
            int32_t m_phase;
        };

        ReadStdout(obj_ptr<child_process_base::SpawnSyncType>& retVal, AsyncEvent* ac)
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

            // Write input to stdin if provided
            if (ac->m_ctx.size() > 1) {
                obj_ptr<Buffer_base> input_buf = (Buffer_base*)ac->m_ctx[1].object();
                if (input_buf) {
                    obj_ptr<Stream_base> stdin_stream;
                    cp->get_stdin(stdin_stream);
                    if (stdin_stream) {
                        m_cnt.inc();
                        new WriteStdin(stdin_stream, input_buf, this);
                    }
                }
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
                if (codec != "buffer")
                    v = exlib::string("");
                else
                    v.setNull();
                return v;
            }

            obj_ptr<Buffer_base> buf;

            stream->rewind();
            result_t hr = stream->cc_readAll(buf);
            if (hr == CALL_RETURN_NULL) {
                if (codec != "buffer")
                    v = exlib::string("");
                else
                    v.setNull();
            } else if (codec == "buffer")
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
                m_retVal = new child_process_base::SpawnSyncType();

                ChildProcess_base* cp = m_ac->m_ctxo.As<ChildProcess_base>();

                cp->get_pid(m_retVal->pid);
                cp->get_exitCode(m_retVal->status);

                if (m_retVal->status < 0) {
                    m_retVal->signal = signo_string(-m_retVal->status);
                    m_retVal->status = 0;
                } else {
                    m_retVal->signal.setNull();
                }

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
        obj_ptr<child_process_base::SpawnSyncType>& m_retVal;
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
        GetConfigValue(opts, "encoding", codec);

        // Extract input option (string or Buffer)
        obj_ptr<Buffer_base> input_buf;
        {
            JSValue input_val = opts->Get(isolate->context(), isolate->NewString("input"));
            if (!input_val.IsEmpty() && !input_val->IsUndefined() && !input_val->IsNull()) {
                if (input_val->IsString()) {
                    exlib::string input_str;
                    GetArgumentValue(isolate, input_val, input_str);
                    Buffer_base::_new(input_str, "utf8", input_buf);
                } else {
                    GetArgumentValue(isolate, input_val, input_buf);
                }
            }
        }

        result_t hr = child_process_base::spawn(command, args, opts, cp);
        if (hr < 0) {
            retVal = new child_process_base::SpawnSyncType();

            retVal->pid = 0;
            retVal->status = 0;

            retVal->stdout.setNull();
            retVal->stderr.setNull();
            retVal->signal.setNull();

            retVal->output = new NArray();
            retVal->output->append(Variant().setNull());
            retVal->output->append(retVal->stdout);
            retVal->output->append(retVal->stderr);

            retVal->error = v8::Exception::Error(
                isolate->NewString(getResultMessage(hr)));

            return 0;
        }

        ac->m_ctxo = cp;
        ac->m_ctx.resize(input_buf ? 2 : 1);
        ac->m_ctx[0] = codec;
        if (input_buf)
            ac->m_ctx[1] = input_buf;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    new ReadStdout(retVal, ac);
    return CALL_E_PENDDING;
}

result_t child_process_base::spawnSync(exlib::string command, v8::Local<v8::Array> args,
    v8::Local<v8::Object> options, obj_ptr<SpawnSyncType>& retVal)
{
    return ChildProcess::ac_async_spawn(command, args, options, retVal);
}

result_t child_process_base::spawnSync(exlib::string command, v8::Local<v8::Object> options,
    obj_ptr<SpawnSyncType>& retVal)
{
    return spawnSync(command, v8::Local<v8::Array>(), options, retVal);
}

result_t child_process_base::execSync(exlib::string command, v8::Local<v8::Object> options, Variant& retVal)
{
    obj_ptr<ExecType> exec_retVal;
    result_t hr = ac_exec(command, options, exec_retVal);
    if (hr < 0)
        return hr;

    // Check if child process exited with non-zero code
    if (exec_retVal->exitCode != 0) {
        throwExecSyncError(command, exec_retVal->exitCode,
            exec_retVal->stdout, exec_retVal->stderr, options);
        return CALL_E_JAVASCRIPT;
    } else {
        retVal = exec_retVal->stdout;
    }

    return 0;
}

result_t child_process_base::execFileSync(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> options, Variant& retVal)
{
    obj_ptr<ExecFileType> exec_retVal;
    result_t hr = ac_execFile(command, args, options, exec_retVal);
    if (hr < 0)
        return hr;

    // Check if child process exited with non-zero code
    if (exec_retVal->exitCode != 0) {
        throwExecSyncError(command, exec_retVal->exitCode,
            exec_retVal->stdout, exec_retVal->stderr, options);
        return CALL_E_JAVASCRIPT;
    } else {
        retVal = exec_retVal->stdout;
    }

    return 0;
}

result_t child_process_base::execFileSync(exlib::string command, v8::Local<v8::Object> options, Variant& retVal)
{
    return execFileSync(command, v8::Local<v8::Array>(), options, retVal);
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
    class WaitJoin : public AsyncEvent {
    public:
        WaitJoin(int32_t& retVal, AsyncEvent* ac)
            : m_retVal(retVal)
            , m_ac(ac)
        {
            setAsync();
            m_cp = ac->m_ctxo.As<ChildProcess_base>();

            m_cp->join(m_status, this);
        }

        virtual int32_t post(int32_t v)
        {
            m_cp->get_exitCode(m_retVal);

            m_ac->post(0);
            delete this;

            return 0;
        }

    private:
        obj_ptr<ChildProcess_base> m_cp;
        int32_t m_status;
        int32_t& m_retVal;
        AsyncEvent* m_ac;
    };

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

    new WaitJoin(retVal, ac);
    return CALL_E_PENDDING;
}

result_t child_process_base::run(exlib::string command, v8::Local<v8::Object> options,
    int32_t& retVal, AsyncEvent* ac)
{
    return run(command, v8::Local<v8::Array>(), options, retVal, ac);
}
}