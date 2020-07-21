/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _winreg_base_H_
#define _winreg_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Buffer_base;

class winreg_base : public object_base {
    DECLARE_CLASS(winreg_base);

public:
    // winreg_base
    static result_t listSubKey(exlib::string key, obj_ptr<NArray>& retVal);
    static result_t listValueName(exlib::string key, obj_ptr<NArray>& retVal);
    static result_t get(exlib::string key, exlib::string name, v8::Local<v8::Value>& retVal);
    static result_t set_dword(exlib::string key, exlib::string name, int32_t value);
    static result_t set_qword(exlib::string key, exlib::string name, int64_t value);
    static result_t set_sz(exlib::string key, exlib::string name, exlib::string value);
    static result_t set_expand_sz(exlib::string key, exlib::string name, exlib::string value);
    static result_t set_multi_sz(exlib::string key, exlib::string name, v8::Local<v8::Array> value);
    static result_t set_binary(exlib::string key, exlib::string name, Buffer_base* value);
    static result_t delKey(exlib::string key);
    static result_t delValue(exlib::string key, exlib::string name);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_listValueName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_dword(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_qword(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_sz(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_expand_sz(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_multi_sz(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_binary(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_delKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_delValue(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& winreg_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "listSubKey", s_static_listSubKey, true },
        { "listValueName", s_static_listValueName, true },
        { "get", s_static_get, true },
        { "set_dword", s_static_set_dword, true },
        { "set_qword", s_static_set_qword, true },
        { "set_sz", s_static_set_sz, true },
        { "set_expand_sz", s_static_set_expand_sz, true },
        { "set_multi_sz", s_static_set_multi_sz, true },
        { "set_binary", s_static_set_binary, true },
        { "delKey", s_static_delKey, true },
        { "delValue", s_static_delValue, true }
    };

    static ClassData s_cd = {
        "winreg", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void winreg_base::s_static_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("winreg.listSubKey");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = listSubKey(v0, vr);

    METHOD_RETURN();
}

inline void winreg_base::s_static_listValueName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("winreg.listValueName");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = listValueName(v0, vr);

    METHOD_RETURN();
}

inline void winreg_base::s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("winreg.get");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = get(v0, v1, vr);

    METHOD_RETURN();
}

inline void winreg_base::s_static_set_dword(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.set_dword");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(int32_t, 2);

    hr = set_dword(v0, v1, v2);

    METHOD_VOID();
}

inline void winreg_base::s_static_set_qword(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.set_qword");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(int64_t, 2);

    hr = set_qword(v0, v1, v2);

    METHOD_VOID();
}

inline void winreg_base::s_static_set_sz(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.set_sz");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);

    hr = set_sz(v0, v1, v2);

    METHOD_VOID();
}

inline void winreg_base::s_static_set_expand_sz(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.set_expand_sz");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);

    hr = set_expand_sz(v0, v1, v2);

    METHOD_VOID();
}

inline void winreg_base::s_static_set_multi_sz(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.set_multi_sz");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Array>, 2);

    hr = set_multi_sz(v0, v1, v2);

    METHOD_VOID();
}

inline void winreg_base::s_static_set_binary(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.set_binary");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    hr = set_binary(v0, v1, v2);

    METHOD_VOID();
}

inline void winreg_base::s_static_delKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.delKey");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = delKey(v0);

    METHOD_VOID();
}

inline void winreg_base::s_static_delValue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("winreg.delValue");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = delValue(v0, v1);

    METHOD_VOID();
}
}

#endif
