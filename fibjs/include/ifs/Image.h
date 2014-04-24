/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Image_base_H_
#define _Image_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class gd_base;
class Buffer_base;
class Stream_base;

class Image_base : public object_base
{
public:
	// Image_base
	virtual result_t get_width(int32_t& retVal) = 0;
	virtual result_t get_height(int32_t& retVal) = 0;
	virtual result_t get_format(int32_t& retVal) = 0;
	virtual result_t get_type(int32_t& retVal) = 0;
	virtual result_t get_colorsTotal(int32_t& retVal) = 0;
	virtual result_t get_transparent(int32_t& retVal) = 0;
	virtual result_t set_transparent(int32_t newVal) = 0;
	virtual result_t get_alphaBlending(bool& retVal) = 0;
	virtual result_t set_alphaBlending(bool newVal) = 0;
	virtual result_t getData(int32_t format, int32_t quality, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t save(Stream_base* stm, int32_t format, int32_t quality, exlib::AsyncEvent* ac) = 0;
	virtual result_t save(const char* fname, int32_t format, int32_t quality, exlib::AsyncEvent* ac) = 0;
	virtual result_t colorAllocate(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorAllocateAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t colorDeallocate(int32_t color) = 0;
	virtual result_t colorClosest(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorClosestHWB(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorClosestAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t colorExact(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorExactAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t colorResolve(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorResolveAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2) = 0;
	virtual result_t getPixel(int32_t x, int32_t y, int32_t& retVal) = 0;
	virtual result_t getTrueColorPixel(int32_t x, int32_t y, int32_t& retVal) = 0;
	virtual result_t setPixel(int32_t x, int32_t y, int32_t color) = 0;
	virtual result_t setThickness(int32_t thickness) = 0;
	virtual result_t line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color) = 0;
	virtual result_t rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color) = 0;
	virtual result_t filledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color) = 0;
	virtual result_t polygon(v8::Local<v8::Array> points, int32_t color) = 0;
	virtual result_t openPolygon(v8::Local<v8::Array> points, int32_t color) = 0;
	virtual result_t filledPolygon(v8::Local<v8::Array> points, int32_t color) = 0;
	virtual result_t ellipse(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color) = 0;
	virtual result_t filledEllipse(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color) = 0;
	virtual result_t arc(int32_t x, int32_t y, int32_t width, int32_t height, int32_t start, int32_t end, int32_t color) = 0;
	virtual result_t filledArc(int32_t x, int32_t y, int32_t width, int32_t height, int32_t start, int32_t end, int32_t color, int32_t style) = 0;
	virtual result_t fill(int32_t x, int32_t y, int32_t color) = 0;
	virtual result_t fillToBorder(int32_t x, int32_t y, int32_t borderColor, int32_t color) = 0;
	virtual result_t colorReplace(int32_t src, int32_t dst, exlib::AsyncEvent* ac) = 0;
	virtual result_t clone(obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t resample(int32_t width, int32_t height, obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t crop(int32_t x, int32_t y, int32_t width, int32_t height, obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t flip(int32_t dir, exlib::AsyncEvent* ac) = 0;
	virtual result_t rotate(int32_t dir, exlib::AsyncEvent* ac) = 0;
	virtual result_t convert(int32_t color, exlib::AsyncEvent* ac) = 0;
	virtual result_t copy(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, exlib::AsyncEvent* ac) = 0;
	virtual result_t copyMerge(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t percent, exlib::AsyncEvent* ac) = 0;
	virtual result_t copyMergeGray(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t percent, exlib::AsyncEvent* ac) = 0;
	virtual result_t copyResized(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW, int32_t srcH, exlib::AsyncEvent* ac) = 0;
	virtual result_t copyResampled(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW, int32_t srcH, exlib::AsyncEvent* ac) = 0;
	virtual result_t copyRotated(Image_base* source, double dstX, double dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t angle, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(Image_base);

public:
	static void s_get_width(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_height(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_format(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_colorsTotal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_transparent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_transparent(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_alphaBlending(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_alphaBlending(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_getData(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_save(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorAllocate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorAllocateAlpha(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorDeallocate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorClosest(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorClosestHWB(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorClosestAlpha(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorExact(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorExactAlpha(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorResolve(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorResolveAlpha(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clip(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getPixel(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getTrueColorPixel(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setPixel(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setThickness(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_line(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rectangle(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_filledRectangle(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_polygon(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_openPolygon(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_filledPolygon(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ellipse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_filledEllipse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_arc(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_filledArc(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_fill(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_fillToBorder(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_colorReplace(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_resample(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_crop(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_flip(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rotate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_convert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copy(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copyMerge(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copyMergeGray(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copyResized(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copyResampled(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copyRotated(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE3(Image_base, getData, int32_t, int32_t, obj_ptr<Buffer_base>);
	ASYNC_MEMBER3(Image_base, save, Stream_base*, int32_t, int32_t);
	ASYNC_MEMBER3(Image_base, save, const char*, int32_t, int32_t);
	ASYNC_MEMBER2(Image_base, colorReplace, int32_t, int32_t);
	ASYNC_MEMBERVALUE1(Image_base, clone, obj_ptr<Image_base>);
	ASYNC_MEMBERVALUE3(Image_base, resample, int32_t, int32_t, obj_ptr<Image_base>);
	ASYNC_MEMBERVALUE5(Image_base, crop, int32_t, int32_t, int32_t, int32_t, obj_ptr<Image_base>);
	ASYNC_MEMBER1(Image_base, flip, int32_t);
	ASYNC_MEMBER1(Image_base, rotate, int32_t);
	ASYNC_MEMBER1(Image_base, convert, int32_t);
	ASYNC_MEMBER7(Image_base, copy, Image_base*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	ASYNC_MEMBER8(Image_base, copyMerge, Image_base*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	ASYNC_MEMBER8(Image_base, copyMergeGray, Image_base*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	ASYNC_MEMBER9(Image_base, copyResized, Image_base*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	ASYNC_MEMBER9(Image_base, copyResampled, Image_base*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	ASYNC_MEMBER8(Image_base, copyRotated, Image_base*, double, double, int32_t, int32_t, int32_t, int32_t, int32_t);
};

}

#include "gd.h"
#include "Buffer.h"
#include "Stream.h"

namespace fibjs
{
	inline ClassInfo& Image_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"getData", s_getData},
			{"save", s_save},
			{"colorAllocate", s_colorAllocate},
			{"colorAllocateAlpha", s_colorAllocateAlpha},
			{"colorDeallocate", s_colorDeallocate},
			{"colorClosest", s_colorClosest},
			{"colorClosestHWB", s_colorClosestHWB},
			{"colorClosestAlpha", s_colorClosestAlpha},
			{"colorExact", s_colorExact},
			{"colorExactAlpha", s_colorExactAlpha},
			{"colorResolve", s_colorResolve},
			{"colorResolveAlpha", s_colorResolveAlpha},
			{"clip", s_clip},
			{"getPixel", s_getPixel},
			{"getTrueColorPixel", s_getTrueColorPixel},
			{"setPixel", s_setPixel},
			{"setThickness", s_setThickness},
			{"line", s_line},
			{"rectangle", s_rectangle},
			{"filledRectangle", s_filledRectangle},
			{"polygon", s_polygon},
			{"openPolygon", s_openPolygon},
			{"filledPolygon", s_filledPolygon},
			{"ellipse", s_ellipse},
			{"filledEllipse", s_filledEllipse},
			{"arc", s_arc},
			{"filledArc", s_filledArc},
			{"fill", s_fill},
			{"fillToBorder", s_fillToBorder},
			{"colorReplace", s_colorReplace},
			{"clone", s_clone},
			{"resample", s_resample},
			{"crop", s_crop},
			{"flip", s_flip},
			{"rotate", s_rotate},
			{"convert", s_convert},
			{"copy", s_copy},
			{"copyMerge", s_copyMerge},
			{"copyMergeGray", s_copyMergeGray},
			{"copyResized", s_copyResized},
			{"copyResampled", s_copyResampled},
			{"copyRotated", s_copyRotated}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"width", s_get_width, block_set},
			{"height", s_get_height, block_set},
			{"format", s_get_format, block_set},
			{"type", s_get_type, block_set},
			{"colorsTotal", s_get_colorsTotal, block_set},
			{"transparent", s_get_transparent, s_set_transparent},
			{"alphaBlending", s_get_alphaBlending, s_set_alphaBlending}
		};

		static ClassData s_cd = 
		{ 
			"Image", NULL, 
			42, s_method, 0, NULL, 7, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Image_base::s_get_width(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_width(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_get_height(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_height(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_get_format(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_format(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_type(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_get_colorsTotal(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_colorsTotal(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_get_transparent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_transparent(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_set_transparent(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_transparent(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void Image_base::s_get_alphaBlending(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_alphaBlending(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_set_alphaBlending(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		PROPERTY_VAL(bool);
		hr = pInst->set_alphaBlending(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void Image_base::s_getData(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 0);

		OPT_ARG(int32_t, 0, gd_base::_PNG);
		OPT_ARG(int32_t, 1, 85);

		hr = pInst->ac_getData(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_save(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 1);

		ARG(obj_ptr<Stream_base>, 0);
		OPT_ARG(int32_t, 1, gd_base::_PNG);
		OPT_ARG(int32_t, 2, 85);

		hr = pInst->ac_save(v0, v1, v2);

		METHOD_OVER(3, 1);

		ARG(arg_string, 0);
		OPT_ARG(int32_t, 1, gd_base::_PNG);
		OPT_ARG(int32_t, 2, 85);

		hr = pInst->ac_save(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Image_base::s_colorAllocate(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorAllocate(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorAllocateAlpha(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorAllocateAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorDeallocate(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->colorDeallocate(v0);

		METHOD_VOID();
	}

	inline void Image_base::s_colorClosest(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorClosest(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorClosestHWB(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorClosestHWB(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorClosestAlpha(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorClosestAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorExact(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorExact(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorExactAlpha(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorExactAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorResolve(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorResolve(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_colorResolveAlpha(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorResolveAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_clip(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->clip(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void Image_base::s_getPixel(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->getPixel(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_getTrueColorPixel(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->getTrueColorPixel(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_setPixel(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->setPixel(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Image_base::s_setThickness(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->setThickness(v0);

		METHOD_VOID();
	}

	inline void Image_base::s_line(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->line(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline void Image_base::s_rectangle(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->rectangle(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline void Image_base::s_filledRectangle(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->filledRectangle(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline void Image_base::s_polygon(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Local<v8::Array>, 0);
		ARG(int32_t, 1);

		hr = pInst->polygon(v0, v1);

		METHOD_VOID();
	}

	inline void Image_base::s_openPolygon(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Local<v8::Array>, 0);
		ARG(int32_t, 1);

		hr = pInst->openPolygon(v0, v1);

		METHOD_VOID();
	}

	inline void Image_base::s_filledPolygon(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Local<v8::Array>, 0);
		ARG(int32_t, 1);

		hr = pInst->filledPolygon(v0, v1);

		METHOD_VOID();
	}

	inline void Image_base::s_ellipse(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->ellipse(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline void Image_base::s_filledEllipse(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->filledEllipse(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline void Image_base::s_arc(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(7, 7);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);

		hr = pInst->arc(v0, v1, v2, v3, v4, v5, v6);

		METHOD_VOID();
	}

	inline void Image_base::s_filledArc(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 7);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		OPT_ARG(int32_t, 7, gd_base::_ARC);

		hr = pInst->filledArc(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

	inline void Image_base::s_fill(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->fill(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Image_base::s_fillToBorder(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->fillToBorder(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void Image_base::s_colorReplace(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->ac_colorReplace(v0, v1);

		METHOD_VOID();
	}

	inline void Image_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_clone(vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_resample(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->ac_resample(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_crop(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->ac_crop(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline void Image_base::s_flip(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, gd_base::_HORIZONTAL);

		hr = pInst->ac_flip(v0);

		METHOD_VOID();
	}

	inline void Image_base::s_rotate(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_rotate(v0);

		METHOD_VOID();
	}

	inline void Image_base::s_convert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, gd_base::_TRUECOLOR);

		hr = pInst->ac_convert(v0);

		METHOD_VOID();
	}

	inline void Image_base::s_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(7, 7);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);

		hr = pInst->ac_copy(v0, v1, v2, v3, v4, v5, v6);

		METHOD_VOID();
	}

	inline void Image_base::s_copyMerge(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 8);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);

		hr = pInst->ac_copyMerge(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

	inline void Image_base::s_copyMergeGray(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 8);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);

		hr = pInst->ac_copyMergeGray(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

	inline void Image_base::s_copyResized(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(9, 9);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);
		ARG(int32_t, 8);

		hr = pInst->ac_copyResized(v0, v1, v2, v3, v4, v5, v6, v7, v8);

		METHOD_VOID();
	}

	inline void Image_base::s_copyResampled(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(9, 9);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);
		ARG(int32_t, 8);

		hr = pInst->ac_copyResampled(v0, v1, v2, v3, v4, v5, v6, v7, v8);

		METHOD_VOID();
	}

	inline void Image_base::s_copyRotated(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 8);

		ARG(obj_ptr<Image_base>, 0);
		ARG(double, 1);
		ARG(double, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);

		hr = pInst->ac_copyRotated(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

}

#endif

