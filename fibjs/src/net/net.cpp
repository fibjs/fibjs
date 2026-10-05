/*
 * net.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/tls.h"
#include "ifs/os.h"
#include "ifs/TcpServer.h"
#include "inetAddr.h"
#include "Url.h"
#include "options.h"
#include "AsyncUV.h"
#include "Socket.h"
#include "union_helpers.h"

namespace fibjs {

DECLARE_MODULE(net);

result_t net_base::get_use_uv_socket(bool& retVal)
{
    retVal = g_uv_socket;
    return 0;
}

result_t net_base::set_use_uv_socket(bool newVal)
{
    g_uv_socket = newVal;
    return 0;
}

// Node.js >= 18.13 net module: autoSelectFamily default options.
// fibjs connect() does not perform family autodetection yet, but the defaults
// must exist and be queryable so Node-style consumers (e.g. playwright-core
// >= 1.63, which reads them at module init) can load unmodified.
static bool s_autoSelectFamily = true;
static int32_t s_autoSelectFamilyAttemptTimeout = 250;

result_t net_base::getDefaultAutoSelectFamily(bool& retVal)
{
    retVal = s_autoSelectFamily;
    return 0;
}

result_t net_base::setDefaultAutoSelectFamily(bool enabled)
{
    s_autoSelectFamily = enabled;
    return 0;
}

result_t net_base::getDefaultAutoSelectFamilyAttemptTimeout(int32_t& retVal)
{
    retVal = s_autoSelectFamilyAttemptTimeout;
    return 0;
}

result_t net_base::setDefaultAutoSelectFamilyAttemptTimeout(int32_t milliseconds)
{
    if (milliseconds < 10)
        return CHECK_ERROR(Runtime::setError(CALL_E_OUTRANGE,
            "setDefaultAutoSelectFamilyAttemptTimeout: milliseconds must be >= 10, got %d.", milliseconds));

    s_autoSelectFamilyAttemptTimeout = milliseconds;
    return 0;
}

result_t net_base::info(v8::Local<v8::Object>& retVal)
{
    return os_base::networkInterfaces(retVal);
}

result_t net_base::resolve(exlib::string name, int32_t family,
    exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (family != net_base::C_AF_INET && family != net_base::C_AF_INET6)
        return CHECK_ERROR(Runtime::setError(
            ErrorPayload::make(errtype::kTypeError, CALL_E_INVALIDARG)
                .format("resolve: invalid address family %d.", family)));

    class resolve_data : public uv_getaddrinfo_t {
    public:
        resolve_data(int32_t family, exlib::string name, exlib::string& retVal, AsyncEvent* ac)
            : _family(family)
            , _name(name)
            , _retVal(retVal)
            , _ac(ac)
        {
        }

    public:
        int32_t _family;
        exlib::string _name;
        exlib::string& _retVal;
        AsyncEvent* _ac;
    };

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    resolve_data* resolver = new resolve_data(family, name, retVal, ac);
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;

            if (status < 0) {
                uv_freeaddrinfo(res);
                // 先取用再释放：resolver 在 post 之前必须保持有效
                AsyncEvent* ac = resolver->_ac;
                exlib::string hostname = resolver->_name;
                int32_t family = resolver->_family;
                exlib::string code;
                if (status == UV_EAI_NODATA || status == UV_EAI_NONAME)
                    code = "ENOTFOUND";
                delete resolver;

                setErrorPayload(ErrorPayload::from_uv(status)
                                    .with_syscall("getaddrinfo")
                                    .with_hostname(hostname)
                                    .with_code(code)
                                    .arg("hostname", hostname)
                                    .arg("family", family));
                ac->post(status);
                return;
            }

            struct addrinfo* ptr = NULL;
            for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
                if (ptr->ai_family == resolver->_family) {
                    inetAddr addr_info;
                    addr_info.init(ptr->ai_addr);
                    resolver->_retVal = addr_info.str();
                    break;
                }
            }

            if (ptr == NULL) {
                exlib::string hostname = resolver->_name;
                int32_t family = resolver->_family;
#ifdef _WIN32
                setErrorPayload(ErrorPayload::from_system(-WSAHOST_NOT_FOUND)
                                    .with_syscall("getaddrinfo")
                                    .with_hostname(hostname)
                                    .with_code("ENOTFOUND")
                                    .arg("hostname", hostname)
                                    .arg("family", family));
                resolver->_ac->post(-WSAHOST_NOT_FOUND);
#else
                setErrorPayload(ErrorPayload::from_system(-ETIME)
                                    .with_syscall("getaddrinfo")
                                    .with_hostname(hostname)
                                    .arg("hostname", hostname)
                                    .arg("family", family));
                resolver->_ac->post(-ETIME);
#endif
            } else
                resolver->_ac->post(0);

            uv_freeaddrinfo(res);
            delete resolver;
        },
        name.c_str(), NULL, &hints);

    if (r < 0) {
        delete resolver;
        return CHECK_ERROR(r);
    }

    return CALL_E_PENDDING;
}

result_t net_base::ip(exlib::string name, exlib::string& retVal,
    AsyncEvent* ac)
{
    return resolve(name, net_base::C_AF_INET, retVal, ac);
}

result_t net_base::ipv6(exlib::string name, exlib::string& retVal,
    AsyncEvent* ac)
{
    return resolve(name, net_base::C_AF_INET6, retVal, ac);
}

result_t net_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal,
    AsyncEvent* ac)
{
    // 纯 tag 分发（不转换、不触碰 V8）：ssl: 直接交给 tls 入口，由它处理相位
    if (!qstrcmp(url.c_str(), "ssl:", 4))
        return tls_base::connect(url, timeout, retVal, ac);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (qstrcmp(url.c_str(), "tcp:", 4) && qstrcmp(url.c_str(), "unix:", 5) && qstrcmp(url.c_str(), "pipe:", 5))
        return CHECK_ERROR(Runtime::setError(
            ErrorPayload::make(errtype::kTypeError, CALL_E_INVALIDARG)
                .format("connect: unknown protocol in url '%s'.", url.c_str())));

    if (!qstrcmp(url.c_str(), "tcp:", 4)) {
        obj_ptr<Url> u = new Url();

        result_t hr = u->parse(url);
        if (hr < 0)
            return hr;

        exlib::string port = u->port();
        if (port.length() == 0)
            return CHECK_ERROR(Runtime::setError(
                ErrorPayload::make(errtype::kTypeError, CALL_E_INVALIDARG)
                    .format("connect: missing port in url '%s'.", url.c_str())));

        int32_t nPort = atoi(port.c_str());
        int32_t family = u->isIPv6() ? net_base::C_AF_INET6 : net_base::C_AF_INET;

        obj_ptr<Socket_base> socket;

        hr = Socket_base::_new(family, socket);
        if (hr < 0)
            return hr;

        return socket->connect(nPort, u->hostname(), timeout, retVal, ac);
    } else {
        obj_ptr<Socket_base> socket;

        result_t hr = Socket_base::_new(net_base::C_AF_UNIX, socket);
        if (hr < 0)
            return hr;

        return socket->connect(url.substr(5), timeout, retVal, ac);
    }
}

result_t net_base::connect(int32_t port, exlib::string host, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    bool is_ipv6 = Url::isIPv6(host);
    int32_t family = is_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET;

    obj_ptr<Socket_base> socket;
    result_t hr = Socket_base::_new(family, socket);
    if (hr < 0)
        return hr;

    return socket->connect(port, host, timeout, retVal, ac);
}

result_t net_base::connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<ConnectOptions> opts;
        Isolate* isolate = Isolate::current(options);
        result_t hr = ConnectOptions::load(options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac->ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    ConnectOptions* opts = (ConnectOptions*)ac->m_ctx[0].object();
    return connect(opts->port.value(), opts->host.value(), opts->timeout.value(), retVal, ac);
}

// the three forms of the merged arity-2 entry: an options object, a port and a
// unix socket path, each with a once connect listener
static result_t connect_by_options(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener,
    obj_ptr<Stream_base>& retVal, AsyncEvent* ac);

static result_t connect_by_port(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return net_base::connect(port, "localhost", 0, connectListener, retVal, ac);
}

result_t net_base::connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, host, 0, connectListener, retVal, ac);
}

result_t net_base::connect(int32_t port, exlib::string host, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        // the listener registration needs the socket wrapper, so the socket is
        // built in the sync phase only when a listener is present; otherwise
        // it is built in the async phase (no `_new` off the JS thread,
        // plans/async-phase-discipline-audit-2026-10-05.md §3-F14)
        if (!connectListener.IsEmpty()) {
            bool is_ipv6 = Url::isIPv6(host);
            int32_t family = is_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET;

            obj_ptr<Socket_base> socket;
            result_t hr = create_socket(family, socket);
            if (hr < 0)
                return hr;

            ac->m_ctx.resize(1);
            ac->m_ctx[0] = socket;

            v8::Local<v8::Object> _retVal;
            socket->once(socket->holder()->NewString("connect"), connectListener, _retVal);
        }

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<Socket_base> holder;
    Socket_base* socket = NULL;

    if (ac->m_ctx.size() > 0 && ac->m_ctx[0].object() != NULL)
        socket = Socket_base::getInstance(ac->m_ctx[0].object());

    if (socket == NULL) {
        bool is_ipv6 = Url::isIPv6(host);
        int32_t family = is_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET;

        result_t hr = create_socket(family, holder);
        if (hr < 0)
            return hr;

        socket = holder;
    }

    return socket->connect(port, host, timeout, retVal, ac);
}

static result_t connect_by_path(exlib::string path, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return net_base::connect(0, path, 0, connectListener, retVal, ac);
}

result_t net_base::connect(Union_connect_options options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (std::holds_alternative<v8::Local<v8::Object>>(options))
        return connect_by_options(std::get<v8::Local<v8::Object>>(options), connectListener, retVal, ac);

    if (std::holds_alternative<exlib::string>(options))
        return connect_by_path(std::get<exlib::string>(options), connectListener, retVal, ac);

    return connect_by_port(std::get<int32_t>(options), connectListener, retVal, ac);
}

result_t net_base::connect(exlib::string path, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(0, path, timeout, connectListener, retVal, ac);
}

static result_t connect_by_options(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<ConnectOptions> opts;
        Isolate* isolate = Isolate::current(options);
        result_t hr = ConnectOptions::load(options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(2);
        ac->m_ctx[0] = opts;

        bool is_ipv6 = Url::isIPv6(opts->host.value());
        int32_t family = is_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET;

        obj_ptr<Socket_base> socket;
        hr = Socket_base::_new(family, socket);
        if (hr < 0)
            return hr;

        ac->m_ctx[1] = socket;

        v8::Local<v8::Object> _retVal;
        socket->once(socket->holder()->NewString("connect"), connectListener, _retVal);

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac->ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;
    ctx_hr = ac->ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    ConnectOptions* opts = (ConnectOptions*)ac->m_ctx[0].object();
    Socket_base* socket = (Socket_base*)ac->m_ctx[1].object();
    return socket->connect(opts->port.value(), opts->host.value(), opts->timeout.value(), retVal, ac);
}

result_t net_base::openSmtp(exlib::string url, int32_t timeout,
    obj_ptr<Smtp_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = Smtp_base::_new(retVal);
    if (hr < 0)
        return hr;

    retVal->set_timeout(timeout);

    return retVal->connect(url, ac);
}

// the detection functions take any value and render it first (an object with
// a toString() is a valid input); this lenient rendering lives here instead of
// an IDL String conversion. Every value is rendered as the DOM renders it, so
// isIP(123) checks "123".
static bool is_ip_string(v8::Local<v8::Value> ip, exlib::string& s)
{
    if (ip.IsEmpty())
        return false;

    GetDOMStringValue(ip, s);
    return true;
}

result_t net_base::isIP(v8::Local<v8::Value> ip, int32_t& retVal)
{
    exlib::string s;

    retVal = 0;

    if (!is_ip_string(ip, s))
        return 0;

    if (Url::isIPv4(s)) {
        retVal = 4;
        return 0;
    }

    if (Url::isIPv6(s))
        retVal = 6;

    return 0;
}

result_t net_base::isIPv4(v8::Local<v8::Value> ip, bool& retVal)
{
    exlib::string s;

    retVal = is_ip_string(ip, s) && Url::isIPv4(s);
    return 0;
}

result_t net_base::isIPv6(v8::Local<v8::Value> ip, bool& retVal)
{
    exlib::string s;

    retVal = is_ip_string(ip, s) && Url::isIPv6(s);
    return 0;
}

result_t net_base::createServer(v8::Local<v8::Object> options, Union_createServer_listener listener, obj_ptr<TcpServer_base>& retVal)
{
    return TcpServer_base::_new(options, listener, retVal);
}

result_t net_base::createServer(Union_createServer_listener listener, obj_ptr<TcpServer_base>& retVal)
{
    return TcpServer_base::_new(listener, retVal);
}
}
