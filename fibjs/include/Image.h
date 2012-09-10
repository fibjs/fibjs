/*
 * Image.h
 *
 *  Created on: Sep 10, 2012
 *      Author: lion
 */

#include "ifs/Image.h"
#include "ifs/SeekableStream.h"
#include <gd.h>

#ifndef IMAGE_H_
#define IMAGE_H_

namespace fibjs
{

class Image: public Image_base
{
public:
	Image() :
			m_image(NULL)
	{
	}

	~Image()
	{
		if (m_image)
			gdImageDestroy(m_image);
	}

public:
	// Image_base
	virtual result_t getData(int32_t format, int32_t quality, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t save(Stream_base* stm, int32_t format, int32_t quality, exlib::AsyncEvent* ac);

public:
	result_t create(int32_t width, int32_t height, int32_t color);
	result_t load(Buffer_base* data);

private:
	gdImagePtr m_image;
};

} /* namespace fibjs */
#endif /* IMAGE_H_ */
