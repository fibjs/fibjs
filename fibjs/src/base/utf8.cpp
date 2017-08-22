#include "utf8.h"

namespace fibjs {

static const char utf8_length[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x80-0x8f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x90-0x9f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xa0-0xaf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xb0-0xbf */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xc0-0xcf */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xd0-0xdf */
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 0xe0-0xef */
    3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 0, 0 /* 0xf0-0xff */
};

static const unsigned char utf8_mask[6] = { 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

exlib::wchar32 utf_getchar(const char*& src, const char* end)
{
    if (src >= end)
        return 0;

    unsigned char ch = (unsigned char)*src++;
    if (ch < 0x80)
        return ch;

    int32_t len = utf8_length[ch - 0x80];
    exlib::wchar32 res = ch & utf8_mask[len];

    switch (len) {
    case 5:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return '?';

        res = (res << 6) | ch;
        src++;
    case 4:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return '?';

        res = (res << 6) | ch;
        src++;
    case 3:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return '?';

        res = (res << 6) | ch;
        src++;
    case 2:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return '?';

        res = (res << 6) | ch;
        src++;
    case 1:
        if (src >= end || (ch = (unsigned char)*src ^ 0x80) >= 0x40)
            return '?';

        res = (res << 6) | ch;
        src++;
    }

    return res;
}

int32_t utf_putchar(exlib::wchar32 ch, char*& dst, const char* end)
{
    if (dst && dst >= end)
        return 0;

    int32_t count;
    int32_t i;

    if (ch < 0)
        ch = '?';

    if (ch < 0x80) {
        if (dst)
            *dst++ = (char)ch;
        return 1;
    } else {
        exlib::wchar ch1;

        if (ch < 0x800) {
            count = 1;
            ch1 = 0xc0;
        } else if (ch < 0x10000) {
            count = 2;
            ch1 = 0xe0;
        } else if (ch < 0x200000) {
            count = 3;
            ch1 = 0xf0;
        } else if (ch < 0x4000000) {
            count = 4;
            ch1 = 0xf8;
        } else {
            count = 5;
            ch1 = 0xfc;
        }

        if (dst) {
            if (dst >= end - count) {
                *dst++ = '?';
                return 1;
            }

            for (i = count; i > 0; i--) {
                dst[i] = (char)((ch & 0x3f) | 0x80);
                ch >>= 6;
            }
            *dst = (char)(ch | ch1);
            dst += count + 1;
        }

        return count + 1;
    }

    return 0;
}

exlib::wchar32 utf_getchar(const exlib::wchar*& src, const exlib::wchar* end)
{
    exlib::wchar32 ch;

    if (src >= end)
        return 0;

    if (((ch = *src++) & 0xf800) != 0xd800)
        return ch;

    exlib::wchar ch1;
    if (src >= end || ((ch1 = *src) & 0xfc00) != 0xdc00)
        return '?';

    src++;
    return ((ch & 0x7ff) << 10) + (ch1 & 0x3ff);
}

int32_t utf_putchar(exlib::wchar32 ch, exlib::wchar*& dst, const exlib::wchar* end)
{
    if (!dst)
        return ch >= 0x10000 ? 2 : 1;

    if (dst >= end)
        return 0;

    if (ch < 0)
        ch = '?';

    if (ch >= 0x10000) {
        if (dst >= end - 1) {
            *dst = '?';
            return 1;
        }

        ch -= 0x10000;
        *dst++ = (exlib::wchar)((ch >> 10) | 0xd800);
        *dst++ = (exlib::wchar)((ch & 0x3ff) | 0xdc00);

        return 2;
    }

    *dst++ = (exlib::wchar)ch;
    return 1;
}

exlib::wchar32 utf_getchar(const exlib::wchar32*& src, const exlib::wchar32* end)
{
    if (src >= end)
        return 0;
    return *src++;
}

int32_t utf_putchar(exlib::wchar32 ch, exlib::wchar32*& dst, const exlib::wchar32* end)
{
    if (!dst)
        return 1;

    if (dst >= end)
        return 0;

    *dst++ = ch;
    return 1;
}
}
