/*
 * Image.h
 *
 *  Created on: Sep 10, 2012
 *      Author: lion
 */

#include "ifs/Image.h"
#include "ifs/SeekableStream.h"
#include <gd.h>
#include <libexif/exif-data.h>

#ifndef IMAGE_H_
#define IMAGE_H_

namespace fibjs
{

class Image: public Image_base
{
public:
    Image() :
        m_image(NULL), m_ed(NULL), m_type(gd_base::_NONE)
    {
    }

    ~Image()
    {
        if (m_image)
            gdImageDestroy(m_image);

        if (m_ed)
            exif_data_unref(m_ed);
    }

public:
    // Image_base
    virtual result_t get_width(int32_t &retVal);
    virtual result_t get_height(int32_t &retVal);
    virtual result_t get_format(int32_t &retVal);
    virtual result_t get_type(int32_t &retVal);
    virtual result_t get_colorsTotal(int32_t &retVal);
    virtual result_t get_transparent(int32_t &retVal);
    virtual result_t set_transparent(int32_t newVal);
    virtual result_t get_alphaBlending(bool &retVal);
    virtual result_t set_alphaBlending(bool newVal);
    virtual result_t getData(int32_t format, int32_t quality, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t save(Stream_base *stm, int32_t format, int32_t quality, exlib::AsyncEvent *ac);
    virtual result_t save(const char *fname, int32_t format, int32_t quality, exlib::AsyncEvent *ac);
    virtual result_t colorAllocate(int32_t red, int32_t green, int32_t blue, int32_t &retVal);
    virtual result_t colorAllocateAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t &retVal);
    virtual result_t colorDeallocate(int32_t colorIndex);
    virtual result_t colorClosest(int32_t red, int32_t green, int32_t blue, int32_t &retVal);
    virtual result_t colorClosestHWB(int32_t red, int32_t green, int32_t blue, int32_t &retVal);
    virtual result_t colorClosestAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t &retVal);
    virtual result_t colorExact(int32_t red, int32_t green, int32_t blue, int32_t &retVal);
    virtual result_t colorExactAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t &retVal);
    virtual result_t colorResolve(int32_t red, int32_t green, int32_t blue, int32_t &retVal);
    virtual result_t colorResolveAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t &retVal);
    virtual result_t clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    virtual result_t getPixel(int32_t x, int32_t y, int32_t &retVal);
    virtual result_t getTrueColorPixel(int32_t x, int32_t y, int32_t &retVal);
    virtual result_t setPixel(int32_t x, int32_t y, int32_t color);
    virtual result_t setThickness(int32_t thickness);
    virtual result_t line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color);
    virtual result_t rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color);
    virtual result_t filledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color);
    virtual result_t polygon(v8::Local<v8::Array> points, int32_t color);
    virtual result_t openPolygon(v8::Local<v8::Array> points, int32_t color);
    virtual result_t filledPolygon(v8::Local<v8::Array> points, int32_t color);
    virtual result_t ellipse(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color);
    virtual result_t filledEllipse(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color);
    virtual result_t arc(int32_t x, int32_t y, int32_t width, int32_t height, int32_t start, int32_t end, int32_t color);
    virtual result_t filledArc(int32_t x, int32_t y, int32_t width, int32_t height, int32_t start, int32_t end, int32_t color, int32_t style);
    virtual result_t fill(int32_t x, int32_t y, int32_t color);
    virtual result_t fillToBorder(int32_t x, int32_t y, int32_t borderColor, int32_t color);
    virtual result_t colorReplace(int32_t src, int32_t dst, exlib::AsyncEvent *ac);
    virtual result_t clone(obj_ptr<Image_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t resample(int32_t width, int32_t height, obj_ptr<Image_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t crop(int32_t x, int32_t y, int32_t width, int32_t height, obj_ptr<Image_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t flip(int32_t dir, exlib::AsyncEvent *ac);
    virtual result_t rotate(int32_t dir, exlib::AsyncEvent *ac);
    virtual result_t convert(int32_t color, exlib::AsyncEvent *ac);
    virtual result_t copy(Image_base *source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, exlib::AsyncEvent *ac);
    virtual result_t copyMerge(Image_base *source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t percent, exlib::AsyncEvent *ac);
    virtual result_t copyMergeGray(Image_base *source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t percent, exlib::AsyncEvent *ac);
    virtual result_t copyResized(Image_base *source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW, int32_t srcH, exlib::AsyncEvent *ac);
    virtual result_t copyResampled(Image_base *source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW, int32_t srcH, exlib::AsyncEvent *ac);
    virtual result_t copyRotated(Image_base *source, double dstX, double dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t angle, exlib::AsyncEvent *ac);

public:
    result_t create(int32_t width, int32_t height, int32_t color);
    result_t load(Buffer_base *data);
    result_t rotate(int32_t dir);

private:
    result_t New(int32_t width, int32_t height, obj_ptr<Image> &retVal);
    void setExtMemory(int add = 1);

private:
    gdImagePtr m_image;
    ExifData *m_ed;
    int32_t m_type;
};

} /* namespace fibjs */
#endif /* IMAGE_H_ */
