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

namespace fibjs
{

class Image_base;
class Buffer_base;
class SeekableStream_base;

class gd_base : public object_base
{
	DECLARE_CLASS(gd_base);

public:
	enum{
		_NONE = 0,
		_JPEG = 1,
		_GIF = 2,
		_PNG = 3,
		_TIFF = 4,
		_BMP = 5,
		_GD = 6,
		_GD2 = 7,
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
		_RIGHT = 2
	};

public:
	// gd_base
	static result_t create(int32_t width, int32_t height, int32_t color, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
	static result_t load(Buffer_base* data, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
	static result_t load(SeekableStream_base* stm, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
	static result_t load(const char* fname, obj_ptr<Image_base>& retVal, AsyncEvent* ac);
	static result_t rgb(int32_t red, int32_t green, int32_t blue, int32_t& retVal);
	static result_t rgba(int32_t red, int32_t green, int32_t blue, double alpha, int32_t& retVal);
	static result_t hsl(double hue, double saturation, double lightness, int32_t& retVal);
	static result_t hsla(double hue, double saturation, double lightness, double alpha, int32_t& retVal);
	static result_t hsb(double hue, double saturation, double brightness, int32_t& retVal);
	static result_t hsba(double hue, double saturation, double brightness, double alpha, int32_t& retVal);
	static result_t color(const char* color, int32_t& retVal);

public:
	static void s_get_NONE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_JPEG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_GIF(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_PNG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_TIFF(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_BMP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_GD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_GD2(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_TRUECOLOR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_PALETTE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ARC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CHORD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_NOFILL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_EDGED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_HORIZONTAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_VERTICAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_BOTH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_LEFT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_RIGHT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
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
	ASYNC_STATICVALUE2(gd_base, load, const char*, obj_ptr<Image_base>);
};

}

#include "Image.h"
#include "Buffer.h"
#include "SeekableStream.h"

namespace fibjs
{
	inline ClassInfo& gd_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"create", s_create, true},
			{"load", s_load, true},
			{"rgb", s_rgb, true},
			{"rgba", s_rgba, true},
			{"hsl", s_hsl, true},
			{"hsla", s_hsla, true},
			{"hsb", s_hsb, true},
			{"hsba", s_hsba, true},
			{"color", s_color, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"NONE", s_get_NONE, block_set},
			{"JPEG", s_get_JPEG, block_set},
			{"GIF", s_get_GIF, block_set},
			{"PNG", s_get_PNG, block_set},
			{"TIFF", s_get_TIFF, block_set},
			{"BMP", s_get_BMP, block_set},
			{"GD", s_get_GD, block_set},
			{"GD2", s_get_GD2, block_set},
			{"TRUECOLOR", s_get_TRUECOLOR, block_set},
			{"PALETTE", s_get_PALETTE, block_set},
			{"ARC", s_get_ARC, block_set},
			{"CHORD", s_get_CHORD, block_set},
			{"NOFILL", s_get_NOFILL, block_set},
			{"EDGED", s_get_EDGED, block_set},
			{"HORIZONTAL", s_get_HORIZONTAL, block_set},
			{"VERTICAL", s_get_VERTICAL, block_set},
			{"BOTH", s_get_BOTH, block_set},
			{"LEFT", s_get_LEFT, block_set},
			{"RIGHT", s_get_RIGHT, block_set}
		};

		static ClassData s_cd = 
		{ 
			"gd", NULL, 
			9, s_method, 0, NULL, 19, s_property, NULL, NULL,
			NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void gd_base::s_get_NONE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _NONE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_JPEG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _JPEG;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_GIF(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _GIF;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_PNG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _PNG;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_TIFF(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _TIFF;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_BMP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BMP;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_GD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _GD;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_GD2(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _GD2;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_TRUECOLOR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _TRUECOLOR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_PALETTE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _PALETTE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_ARC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ARC;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_CHORD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CHORD;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_NOFILL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _NOFILL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_EDGED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _EDGED;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_HORIZONTAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _HORIZONTAL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_VERTICAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _VERTICAL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_BOTH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BOTH;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_LEFT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _LEFT;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_get_RIGHT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _RIGHT;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void gd_base::s_create(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(int32_t, 2, gd_base::_TRUECOLOR);

		hr = ac_create(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void gd_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ac_load(v0, vr);

		METHOD_OVER(1, 1);

		ARG(obj_ptr<SeekableStream_base>, 0);

		hr = ac_load(v0, vr);

		METHOD_OVER(1, 1);

		ARG(arg_string, 0);

		hr = ac_load(v0, vr);

		METHOD_RETURN();
	}

	inline void gd_base::s_rgb(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = rgb(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void gd_base::s_rgba(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_ENTER(4, 4);

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

		METHOD_ENTER(3, 3);

		ARG(double, 0);
		ARG(double, 1);
		ARG(double, 2);

		hr = hsl(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void gd_base::s_hsla(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_ENTER(4, 4);

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

		METHOD_ENTER(3, 3);

		ARG(double, 0);
		ARG(double, 1);
		ARG(double, 2);

		hr = hsb(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void gd_base::s_hsba(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_ENTER(4, 4);

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

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = color(v0, vr);

		METHOD_RETURN();
	}

}

#endif

