/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _dgram_base_H_
#define _dgram_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class DgramSocket_base;

class dgram_base : public object_base {
    DECLARE_CLASS(dgram_base);

public:
    // dgram_base
    static result_t createSocket(v8::Local<v8::Object> opts, obj_ptr<DgramSocket_base>& retVal);
    static result_t createSocket(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<DgramSocket_base>& retVal);
    static result_t createSocket(exlib::string type, obj_ptr<DgramSocket_base>& retVal);
    static result_t createSocket(exlib::string type, v8::Local<v8::Function> callback, obj_ptr<DgramSocket_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_createSocket(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "DgramSocket.h"

namespace fibjs {
inline ClassInfo& dgram_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createSocket", s_createSocket, true }
    };

    static ClassData s_cd = {
        "dgram", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void dgram_base::s_createSocket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DgramSocket_base> vr;

    METHOD_NAME("dgram.createSocket");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = createSocket(v0, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = createSocket(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = createSocket(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = createSocket(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
