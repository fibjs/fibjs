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

    pThis->leave();
    v8::Local<v8::Value> result = func->Call(pThis->wrap(), 1, &v);
    pThis->enter();

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

template <class base, bool mysql, bool mssql>
class db_tmpl : public base {
public:
    db_tmpl()
        : m_conn(NULL)
    {
    }

public:
    result_t format(exlib::string method, v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        return db_format<mysql, mssql>::format(method, opts, retVal);
    }

    result_t format(exlib::string sql, OptArgs args, exlib::string& retVal)
    {
        return db_format<mysql, mssql>::format(sql.c_str(), args, retVal);
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

        if (point.empty()) {
            if (mssql)
                return execute("BEGIN TRANSACTION", retVal, ac);
            else
                return execute("BEGIN", retVal, ac);
        } else {
            exlib::string str((mssql ? "SAVE TRANSACTION " : "SAVEPOINT ") + point);
            return execute(str, retVal, ac);
        }
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
        else {
            if (mssql)
                return 0;
            exlib::string str((mssql ? "COMMIT TRANSACTION " : "RELEASE SAVEPOINT ") + point);
            return execute(str, retVal, ac);
        }
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
        else {
            exlib::string str((mssql ? "ROLLBACK TRANSACTION " : "ROLLBACK TO ") + point);
            return execute(str, retVal, ac);
        }
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

    result_t execute(exlib::string method, v8::Local<v8::Object> opts,
        obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync()) {
            exlib::string str;
            result_t hr = format(method, opts, str);
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
        return execute("createTable", opts, _retVal, ac);
    }

    result_t dropTable(v8::Local<v8::Object> opts, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        return execute("dropTable", opts, _retVal, ac);
    }

    result_t createIndex(v8::Local<v8::Object> opts, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        return execute("createIndex", opts, _retVal, ac);
    }

    result_t dropIndex(v8::Local<v8::Object> opts, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        return execute("dropIndex", opts, _retVal, ac);
    }

    result_t insert(v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        result_t hr = execute("insert", opts, _retVal, ac);
        if (hr < 0)
            return hr;

        retVal = _retVal->m_values[1].m_val.dblVal();

        return 0;
    }

    result_t find(v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal,
        AsyncEvent* ac)
    {
        return execute("find", opts, retVal, ac);
    }

    result_t count(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        result_t hr = execute("count", opts, _retVal, ac);
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
        result_t hr = execute("update", opts, _retVal, ac);
        if (hr < 0)
            return hr;

        retVal = _retVal->m_values[0].m_val.intVal();

        return 0;
    }

    result_t remove(v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
    {
        obj_ptr<NArray> _retVal;
        result_t hr = execute("remove", opts, _retVal, ac);
        if (hr < 0)
            return hr;

        retVal = _retVal->m_values[0].m_val.intVal();

        return 0;
    }

public:
    void* m_conn;
};
}