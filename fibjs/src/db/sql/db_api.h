/*
 * SQLite.cpp
 *
 *  Created on: Oct 27, 2019
 *      Author: lion
 */

#include "object.h"

namespace fibjs {

result_t db_format(exlib::string table, exlib::string method, v8::Local<v8::Object> opts, bool mysql, bool mssql,
    exlib::string& retVal);

template <typename T>
result_t db_execute(T* pThis, exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        exlib::string str;
        result_t hr = pThis->format(sql, args, str);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = str;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string str = ac->m_ctx[0].string();
    return pThis->execute(str.c_str(), (int32_t)str.length(), retVal);
}

template <typename T>
result_t db_execute(T* pThis, exlib::string table, exlib::string method, v8::Local<v8::Object> opts,
    obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        exlib::string str;
        result_t hr = pThis->format(table, method, opts, str);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = str;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string str = ac->m_ctx[0].string();
    return pThis->execute(str.c_str(), (int32_t)str.length(), retVal);
}

template <typename T>
result_t db_insert(T* pThis, exlib::string table, v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    return db_execute(pThis, table, "insert", opts, _retVal, ac);
}

template <typename T>
result_t db_find(T* pThis, exlib::string table, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal,
    AsyncEvent* ac)
{
    return db_execute(pThis, table, "find", opts, retVal, ac);
}

template <typename T>
result_t db_count(T* pThis, exlib::string table, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, table, "count", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    Variant v;
    _retVal->_indexed_getter(0, v);
    retVal = ((NObject*)v.object())->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_update(T* pThis, exlib::string table, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, table, "update", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_remove(T* pThis, exlib::string table, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, table, "remove", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_begin(T* pThis, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;
    return pThis->execute("BEGIN", 5, retVal);
}

template <typename T>
result_t db_commit(T* pThis, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;
    return pThis->execute("COMMIT", 6, retVal);
}

template <typename T>
result_t db_rollback(T* pThis, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;
    return pThis->execute("ROLLBACK", 8, retVal);
}

template <typename T>
inline result_t db_trans(T* pThis, v8::Local<v8::Function> func, bool& retVal)
{
    result_t hr = 0;
    retVal = false;

    hr = pThis->ac_begin();
    if (hr < 0)
        return hr;

    pThis->leave();
    v8::Local<v8::Value> result = func->Call(pThis->wrap(), 0, NULL);
    pThis->enter();

    if (result.IsEmpty()) {
        pThis->ac_rollback();
        return CALL_E_JAVASCRIPT;
    }

    if (result->IsFalse())
        return pThis->ac_rollback();
    else {
        retVal = true;
        return pThis->ac_commit();
    }
}
}
