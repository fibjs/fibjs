/*
 * uuidVar.cpp
 *
 *  Created on: Jul 6, 2012
 *      Author: lion
 */

#include "object.h"
#include "Buffer.h"
#include "ifs/uuid.h"
#include "ifs/coroutine.h"
#include <stdlib.h>
#include <uuid/include/uuid.h>

namespace fibjs
{

DECLARE_MODULE(uuid);

static uuid_st *s_ns[4];
static const char *s_ns_str[4] =
{ "ns:DNS", "ns:URL", "ns:OID", "ns:X500" };


inline result_t toBuffer(uuid_st *_uid, obj_ptr<Buffer_base> &retVal)
{
    void *gen = NULL;
    size_t len = 0;

    uuid_export(_uid, UUID_FMT_BIN, &gen, &len);
    uuid_destroy(_uid);
    if (gen)
    {
        exlib::string s;

        s.append((const char *) gen, len);
        free(gen);

        retVal = new Buffer(s);

        return 0;
    }

    return CALL_RETURN_NULL;
}

result_t makeUUID(int32_t vers, int32_t ns, exlib::string name,
                  obj_ptr<Buffer_base> &retVal)
{
    if (ns < uuid_base::_DNS || ns > uuid_base::_X509)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    uuid_st *_uid;

    uuid_create(&_uid);

    if (vers == UUID_MAKE_V3 || vers == UUID_MAKE_V5)
    {
        if (!s_ns[ns])
        {
            uuid_create(&s_ns[ns]);
            uuid_load(s_ns[ns], s_ns_str[ns]);
        }

        uuid_make(_uid, vers, s_ns[ns], name.c_str());
    }
    else
        uuid_make(_uid, vers);

    return toBuffer(_uid, retVal);
}

result_t uuid_base::node(obj_ptr<Buffer_base> &retVal)
{
    return makeUUID(UUID_MAKE_V1, uuid_base::_X509, "", retVal);
}

result_t uuid_base::md5(int32_t ns, exlib::string name,
                        obj_ptr<Buffer_base> &retVal)
{
    return makeUUID(UUID_MAKE_V3, ns, name, retVal);
}

result_t uuid_base::random(obj_ptr<Buffer_base> &retVal)
{
    return makeUUID(UUID_MAKE_V4, uuid_base::_X509, "", retVal);
}

result_t uuid_base::sha1(int32_t ns, exlib::string name,
                         obj_ptr<Buffer_base> &retVal)
{
    return makeUUID(UUID_MAKE_V5, ns, name, retVal);
}

inline int64_t generateStamp()
{
    date_t d;
    d.now();
    return (int64_t)d.date();
}

result_t uuid_base::snowflake(obj_ptr<Buffer_base>& retVal)
{
    Isolate *isolate = Isolate::current();
    int64_t tm = generateStamp();

    if (isolate->m_flake_tm != tm)
    {
        isolate->m_flake_tm = tm;
        isolate->m_flake_count = 0;
    } else if (isolate->m_flake_count == 0xfff)
    {
        int64_t tm1 = tm;

        while (tm1 == tm)
        {
            coroutine_base::ac_sleep(0);
            tm = generateStamp();
        }

        if (isolate->m_flake_tm != tm)
        {
            isolate->m_flake_tm = tm;
            isolate->m_flake_count = 0;
        }
    }

    tm <<= 22;
    tm |= isolate->m_flake_host << 12;
    tm |= isolate->m_flake_count ++;

    obj_ptr<Buffer> data = new Buffer();
    data->resize(8);
    data->writeInt64BE(tm, 0, true);

    retVal = data;

    return 0;
}

result_t uuid_base::get_hostID(int32_t& retVal)
{
    Isolate *isolate = Isolate::current();
    retVal = isolate->m_flake_host;
    return 0;
}

result_t uuid_base::set_hostID(int32_t newVal)
{
    Isolate *isolate = Isolate::current();
    isolate->m_flake_host = newVal & 0x3ff;
    return 0;
}

} /* namespace fibjs */
