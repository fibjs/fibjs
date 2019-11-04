/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _path_win32_base_H_
#define _path_win32_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class path_win32_base : public object_base {
    DECLARE_CLASS(path_win32_base);

public:
    // path_win32_base
    static result_t normalize(exlib::string path, exlib::string& retVal);
    static result_t basename(exlib::string path, exlib::string ext, exlib::string& retVal);
    static result_t extname(exlib::string path, exlib::string& retVal);
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

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_basename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_extname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dirname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fullpath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isAbsolute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_join(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_relative(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_toNamespacedPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_sep(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_delimiter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_posix(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_win32(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& path_win32_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "normalize", s_static_normalize, true },
        { "basename", s_static_basename, true },
        { "extname", s_static_extname, true },
        { "dirname", s_static_dirname, true },
        { "fullpath", s_static_fullpath, true },
        { "isAbsolute", s_static_isAbsolute, true },
        { "join", s_static_join, true },
        { "resolve", s_static_resolve, true },
        { "relative", s_static_relative, true },
        { "toNamespacedPath", s_static_toNamespacedPath, true }
    };

    static ClassData::ClassProperty s_property[] = {
        { "sep", s_static_get_sep, block_set, true },
        { "delimiter", s_static_get_delimiter, block_set, true },
        { "posix", s_static_get_posix, block_set, true },
        { "win32", s_static_get_win32, block_set, true }
    };

    static ClassData s_cd = {
        "path_win32", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void path_win32_base::s_static_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.normalize");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = normalize(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_basename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.basename");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = basename(v0, v1, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_extname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.extname");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = extname(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_dirname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.dirname");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = dirname(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_fullpath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.fullpath");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = fullpath(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_isAbsolute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("path_win32.isAbsolute");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = isAbsolute(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_join(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.join");
    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = join(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.resolve");
    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = resolve(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_relative(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.relative");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = relative(v0, v1, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_toNamespacedPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("path_win32.toNamespacedPath");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));

    hr = toNamespacedPath(v0, vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_get_sep(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.sep");
    PROPERTY_ENTER();

    hr = get_sep(vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_get_delimiter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("path_win32.delimiter");
    PROPERTY_ENTER();

    hr = get_delimiter(vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_get_posix(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("path_win32.posix");
    PROPERTY_ENTER();

    hr = get_posix(vr);

    METHOD_RETURN();
}

inline void path_win32_base::s_static_get_win32(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("path_win32.win32");
    PROPERTY_ENTER();

    hr = get_win32(vr);

    METHOD_RETURN();
}
}

#endif
