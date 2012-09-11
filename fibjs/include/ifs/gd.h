/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _gd_base_H_
#define _gd_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Image_base;
class Buffer_base;
class SeekableStream_base;

class gd_base : public module_base
{
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
		_BOTH = 3
	};

public:
	// gd_base
	static result_t create(int32_t width, int32_t height, int32_t color, obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac);
	static result_t load(Buffer_base* data, obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac);
	static result_t loadFrom(SeekableStream_base* stm, obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(gd_base);

public:
	static v8::Handle<v8::Value> s_get_NONE(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_JPEG(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_GIF(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PNG(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_TIFF(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_BMP(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_GD(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_GD2(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_TRUECOLOR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PALETTE(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_ARC(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_CHORD(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_NOFILL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_EDGED(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_HORIZONTAL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_VERTICAL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_BOTH(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_create(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_load(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_loadFrom(const v8::Arguments& args);

public:
	ASYNC_STATIC4(gd_base, create);
	ASYNC_STATIC2(gd_base, load);
	ASYNC_STATIC2(gd_base, loadFrom);
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
			{"loadFrom", s_loadFrom, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"NONE", s_get_NONE, NULL, true},
			{"JPEG", s_get_JPEG, NULL, true},
			{"GIF", s_get_GIF, NULL, true},
			{"PNG", s_get_PNG, NULL, true},
			{"TIFF", s_get_TIFF, NULL, true},
			{"BMP", s_get_BMP, NULL, true},
			{"GD", s_get_GD, NULL, true},
			{"GD2", s_get_GD2, NULL, true},
			{"TRUECOLOR", s_get_TRUECOLOR, NULL, true},
			{"PALETTE", s_get_PALETTE, NULL, true},
			{"ARC", s_get_ARC, NULL, true},
			{"CHORD", s_get_CHORD, NULL, true},
			{"NOFILL", s_get_NOFILL, NULL, true},
			{"EDGED", s_get_EDGED, NULL, true},
			{"HORIZONTAL", s_get_HORIZONTAL, NULL, true},
			{"VERTICAL", s_get_VERTICAL, NULL, true},
			{"BOTH", s_get_BOTH, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"gd", NULL, 
			3, s_method, 0, NULL, 17, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> gd_base::s_get_NONE(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _NONE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_JPEG(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _JPEG;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_GIF(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _GIF;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_PNG(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PNG;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_TIFF(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _TIFF;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_BMP(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _BMP;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_GD(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _GD;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_GD2(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _GD2;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_TRUECOLOR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _TRUECOLOR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_PALETTE(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PALETTE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_ARC(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _ARC;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_CHORD(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _CHORD;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_NOFILL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _NOFILL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_EDGED(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _EDGED;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_HORIZONTAL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _HORIZONTAL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_VERTICAL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _VERTICAL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_get_BOTH(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _BOTH;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_create(const v8::Arguments& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(int32_t, 2, gd_base::_TRUECOLOR);

		hr = ac_create(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_load(const v8::Arguments& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ac_load(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> gd_base::s_loadFrom(const v8::Arguments& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<SeekableStream_base>, 0);

		hr = ac_loadFrom(v0, vr);

		METHOD_RETURN();
	}

}

#endif

