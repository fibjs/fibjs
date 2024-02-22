/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class crypto_constants_base : public object_base {
    DECLARE_CLASS(crypto_constants_base);

public:
    enum {
        C_RSA_PKCS1_PADDING = 1,
        C_RSA_NO_PADDING = 3,
        C_RSA_PKCS1_OAEP_PADDING = 4,
        C_RSA_X931_PADDING = 5,
        C_RSA_PKCS1_PSS_PADDING = 6,
        C_RSA_PSS_SALTLEN_DIGEST = -1,
        C_RSA_PSS_SALTLEN_MAX_SIGN = -2,
        C_RSA_PSS_SALTLEN_AUTO = -2
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }
};
}

namespace fibjs {
inline ClassInfo& crypto_constants_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "RSA_PKCS1_PADDING", C_RSA_PKCS1_PADDING },
        { "RSA_NO_PADDING", C_RSA_NO_PADDING },
        { "RSA_PKCS1_OAEP_PADDING", C_RSA_PKCS1_OAEP_PADDING },
        { "RSA_X931_PADDING", C_RSA_X931_PADDING },
        { "RSA_PKCS1_PSS_PADDING", C_RSA_PKCS1_PSS_PADDING },
        { "RSA_PSS_SALTLEN_DIGEST", C_RSA_PSS_SALTLEN_DIGEST },
        { "RSA_PSS_SALTLEN_MAX_SIGN", C_RSA_PSS_SALTLEN_MAX_SIGN },
        { "RSA_PSS_SALTLEN_AUTO", C_RSA_PSS_SALTLEN_AUTO }
    };

    static ClassData s_cd = {
        "crypto_constants", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
