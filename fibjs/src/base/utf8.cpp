#include "utf8.h"

namespace fibjs {

inline char _swap(char ch)
{
    return ch;
}

inline exlib::wchar _swap(exlib::wchar ch)
{
    return ((ch & 0xff) << 8)
        | ((ch & 0xff00) >> 8);
}

inline exlib::wchar32 _swap(exlib::wchar32 ch)
{
    return ((ch & 0xff) << 24)
        | ((ch & 0xff00) << 8)
        | ((ch & 0xff0000) >> 8)
        | ((ch & 0xff000000) >> 24);
}

inline exlib::wchar32 _getchar(const char*& src, const char* end)
{
    unsigned char ch = (unsigned char)*src++;
    if (ch < 0x80)
        return ch;

    ssize_t len;
    exlib::wchar32 res;

    if (ch < 0xc0 || ch >= 0xfe) {
        len = 0;
        res = ch & 0x7f;
    } else if (ch < 0xe0) {
        len = 1;
        res = ch & 0x1f;
    } else if (ch < 0xf0) {
        len = 2;
        res = ch & 0x0f;
    } else if (ch < 0xf8) {
        len = 3;
        res = ch & 0x07;
    } else if (ch < 0xfc) {
        len = 4;
        res = ch & 0x03;
    } else {
        len = 5;
        res = ch & 0x01;
    }

    switch (len) {
    case 0:
        return 0xFFFD;
    case 5:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return 0xFFFD;

        res = (res << 6) | ch;
        src++;
    case 4:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return 0xFFFD;

        res = (res << 6) | ch;
        src++;
    case 3:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return 0xFFFD;

        res = (res << 6) | ch;
        src++;
    case 2:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return 0xFFFD;

        res = (res << 6) | ch;
        src++;
    case 1:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return 0xFFFD;

        res = (res << 6) | ch;
        src++;
    }

    return res;
}

inline exlib::wchar32 _getchar_s(const char*& src, const char* end)
{
    return _getchar(src, end);
}

inline ssize_t _putchar(exlib::wchar32 ch, char*& dst, const char* end)
{
    ssize_t count;
    ssize_t i;

    if (ch < 0x80) {
        if (dst)
            *dst++ = (char)ch;
        return 1;
    } else {
        exlib::wchar ch1;

        if (ch < 0x800) {
            count = 2;
            ch1 = 0xc0;
        } else if (ch < 0x10000) {
            count = 3;
            ch1 = 0xe0;
        } else if (ch < 0x200000) {
            count = 4;
            ch1 = 0xf0;
        } else if (ch < 0x4000000) {
            count = 5;
            ch1 = 0xf8;
        } else {
            count = 6;
            ch1 = 0xfc;
        }

        if (dst) {
            char buf[8];

            for (i = count - 1; i > 0; i--) {
                buf[i] = (char)((ch & 0x3f) | 0x80);
                ch >>= 6;
            }
            buf[0] = (char)(ch | ch1);

            if (dst + count > end)
                count = (ssize_t)(end - dst);

            memcpy(dst, buf, count);
            dst += count;
        }

        return count;
    }

    return 0;
}

inline ssize_t _putchar_s(exlib::wchar32 ch, char*& dst, const char* end)
{
    return _putchar(ch, dst, end);
}

inline exlib::wchar32 _getchar(const exlib::wchar*& src, const exlib::wchar* end)
{
    exlib::wchar32 ch;

    if (((ch = *src++) & 0xf800) != 0xd800)
        return ch;

    exlib::wchar32 ch1;
    if (src >= end || ((ch1 = *src) & 0xfc00) != 0xdc00)
        return ch;

    src++;
    return ((ch & 0x7ff) << 10) + (ch1 & 0x3ff) + 0x10000;
}

inline exlib::wchar32 _getchar_s(const exlib::wchar*& src, const exlib::wchar* end)
{
    exlib::wchar32 ch;

    if (((ch = _swap(*src++)) & 0xf800) != 0xd800)
        return ch;

    exlib::wchar32 ch1;
    if (src >= end || ((ch1 = _swap(*src)) & 0xfc00) != 0xdc00)
        return ch;

    src++;
    return ((ch & 0x7ff) << 10) + (ch1 & 0x3ff) + 0x10000;
}

inline ssize_t _putchar(exlib::wchar32 ch, exlib::wchar*& dst, const exlib::wchar* end)
{
    if (!dst)
        return ch >= 0x10000 ? 2 : 1;

    if (ch >= 0x10000) {
        ch -= 0x10000;
        *dst++ = (exlib::wchar)((ch >> 10) | 0xd800);
        if (dst == end)
            return 1;

        *dst++ = (exlib::wchar)((ch & 0x3ff) | 0xdc00);
        return 2;
    }

    *dst++ = (exlib::wchar)ch;
    return 1;
}

inline ssize_t _putchar_s(exlib::wchar32 ch, exlib::wchar*& dst, const exlib::wchar* end)
{
    if (!dst)
        return ch >= 0x10000 ? 2 : 1;

    if (ch >= 0x10000) {
        ch -= 0x10000;
        *dst++ = _swap((exlib::wchar)((ch >> 10) | 0xd800));
        if (dst == end)
            return 1;

        *dst++ = _swap((exlib::wchar)((ch & 0x3ff) | 0xdc00));
        return 2;
    }

    *dst++ = _swap((exlib::wchar)ch);
    return 1;
}

inline exlib::wchar32 _getchar(const exlib::wchar32*& src, const exlib::wchar32* end)
{
    return *src++;
}

inline exlib::wchar32 _getchar_s(const exlib::wchar32*& src, const exlib::wchar32* end)
{
    return _swap(*src++);
}

inline ssize_t _putchar(exlib::wchar32 ch, exlib::wchar32*& dst, const exlib::wchar32* end)
{
    if (!dst)
        return 1;

    *dst++ = ch;
    return 1;
}

inline ssize_t _putchar_s(exlib::wchar32 ch, exlib::wchar32*& dst, const exlib::wchar32* end)
{
    if (!dst)
        return 1;

    *dst++ = _swap(ch);
    return 1;
}

exlib::wchar32 utf_getchar(const char*& src, const char* end)
{
    if (src >= end)
        return 0;

    return _getchar(src, end);
}

ssize_t utf_putchar(exlib::wchar32 ch, char*& dst, const char* end)
{
    if (dst >= end)
        return 0;

    return _putchar(ch, dst, end);
}

exlib::wchar32 utf_getchar(const exlib::wchar*& src, const exlib::wchar* end)
{
    if (src >= end)
        return 0;

    return _getchar(src, end);
}

ssize_t utf_putchar(exlib::wchar32 ch, exlib::wchar*& dst, const exlib::wchar* end)
{
    if (dst >= end)
        return 0;

    return _putchar(ch, dst, end);
}

exlib::wchar32 utf_getchar(const exlib::wchar32*& src, const exlib::wchar32* end)
{
    if (src >= end)
        return 0;

    return _getchar(src, end);
}

ssize_t utf_putchar(exlib::wchar32 ch, exlib::wchar32*& dst, const exlib::wchar32* end)
{
    if (dst >= end)
        return 0;

    return _putchar(ch, dst, end);
}

template <typename T1, typename T2>
inline ssize_t _test(const T1* src, ssize_t srclen, T2* dst)
{
    ssize_t count = 0;
    const T1* src_end = src + srclen;

    while (src < src_end)
        count += _putchar(_getchar(src, src_end), dst, dst);

    return count;
}

template <typename T1, typename T2>
inline ssize_t _convert(const T1* src, ssize_t srclen, T2* dst, ssize_t dstlen)
{
    ssize_t count = 0;
    const T1* src_end = src + srclen;
    const T2* dst_end = dst + dstlen;

    while (src < src_end && dst < dst_end) {
        exlib::wchar32 ch = *src;

        if (ch < 0x80) {
            src++;
            *dst++ = ch;
            count++;
        } else
            count += _putchar(_getchar(src, src_end), dst, dst_end);
    }

    return count;
}

ssize_t utf_convert(const char* src, ssize_t srclen, exlib::wchar* dst, ssize_t dstlen)
{
    return dst ? _convert(src, srclen, dst, dstlen) : _test(src, srclen, (exlib::wchar*)NULL);
}

ssize_t utf_convert(const exlib::wchar* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    return dst ? _convert(src, srclen, dst, dstlen) : _test(src, srclen, (char*)NULL);
}

ssize_t utf_convert(const char* src, ssize_t srclen, exlib::wchar32* dst, ssize_t dstlen)
{
    return dst ? _convert(src, srclen, dst, dstlen) : _test(src, srclen, (exlib::wchar32*)NULL);
}

ssize_t utf_convert(const exlib::wchar32* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    return dst ? _convert(src, srclen, dst, dstlen) : _test(src, srclen, (char*)NULL);
}

template <typename T1, typename T2>
inline ssize_t _test_s(const T1* src, ssize_t srclen, T2* dst)
{
    ssize_t count = 0;
    const T1* src_end = src + srclen;

    while (src < src_end)
        count += _putchar_s(_getchar_s(src, src_end), dst, dst);

    return count;
}

template <typename T1, typename T2>
inline ssize_t _convert_s(const T1* src, ssize_t srclen, T2* dst, ssize_t dstlen)
{
    ssize_t count = 0;
    const T1* src_end = src + srclen;
    const T2* dst_end = dst + dstlen;

    while (src < src_end && dst < dst_end) {
        exlib::wchar32 ch = _swap(*src);

        if (ch < 0x80) {
            src++;
            *dst++ = _swap((T2)ch);
            count++;
        } else
            count += _putchar_s(_getchar_s(src, src_end), dst, dst_end);
    }

    return count;
}

ssize_t utf_convert_s(const char* src, ssize_t srclen, exlib::wchar* dst, ssize_t dstlen)
{
    return dst ? _convert_s(src, srclen, dst, dstlen) : _test_s(src, srclen, (exlib::wchar*)NULL);
}

ssize_t utf_convert_s(const exlib::wchar* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    return dst ? _convert_s(src, srclen, dst, dstlen) : _test_s(src, srclen, (char*)NULL);
}

ssize_t utf_convert_s(const char* src, ssize_t srclen, exlib::wchar32* dst, ssize_t dstlen)
{
    return dst ? _convert_s(src, srclen, dst, dstlen) : _test_s(src, srclen, (exlib::wchar32*)NULL);
}

ssize_t utf_convert_s(const exlib::wchar32* src, ssize_t srclen, char* dst, ssize_t dstlen)
{
    return dst ? _convert_s(src, srclen, dst, dstlen) : _test_s(src, srclen, (char*)NULL);
}
}
