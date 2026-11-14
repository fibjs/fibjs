/*
 * cares_channel.h
 *
 * c-ares channel bound to the fibjs uv loop thread.
 *
 * Every ares_* call runs on the s_uv_loop thread: the public methods marshal
 * with uv_call/uv_post, and the loop state owns the ares_channel_t plus the
 * uv_poll/uv_timer handles that drive it.  A query keeps the channel alive
 * until its completion callback has been delivered.
 */

#pragma once

#include "object.h"
#include "AsyncCall.h"
#include <vector>

#ifndef CARES_STATICLIB
#define CARES_STATICLIB
#endif
#ifndef CARES_NO_DEPRECATED
#define CARES_NO_DEPRECATED
#endif
#include <ares.h>

namespace fibjs {

class CaresLoopState;
class CaresQuery;

// Called on the uv thread with a completed, still-valid record.  It fills the
// caller's result before the continuation is posted.
typedef result_t (*CaresParseFn)(int32_t type, bool ttl,
    const ares_dns_record_t* rec, Variant& retVal);
typedef result_t (*CaresArrayFn)(int32_t type, bool ttl,
    const ares_dns_record_t* rec, NArray* retVal);
typedef result_t (*CaresNamesFn)(int32_t type,
    const ares_dns_record_t* rec, std::vector<exlib::string>& retVal);

class CaresChannel : public obj_base {
public:
    // timeout/tries/maxTimeout follow Node's Resolver options.
    static result_t create(int32_t timeout, int32_t tries, int32_t maxTimeout,
        obj_ptr<CaresChannel>& retVal);

    // The async phase hands its continuation over here; the query returns
    // CALL_E_PENDDING and the completion posts either the parsed records or
    // the error.
    result_t query(const exlib::string& name, int32_t type, bool ttl,
        CaresParseFn parse, const char* syscall, Variant& retVal, AsyncHandle ac);
    result_t query_array(const exlib::string& name, int32_t type, bool ttl,
        CaresArrayFn parse, const char* syscall, obj_ptr<NArray>& retVal,
        AsyncHandle ac);
    result_t query_variant_array(const exlib::string& name, int32_t type,
        bool ttl, CaresArrayFn parse, const char* syscall, Variant& retVal,
        AsyncHandle ac);
    result_t query_names(const exlib::string& name, int32_t type,
        CaresNamesFn parse, const char* syscall,
        std::vector<exlib::string>& retVal, AsyncHandle ac);
    result_t reverse(const exlib::string& ip, std::vector<exlib::string>& retVal,
        AsyncHandle ac);

    result_t cancel();
    result_t getServers(std::vector<exlib::string>& retVal);
    result_t setServers(const std::vector<exlib::string>& servers);
    result_t setLocalAddress(const exlib::string& ipv4, const exlib::string& ipv6);

    ~CaresChannel();

private:
    obj_ptr<CaresQuery> prepare_query(const exlib::string& name, int32_t type,
        bool ttl, const char* syscall, AsyncHandle ac);

private:
    obj_ptr<CaresLoopState> m_state;
};

// Build a failed result for an ares status; shared by the channel and the
// resolver-class wrappers.
result_t cares_error(int32_t status, const char* syscall,
    const exlib::string& hostname);

} // namespace fibjs
