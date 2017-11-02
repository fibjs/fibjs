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
};
}

#include "ZmqSocket.h"

namespace fibjs {
inline ClassInfo& zmq_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "Socket", ZmqSocket_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "PAIR", _PAIR },
        { "PUB", _PUB },
        { "SUB", _SUB },
        { "REQ", _REQ },
        { "REP", _REP },
        { "DEALER", _DEALER },
        { "ROUTER", _ROUTER },
        { "PULL", _PULL },
        { "PUSH", _PUSH },
        { "XPUB", _XPUB },
        { "XSUB", _XSUB }
    };

    static ClassData s_cd = {
        "zmq", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}

#endif
