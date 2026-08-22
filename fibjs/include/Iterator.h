/*
 * Iterator.h
 *
 *  Created on: Apr 21, 2021
 *      Author: lion
 */

#pragma once

#include "ifs/Iterator.h"
#include <functional>

namespace fibjs {

class Iterator : public Iterator_base {
public:
    // (err, has_value): err == 0 且 has_value == true  => 正常取值
    //                    err == 0 且 has_value == false => 迭代结束
    //                    err < 0                        => 失败，抛给调用方
    typedef std::function<void(result_t, bool)> IteratorCallback;
    typedef std::function<void(size_t, Variant&, IteratorCallback)> IteratorFunc;
    typedef std::function<void()> IteratorEndFunc;

public:
    Iterator(object_base* obj, IteratorFunc proc, IteratorEndFunc onclose = nullptr)
        : m_obj(obj)
        , m_proc(proc)
        , m_onclose(onclose)
        , m_index(0)
        , m_done(false)
    {
    }

    ~Iterator()
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

    // 异步迭代器：返回自身。sync 原型下 next() 返回普通 {done,value}，
    // async 原型下返回 Promise<{done,value}>，两者均可被 for await 消费。
    virtual result_t symbol_asyncIterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    virtual result_t _return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal)
    {
        close();
        retVal = new ReturnType();
        retVal->done = true;
        return 0;
    }

    virtual result_t next(obj_ptr<NextType>& retVal, AsyncEvent* ac)
    {
        if (m_done) {
            retVal = new NextType();
            retVal->done = true;
            return 0;
        }

        if (ac->isSync()) {
            return CALL_E_NOSYNC;
        }

        retVal = new NextType();
        m_proc(m_index++, retVal->value,
            [this, &retVal, ac](result_t err, bool has_value) {
                if (m_done) // 已被 return()/close 提前终止，丢弃迟到的回调
                    return;
                retVal->done = m_done = !has_value;
                ac->post(err); // err < 0 => JS 侧 next() 抛异常
            });

        return CALL_E_PENDDING;
    }

public:
    // 幂等关闭：释放 m_onclose 持有的资源；之后 next() 恒返回 {done:true}
    void close()
    {
        if (!m_done) {
            m_done = true;
            if (m_onclose)
                m_onclose();
        }
    }

private:
    obj_ptr<object_base> m_obj; // 保证宿主对象存活
    IteratorFunc m_proc;
    IteratorEndFunc m_onclose; // 可选；HttpCollection/XmlNodeList 传 nullptr
    size_t m_index;
    bool m_done;
};

} /* namespace fibjs */
