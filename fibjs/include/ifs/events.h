/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _events_base_H_
#define _events_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class EventEmitter_base;

class events_base : public object_base
{
    DECLARE_CLASS(events_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

};

}

#include "EventEmitter.h"

namespace fibjs
{
    inline ClassInfo& events_base::class_info()
    {
        static ClassData::ClassObject s_object[] = 
        {
            {"EventEmitter", EventEmitter_base::class_info}
        };

        static ClassData s_cd = 
        { 
            "events", true, s__new, NULL, 
            0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

}

#endif

