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

/*! @brief 预编译语句对象，可反复执行，支持按条读取。

各引擎通过注入 impl（游标能力）接入：prepare 时编译，open 时绑定参数并
执行，fetchRow 逐行抓取。get/all/run/iterate/close 的公共语义在此实现。
*/
class Statement : public Statement_base {
public:
    // 引擎游标接口
    class impl {
    public:
        virtual ~impl() {}

        // 绑定参数并执行；hasResult=true 表示有结果集可逐行读取。
        // 调用后游标处于打开状态（连接级互斥由引擎负责）。
        // args 已由主线程从 v8 转换为 Variant（fiber 中安全）。
        virtual result_t open(std::vector<Variant>& args, bool& hasResult) = 0;

        // 抓取一行填充 row（列名 → 值）；done=true 表示耗尽（游标自动释放）。
        virtual result_t fetchRow(NObject* row, bool& done) = 0;

        // 无结果集语句的受影响行数与自增 id。
        virtual result_t runResult(int64_t& changes, int64_t& lastInsertId) = 0;

        // 列元数据（不执行语句）。
        virtual result_t columns(obj_ptr<NArray>& retVal) = 0;

        // 游标复位：每次执行/迭代结束后调用，释放连接级占用；
        // impl 保留，可再次 open()（引擎在此实现 drain 等语义）。
        virtual void reset() = 0;

        // 彻底释放底层句柄（幂等），仅 Statement 销毁时调用。
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

    // 迭代器访问：抓取一行 / 中止并释放游标
    result_t fetchRow(obj_ptr<NObject>& row, bool& done)
    {
        if (!m_impl)
            return CHECK_ERROR(CALL_E_CLOSED);

        row = new NObject();
        return m_impl->fetchRow(row, done);
    }

    // 执行/迭代结束：释放游标占用，impl 保留供复用
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

    // 内部：用主线程已转换好的参数打开游标并返回迭代器（conn.iterate 使用）
    result_t iteratePrepared(std::vector<Variant>& args,
        obj_ptr<Iterator_base>& retVal);

public:
    // 供引擎 prepare 使用
    exlib::string& sql() { return m_sql; }

private:
    // 主线程（isSync 分支）把 v8 参数转换为 Variant 存到 ac->m_ctx：
    // 中间参数挂在每次调用独立的 AsyncEvent 上（fiber 重入时同一 ac），
    // fiber 中只读 m_ctx，不触碰 v8。Date 在此转成 SQL 字符串
    // （Variant 的 date_t 无公开访问接口）。
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

/*! @brief Statement 的行迭代器：直接实现 Iterator_base，next() 在协程内抓行。 */
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
