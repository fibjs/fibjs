/* 
 * Int64.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: lion
 */

#include "object.h"
#include "Int64.h"

namespace fibjs {

result_t Int64_base::_new(int64_t num, obj_ptr<Int64_base>& retVal,
    v8::Local<v8::Object>)
{
    retVal = new Int64(num);
    return 0;
}

result_t Int64_base::_new(Int64_base* num, obj_ptr<Int64_base>& retVal,
    v8::Local<v8::Object>)
{
    retVal = new Int64(((Int64*)num)->m_num);
    return 0;
}

result_t Int64_base::_new(int64_t hi, int64_t lo,
    obj_ptr<Int64_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new Int64(hi, lo);
    return 0;
}

result_t Int64_base::_new(exlib::string num, int32_t base,
    obj_ptr<Int64_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Int64> i = new Int64();
    result_t hr = i->fromString(num, base);
    if (hr < 0)
        return hr;

    retVal = i;
    return 0;
}

result_t Int64::get_hi(int64_t& retVal)
{
    retVal = (m_num >> 32);
    return 0;
}

result_t Int64::set_hi(int64_t newVal)
{
    m_num = (m_num & 0xffffffff) | ((newVal & 0xffffffff) << 32);
    return 0;
}

result_t Int64::get_lo(int64_t& retVal)
{
    retVal = (m_num & 0xffffffff);
    return 0;
}

result_t Int64::set_lo(int64_t newVal)
{
    m_num = (m_num & 0xffffffff00000000) | (newVal & 0xffffffff);
    return 0;
}

result_t Int64::compare(Int64_base* num, int32_t& retVal)
{
    int64_t num1 = ((Int64*)num)->m_num;
    if (m_num > num1)
        retVal = 1;
    else if (m_num < num1)
        retVal = -1;
    else
        retVal = 0;

    return 0;
}

result_t Int64::shiftLeft(int32_t bits, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num << bits);
    return 0;
}

result_t Int64::shiftRight(int32_t bits, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num >> bits);
    return 0;
}

result_t Int64::_and(Int64_base* num, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num & ((Int64*)num)->m_num);
    return 0;
}

result_t Int64::_or(Int64_base* num, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num | ((Int64*)num)->m_num);
    return 0;
}

result_t Int64::_xor(Int64_base* num, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num ^ ((Int64*)num)->m_num);
    return 0;
}

result_t Int64::add(Int64_base* num, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num + ((Int64*)num)->m_num);
    return 0;
}

result_t Int64::sub(Int64_base* num, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num - ((Int64*)num)->m_num);
    return 0;
}

result_t Int64::multi(Int64_base* num, obj_ptr<Int64_base>& retVal)
{
    retVal = new Int64(m_num * ((Int64*)num)->m_num);
    return 0;
}

result_t Int64::div(Int64_base* num, obj_ptr<Int64_base>& retVal)
{
    int64_t num1 = ((Int64*)num)->m_num;
    if (num1 == 0) {
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }
    retVal = new Int64(m_num / num1);
    return 0;
}

result_t Int64::fromString(exlib::string numStr, int32_t base)
{
    static const char __base32_map[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 2x  !"#$%&'()*+,-./   */
        14, 11, 26, 27, 28, 29, 30, 31, -1, 6, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 6x `abcdefghijklmno   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    static const char __base64_map[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, /* 2x  !"#$%&'()*+,-./   */
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 6x `abcdefghijklmno   */
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    const char* c_str = numStr.c_str();

    while (*c_str && qisspace(*c_str))
        c_str++;

    bool bNeg = false;

    if (base == 10) {
        if (c_str[0] == '-') {
            bNeg = true;
            c_str++;
        }
        if (c_str[0] == '0' && c_str[1] == 'x') {
            base = 16;
            c_str += 2;
        }
    }

    const char* __base;
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

    while (0 != (ch = *c_str++)) {
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

result_t Int64::equals(object_base* expected, bool& retVal)
{
    obj_ptr<Int64_base> v = Int64_base::getInstance(expected);
    if (!v) {
        retVal = false;
        return 0;
    }

    retVal = m_num == ((Int64*)(Int64_base*)v)->m_num;
    return 0;
}

result_t Int64::toString(int32_t base, exlib::string& retVal)
{
    static char __base16_map[] = "0123456789abcdef";
    static char __base32_map[] = "abcdefghijklmnopqrstuvwxyz234567";
    static char __base64_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char* __base;
    char buf[64] = { 0 };
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
        val = (uint64_t)-m_num;
    else
        val = (uint64_t)m_num;

    while (pos > 0 && val > 0) {
        buf[--pos] = __base[val % base];
        val /= base;
    }

    if (base == 16) {
        if (64 == pos)
            buf[--pos] = '0';

        buf[--pos] = 'x';
        buf[--pos] = '0';
    } else if (base == 10 && m_num < 0)
        buf[--pos] = '-';

    if (64 - pos > 0)
        retVal.assign(buf + pos, 64 - pos);
    else
        retVal.assign(__base, 1);

    return 0;
}

result_t Int64::toNumber(double& retVal)
{
    if (m_num < -9007199254740992ll || m_num > 9007199254740992ll)
        retVal = INFINITY;
    else
        retVal = (double)m_num;

    return 0;
}

result_t Int64::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    exlib::string str;

    toString(16, str);
    retVal = holder()->NewString(str);

    return 0;
}

result_t Int64::unbind(obj_ptr<object_base>& retVal)
{
    retVal = new Int64(m_num);
    return 0;
}
}
