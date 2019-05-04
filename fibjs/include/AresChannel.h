/*
 * DnsClient.h
 *
 *  Created on: May 10, 2019
 *      Author: Asion
 */

#ifndef ARESCHANNEL_H_
#define ARESCHANNEL_H_

#include "Timer.h"
#include <cares/ares.h>
#include <unordered_set>

namespace fibjs {

class AresChannel;
class ChannelTask {
public:
    ChannelTask()
        : m_channel(NULL)
        , m_sock(ARES_SOCKET_BAD)
    {
    }
    ChannelTask(AresChannel* channel, ares_socket_t sock)
        : m_channel(channel)
        , m_sock(sock)
    {
    }
    virtual ~ChannelTask() {}

public:
    AresChannel* m_channel;
    ares_socket_t m_sock;
};

struct TaskHash {
    size_t operator()(ChannelTask* a) const
    {
        return std::hash<ares_socket_t>()(a->m_sock);
    }
};

struct TaskEqual {
    inline bool operator()(ChannelTask* a, ChannelTask* b) const
    {
        return a->m_sock == b->m_sock;
    }
};

typedef std::unordered_set<ChannelTask*, TaskHash, TaskEqual> fib_ares_task_list;
typedef std::unordered_set<ChannelTask*, TaskHash, TaskEqual>::iterator fib_ares_task_list_iterator;

class asyncDNSQuery : public exlib::Task_base {
public:
    asyncDNSQuery(AsyncEvent* ac)
        : m_ac(ac)
        , m_sock(ARES_SOCKET_BAD)
    {
    }
    asyncDNSQuery()
    {
    }
    void resume()
    {
    }

public:
    result_t call();

    virtual int32_t process()
    {
        return 0;
    }

    virtual void proc()
    {
        ready(process());
    }

    void ready(int32_t v)
    {
        m_ac->apost(v);
        delete this;
    }
    void onready()
    {
        proc();
    }

public:
    AsyncEvent* m_ac;
    ares_socket_t m_sock;
};

class AresChannel : public obj_base {
public:
    class QueryTimer : public Timer {
    public:
        QueryTimer(AresChannel* channel)
            : Timer(1000, true)
            , m_channel(channel)
        {
        }
        void on_timer()
        {
            m_channel->AresTimeout();
        }

    public:
        obj_ptr<AresChannel> m_channel;
    };

public:
    AresChannel()
        : m_bLibraryInited(false)
        , m_bDefaultServer(true)
        , m_queryLastOk(true)
        , m_channel(NULL)
        , m_timer(NULL)
    {
    }
    ~AresChannel();
    int32_t Setup();
    void EnsureServers();
    void StartTimer();
    void CloseTimer();
    void ResetTimer();
    void AresTimeout();
    ChannelTask* AsyncSend(ChannelTask* task, int32_t read, int32_t write, bool exists);
    void AsyncCallback(asyncDNSQuery* query);
    void AsyncClose(ChannelTask* task);
    inline fib_ares_task_list* getTaskList() { return &m_taskList; }
    static void ares_sockstate_cb(void* data, ares_socket_t sock, int read, int write);

public:
    bool m_bLibraryInited;
    bool m_bDefaultServer;
    bool m_queryLastOk;
    exlib::Semaphore m_sem;
    exlib::atomic m_activeQueryCnt;
    ares_channel m_channel;
    fib_ares_task_list m_taskList;
    obj_ptr<QueryTimer> m_timer;
};
}

#endif //ARESCHANNEL_H_
