/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-28 16:16:02
 * @modify date 2020-06-28 16:16:02
 * @desc [description]
 */

#pragma once

#include "ifs/StatsWatcher.h"
#include "ifs/fs.h"
#include "EventEmitter.h"
#include "EventInfo.h"
#include <uv/include/uv.h>
#include "Timer.h"
#include "utf8.h"
#include "Stat.h"
#include "AsyncUV.h"

namespace fibjs {

#define DEFAULT_STATS_WATCH_INTERVAL 5007

class StatsWatcher;
static exlib::spinlock s_TargetWatcherMapLock;
static std::unordered_map<exlib::string, StatsWatcher*> s_TargetWatcherMap;

class StatsWatcher : public StatsWatcher_base {

public:
    StatsWatcher(
        exlib::string target,
        bool persistent = true,
        int32_t interval = DEFAULT_STATS_WATCH_INTERVAL,
        bool useBigInt = false)
        : m_closed(false)
        , m_target(target)
        , m_Persistent(persistent)
        , m_intervalMS(interval)
        , m_useBigInt(useBigInt)
    {
        if (m_intervalMS < 20)
            m_intervalMS = DEFAULT_STATS_WATCH_INTERVAL;

        obj_ptr<Stat> tmp;
        tmp = new Stat();
        tmp->init();
        prev = tmp;

        tmp = new Stat();
        tmp->init();
        cur = tmp;

        setTargetWatcher(m_target, this);
    }

    ~StatsWatcher() {};

public:
    EVENT_SUPPORT();

public:
    EVENT_FUNC(change);
    EVENT_FUNC(close);
    EVENT_FUNC(error);

public:
    static bool setTargetWatcher(exlib::string& target, StatsWatcher* watcher)
    {
        s_TargetWatcherMapLock.lock();

        std::pair<std::unordered_map<exlib::string, StatsWatcher*>::iterator, bool> ret;
        ret = s_TargetWatcherMap.insert(std::pair<exlib::string, StatsWatcher*>(target, watcher));

        s_TargetWatcherMapLock.unlock();

        return ret.second;
    }

    static StatsWatcher* getTargetWatcher(exlib::string& target)
    {
        s_TargetWatcherMapLock.lock();

        std::unordered_map<exlib::string, StatsWatcher*>::iterator it = s_TargetWatcherMap.find(target);
        StatsWatcher* result = it != s_TargetWatcherMap.end() ? it->second : NULL;

        s_TargetWatcherMapLock.unlock();

        return result;
    }

    static void removeTargetWatcher(exlib::string& target)
    {
        s_TargetWatcherMapLock.lock();

        if ((bool)s_TargetWatcherMap.count(target))
            s_TargetWatcherMap.erase(target);

        s_TargetWatcherMapLock.unlock();
    }

public:
    void onError(result_t hr, const char* msg)
    {
        _emit("error", new EventInfo(this, "error", hr, msg));
    }

public:
    static void timer_callback(uv_timer_t* timer_req)
    {
        StatsWatcher* pThis = NULL;
        pThis = container_of(timer_req, StatsWatcher, m_timer_req);

        assert(&pThis->m_timer_req == timer_req);

        if (!pThis)
            return;

        uv_handle_t* handle = (uv_handle_t*)timer_req;
        if (pThis->m_closed) {
            handle->data = (void*)pThis;

            uv_close(handle, NULL);
        } else {
            pThis->checkStatsChangeOnTimerCb();
        }
    }

    result_t start()
    {
        if (m_closed)
            return 0;

        m_vholder = new ValueHolder(wrap());

        if (m_Persistent)
            isolate_ref();

        return uv_call([&] {
            uv_timer_init(s_uv_loop, &m_timer_req);
            int32_t uv_err_no = uv_timer_start(&m_timer_req, timer_callback, 0, getIntervalMS());
            if (uv_err_no != 0) {
                onError(CALL_E_INVALID_CALL, uv_strerror(uv_err_no));
                close();
            }

            return uv_err_no;
        });
    };

public:
    void bindChangeHandler(v8::Local<v8::Function> callback)
    {
        assert(!callback.IsEmpty());

        exlib::string _chname = "change";
        JSTrigger self = JSTrigger(this);
        self.putFunction(self.GetHiddenList(_chname, true), callback, _chname);
    }
    void removeChangeHandler(v8::Local<v8::Function> callback)
    {
        v8::Local<v8::Object> triggers;
        this->off(exlib::string("change"), callback, triggers);

        int32_t count;
        listenerCount("change", count);

        if (count == 0)
            close();
    }
    void removeChangeHandler()
    {
        v8::Local<v8::Object> triggers;
        this->off(exlib::string("change"), triggers);

        close();
    }

public:
    // StatsWatcher_base
    virtual result_t close()
    {
        if (m_closed)
            return 0;

        removeTargetWatcher(m_target);

        m_closed = true;

        if (m_Persistent)
            isolate_unref();

        if (m_vholder)
            m_vholder.Release();

        _emit("close");

        return 0;
    };

    virtual result_t ref(obj_ptr<StatsWatcher_base>& retVal)
    {
        isolate_ref();

        retVal = this;

        return 0;
    };

    virtual result_t unref(obj_ptr<StatsWatcher_base>& retVal)
    {
        isolate_unref();

        retVal = this;

        return 0;
    };

public:
    result_t checkStatsChangeOnTimerCb()
    {
        if (m_closed)
            return 0;

        async([this]() {
            if (m_closed)
                return;

            result_t hr;
            bool existed;
            bool ready = m_bStarted;
            if (!ready)
                m_bStarted = true;

            hr = fs_base::cc_exists(m_target, existed, holder());
            if (hr < 0)
                return;

            if (existed) {
                obj_ptr<Stat_base> fileStat;
                hr = fs_base::cc_stat(m_target, fileStat, holder());
                if (hr < 0)
                    return;

                prev = cur;
                cur = fileStat;

                double pt;
                prev->get_mtimeMs(pt);
                double ct;
                cur->get_mtimeMs(ct);

                if (pt == ct) {
                    return;
                }
            } else if (ready)
                return;

            Variant args[2];
            args[0] = cur;
            args[1] = prev;

            _emit("change", args, 2);
        });

        return 0;
    }

public:
    int32_t getIntervalMS() { return m_intervalMS; }
    bool isPersistent() { return m_Persistent; }
    bool isUseBigInt() { return m_useBigInt; }

    exlib::atomic m_closed;
    exlib::atomic m_bStarted;

protected:
    exlib::string m_target;

    bool m_Persistent;
    int32_t m_intervalMS;
    bool m_useBigInt;
    obj_ptr<ValueHolder> m_vholder;
    obj_ptr<Stat_base> prev;
    obj_ptr<Stat_base> cur;

private:
    uv_timer_t m_timer_req;
};
}
