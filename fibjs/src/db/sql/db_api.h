/*
 * SQLite.cpp
 *
 *  Created on: Oct 27, 2019
 *      Author: lion
 */

#include "object.h"

namespace fibjs {

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
result_t db_execute(T* pThis, exlib::string method, v8::Local<v8::Object> opts,
    obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        exlib::string str;
        result_t hr = pThis->format(method, opts, str);
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
result_t db_createTable(T* pThis, v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    return db_execute(pThis, "createTable", opts, _retVal, ac);
}

template <typename T>
result_t db_dropTable(T* pThis, v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    return db_execute(pThis, "dropTable", opts, _retVal, ac);
}

template <typename T>
result_t db_createIndex(T* pThis, v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    return db_execute(pThis, "createIndex", opts, _retVal, ac);
}

template <typename T>
result_t db_dropIndex(T* pThis, v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    return db_execute(pThis, "dropIndex", opts, _retVal, ac);
}

template <typename T>
result_t db_insert(T* pThis, v8::Local<v8::Object> opts, double& retVal, AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "insert", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[1].m_val.dblVal();

    return 0;
}

template <typename T>
result_t db_find(T* pThis, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal,
    AsyncEvent* ac)
{
    return db_execute(pThis, "find", opts, retVal, ac);
}

template <typename T>
result_t db_count(T* pThis, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "count", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    Variant v;
    _retVal->_indexed_getter(0, v);
    retVal = ((NObject*)v.object())->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_update(T* pThis, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "update", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_remove(T* pThis, v8::Local<v8::Object> opts, int32_t& retVal,
    AsyncEvent* ac)
{
    obj_ptr<NArray> _retVal;
    result_t hr = db_execute(pThis, "remove", opts, _retVal, ac);
    if (hr < 0)
        return hr;

    retVal = _retVal->m_values[0].m_val.intVal();

    return 0;
}

template <typename T>
result_t db_begin(T* pThis, exlib::string point, AsyncEvent* ac, bool mssql = false)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;

    if (point.empty())
        return pThis->execute("BEGIN", 5, retVal);
    else {
        exlib::string str((mssql ? "SAVE TRANSACTION " : "SAVEPOINT ") + point);
        return pThis->execute(str.c_str(), str.length(), retVal);
    }
}

template <typename T>
result_t db_commit(T* pThis, exlib::string point, AsyncEvent* ac, bool mssql = false)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;

    if (point.empty())
        return pThis->execute("COMMIT", 6, retVal);
    else {
        exlib::string str((mssql ? "COMMIT TRANSACTION " : "RELEASE SAVEPOINT ") + point);
        return pThis->execute(str.c_str(), str.length(), retVal);
    }
}

template <typename T>
result_t db_rollback(T* pThis, exlib::string point, AsyncEvent* ac, bool mssql = false)
{
    if (!pThis->m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NArray> retVal;

    if (point.empty())
        return pThis->execute("ROLLBACK", 8, retVal);
    else {
        exlib::string str((mssql ? "ROLLBACK TRANSACTION " : "ROLLBACK TO ") + point);
        return pThis->execute(str.c_str(), str.length(), retVal);
    }
}

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
}
