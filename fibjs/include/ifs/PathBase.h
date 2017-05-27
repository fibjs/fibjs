/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _PathBase_base_H_
#define _PathBase_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class PathBase_base : public object_base {
    DECLARE_CLASS(PathBase_base);

public:
    // PathBase_base
    virtual result_t normalize(exlib::string path, exlib::string& retVal) = 0;
    virtual result_t basename(exlib::string path, exlib::string ext, exlib::string& retVal) = 0;
    virtual result_t extname(exlib::string path, exlib::string& retVal) = 0;
    virtual result_t dirname(exlib::string path, exlib::string& retVal) = 0;
    virtual result_t fullpath(exlib::string path, exlib::string& retVal) = 0;
    virtual result_t join(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal) = 0;
    virtual result_t resolve(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal) = 0;
    virtual result_t get_sep(exlib::string& retVal) = 0;
    virtual result_t get_delimiter(exlib::string& retVal) = 0;
    virtual result_t get_posix(obj_ptr<PathBase_base>& retVal) = 0;
    virtual result_t get_win32(obj_ptr<PathBase_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_basename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dirname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fullpath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_join(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_sep(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_delimiter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_posix(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_win32(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& PathBase_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "normalize", s_normalize, false },
        { "basename", s_basename, false },
        { "extname", s_extname, false },
        { "dirname", s_dirname, false },
        { "fullpath", s_fullpath, false },
        { "join", s_join, false },
        { "resolve", s_resolve, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "sep", s_get_sep, block_set, false },
        { "delimiter", s_get_delimiter, block_set, false },
        { "posix", s_get_posix, block_set, false },
        { "win32", s_get_win32, block_set, false }
    };

    static ClassData s_cd = {
        "PathBase", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void PathBase_base::s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->normalize(v0, vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_basename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->basename(v0, v1, vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_extname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->extname(v0, vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_dirname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->dirname(v0, vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_fullpath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->fullpath(v0, vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_join(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    hr = pInst->join(args, vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    hr = pInst->resolve(args, vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_get_sep(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    PROPERTY_ENTER();

    hr = pInst->get_sep(vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_get_delimiter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PathBase_base);
    PROPERTY_ENTER();

    hr = pInst->get_delimiter(vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_get_posix(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<PathBase_base> vr;

    METHOD_INSTANCE(PathBase_base);
    PROPERTY_ENTER();

    hr = pInst->get_posix(vr);

    METHOD_RETURN();
}

inline void PathBase_base::s_get_win32(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<PathBase_base> vr;

    METHOD_INSTANCE(PathBase_base);
    PROPERTY_ENTER();

    hr = pInst->get_win32(vr);

    METHOD_RETURN();
}
}

#endif
