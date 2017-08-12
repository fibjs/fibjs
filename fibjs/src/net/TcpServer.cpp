/*
 * TcpServer.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "object.h"
#include "TcpServer.h"
#include "ifs/mq.h"
#include "ifs/console.h"

namespace fibjs {

result_t _new_tcpServer(exlib::string addr, int32_t port,
    Handler_base* listener, obj_ptr<TcpServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<TcpServer> svr = new TcpServer();
    svr->wrap(This);

    result_t hr = svr->create(addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t TcpServer_base::_new(int32_t port, Handler_base* listener,
    obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This)
{
    return _new("", port, listener, retVal, This);
}

result_t TcpServer_base::_new(exlib::string addr, int32_t port,
    Handler_base* listener, obj_ptr<TcpServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new_tcpServer(addr, port, listener, retVal, This);
}

static const char* s_staticCounter[] = { "total", "connections" };
static const char* s_Counter[] = { "accept", "close" };

enum {
    TCPS_TOTAL = 0,
    TCPS_CONNECTIONS,
    TCPS_ACCEPT,
    TCPS_CLOSE
};

TcpServer::TcpServer()
{
    m_stats = new Stats();
    m_stats->init(s_staticCounter, 2, s_Counter, 2);
    m_running = false;
}

result_t TcpServer::create(exlib::string addr, int32_t port,
    Handler_base* listener)
{
    result_t hr;

    hr = Socket_base::_new(net_base::_AF_INET, net_base::_SOCK_STREAM,
        m_socket);
    if (hr < 0)
        return hr;

    hr = m_socket->bind(addr, port, false);
    if (hr < 0)
        return hr;

    hr = m_socket->listen(1024);
    if (hr < 0)
        return hr;

    set_handler(listener);

    return 0;
}

result_t TcpServer::run(AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(TcpServer* pThis, Socket_base* pSock, object_base* holder)
            : AsyncState(NULL)
            , m_pThis(pThis)
            , m_sock(pSock)
            , m_holder(holder)
        {
            set(invoke);
        }

    public:
        static int32_t invoke(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            pThis->set(close);
            return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_sock, pThis);
        }

        static int32_t close(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;

            pThis->done();
            pThis->m_pThis->m_stats->inc(TCPS_CLOSE);
            pThis->m_pThis->m_stats->dec(TCPS_CONNECTIONS);
            return pThis->m_sock->close(pThis);
        }

        virtual int32_t error(int32_t v)
        {
            errorLog("TcpServer: " + getResultMessage(v));
            set(close);
            return 0;
        }

    private:
        obj_ptr<TcpServer> m_pThis;
        obj_ptr<Socket_base> m_sock;
        obj_ptr<Handler_base> m_hdlr;
        obj_ptr<object_base> m_holder;
    };

    class asyncAccept : public AsyncState {
    public:
        asyncAccept(TcpServer* pThis, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_holder(ac->m_ctx[0].object())
        {
            set(accept);
        }

    public:
        static int32_t accept(AsyncState* pState, int32_t n)
        {
            asyncAccept* pThis = (asyncAccept*)pState;

            pThis->set(invoke);
            return pThis->m_pThis->m_socket->accept(pThis->m_retVal, pThis);
        }

        static int32_t invoke(AsyncState* pState, int32_t n)
        {
            asyncAccept* pThis = (asyncAccept*)pState;

            pThis->m_pThis->m_stats->inc(TCPS_TOTAL);
            pThis->m_pThis->m_stats->inc(TCPS_ACCEPT);
            pThis->m_pThis->m_stats->inc(TCPS_CONNECTIONS);

            if (pThis->m_retVal) {
                (new asyncInvoke(pThis->m_pThis, pThis->m_retVal, pThis->m_holder))->apost(0);
                pThis->m_retVal.Release();
            }

            return pThis->m_pThis->m_socket->accept(pThis->m_retVal, pThis);
        }

        virtual int32_t error(int32_t v)
        {
            if (v == CALL_E_BAD_FILE || v == CALL_E_INVALID_CALL)
                return v;

            errorLog("TcpServer: " + getResultMessage(v));
            return 0;
        }

    private:
        obj_ptr<TcpServer> m_pThis;
        obj_ptr<Socket_base> m_retVal;
        obj_ptr<object_base> m_holder;
    };

    if (ac->isSync()) {
        obj_ptr<Holder> h = new Holder();
        h->setJSObject();
        h->m_server.Reset(holder()->m_isolate, wrap());

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = h;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_running)
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    m_running = true;

    return (new asyncAccept(this, ac))->post(0);
}

result_t TcpServer::asyncRun()
{
    class asyncCall : public AsyncState {
    public:
        asyncCall(TcpServer* pThis)
            : AsyncState(NULL)
            , m_pThis(pThis)
        {
            set(accept);
        }

    public:
        static int32_t accept(AsyncState* pState, int32_t n)
        {
            asyncCall* pThis = (asyncCall*)pState;
            return pThis->m_pThis->run(pThis);
        }

    private:
        obj_ptr<TcpServer> m_pThis;
    };

    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_running)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    AsyncEvent* ac = new asyncCall(this);

    obj_ptr<Holder> h = new Holder();
    h->setJSObject();
    h->m_server.Reset(holder()->m_isolate, wrap());

    ac->m_ctx.resize(1);
    ac->m_ctx[0] = h;

    ac->apost(0);
    return 0;
}

result_t TcpServer::stop(AsyncEvent* ac)
{
    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_socket->close(ac);
}

result_t TcpServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_socket;
    return 0;
}

result_t TcpServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    retVal = m_hdlr;
    return 0;
}

result_t TcpServer::set_handler(Handler_base* newVal)
{
    SetPrivate("handler", newVal->wrap());
    m_hdlr = newVal;

    return 0;
}

result_t TcpServer::get_stats(obj_ptr<Stats_base>& retVal)
{
    retVal = m_stats;
    return 0;
}

} /* namespace fibjs */
