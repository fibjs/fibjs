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

class X509Certificate_base;
class KeyObject_base;

class SecureContext_base : public object_base {
    DECLARE_CLASS(SecureContext_base);

public:
    // SecureContext_base
    virtual result_t get_ca(obj_ptr<X509Certificate_base>& retVal) = 0;
    virtual result_t get_key(obj_ptr<KeyObject_base>& retVal) = 0;
    virtual result_t get_cert(obj_ptr<X509Certificate_base>& retVal) = 0;
    virtual result_t get_maxVersion(exlib::string& retVal) = 0;
    virtual result_t get_minVersion(exlib::string& retVal) = 0;
    virtual result_t get_secureProtocol(exlib::string& retVal) = 0;
    virtual result_t get_requestCert(bool& retVal) = 0;
    virtual result_t get_rejectUnverified(bool& retVal) = 0;
    virtual result_t get_rejectUnauthorized(bool& retVal) = 0;
    virtual result_t get_sessionTimeout(int32_t& retVal) = 0;
    virtual result_t setSNIContext(exlib::string servername, SecureContext_base* context) = 0;
    virtual result_t setSNIContext(exlib::string servername, v8::Local<v8::Object> options) = 0;
    virtual result_t getSNIContext(exlib::string servername, bool auto_resolve, obj_ptr<SecureContext_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t removeSNIContext(exlib::string servername) = 0;
    virtual result_t clearSNIContexts() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_key(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_cert(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_maxVersion(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_minVersion(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_secureProtocol(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_requestCert(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_rejectUnverified(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_rejectUnauthorized(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_sessionTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_setSNIContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSNIContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeSNIContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clearSNIContexts(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(SecureContext_base, getSNIContext, exlib::string, bool, obj_ptr<SecureContext_base>);
};
}

#include "ifs/X509Certificate.h"
#include "ifs/KeyObject.h"

namespace fibjs {
inline ClassInfo& SecureContext_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setSNIContext", s_setSNIContext, false, ClassData::ASYNC_SYNC },
        { "getSNIContext", s_getSNIContext, false, ClassData::ASYNC_ASYNC },
        { "removeSNIContext", s_removeSNIContext, false, ClassData::ASYNC_SYNC },
        { "clearSNIContexts", s_clearSNIContexts, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "ca", s_get_ca, block_set, false },
        { "key", s_get_key, block_set, false },
        { "cert", s_get_cert, block_set, false },
        { "maxVersion", s_get_maxVersion, block_set, false },
        { "minVersion", s_get_minVersion, block_set, false },
        { "secureProtocol", s_get_secureProtocol, block_set, false },
        { "requestCert", s_get_requestCert, block_set, false },
        { "rejectUnverified", s_get_rejectUnverified, block_set, false },
        { "rejectUnauthorized", s_get_rejectUnauthorized, block_set, false },
        { "sessionTimeout", s_get_sessionTimeout, block_set, false }
    };

    static ClassData s_cd = {
        "SecureContext", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void SecureContext_base::s_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Certificate_base> vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_ca(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_key(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<KeyObject_base> vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_key(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_cert(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Certificate_base> vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_cert(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_maxVersion(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxVersion(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_minVersion(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_minVersion(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_secureProtocol(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_secureProtocol(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_requestCert(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_requestCert(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_rejectUnverified(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_rejectUnverified(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_rejectUnauthorized(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_rejectUnauthorized(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_get_sessionTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(SecureContext_base);
    PROPERTY_ENTER();

    hr = pInst->get_sessionTimeout(vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_setSNIContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SecureContext_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<SecureContext_base>, 1);

    hr = pInst->setSNIContext(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = pInst->setSNIContext(v0, v1);

    METHOD_VOID();
}

inline void SecureContext_base::s_getSNIContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    ASYNC_METHOD_INSTANCE(SecureContext_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);

    if (!cb.IsEmpty())
        hr = pInst->acb_getSNIContext(v0, v1, cb, args);
    else
        hr = pInst->ac_getSNIContext(v0, v1, vr);

    METHOD_RETURN();
}

inline void SecureContext_base::s_removeSNIContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SecureContext_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->removeSNIContext(v0);

    METHOD_VOID();
}

inline void SecureContext_base::s_clearSNIContexts(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SecureContext_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clearSNIContexts();

    METHOD_VOID();
}
}
