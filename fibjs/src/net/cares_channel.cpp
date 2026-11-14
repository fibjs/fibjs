/*
 * cares_channel.cpp
 *
 * The uv-thread side of the c-ares integration.  See cares_channel.h for the
 * threading contract.
 */

#include "object.h"
#include "cares_channel.h"
#include "AsyncUV.h"

#ifndef CARES_STATICLIB
#define CARES_STATICLIB
#endif
// ares_set_servers_ports()/ares_get_servers_ports() are marked deprecated in
// 1.34 in favour of the *_csv variants.  The node-parity formatting rules are
// easier to keep with the structured form, so suppress the attribute here.
#ifndef CARES_NO_DEPRECATED
#define CARES_NO_DEPRECATED
#endif
#include <ares.h>

#include <map>
#include <mutex>
#include <string>
#include <string.h>

namespace fibjs {

// ---------------------------------------------------------------------------
// ares library lifetime (process wide, one init/cleanup pair)
// ---------------------------------------------------------------------------

static std::mutex s_cares_lib_lock;
static int32_t s_cares_lib_users = 0;

static int32_t cares_library_acquire()
{
    std::lock_guard<std::mutex> lock(s_cares_lib_lock);

    if (s_cares_lib_users == 0) {
        int32_t r = ares_library_init(ARES_LIB_INIT_ALL);
        if (r != ARES_SUCCESS)
            return r;
    }

    s_cares_lib_users++;
    return ARES_SUCCESS;
}

static void cares_library_release()
{
    std::lock_guard<std::mutex> lock(s_cares_lib_lock);

    if (s_cares_lib_users > 0 && --s_cares_lib_users == 0)
        ares_library_cleanup();
}

// ---------------------------------------------------------------------------
// status -> Node error code
// ---------------------------------------------------------------------------

static const char* cares_code(int32_t status)
{
    switch (status) {
#define V(code) \
    case ARES_##code: \
        return #code;
        V(EADDRGETNETWORKPARAMS)
        V(EBADFAMILY)
        V(EBADFLAGS)
        V(EBADHINTS)
        V(EBADNAME)
        V(EBADQUERY)
        V(EBADRESP)
        V(EBADSTR)
        V(ECANCELLED)
        V(ECONNREFUSED)
        V(EDESTRUCTION)
        V(EFILE)
        V(EFORMERR)
        V(ELOADIPHLPAPI)
        V(ENODATA)
        V(ENOMEM)
        V(ENONAME)
        V(ENOTFOUND)
        V(ENOTIMP)
        V(ENOTINITIALIZED)
        V(EOF)
        V(EREFUSED)
        V(ESERVFAIL)
        V(ETIMEOUT)
#undef V
    }

    return "UNKNOWN_ARES_ERROR";
}

result_t cares_error(int32_t status, const char* syscall,
    const exlib::string& hostname)
{
    result_t::builder b = result_t::make(CALL_E_INVALID_DATA)
                              .with_type_name(errtype::kError)
                              .with_code(cares_code(status))
                              .with_syscall(syscall);

    if (!hostname.empty())
        return std::move(b).with_hostname(hostname).arg("hostname", hostname);

    return std::move(b);
}

// ---------------------------------------------------------------------------
// uv handle wrappers
// ---------------------------------------------------------------------------

class CaresLoopState;

struct CaresPoll {
    uv_poll_t handle;
    ares_socket_t sock;
    CaresLoopState* state;
};

class CaresQuery : public obj_base {
public:
    void finish(ares_status_t status, const ares_dns_record* rec);
    void finish_reverse(int status, struct hostent* host);

public:
    AsyncHandle m_ac;
    // Keeps the channel (and therefore the loop state) alive while this query
    // is in flight; the resolver may be dropped by the caller at any time.
    obj_ptr<CaresChannel> m_channel;
    CaresLoopState* m_state = nullptr;
    exlib::string m_name;
    int32_t m_type = ARES_REC_TYPE_A;
    bool m_ttl = false;
    bool m_variant_array = false;
    CaresParseFn m_parse = nullptr;
    CaresArrayFn m_parse_array = nullptr;
    CaresNamesFn m_parse_names = nullptr;
    const char* m_syscall = "";
    Variant* m_retVal = nullptr;
    obj_ptr<NArray>* m_array = nullptr;
    std::vector<exlib::string>* m_names = nullptr;
};

class CaresLoopState : public obj_base {
public:
    static int32_t create(int32_t timeout, int32_t tries, int32_t maxTimeout,
        obj_ptr<CaresLoopState>& retVal);

    ~CaresLoopState();

    void close();
    void cancel();

    void start_query(obj_ptr<CaresQuery> q);
    void start_reverse(obj_ptr<CaresQuery> q);

private:
    void start_timer();
    void stop_timer_if_idle();

    void complete_query(CaresQuery* q, ares_status_t status,
        const ares_dns_record* rec);
    void complete_reverse(CaresQuery* q, int status, struct hostent* host);

    static void sock_state_cb(void* data, ares_socket_t sock, int read, int write);
    static void poll_cb(uv_poll_t* handle, int status, int events);
    static void timer_cb(uv_timer_t* handle);
    static void on_poll_closed(uv_handle_t* handle);
    static void on_timer_closed(uv_handle_t* handle);
    static void query_cb(void* arg, ares_status_t status, size_t timeouts,
        const ares_dns_record_t* rec);
    static void host_cb(void* arg, int status, int timeouts, struct hostent* host);

public:
    ares_channel_t* m_channel = nullptr;
    int32_t m_timeout = -1;
    bool m_lib_acquired = false;
    bool m_closed = false;

    uv_timer_t m_timer;
    bool m_timer_inited = false;

    std::map<ares_socket_t, CaresPoll*> m_polls;
    std::map<CaresQuery*, obj_ptr<CaresQuery>> m_queries;
};

int32_t CaresLoopState::create(int32_t timeout, int32_t tries,
    int32_t maxTimeout, obj_ptr<CaresLoopState>& retVal)
{
    int32_t r = cares_library_acquire();
    if (r != ARES_SUCCESS)
        return r;

    obj_ptr<CaresLoopState> st = new CaresLoopState();
    st->m_lib_acquired = true;
    st->m_timeout = timeout;

    ares_options options;
    memset(&options, 0, sizeof(options));
    options.flags = ARES_FLAG_NOCHECKRESP;
    options.sock_state_cb = sock_state_cb;
    options.sock_state_cb_data = st;
    options.timeout = timeout;
    options.tries = tries;
    // Node v25 explicitly disables c-ares' response cache.
    options.qcache_max_ttl = 0;

    int32_t optmask = ARES_OPT_FLAGS | ARES_OPT_TIMEOUTMS
        | ARES_OPT_SOCK_STATE_CB | ARES_OPT_TRIES | ARES_OPT_QUERY_CACHE;

    if (maxTimeout > 0) {
        options.maxtimeout = maxTimeout;
        optmask |= ARES_OPT_MAXTIMEOUTMS;
    }

    r = ares_init_options(&st->m_channel, &options, optmask);
    if (r != ARES_SUCCESS) {
        st->m_channel = nullptr;
        return r;
    }

    retVal = st;
    return ARES_SUCCESS;
}

CaresLoopState::~CaresLoopState()
{
    if (m_channel) {
        ares_destroy(m_channel);
        m_channel = nullptr;
    }

    if (m_lib_acquired)
        cares_library_release();
}

void CaresLoopState::start_timer()
{
    if (m_closed)
        return;

    if (!m_timer_inited) {
        memset(&m_timer, 0, sizeof(m_timer));
        uv_timer_init(s_uv_loop, &m_timer);
        m_timer.data = this;
        m_timer_inited = true;
    }

    int32_t interval = (m_timeout <= 0 || m_timeout > 1000) ? 1000 : m_timeout;

    if (!uv_is_active((uv_handle_t*)&m_timer))
        uv_timer_start(&m_timer, timer_cb, interval, interval);
}

void CaresLoopState::stop_timer_if_idle()
{
    if (m_timer_inited && m_queries.empty())
        uv_timer_stop(&m_timer);
}

void CaresLoopState::sock_state_cb(void* data, ares_socket_t sock, int read,
    int write)
{
    CaresLoopState* st = (CaresLoopState*)data;

    if (st->m_closed)
        return;

    auto it = st->m_polls.find(sock);

    if (read || write) {
        int32_t events = (read ? UV_READABLE : 0) | (write ? UV_WRITABLE : 0);

        if (it == st->m_polls.end()) {
            CaresPoll* p = new CaresPoll();
            p->sock = sock;
            p->state = st;

            if (uv_poll_init_socket(s_uv_loop, &p->handle, sock) < 0) {
                delete p;
                return;
            }

            p->handle.data = p;
            st->m_polls[sock] = p;
            uv_poll_start(&p->handle, events, poll_cb);
        } else {
            uv_poll_start(&it->second->handle, events, poll_cb);
        }
    } else if (it != st->m_polls.end()) {
        CaresPoll* p = it->second;
        st->m_polls.erase(it);
        uv_poll_stop(&p->handle);
        st->Ref();
        uv_close((uv_handle_t*)&p->handle, on_poll_closed);
    }
}

void CaresLoopState::poll_cb(uv_poll_t* handle, int status, int events)
{
    CaresPoll* p = (CaresPoll*)handle->data;
    CaresLoopState* st = p->state;

    if (st->m_closed || !st->m_channel)
        return;

    if (status < 0) {
        ares_socket_t sock = p->sock;
        auto it = st->m_polls.find(sock);
        if (it != st->m_polls.end()) {
            st->m_polls.erase(it);
            uv_poll_stop(&p->handle);
            st->Ref();
            uv_close((uv_handle_t*)&p->handle, on_poll_closed);
        }
        return;
    }

    ares_socket_t read_fd = (events & UV_READABLE) ? p->sock : ARES_SOCKET_BAD;
    ares_socket_t write_fd = (events & UV_WRITABLE) ? p->sock : ARES_SOCKET_BAD;

    ares_process_fd(st->m_channel, read_fd, write_fd);
}

void CaresLoopState::timer_cb(uv_timer_t* handle)
{
    CaresLoopState* st = (CaresLoopState*)handle->data;

    if (!st->m_closed && st->m_channel)
        ares_process_fd(st->m_channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
}

void CaresLoopState::on_poll_closed(uv_handle_t* handle)
{
    CaresPoll* p = (CaresPoll*)handle->data;

    p->state->Unref();
    delete p;
}

void CaresLoopState::on_timer_closed(uv_handle_t* handle)
{
    ((CaresLoopState*)handle->data)->Unref();
}

void CaresLoopState::start_query(obj_ptr<CaresQuery> q)
{
    if (m_closed || !m_channel) {
        q->m_ac.post(cares_error(ARES_EDESTRUCTION, q->m_syscall, q->m_name));
        return;
    }

    start_timer();
    m_queries[q.get()] = q;

    unsigned short qid = 0;
    ares_status_t r = ares_query_dnsrec(m_channel, q->m_name.c_str(),
        ARES_CLASS_IN, (ares_dns_rec_type_t)q->m_type, query_cb, q.get(), &qid);

    if (r != ARES_SUCCESS) {
        m_queries.erase(q.get());
        q->m_ac.post(cares_error(r, q->m_syscall, q->m_name));
        stop_timer_if_idle();
    }
}

void CaresLoopState::start_reverse(obj_ptr<CaresQuery> q)
{
    if (m_closed || !m_channel) {
        q->m_ac.post(cares_error(ARES_EDESTRUCTION, q->m_syscall, q->m_name));
        return;
    }

    struct in_addr addr4;
    struct ares_in6_addr addr6;
    const void* addr = nullptr;
    int32_t addrlen = 0;
    int32_t family = 0;

    if (uv_inet_pton(AF_INET, q->m_name.c_str(), &addr4) == 0) {
        addr = &addr4;
        addrlen = 4;
        family = AF_INET;
    } else if (uv_inet_pton(AF_INET6, q->m_name.c_str(), &addr6) == 0) {
        addr = &addr6;
        addrlen = 16;
        family = AF_INET6;
    } else {
        q->m_ac.post(result_t::make(CALL_E_INVALID_DATA)
                         .with_type_name(errtype::kTypeError)
                         .with_code("ERR_INVALID_ARG_VALUE")
                         .with_format("The argument 'address' must be a valid IP address. Received %s",
                             q->m_name.c_str())
                         .arg("address", q->m_name));
        return;
    }

    start_timer();
    m_queries[q.get()] = q;
    ares_gethostbyaddr(m_channel, addr, addrlen, family, host_cb, q.get());
}

void CaresLoopState::query_cb(void* arg, ares_status_t status, size_t timeouts,
    const ares_dns_record_t* rec)
{
    (void)timeouts;
    CaresQuery* q = (CaresQuery*)arg;
    q->m_state->complete_query(q, status, rec);
}

void CaresLoopState::host_cb(void* arg, int status, int timeouts,
    struct hostent* host)
{
    (void)timeouts;
    CaresQuery* q = (CaresQuery*)arg;
    q->m_state->complete_reverse(q, status, host);
}

void CaresLoopState::complete_query(CaresQuery* qraw, ares_status_t status,
    const ares_dns_record* rec)
{
    auto it = m_queries.find(qraw);
    if (it == m_queries.end())
        return;

    obj_ptr<CaresQuery> q = it->second;
    m_queries.erase(it);

    q->finish(status, rec);
    stop_timer_if_idle();
}

void CaresLoopState::complete_reverse(CaresQuery* qraw, int status,
    struct hostent* host)
{
    auto it = m_queries.find(qraw);
    if (it == m_queries.end())
        return;

    obj_ptr<CaresQuery> q = it->second;
    m_queries.erase(it);

    q->finish_reverse(status, host);
    stop_timer_if_idle();
}

void CaresLoopState::cancel()
{
    if (!m_closed && m_channel)
        ares_cancel(m_channel);
}

void CaresLoopState::close()
{
    if (m_closed)
        return;

    m_closed = true;

    if (m_channel) {
        // ares_cancel() delivers ECANCELLED synchronously; the callbacks post
        // their continuations and erase themselves from m_queries.
        ares_cancel(m_channel);
        ares_destroy(m_channel);
        m_channel = nullptr;
    }

    for (auto& item : m_queries) {
        obj_ptr<CaresQuery> q = item.second;
        q->m_ac.post(cares_error(ARES_EDESTRUCTION, q->m_syscall, q->m_name));
    }
    m_queries.clear();

    if (m_timer_inited) {
        uv_timer_stop(&m_timer);
        Ref();
        uv_close((uv_handle_t*)&m_timer, on_timer_closed);
        m_timer_inited = false;
    }

    for (auto& item : m_polls) {
        CaresPoll* p = item.second;
        uv_poll_stop(&p->handle);
        Ref();
        uv_close((uv_handle_t*)&p->handle, on_poll_closed);
    }
    m_polls.clear();
}

void CaresQuery::finish(ares_status_t status, const ares_dns_record* rec)
{
    if (status != ARES_SUCCESS) {
        m_ac.post(cares_error(status, m_syscall, m_name));
        return;
    }

    result_t hr;

    if (m_parse_array) {
        obj_ptr<NArray> arr = new NArray();
        hr = m_parse_array(m_type, m_ttl, rec, arr);
        if (hr.ok()) {
            if (arr->length() == 0)
                hr = cares_error(ARES_ENODATA, m_syscall, m_name);
            else if (m_variant_array)
                *m_retVal = arr;
            else
                *m_array = arr;
        }
    } else if (m_parse_names) {
        hr = m_parse_names(m_type, rec, *m_names);
        if (hr.ok() && m_names->empty())
            hr = cares_error(ARES_ENODATA, m_syscall, m_name);
    } else {
        hr = m_parse(m_type, m_ttl, rec, *m_retVal);
        if (hr.failed() && hr.payload()
            && hr.payload()->code == "ENODATA")
            hr = cares_error(ARES_ENODATA, m_syscall, m_name);
    }

    m_ac.post(hr);
}

void CaresQuery::finish_reverse(int status, struct hostent* host)
{
    if (status != ARES_SUCCESS) {
        m_ac.post(cares_error(status, "getHostByAddr", m_name));
        return;
    }

    // Node's HostentToNames() collects host->h_aliases only.  c-ares fills
    // h_aliases with every PTR name and h_name with a duplicate of the last
    // one, so including h_name here would add an alias Node never reports.
    if (host && host->h_aliases) {
        for (char** p = host->h_aliases; *p; p++)
            m_names->push_back(*p);
    }

    m_ac.post(result_t());
}

// ---------------------------------------------------------------------------
// CaresChannel
// ---------------------------------------------------------------------------

result_t CaresChannel::create(int32_t timeout, int32_t tries,
    int32_t maxTimeout, obj_ptr<CaresChannel>& retVal)
{
    obj_ptr<CaresLoopState> state;
    int32_t status = ARES_SUCCESS;

    result_t hr = uv_call([&]() -> int32_t {
        status = CaresLoopState::create(timeout, tries, maxTimeout, state);
        return 0;
    });

    if (hr.failed())
        return hr;

    if (status != ARES_SUCCESS)
        return cares_error(status, "ares_init_options", "");

    obj_ptr<CaresChannel> ch = new CaresChannel();
    ch->m_state = state;
    retVal = ch;
    return result_t();
}

CaresChannel::~CaresChannel()
{
    if (m_state) {
        obj_ptr<CaresLoopState> state = m_state;
        m_state.Release();
        uv_post([state]() { state->close(); });
    }
}

obj_ptr<CaresQuery> CaresChannel::prepare_query(const exlib::string& name,
    int32_t type, bool ttl, const char* syscall, AsyncHandle ac)
{
    obj_ptr<CaresLoopState> state = m_state;
    if (!state)
        return nullptr;

    obj_ptr<CaresQuery> q = new CaresQuery();
    q->m_ac = std::move(ac);
    q->m_channel = this;
    q->m_state = state;
    q->m_name = name;
    q->m_type = type;
    q->m_ttl = ttl;
    q->m_syscall = syscall;
    return q;
}

result_t CaresChannel::query(const exlib::string& name, int32_t type, bool ttl,
    CaresParseFn parse, const char* syscall, Variant& retVal, AsyncHandle ac)
{
    obj_ptr<CaresQuery> q = prepare_query(name, type, ttl, syscall,
        std::move(ac));
    if (!q)
        return cares_error(ARES_EDESTRUCTION, syscall, name);

    q->m_parse = parse;
    q->m_retVal = &retVal;

    obj_ptr<CaresLoopState> state = q->m_state;
    uv_post([state, q]() { state->start_query(q); });
    return result_t::pending();
}

result_t CaresChannel::query_array(const exlib::string& name, int32_t type,
    bool ttl, CaresArrayFn parse, const char* syscall, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    obj_ptr<CaresQuery> q = prepare_query(name, type, ttl, syscall,
        std::move(ac));
    if (!q)
        return cares_error(ARES_EDESTRUCTION, syscall, name);

    q->m_parse_array = parse;
    q->m_array = &retVal;

    obj_ptr<CaresLoopState> state = q->m_state;
    uv_post([state, q]() { state->start_query(q); });
    return result_t::pending();
}

result_t CaresChannel::query_variant_array(const exlib::string& name,
    int32_t type, bool ttl, CaresArrayFn parse, const char* syscall,
    Variant& retVal, AsyncHandle ac)
{
    obj_ptr<CaresQuery> q = prepare_query(name, type, ttl, syscall,
        std::move(ac));
    if (!q)
        return cares_error(ARES_EDESTRUCTION, syscall, name);

    q->m_parse_array = parse;
    q->m_variant_array = true;
    q->m_retVal = &retVal;

    obj_ptr<CaresLoopState> state = q->m_state;
    uv_post([state, q]() { state->start_query(q); });
    return result_t::pending();
}

result_t CaresChannel::query_names(const exlib::string& name, int32_t type,
    CaresNamesFn parse, const char* syscall,
    std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    obj_ptr<CaresQuery> q = prepare_query(name, type, false, syscall,
        std::move(ac));
    if (!q)
        return cares_error(ARES_EDESTRUCTION, syscall, name);

    q->m_parse_names = parse;
    q->m_names = &retVal;

    obj_ptr<CaresLoopState> state = q->m_state;
    uv_post([state, q]() { state->start_query(q); });
    return result_t::pending();
}

result_t CaresChannel::reverse(const exlib::string& ip,
    std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    obj_ptr<CaresQuery> q = prepare_query(ip, ARES_REC_TYPE_A, false,
        "getHostByAddr", std::move(ac));
    if (!q)
        return cares_error(ARES_EDESTRUCTION, "getHostByAddr", ip);

    q->m_names = &retVal;

    obj_ptr<CaresLoopState> state = q->m_state;
    uv_post([state, q]() { state->start_reverse(q); });
    return result_t::pending();
}

result_t CaresChannel::cancel()
{
    obj_ptr<CaresLoopState> state = m_state;
    if (!state)
        return cares_error(ARES_EDESTRUCTION, "cancel", "");

    return uv_call([&]() -> int32_t {
        state->cancel();
        return 0;
    });
}

result_t CaresChannel::getServers(std::vector<exlib::string>& retVal)
{
    obj_ptr<CaresLoopState> state = m_state;
    if (!state)
        return cares_error(ARES_EDESTRUCTION, "getServers", "");

    int32_t status = ARES_SUCCESS;

    result_t hr = uv_call([&]() -> int32_t {
        if (!state->m_channel) {
            status = ARES_EDESTRUCTION;
            return 0;
        }

        ares_addr_port_node* servers = nullptr;
        status = ares_get_servers_ports(state->m_channel, &servers);
        if (status != ARES_SUCCESS)
            return 0;

        for (ares_addr_port_node* p = servers; p; p = p->next) {
            char buf[64];
            const char* host = nullptr;

            if (p->family == AF_INET)
                host = ares_inet_ntop(AF_INET, &p->addr.addr4, buf, sizeof(buf));
            else if (p->family == AF_INET6)
                host = ares_inet_ntop(AF_INET6, &p->addr.addr6, buf, sizeof(buf));

            if (!host)
                continue;

            int32_t port = p->udp_port;
            if (port == 0)
                port = 53;

            exlib::string s(host);
            if (port != 53) {
                if (p->family == AF_INET6) {
                    s = "[";
                    s.append(host);
                    s.append("]");
                }
                s.append(":");
                s.append(std::to_string(port).c_str());
            }

            retVal.push_back(s);
        }

        if (servers)
            ares_free_data(servers);

        return 0;
    });

    if (hr.failed())
        return hr;

    if (status != ARES_SUCCESS)
        return cares_error(status, "getServers", "");

    return result_t();
}

namespace {

struct ServerNode {
    int32_t family;
    uint8_t addr[16];
    int32_t port;
};

result_t invalid_ip_address(const exlib::string& value)
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_IP_ADDRESS")
        .with_format("Invalid IP address: %s", value.c_str())
        .arg("servers", value);
}

static bool parse_ip(const exlib::string& value, int32_t& family,
    uint8_t addr[16])
{
    if (uv_inet_pton(AF_INET, value.c_str(), addr) == 0) {
        family = AF_INET;
        return true;
    }

    if (uv_inet_pton(AF_INET6, value.c_str(), addr) == 0) {
        family = AF_INET6;
        return true;
    }

    return false;
}

// Node's addrSplitRE is /(^.+?)(?::(\d+))?$/: a non-empty host followed by
// an optional trailing ":digits" port.  parseInt() is then applied to the
// digits, so ports are intentionally not range-checked here; c-ares stores
// them in an unsigned short.
static void split_port(const exlib::string& value, exlib::string& host,
    int32_t& port)
{
    size_t colon = exlib::string::npos;
    for (size_t i = value.size(); i-- > 0; ) {
        if (value[i] == ':') {
            colon = i;
            break;
        }
    }

    host = value;
    port = 53;

    if (colon == exlib::string::npos || colon == 0
        || colon + 1 == value.size())
        return;

    uint32_t parsed = 0;
    for (size_t i = colon + 1; i < value.size(); i++) {
        char c = value[i];
        if (c < '0' || c > '9')
            return;
        parsed = parsed * 10 + (uint32_t)(c - '0');
    }

    host = value.substr(0, colon);
    port = (int32_t)(uint16_t)parsed;
}

result_t parse_server(const exlib::string& server, ServerNode& node)
{
    if (server.empty())
        return invalid_ip_address(server);

    // A bare address (including IPv6 with an IPv4-mapped tail) wins, exactly
    // like Node's isIP(serv) fast path.
    int32_t family;
    uint8_t addr[16];

    if (parse_ip(server, family, addr)) {
        node.family = family;
        memcpy(node.addr, addr, family == AF_INET ? 4 : 16);
        node.port = 53;
        return result_t();
    }

    // Bracketed IPv6: Node matches /^\[([^[\]]*)\]/ and only requires the
    // bracketed part to be an IP.  Anything after the closing bracket is
    // ignored except for a trailing ":digits" port.
    if (server[0] == '[') {
        size_t end = server.find(']');
        if (end != exlib::string::npos) {
            exlib::string inner = server.substr(1, end - 1);

            if (inner.find('[') == exlib::string::npos
                && parse_ip(inner, family, addr)) {
                exlib::string host;
                int32_t port;
                split_port(server, host, port);

                // Node uses parseInt(...) || 53 for the bracketed form, so a
                // literal port 0 falls back to the default IANA port.
                if (port == 0)
                    port = 53;

                node.family = family;
                memcpy(node.addr, addr, family == AF_INET ? 4 : 16);
                node.port = port;
                return result_t();
            }
        }
    }

    exlib::string host;
    int32_t port;
    split_port(server, host, port);

    if (parse_ip(host, family, addr)) {
        node.family = family;
        memcpy(node.addr, addr, family == AF_INET ? 4 : 16);
        node.port = port;
        return result_t();
    }

    return invalid_ip_address(server);
}

result_t set_servers_error(int32_t status, const exlib::string& servers)
{
    // Node's DNS_ESETSRVPENDING is a private sentinel (-1000) with its own
    // message; c-ares itself has no such status.
    static const int32_t DNS_ESETSRVPENDING = -1000;
    const char* message = status == DNS_ESETSRVPENDING
        ? "There are pending queries."
        : ares_strerror(status);

    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kError)
        .with_code("ERR_DNS_SET_SERVERS_FAILED")
        .with_format("c-ares failed to set servers: \"%s\" [%s]",
            message, servers.c_str())
        .arg("servers", servers);
}

} // namespace

result_t CaresChannel::setServers(
    const std::vector<exlib::string>& servers)
{
    obj_ptr<CaresLoopState> state = m_state;
    if (!state)
        return cares_error(ARES_EDESTRUCTION, "setServers", "");

    std::vector<ServerNode> nodes;
    nodes.reserve(servers.size());

    for (auto& server : servers) {
        ServerNode node;
        result_t hr = parse_server(server, node);
        if (hr.failed())
            return hr;
        nodes.push_back(node);
    }

    int32_t status = ARES_ESERVFAIL;

    result_t hr = uv_call([&]() -> int32_t {
        // Node refuses to replace the server list while queries are in
        // flight, because c-ares would abandon their current name servers.
        if (!state->m_queries.empty()) {
            status = -1000; // DNS_ESETSRVPENDING, as in Node's cares_wrap
            return 0;
        }

        if (!state->m_channel) {
            status = ARES_EDESTRUCTION;
            return 0;
        }

        std::vector<ares_addr_port_node> list(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            memset(&list[i], 0, sizeof(list[i]));
            list[i].family = nodes[i].family;
            list[i].udp_port = nodes[i].port;
            list[i].tcp_port = nodes[i].port;
            if (nodes[i].family == AF_INET)
                memcpy(&list[i].addr.addr4, nodes[i].addr, 4);
            else
                memcpy(&list[i].addr.addr6, nodes[i].addr, 16);
            list[i].next = (i + 1 < nodes.size()) ? &list[i + 1] : nullptr;
        }

        ares_addr_port_node* old = nullptr;
        ares_get_servers_ports(state->m_channel, &old);

        status = ares_set_servers_ports(state->m_channel,
            list.empty() ? nullptr : &list[0]);

        if (status != ARES_SUCCESS)
            ares_set_servers_ports(state->m_channel, old);

        if (old)
            ares_free_data(old);

        return 0;
    });

    if (hr.failed())
        return hr;

    if (status != ARES_SUCCESS) {
        exlib::string joined;
        for (size_t i = 0; i < servers.size(); i++) {
            if (i)
                joined += ",";
            joined += servers[i];
        }
        return set_servers_error(status, joined);
    }
    return result_t();
}

result_t CaresChannel::setLocalAddress(const exlib::string& ipv4,
    const exlib::string& ipv6)
{
    obj_ptr<CaresLoopState> state = m_state;
    if (!state)
        return cares_error(ARES_EDESTRUCTION, "setLocalAddress", "");

    uint8_t addr0[16] = { 0 };
    uint8_t addr1[16] = { 0 };
    int32_t type0 = 0;

    exlib::string first = ipv4;
    exlib::string second = ipv6;

    if (first.empty())
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_message("Invalid IP address.");

    if (uv_inet_pton(AF_INET, first.c_str(), addr0) == 0) {
        type0 = 4;
    } else if (uv_inet_pton(AF_INET6, first.c_str(), addr0) == 0) {
        type0 = 6;
    } else {
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_message("Invalid IP address.");
    }

    bool has_second = !second.empty();

    if (has_second) {
        if (uv_inet_pton(AF_INET, second.c_str(), addr1) == 0) {
            if (type0 == 4)
                return result_t::make(CALL_E_INVALID_DATA)
                    .with_type_name(errtype::kTypeError)
                    .with_code("ERR_INVALID_ARG_VALUE")
                    .with_message("Cannot specify two IPv4 addresses.");
        } else if (uv_inet_pton(AF_INET6, second.c_str(), addr1) == 0) {
            if (type0 == 6)
                return result_t::make(CALL_E_INVALID_DATA)
                    .with_type_name(errtype::kTypeError)
                    .with_code("ERR_INVALID_ARG_VALUE")
                    .with_message("Cannot specify two IPv6 addresses.");
        } else {
            return result_t::make(CALL_E_INVALID_DATA)
                .with_type_name(errtype::kTypeError)
                .with_code("ERR_INVALID_ARG_VALUE")
                .with_message("Invalid IP address.");
        }
    }

    return uv_call([&]() -> int32_t {
        if (!state->m_channel)
            return 0;

        if (type0 == 4) {
            uint32_t v = ((uint32_t)addr0[0] << 24) | ((uint32_t)addr0[1] << 16)
                | ((uint32_t)addr0[2] << 8) | addr0[3];
            ares_set_local_ip4(state->m_channel, v);
        } else {
            ares_set_local_ip6(state->m_channel, addr0);
        }

        if (has_second) {
            if (type0 == 4)
                ares_set_local_ip6(state->m_channel, addr1);
            else {
                uint32_t v = ((uint32_t)addr1[0] << 24)
                    | ((uint32_t)addr1[1] << 16) | ((uint32_t)addr1[2] << 8)
                    | addr1[3];
                ares_set_local_ip4(state->m_channel, v);
            }
        } else if (type0 == 4) {
            uint8_t zero[16] = { 0 };
            ares_set_local_ip6(state->m_channel, zero);
        } else {
            ares_set_local_ip4(state->m_channel, 0);
        }

        return 0;
    });
}

} // namespace fibjs
