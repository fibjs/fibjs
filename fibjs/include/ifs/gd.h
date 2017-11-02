/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _gd_base_H_
#define _gd_base_H_

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
        _NONE = 0,
        _JPEG = 1,
        _GIF = 2,
        _PNG = 3,
        _TIFF = 4,
        _BMP = 5,
        _WEBP = 6,
        _TRUECOLOR = 0,
        _PALETTE = 1,
        _ARC = 0,
        _CHORD = 1,
        _NOFILL = 2,
        _EDGED = 4,
        _HORIZONTAL = 1,
        _VERTICAL = 2,
        _BOTH = 3,
        _LEFT = 1,
        _RIGHT = 2,
        _MEAN_REMOVAL = 0,
        _EDGEDETECT = 1,
        _EMBOSS = 2,
        _SELECTIVE_BLUR = 3,
        _GAUSSIAN_BLUR = 4,
        _NEGATE = 5,
        _GRAYSCALE = 6,
        _SMOOTH = 7,
        _BRIGHTNESS = 8,
        _CONTRAST = 9,
        _COLORIZE = 10
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

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_create(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rgb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rgba(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hsl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hsla(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hsb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hsba(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_color(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE4(gd_base, create, int32_t, int32_t, int32_t, obj_ptr<Image_base>);
    ASYNC_STATICVALUE2(gd_base, load, Buffer_base*, obj_ptr<Image_base>);
    ASYNC_STATICVALUE2(gd_base, load, SeekableStream_base*, obj_ptr<Image_base>);
    ASYNC_STATICVALUE2(gd_base, load, exlib::string, obj_ptr<Image_base>);
};
}

#include "Image.h"
#include "Buffer.h"
#include "SeekableStream.h"

namespace fibjs {
inline ClassInfo& gd_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "create", s_create, true },
        { "createSync", s_create, true },
        { "load", s_load, true },
        { "loadSync", s_load, true },
        { "rgb", s_rgb, true },
        { "rgba", s_rgba, true },
        { "hsl", s_hsl, true },
        { "hsla", s_hsla, true },
        { "hsb", s_hsb, true },
        { "hsba", s_hsba, true },
        { "color", s_color, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "NONE", _NONE },
        { "JPEG", _JPEG },
        { "GIF", _GIF },
        { "PNG", _PNG },
        { "TIFF", _TIFF },
        { "BMP", _BMP },
        { "WEBP", _WEBP },
        { "TRUECOLOR", _TRUECOLOR },
        { "PALETTE", _PALETTE },
        { "ARC", _ARC },
        { "CHORD", _CHORD },
        { "NOFILL", _NOFILL },
        { "EDGED", _EDGED },
        { "HORIZONTAL", _HORIZONTAL },
        { "VERTICAL", _VERTICAL },
        { "BOTH", _BOTH },
        { "LEFT", _LEFT },
        { "RIGHT", _RIGHT },
        { "MEAN_REMOVAL", _MEAN_REMOVAL },
        { "EDGEDETECT", _EDGEDETECT },
        { "EMBOSS", _EMBOSS },
        { "SELECTIVE_BLUR", _SELECTIVE_BLUR },
        { "GAUSSIAN_BLUR", _GAUSSIAN_BLUR },
        { "NEGATE", _NEGATE },
        { "GRAYSCALE", _GRAYSCALE },
        { "SMOOTH", _SMOOTH },
        { "BRIGHTNESS", _BRIGHTNESS },
        { "CONTRAST", _CONTRAST },
        { "COLORIZE", _COLORIZE }
    };

    static ClassData s_cd = {
        "gd", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void gd_base::s_create(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Image_base> vr;

    METHOD_NAME("gd.create");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    OPT_ARG(int32_t, 2, gd_base::_TRUECOLOR);

    if (!cb.IsEmpty()) {
        acb_create(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_create(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void gd_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Image_base> vr;

    METHOD_NAME("gd.load");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty()) {
        acb_load(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_load(v0, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<SeekableStream_base>, 0);

    if (!cb.IsEmpty()) {
        acb_load(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_load(v0, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_load(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_load(v0, vr);

    METHOD_RETURN();
}

inline void gd_base::s_rgb(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void gd_base::s_rgba(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void gd_base::s_hsl(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void gd_base::s_hsla(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void gd_base::s_hsb(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void gd_base::s_hsba(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void gd_base::s_color(const v8::FunctionCallbackInfo<v8::Value>& args)
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

#endif
