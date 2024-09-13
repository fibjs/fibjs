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

class Buffer_base;
class KeyObject_base;

class X509Certificate_base : public object_base {
    DECLARE_CLASS(X509Certificate_base);

public:
    // X509Certificate_base
    static result_t _new(Buffer_base* cert, obj_ptr<X509Certificate_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Array> certs, obj_ptr<X509Certificate_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_subject(exlib::string& retVal) = 0;
    virtual result_t get_serialNumber(exlib::string& retVal) = 0;
    virtual result_t get_publicKey(obj_ptr<KeyObject_base>& retVal) = 0;
    virtual result_t get_subjectAltName(exlib::string& retVal) = 0;
    virtual result_t get_infoAccess(exlib::string& retVal) = 0;
    virtual result_t get_issuer(exlib::string& retVal) = 0;
    virtual result_t get_ca(bool& retVal) = 0;
    virtual result_t get_pathlen(int32_t& retVal) = 0;
    virtual result_t get_keyUsage(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t get_type(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t get_validFrom(exlib::string& retVal) = 0;
    virtual result_t get_validTo(exlib::string& retVal) = 0;
    virtual result_t get_raw(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t get_pem(exlib::string& retVal) = 0;
    virtual result_t get_fingerprint(exlib::string& retVal) = 0;
    virtual result_t get_fingerprint256(exlib::string& retVal) = 0;
    virtual result_t get_fingerprint512(exlib::string& retVal) = 0;
    virtual result_t next(obj_ptr<X509Certificate_base>& retVal) = 0;
    virtual result_t checkEmail(exlib::string email, v8::Local<v8::Object> options, exlib::string& retVal) = 0;
    virtual result_t checkHost(exlib::string name, v8::Local<v8::Object> options, exlib::string& retVal) = 0;
    virtual result_t checkIP(exlib::string ip, exlib::string& retVal) = 0;
    virtual result_t checkIssued(X509Certificate_base* issuer, bool& retVal) = 0;
    virtual result_t checkPrivateKey(KeyObject_base* privateKey, bool& retVal) = 0;
    virtual result_t verify(KeyObject_base* publicKey, bool& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_subject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_serialNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_publicKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_subjectAltName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_infoAccess(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_issuer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ca(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_pathlen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_keyUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_validFrom(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_validTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_raw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_pem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_fingerprint(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_fingerprint256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_fingerprint512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_next(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_checkEmail(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_checkHost(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_checkIP(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_checkIssued(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_checkPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_verify(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"
#include "ifs/KeyObject.h"

namespace fibjs {
inline ClassInfo& X509Certificate_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "next", s_next, false, ClassData::ASYNC_SYNC },
        { "checkEmail", s_checkEmail, false, ClassData::ASYNC_SYNC },
        { "checkHost", s_checkHost, false, ClassData::ASYNC_SYNC },
        { "checkIP", s_checkIP, false, ClassData::ASYNC_SYNC },
        { "checkIssued", s_checkIssued, false, ClassData::ASYNC_SYNC },
        { "checkPrivateKey", s_checkPrivateKey, false, ClassData::ASYNC_SYNC },
        { "verify", s_verify, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "subject", s_get_subject, block_set, false },
        { "serialNumber", s_get_serialNumber, block_set, false },
        { "publicKey", s_get_publicKey, block_set, false },
        { "subjectAltName", s_get_subjectAltName, block_set, false },
        { "infoAccess", s_get_infoAccess, block_set, false },
        { "issuer", s_get_issuer, block_set, false },
        { "ca", s_get_ca, block_set, false },
        { "pathlen", s_get_pathlen, block_set, false },
        { "keyUsage", s_get_keyUsage, block_set, false },
        { "type", s_get_type, block_set, false },
        { "validFrom", s_get_validFrom, block_set, false },
        { "validTo", s_get_validTo, block_set, false },
        { "raw", s_get_raw, block_set, false },
        { "pem", s_get_pem, block_set, false },
        { "fingerprint", s_get_fingerprint, block_set, false },
        { "fingerprint256", s_get_fingerprint256, block_set, false },
        { "fingerprint512", s_get_fingerprint512, block_set, false }
    };

    static ClassData s_cd = {
        "X509Certificate", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void X509Certificate_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void X509Certificate_base::__new(const T& args)
{
    obj_ptr<X509Certificate_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void X509Certificate_base::s_get_subject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_subject(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_serialNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_serialNumber(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_publicKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<KeyObject_base> vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_publicKey(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_subjectAltName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_subjectAltName(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_infoAccess(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_infoAccess(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_issuer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_issuer(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_ca(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ca(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_pathlen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_pathlen(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_keyUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_keyUsage(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_validFrom(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_validFrom(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_validTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_validTo(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_raw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_raw(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_pem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_pem(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_fingerprint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_fingerprint(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_fingerprint256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_fingerprint256(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_get_fingerprint512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_fingerprint512(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_next(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Certificate_base> vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->next(vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_checkEmail(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->checkEmail(v0, v1, vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_checkHost(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->checkHost(v0, v1, vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_checkIP(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->checkIP(v0, vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_checkIssued(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<X509Certificate_base>, 0);

    hr = pInst->checkIssued(v0, vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_checkPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<KeyObject_base>, 0);

    hr = pInst->checkPrivateKey(v0, vr);

    METHOD_RETURN();
}

inline void X509Certificate_base::s_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(X509Certificate_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<KeyObject_base>, 0);

    hr = pInst->verify(v0, vr);

    METHOD_RETURN();
}
}
