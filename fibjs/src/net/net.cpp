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
#include "net_resolve.h"

namespace fibjs {

DECLARE_MODULE(net);

result_t net_base::get_use_uv_socket(bool& retVal)
{
    retVal = g_uv_socket;
    return result_t();
}

result_t net_base::set_use_uv_socket(bool newVal)
{
    g_uv_socket = newVal;
    return result_t();
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
    return result_t();
}

result_t net_base::setDefaultAutoSelectFamily(bool enabled)
{
    s_autoSelectFamily = enabled;
    return result_t();
}

result_t net_base::getDefaultAutoSelectFamilyAttemptTimeout(int32_t& retVal)
{
    retVal = s_autoSelectFamilyAttemptTimeout;
    return result_t();
}

result_t net_base::setDefaultAutoSelectFamilyAttemptTimeout(int32_t milliseconds)
{
    if (milliseconds < 10)
        return result_t::make(CALL_E_OVERFLOW).with_type_name(errtype::kRangeError).with_code("ERR_OUT_OF_RANGE").with_format("setDefaultAutoSelectFamilyAttemptTimeout: milliseconds must be >= 10, got %d.", milliseconds).with_result(result_t::out_of_range());

    s_autoSelectFamilyAttemptTimeout = milliseconds;
    return result_t();
}

result_t net_base::info(ValueMap& retVal)
{
    return os_base::networkInterfaces(retVal);
}

result_t net_base::resolve(exlib::string name, int32_t family,
    exlib::string& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    if (family != net_base::C_AF_INET && family != net_base::C_AF_INET6)
        return result_t::make(CALL_E_INVALID_DATA).with_code("ERR_INVALID_ARG_VALUE").with_type_name(errtype::kTypeError).with_result(result_t::invalid_arg()).with_format("resolve: invalid address family %d.", family);

    ResolvedHost resolved;
    result_t hr = resolve_host(name, family == net_base::C_AF_INET ? 4 : 6,
        0, "", resolved, ac.isolate());
    if (hr.failed())
        return hr;

    retVal = resolved.address;
    return result_t();
}

result_t net_base::ip(exlib::string name, exlib::string& retVal,
    AsyncHandle ac)
{
    return resolve(name, net_base::C_AF_INET, retVal, std::move(ac));
}

result_t net_base::ipv6(exlib::string name, exlib::string& retVal,
    AsyncHandle ac)
{
    return resolve(name, net_base::C_AF_INET6, retVal, std::move(ac));
}

// Resolve a name and create the socket with the resulting family. A background
// fiber without a JS isolate (a worker being torn down) keeps the old
// literal-family path instead of posting a lookup through a dying isolate.
static result_t connect_resolved(exlib::string host, int32_t port, int32_t timeout,
    int32_t family, int32_t hints, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    obj_ptr<Socket_base> socket;

    if (port == 0) {
        result_t hr = Socket_base::_new(net_base::C_AF_UNIX, socket);
        if (hr.failed())
            return hr;

        return socket->connect(0, host, timeout, retVal, std::move(ac));
    }

    Isolate* isolate = Isolate::current();
    if (!isolate) {
        int32_t literal_family = Url::isIPv6(host) ? net_base::C_AF_INET6
                                                   : net_base::C_AF_INET;

        result_t hr = Socket_base::_new(literal_family, socket);
        if (hr.failed())
            return hr;

        return socket->connect(port, host, timeout, retVal, std::move(ac));
    }

    ResolvedHost resolved;
    result_t hr = resolve_host(host, family,
        connect_lookup_hints(family, hints), "ipv4first", resolved, isolate);
    if (hr.failed())
        return hr;

    hr = Socket_base::_new(resolved.family == 6 ? net_base::C_AF_INET6
                                                : net_base::C_AF_INET,
        socket);
    if (hr.failed())
        return hr;

    return socket->connect(port, resolved.address, timeout, retVal, std::move(ac));
}

result_t net_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal,
    AsyncHandle ac)
{
    // 纯 tag 分发（不转换、不触碰 V8）：ssl: 直接交给 tls 入口，由它处理相位
    if (!qstrcmp(url.c_str(), "ssl:", 4))
        return tls_base::connect(url, timeout, retVal, std::move(ac));

    if (ac.isSync())
        return result_t::nosync();

    if (qstrcmp(url.c_str(), "tcp:", 4) && qstrcmp(url.c_str(), "unix:", 5) && qstrcmp(url.c_str(), "pipe:", 5))
        return result_t::make(CALL_E_INVALID_DATA).with_code("ERR_INVALID_ARG_VALUE").with_type_name(errtype::kTypeError).with_result(result_t::invalid_arg()).with_format("connect: unknown protocol in url '%s'.", url.c_str());

    if (!qstrcmp(url.c_str(), "tcp:", 4)) {
        obj_ptr<Url> u = new Url();

        result_t hr = u->parse(url);
        if (hr.failed())
            return hr;

        exlib::string port = u->port();
        if (port.length() == 0)
            return result_t::make(CALL_E_INVALID_DATA).with_code("ERR_INVALID_ARG_VALUE").with_type_name(errtype::kTypeError).with_result(result_t::invalid_arg()).with_format("connect: missing port in url '%s'.", url.c_str());

        int32_t nPort = atoi(port.c_str());

        return connect_resolved(u->hostname(), nPort, timeout, 0, 0, retVal,
            std::move(ac));
    } else {
        obj_ptr<Socket_base> socket;

        result_t hr = Socket_base::_new(net_base::C_AF_UNIX, socket);
        if (hr.failed())
            return hr;

        return socket->connect(url.substr(5), timeout, retVal, std::move(ac));
    }
}

result_t net_base::connect(int32_t port, exlib::string host, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return connect_resolved(host, port, timeout, 0, 0, retVal, std::move(ac));
}

result_t net_base::connect(ConnectOptions* options, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return connect_resolved(options->host.value(), options->port.value(),
        options->timeout.value(), options->family.value_or(0),
        options->hints.value_or(0), retVal, std::move(ac));
}

// the three forms of the merged arity-2 entry: an options object, a port and a
// unix socket path, each with a once connect listener
static result_t connect_by_options(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener,
    obj_ptr<Stream_base>& retVal, AsyncHandle ac);

static result_t connect_by_port(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return net_base::connect(port, "localhost", 0, connectListener, retVal, std::move(ac));
}

result_t net_base::connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(port, host, 0, connectListener, retVal, std::move(ac));
}

result_t net_base::connect(int32_t port, exlib::string host, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t hr;

        // the listener registration needs the socket wrapper, so the socket is
        // built in the sync phase only when a listener is present; otherwise
        // it is built in the async phase (no `_new` off the JS thread,
        // plans/async-phase-discipline-audit-2026-10-05.md §3-F14)
        if (!connectListener.IsEmpty()) {
            ResolvedHost resolved;
            int32_t family = net_base::C_AF_UNIX;

            if (port != 0) {
                hr = resolve_host(host, 0, connect_lookup_hints(0, 0),
                    "ipv4first", resolved, ac.isolate());
                if (hr.failed())
                    return hr;
                family = resolved.family == 6 ? net_base::C_AF_INET6
                                              : net_base::C_AF_INET;
            }

            obj_ptr<Socket_base> socket;
            hr = create_socket(family, socket);
            if (hr.failed())
                return hr;

            ac.ctxv().resize(2);
            ac.ctxv()[0] = socket;
            ac.ctxv()[1] = port == 0 ? host : resolved.address;

            v8::Local<v8::Object> _retVal;
            socket->once(socket->holder()->NewString("connect"), connectListener, _retVal);
        }

        return result_t::nosync();
    }

    obj_ptr<Socket_base> holder;
    Socket_base* socket = NULL;

    if (ac.ctxv().size() > 0 && ac.ctxv()[0].object() != NULL)
        socket = Socket_base::getInstance(ac.ctxv()[0].object());

    if (socket == NULL) {
        int32_t family = port == 0 ? net_base::C_AF_UNIX : 0;

        result_t hr = create_socket(family, holder);
        if (hr.failed())
            return hr;

        socket = holder;
    }

    exlib::string target = host;
    if (ac.ctxv().size() > 1)
        target = ac.ctxv()[1].string();

    return socket->connect(port, target, timeout, retVal, std::move(ac));
}

static result_t connect_by_path(exlib::string path, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return net_base::connect(0, path, 0, connectListener, retVal, std::move(ac));
}

result_t net_base::connect(Union_connect_options options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<v8::Local<v8::Object>>(options))
        return connect_by_options(std::get<v8::Local<v8::Object>>(options), connectListener, retVal, std::move(ac));

    if (std::holds_alternative<exlib::string>(options))
        return connect_by_path(std::get<exlib::string>(options), connectListener, retVal, std::move(ac));

    return connect_by_port(std::get<int32_t>(options), connectListener, retVal, std::move(ac));
}

result_t net_base::connect(exlib::string path, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(0, path, timeout, connectListener, retVal, std::move(ac));
}

static result_t connect_by_options(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        obj_ptr<ConnectOptions> opts;
        result_t hr = ConnectOptions::load(options, opts);
        if (hr.failed())
            return hr;

        int32_t family = opts->family.value_or(0);
        ResolvedHost resolved;
        hr = resolve_host(opts->host.value(), family,
            connect_lookup_hints(family, opts->hints.value_or(0)),
            "ipv4first", resolved, ac.isolate());
        if (hr.failed())
            return hr;

        ac.ctxv().resize(3);
        ac.ctxv()[0] = opts;
        ac.ctxv()[2] = resolved.address;

        obj_ptr<Socket_base> socket;
        hr = Socket_base::_new(resolved.family == 6 ? net_base::C_AF_INET6
                                                     : net_base::C_AF_INET,
            socket);
        if (hr.failed())
            return hr;

        ac.ctxv()[1] = socket;

        v8::Local<v8::Object> _retVal;
        socket->once(socket->holder()->NewString("connect"), connectListener, _retVal);

        return result_t::nosync();
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr.failed())
        return ctx_hr;
    ctx_hr = ac.ctx(1);
    if (ctx_hr.failed())
        return ctx_hr;
    ctx_hr = ac.ctx(2);
    if (ctx_hr.failed())
        return ctx_hr;

    ConnectOptions* opts = (ConnectOptions*)ac.ctxv()[0].object();
    Socket_base* socket = (Socket_base*)ac.ctxv()[1].object();

    // Every read of `ac` has to happen before the handle is moved: the
    // parameter initializations of one call are only indeterminately
    // sequenced, so `ac.ctxv()[2].string()` next to `std::move(ac)` may run
    // after the move has already nulled the handle. clang on Windows/ARM64
    // orders it that way and folds `ac.ctxv()` on the moved-from (null)
    // handle into `mov w8, #0x20; ldr x8, [x8]` (m_ctx is at 0x20), which
    // faults on `net.connect(options, connectListener)`.
    int32_t port = opts->port.value();
    int32_t timeout = opts->timeout.value();
    exlib::string host = ac.ctxv()[2].string();

    return socket->connect(port, std::move(host), timeout, retVal, std::move(ac));
}

result_t net_base::openSmtp(exlib::string url, int32_t timeout,
    obj_ptr<Smtp_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    result_t hr;

    hr = Smtp_base::_new(retVal);
    if (hr.failed())
        return hr;

    retVal->set_timeout(timeout);

    return retVal->connect(url, std::move(ac));
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
        return result_t();

    if (Url::isIPv4(s)) {
        retVal = 4;
        return result_t();
    }

    if (Url::isIPv6(s))
        retVal = 6;

    return result_t();
}

result_t net_base::isIPv4(v8::Local<v8::Value> ip, bool& retVal)
{
    exlib::string s;

    retVal = is_ip_string(ip, s) && Url::isIPv4(s);
    return result_t();
}

result_t net_base::isIPv6(v8::Local<v8::Value> ip, bool& retVal)
{
    exlib::string s;

    retVal = is_ip_string(ip, s) && Url::isIPv6(s);
    return result_t();
}

result_t net_base::createServer(TcpServerOptions* options, Union_createServer_listener listener, obj_ptr<TcpServer_base>& retVal)
{
    return TcpServer_base::_new(options, listener, retVal);
}

result_t net_base::createServer(Union_createServer_listener listener, obj_ptr<TcpServer_base>& retVal)
{
    return TcpServer_base::_new(listener, retVal);
}
}
