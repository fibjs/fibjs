/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _EventEmitter_base_H_
#define _EventEmitter_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;

class EventEmitter_base : public Trigger_base
{
    DECLARE_CLASS(EventEmitter_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:

};

}


namespace fibjs
{
    inline ClassInfo& EventEmitter_base::class_info()
    {
        static ClassData s_cd = 
        { 
            "EventEmitter", s__new, NULL, 
            0, NULL, 0, NULL, 0, NULL, NULL, NULL,
            &Trigger_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


}

#endif

