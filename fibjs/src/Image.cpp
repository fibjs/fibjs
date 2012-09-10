/*
 * Image.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: lion
 */

#include "Image.h"
#include "Buffer.h"

namespace fibjs
{

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

result_t gd_base::loadFrom(SeekableStream_base* stm,
		obj_ptr<Image_base>& retVal, exlib::AsyncEvent* ac)
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
			return pThis->m_stm->read((int32_t)len, pThis->m_buffer, pThis);
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

	if (m_image == NULL)
		return CALL_E_INVALIDARG;

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
	else if (ch1 == 0x49 && ch2 == 0x49)
		format = gd_base::_TIFF;
	else if (ch1 == 0x4d && ch2 == 0x4d)
		format = gd_base::_TIFF;
	else if (ch1 == 0x42 && ch2 == 0x4d)
		format = gd_base::_BMP;
	else if (ch1 == 0xff && ch2 == 0xfe)
		format = gd_base::_GD;
	else if (ch1 == 0x67 && ch2 == 0x64)
		format = gd_base::_GD2;
	else
		return CALL_E_INVALID_DATA;

	switch (format)
	{
	case gd_base::_GIF:
		m_image = gdImageCreateFromGifPtr((int)strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_PNG:
		m_image = gdImageCreateFromPngPtr((int)strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_JPEG:
		m_image = gdImageCreateFromJpegPtr((int)strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_TIFF:
		m_image = gdImageCreateFromTiffPtr((int)strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_BMP:
		m_image = gdImageCreateFromBmpPtr((int)strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_GD:
		m_image = gdImageCreateFromGdPtr((int)strBuf.length(),
				(void*) strBuf.c_str());
		break;
	case gd_base::_GD2:
		m_image = gdImageCreateFromGd2Ptr((int)strBuf.length(),
				(void*) strBuf.c_str());
		break;
	}

	if (m_image == NULL)
		return CALL_E_INVALIDARG;

	return 0;
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

} /* namespace fibjs */
