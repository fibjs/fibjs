/*
 * PKey_json.cpp
 *
 *  Created on: May 4, 2021
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/crypto.h"
#include "PKey.h"
#include "PKey_impl.h"
#include "ssl.h"
#include "encoding.h"

namespace fibjs {

result_t PKey_base::_new(v8::Local<v8::Object> jsonKey, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    return from(jsonKey, retVal);
}

static void mpi_dump(Isolate* isolate, v8::Local<v8::Object> o, exlib::string key, const mbedtls_mpi* n)
{
    exlib::string data;
    int32_t sz = (int32_t)mbedtls_mpi_size(n);
    if (sz) {
        data.resize(sz);
        mbedtls_mpi_write_binary(n, (unsigned char*)data.c_buffer(), sz);

        exlib::string b64;
        base64Encode(data.c_str(), data.length(), true, b64);

        o->Set(isolate->context(), isolate->NewString(key), isolate->NewString(b64));
    }
}

static result_t mpi_load(Isolate* isolate, mbedtls_mpi* n, v8::Local<v8::Object> o, const char* key)
{
    exlib::string b64, s;
    result_t hr;

    hr = GetConfigValue(isolate->m_isolate, o, key, b64);
    if (hr < 0)
        return hr;

    base64Decode(b64, s);

    int32_t ret;
    ret = mbedtls_mpi_read_binary(n, (unsigned char*)s.c_str(), s.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

static const mbedtls_pk_info_t* get_pk_info_from_curve(int32_t id)
{
    mbedtls_pk_type_t pk_type;

    if (id == MBEDTLS_ECP_DP_SM2P256R1)
        pk_type = MBEDTLS_PK_SM2;
    else
        pk_type = MBEDTLS_PK_ECKEY;

    return mbedtls_pk_info_from_type(pk_type);
}

result_t PKey_base::from(v8::Local<v8::Object> jsonKey, obj_ptr<PKey_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    int32_t ret;
    result_t hr;
    exlib::string kty;
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);

    hr = GetConfigValue(isolate->m_isolate, jsonKey, "kty", kty);
    if (hr < 0)
        return hr;

    if (kty == "RSA") {
        ret = mbedtls_pk_setup(&ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(ctx);

        do {
            hr = mpi_load(isolate, &rsa->N, jsonKey, "n");
            if (hr < 0)
                break;

            hr = mpi_load(isolate, &rsa->E, jsonKey, "e");
            if (hr < 0)
                break;

            rsa->len = mbedtls_mpi_size(&rsa->N);

            ret = mbedtls_rsa_check_pubkey(rsa);
            if (ret != 0) {
                hr = CHECK_ERROR(_ssl::setError(ret));
                break;
            }

            hr = mpi_load(isolate, &rsa->D, jsonKey, "d");
            if (hr >= 0) {
                hr = mpi_load(isolate, &rsa->P, jsonKey, "p");
                if (hr < 0)
                    break;

                hr = mpi_load(isolate, &rsa->Q, jsonKey, "q");
                if (hr < 0)
                    break;

                hr = mpi_load(isolate, &rsa->DP, jsonKey, "dp");
                if (hr < 0)
                    break;

                hr = mpi_load(isolate, &rsa->DQ, jsonKey, "dq");
                if (hr < 0)
                    break;

                hr = mpi_load(isolate, &rsa->QP, jsonKey, "qi");
                if (hr < 0)
                    break;

                ret = mbedtls_rsa_check_privkey(rsa);
                if (ret != 0) {
                    hr = CHECK_ERROR(_ssl::setError(ret));
                    break;
                }
            } else if (hr == CALL_E_PARAMNOTOPTIONAL)
                hr = 0;
        } while (false);
    } else if (kty == "EC" || kty == "OKP") {
        exlib::string curve;

        hr = GetConfigValue(isolate->m_isolate, jsonKey, "crv", curve);
        if (hr < 0)
            return hr;

        int32_t id = PKey_ecc::get_curve_id(curve);

        if (id == MBEDTLS_ECP_DP_NONE)
            return CHECK_ERROR(Runtime::setError("PKey: Unknown curve"));

        ret = mbedtls_pk_setup(&ctx, get_pk_info_from_curve(id));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);

        PKey_ecc::load_group(&ecp->grp, (mbedtls_ecp_group_id)id);

        do {
            hr = mpi_load(isolate, &ecp->d, jsonKey, "d");
            if (hr >= 0) {
                if (id < MBEDTLS_ECP_DP_ED25519) {
                    ret = mbedtls_ecp_check_privkey(&ecp->grp, &ecp->d);
                    if (ret != 0) {
                        hr = CHECK_ERROR(_ssl::setError(ret));
                        break;
                    }
                }
            } else if (hr != CALL_E_PARAMNOTOPTIONAL)
                break;

            hr = mpi_load(isolate, &ecp->Q.X, jsonKey, "x");
            if (hr == CALL_E_PARAMNOTOPTIONAL) {
                hr = 0;
                break;
            }

            mpi_load(isolate, &ecp->Q.Y, jsonKey, "y");
            mbedtls_mpi_lset(&ecp->Q.Z, 1);

            if (id < MBEDTLS_ECP_DP_ED25519) {
                ret = mbedtls_ecp_check_pubkey(&ecp->grp, &ecp->Q);
                if (ret != 0) {
                    hr = CHECK_ERROR(_ssl::setError(ret));
                    break;
                }
            }
        } while (false);
    }

    if (hr < 0) {
        mbedtls_pk_free(&ctx);
        return hr;
    }

    retVal = PKey::create(ctx);

    return 0;
}

result_t PKey::json(v8::Local<v8::Object>& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);

    if (!type)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (type == MBEDTLS_PK_RSA) {
        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

        o->Set(context, isolate->NewString("kty"), isolate->NewString("RSA"));
        mpi_dump(isolate, o, "n", &rsa->N);
        mpi_dump(isolate, o, "e", &rsa->E);

        if (priv) {
            mpi_dump(isolate, o, "d", &rsa->D);
            mpi_dump(isolate, o, "p", &rsa->P);
            mpi_dump(isolate, o, "q", &rsa->Q);
            mpi_dump(isolate, o, "dp", &rsa->DP);
            mpi_dump(isolate, o, "dq", &rsa->DQ);
            mpi_dump(isolate, o, "qi", &rsa->QP);
        }

        retVal = o;
        return 0;
    } else {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
        const char* _name = PKey_ecc::get_curve_name(ecp->grp.id);

        o->Set(context, isolate->NewString("kty"),
            isolate->NewString(ecp->grp.id == MBEDTLS_ECP_DP_ED25519 ? "OKP" : "EC"));

        if (_name)
            o->Set(context, isolate->NewString("crv"), isolate->NewString(_name));

        mpi_dump(isolate, o, "x", &ecp->Q.X);
        mpi_dump(isolate, o, "y", &ecp->Q.Y);

        if (priv)
            mpi_dump(isolate, o, "d", &ecp->d);

        retVal = o;
        return 0;
    }

    return 0;
}

}
