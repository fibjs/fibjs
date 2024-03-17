/*
 * TLSHandler.cpp
 *
 *  Created on: Mar 14, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/tls.h"
#include "ifs/mq.h"
#include "TLSHandler.h"
#include "TLSSocket.h"

namespace fibjs {

result_t TLSHandler_base::_new(SecureContext_base* context, Handler_base* handler, obj_ptr<TLSHandler_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new TLSHandler(context);
    retVal->wrap(This);
    retVal->set_handler(handler);

    return 0;
}

result_t TLSHandler_base::_new(v8::Local<v8::Object> options, Handler_base* handler, obj_ptr<TLSHandler_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<SecureContext_base> ctx;
    result_t hr = tls_base::createSecureContext(options, true, ctx);
    if (hr < 0)
        return hr;

    return _new(ctx, handler, retVal, This);
}

result_t TLSHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(TLSHandler* pThis, Stream_base* stm, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
        {
            m_socket = new TLSSocket();
            m_socket->init(m_pThis->m_ctx);

            next(accept);
        }

        ON_STATE(asyncInvoke, accept)
        {
            return m_socket->accept(m_stm, next(invoke));
        }

        ON_STATE(asyncInvoke, invoke)
        {
            return mq_base::invoke(m_pThis->m_handler, m_socket, next(close));
        }

        ON_STATE(asyncInvoke, close)
        {
            return m_socket->close(next(exit));
        }

        ON_STATE(asyncInvoke, exit)
        {
            return next(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<TLSHandler> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<TLSSocket> m_socket;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stream_base> stm = Stream_base::getInstance(v);
    if (stm == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, stm, ac))->post(0);
}

result_t TLSHandler::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    retVal = m_ctx;
    return 0;
}

result_t TLSHandler::get_handler(obj_ptr<Handler_base>& retVal)
{
    retVal = m_handler;
    return 0;
}

result_t TLSHandler::set_handler(Handler_base* newVal)
{
    SetPrivate("handler", newVal->wrap());
    m_handler = newVal;

    return 0;
}
}
