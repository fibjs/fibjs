/*
 * Image.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: lion
 */

#include "Image.h"
#include "Buffer.h"
#include <vector>

namespace fibjs
{

void Image::setExtMemory(int add)
{
	if (m_image)
	{
		int psize =
				gdImageTrueColor(m_image) ? sizeof(int) : sizeof(unsigned char);
		int sx = gdImageSX(m_image);
		int sy = gdImageSY(m_image);
		extMemory(
				(sizeof(gdImage) + sizeof(void*) * sy + psize * sx * sy) * add);
	}
}

result_t gd_base::create(int32_t width, int32_t height, int32_t color,
		obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Image> img = new Image();
	result_t hr = img->create(width, height, color);
	if (hr < 0)
		return hr;

	retVal = img;
	return 0;
}

result_t gd_base::load(Buffer_base* data, obj_ptr<Image_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Image> img = new Image();
	result_t hr = img->load(data);
	if (hr < 0)
		return hr;

	retVal = img;
	return 0;
}

result_t gd_base::load(SeekableStream_base* stm, obj_ptr<Image_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncLoad: public asyncState
	{
	public:
		asyncLoad(SeekableStream_base* stm, obj_ptr<Image_base>& retVal,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_stm(stm), m_retVal(retVal)
		{
			set(read);
		}

		static int read(asyncState* pState, int n)
		{
			asyncLoad* pThis = (asyncLoad*) pState;
			result_t hr;
			int64_t len;

			hr = pThis->m_stm->size(len);
			if (hr < 0)
				return hr;

			hr = pThis->m_stm->rewind();
			if (hr < 0)
				return hr;

			pThis->set(load);
			return pThis->m_stm->read((int32_t) len, pThis->m_buffer, pThis);
		}

		static int load(asyncState* pState, int n)
		{
			asyncLoad* pThis = (asyncLoad*) pState;

			if (n == CALL_RETURN_NULL)
				return CALL_E_INVALID_DATA;

			obj_ptr<Image> img = new Image();
			result_t hr = img->load(pThis->m_buffer);
			if (hr < 0)
				return hr;

			pThis->m_retVal = img;
			return pThis->done(0);
		}

	private:
		obj_ptr<SeekableStream_base> m_stm;
		obj_ptr<Image_base>& m_retVal;
		obj_ptr<Buffer_base> m_buffer;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncLoad(stm, retVal, ac))->post(0);
}

result_t Image::create(int32_t width, int32_t height, int32_t color)
{
	if (color == gd_base::_PALETTE)
		m_image = gdImageCreate(width, height);
	else if (color == gd_base::_TRUECOLOR)
		m_image = gdImageCreateTrueColor(width, height);
	else
		return CALL_E_INVALIDARG;

	if (m_image == NULL)
		return CALL_E_INVALIDARG;

	setExtMemory();

	return 0;
}

result_t Image::load(Buffer_base* data)
{
	std::string strBuf;
	data->toString(strBuf);

	if (strBuf.length() < 2)
		return CALL_E_INVALIDARG;

	int32_t format;
	unsigned char ch1 = (unsigned char) strBuf[0];
	unsigned char ch2 = (unsigned char) strBuf[1];

	if (ch1 == 0x47 && ch2 == 0x49)
		format = gd_base::_GIF;
	else if (ch1 == 0x89 && ch2 == 0x50)
		format = gd_base::_PNG;
	else if (ch1 == 0xff && ch2 == 0xd8)
		format = gd_base::_JPEG;
	else if ((ch1 == 0x49 && ch2 == 0x49) || (ch1 == 0x4d && ch2 == 0x4d))
		format = gd_base::_TIFF;
	else if (ch1 == 0x42 && ch2 == 0x4d)
		format = gd_base::_BMP;
	else if ((ch1 == 0xff && ch2 == 0xfe) || (ch1 == 0xff && ch2 == 0xff))
		format = gd_base::_GD;
	else if (ch1 == 0x67 && ch2 == 0x64)
		format = gd_base::_GD2;
	else
		return CALL_E_INVALID_DATA;

	switch (format)
	{
	case gd_base::_GIF:
		m_image = gdImageCreateFromGifPtr((int) strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_PNG:
		m_image = gdImageCreateFromPngPtr((int) strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_JPEG:
		m_image = gdImageCreateFromJpegPtr((int) strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_TIFF:
		m_image = gdImageCreateFromTiffPtr((int) strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_BMP:
		m_image = gdImageCreateFromBmpPtr((int) strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_GD:
		m_image = gdImageCreateFromGdPtr((int) strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_GD2:
		m_image = gdImageCreateFromGd2Ptr((int) strBuf.length(),
				(void*) strBuf.c_str());
		break;
	}

	if (m_image == NULL)
		return CALL_E_INVALIDARG;

	setExtMemory();
	m_type = format;

	return 0;
}

int my_replacer(gdImagePtr im, int src)
{
	if (src == gdImageGetTransparent(im))
		return gdTrueColor(255, 255, 255);

	return gdAlphaBlend(gdTrueColor(255, 255, 255), src);
}

result_t Image::getData(int32_t format, int32_t quality,
		obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	int size = 0;
	void *data = NULL;

	if (format == gd_base::_JPEG || format == gd_base::_TIFF
			|| format == gd_base::_GIF || format == gd_base::_BMP)
	{
		if (gdImageTrueColor(m_image))
			gdImageColorReplaceCallback(m_image, my_replacer);

		if (gdImageGetTransparent(m_image) != -1)
			gdImageColorReplace(m_image, gdImageGetTransparent(m_image),
					gdTrueColor(255, 255, 255));
	}
	else if (gdImageTrueColor(m_image))
		gdImageSaveAlpha(m_image, 1);

	switch (format)
	{
	case gd_base::_GIF:
		data = gdImageGifPtr(m_image, &size);
		break;
	case gd_base::_PNG:
		data = gdImagePngPtr(m_image, &size);
		break;
	case gd_base::_JPEG:
		data = gdImageJpegPtr(m_image, &size, quality);
		break;
	case gd_base::_TIFF:
		data = gdImageTiffPtr(m_image, &size);
		break;
	case gd_base::_BMP:
		data = gdImageBmpPtr(m_image, &size, 1);
		break;
	case gd_base::_GD:
		data = gdImageGdPtr(m_image, &size);
		break;
	case gd_base::_GD2:
		data = gdImageGd2Ptr(m_image, 128, 2, &size);
		break;
	}

	if (data == NULL)
		return CALL_E_INVALIDARG;

	retVal = new Buffer(std::string((char*) data, size));
	gdFree(data);

	return 0;
}

result_t Image::save(Stream_base* stm, int32_t format, int32_t quality,
		exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Buffer_base> buf;
	result_t hr = getData(format, quality, buf, ac);
	if (hr < 0)
		return hr;

	return stm->write(buf, ac);
}

result_t Image::get_width(int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = gdImageSX(m_image);
	return 0;
}

result_t Image::get_height(int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = gdImageSY(m_image);
	return 0;
}

result_t Image::get_format(int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = m_type;
	return 0;
}

result_t Image::get_type(int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal =
			gdImageTrueColor(m_image) ? gd_base::_TRUECOLOR : gd_base::_PALETTE;
	return 0;
}

result_t Image::get_colorsTotal(int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = gdImageColorsTotal(m_image);
	return 0;
}

result_t Image::colorAllocate(int32_t red, int32_t green, int32_t blue,
		int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorAllocate(m_image, red, green, blue);
	return 0;
}

result_t Image::colorAllocateAlpha(int32_t red, int32_t green, int32_t blue,
		int32_t alpha, int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255 || alpha < 0 || alpha > 127)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorAllocateAlpha(m_image, red, green, blue, alpha);
	return 0;
}

result_t Image::colorDeallocate(int32_t colorIndex)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageColorDeallocate(m_image, colorIndex);
	return 0;
}

result_t Image::colorClosest(int32_t red, int32_t green, int32_t blue,
		int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorClosest(m_image, red, green, blue);
	return 0;
}

result_t Image::colorClosestHWB(int32_t red, int32_t green, int32_t blue,
		int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorClosestHWB(m_image, red, green, blue);
	return 0;
}

result_t Image::colorClosestAlpha(int32_t red, int32_t green, int32_t blue,
		int32_t alpha, int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255 || alpha < 0 || alpha > 127)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorClosestAlpha(m_image, red, green, blue, alpha);
	return 0;
}

result_t Image::colorExact(int32_t red, int32_t green, int32_t blue,
		int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorExact(m_image, red, green, blue);
	return 0;
}

result_t Image::colorExactAlpha(int32_t red, int32_t green, int32_t blue,
		int32_t alpha, int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255 || alpha < 0 || alpha > 127)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorExactAlpha(m_image, red, green, blue, alpha);
	return 0;
}

result_t Image::colorResolve(int32_t red, int32_t green, int32_t blue,
		int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorResolve(m_image, red, green, blue);
	return 0;
}

result_t Image::colorResolveAlpha(int32_t red, int32_t green, int32_t blue,
		int32_t alpha, int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
			|| blue > 255 || alpha < 0 || alpha > 127)
		return CALL_E_INVALIDARG;

	retVal = gdImageColorResolveAlpha(m_image, red, green, blue, alpha);
	return 0;
}

result_t Image::clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageSetClip(m_image, x1, y1, x2, x2);
	return 0;
}

result_t Image::getPixel(int32_t x, int32_t y, int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = gdImageGetPixel(m_image, x, y);
	return 0;
}

result_t Image::getTrueColorPixel(int32_t x, int32_t y, int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = gdImageGetTrueColorPixel(m_image, x, y);
	return 0;
}

result_t Image::setPixel(int32_t x, int32_t y, int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageSetPixel(m_image, x, y, color);
	return 0;
}

result_t Image::get_transparent(int32_t& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = gdImageGetTransparent(m_image);
	return 0;
}

result_t Image::set_transparent(int32_t newVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageColorTransparent(m_image, newVal);
	return 0;
}

result_t Image::get_alphaBlending(bool& retVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	retVal = m_image->alphaBlendingFlag != 0;
	return 0;
}

result_t Image::set_alphaBlending(bool newVal)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageAlphaBlending(m_image, newVal);
	return 0;
}

result_t Image::setThickness(int32_t thickness)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageSetThickness(m_image, thickness);
	return 0;
}

result_t Image::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
		int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageLine(m_image, x1, y1, x2, y2, color);
	return 0;
}

result_t Image::rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
		int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageRectangle(m_image, x1, y1, x2, y2, color);
	return 0;
}

result_t Image::filledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
		int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageFilledRectangle(m_image, x1, y1, x2, y2, color);
	return 0;
}

result_t getPoints(v8::Handle<v8::Array>& points, std::vector<gdPoint>& pts)
{
	int32_t i, len = points->Length();

	pts.resize(len);

	for (i = 0; i < len; i++)
	{
		v8::Handle<v8::Value> v = points->Get(i);

		if (!v->IsArray())
			return CALL_E_TYPEMISMATCH;

		v8::Handle<v8::Array> pt = v8::Handle<v8::Array>::Cast(v);

		if (pt->Length() != 2)
			return CALL_E_TYPEMISMATCH;

		pts[i].x = pt->Get(0)->Int32Value();
		pts[i].y = pt->Get(1)->Int32Value();
	}

	return 0;
}

result_t Image::polygon(v8::Handle<v8::Array> points, int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	std::vector<gdPoint> pts;

	result_t hr = getPoints(points, pts);
	if (hr < 0)
		return hr;

	gdImagePolygon(m_image, pts.data(), (int) pts.size(), color);
	return 0;
}

result_t Image::openPolygon(v8::Handle<v8::Array> points, int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	std::vector<gdPoint> pts;

	result_t hr = getPoints(points, pts);
	if (hr < 0)
		return hr;

	gdImageOpenPolygon(m_image, pts.data(), (int) pts.size(), color);
	return 0;
}

result_t Image::filledPolygon(v8::Handle<v8::Array> points, int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	std::vector<gdPoint> pts;

	result_t hr = getPoints(points, pts);
	if (hr < 0)
		return hr;

	gdImageFilledPolygon(m_image, pts.data(), (int) pts.size(), color);
	return 0;
}

result_t Image::ellipse(int32_t x, int32_t y, int32_t width, int32_t height,
		int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageEllipse(m_image, x, y, width, height, color);
	return 0;
}

result_t Image::filledEllipse(int32_t x, int32_t y, int32_t width,
		int32_t height, int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageFilledEllipse(m_image, x, y, width, height, color);
	return 0;
}

result_t Image::arc(int32_t x, int32_t y, int32_t width, int32_t height,
		int32_t start, int32_t end, int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (start < 0 || start > 360 || end < 0 || end > 360)
		return CALL_E_INVALIDARG;

	gdImageArc(m_image, x, y, width, height, start, end, color);
	return 0;
}

result_t Image::filledArc(int32_t x, int32_t y, int32_t width, int32_t height,
		int32_t start, int32_t end, int32_t color, int32_t style)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (start < 0 || start > 360 || end < 0 || end > 360)
		return CALL_E_INVALIDARG;

	gdImageFilledArc(m_image, x, y, width, height, start, end, color, style);
	return 0;
}

result_t Image::fill(int32_t x, int32_t y, int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageFill(m_image, x, y, color);
	return 0;
}

result_t Image::fillToBorder(int32_t x, int32_t y, int32_t borderColor,
		int32_t color)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	gdImageFillToBorder(m_image, x, y, borderColor, color);
	return 0;
}

result_t Image::New(int32_t width, int32_t height, obj_ptr<Image>& retVal)
{
	obj_ptr<Image> img = new Image();

	if (gdImageTrueColor(m_image))
		img->m_image = gdImageCreateTrueColor(width, height);
	else
		img->m_image = gdImageCreate(width, height);

	gdImagePaletteCopy(img->m_image, m_image);
	gdImageColorTransparent(img->m_image, gdImageGetTransparent(m_image));

	img->setExtMemory();

	retVal = img;
	return 0;
}

result_t Image::colorReplace(int32_t src, int32_t dst, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	gdImageColorReplace(m_image, src, dst);
	return 0;
}

result_t Image::crop(int32_t x, int32_t y, int32_t width, int32_t height,
		obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	gdRect r =
	{ x, y, width, height };

	obj_ptr<Image> img = new Image();

	img->m_image = gdImageCrop(m_image, &r);
	img->setExtMemory();

	retVal = img;

	return 0;
}

result_t Image::clone(obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	int32_t w = gdImageSX(m_image);
	int32_t h = gdImageSY(m_image);

	return crop(0, 0, w, h, retVal, ac);
}

result_t Image::resample(int32_t width, int32_t height,
		obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Image> dst = new Image();

	if (gdImageTrueColor(m_image))
		dst->m_image = gdImageCreateTrueColor(width, height);
	else
		dst->m_image = gdImageCreate(width, height);

	gdImagePaletteCopy(dst->m_image, m_image);
	gdImageColorTransparent(dst->m_image, gdImageGetTransparent(m_image));

	dst->setExtMemory();

	gdImageAlphaBlending(dst->m_image, 0);
	gdImageCopyResampled(dst->m_image, m_image, 0, 0, 0, 0, width, height,
			gdImageSX(m_image), gdImageSY(m_image));
	gdImageAlphaBlending(dst->m_image, 1);

	retVal = dst;
	return 0;
}

result_t Image::flip(int32_t dir, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	if (dir == gd_base::_HORIZONTAL)
		gdImageFlipHorizontal(m_image);
	else if (dir == gd_base::_VERTICAL)
		gdImageFlipVertical(m_image);
	else if (dir == gd_base::_BOTH)
		gdImageFlipBoth(m_image);
	else
		return CALL_E_INVALIDARG;

	return 0;
}

result_t Image::convert(int32_t color, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	if (color != gd_base::_TRUECOLOR && color != gd_base::_PALETTE)
		return CALL_E_INVALIDARG;

	if (color == gd_base::_TRUECOLOR && !gdImageTrueColor(m_image))
	{
		int sx = gdImageSX(m_image);
		int sy = gdImageSY(m_image);
		gdImagePtr newImage = gdImageCreateTrueColor(sx, sy);
		int trans = gdImageGetTransparent(m_image);

		if (trans != -1)
		{
			trans = gdImageColorAllocate(newImage, m_image->red[trans],
					m_image->green[trans], m_image->blue[trans]);
			gdImageFilledRectangle(newImage, 0, 0, sx, sy, trans);
			gdImageColorTransparent(newImage, trans);
		}

		gdImageCopy(newImage, m_image, 0, 0, 0, 0, sx, sy);

		setExtMemory(-1);
		gdImageDestroy(m_image);
		m_image = newImage;
		setExtMemory();
	}
	else if (color == gd_base::_PALETTE && gdImageTrueColor(m_image))
	{
		setExtMemory(-1);
		gdImageTrueColorToPalette(m_image, 1, 256);
		setExtMemory();
	}
	else
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Image::copy(Image_base* source, int32_t dstX, int32_t dstY,
		int32_t srcX, int32_t srcY, int32_t width, int32_t height,
		exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	Image* src = (Image*) source;
	if (!src->m_image)
		return CALL_E_INVALID_CALL;

	gdImageCopy(m_image, src->m_image, dstX, dstY, srcX, srcY, width, height);
	return 0;
}

result_t Image::copyMerge(Image_base* source, int32_t dstX, int32_t dstY,
		int32_t srcX, int32_t srcY, int32_t width, int32_t height,
		int32_t percent, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	Image* src = (Image*) source;
	if (!src->m_image)
		return CALL_E_INVALID_CALL;

	gdImageCopyMerge(m_image, src->m_image, dstX, dstY, srcX, srcY, width,
			height, percent);
	return 0;
}

result_t Image::copyMergeGray(Image_base* source, int32_t dstX, int32_t dstY,
		int32_t srcX, int32_t srcY, int32_t width, int32_t height,
		int32_t percent, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	Image* src = (Image*) source;
	if (!src->m_image)
		return CALL_E_INVALID_CALL;

	gdImageCopyMergeGray(m_image, src->m_image, dstX, dstY, srcX, srcY, width,
			height, percent);
	return 0;
}

result_t Image::copyResized(Image_base* source, int32_t dstX, int32_t dstY,
		int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW,
		int32_t srcH, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	Image* src = (Image*) source;
	if (!src->m_image)
		return CALL_E_INVALID_CALL;

	gdImageCopyResized(m_image, src->m_image, dstX, dstY, srcX, srcY, dstW,
			dstH, srcW, srcH);
	return 0;
}

result_t Image::copyResampled(Image_base* source, int32_t dstX, int32_t dstY,
		int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW,
		int32_t srcH, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	Image* src = (Image*) source;
	if (!src->m_image)
		return CALL_E_INVALID_CALL;

	gdImageCopyResampled(m_image, src->m_image, dstX, dstY, srcX, srcY, dstW,
			dstH, srcW, srcH);
	return 0;
}

result_t Image::copyRotated(Image_base* source, int32_t dstX, int32_t dstY,
		int32_t srcX, int32_t srcY, int32_t width, int32_t height,
		int32_t angle, exlib::AsyncEvent* ac)
{
	if (!m_image)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	Image* src = (Image*) source;
	if (!src->m_image)
		return CALL_E_INVALID_CALL;

	gdImageCopyRotated(m_image, src->m_image, dstX, dstY, srcX, srcY, width,
			height, angle);
	return 0;
}

} /* namespace fibjs */
