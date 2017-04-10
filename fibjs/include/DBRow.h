/*
 * DBRow.h
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#ifndef DBROW_H_
#define DBROW_H_

#include "ifs/DBRow.h"
#include "DBField.h"

namespace fibjs {

class DBRow : public DBRow_base {
public:
    DBRow(DBField* fields, int32_t sz)
    {
        m_fields = fields;
        m_cols.resize(sz);
    }

public:
    // object_base
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    // DBRow_base
    virtual result_t _indexed_getter(uint32_t index, v8::Local<v8::Value>& retVal);
    virtual result_t _named_getter(const char* property, v8::Local<v8::Value>& retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);

public:
    void setValue(int32_t i, Variant& v)
    {
        if (i >= 0 && i < (int32_t)m_cols.size())
            m_cols[i] = v;
    }

private:
    std::vector<VariantEx> m_cols;
    obj_ptr<DBField> m_fields;
};

} /* namespace fibjs */
#endif /* DBROW_H_ */
