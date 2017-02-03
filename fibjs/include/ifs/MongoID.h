/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoID_base_H_
#define _MongoID_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoID_base : public object_base
{
    DECLARE_CLASS(MongoID_base);

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

namespace fibjs
{
    inline ClassInfo& MongoID_base::class_info()
    {
        static ClassData s_cd = 
        { 
            "MongoID", s__new, NULL, 
            0, NULL, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

}

#endif

