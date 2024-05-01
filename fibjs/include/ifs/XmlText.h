/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/XmlCharacterData.h"

namespace fibjs {

class XmlCharacterData_base;

class XmlText_base : public XmlCharacterData_base {
    DECLARE_CLASS(XmlText_base);

public:
    // XmlText_base
    virtual result_t splitText(int32_t offset, obj_ptr<XmlText_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_splitText(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& XmlText_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "splitText", s_splitText, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "XmlText", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &XmlCharacterData_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlText_base::s_splitText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlText_base> vr;

    METHOD_INSTANCE(XmlText_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->splitText(v0, vr);

    METHOD_RETURN();
}
}
