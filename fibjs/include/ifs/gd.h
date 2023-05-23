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

class Image_base;
class Buffer_base;
class SeekableStream_base;

class gd_base : public object_base {
    DECLARE_CLASS(gd_base);

public:
    enum {
        C_NONE = 0,
        C_JPEG = 1,
        C_GIF = 2,
        C_PNG = 3,
        C_TIFF = 4,
        C_BMP = 5,
        C_WEBP = 6,
        C_TRUECOLOR = 0,
        C_PALETTE = 1,
        C_ARC = 0,
        C_CHORD = 1,
        C_NOFILL = 2,
        C_EDGED = 4,
        C_HORIZONTAL = 1,
        C_VERTICAL = 2,
        C_BOTH = 3,
        C_LEFT = 1,
        C_RIGHT = 2,
        C_MEAN_REMOVAL = 0,
        C_EDGEDETECT = 1,
        C_EMBOSS = 2,
        C_SELECTIVE_BLUR = 3,
        C_GAUSSIAN_BLUR = 4,
        C_NEGATE = 5,
        C_GRAYSCALE = 6,
        C_SMOOTH = 7,
        C_BRIGHTNESS = 8,
        C_CONTRAST = 9,
        C_COLORIZE = 10
    };

public:
    // gd_base
    static result_t create(int32_t width, int32_t height, int32_t color, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
    static result_t load(Buffer_base* data, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
    static result_t load(SeekableStream_base* stm, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
    static result_t load(exlib::string fname, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
    static result_t rgb(int32_t red, int32_t green, int32_t blue, int32_t& retVal);
    static result_t rgba(int32_t red, int32_t green, int32_t blue, double alpha, int32_t& retVal);
    static result_t hsl(double hue, double saturation, double lightness, int32_t& retVal);
    static result_t hsla(double hue, double saturation, double lightness, double alpha, int32_t& retVal);
    static result_t hsb(double hue, double saturation, double brightness, int32_t& retVal);
    static result_t hsba(double hue, double saturation, double brightness, double alpha, int32_t& retVal);
    static result_t color(exlib::string color, int32_t& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_create(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rgb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_rgba(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hsl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hsla(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hsb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hsba(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_color(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE4(gd_base, create, int32_t, int32_t, int32_t, obj_ptr<Image_base>);
    ASYNC_STATICVALUE2(gd_base, load, Buffer_base*, obj_ptr<Image_base>);
    ASYNC_STATICVALUE2(gd_base, load, SeekableStream_base*, obj_ptr<Image_base>);
    ASYNC_STATICVALUE2(gd_base, load, exlib::string, obj_ptr<Image_base>);
};
}

#include "ifs/Image.h"
#include "ifs/Buffer.h"
#include "ifs/SeekableStream.h"

namespace fibjs {
inline ClassInfo& gd_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "create", s_static_create, true, true },
        { "createSync", s_static_create, true, false },
        { "load", s_static_load, true, true },
        { "loadSync", s_static_load, true, false },
        { "rgb", s_static_rgb, true, false },
        { "rgba", s_static_rgba, true, false },
        { "hsl", s_static_hsl, true, false },
        { "hsla", s_static_hsla, true, false },
        { "hsb", s_static_hsb, true, false },
        { "hsba", s_static_hsba, true, false },
        { "color", s_static_color, true, false }
    };

    static ClassData::ClassConst s_const[] = {
        { "NONE", C_NONE },
        { "JPEG", C_JPEG },
        { "GIF", C_GIF },
        { "PNG", C_PNG },
        { "TIFF", C_TIFF },
        { "BMP", C_BMP },
        { "WEBP", C_WEBP },
        { "TRUECOLOR", C_TRUECOLOR },
        { "PALETTE", C_PALETTE },
        { "ARC", C_ARC },
        { "CHORD", C_CHORD },
        { "NOFILL", C_NOFILL },
        { "EDGED", C_EDGED },
        { "HORIZONTAL", C_HORIZONTAL },
        { "VERTICAL", C_VERTICAL },
        { "BOTH", C_BOTH },
        { "LEFT", C_LEFT },
        { "RIGHT", C_RIGHT },
        { "MEAN_REMOVAL", C_MEAN_REMOVAL },
        { "EDGEDETECT", C_EDGEDETECT },
        { "EMBOSS", C_EMBOSS },
        { "SELECTIVE_BLUR", C_SELECTIVE_BLUR },
        { "GAUSSIAN_BLUR", C_GAUSSIAN_BLUR },
        { "NEGATE", C_NEGATE },
        { "GRAYSCALE", C_GRAYSCALE },
        { "SMOOTH", C_SMOOTH },
        { "BRIGHTNESS", C_BRIGHTNESS },
        { "CONTRAST", C_CONTRAST },
        { "COLORIZE", C_COLORIZE }
    };

    static ClassData s_cd = {
        "gd", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void gd_base::s_static_create(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Image_base> vr;

    METHOD_NAME("gd.create");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    OPT_ARG(int32_t, 2, gd_base::C_TRUECOLOR);

    if (!cb.IsEmpty())
        hr = acb_create(v0, v1, v2, cb, args);
    else
        hr = ac_create(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_load(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Image_base> vr;

    METHOD_NAME("gd.load");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = acb_load(v0, cb, args);
    else
        hr = ac_load(v0, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<SeekableStream_base>, 0);

    if (!cb.IsEmpty())
        hr = acb_load(v0, cb, args);
    else
        hr = ac_load(v0, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_load(v0, cb, args);
    else
        hr = ac_load(v0, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_rgb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("gd.rgb");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);

    hr = rgb(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_rgba(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("gd.rgba");
    METHOD_ENTER();

    METHOD_OVER(4, 4);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);
    ARG(double, 3);

    hr = rgba(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_hsl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("gd.hsl");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(double, 0);
    ARG(double, 1);
    ARG(double, 2);

    hr = hsl(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_hsla(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("gd.hsla");
    METHOD_ENTER();

    METHOD_OVER(4, 4);

    ARG(double, 0);
    ARG(double, 1);
    ARG(double, 2);
    ARG(double, 3);

    hr = hsla(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_hsb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("gd.hsb");
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(double, 0);
    ARG(double, 1);
    ARG(double, 2);

    hr = hsb(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_hsba(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("gd.hsba");
    METHOD_ENTER();

    METHOD_OVER(4, 4);

    ARG(double, 0);
    ARG(double, 1);
    ARG(double, 2);
    ARG(double, 3);

    hr = hsba(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void gd_base::s_static_color(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("gd.color");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = color(v0, vr);

    METHOD_RETURN();
}
}
