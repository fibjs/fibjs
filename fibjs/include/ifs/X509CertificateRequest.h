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

class KeyObject_base;
class Buffer_base;
class X509Certificate_base;

class X509CertificateRequest_base : public object_base {
    DECLARE_CLASS(X509CertificateRequest_base);

public:
    // X509CertificateRequest_base
    virtual result_t get_subject(exlib::string& retVal) = 0;
    virtual result_t get_publicKey(obj_ptr<KeyObject_base>& retVal) = 0;
    virtual result_t get_subjectAltName(exlib::string& retVal) = 0;
    virtual result_t get_infoAccess(exlib::string& retVal) = 0;
    virtual result_t get_raw(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t get_pem(exlib::string& retVal) = 0;
    virtual result_t checkPrivateKey(KeyObject_base* privateKey, bool& retVal) = 0;
    virtual result_t issue(v8::Local<v8::Object> options, obj_ptr<X509Certificate_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_subject(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_publicKey(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_subjectAltName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_infoAccess(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_raw(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_pem(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_checkPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_issue(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/KeyObject.h"
#include "ifs/Buffer.h"
#include "ifs/X509Certificate.h"

namespace fibjs {
inline ClassInfo& X509CertificateRequest_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "checkPrivateKey", s_checkPrivateKey, false, false },
        { "issue", s_issue, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "subject", s_get_subject, block_set, false },
        { "publicKey", s_get_publicKey, block_set, false },
        { "subjectAltName", s_get_subjectAltName, block_set, false },
        { "infoAccess", s_get_infoAccess, block_set, false },
        { "raw", s_get_raw, block_set, false },
        { "pem", s_get_pem, block_set, false }
    };

    static ClassData s_cd = {
        "X509CertificateRequest", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void X509CertificateRequest_base::s_get_subject(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    PROPERTY_ENTER();

    hr = pInst->get_subject(vr);

    METHOD_RETURN();
}

inline void X509CertificateRequest_base::s_get_publicKey(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<KeyObject_base> vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    PROPERTY_ENTER();

    hr = pInst->get_publicKey(vr);

    METHOD_RETURN();
}

inline void X509CertificateRequest_base::s_get_subjectAltName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    PROPERTY_ENTER();

    hr = pInst->get_subjectAltName(vr);

    METHOD_RETURN();
}

inline void X509CertificateRequest_base::s_get_infoAccess(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    PROPERTY_ENTER();

    hr = pInst->get_infoAccess(vr);

    METHOD_RETURN();
}

inline void X509CertificateRequest_base::s_get_raw(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    PROPERTY_ENTER();

    hr = pInst->get_raw(vr);

    METHOD_RETURN();
}

inline void X509CertificateRequest_base::s_get_pem(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    PROPERTY_ENTER();

    hr = pInst->get_pem(vr);

    METHOD_RETURN();
}

inline void X509CertificateRequest_base::s_checkPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<KeyObject_base>, 0);

    hr = pInst->checkPrivateKey(v0, vr);

    METHOD_RETURN();
}

inline void X509CertificateRequest_base::s_issue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Certificate_base> vr;

    METHOD_INSTANCE(X509CertificateRequest_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->issue(v0, vr);

    METHOD_RETURN();
}
}
