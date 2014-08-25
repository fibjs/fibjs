/*
 * Integer64.h
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "ifs/Integer64.h"

#ifndef INTEGER64_H_
#define INTEGER64_H_

static char __base64_map[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!*";

namespace fibjs
{

class Integer64: public Integer64_base
{
public:
    Integer64(const int64_t num) :
        m_num(num)
    {
    }

    Integer64(int64_t hi, int64_t lo)
    {
        m_num = ((hi & 0xffffffff) << 32) | (lo & 0xffffffff);
    }

public:
    // Integer64_base
    virtual result_t get_hi(int64_t &retVal)
    {
        retVal = (m_num >> 32);
        return 0;
    }

    virtual result_t set_hi(int64_t newVal)
    {
        m_num = (m_num & 0xffffffff) | ((newVal & 0xffffffff) << 32);
        return 0;
    }

    virtual result_t get_lo(int64_t &retVal)
    {
        retVal = (m_num & 0xffffffff);
        return 0;
    }

    virtual result_t set_lo(int64_t newVal)
    {
        m_num = (m_num & 0xffffffff00000000) | (newVal & 0xffffffff);
        return 0;
    }

    virtual result_t fromHex(const char *hexStr)
    {
        int pos = 0;
        char cur = 0;
        uint64_t newVal = 0;

        while (pos < 16 && hexStr[pos])
        {
            cur = hexStr[pos];
            if (cur >= 48 && cur <= 57)
            {
                cur -= 48;
            }
            else if (cur >= 65 && cur <= 70)
            {
                cur -= 55;
            }
            else if (cur >= 97 && cur <= 102)
            {
                cur -= 87;
            }
            else
            {
                break;
            }
            newVal = (newVal << 4) + cur;
            pos++;
        }
        m_num = (int64_t)newVal;

        return 0;
    }

    virtual result_t fromString(const char *numStr, int32_t base)
    {
        int pos = 0;
        char cur = 0;
        uint64_t newVal = 0;

        while (pos < 64 && numStr[pos])
        {
            cur = numStr[pos];
            if (cur >= 48 && cur <= 57)
            {
                cur -= 48;
            }
            else if (cur >= 65 && cur <= 70)
            {
                cur -= 55;
            }
            else if (cur >= 97 && cur <= 102)
            {
                cur -= 61;
            }
            else if (cur == '!')
            {
                cur = 62;
            }
            else if (cur == '*')
            {
                cur = 63;
            }
            else
            {
                break;
            }
            newVal = (newVal * base) + cur;
            pos++;
        }
        m_num = (int64_t)newVal;

        return 0;
    }

    virtual result_t toString(int32_t base, std::string &retVal)
    {
        char buf[64] = {0};
        int pos = 64;
        uint64_t val = (uint64_t)m_num;

        if (base < 2 || base > 64)
        {
            return 0;
        }

        while (pos > 0 && val > 0)
        {
            buf[--pos] = __base64_map[val % base];
            val /= base;
        }
        retVal.assign(buf + pos, 64 - pos);

        return 0;
    }

    virtual result_t valueOf(double &retVal)
    {
        retVal = (double) m_num;
        return 0;
    }

    virtual result_t toJSON(const char *key, v8::Local<v8::Value> &retVal)
    {
        std::string str;

        toString(64, str);
        retVal = v8::String::NewFromUtf8(isolate, str.c_str(),
                                         v8::String::kNormalString, (int) str.length());

        return 0;
    }

public:
    int64_t m_num;
};

} /* namespace fibjs */
#endif /* INTEGER64_H_ */
