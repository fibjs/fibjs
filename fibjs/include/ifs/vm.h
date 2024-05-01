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

class SandBox_base;
class Script_base;

class vm_base : public object_base {
    DECLARE_CLASS(vm_base);

public:
    // vm_base
    static result_t createContext(v8::Local<v8::Object> contextObject, v8::Local<v8::Object> opts, v8::Local<v8::Object>& retVal);
    static result_t isContext(v8::Local<v8::Object> contextObject, bool& retVal);
    static result_t runInContext(exlib::string code, v8::Local<v8::Object> contextifiedObject, v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal);
    static result_t runInContext(exlib::string code, v8::Local<v8::Object> contextifiedObject, exlib::string filename, v8::Local<v8::Value>& retVal);
    static result_t runInNewContext(exlib::string code, v8::Local<v8::Object> contextObject, v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal);
    static result_t runInNewContext(exlib::string code, v8::Local<v8::Object> contextObject, exlib::string filename, v8::Local<v8::Value>& retVal);
    static result_t runInThisContext(exlib::string code, v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal);
    static result_t runInThisContext(exlib::string code, exlib::string filename, v8::Local<v8::Value>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_createContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_runInContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_runInNewContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_runInThisContext(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/SandBox.h"
#include "ifs/Script.h"

namespace fibjs {
inline ClassInfo& vm_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createContext", s_static_createContext, true, ClassData::ASYNC_SYNC },
        { "isContext", s_static_isContext, true, ClassData::ASYNC_SYNC },
        { "runInContext", s_static_runInContext, true, ClassData::ASYNC_SYNC },
        { "runInNewContext", s_static_runInNewContext, true, ClassData::ASYNC_SYNC },
        { "runInThisContext", s_static_runInThisContext, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "SandBox", SandBox_base::class_info },
        { "Script", Script_base::class_info }
    };

    static ClassData s_cd = {
        "vm", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void vm_base::s_static_createContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = createContext(v0, v1, vr);

    METHOD_RETURN();
}

inline void vm_base::s_static_isContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = isContext(v0, vr);

    METHOD_RETURN();
}

inline void vm_base::s_static_runInContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = runInContext(v0, v1, v2, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(exlib::string, 2);

    hr = runInContext(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void vm_base::s_static_runInNewContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = runInNewContext(v0, v1, v2, vr);

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));
    ARG(exlib::string, 2);

    hr = runInNewContext(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void vm_base::s_static_runInThisContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = runInThisContext(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = runInThisContext(v0, v1, vr);

    METHOD_RETURN();
}
}
