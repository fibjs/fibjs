/*
 * Smtp.cpp
 *
 *  Created on: Dec 5, 2013
 *      Author: lion
 */

#include "object.h"
#include "Smtp.h"
#include "Buffer.h"
#include "Url.h"
#include "ifs/encoding.h"
#include "ifs/net.h"
#include "SslSocket.h"

namespace fibjs {

result_t Smtp_base::_new(obj_ptr<Smtp_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Smtp();
    return 0;
}

#define SMTP_MAX_LINE 1024

class asyncSmtp : public AsyncState {
public:
    asyncSmtp(Smtp* pThis, exlib::string& retVal, AsyncEvent* ac)
        : AsyncState(ac)
        , m_pThis(pThis)
        , m_retVal(retVal)
    {
        m_stmBuffered = pThis->m_stmBuffered;
    }

    asyncSmtp(Smtp* pThis, AsyncEvent* ac)
        : AsyncState(ac)
        , m_pThis(pThis)
        , m_retVal(m_strLine)
    {
        m_stmBuffered = pThis->m_stmBuffered;
    }

    ON_STATE(asyncSmtp, ok)
    {
        return m_stmBuffered->readLine(SMTP_MAX_LINE, m_strLine, next(recv));
    }

    ON_STATE(asyncSmtp, recv)
    {
        exlib::string& strLine = m_strLine;

        if (&m_retVal != &strLine) {
            if (m_retVal.length())
                m_retVal.append("\r\n", 2);
            m_retVal.append(strLine);
        }

        if (qisdigit(strLine[0]) && qisdigit(strLine[1]) && qisdigit(strLine[2])
            && strLine[3] == ' ') {
            if (strLine[0] == '5')
                return CHECK_ERROR(Runtime::setError(strLine));

            return recv_ok();
        }

        return m_stmBuffered->readLine(SMTP_MAX_LINE, strLine, this);
    }

    virtual int32_t recv_ok()
    {
        return next();
    }

protected:
    obj_ptr<Smtp> m_pThis;
    exlib::string& m_retVal;
    obj_ptr<BufferedStream_base> m_stmBuffered;
    exlib::string m_strLine;
};

class asyncCommand : public asyncSmtp {
public:
    asyncCommand(Smtp* pThis, exlib::string cmd, exlib::string arg,
        exlib::string& retVal, AsyncEvent* ac)
        : asyncSmtp(pThis, retVal, ac)
    {
        exlib::string s(cmd);

        s.append(" ", 1);
        s.append(arg);
        s.append("\r\n", 2);

        m_buf = new Buffer(s);

        next(command);
    }

    asyncCommand(Smtp* pThis, exlib::string cmd, exlib::string arg, AsyncEvent* ac)
        : asyncSmtp(pThis, ac)
    {
        exlib::string s(cmd);

        s.append(" ", 1);
        s.append(arg);
        s.append("\r\n", 2);

        m_buf = new Buffer(s);

        next(command);
    }

public:
    ON_STATE(asyncCommand, command)
    {
        return m_stmBuffered->write(m_buf, next(ok));
    }

private:
    obj_ptr<Buffer> m_buf;
};

result_t Smtp::connect(exlib::string url, AsyncEvent* ac)
{
    class asyncConnect : public asyncSmtp {
    public:
        asyncConnect(Smtp* pThis, exlib::string url, AsyncEvent* ac)
            : asyncSmtp(pThis, ac)
            , m_url(url)
            , m_tls(false)
        {
            next(connect);
        }

    public:
        ON_STATE(asyncConnect, connect)
        {
            m_u = new Url();
            result_t hr;

            hr = m_u->parse(m_url);
            if (hr < 0)
                return hr;

            return net_base::connect(m_url, m_pThis->m_timeout,
                m_pThis->m_conn, next(connected));
        }

        ON_STATE(asyncConnect, connected)
        {
            m_pThis->m_stmBuffered = new BufferedStream(m_pThis->m_conn);
            m_pThis->m_stmBuffered->set_EOL("\r\n");

            m_stmBuffered = m_pThis->m_stmBuffered;
            return next(ok);
        }

        ON_STATE(asyncConnect, starttls)
        {
            m_tls = true;
            m_buf = new Buffer("STARTTLS\r\n");
            return m_stmBuffered->write(m_buf, next(ok));
        }

        ON_STATE(asyncConnect, ssl_handshake)
        {
            obj_ptr<SslSocket> ss = new SslSocket();
            obj_ptr<Stream_base> conn = m_pThis->m_conn;
            m_pThis->m_conn = ss;

            if (g_ssl.m_crt && g_ssl.m_key) {
                result_t hr = ss->setCert("", g_ssl.m_crt, g_ssl.m_key);
                if (hr < 0)
                    return hr;
            }

            return ss->connect(conn, m_u->m_hostname, m_temp, next(ssl_connected));
        }

        ON_STATE(asyncConnect, ssl_connected)
        {
            m_pThis->m_stmBuffered = new BufferedStream(m_pThis->m_conn);
            m_pThis->m_stmBuffered->set_EOL("\r\n");

            m_stmBuffered = m_pThis->m_stmBuffered;
            return next();
        }

        virtual int32_t recv_ok()
        {
            if (m_u->m_protocol == "ssl:")
                return next();

            return next(m_tls ? ssl_handshake : starttls);
        }

        virtual int32_t error(int32_t v)
        {
            if (at(recv) && m_tls)
                return next();
            return v;
        }

    private:
        exlib::string m_url;
        obj_ptr<Url> m_u;
        obj_ptr<Buffer> m_buf;
        bool m_tls;
        int32_t m_temp;
    };

    if (m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncConnect(this, url, ac))->post(0);
}

result_t Smtp::command(exlib::string cmd, exlib::string arg, exlib::string& retVal,
    AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncCommand(this, cmd, arg, retVal, ac))->post(0);
}

result_t Smtp::command(exlib::string cmd, exlib::string arg, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncCommand(this, cmd, arg, ac))->post(0);
}

result_t Smtp::hello(exlib::string hostname, AsyncEvent* ac)
{
    return command("HELO", hostname, ac);
}

result_t Smtp::login(exlib::string username, exlib::string password,
    AsyncEvent* ac)
{
    class asyncLogin : public asyncSmtp {
    public:
        asyncLogin(Smtp* pThis, exlib::string username, exlib::string password,
            AsyncEvent* ac)
            : asyncSmtp(pThis, ac)
            , m_username(username)
            , m_password(
                  password)
            , step(0)
        {
            next(begin);
        }

        ON_STATE(asyncLogin, begin)
        {
            exlib::string s("AUTH LOGIN\r\n", 12);

            obj_ptr<Buffer> buf = new Buffer(s);
            return m_stmBuffered->write(buf, next(ok));
        }

        int32_t send_base64(exlib::string str)
        {
            obj_ptr<Buffer> buf = new Buffer(str);
            exlib::string s;

            base64_base::encode(buf, false, s);
            s.append("\r\n", 2);

            buf = new Buffer(s);
            return m_stmBuffered->write(buf, next(ok));
        }

        ON_STATE(asyncLogin, send_username)
        {
            return send_base64(m_username);
        }

        ON_STATE(asyncLogin, send_password)
        {
            return send_base64(m_password);
        }

        virtual int32_t recv_ok()
        {
            switch (step) {
            case 0:
                step++;
                return next(send_username);
            case 1:
                step++;
                return next(send_password);
            }
            return next();
        }

    private:
        exlib::string m_username;
        exlib::string m_password;
        int32_t step;
    };

    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncLogin(this, username, password, ac))->post(0);
}

result_t Smtp::from(exlib::string address, AsyncEvent* ac)
{
    return command("MAIL FROM", address, ac);
}

result_t Smtp::to(exlib::string address, AsyncEvent* ac)
{
    return command("RCPT TO", address, ac);
}

result_t Smtp::data(exlib::string txt, AsyncEvent* ac)
{
    class asyncData : public asyncSmtp {
    public:
        asyncData(Smtp* pThis, exlib::string txt, AsyncEvent* ac)
            : asyncSmtp(pThis, ac)
            , m_txt(txt)
            , step(0)
        {
            next(begin);
        }

        ON_STATE(asyncData, begin)
        {
            exlib::string s("DATA\r\n", 6);

            obj_ptr<Buffer> buf = new Buffer(s);
            return m_stmBuffered->write(buf, next(ok));
        }

        ON_STATE(asyncData, send_data)
        {
            exlib::string s(m_txt);

            s.append("\r\n.\r\n", 5);

            obj_ptr<Buffer> buf = new Buffer(s);
            return m_stmBuffered->write(buf, next(ok));
        }

        virtual int32_t recv_ok()
        {
            switch (step) {
            case 0:
                step++;
                return next(send_data);
            }
            return next();
        }

    private:
        exlib::string m_txt;
        int32_t step;
    };

    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncData(this, txt, ac))->post(0);
}

result_t Smtp::quit(AsyncEvent* ac)
{
    return command("QUIT", "", ac);
}

result_t Smtp::get_timeout(int32_t& retVal)
{
    retVal = m_timeout;
    return 0;
}

result_t Smtp::set_timeout(int32_t newVal)
{
    m_timeout = newVal;
    return 0;
}

result_t Smtp::get_socket(obj_ptr<Stream_base>& retVal)
{
    retVal = m_conn;
    return 0;
}

} /* namespace fibjs */
