/*
 * JSHandler.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "JSHandler.h"
#include "Fiber.h"
#include "ifs/Message.h"
#include "ifs/global.h"
#include "ifs/mq.h"
#include "ifs/console.h"

namespace fibjs
{

inline result_t msgMethod(Message_base *msg, std::string &method)
{
    std::string str;
    const char *p, *p1;

    msg->get_value(str);

    p = p1 = str.c_str();
    while (true)
    {
        while (*p && *p != '.' && *p != '/' && *p != '\\')
            p++;
        if (p != p1)
            break;
        if (!*p)
            return CHECK_ERROR(Runtime::setError("method \"" + method + "\" not found."));
        p++;
        p1 = p;
    }

    msg->set_value(*p ? p + 1 : "");
    method.assign(p1, (int) (p - p1));

    return 0;
}

result_t JSHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                           exlib::AsyncEvent *ac)
{
    if (ac)
        return CHECK_ERROR(CALL_E_NOASYNC);

    v8::Local<v8::Object> o = v->wrap();

    obj_ptr<Message_base> msg = Message_base::getInstance(v);
    v8::Local<v8::Value> a = v8::Local<v8::Value>::New(isolate, o);
    v8::Local<v8::Value> hdlr = v8::Local<v8::Value>::New(isolate, m_handler);
    result_t hr;
    bool bResult = false;

    while (true)
    {
        if (hdlr->IsFunction())
        {
            v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(hdlr);
            obj_ptr<List_base> params;
            std::vector<v8::Local<v8::Value> > argv;
            v8::Local<v8::Value> *pargv;
            int32_t len = 0, i;

            if (msg != NULL)
            {
                msg->get_params(params);
                params->get_length(len);
            }

            if (len > 0)
            {
                argv.resize(len + 1);
                argv[0] = a;

                for (i = 0; i < len; i++)
                {
                    Variant v;
                    params->_indexed_getter(i, v);
                    argv[i + 1] = v;
                }

                pargv = argv.data();
            }
            else
                pargv = &a;

            if (!isolate->GetCallingContext().IsEmpty())
            {
                hdlr = func->Call(v8::Undefined(isolate), len + 1, pargv);
                if (hdlr.IsEmpty())
                    return CHECK_ERROR(CALL_E_JAVASCRIPT);
            }
            else
            {
                v8::TryCatch try_catch;
                hdlr = func->Call(v8::Undefined(isolate), len + 1, pargv);
                if (try_catch.HasCaught())
                    return CHECK_ERROR(Runtime::setError(GetException(try_catch, 0)));
            }

            if (IsEmpty (hdlr))
                return CALL_RETURN_NULL;

            bResult = true;
        }
        else if (hdlr->IsObject())
        {
            if ((retVal = Handler_base::getInstance(hdlr)) != NULL)
                return 0;

            if (msg == NULL)
                return CHECK_ERROR(CALL_E_BADVARTYPE);

            if (bResult)
            {
                msg->set_result(hdlr);
                return CALL_RETURN_NULL;
            }

            std::string method;
            hr = msgMethod(msg, method);
            if (hr < 0)
                return hr;

            hdlr = v8::Local<v8::Object>::Cast(hdlr)->Get(
                       v8::String::NewFromUtf8(isolate, method.c_str(),
                                               v8::String::kNormalString,
                                               (int) method.length()));
            if (IsEmpty (hdlr))
                return CHECK_ERROR(Runtime::setError("method \"" + method + "\" not found."));

            bResult = false;
        }
        else if (bResult && msg)
        {
            msg->set_result(hdlr);
            return CALL_RETURN_NULL;
        }
        else
            return CHECK_ERROR(CALL_E_INTERNAL);
    }

    return 0;
}

result_t JSHandler::js_invoke(Handler_base *hdlr, object_base *v,
                              obj_ptr<Handler_base> &retVal, exlib::AsyncEvent *ac)
{
    class asyncInvoke: public asyncState
    {
    public:
        asyncInvoke(Handler_base *pThis, object_base *v,
                    obj_ptr<Handler_base> &retVal, exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_v(v), m_retVal(retVal)
        {
            set(call);
        }

    public:
        static int call(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            int v = pThis->done(CALL_E_PENDDING);
            pThis->asyncEvent::post(0);
            return v;
        }

    public:
        virtual void js_callback()
        {
            {
                JSFiber::scope s;
                m_hr = js_invoke(m_pThis, m_v, m_retVal, NULL);
                if (m_hr == CALL_E_EXCEPTION)
                    m_message = Runtime::errMessage();
            }

            s_acPool.put(this);
        }

        virtual void invoke()
        {
            if (m_hr == CALL_E_EXCEPTION)
                Runtime::setError(m_message);
            post(m_hr);
        }

    private:
        obj_ptr<Handler_base> m_pThis;
        obj_ptr<object_base> m_v;
        obj_ptr<Handler_base> &m_retVal;
        result_t m_hr;
        std::string m_message;
    };

    if (!ac)
    {
        result_t hr;
        obj_ptr<Handler_base> hdlr1 = hdlr;
        obj_ptr<Handler_base> hdlr2;

        while (true)
        {
            hr = hdlr1->invoke(v, hdlr2, NULL);
            if (hr == CALL_E_NOSYNC)
            {
                retVal = hdlr1;
                return 0;
            }

            if (hr < 0 || hr == CALL_RETURN_NULL)
                return hr;

            hdlr1 = hdlr2;
        }

        return 0;
    }

    return (new asyncInvoke(hdlr, v, retVal, ac))->post(0);
}

} /* namespace fibjs */
