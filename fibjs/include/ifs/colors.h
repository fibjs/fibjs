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

class colors_base : public object_base {
    DECLARE_CLASS(colors_base);

public:
    // colors_base
    static result_t get_hasColors(bool& retVal);
    static result_t get_clear(exlib::string& retVal);
    static result_t get_normal(exlib::string& retVal);
    static result_t get_black(exlib::string& retVal);
    static result_t get_gray(exlib::string& retVal);
    static result_t get_red(exlib::string& retVal);
    static result_t get_green(exlib::string& retVal);
    static result_t get_yellow(exlib::string& retVal);
    static result_t get_blue(exlib::string& retVal);
    static result_t get_magenta(exlib::string& retVal);
    static result_t get_cyan(exlib::string& retVal);
    static result_t get_white(exlib::string& retVal);
    static result_t get_lightred(exlib::string& retVal);
    static result_t get_lightgreen(exlib::string& retVal);
    static result_t get_lightyellow(exlib::string& retVal);
    static result_t get_lightblue(exlib::string& retVal);
    static result_t get_lightmagenta(exlib::string& retVal);
    static result_t get_lightcyan(exlib::string& retVal);
    static result_t get_lightwhite(exlib::string& retVal);
    static result_t get_bold(exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<colors_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_hasColors(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_normal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_black(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_gray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_red(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_green(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_yellow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_blue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_magenta(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_cyan(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_white(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_lightred(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_lightgreen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_lightyellow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_lightblue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_lightmagenta(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_lightcyan(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_lightwhite(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_bold(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& colors_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "hasColors", s_static_get_hasColors, block_set, true },
        { "clear", s_static_get_clear, block_set, true },
        { "normal", s_static_get_normal, block_set, true },
        { "black", s_static_get_black, block_set, true },
        { "gray", s_static_get_gray, block_set, true },
        { "red", s_static_get_red, block_set, true },
        { "green", s_static_get_green, block_set, true },
        { "yellow", s_static_get_yellow, block_set, true },
        { "blue", s_static_get_blue, block_set, true },
        { "magenta", s_static_get_magenta, block_set, true },
        { "cyan", s_static_get_cyan, block_set, true },
        { "white", s_static_get_white, block_set, true },
        { "lightred", s_static_get_lightred, block_set, true },
        { "lightgreen", s_static_get_lightgreen, block_set, true },
        { "lightyellow", s_static_get_lightyellow, block_set, true },
        { "lightblue", s_static_get_lightblue, block_set, true },
        { "lightmagenta", s_static_get_lightmagenta, block_set, true },
        { "lightcyan", s_static_get_lightcyan, block_set, true },
        { "lightwhite", s_static_get_lightwhite, block_set, true },
        { "bold", s_static_get_bold, block_set, true }
    };

    static ClassData s_cd = {
        "colors", true, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void colors_base::s_static_get_hasColors(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_hasColors(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_clear(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_normal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_normal(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_black(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_black(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_gray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_gray(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_red(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_red(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_green(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_green(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_yellow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_yellow(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_blue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_blue(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_magenta(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_magenta(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_cyan(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_cyan(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_white(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_white(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_lightred(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_lightred(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_lightgreen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_lightgreen(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_lightyellow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_lightyellow(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_lightblue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_lightblue(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_lightmagenta(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_lightmagenta(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_lightcyan(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_lightcyan(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_lightwhite(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_lightwhite(vr);

    METHOD_RETURN();
}

inline void colors_base::s_static_get_bold(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_bold(vr);

    METHOD_RETURN();
}
}
