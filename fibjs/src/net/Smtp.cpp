/*
 * Smtp.cpp
 *
 *  Created on: Dec 5, 2013
 *      Author: lion
 */

#include "Smtp.h"
#include "Buffer.h"
#include "ifs/encoding.h"

namespace fibjs
{

result_t Smtp_base::_new(obj_ptr<Smtp_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new Smtp();
    return 0;
}

#define SMTP_MAX_LINE 1024

class asyncSmtp: public AsyncState
{
public:
    asyncSmtp(Smtp *pThis, std::string &retVal, AsyncEvent *ac) :
        AsyncState(ac), m_pThis(pThis), m_retVal(retVal)
    {
        m_stmBuffered = pThis->m_stmBuffered;
        set(ok);
    }

    asyncSmtp(Smtp *pThis, AsyncEvent *ac) :
        AsyncState(ac), m_pThis(pThis), m_retVal(m_strLine)
    {
        m_stmBuffered = pThis->m_stmBuffered;
        set(ok);
    }

    static int32_t ok(AsyncState *pState, int32_t n)
    {
        asyncSmtp *pThis = (asyncSmtp *) pState;

        pThis->set(recv);
        return pThis->m_stmBuffered->readLine(SMTP_MAX_LINE, pThis->m_strLine,
                                              pThis);
    }

    static int32_t recv(AsyncState *pState, int32_t n)
    {
        asyncSmtp *pThis = (asyncSmtp *) pState;

        std::string &strLine = pThis->m_strLine;

        if (&pThis->m_retVal != &strLine)
        {
            if (pThis->m_retVal.length())
                pThis->m_retVal.append("\r\n", 2);
            pThis->m_retVal.append(strLine);
        }

        if (qisdigit(strLine[0]) && qisdigit(strLine[1]) && qisdigit(strLine[2])
                && strLine[3] == ' ')
        {
            if (strLine[0] == '5')
                return CHECK_ERROR(Runtime::setError(strLine));

            return pThis->recv_ok();
        }

        return pThis->m_stmBuffered->readLine(SMTP_MAX_LINE, strLine, pThis);
    }

    int32_t connect(const char *host, int32_t port)
    {
        int32_t r = m_pThis->m_sock->connect(host, port, this);

        if (r == CALL_E_PENDDING)
            return r;
        return post(r);
    }

    int32_t send(Buffer_base *data)
    {
        int32_t r = m_stmBuffered->write(data, this);

        if (r == CALL_E_PENDDING)
            return r;
        return post(r);
    }

    int32_t command(const char *cmd, const char *arg)
    {
        std::string s(cmd);

        s.append(" ", 1);
        s.append(arg);
        s.append("\r\n", 2);

        obj_ptr<Buffer> buf = new Buffer(s);
        return send(buf);
    }

    virtual int32_t recv_ok()
    {
        return done();
    }

protected:
    obj_ptr<Smtp> m_pThis;
    std::string &m_retVal;
    obj_ptr<BufferedStream_base> m_stmBuffered;
    std::string m_strLine;
};

result_t Smtp::connect(const char *host, int32_t port, int32_t family,
                       AsyncEvent *ac)
{
    if (m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (family != net_base::_AF_INET && family != net_base::_AF_INET6)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = Socket_base::_new(family, net_base::_SOCK_STREAM, m_sock);
    if (hr < 0)
        return hr;

    m_stmBuffered = new BufferedStream(m_sock);
    m_stmBuffered->set_EOL("\r\n");

    return (new asyncSmtp(this, ac))->connect(host, port);
}

result_t Smtp::command(const char *cmd, const char *arg, std::string &retVal,
                       AsyncEvent *ac)
{
    if (!m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSmtp(this, retVal, ac))->command(cmd, arg);
}

result_t Smtp::command(const char *cmd, const char *arg, AsyncEvent *ac)
{
    if (!m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSmtp(this, ac))->command(cmd, arg);
}

result_t Smtp::hello(const char *hostname, AsyncEvent *ac)
{
    return command("HELO", hostname, ac);
}

result_t Smtp::login(const char *username, const char *password,
                     AsyncEvent *ac)
{
    class asyncLogin: public asyncSmtp
    {
    public:
        asyncLogin(Smtp *pThis, const char *username, const char *password,
                   AsyncEvent *ac) :
            asyncSmtp(pThis, ac), m_username(username), m_password(
                password), step(0)
        {
            set(begin);
        }

        static int32_t begin(AsyncState *pState, int32_t n)
        {
            asyncLogin *pThis = (asyncLogin *) pState;

            std::string s("AUTH LOGIN\r\n", 12);

            obj_ptr<Buffer> buf = new Buffer(s);
            pThis->set(ok);
            return pThis->m_stmBuffered->write(buf, pThis);
        }

        int32_t send_base64(const char *str)
        {
            obj_ptr<Buffer> buf = new Buffer(str);
            std::string s;

            encoding_base::base64Encode(buf, s);
            s.append("\r\n", 2);

            buf = new Buffer(s);

            set (ok);
            return m_stmBuffered->write(buf, this);
        }

        static int32_t send_username(AsyncState *pState, int32_t n)
        {
            asyncLogin *pThis = (asyncLogin *) pState;
            return pThis->send_base64(pThis->m_username);
        }

        static int32_t send_password(AsyncState *pState, int32_t n)
        {
            asyncLogin *pThis = (asyncLogin *) pState;
            return pThis->send_base64(pThis->m_password);
        }

        virtual int32_t recv_ok()
        {
            switch (step)
            {
            case 0:
                step++;
                set(send_username);
                return 0;
            case 1:
                step++;
                set(send_password);
                return 0;
            }
            return done();
        }

    private:
        const char *m_username;
        const char *m_password;
        int32_t step;
    };

    if (!m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncLogin(this, username, password, ac))->post(0);
}

result_t Smtp::from(const char *address, AsyncEvent *ac)
{
    return command("MAIL FROM", address, ac);
}

result_t Smtp::to(const char *address, AsyncEvent *ac)
{
    return command("RCPT TO", address, ac);
}

result_t Smtp::data(const char *txt, AsyncEvent *ac)
{
    class asyncData: public asyncSmtp
    {
    public:
        asyncData(Smtp *pThis, const char *txt, AsyncEvent *ac) :
            asyncSmtp(pThis, ac), m_txt(txt), step(0)
        {
            set(begin);
        }

        static int32_t begin(AsyncState *pState, int32_t n)
        {
            asyncData *pThis = (asyncData *) pState;

            std::string s("DATA\r\n", 6);

            obj_ptr<Buffer> buf = new Buffer(s);
            pThis->set(ok);
            return pThis->m_stmBuffered->write(buf, pThis);
        }

        static int32_t send_data(AsyncState *pState, int32_t n)
        {
            asyncData *pThis = (asyncData *) pState;

            std::string s(pThis->m_txt);

            s.append("\r\n.\r\n", 5);

            obj_ptr<Buffer> buf = new Buffer(s);
            pThis->set(ok);
            return pThis->m_stmBuffered->write(buf, pThis);
        }

        virtual int32_t recv_ok()
        {
            switch (step)
            {
            case 0:
                step++;
                set(send_data);
                return 0;
            }
            return done();
        }

    private:
        const char *m_txt;
        int32_t step;
    };

    if (!m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncData(this, txt, ac))->post(0);
}

result_t Smtp::quit(AsyncEvent *ac)
{
    return command("QUIT", "", ac);
}

result_t Smtp::get_socket(obj_ptr<Socket_base> &retVal)
{
    retVal = m_sock;
    return 0;
}

} /* namespace fibjs */
