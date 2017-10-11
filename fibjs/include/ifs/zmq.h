/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _zmq_base_H_
#define _zmq_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class ZmqSocket_base;

class zmq_base : public object_base {
    DECLARE_CLASS(zmq_base);

public:
    enum {
        _PAIR = 0,
        _PUB = 1,
        _SUB = 2,
        _REQ = 3,
        _REP = 4,
        _DEALER = 5,
        _ROUTER = 6,
        _PULL = 7,
        _PUSH = 8,
        _XPUB = 9,
        _XSUB = 10
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_PAIR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_PUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_SUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_REQ(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_REP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_DEALER(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ROUTER(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_PULL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_PUSH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_XPUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_XSUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ZmqSocket.h"

namespace fibjs {
inline ClassInfo& zmq_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "Socket", ZmqSocket_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "PAIR", s_get_PAIR, block_set, true },
        { "PUB", s_get_PUB, block_set, true },
        { "SUB", s_get_SUB, block_set, true },
        { "REQ", s_get_REQ, block_set, true },
        { "REP", s_get_REP, block_set, true },
        { "DEALER", s_get_DEALER, block_set, true },
        { "ROUTER", s_get_ROUTER, block_set, true },
        { "PULL", s_get_PULL, block_set, true },
        { "PUSH", s_get_PUSH, block_set, true },
        { "XPUB", s_get_XPUB, block_set, true },
        { "XSUB", s_get_XSUB, block_set, true }
    };

    static ClassData s_cd = {
        "zmq", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void zmq_base::s_get_PAIR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.PAIR");
    int32_t vr = _PAIR;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_PUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.PUB");
    int32_t vr = _PUB;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_SUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.SUB");
    int32_t vr = _SUB;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_REQ(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.REQ");
    int32_t vr = _REQ;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_REP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.REP");
    int32_t vr = _REP;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_DEALER(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.DEALER");
    int32_t vr = _DEALER;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_ROUTER(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.ROUTER");
    int32_t vr = _ROUTER;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_PULL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.PULL");
    int32_t vr = _PULL;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_PUSH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.PUSH");
    int32_t vr = _PUSH;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_XPUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.XPUB");
    int32_t vr = _XPUB;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void zmq_base::s_get_XSUB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("zmq.XSUB");
    int32_t vr = _XSUB;
    PROPERTY_ENTER();
    METHOD_RETURN();
}
}

#endif
