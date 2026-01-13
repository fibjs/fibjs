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

namespace fibjs {

class XmlNode_base;

class XMLSerializer_base : public object_base {
    DECLARE_CLASS(XMLSerializer_base);

public:
    // XMLSerializer_base
    static result_t _new(obj_ptr<XMLSerializer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t serializeToString(XmlNode_base* node, exlib::string& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<XMLSerializer_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_serializeToString(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/XmlNode.h"

namespace fibjs {
inline ClassInfo& XMLSerializer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "serializeToString", s_serializeToString, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "XMLSerializer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XMLSerializer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void XMLSerializer_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XMLSerializer_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t XMLSerializer_base::load(v8::Local<v8::Value> v, obj_ptr<XMLSerializer_base>& retVal)
{
    obj_ptr<XMLSerializer_base> vr;

    LOAD_ENTER();

    LOAD_RETURN();
}

inline void XMLSerializer_base::s_serializeToString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XMLSerializer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<XmlNode_base>, 0);

    hr = pInst->serializeToString(v0.get(), vr);

    METHOD_RETURN();
}
}
