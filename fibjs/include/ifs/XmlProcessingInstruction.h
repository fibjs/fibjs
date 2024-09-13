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
#include "ifs/XmlNode.h"

namespace fibjs {

class XmlNode_base;

class XmlProcessingInstruction_base : public XmlNode_base {
    DECLARE_CLASS(XmlProcessingInstruction_base);

public:
    // XmlProcessingInstruction_base
    virtual result_t get_target(exlib::string& retVal) = 0;
    virtual result_t get_data(exlib::string& retVal) = 0;
    virtual result_t set_data(exlib::string newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_target(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_data(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& XmlProcessingInstruction_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "target", s_get_target, block_set, false },
        { "data", s_get_data, s_set_data, false }
    };

    static ClassData s_cd = {
        "XmlProcessingInstruction", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &XmlNode_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlProcessingInstruction_base::s_get_target(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlProcessingInstruction_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_target(vr);

    METHOD_RETURN();
}

inline void XmlProcessingInstruction_base::s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlProcessingInstruction_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_data(vr);

    METHOD_RETURN();
}

inline void XmlProcessingInstruction_base::s_set_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlProcessingInstruction_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_data(v0);

    METHOD_VOID();
}
}
