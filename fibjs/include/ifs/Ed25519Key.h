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
#include "ifs/ECCKey.h"

namespace fibjs {

class ECCKey_base;

class Ed25519Key_base : public ECCKey_base {
    DECLARE_CLASS(Ed25519Key_base);

public:
    // Ed25519Key_base
    virtual result_t toX25519(obj_ptr<ECCKey_base>& retVal, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_toX25519(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(Ed25519Key_base, toX25519, obj_ptr<ECCKey_base>);
};
}

namespace fibjs {
inline ClassInfo& Ed25519Key_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "toX25519", s_toX25519, false, true },
        { "toX25519Sync", s_toX25519, false, false }
    };

    static ClassData s_cd = {
        "Ed25519Key", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &ECCKey_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Ed25519Key_base::s_toX25519(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ECCKey_base> vr;

    METHOD_NAME("Ed25519Key.toX25519");
    METHOD_INSTANCE(Ed25519Key_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_toX25519(cb, args);
    else
        hr = pInst->ac_toX25519(vr);

    METHOD_RETURN();
}
}
