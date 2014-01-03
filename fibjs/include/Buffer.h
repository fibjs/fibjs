#include <string>
#include <list>

#include "ifs/Buffer.h"

#ifndef _fj_BUFFER_H
#define _fj_BUFFER_H

namespace fibjs
{

class Buffer: public Buffer_base
{
public:
    Buffer()
    {
    }

    Buffer(std::string strData) :
        m_data(strData)
    {
        extMemory((int)strData.length());
    }

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    virtual result_t _indexed_getter(uint32_t index, int32_t &retVal);
    virtual result_t _indexed_setter(uint32_t index, int32_t newVal);
    virtual result_t get_length(int32_t &retVal);
    virtual result_t resize(int32_t sz);
    virtual result_t write(const char *str);
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base> &retVal);
    virtual result_t hex(std::string &retVal);
    virtual result_t base64(std::string &retVal);

    virtual result_t toJSON(const char *key, v8::Handle<v8::Value> &retVal)
    {
        retVal = v8::Object::New(isolate);
        return 0;
    }

private:
    std::string m_data;
};

}

#endif // BUFFER_H
