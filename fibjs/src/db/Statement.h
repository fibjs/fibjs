/*
 * Statement.h
 *
 *  Created on: Aug 19, 2026
 *      Author: fibjs
 */

#pragma once

#include "ifs/Statement.h"
#include "SimpleObject.h"
#include "Buffer.h"
#include <memory>

namespace fibjs {

class StatementIterator;

// Convert one JS argument into a Variant that can safely cross the fiber
// boundary. GetArgumentValue materializes binary views as Buffer and JS
// objects as native snapshots; the database keeps its own rule for containers
// so a bound parameter stringifies exactly like execute()/format().
inline result_t db_stash_arg(Isolate* isolate, v8::Local<v8::Value> v, Variant& out)
{
    // Date is a SQL timestamp string, containers String(value); binary views
    // (including ArrayBuffer/DataView) and wrapper objects fall through to
    // GetArgumentValue.
    if (!v.IsEmpty() && !v->IsDate() && !v->IsFunction()
        && !v->IsBooleanObject() && !v->IsNumberObject()
        && !v->IsStringObject() && !v->IsBigIntObject()
        && !IsJSBuffer(v, false) && v->IsObject()) {
        exlib::string s;
        result_t hr = GetDOMStringValue(v, s);
        if (hr.failed())
            return hr;
        out = s;
        return result_t();
    }

    result_t hr = GetArgumentValue(isolate, v, out);
    if (hr.failed())
        return hr;

    if (out.type() == Variant::VT_Date) {
        // Variant's date_t has no public sqlString accessor.
        date_t d = v;
        exlib::string s;
        d.sqlString(s);
        out = s;
    }

    return result_t();
}

/*! @brief Prepared statement object: reusable, supports row-by-row reads.

Engines plug in through an injected impl (cursor capability): compilation
happens at prepare time, binding and execution at open, and fetchRow pulls
rows one by one. The shared semantics of get/all/run/iterate/close are
implemented here.
*/
class Statement : public Statement_base {
public:
    // Receives one column at a time. Object rows keep the column name as a key;
    // positional rows keep the order and therefore preserve duplicate names.
    class RowSink {
    public:
        virtual ~RowSink() {}
        virtual void add(exlib::string name, Variant value) = 0;
    };

    // Engine cursor interface
    class impl {
    public:
        virtual ~impl() {}

        // Bind args and execute; hasResult=true means a result set is available
        // for row-by-row reads. The cursor is left open on return (the engine
        // is responsible for the connection-level mutex).
        // args were already converted from v8 to Variant on the main thread
        // (safe in a fiber).
        virtual result_t open(std::vector<Variant>& args, bool& hasResult) = 0;

        // Fetch one row into the sink in column order; done=true means
        // exhausted (cursor released automatically).
        virtual result_t fetchRow(RowSink* row, bool& done) = 0;

        // Affected-row count and auto-increment id for statements without a
        // result set.
        virtual result_t runResult(int64_t& changes, int64_t& lastInsertId) = 0;

        // Column metadata (does not execute the statement).
        virtual result_t columns(std::vector<obj_ptr<Statement_base::ColumnsType>>& retVal) = 0;

        // Cursor reset: called after each execute/iteration, releases the
        // connection-level occupancy; the impl is retained and can be open()ed
        // again (engines implement drain-like semantics here).
        virtual void reset() = 0;

        // Fully release the underlying handle (idempotent); only called when
        // the Statement is destroyed.
        virtual void close() = 0;
    };

public:
    Statement(exlib::string sql, impl* p)
        : m_sql(sql)
        , m_impl(p)
        , m_active(false)
    {
    }

    ~Statement()
    {
        closeImpl();
    }

    // Object-row fetch helper (legacy shape).
    result_t fetchRow(obj_ptr<NObject>& row, bool& done);

    // Positional-row fetch helper: values are appended in column order and
    // duplicate column names are preserved.
    result_t fetchRow(obj_ptr<NArray>& row, bool& done);

    // Execute/iteration finished: release the cursor occupancy, keep the impl
    // for reuse
    void finish()
    {
        if (m_active) {
            m_active = false;
            if (m_impl)
                m_impl->reset();
        }
    }

    void abortIterate()
    {
        finish();
    }

public:
    // Statement_base
    virtual result_t get(OptArgs args, Variant& retVal, AsyncHandle ac);
    virtual result_t rawGet(OptArgs args, Variant& retVal, AsyncHandle ac);
    virtual result_t all(OptArgs args, obj_ptr<NArray>& retVal, AsyncHandle ac);
    virtual result_t rawAll(OptArgs args, obj_ptr<NArray>& retVal, AsyncHandle ac);
    virtual result_t run(OptArgs args, obj_ptr<RunType>& retVal, AsyncHandle ac);
    virtual result_t iterate(OptArgs args, obj_ptr<Iterator_base>& retVal, AsyncHandle ac);
    virtual result_t rawIterate(OptArgs args, obj_ptr<Iterator_base>& retVal, AsyncHandle ac);
    virtual result_t columns(std::vector<obj_ptr<ColumnsType>>& retVal, AsyncHandle ac);
    virtual result_t get_sourceSQL(exlib::string& retVal);
    virtual result_t close(AsyncHandle ac);

    // Internal: open the cursor with args already converted on the main thread
    // and return an iterator (used by conn.iterate); raw=true yields arrays.
    result_t iteratePrepared(std::vector<Variant>& args,
        obj_ptr<Iterator_base>& retVal, bool raw = false);

public:
    // For engine prepare
    exlib::string& sql() { return m_sql; }

private:
    // Shared row-fetch implementations for the object and positional forms.
    result_t getImpl(OptArgs args, Variant& retVal, AsyncHandle ac, bool raw);
    result_t allImpl(OptArgs args, obj_ptr<NArray>& retVal, AsyncHandle ac, bool raw);

    // On the main thread (isSync branch), convert v8 args to Variant stored in
    // ac->m_ctx: intermediate args are attached to the per-call AsyncEvent (the
    // same ac on fiber re-entry), and the fiber only reads m_ctx without
    // touching v8. Date is converted to a SQL string here (Variant's date_t has
    // no public accessor).
    result_t stashArgs(OptArgs args, AsyncHandle& ac)
    {
        ac.ctxv().resize(args.Length());
        Isolate* isolate = holder();
        for (int32_t i = 0; i < args.Length(); i++) {
            result_t hr = db_stash_arg(isolate, args[i], ac.ctxv()[i]);
            if (hr.failed())
                return hr;
        }
        return result_t();
    }

    result_t openImpl(std::vector<Variant>& args, bool& hasResult)
    {
        if (!m_impl)
            return result_t::closed();

        result_t hr = m_impl->open(args, hasResult);
        if (hr.failed()) {
            finish();
            return hr;
        }

        m_active = true;
        return result_t();
    }

    void closeImpl()
    {
        if (m_impl) {
            m_impl->close();
            m_impl.reset();
        }
        m_active = false;
    }

private:
    friend class StatementIterator;

    exlib::string m_sql;
    std::unique_ptr<impl> m_impl;
    bool m_active;
};

/*! @brief Row iterator of a Statement: implements Iterator_base directly,
next() fetches a row inside the coroutine. */
class StatementIterator : public Iterator_base {
public:
    StatementIterator(Statement* stmt, bool raw = false)
        : m_stmt(stmt)
        , m_done(false)
        , m_raw(raw)
    {
    }

    ~StatementIterator()
    {
        close();
    }

public:
    // Iterator_base
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return result_t();
    }

    // Async iterator: returns itself. On the async prototype (db.promises
    // connections) next() returns Promise<{done,value}> and for await uses the
    // native async iterator protocol; on the sync prototype next() returns a
    // plain {done,value}, which for await also accepts.
    virtual result_t symbol_asyncIterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return result_t();
    }

    virtual result_t next(obj_ptr<NextType>& retVal, AsyncHandle ac);
    virtual result_t _return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal);

private:
    void close()
    {
        if (!m_done) {
            m_done = true;
            m_stmt->abortIterate();
        }
    }

private:
    obj_ptr<Statement> m_stmt;
    bool m_done;
    bool m_raw;
};

} /* namespace fibjs */
