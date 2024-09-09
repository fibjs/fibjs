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

class path_posix_base : public object_base {
    DECLARE_CLASS(path_posix_base);

public:
    // path_posix_base
    static result_t normalize(exlib::string path, exlib::string& retVal);
    static result_t basename(exlib::string path, exlib::string ext, exlib::string& retVal);
    static result_t extname(exlib::string path, exlib::string& retVal);
    static result_t format(v8::Local<v8::Object> pathObject, exlib::string& retVal);
    static result_t parse(exlib::string path, obj_ptr<NObject>& retVal);
    static result_t dirname(exlib::string path, exlib::string& retVal);
    static result_t fullpath(exlib::string path, exlib::string& retVal);
    static result_t isAbsolute(exlib::string path, bool& retVal);
    static result_t join(OptArgs ps, exlib::string& retVal);
    static result_t resolve(OptArgs ps, exlib::string& retVal);
    static result_t relative(exlib::string _from, exlib::string to, exlib::string& retVal);
    static result_t toNamespacedPath(v8::Local<v8::Value> path, v8::Local<v8::Value>& retVal);
    static result_t get_sep(exlib::string& retVal);
    static result_t get_delimiter(exlib::string& retVal);
    static result_t get_posix(v8::Local<v8::Object>& retVal);
    static result_t get_win32(v8::Local<v8::Object>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_basename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_extname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dirname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fullpath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isAbsolute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_join(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_relative(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_toNamespacedPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_sep(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_delimiter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_posix(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_win32(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& path_posix_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "normalize", s_static_normalize, true, ClassData::ASYNC_SYNC },
        { "basename", s_static_basename, true, ClassData::ASYNC_SYNC },
        { "extname", s_static_extname, true, ClassData::ASYNC_SYNC },
        { "format", s_static_format, true, ClassData::ASYNC_SYNC },
        { "parse", s_static_parse, true, ClassData::ASYNC_SYNC },
        { "dirname", s_static_dirname, true, ClassData::ASYNC_SYNC },
        { "fullpath", s_static_fullpath, true, ClassData::ASYNC_SYNC },
        { "isAbsolute", s_static_isAbsolute, true, ClassData::ASYNC_SYNC },
        { "join", s_static_join, true, ClassData::ASYNC_SYNC },
        { "resolve", s_static_resolve, true, ClassData::ASYNC_SYNC },
        { "relative", s_static_relative, true, ClassData::ASYNC_SYNC },
        { "toNamespacedPath", s_static_toNamespacedPath, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "sep", s_static_get_sep, block_set, true },
        { "delimiter", s_static_get_delimiter, block_set, true },
        { "posix", s_static_get_posix, block_set, true },
        { "win32", s_static_get_win32, block_set, true }
    };

    static ClassData s_cd = {
        "path_posix", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void path_posix_base::s_static_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = normalize(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_basename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = basename(v0, v1, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_extname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = extname(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = format(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = parse(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_dirname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = dirname(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_fullpath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = fullpath(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_isAbsolute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = isAbsolute(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_join(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = join(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = resolve(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_relative(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = relative(v0, v1, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_toNamespacedPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate->m_isolate));

    hr = toNamespacedPath(v0, vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_get_sep(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_sep(vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_get_delimiter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_delimiter(vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_get_posix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_posix(vr);

    METHOD_RETURN();
}

inline void path_posix_base::s_static_get_win32(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_win32(vr);

    METHOD_RETURN();
}
}
