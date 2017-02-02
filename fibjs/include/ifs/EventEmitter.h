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
    // EventEmitter_base
    static result_t _new(obj_ptr<EventEmitter_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
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


    inline void EventEmitter_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void EventEmitter_base::__new(const T& args)
    {
        obj_ptr<EventEmitter_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(0, 0);

        hr = _new(vr, args.This());

        CONSTRUCT_RETURN();
    }

}

#endif

