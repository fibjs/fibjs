/*
 * AbortController.h
 *
 *  Created on: Jul 13, 2024
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "ifs/AbortController.h"
#include "v8.h"

namespace fibjs {

class AbortSignal : public AbortSignal_base {
public:
    // AbortSignal_base
    virtual result_t throwIfAborted();
    virtual result_t get_aborted(bool& retVal);
    virtual result_t get_reason(v8::Local<v8::Value>& retVal);

public:
    // Internal abort methods (called by AbortController)
    result_t do_abort(exlib::string reason, obj_ptr<AbortSignal_base>& retVal);
    result_t do_abort(v8::Local<v8::Value> reason, obj_ptr<AbortSignal_base>& retVal);

public:
    // object_base
    virtual result_t onEventEmit(exlib::string ev)
    {
        if (ev == "abort") {
            m_aborted = true;
            for (const auto& callback : m_callbacks) {
                callback();
            }
        }
        return 0;
    }

public:
    bool is_aborted() const
    {
        return m_aborted;
    }

    void addAbortCallback(std::function<void()> callback)
    {
        m_callbacks.push_back(callback);
    }

private:
    std::vector<std::function<void()>> m_callbacks;
    std::atomic_bool m_aborted = false;
    exlib::string m_reason;
    bool m_has_value_reason = false; // 标识是否使用了 v8::Value reason
};

class AbortController : public AbortController_base {
public:
    AbortController(Isolate* isolate)
    {
        holder(isolate);
        m_signal = new AbortSignal();
        m_signal->holder(isolate);
    }

public:
    // AbortController_base
    virtual result_t get_signal(obj_ptr<AbortSignal_base>& retVal);
    virtual result_t abort(exlib::string reason);
    virtual result_t abort(v8::Local<v8::Value> reason);

private:
    obj_ptr<AbortSignal> m_signal;
};

}
