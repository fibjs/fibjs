/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _vm_base_H_
#define _vm_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class SandBox_base;

class vm_base : public object_base
{
    DECLARE_CLASS(vm_base);

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

#include "SandBox.h"

namespace fibjs
{
    inline ClassInfo& vm_base::class_info()
    {
        static ClassData::ClassObject s_object[] = 
        {
            {"SandBox", SandBox_base::class_info}
        };

        static ClassData s_cd = 
        { 
            "vm", true, s__new, NULL, 
            0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

}

#endif

