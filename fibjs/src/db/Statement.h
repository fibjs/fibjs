/*
 * Statement.h
 *
 *  Created on: Aug 19, 2026
 *      Author: fibjs
 */

#pragma once

#include "ifs/Statement.h"
#include "SimpleObject.h"
#include <memory>

namespace fibjs {

class StatementIterator;

/*! @brief Prepared statement object: reusable, supports row-by-row reads.

Engines plug in through an injected impl (cursor capability): compilation
happens at prepare time, binding and execution at open, and fetchRow pulls
rows one by one. The shared semantics of get/all/run/iterate/close are
implemented here.
*/
class Statement : public Statement_base {
public:
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

        // Fetch one row into row (column name → value); done=true means
        // exhausted (cursor released automatically).
        virtual result_t fetchRow(NObject* row, bool& done) = 0;

        // Affected-row count and auto-increment id for statements without a
        // result set.
        virtual result_t runResult(int64_t& changes, int64_t& lastInsertId) = 0;

        // Column metadata (does not execute the statement).
        virtual result_t columns(obj_ptr<NArray>& retVal) = 0;

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

    // Iterator access: fetch one row / abort and release the cursor
    result_t fetchRow(obj_ptr<NObject>& row, bool& done)
    {
        if (!m_impl)
            return CHECK_ERROR(CALL_E_CLOSED);

        row = new NObject();
        return m_impl->fetchRow(row, done);
    }

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
    virtual result_t get(OptArgs args, Variant& retVal, AsyncEvent* ac);
    virtual result_t all(OptArgs args, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t run(OptArgs args, Variant& retVal, AsyncEvent* ac);
    virtual result_t iterate(OptArgs args, obj_ptr<Iterator_base>& retVal, AsyncEvent* ac);
    virtual result_t columns(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t get_sourceSQL(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);

    // Internal: open the cursor with args already converted on the main thread
    // and return an iterator (used by conn.iterate)
    result_t iteratePrepared(std::vector<Variant>& args,
        obj_ptr<Iterator_base>& retVal);

public:
    // For engine prepare
    exlib::string& sql() { return m_sql; }

private:
    // On the main thread (isSync branch), convert v8 args to Variant stored in
    // ac->m_ctx: intermediate args are attached to the per-call AsyncEvent (the
    // same ac on fiber re-entry), and the fiber only reads m_ctx without
    // touching v8. Date is converted to a SQL string here (Variant's date_t has
    // no public accessor).
    result_t stashArgs(OptArgs args, AsyncEvent* ac)
    {
        ac->m_ctx.resize(args.Length());
        Isolate* isolate = holder();
        for (int32_t i = 0; i < args.Length(); i++) {
            result_t hr = GetArgumentValue(isolate, args[i], ac->m_ctx[i]);
            if (hr < 0)
                return hr;
            if (ac->m_ctx[i].type() == Variant::VT_Date) {
                date_t d = args[i];
                exlib::string s;
                d.sqlString(s);
                ac->m_ctx[i] = s;
            }
        }
        return 0;
    }

    result_t openImpl(std::vector<Variant>& args, bool& hasResult)
    {
        if (!m_impl)
            return CHECK_ERROR(CALL_E_CLOSED);

        result_t hr = m_impl->open(args, hasResult);
        if (hr < 0) {
            finish();
            return hr;
        }

        m_active = true;
        return 0;
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
    StatementIterator(Statement* stmt)
        : m_stmt(stmt)
        , m_done(false)
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
        return 0;
    }

    // Async iterator: returns itself. On the async prototype (db.promises
    // connections) next() returns Promise<{done,value}> and for await uses the
    // native async iterator protocol; on the sync prototype next() returns a
    // plain {done,value}, which for await also accepts.
    virtual result_t symbol_asyncIterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    virtual result_t next(obj_ptr<NextType>& retVal, AsyncEvent* ac);
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
};

} /* namespace fibjs */
