/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _X509Cert_base_H_
#define _X509Cert_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Buffer_base;
class PKey_base;

class X509Cert_base : public object_base {
    DECLARE_CLASS(X509Cert_base);

public:
    // X509Cert_base
    static result_t _new(obj_ptr<X509Cert_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t load(Buffer_base* derCert) = 0;
    virtual result_t load(exlib::string txtCert) = 0;
    virtual result_t loadFile(exlib::string filename) = 0;
    virtual result_t loadRootCerts() = 0;
    virtual result_t verify(X509Cert_base* cert, bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t dump(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t clear() = 0;
    virtual result_t get_version(int32_t& retVal) = 0;
    virtual result_t get_serial(exlib::string& retVal) = 0;
    virtual result_t get_issuer(exlib::string& retVal) = 0;
    virtual result_t get_subject(exlib::string& retVal) = 0;
    virtual result_t get_notBefore(date_t& retVal) = 0;
    virtual result_t get_notAfter(date_t& retVal) = 0;
    virtual result_t get_ca(bool& retVal) = 0;
    virtual result_t get_pathlen(int32_t& retVal) = 0;
    virtual result_t get_usage(exlib::string& retVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal) = 0;
    virtual result_t get_next(obj_ptr<X509Cert_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadRootCerts(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_verify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dump(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_serial(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_issuer(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_notBefore(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_notAfter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_pathlen(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_usage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_next(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(X509Cert_base, verify, X509Cert_base*, bool);
};
}

#include "Buffer.h"
#include "PKey.h"

namespace fibjs {
inline ClassInfo& X509Cert_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "load", s_load, false },
        { "loadFile", s_loadFile, false },
        { "loadRootCerts", s_loadRootCerts, false },
        { "verify", s_verify, false },
        { "verifySync", s_verify, false },
        { "dump", s_dump, false },
        { "clear", s_clear, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "version", s_get_version, block_set, false },
        { "serial", s_get_serial, block_set, false },
        { "issuer", s_get_issuer, block_set, false },
        { "subject", s_get_subject, block_set, false },
        { "notBefore", s_get_notBefore, block_set, false },
        { "notAfter", s_get_notAfter, block_set, false },
        { "ca", s_get_ca, block_set, false },
        { "pathlen", s_get_pathlen, block_set, false },
        { "usage", s_get_usage, block_set, false },
        { "type", s_get_type, block_set, false },
        { "publicKey", s_get_publicKey, block_set, false },
        { "next", s_get_next, block_set, false }
    };

    static ClassData s_cd = {
        "X509Cert", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void X509Cert_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void X509Cert_base::__new(const T& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_NAME("new X509Cert()");
    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void X509Cert_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("X509Cert.load");
    METHOD_INSTANCE(X509Cert_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->load(v0);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->load(v0);

    METHOD_VOID();
}

inline void X509Cert_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("X509Cert.loadFile");
    METHOD_INSTANCE(X509Cert_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->loadFile(v0);

    METHOD_VOID();
}

inline void X509Cert_base::s_loadRootCerts(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("X509Cert.loadRootCerts");
    METHOD_INSTANCE(X509Cert_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->loadRootCerts();

    METHOD_VOID();
}

inline void X509Cert_base::s_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("X509Cert.verify");
    METHOD_INSTANCE(X509Cert_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<X509Cert_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_verify(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_verify(v0, vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_dump(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("X509Cert.dump");
    METHOD_INSTANCE(X509Cert_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->dump(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("X509Cert.clear");
    METHOD_INSTANCE(X509Cert_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clear();

    METHOD_VOID();
}

inline void X509Cert_base::s_get_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("X509Cert.version");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_version(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_serial(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("X509Cert.serial");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_serial(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_issuer(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("X509Cert.issuer");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_issuer(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("X509Cert.subject");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_subject(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_notBefore(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("X509Cert.notBefore");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_notBefore(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_notAfter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("X509Cert.notAfter");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_notAfter(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("X509Cert.ca");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_ca(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_pathlen(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("X509Cert.pathlen");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_pathlen(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_usage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("X509Cert.usage");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_usage(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("X509Cert.type");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_NAME("X509Cert.publicKey");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_publicKey(vr);

    METHOD_RETURN();
}

inline void X509Cert_base::s_get_next(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_NAME("X509Cert.next");
    METHOD_INSTANCE(X509Cert_base);
    PROPERTY_ENTER();

    hr = pInst->get_next(vr);

    METHOD_RETURN();
}
}

#endif
