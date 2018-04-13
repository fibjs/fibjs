/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RadosStat_base_H_
#define _RadosStat_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class RadosStat_base : public object_base {
    DECLARE_CLASS(RadosStat_base);

public:
    // RadosStat_base
    virtual result_t get_size(int64_t& retVal) = 0;
    virtual result_t get_mtime(date_t& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_size(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_mtime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& RadosStat_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "size", s_get_size, block_set, false },
        { "mtime", s_get_mtime, block_set, false }
    };

    static ClassData s_cd = {
        "RadosStat", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RadosStat_base::s_get_size(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("RadosStat.size");
    METHOD_INSTANCE(RadosStat_base);
    PROPERTY_ENTER();

    hr = pInst->get_size(vr);

    METHOD_RETURN();
}

inline void RadosStat_base::s_get_mtime(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("RadosStat.mtime");
    METHOD_INSTANCE(RadosStat_base);
    PROPERTY_ENTER();

    hr = pInst->get_mtime(vr);

    METHOD_RETURN();
}
}

#endif
