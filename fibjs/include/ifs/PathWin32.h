/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _PathWin32_base_H_
#define _PathWin32_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "PathBase.h"

namespace fibjs {

class PathBase_base;

class PathWin32_base : public PathBase_base {
    DECLARE_CLASS(PathWin32_base);

public:
    // PathWin32_base
    static result_t _new(obj_ptr<PathWin32_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& PathWin32_base::class_info()
{
    static ClassData s_cd = {
        "PathWin32", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &PathBase_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void PathWin32_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void PathWin32_base::__new(const T& args)
{
    obj_ptr<PathWin32_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}
}

#endif
