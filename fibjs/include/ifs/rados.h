/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _rados_base_H_
#define _rados_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class RadosCluster_base;

class rados_base : public object_base {
    DECLARE_CLASS(rados_base);

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

#include "RadosCluster.h"

namespace fibjs {
inline ClassInfo& rados_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "Rados", RadosCluster_base::class_info }
    };

    static ClassData s_cd = {
        "rados", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}

#endif
