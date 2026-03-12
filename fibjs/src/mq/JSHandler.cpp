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
#include "ifs/HttpRequest.h"

namespace fibjs {

static void _done(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ((AsyncWaitHandler*)Handler_base::getInstance(args.Data()))->end();
    args.GetReturnValue().SetUndefined();
}

JSHandler::JSHandler(v8::Local<v8::Value> proc, bool async)
    : m_async(async)
{
    SetPrivate("handler", proc);
}

result_t JSHandler::isRouting(bool& retVal)
{
    retVal = false;
    return 0;
}

result_t JSHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isAsync())
        return CHECK_ERROR(CALL_E_NOASYNC);

    v8::Local<v8::Object> o = v->wrap();
    Isolate* isolate = holder();
    obj_ptr<Message_base> msg = Message_base::getInstance(v);
    v8::Local<v8::Value> a = o;

    // If the message is an HttpRequest, inject res = req.response as extra arg
    obj_ptr<HttpRequest_base> httpReq = HttpRequest_base::getInstance(v);
    v8::Local<v8::Value> resArg;
    if (httpReq != NULL) {
        obj_ptr<HttpResponse_base> httpResp;
        httpReq->get_response(httpResp);
        if (httpResp != NULL)
            resArg = httpResp->wrap();
    }
    bool hasRes = !resArg.IsEmpty();

    if (m_async) {
        v8::Local<v8::Value> v1 = GetPrivate("handler");
        if (IsEmpty(v1))
            return CALL_RETURN_NULL;

        retVal = new AsyncWaitHandler();
        JSFunction proc = v1.As<v8::Function>();

        obj_ptr<NArray> params;
        std::vector<v8::Local<v8::Value>> argv;
        int32_t len = 0, i;

        if (msg != NULL) {
            msg->get_params(params);
            len = params->length();
        }

        // argv layout: [req, ...params, res(if HttpRequest), done]
        int32_t extra = hasRes ? 1 : 0;
        argv.resize(len + 2 + extra);

        argv[0] = a;
        for (i = 0; i < len; i++) {
            Variant v;
            params->_indexed_getter(i, v);
            argv[i + 1] = v;
        }
        if (hasRes)
            argv[len + 1] = resArg;
        argv[len + 1 + extra] = isolate->NewFunction("done", _done, retVal->wrap());
        if (argv[len + 1 + extra].IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));

        proc.Call(v8::Undefined(isolate->m_isolate), len + 2 + extra, argv.data());
        return 0;
    }

    v8::Local<v8::Value> hdlr = GetPrivate("handler");

    while (hdlr->IsFunction()) {
        JSFunction func = hdlr.As<v8::Function>();
        obj_ptr<NArray> params;
        std::vector<v8::Local<v8::Value>> argv;
        v8::Local<v8::Value>* pargv;
        int32_t len = 0, i;

        if (msg != NULL) {
            msg->get_params(params);
            len = params->length();
        }

        // argv layout: [req, ...params, res(if HttpRequest)]
        int32_t extra = hasRes ? 1 : 0;
        if (len > 0 || hasRes) {
            argv.resize(len + 1 + extra);
            argv[0] = a;

            for (i = 0; i < len; i++) {
                Variant v;
                params->_indexed_getter(i, v);
                argv[i + 1] = v;
            }
            if (hasRes)
                argv[len + 1] = resArg;

            pargv = argv.data();
        } else
            pargv = &a;

        {
            TryCatch try_catch;
            hdlr = func.Call(v8::Undefined(isolate->m_isolate), len + 1 + extra, pargv);
            if (try_catch.HasCaught()) {
                v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
                    isolate->m_isolate, 1, v8::StackTrace::kScriptId);
                if (stackTrace->GetFrameCount() > 0) {
                    try_catch.ReThrow();
                    return CALL_E_JAVASCRIPT;
                } else
                    return CHECK_ERROR(Runtime::setError(GetException(try_catch, 0, false, true)));
            }
        }

        if (IsEmpty(hdlr))
            return CALL_RETURN_NULL;
    }

    return GetArgumentValue(isolate, hdlr, retVal);
}

} /* namespace fibjs */
