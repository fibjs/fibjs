/*
 * DBResult.h
 *
 *  Created on: Jul 25, 2012
 *      Author: lion
 */

#include "ifs/DBResult.h"
#include "List.h"
#include "DBRow.h"

#ifndef DBRESULT_H_
#define DBRESULT_H_

namespace fibjs
{

class DBResult: public DBResult_base
{
public:
    DBResult(int32_t sz, int64_t affected = 0, int64_t insertId = 0) :
        m_size(sz), m_affected(affected), m_insertId(insertId)
    {
        m_array = new List();

        if (sz)
            m_fields = new DBField(sz);
    }

public:
    // object_base
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value> &retVal);

public:
    // List_base
    virtual result_t _indexed_getter(uint32_t index, Variant &retVal);
    virtual result_t _indexed_setter(uint32_t index, Variant newVal);
    virtual result_t freeze();
    virtual result_t get_length(int32_t &retVal);
    virtual result_t resize(int32_t sz);
    virtual result_t push(Variant v);
    virtual result_t push(const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual result_t pushArray(v8::Local<v8::Array> data);
    virtual result_t pop(Variant &retVal);
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<List_base> &retVal);
    virtual result_t concat(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<List_base> &retVal);
    virtual result_t every(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, bool &retVal);
    virtual result_t some(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, bool &retVal);
    virtual result_t filter(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal);
    virtual result_t forEach(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp);
    virtual result_t map(v8::Local<v8::Function> func, v8::Local<v8::Object> thisp, obj_ptr<List_base> &retVal);
    virtual result_t sort(v8::Local<v8::Function> func, obj_ptr<List_base>& retVal);
    virtual result_t sort(obj_ptr<List_base>& retVal);
    virtual result_t toArray(v8::Local<v8::Array> &retVal);

public:
    // DBResult_base
    virtual result_t get_insertId(int64_t &retVal);
    virtual result_t get_affected(int64_t &retVal);
    virtual result_t get_fields(v8::Local<v8::Array> &retVal);

public:
    void setField(int32_t i, exlib::string &s)
    {
        m_fields->setField(i, s);
    }

    void beginRow()
    {
        m_nowRow = new DBRow(m_fields, m_size);
    }

    void endRow()
    {
        m_array->append(m_nowRow);
        m_nowRow.Release();
    }

    void rowValue(int32_t i, Variant &v)
    {
        m_nowRow->setValue(i, v);
        extMemory((int32_t)v.size());
    }

private:
    obj_ptr<List> m_array;
    int32_t m_size;
    int64_t m_affected;
    int64_t m_insertId;
    obj_ptr<DBField> m_fields;
    obj_ptr<DBRow> m_nowRow;
};

} /* namespace fibjs */
#endif /* DBRESULT_H_ */
