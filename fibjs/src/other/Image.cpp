/*
 * Image.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: lion
 */

#include "utils.h"
#include "Image.h"
#include "Buffer.h"
#include "ifs/fs.h"
#include <vector>
#include <stdlib.h>
#include <exif/exif.h>

namespace fibjs
{

void Image::setExtMemory(int32_t add)
{
    if (m_image)
    {
        int32_t psize =
            gdImageTrueColor(m_image) ? sizeof(int32_t) : sizeof(unsigned char);
        int32_t sx = gdImageSX(m_image);
        int32_t sy = gdImageSY(m_image);
        extMemory(
            (sizeof(gdImage) + sizeof(void *) * sy + psize * sx * sy) * add);
    }
}

result_t gd_base::create(int32_t width, int32_t height, int32_t color,
                         obj_ptr<Image_base> &retVal, AsyncEvent *ac)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Image> img = new Image();
    result_t hr = img->create(width, height, color);
    if (hr < 0)
        return hr;

    retVal = img;
    return 0;
}

result_t gd_base::load(Buffer_base *data, obj_ptr<Image_base> &retVal,
                       AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Image> img = new Image();
    result_t hr = img->load(data);
    if (hr < 0)
        return hr;

    retVal = img;
    return 0;
}

result_t gd_base::load(SeekableStream_base *stm, obj_ptr<Image_base> &retVal,
                       AsyncEvent *ac)
{
    class asyncLoad: public AsyncState
    {
    public:
        asyncLoad(SeekableStream_base *stm, obj_ptr<Image_base> &retVal,
                  AsyncEvent *ac) :
            AsyncState(ac), m_stm(stm), m_retVal(retVal)
        {
            set(read);
        }

        static int32_t read(AsyncState *pState, int32_t n)
        {
            asyncLoad *pThis = (asyncLoad *) pState;
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

        static int32_t load(AsyncState *pState, int32_t n)
        {
            asyncLoad *pThis = (asyncLoad *) pState;

            if (n == CALL_RETURN_NULL)
                return CHECK_ERROR(CALL_E_INVALID_DATA);

            obj_ptr<Image> img = new Image();
            result_t hr = img->load(pThis->m_buffer);
            if (hr < 0)
                return hr;

            pThis->m_retVal = img;
            return pThis->done();
        }

    private:
        obj_ptr<SeekableStream_base> m_stm;
        obj_ptr<Image_base> &m_retVal;
        obj_ptr<Buffer_base> m_buffer;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncLoad(stm, retVal, ac))->post(0);
}

result_t gd_base::load(const char *fname, obj_ptr<Image_base> &retVal,
                       AsyncEvent *ac)
{
    class asyncLoad: public AsyncState
    {
    public:
        asyncLoad(const char *fname, obj_ptr<Image_base> &retVal,
                  AsyncEvent *ac) :
            AsyncState(ac), m_fname(fname), m_retVal(retVal)
        {
            set(open);
        }

        static int32_t open(AsyncState *pState, int32_t n)
        {
            asyncLoad *pThis = (asyncLoad *) pState;

            pThis->set(read);
            return fs_base::open(pThis->m_fname.c_str(), "r", pThis->m_file, pThis);
        }

        static int32_t read(AsyncState *pState, int32_t n)
        {
            asyncLoad *pThis = (asyncLoad *) pState;

            pThis->set(close);
            return load(pThis->m_file, pThis->m_retVal, pThis);
        }

        static int32_t close(AsyncState *pState, int32_t n)
        {
            asyncLoad *pThis = (asyncLoad *) pState;

            pThis->done();
            return pThis->m_file->close(pThis);
        }

    private:
        std::string m_fname;
        obj_ptr<File_base> m_file;
        obj_ptr<Image_base> &m_retVal;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncLoad(fname, retVal, ac))->post(0);
}

result_t gd_base::rgb(int32_t red, int32_t green, int32_t blue, int32_t &retVal)
{
    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdTrueColor(red, green, blue);
    return 0;
}

result_t gd_base::rgba(int32_t red, int32_t green, int32_t blue,
                       double alpha, int32_t &retVal)
{
    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255 || alpha < 0 || alpha > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdTrueColorAlpha(red, green, blue, doubleToInt(alpha * 127));
    return 0;
}

inline int32_t rgb_quant(double p, double q, double h)
{
    if (h < 0)
        h += 360;

    if (h >= 360 )
        h -= 360;

    if (h < 60)
        return doubleToInt((p + (q - p) * h / 60) * 255);

    if (h < 180)
        return doubleToInt(q * 255 );

    if (h < 240)
        return doubleToInt((p + (q - p) * (240 - h) / 60) * 255);

    return doubleToInt(p * 255);
}

inline int32_t hsl2rgb(double h, double s, double l)
{
    double p, q;

    if (s == 0)
    {
        int32_t c = doubleToInt(l * 255);
        return gdTrueColor(c, c, c);
    }

    if (l <= 0.5)
    {
        p = l * (1 - s);
        q = 2 * l - p;
    }
    else
    {
        q = l + s - (l * s);
        p = 2 * l - q;
    }

    return gdTrueColor(rgb_quant(p, q, h + 120),
                       rgb_quant(p, q, h),
                       rgb_quant(p, q, h - 120));
}

result_t gd_base::hsl(double hue, double saturation, double lightness, int32_t &retVal)
{
    if (hue < 0 || hue > 360 || saturation < 0 || saturation > 1 || lightness < 0
            || lightness > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = hsl2rgb(hue, saturation, lightness);
    return 0;
}

result_t gd_base::hsla(double hue, double saturation, double lightness,
                       double alpha, int32_t &retVal)
{
    if (hue < 0 || hue > 360 || saturation < 0 || saturation > 1 || lightness < 0
            || lightness > 1 || alpha < 0 || alpha > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = (doubleToInt(alpha * 127) << 24) | hsl2rgb(hue, saturation, lightness);
    return 0;
}

inline int32_t hsb2rgb(double h, double s, double b)
{
    double cr = 0, cg = 0, cb = 0;
    int32_t i = doubleToInt(h / 60) % 6;
    double f = (h / 60) - i;
    double p = b * (1 - s);
    double q = b * (1 - f * s);
    double t = b * (1 - (1 - f) * s);

    switch (i) {
    case 0:
        cr = b;
        cg = t;
        cb = p;
        break;
    case 1:
        cr = q;
        cg = b;
        cb = p;
        break;
    case 2:
        cr = p;
        cg = b;
        cb = t;
        break;
    case 3:
        cr = p;
        cg = q;
        cb = b;
        break;
    case 4:
        cr = t;
        cg = p;
        cb = b;
        break;
    case 5:
        cr = b;
        cg = p;
        cb = q;
        break;
    default:
        break;
    }

    return gdTrueColor(doubleToInt (cr * 255.0),
                       doubleToInt (cg * 255.0),
                       doubleToInt (cb * 255.0));
}

result_t gd_base::hsb(double hue, double saturation, double brightness, int32_t& retVal)
{
    if (hue < 0 || hue > 360 || saturation < 0 || saturation > 1 || brightness < 0
            || brightness > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = hsb2rgb(hue, saturation, brightness);
    return 0;
}

result_t gd_base::hsba(double hue, double saturation, double brightness,
                       double alpha, int32_t& retVal)
{
    if (hue < 0 || hue > 360 || saturation < 0 || saturation > 1 || brightness < 0
            || brightness > 1 || alpha < 0 || alpha > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = (doubleToInt(alpha * 127) << 24) | hsb2rgb(hue, saturation, brightness);
    return 0;
}

result_t gd_base::color(const char *color, int32_t &retVal)
{
    if (*color == '#')
        color ++;

    if (!qisxdigit(color[0]) || !qisxdigit(color[1]) || !qisxdigit(color[2]))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!color[3])
        retVal = gdTrueColor(qhex(color[0]) * 17,
                             qhex(color[1]) * 17,
                             qhex(color[2]) * 17);
    else
    {
        if (!qisxdigit(color[3]) || !qisxdigit(color[4]) || !qisxdigit(color[5])
                || color[6])
            return CHECK_ERROR(CALL_E_INVALIDARG);

        retVal = gdTrueColor((qhex(color[0]) << 4) | qhex(color[1]),
                             (qhex(color[2]) << 4) | qhex(color[3]),
                             (qhex(color[4]) << 4) | qhex(color[5]));
    }

    return 0;
}

result_t Image::create(int32_t width, int32_t height, int32_t color)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (color == gd_base::_PALETTE)
        m_image = gdImageCreate(width, height);
    else if (color == gd_base::_TRUECOLOR)
        m_image = gdImageCreateTrueColor(width, height);
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (m_image == NULL)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    setExtMemory();

    return 0;
}

result_t Image::load(Buffer_base *data)
{
    std::string strBuf;
    data->toString(strBuf);

    if (strBuf.length() < 2)
        return CHECK_ERROR(CALL_E_INVALIDARG);

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
    else if (ch1 == 0x52 && ch2 == 0x49)
        format = gd_base::_WEBP;
    else
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    switch (format)
    {
    case gd_base::_GIF:
        m_image = gdImageCreateFromGifPtr((int32_t) strBuf.length(),
                                          (void *) strBuf.c_str());
        break;
    case gd_base::_PNG:
        m_image = gdImageCreateFromPngPtr((int32_t) strBuf.length(),
                                          (void *) strBuf.c_str());
        break;
    case gd_base::_JPEG:
        m_image = gdImageCreateFromJpegPtr((int32_t) strBuf.length(),
                                           (void *) strBuf.c_str());
        if (m_image != NULL)
        {
            EXIFInfo result;
            result.parseFrom((const unsigned char *) strBuf.c_str(),
                             (uint32_t)strBuf.length());

            switch (result.Orientation)
            {
            case 2:
                gdImageFlipHorizontal(m_image);
                break;
            case 3:
                gdImageFlipBoth(m_image);
                break;
            case 4:
                gdImageFlipVertical(m_image);
                break;
            case 5:
                gdImageFlipVertical(m_image);
            case 6:
                rotate(gd_base::_RIGHT);
                break;
            case 7:
                gdImageFlipVertical(m_image);
            case 8:
                rotate(gd_base::_LEFT);
                break;
            }
        }

        break;
    case gd_base::_TIFF:
        m_image = gdImageCreateFromTiffPtr((int32_t) strBuf.length(),
                                           (void *) strBuf.c_str());
        break;
    case gd_base::_BMP:
        m_image = gdImageCreateFromBmpPtr((int32_t) strBuf.length(),
                                          (void *) strBuf.c_str());
        break;
    case gd_base::_WEBP:
        m_image = gdImageCreateFromWebpPtr((int32_t) strBuf.length(),
                                           (void *) strBuf.c_str());
        break;
    }

    if (m_image == NULL)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    setExtMemory();
    m_type = format;

    return 0;
}

int32_t my_replacer(gdImagePtr im, int32_t src)
{
    if (src == gdImageGetTransparent(im))
        return gdTrueColor(255, 255, 255);

    return gdAlphaBlend(gdTrueColor(255, 255, 255), src);
}

result_t Image::getData(int32_t format, int32_t quality,
                        obj_ptr<Buffer_base> &retVal, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t size = 0;
    void *data = NULL;
    int32_t sx = gdImageSX(m_image);
    int32_t sy = gdImageSY(m_image);
    int32_t i, j;
    int32_t trans = -1;

    if (gdImageTrueColor(m_image))
    {
        for (i = 0; i < sx && trans == -1; i ++)
            for (j = 0; j < sy && trans == -1; j++)
                if ((gdImageGetPixel(m_image, i, j) & 0xff000000) != 0)
                    trans = 0;
    }
    else
        trans = gdImageGetTransparent(m_image);

    gdImagePtr nowImage = m_image;

    if (trans != -1)
    {
        if (format != gd_base::_PNG)
        {
            if (gdImageTrueColor(m_image))
                nowImage = gdImageCreateTrueColor(sx, sy);
            else
            {
                nowImage = gdImageCreate(sx, sy);
                gdImagePaletteCopy(nowImage, m_image);
            }

            gdImageFilledRectangle(nowImage, 0, 0, sx, sy, gdImageColorAllocate(nowImage, 255, 255, 255));
            gdImageCopy(nowImage, m_image, 0, 0, 0, 0, sx, sy);
        }
        else if (gdImageTrueColor(m_image))
            gdImageSaveAlpha(m_image, 1);
    }

    switch (format)
    {
    case gd_base::_GIF:
        data = gdImageGifPtr(nowImage, &size);
        break;
    case gd_base::_PNG:
        data = gdImagePngPtr(nowImage, &size);
        break;
    case gd_base::_JPEG:
    {
        unsigned char *ed_data = NULL;
        uint32_t  ed_size = 0;

        data = gdImageJpegPtr(nowImage, &size, quality, ed_data, ed_size);

        if (ed_data)
            free(ed_data);

        break;
    }
    case gd_base::_TIFF:
        data = gdImageTiffPtr(nowImage, &size);
        break;
    case gd_base::_BMP:
        data = gdImageBmpPtr(nowImage, &size, 1);
        break;
    case gd_base::_WEBP:
        data = gdImageWebpPtrEx(nowImage, &size, quality);
        break;
    }

    if (nowImage != m_image)
        gdImageDestroy(nowImage);

    if (data == NULL)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = new Buffer(std::string((char *) data, size));
    gdFree(data);

    return 0;
}

result_t Image::save(Stream_base *stm, int32_t format, int32_t quality,
                     AsyncEvent *ac)
{
    class asyncSave: public AsyncState
    {
    public:
        asyncSave(Image *img, Stream_base *stm, int32_t format, int32_t quality,
                  AsyncEvent *ac) :
            AsyncState(ac), m_pThis(img), m_stm(stm), m_format(format), m_quality(quality)
        {
            set(getData);
        }

        static int32_t getData(AsyncState *pState, int32_t n)
        {
            asyncSave *pThis = (asyncSave *) pState;

            pThis->set(save);
            return pThis->m_pThis->getData(pThis->m_format, pThis->m_quality, pThis->m_buf, pThis);
        }

        static int32_t save(AsyncState *pState, int32_t n)
        {
            asyncSave *pThis = (asyncSave *) pState;

            pThis->done();
            return pThis->m_stm->write(pThis->m_buf, pThis);
        }

    private:
        obj_ptr<Image> m_pThis;
        obj_ptr<Stream_base> m_stm;
        int32_t m_format;
        int32_t m_quality;
        obj_ptr<Buffer_base> m_buf;
    };

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSave(this, stm, format, quality, ac))->post(0);
}

result_t Image::save(const char *fname, int32_t format, int32_t quality,
                     AsyncEvent *ac)
{
    class asyncSave: public AsyncState
    {
    public:
        asyncSave(Image *img, const char *fname, int32_t format, int32_t quality,
                  AsyncEvent *ac) :
            AsyncState(ac), m_pThis(img), m_fname(fname), m_format(format), m_quality(quality)
        {
            set(open);
        }

        static int32_t open(AsyncState *pState, int32_t n)
        {
            asyncSave *pThis = (asyncSave *) pState;

            pThis->set(save);
            return fs_base::open(pThis->m_fname.c_str(), "w", pThis->m_file, pThis);
        }

        static int32_t save(AsyncState *pState, int32_t n)
        {
            asyncSave *pThis = (asyncSave *) pState;

            pThis->set(close);
            return pThis->m_pThis->save(pThis->m_file, pThis->m_format, pThis->m_quality, pThis);
        }

        static int32_t close(AsyncState *pState, int32_t n)
        {
            asyncSave *pThis = (asyncSave *) pState;

            pThis->done();
            return pThis->m_file->close(pThis);
        }

    private:
        obj_ptr<Image> m_pThis;
        std::string m_fname;
        obj_ptr<File_base> m_file;
        int32_t m_format;
        int32_t m_quality;
        obj_ptr<Buffer_base> m_buf;
    };

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSave(this, fname, format, quality, ac))->post(0);
}

result_t Image::get_width(int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = gdImageSX(m_image);
    return 0;
}

result_t Image::get_height(int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = gdImageSY(m_image);
    return 0;
}

result_t Image::get_format(int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_type;
    return 0;
}

result_t Image::get_type(int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal =
        gdImageTrueColor(m_image) ? gd_base::_TRUECOLOR : gd_base::_PALETTE;
    return 0;
}

result_t Image::get_colorsTotal(int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = gdImageColorsTotal(m_image);
    return 0;
}

result_t Image::colorAllocate(int32_t red, int32_t green, int32_t blue,
                              int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorAllocate(m_image, red, green, blue);
    return 0;
}

result_t Image::colorAllocate(int32_t color, int32_t &retVal)
{
    return colorAllocate((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, retVal);
}

result_t Image::colorAllocateAlpha(int32_t red, int32_t green, int32_t blue,
                                   double alpha, int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255 || alpha < 0 || alpha > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorAllocateAlpha(m_image, red, green, blue, doubleToInt(alpha * 127));
    return 0;
}

result_t Image::colorAllocateAlpha(int32_t color, int32_t &retVal)
{
    return colorAllocateAlpha((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff,
                              ((color >> 24) & 0xff) / 127.0, retVal);
}

result_t Image::colorClosest(int32_t red, int32_t green, int32_t blue,
                             int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorClosest(m_image, red, green, blue);
    return 0;
}

result_t Image::colorClosest(int32_t color, int32_t &retVal)
{
    return colorClosest((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, retVal);
}

result_t Image::colorClosestHWB(int32_t red, int32_t green, int32_t blue,
                                int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorClosestHWB(m_image, red, green, blue);
    return 0;
}

result_t Image::colorClosestHWB(int32_t color, int32_t &retVal)
{
    return colorClosestHWB((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, retVal);
}

result_t Image::colorClosestAlpha(int32_t red, int32_t green, int32_t blue,
                                  double alpha, int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255 || alpha < 0 || alpha > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorClosestAlpha(m_image, red, green, blue, doubleToInt(alpha * 127));
    return 0;
}

result_t Image::colorClosestAlpha(int32_t color, int32_t &retVal)
{
    return colorClosestAlpha((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff,
                             ((color >> 24) & 0xff) / 127.0, retVal);
}

result_t Image::colorExact(int32_t red, int32_t green, int32_t blue,
                           int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorExact(m_image, red, green, blue);
    return 0;
}

result_t Image::colorExact(int32_t color, int32_t &retVal)
{
    return colorExact((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, retVal);
}

result_t Image::colorExactAlpha(int32_t red, int32_t green, int32_t blue,
                                double alpha, int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255 || alpha < 0 || alpha > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorExactAlpha(m_image, red, green, blue, doubleToInt(alpha * 127));
    return 0;
}

result_t Image::colorExactAlpha(int32_t color, int32_t &retVal)
{
    return colorExactAlpha((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff,
                           ((color >> 24) & 0xff) / 127.0, retVal);
}

result_t Image::colorResolve(int32_t red, int32_t green, int32_t blue,
                             int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorResolve(m_image, red, green, blue);
    return 0;
}

result_t Image::colorResolve(int32_t color, int32_t &retVal)
{
    return colorResolve((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, retVal);
}

result_t Image::colorResolveAlpha(int32_t red, int32_t green, int32_t blue,
                                  double alpha, int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0
            || blue > 255 || alpha < 0 || alpha > 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = gdImageColorResolveAlpha(m_image, red, green, blue, doubleToInt(alpha * 127));
    return 0;
}

result_t Image::colorResolveAlpha(int32_t color, int32_t &retVal)
{
    return colorResolveAlpha((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff,
                             ((color >> 24) & 0xff) / 127.0, retVal);
}

result_t Image::colorDeallocate(int32_t colorIndex)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageColorDeallocate(m_image, colorIndex);
    return 0;
}

result_t Image::clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageSetClip(m_image, x1, y1, x2, x2);
    return 0;
}

result_t Image::getPixel(int32_t x, int32_t y, int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = gdImageGetPixel(m_image, x, y);
    return 0;
}

result_t Image::getTrueColorPixel(int32_t x, int32_t y, int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = gdImageGetTrueColorPixel(m_image, x, y);
    return 0;
}

result_t Image::setPixel(int32_t x, int32_t y, int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageSetPixel(m_image, x, y, color);
    return 0;
}

result_t Image::get_transparent(int32_t &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = gdImageGetTransparent(m_image);
    return 0;
}

result_t Image::set_transparent(int32_t newVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageColorTransparent(m_image, newVal);
    return 0;
}

result_t Image::get_progressive(bool& retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = gdImageGetInterlaced(m_image) != 0;
    return 0;
}

result_t Image::set_progressive(bool newVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageInterlace(m_image, newVal);
    return 0;
}

result_t Image::get_alphaBlending(bool &retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_image->alphaBlendingFlag != 0;
    return 0;
}

result_t Image::set_alphaBlending(bool newVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageAlphaBlending(m_image, newVal);
    return 0;
}

result_t Image::setThickness(int32_t thickness)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageSetThickness(m_image, thickness);
    return 0;
}

result_t Image::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                     int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageLine(m_image, x1, y1, x2, y2, color);
    return 0;
}

result_t Image::rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                          int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageRectangle(m_image, x1, y1, x2, y2, color);
    return 0;
}

result_t Image::filledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageFilledRectangle(m_image, x1, y1, x2, y2, color);
    return 0;
}

result_t getPoints(v8::Local<v8::Array> &points, std::vector<gdPoint> &pts)
{
    int32_t i, len = points->Length();

    pts.resize(len);

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> v = points->Get(i);

        if (!v->IsArray())
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        v8::Local<v8::Array> pt = v8::Local<v8::Array>::Cast(v);

        if (pt->Length() != 2)
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        pts[i].x = pt->Get(0)->Int32Value();
        pts[i].y = pt->Get(1)->Int32Value();
    }

    return 0;
}

result_t Image::polygon(v8::Local<v8::Array> points, int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::vector < gdPoint > pts;

    result_t hr = getPoints(points, pts);
    if (hr < 0)
        return hr;

    gdImagePolygon(m_image, pts.data(), (int32_t) pts.size(), color);
    return 0;
}

result_t Image::openPolygon(v8::Local<v8::Array> points, int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::vector < gdPoint > pts;

    result_t hr = getPoints(points, pts);
    if (hr < 0)
        return hr;

    gdImageOpenPolygon(m_image, pts.data(), (int32_t) pts.size(), color);
    return 0;
}

result_t Image::filledPolygon(v8::Local<v8::Array> points, int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::vector < gdPoint > pts;

    result_t hr = getPoints(points, pts);
    if (hr < 0)
        return hr;

    gdImageFilledPolygon(m_image, pts.data(), (int32_t) pts.size(), color);
    return 0;
}

result_t Image::ellipse(int32_t x, int32_t y, int32_t width, int32_t height,
                        int32_t color)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageEllipse(m_image, x, y, width, height, color);
    return 0;
}

result_t Image::filledEllipse(int32_t x, int32_t y, int32_t width,
                              int32_t height, int32_t color)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageFilledEllipse(m_image, x, y, width, height, color);
    return 0;
}

result_t Image::arc(int32_t x, int32_t y, int32_t width, int32_t height,
                    int32_t start, int32_t end, int32_t color)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (start < 0 || start > 360 || end < 0 || end > 360)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    gdImageArc(m_image, x, y, width, height, start, end, color);
    return 0;
}

result_t Image::filledArc(int32_t x, int32_t y, int32_t width, int32_t height,
                          int32_t start, int32_t end, int32_t color, int32_t style)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (start < 0 || start > 360 || end < 0 || end > 360)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    gdImageFilledArc(m_image, x, y, width, height, start, end, color, style);
    return 0;
}

result_t Image::fill(int32_t x, int32_t y, int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageFill(m_image, x, y, color);
    return 0;
}

result_t Image::fillToBorder(int32_t x, int32_t y, int32_t borderColor,
                             int32_t color)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageFillToBorder(m_image, x, y, borderColor, color);
    return 0;
}

result_t Image::New(int32_t width, int32_t height, obj_ptr<Image> &retVal)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

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

result_t Image::colorReplace(int32_t src, int32_t dst, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    gdImageColorReplace(m_image, src, dst);
    return 0;
}

result_t Image::crop(int32_t x, int32_t y, int32_t width, int32_t height,
                     obj_ptr<Image_base> &retVal, AsyncEvent *ac)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Image> dst;
    result_t hr = New(width, height, dst);
    if (hr < 0)
        return hr;

    gdImageAlphaBlending(dst->m_image, 0);
    gdImageCopy(dst->m_image, m_image, 0, 0, x, y, width, height);
    gdImageAlphaBlending(dst->m_image, 1);

    retVal = dst;

    return 0;
}

result_t Image::clone(obj_ptr<Image_base> &retVal, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Image> img = new Image();
    img->m_image = gdImageClone(m_image);
    img->setExtMemory();

    retVal = img;

    return 0;
}

result_t Image::resample(int32_t width, int32_t height,
                         obj_ptr<Image_base> &retVal, AsyncEvent *ac)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Image> dst;
    result_t hr = New(width, height, dst);
    if (hr < 0)
        return hr;

    gdImageAlphaBlending(dst->m_image, 0);
    gdImageCopyResampled(dst->m_image, m_image, 0, 0, 0, 0, width, height,
                         gdImageSX(m_image), gdImageSY(m_image));
    gdImageAlphaBlending(dst->m_image, 1);

    retVal = dst;
    return 0;
}

result_t Image::flip(int32_t dir, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (dir == gd_base::_HORIZONTAL)
        gdImageFlipHorizontal(m_image);
    else if (dir == gd_base::_VERTICAL)
        gdImageFlipVertical(m_image);
    else if (dir == gd_base::_BOTH)
        gdImageFlipBoth(m_image);
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return 0;
}

result_t Image::rotate(int32_t dir)
{
    if (dir != gd_base::_LEFT && dir != gd_base::_RIGHT)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t sx = gdImageSX(m_image);
    int32_t sy = gdImageSY(m_image);
    int32_t i, j;
    gdImagePtr newImage;

    if (gdImageTrueColor(m_image))
        newImage = gdImageCreateTrueColor(sy, sx);
    else
    {
        newImage = gdImageCreate(sy, sx);
        gdImagePaletteCopy(newImage, m_image);
    }

    gdImageColorTransparent(newImage, gdImageGetTransparent(m_image));

    if (dir == gd_base::_LEFT)
    {
        for (i = 0; i < sx; i++)
            for (j = 0; j < sy; j++)
                gdImageSetPixel(newImage, j, sx - i - 1,
                                gdImageGetPixel(m_image, i, j));
    }
    else
    {
        for (i = 0; i < sx; i++)
            for (j = 0; j < sy; j++)
                gdImageSetPixel(newImage, sy - j - 1, i,
                                gdImageGetPixel(m_image, i, j));
    }

    gdImageDestroy(m_image);
    m_image = newImage;

    return 0;
}

result_t Image::rotate(int32_t dir, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return rotate(dir);
}

result_t Image::convert(int32_t color, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (color != gd_base::_TRUECOLOR && color != gd_base::_PALETTE)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (color == gd_base::_TRUECOLOR && !gdImageTrueColor(m_image))
    {
        setExtMemory(-1);
        gdImagePaletteToTrueColor(m_image);
        setExtMemory();
    }
    else if (color == gd_base::_PALETTE && gdImageTrueColor(m_image))
    {
        setExtMemory(-1);
        gdImageTrueColorToPalette(m_image, 1, 256);
        setExtMemory();
    }
    else
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return 0;
}

result_t Image::copy(Image_base *source, int32_t dstX, int32_t dstY,
                     int32_t srcX, int32_t srcY, int32_t width, int32_t height,
                     AsyncEvent *ac)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    Image *src = (Image *) source;
    if (!src->m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageCopy(m_image, src->m_image, dstX, dstY, srcX, srcY, width, height);
    return 0;
}

result_t Image::copyMerge(Image_base *source, int32_t dstX, int32_t dstY,
                          int32_t srcX, int32_t srcY, int32_t width, int32_t height,
                          int32_t percent, AsyncEvent *ac)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    Image *src = (Image *) source;
    if (!src->m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageCopyMerge(m_image, src->m_image, dstX, dstY, srcX, srcY, width,
                     height, percent);
    return 0;
}

result_t Image::copyMergeGray(Image_base *source, int32_t dstX, int32_t dstY,
                              int32_t srcX, int32_t srcY, int32_t width, int32_t height,
                              int32_t percent, AsyncEvent *ac)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    Image *src = (Image *) source;
    if (!src->m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageCopyMergeGray(m_image, src->m_image, dstX, dstY, srcX, srcY, width,
                         height, percent);
    return 0;
}

result_t Image::copyResized(Image_base *source, int32_t dstX, int32_t dstY,
                            int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW,
                            int32_t srcH, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    Image *src = (Image *) source;
    if (!src->m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageCopyResized(m_image, src->m_image, dstX, dstY, srcX, srcY, dstW,
                       dstH, srcW, srcH);
    return 0;
}

result_t Image::copyResampled(Image_base *source, int32_t dstX, int32_t dstY,
                              int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW,
                              int32_t srcH, AsyncEvent *ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    Image *src = (Image *) source;
    if (!src->m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageCopyResampled(m_image, src->m_image, dstX, dstY, srcX, srcY, dstW,
                         dstH, srcW, srcH);
    return 0;
}

result_t Image::copyRotated(Image_base *source, double dstX, double dstY,
                            int32_t srcX, int32_t srcY, int32_t width, int32_t height,
                            double angle, AsyncEvent *ac)
{
    if (width <= 0 || height <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    Image *src = (Image *) source;
    if (!src->m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    gdImageCopyRotated(m_image, src->m_image, dstX, dstY, srcX, srcY, width,
                       height, angle);
    return 0;
}

result_t Image::filter(int32_t filterType, double arg1, double arg2, double arg3, double arg4, AsyncEvent* ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    switch (filterType)
    {
    case gd_base::_MEAN_REMOVAL:
        gdImageMeanRemoval(m_image);
        break;
    case gd_base::_EDGEDETECT:
        gdImageEdgeDetectQuick(m_image);
        break;
    case gd_base::_EMBOSS:
        gdImageEmboss(m_image);
        break;
    case gd_base::_SELECTIVE_BLUR:
        gdImageSelectiveBlur(m_image);
        break;
    case gd_base::_GAUSSIAN_BLUR:
        gdImageGaussianBlur(m_image);
        break;
    case gd_base::_NEGATE:
        gdImageNegate(m_image);
        break;
    case gd_base::_GRAYSCALE:
        gdImageGrayScale(m_image);
        break;
    case gd_base::_SMOOTH:
        gdImageSmooth(m_image, (float)arg1);
        break;
    case gd_base::_BRIGHTNESS:
        if (arg1 < -255 || arg1 > 255)
            return CHECK_ERROR(CALL_E_INVALIDARG);
        gdImageBrightness(m_image, (int)arg1);
        break;
    case gd_base::_CONTRAST:
        if (arg1 < 0 || arg1 > 100)
            return CHECK_ERROR(CALL_E_INVALIDARG);
        gdImageContrast(m_image, (int)arg1);
        break;
    case gd_base::_COLORIZE:
        if (arg1 < 0 || arg2 < 0 || arg3 < 0 || arg4 < 0 || arg1 > 255 || arg2 > 255 || arg3 > 255 || arg4 > 127)
            return CHECK_ERROR(CALL_E_INVALIDARG);
        gdImageColor(m_image, (int)arg1, (int)arg2, (int)arg3, (int)arg4);
        break;
    }

    return 0;
}

result_t Image::affine(v8::Local<v8::Array> affine, int32_t x, int32_t y, int32_t width, int32_t height,
                       obj_ptr<Image_base>& retVal)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<Image> dst;
    if (x == -1 && y == -1 && width == -1 && height == -1) {
        width = gdImageSX(m_image);
        height = gdImageSY(m_image);
    }
    else if (x < 0 || y < 0 || width < 0 || height < 0 || affine->Length() != 6 )
        return CHECK_ERROR(CALL_E_INVALIDARG);

    result_t hr = New(width, height, dst);
    if (hr < 0)
        return hr;
    gdRect rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;

    double affineMatrix[6];
    for ( int32_t i = 0; i <= 5; i++)
        affineMatrix[i] = affine->Get(i)->NumberValue();

    gdTransformAffineGetImage(&dst->m_image, m_image, &rect, affineMatrix);
    retVal = dst;

    return 0;
}


result_t Image::gaussianBlur(int32_t radius, AsyncEvent* ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (radius >= gdImageSY(m_image) || radius >= gdImageSX(m_image))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    gdImagePtr dst = gdImageCopyGaussianBlurred(m_image, radius, -1);
    if (dst)
    {
        gdImageDestroy(m_image);
        m_image = dst;
    }

    return 0;
}

} /* namespace fibjs */
