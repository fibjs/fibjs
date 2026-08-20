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
        // 主线程：v8 参数 → Variant 存 ac->m_ctx（fiber 中不得触碰 v8）
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

        // 无结果时 retVal 保持默认（undefined）
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
        // 无结果集：与 execute 语义一致，附加 affected/insertId
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

    // 丢弃结果集（run 语义：只关心影响统计）；若是有结果集的查询，逐行耗尽
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

    // NObject 是 fibjs 封装（内部处理 v8 上下文），在协程中安全；
    // 不直接创建裸 v8::Object（fiber 中创建 v8 value 会崩溃）。
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

    // 无结果集：返回空迭代器（done 立即为 true）
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
        close(); // finish：游标复位，impl 保留
    } else
        retVal->value = row;

    return 0;
}

result_t StatementIterator::_return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal)
{
    // for await 提前 break 时由 JS 引擎调用：释放游标
    close();

    retVal = new ReturnType();
    retVal->done = true;
    return 0;
}

} /* namespace fibjs */
