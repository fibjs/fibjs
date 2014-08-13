/*
 * uuidVar.cpp
 *
 *  Created on: Jul 6, 2012
 *      Author: lion
 */

#include "uuidVar.h"
#include "Buffer.h"
#include "ifs/uuid.h"
#include <stdlib.h>

namespace fibjs
{

result_t uuid_base::uuid(const char *s, obj_ptr<uuidValue_base> &retVal)
{
    obj_ptr<uuidVar> id = new uuidVar();

    uuid_create(&id->m_uid);
    uuid_import(id->m_uid, UUID_FMT_STR, s, qstrlen(s));
    retVal = id;

    return 0;
}

result_t uuid_base::uuid(Buffer_base *data, obj_ptr<uuidValue_base> &retVal)
{
    std::string s;

    data->toString(s);

    obj_ptr<uuidVar> id = new uuidVar();

    uuid_create(&id->m_uid);
    uuid_import(id->m_uid, UUID_FMT_BIN, s.c_str(), s.length());
    retVal = id;

    return 0;
}

static uuid_st *s_ns[4];
static const char *s_ns_str[4] =
{ "ns:DNS", "ns:URL", "ns:OID", "ns:X500" };

result_t makeUUID(int32_t vers, int32_t ns, const char *name,
                  obj_ptr<uuidValue_base> &retVal)
{
    if (ns < uuid_base::_DNS || ns > uuid_base::_X509)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<uuidVar> id = new uuidVar();

    uuid_create(&id->m_uid);

    if (vers == UUID_MAKE_V3 || vers == UUID_MAKE_V5)
    {
        if (!s_ns[ns])
        {
            uuid_create(&s_ns[ns]);
            uuid_load(s_ns[ns], s_ns_str[ns]);
        }

        uuid_make(id->m_uid, vers, s_ns[ns], name);
    }
    else
        uuid_make(id->m_uid, vers);

    retVal = id;

    return 0;
}

result_t uuid_base::node(obj_ptr<uuidValue_base> &retVal)
{
    return makeUUID(UUID_MAKE_V1, uuid_base::_X509, NULL, retVal);
}

result_t uuid_base::md5(int32_t ns, const char *name,
                        obj_ptr<uuidValue_base> &retVal)
{
    return makeUUID(UUID_MAKE_V3, ns, name, retVal);
}

result_t uuid_base::random(obj_ptr<uuidValue_base> &retVal)
{
    return makeUUID(UUID_MAKE_V4, uuid_base::_X509, NULL, retVal);
}

result_t uuid_base::sha1(int32_t ns, const char *name,
                         obj_ptr<uuidValue_base> &retVal)
{
    return makeUUID(UUID_MAKE_V5, ns, name, retVal);
}

uuidVar::~uuidVar()
{
    if (m_uid)
        uuid_destroy(m_uid);
}

result_t uuidVar::data(obj_ptr<Buffer_base> &retVal)
{
    if (m_uid)
    {
        void *gen = NULL;
        size_t len = 0;

        uuid_export(m_uid, UUID_FMT_BIN, &gen, &len);
        if (gen)
        {
            std::string s;

            s.append((const char *) gen, len);
            free(gen);

            retVal = new Buffer(s);
        }
    }

    return 0;
}

result_t uuidVar::toString(std::string &retVal)
{
    if (m_uid)
    {
        void *gen = NULL;
        size_t len = 0;

        uuid_export(m_uid, UUID_FMT_STR, &gen, &len);
        if (gen)
        {
            retVal.append((const char *) gen, len - 1);
            free(gen);
        }
    }

    return 0;
}

result_t uuidVar::detail(std::string &retVal)
{
    if (m_uid)
    {
        void *gen = NULL;
        size_t len = 0;

        uuid_export(m_uid, UUID_FMT_TXT, &gen, &len);
        if (gen)
        {
            retVal.append((const char *) gen, len - 1);
            free(gen);
        }
    }

    return 0;
}

} /* namespace fibjs */
