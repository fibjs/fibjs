/*
 * Statement.cpp
 *
 *  Created on: Aug 19, 2026
 *      Author: fibjs
 */

#include "Statement.h"
#include "ifs/Iterator.h"

namespace fibjs {

class ObjectRowSink : public Statement::RowSink {
public:
    ObjectRowSink(NObject* row)
        : m_row(row)
    {
    }

    virtual void add(exlib::string name, Variant value)
    {
        m_row->add(name, value);
    }

private:
    NObject* m_row;
};

class ArrayRowSink : public Statement::RowSink {
public:
    ArrayRowSink(NArray* row)
        : m_row(row)
    {
    }

    virtual void add(exlib::string name, Variant value)
    {
        m_row->append(value);
    }

private:
    NArray* m_row;
};

class NullRowSink : public Statement::RowSink {
public:
    virtual void add(exlib::string, Variant) {}
};

result_t Statement::fetchRow(obj_ptr<NObject>& row, bool& done)
{
    if (!m_impl)
        return result_t::closed();

    row = new NObject();
    ObjectRowSink sink(row.operator->());
    return m_impl->fetchRow(&sink, done);
}

result_t Statement::fetchRow(obj_ptr<NArray>& row, bool& done)
{
    if (!m_impl)
        return result_t::closed();

    row = new NArray();
    ArrayRowSink sink(row.operator->());
    return m_impl->fetchRow(&sink, done);
}

result_t Statement::getImpl(OptArgs args, Variant& retVal, AsyncHandle ac, bool raw)
{
    if (ac.isSync()) {
        // Main thread: v8 args → Variant stored in ac->m_ctx (must not touch v8
        // in a fiber)
        result_t hr = stashArgs(args, ac);
        if (hr.failed())
            return hr;
        return result_t::longsync();
    }

    bool hasResult = false;
    result_t hr = openImpl(ac.ctxv(), hasResult);
    if (hr.failed())
        return hr;

    if (hasResult) {
        bool done = false;
        if (raw) {
            obj_ptr<NArray> row;
            hr = fetchRow(row, done);
            if (hr.failed()) {
                finish();
                return hr;
            }
            if (!done)
                retVal = row;
        } else {
            obj_ptr<NObject> row;
            hr = fetchRow(row, done);
            if (hr.failed()) {
                finish();
                return hr;
            }
            if (!done)
                retVal = row;
        }
    }

    finish();
    return result_t();
}

result_t Statement::get(OptArgs args, Variant& retVal, AsyncHandle ac)
{
    return getImpl(args, retVal, std::move(ac), false);
}

result_t Statement::rawGet(OptArgs args, Variant& retVal, AsyncHandle ac)
{
    return getImpl(args, retVal, std::move(ac), true);
}

result_t Statement::allImpl(OptArgs args, obj_ptr<NArray>& retVal, AsyncHandle ac, bool raw)
{
    if (ac.isSync()) {
        result_t hr = stashArgs(args, ac);
        if (hr.failed())
            return hr;
        return result_t::longsync();
    }

    bool hasResult = false;
    result_t hr = openImpl(ac.ctxv(), hasResult);
    if (hr.failed())
        return hr;

    obj_ptr<NArray> arr = new NArray();

    if (hasResult) {
        while (true) {
            bool done = false;
            if (raw) {
                obj_ptr<NArray> row;
                hr = fetchRow(row, done);
                if (hr.failed() || done)
                    break;
                arr->append(row);
            } else {
                obj_ptr<NObject> row;
                hr = fetchRow(row, done);
                if (hr.failed() || done)
                    break;
                arr->append(row);
            }
        }
    } else {
        // No result set: same semantics as execute, attach affected/insertId
        int64_t changes = 0;
        int64_t lastInsertId = 0;
        hr = m_impl->runResult(changes, lastInsertId);
        if (hr.ok()) {
            arr->add("affected", changes);
            arr->add("insertId", lastInsertId);
        }
    }

    finish();

    if (hr.failed())
        return hr;

    retVal = arr;
    return result_t();
}

result_t Statement::all(OptArgs args, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    return allImpl(args, retVal, std::move(ac), false);
}

result_t Statement::rawAll(OptArgs args, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    return allImpl(args, retVal, std::move(ac), true);
}

result_t Statement::run(OptArgs args, obj_ptr<RunType>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t hr = stashArgs(args, ac);
        if (hr.failed())
            return hr;
        return result_t::longsync();
    }

    bool hasResult = false;
    result_t hr = openImpl(ac.ctxv(), hasResult);
    if (hr.failed())
        return hr;

    // Discard the result set (run semantics: only the impact stats matter);
    // for queries with a result set, drain it row by row
    if (hasResult) {
        NullRowSink sink;
        while (true) {
            bool done = false;
            hr = m_impl->fetchRow(&sink, done);
            if (hr.failed() || done)
                break;
        }
    }

    int64_t changes = 0;
    int64_t lastInsertId = 0;
    if (hr.ok())
        hr = m_impl->runResult(changes, lastInsertId);

    finish();
    if (hr.failed())
        return hr;

    // The generated record keeps the two counters typed as Int64, so they
    // cross the JS boundary as BigInt without the precision loss of a Number.
    obj_ptr<RunType> o = new RunType();
    o->changes = changes;
    o->lastInsertRowid = lastInsertId;

    retVal = o;
    return result_t();
}

result_t Statement::iterate(OptArgs args, obj_ptr<Iterator_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t hr = stashArgs(args, ac);
        if (hr.failed())
            return hr;
        return result_t::longsync();
    }

    return iteratePrepared(ac.ctxv(), retVal);
}

result_t Statement::rawIterate(OptArgs args, obj_ptr<Iterator_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        result_t hr = stashArgs(args, ac);
        if (hr.failed())
            return hr;
        return result_t::longsync();
    }

    return iteratePrepared(ac.ctxv(), retVal, true);
}

result_t Statement::iteratePrepared(std::vector<Variant>& args,
    obj_ptr<Iterator_base>& retVal, bool raw)
{
    bool hasResult = false;
    result_t hr = openImpl(args, hasResult);
    if (hr.failed())
        return hr;

    // No result set: return an empty iterator (done is immediately true)
    if (!hasResult) {
        finish();
        retVal = new StatementIterator(this, raw);
        return result_t();
    }

    retVal = new StatementIterator(this, raw);
    return result_t();
}

result_t Statement::columns(std::vector<obj_ptr<ColumnsType>>& retVal, AsyncHandle ac)
{
    if (!m_impl)
        return result_t::closed();

    return m_impl->columns(retVal);
}

result_t Statement::get_sourceSQL(exlib::string& retVal)
{
    retVal = m_sql;
    return result_t();
}

result_t Statement::close(AsyncHandle ac)
{
    closeImpl();
    return result_t();
}

// ---------------------------------------------------------------------------
// StatementIterator
// ---------------------------------------------------------------------------

result_t StatementIterator::next(obj_ptr<NextType>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::longsync();

    if (m_done) {
        retVal = new NextType();
        retVal->done = true;
        return result_t();
    }

    obj_ptr<NObject> row;
    obj_ptr<NArray> rawRow;
    bool done = false;
    result_t hr;
    if (m_raw)
        hr = m_stmt->fetchRow(rawRow, done);
    else
        hr = m_stmt->fetchRow(row, done);
    if (hr.failed()) {
        close();
        return hr;
    }

    retVal = new NextType();
    if (done) {
        retVal->done = true;
        close(); // finish: reset the cursor, keep the impl
    } else if (m_raw)
        retVal->value = rawRow;
    else
        retVal->value = row;

    return result_t();
}

result_t StatementIterator::_return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal)
{
    // Called by the JS engine when for await breaks early: release the cursor
    close();

    retVal = new ReturnType();
    retVal->done = true;
    return result_t();
}

} /* namespace fibjs */
