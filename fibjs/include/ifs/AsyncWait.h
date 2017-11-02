/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _AsyncWait_base_H_
#define _AsyncWait_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs {

class Handler_base;

class AsyncWait_base : public Handler_base {
    DECLARE_CLASS(AsyncWait_base);

public:
    // AsyncWait_base
    virtual result_t end() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_end(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& AsyncWait_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "end", s_end, false }
    };

    static ClassData s_cd = {
        "AsyncWait", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &Handler_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void AsyncWait_base::s_end(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("AsyncWait.end");
    METHOD_INSTANCE(AsyncWait_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->end();

    METHOD_VOID();
}
}

#endif
