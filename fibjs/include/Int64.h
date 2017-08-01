/*
 * Int64.h
 *
 *  Created on: Aug 26, 2014
 *      Author: lion
 */

#include "ifs/Int64.h"

#ifndef __INT64_H_
#define __INT64_H_

namespace fibjs {

class Int64 : public Int64_base {
public:
    Int64(const int64_t num = 0)
        : m_num(num)
    {
    }

    Int64(int64_t hi, int64_t lo)
    {
        m_num = ((hi & 0xffffffff) << 32) | (lo & 0xffffffff);
    }

public:
    // object_base
    virtual result_t equals(object_base* expected, bool& retVal);
    virtual result_t valueOf(v8::Local<v8::Value>& retVal);
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);
    virtual result_t unbind(obj_ptr<object_base>& retVal);

public:
    // Int64_base
    virtual result_t get_hi(int64_t& retVal);
    virtual result_t set_hi(int64_t newVal);
    virtual result_t get_lo(int64_t& retVal);
    virtual result_t set_lo(int64_t newVal);
    virtual result_t compare(Int64_base* num, int32_t& retVal);
    virtual result_t shiftLeft(int32_t bits, obj_ptr<Int64_base>& retVal);
    virtual result_t shiftRight(int32_t bits, obj_ptr<Int64_base>& retVal);
    virtual result_t _and(Int64_base* num, obj_ptr<Int64_base>& retVal);
    virtual result_t _or(Int64_base* num, obj_ptr<Int64_base>& retVal);
    virtual result_t _xor(Int64_base* num, obj_ptr<Int64_base>& retVal);
    virtual result_t add(Int64_base* num, obj_ptr<Int64_base>& retVal);
    virtual result_t sub(Int64_base* num, obj_ptr<Int64_base>& retVal);
    virtual result_t multi(Int64_base* num, obj_ptr<Int64_base>& retVal);
    virtual result_t div(Int64_base* num, obj_ptr<Int64_base>& retVal);
    virtual result_t toNumber(double& retVal);
    virtual result_t toString(int32_t base, exlib::string& retVal);

public:
    result_t fromString(exlib::string numStr, int32_t base);

public:
    int64_t m_num;
};

} /* namespace fibjs */
#endif /* __INT64_H_ */
