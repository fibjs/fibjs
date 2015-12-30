/*
 * Int64.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: lion
 */

#include "Int64.h"

namespace fibjs
{

result_t Int64_base::_new(int64_t num, obj_ptr<Int64_base> &retVal,
                          v8::Local<v8::Object>)
{
    retVal = new Int64(num);
    return 0;
}

result_t Int64_base::_new(Int64_base *num, obj_ptr<Int64_base> &retVal,
                          v8::Local<v8::Object>)
{
    retVal = new Int64(((Int64 *)num)->m_num);
    return 0;
}

result_t Int64_base::_new(int64_t hi, int64_t lo,
                          obj_ptr<Int64_base> &retVal,
                          v8::Local<v8::Object> This)
{
    retVal = new Int64(hi, lo);
    return 0;
}

result_t Int64_base::_new(const char *num, int32_t base,
                          obj_ptr<Int64_base> &retVal,
                          v8::Local<v8::Object> This)
{
    obj_ptr<Int64> i = new Int64();
    result_t hr = i->fromString(num, base);
    if (hr < 0)
        return hr;

    retVal = i;
    return 0;
}

result_t Int64::get_hi(int64_t &retVal)
{
    retVal = (m_num >> 32);
    return 0;
}

result_t Int64::set_hi(int64_t newVal)
{
    m_num = (m_num & 0xffffffff) | ((newVal & 0xffffffff) << 32);
    return 0;
}

result_t Int64::get_lo(int64_t &retVal)
{
    retVal = (m_num & 0xffffffff);
    return 0;
}

result_t Int64::set_lo(int64_t newVal)
{
    m_num = (m_num & 0xffffffff00000000) | (newVal & 0xffffffff);
    return 0;
}

result_t Int64::equal(Int64_base *num, bool &retVal)
{
    retVal = m_num == ((Int64 *)num)->m_num;
    return 0;
}

result_t Int64::compare(Int64_base *num, int32_t &retVal)
{
    int64_t num1 = ((Int64 *)num)->m_num;
    if (m_num > num1)
        retVal = 1;
    else if (m_num < num1)
        retVal = -1;
    else retVal = 0;

    return 0;
}

result_t Int64::shiftLeft(int32_t bits, obj_ptr<Int64_base> &retVal)
{
    retVal = new Int64(m_num << bits);
    return 0;
}

result_t Int64::shiftRight(int32_t bits, obj_ptr<Int64_base> &retVal)
{
    retVal = new Int64(m_num >> bits);
    return 0;
}

result_t Int64::_and(Int64_base *num, obj_ptr<Int64_base> &retVal)
{
    retVal = new Int64(m_num & ((Int64 *)num)->m_num);
    return 0;
}

result_t Int64::_or(Int64_base *num, obj_ptr<Int64_base> &retVal)
{
    retVal = new Int64(m_num | ((Int64 *)num)->m_num);
    return 0;
}

result_t Int64::_xor(Int64_base *num, obj_ptr<Int64_base> &retVal)
{
    retVal = new Int64(m_num ^ ((Int64 *)num)->m_num);
    return 0;
}

result_t Int64::add(Int64_base *num, obj_ptr<Int64_base> &retVal)
{
    retVal = new Int64(m_num + ((Int64 *)num)->m_num);
    return 0;
}

result_t Int64::sub(Int64_base *num, obj_ptr<Int64_base> &retVal)
{
    retVal = new Int64(m_num - ((Int64 *)num)->m_num);
    return 0;
}

result_t Int64::fromString(const char *numStr, int32_t base)
{
    static const char __base32_map[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 2x  !"#$%&'()*+,-./   */
        14, 11, 26, 27, 28, 29, 30, 31, -1, 6, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 6x `abcdefghijklmno   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    static const char __base64_map[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, /* 2x  !"#$%&'()*+,-./   */
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 6x `abcdefghijklmno   */
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    while (*numStr && qisspace(*numStr))
        numStr++;

    bool bNeg = false;

    if (base == 10)
    {
        if (numStr[0] == '-')
        {
            bNeg = true;
            numStr ++;
        }
        if (numStr[0] == '0' && numStr[1] == 'x')
        {
            base = 16;
            numStr += 2;
        }
    }

    const char *__base;
    uint64_t val = 0;
    int32_t ch;

    if (base == 64)
        __base = __base64_map;
    else if (base == 32)
        __base = __base32_map;
    else if (base >= 2 && base <= 16)
        __base = NULL;
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    while (0 != (ch = *numStr ++))
    {
        if (__base)
            ch = (ch > 0x20 && ch < 0x80) ? __base[ch - 0x20] : -1;
        else
            ch = qhex(ch);
        if (ch < 0 || ch >= base)
            return CHECK_ERROR(CALL_E_INVALIDARG);

        val = val * base + ch;
    }

    if (bNeg)
        m_num = -(int64_t)val;
    else
        m_num = (int64_t)val;

    return 0;
}

result_t Int64::toString(int32_t base, std::string &retVal)
{
    static char __base16_map[] = "0123456789abcdef";
    static char __base32_map[] = "abcdefghijklmnopqrstuvwxyz234567";
    static char __base64_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char *__base;
    char buf[64] = {0};
    int32_t pos = 64;
    uint64_t val;

    if (base == 64)
        __base = __base64_map;
    else if (base == 32)
        __base = __base32_map;
    else if (base <= 16 && base >= 2)
        __base = __base16_map;
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (base == 10 && m_num < 0)
        val = (uint64_t) - m_num;
    else
        val = (uint64_t)m_num;

    while (pos > 0 && val > 0)
    {
        buf[--pos] = __base[val % base];
        val /= base;
    }

    if (base == 16)
    {
        if (64 == pos)
            buf[--pos] = '0';

        buf[--pos] = 'x';
        buf[--pos] = '0';
    }
    else if (base == 10 && m_num < 0)
        buf[--pos] = '-';

    if (64 - pos > 0)
        retVal.assign(buf + pos, 64 - pos);
    else
        retVal.assign(__base, 1);

    return 0;
}

result_t Int64::valueOf(v8::Local<v8::Value> &retVal)
{
    double num;

    toNumber(num);
    retVal = v8::Number::New(holder()->m_isolate, num);
    return 0;
}

result_t Int64::toNumber(double &retVal)
{
    if (m_num < -9007199254740992ll || m_num > 9007199254740992ll)
        retVal = INFINITY;
    else
        retVal = (double)m_num;

    return 0;
}

result_t Int64::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    std::string str;

    toString(16, str);
    retVal = v8::String::NewFromUtf8(holder()->m_isolate, str.c_str(),
                                     v8::String::kNormalString, (int32_t) str.length());

    return 0;
}

}
