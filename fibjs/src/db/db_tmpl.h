/*
 * db_tmpl.h
 *
 *  Created on: May 19, 2021
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "ifs/db.h"
#include "db_format.h"

namespace fibjs {

template <typename T>
inline result_t db_trans(T* pThis, exlib::string point, v8::Local<v8::Function> func, bool& retVal)
{
    v8::Local<v8::Value> v = pThis->wrap();
    result_t hr = 0;
    retVal = false;

    hr = pThis->ac_begin(point);
    if (hr < 0)
        return hr;

    v8::Local<v8::Value> result = func->Call(func->GetCreationContextChecked(), pThis->wrap(), 1, &v).FromMaybe(v8::Local<v8::Value>());

    if (result.IsEmpty()) {
        pThis->ac_rollback(point);
        return CALL_E_JAVASCRIPT;
    }

    if (result->IsFalse())
        return pThis->ac_rollback(point);
    else {
        retVal = true;
        return pThis->ac_commit(point);
    }
}

template <class base, class impl>
class db_tmpl : public base {
public:
    db_tmpl()
        : m_conn(NULL)
    {
    }

public:
    result_t format(exlib::string method, v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        return db_format<impl>::format(method, opts, retVal);
    }

    result_t format(exlib::string sql, OptArgs args, exlib::string& retVal)
    {
        return db_format<impl>::format(sql.c_str(), args, retVal);
    }

    result_t use(exlib::string dbName, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;
        exlib::string s("USE ", 4);
        s.append(dbName);
        return execute(s, retVal, ac);
    }

    result_t begin(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("BEGIN", retVal, ac);

        exlib::string str("SAVEPOINT " + point);
        return execute(str, retVal, ac);
    }

    result_t commit(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("COMMIT", retVal, ac);

        exlib::string str("RELEASE SAVEPOINT " + point);
        return execute(str, retVal, ac);
    }

    result_t rollback(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("ROLLBACK", retVal, ac);

        exlib::string str("ROLLBACK TO " + point);
        return execute(str, retVal, ac);
    }

    result_t trans(v8::Local<v8::Function> func, bool& retVal)
    {
        return trans("", func, retVal);
    }

    result_t trans(exlib::string point, v8::Local<v8::Function> func, bool& retVal)
    {
        return db_trans(this, point, func, retVal);
    }

    result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    result_t execute(exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal,
        AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync()) {
            exlib::string str;
            result_t hr = format(sql, args, str);
            if (hr < 0)
                return hr;

            ac->m_ctx.resize(1);
            ac->m_ctx[0] = str;

            return CHECK_ERROR(CALL_E_LONGSYNC);
        }

        exlib::string str = ac->m_ctx[0].string();
        return execute(str, retVal, ac);
    }

    typedef result_t (*formater)(v8::Local<v8::Object> opts, exlib::string& retVal);

    result_t execute(formater fmt, v8::Local<v8::Object> opts,
        obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync()) {
            exlib::string str;
            result_t hr = fmt(opts, str);
            if (hr < 0)
                return hr;

            ac->m_ctx.resize(1);
            ac->m_ctx[0] = str;

            return CHECK_ERROR(CALL_E_LONGSYNC);
        }

        exlib::string str = ac->m_ctx[0].string();
        return execute(str, retVal, ac);
    }

    result_t createTable(v8::Local<v8::Object> opts, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        return execute(db_format<impl>::createTable, opts, _retVal, ac);
    }

    result_t dropTable(v8::Local<v8::Object> opts, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        return execute(db_format<impl>::dropTable, opts, _retVal, ac);
    }

    result_t createIndex(v8::Local<v8::Object> opts, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        return execute(db_format<impl>::createIndex, opts, _retVal, ac);
    }

    result_t dropIndex(v8::Local<v8::Object> opts, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        return execute(db_format<impl>::dropIndex, opts, _retVal, ac);
    }

    result_t insert(v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        result_t hr = execute(db_format<impl>::insert, opts, _retVal, ac);
        if (hr < 0)
            return hr;

        retVal = _retVal->m_values[1].m_val.dblVal();

        return 0;
    }

    result_t find(v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal,
        AsyncEvent* ac)
    {
        return execute(db_format<impl>::find, opts, retVal, ac);
    }

    result_t count(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        result_t hr = execute(db_format<impl>::count, opts, _retVal, ac);
        if (hr < 0)
            return hr;

        Variant v;
        _retVal->_indexed_getter(0, v);
        retVal = ((NObject*)v.object())->m_values[0].m_val.intVal();

        return 0;
    }

    result_t update(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        result_t hr = execute(db_format<impl>::update, opts, _retVal, ac);
        if (hr < 0)
            return hr;

        retVal = _retVal->m_values[0].m_val.intVal();

        return 0;
    }

    result_t remove(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        result_t hr = execute(db_format<impl>::remove, opts, _retVal, ac);
        if (hr < 0)
            return hr;

        retVal = _retVal->m_values[0].m_val.intVal();

        return 0;
    }

public:
    static exlib::string escape_string(exlib::string v)
    {
        const char* str = v.c_str();
        int32_t sz = (int32_t)v.length();
        int32_t len, l;
        const char* src;
        char* bstr;
        char ch;
        exlib::string retVal;

        for (len = 0, src = str, l = sz; l > 0; len++, l--) {
            ch = (unsigned char)*src++;

            if (ch == '\'')
                len++;
        }

        retVal.resize(len + 2);

        bstr = retVal.data();
        *bstr++ = '\'';

        for (src = str, l = sz; l > 0; l--) {
            ch = (unsigned char)*src++;

            if (ch == '\'')
                *bstr++ = '\'';
            *bstr++ = ch;
        }

        *bstr++ = '\'';

        return retVal;
    }

    static exlib::string escape_binary(Buffer* bin)
    {
        exlib::string retVal;
        exlib::string s;

        bin->hex(s);

        retVal.append("x\'", 2);
        retVal.append(s);
        retVal += '\'';

        return retVal;
    }

    static exlib::string escape_date(v8::Local<v8::Value>& v)
    {
        exlib::string retVal;
        exlib::string s;

        retVal.append(1, '\'');

        date_t d = v;
        d.sqlString(s);
        retVal.append(s);

        retVal.append(1, '\'');

        return retVal;
    }

    static exlib::string escape_field(const char* str, int32_t sz,
        char quote_left = '`', char quote_right = '`')
    {
        exlib::string retVal;

        retVal += quote_left;
        while (sz--) {
            char ch = *str++;
            if (ch == quote_right) {
                retVal += quote_right;
                retVal += quote_right;
            } else if (ch == '.') {
                retVal += quote_right;
                retVal += '.';
                retVal += quote_left;
            } else
                retVal += ch;
        }
        retVal += quote_right;

        return retVal;
    }

public:
    struct DataType {
        const char* FLOAT;
        const char* DOUBLE;
        const char* DATETIME;
        const char* TEXT;
        const char* BLOB;
        const char* LONGBLOB;
    };

    static const DataType& data_type()
    {
        static DataType _data_type = {
            "FLOAT",
            "DOUBLE",
            "DATETIME",
            "TEXT",
            "BLOB",
            "LONGBLOB"
        };

        return _data_type;
    }

public:
    void* m_conn;
};
}