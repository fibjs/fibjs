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

class XmlDocument_base;

class DOMParser_base : public object_base {
    DECLARE_CLASS(DOMParser_base);

public:
    // DOMParser_base
    static result_t _new(obj_ptr<DOMParser_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t parseFromString(exlib::string string, exlib::string mimeType, obj_ptr<XmlDocument_base>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<DOMParser_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_parseFromString(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/XmlDocument.h"

namespace fibjs {
inline ClassInfo& DOMParser_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "parseFromString", s_parseFromString, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "DOMParser", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DOMParser_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void DOMParser_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DOMParser_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t DOMParser_base::load(v8::Local<v8::Value> v, obj_ptr<DOMParser_base>& retVal)
{
    obj_ptr<DOMParser_base> vr;

    LOAD_ENTER();

    LOAD_RETURN();
}

inline void DOMParser_base::s_parseFromString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlDocument_base> vr;

    METHOD_INSTANCE(DOMParser_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->parseFromString(v0, v1, vr);

    METHOD_RETURN();
}
}
