/*
 * JSHandler.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "JSHandler.h"
#include "Fiber.h"
#include "SandBox.h"
#include "ifs/Message.h"
#include "ifs/global.h"
#include "ifs/mq.h"
#include "ifs/console.h"
#include "AsyncWaitHandler.h"

namespace fibjs {

static void _done(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ((AsyncWaitHandler*)Handler_base::getInstance(args.Data()->ToObject()))->end();
    args.GetReturnValue().SetUndefined();
}

JSHandler::JSHandler(v8::Local<v8::Value> proc, bool async)
    : m_async(async)
{
    SetPrivate("handler", proc);
}

result_t JSHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isAsync())
        return CHECK_ERROR(CALL_E_NOASYNC);

    v8::Local<v8::Object> o = v->wrap();
    Isolate* isolate = holder();
    obj_ptr<Message_base> msg = Message_base::getInstance(v);
    v8::Local<v8::Value> a = v8::Local<v8::Value>::New(isolate->m_isolate, o);

    if (m_async) {
        v8::Local<v8::Value> v1 = GetPrivate("handler");
        if (IsEmpty(v1))
            return CALL_RETURN_NULL;

        retVal = new AsyncWaitHandler();
        v8::Local<v8::Function> proc = v8::Local<v8::Function>::Cast(v1);

        obj_ptr<NArray> params;
        std::vector<v8::Local<v8::Value>> argv;
        int32_t len = 0, i;

        if (msg != NULL) {
            msg->get_params(params);
            params->get_length(len);
        }

        argv.resize(len + 2);

        argv[0] = a;
        for (i = 0; i < len; i++) {
            Variant v;
            params->_indexed_getter(i, v);
            argv[i + 1] = v;
        }
        argv[len + 1] = isolate->NewFunction("done", _done, retVal->wrap());

        proc->Call(v8::Undefined(isolate->m_isolate), len + 2, argv.data());
        return 0;
    }

    v8::Local<v8::Value> hdlr = GetPrivate("handler");

    while (true) {
        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(hdlr);
        obj_ptr<NArray> params;
        std::vector<v8::Local<v8::Value>> argv;
        v8::Local<v8::Value>* pargv;
        int32_t len = 0, i;

        if (msg != NULL) {
            msg->get_params(params);
            params->get_length(len);
        }

        if (len > 0) {
            argv.resize(len + 1);
            argv[0] = a;

            for (i = 0; i < len; i++) {
                Variant v;
                params->_indexed_getter(i, v);
                argv[i + 1] = v;
            }

            pargv = argv.data();
        } else
            pargv = &a;

        {
            TryCatch try_catch;
            hdlr = func->Call(v8::Undefined(isolate->m_isolate), len + 1, pargv);
            if (try_catch.HasCaught()) {
                v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
                    isolate->m_isolate, 1, v8::StackTrace::kScriptId);
                if (stackTrace->GetFrameCount() > 0) {
                    try_catch.ReThrow();
                    return CALL_E_JAVASCRIPT;
                } else
                    return CHECK_ERROR(Runtime::setError(GetException(try_catch, 0)));
            }
        }

        if (IsEmpty(hdlr))
            return CALL_RETURN_NULL;

        if (!hdlr->IsFunction()) {
            if (hdlr->IsObject())
                return JSHandler::New(hdlr, retVal);
            return CALL_RETURN_NULL;
        }
    }

    return 0;
}

} /* namespace fibjs */
