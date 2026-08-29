/*
 * Statement.cpp
 *
 *  Created on: Aug 19, 2026
 *      Author: fibjs
 */

#include "Statement.h"
#include "ifs/Iterator.h"

namespace fibjs {

result_t Statement::get(OptArgs args, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        // Main thread: v8 args → Variant stored in ac->m_ctx (must not touch v8
        // in a fiber)
        result_t hr = stashArgs(args, ac);
        if (hr < 0)
            return hr;
        return CHECK_ERROR(CALL_E_LONGSYNC);
    }

    bool hasResult = false;
    result_t hr = openImpl(ac->m_ctx, hasResult);
    if (hr < 0)
        return hr;

    if (hasResult) {
        obj_ptr<NObject> row;
        bool done = false;
        hr = fetchRow(row, done);
        if (hr < 0) {
            finish();
            return hr;
        }

        // Leave retVal at its default (undefined) when there is no result
        if (!done)
            retVal = row;
    }

    finish();
    return 0;
}

result_t Statement::all(OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = stashArgs(args, ac);
        if (hr < 0)
            return hr;
        return CHECK_ERROR(CALL_E_LONGSYNC);
    }

    bool hasResult = false;
    result_t hr = openImpl(ac->m_ctx, hasResult);
    if (hr < 0)
        return hr;

    obj_ptr<NArray> arr = new NArray();

    if (hasResult) {
        while (true) {
            obj_ptr<NObject> row;
            bool done = false;
            hr = fetchRow(row, done);
            if (hr < 0)
                break;
            if (done)
                break;

            arr->append(row);
        }
    } else {
        // No result set: same semantics as execute, attach affected/insertId
        int64_t changes = 0;
        int64_t lastInsertId = 0;
        hr = m_impl->runResult(changes, lastInsertId);
        if (hr >= 0) {
            arr->add("affected", changes);
            arr->add("insertId", lastInsertId);
        }
    }

    finish();

    if (hr < 0)
        return hr;

    retVal = arr;
    return 0;
}

result_t Statement::run(OptArgs args, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = stashArgs(args, ac);
        if (hr < 0)
            return hr;
        return CHECK_ERROR(CALL_E_LONGSYNC);
    }

    bool hasResult = false;
    result_t hr = openImpl(ac->m_ctx, hasResult);
    if (hr < 0)
        return hr;

    // Discard the result set (run semantics: only the impact stats matter);
    // for queries with a result set, drain it row by row
    if (hasResult) {
        while (true) {
            obj_ptr<NObject> row;
            bool done = false;
            hr = fetchRow(row, done);
            if (hr < 0 || done)
                break;
        }
    }

    int64_t changes = 0;
    int64_t lastInsertId = 0;
    if (hr >= 0)
        hr = m_impl->runResult(changes, lastInsertId);

    finish();
    if (hr < 0)
        return hr;

    // NObject is a fibjs wrapper (handles the v8 context internally) and is
    // safe in a coroutine; do not create a bare v8::Object here (creating v8
    // values inside a fiber crashes).
    obj_ptr<NObject> o = new NObject();
    o->add("changes", changes);
    o->add("lastInsertRowid", lastInsertId);

    retVal = o;
    return 0;
}

result_t Statement::iterate(OptArgs args, obj_ptr<Iterator_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = stashArgs(args, ac);
        if (hr < 0)
            return hr;
        return CHECK_ERROR(CALL_E_LONGSYNC);
    }

    return iteratePrepared(ac->m_ctx, retVal);
}

result_t Statement::iteratePrepared(std::vector<Variant>& args,
    obj_ptr<Iterator_base>& retVal)
{
    bool hasResult = false;
    result_t hr = openImpl(args, hasResult);
    if (hr < 0)
        return hr;

    // No result set: return an empty iterator (done is immediately true)
    if (!hasResult) {
        finish();
        retVal = new StatementIterator(this);
        return 0;
    }

    retVal = new StatementIterator(this);
    return 0;
}

result_t Statement::columns(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_impl)
        return CHECK_ERROR(CALL_E_CLOSED);

    return m_impl->columns(retVal);
}

result_t Statement::get_sourceSQL(exlib::string& retVal)
{
    retVal = m_sql;
    return 0;
}

result_t Statement::close(AsyncEvent* ac)
{
    closeImpl();
    return 0;
}

// ---------------------------------------------------------------------------
// StatementIterator
// ---------------------------------------------------------------------------

result_t StatementIterator::next(obj_ptr<NextType>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (m_done) {
        retVal = new NextType();
        retVal->done = true;
        return 0;
    }

    obj_ptr<NObject> row;
    bool done = false;
    result_t hr = m_stmt->fetchRow(row, done);
    if (hr < 0) {
        close();
        return hr;
    }

    retVal = new NextType();
    if (done) {
        retVal->done = true;
        close(); // finish: reset the cursor, keep the impl
    } else
        retVal->value = row;

    return 0;
}

result_t StatementIterator::_return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal)
{
    // Called by the JS engine when for await breaks early: release the cursor
    close();

    retVal = new ReturnType();
    retVal->done = true;
    return 0;
}

} /* namespace fibjs */
