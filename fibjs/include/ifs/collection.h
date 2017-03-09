/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _collection_base_H_
#define _collection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Map_base;
class List_base;
class Queue_base;

class collection_base : public object_base
{
    DECLARE_CLASS(collection_base);

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

#include "Map.h"
#include "List.h"
#include "Queue.h"

namespace fibjs
{
    inline ClassInfo& collection_base::class_info()
    {
        static ClassData::ClassObject s_object[] = 
        {
            {"Map", Map_base::class_info},
            {"List", List_base::class_info},
            {"Queue", Queue_base::class_info}
        };

        static ClassData s_cd = 
        { 
            "collection", true, s__new, NULL, 
            0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

}

#endif

