/*
 * EventSource.h
 *
 *  Created on: Dec 31, 2024
 *      Author: lion
 */

#pragma once

#include <boost/preprocessor.hpp>

#include "ifs/EventSource.h"
#include "ifs/sse.h"

namespace fibjs {

class EventSource : public EventSource_base {
public:
    class SendOptions : public obj_base {
    public:
        LOAD_OPTIONS(SendOptions, (id)(event)(retry));

    public:
        std::optional<exlib::string> id;
        std::optional<exlib::string> event;
        std::optional<int32_t> retry;
    };

    // 一次性票据：把 sse.upgrade 交出的「票」（AsyncState 的 continuation）包一层，
    // 保证**回投有且只有一次**（对齐 WebSocket::m_closeState 的仲裁语义）。
    // 票指向的机器是自删除对象（AsyncState::post_ 结尾 delete this），所以裸指针的
    // 第二次回投就是对已释放对象的虚调用 —— Windows worker 套件 core dump 的成因。
    class Ticket;

public:
    EventSource() = default;
    ~EventSource();

    // EventSource_base
    virtual result_t close(AsyncEvent* ac);
    virtual result_t send(exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac);
    virtual result_t get_readyState(int32_t& retVal);
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_withCredentials(bool& retVal);
    virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal);

public:
    // sse.upgrade 的 accept 状态调用：接管票据（唯一入口，保证票只有一张）
    void setTicket(AsyncEvent* target);

public:
    exlib::string m_url;
    int32_t m_readyState = sse_base::C_CONNECTING;
    obj_ptr<HttpResponse_base> m_response;
    obj_ptr<Stream_base> m_stream;
    Ticket* m_ac = nullptr;   // 本对象持有；被 close() 或析构消费后置空
};

}
