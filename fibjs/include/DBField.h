/*
 * DBField.h
 *
 *  Created on: Jul 28, 2012
 *      Author: lion
 */

#include "utils.h"

#ifndef DBFIELD_H_
#define DBFIELD_H_

namespace fibjs
{

class DBField: public obj_base
{
public:
    DBField(int32_t sz)
    {
        m_fields.resize(sz);
    }

public:
    void setField(int32_t i, std::string &s)
    {
        m_fields[i] = s;
    }

    uint32_t index(const char *name)
    {
        uint32_t i;

        for (i = 0; i < (uint32_t) m_fields.size(); i++)
            if (!qstricmp(name, m_fields[i].c_str()))
                return i;

        return -1;
    }

    std::string &name(int32_t i)
    {
        return m_fields[i];
    }

    void names(v8::Local<v8::Array> &retVal)
    {
        int32_t i;

        retVal = v8::Array::New(Isolate::now().isolate);
        for (i = 0; i < (int32_t)m_fields.size(); i++)
            retVal->Set(i, GetReturnValue(m_fields[i]));
    }


private:
    std::vector<std::string> m_fields;
};

}

#endif /* DBFIELD_H_ */
