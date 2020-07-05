/**
 * @author Richard
 * @email ricahrdo2016@mail.com
 * @create date 2020-06-28 16:16:02
 * @modify date 2020-06-28 16:16:02
 * @desc [description]
 */

#include "ifs/StatsWatcher.h"
#include "ifs/fs.h"
#include "EventEmitter.h"
#include "EventInfo.h"
#include <uv/include/uv.h>
#include "Timer.h"
#include "utf8.h"
#include "Stat.h"
#include "AsyncUV.h"

#ifndef _fj_StatsWATCHER_H
#define _fj_StatsWATCHER_H

namespace fibjs {

#define DEFAULT_STATS_WATCH_INTERVAL 5007

class StatsWatcher;
static exlib::spinlock s_TargetWatcherMapLock;
static std::map<exlib::string, StatsWatcher*> s_TargetWatcherMap;

class StatsWatcher : public StatsWatcher_base {

public:
    StatsWatcher(
        exlib::string target,
        bool persistent = true,
        int32_t interval = DEFAULT_STATS_WATCH_INTERVAL,
        bool useBigInt = false)
        : m_target(target)
        , m_Persistent(persistent)
        , m_intervalMS(interval)
        , m_useBigInt(useBigInt)
        , m_closed(false)
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
    static bool isTargetBeingWatched(exlib::string& target)
    {
        s_TargetWatcherMapLock.lock();
        bool watching = (bool)s_TargetWatcherMap.count(target);
        s_TargetWatcherMapLock.unlock();

        return watching;
    }

    static bool setTargetWatcher(exlib::string& target, StatsWatcher* watcher)
    {
        s_TargetWatcherMapLock.lock();

        std::pair<std::map<exlib::string, StatsWatcher*>::iterator, bool> ret;
        ret = s_TargetWatcherMap.insert(std::pair<exlib::string, StatsWatcher*>(target, watcher));

        s_TargetWatcherMapLock.unlock();

        return ret.second;
    }

    static StatsWatcher* getTargetWatcher(exlib::string& target)
    {
        s_TargetWatcherMapLock.lock();

        std::map<exlib::string, StatsWatcher*>::iterator it = s_TargetWatcherMap.find(target);
        StatsWatcher* result = it != s_TargetWatcherMap.end() ? it->second : NULL;

        s_TargetWatcherMapLock.unlock();

        return result;
    }

    static void removeTargetWatcher(exlib::string& target)
    {
        if (!isTargetBeingWatched(target))
            return;
        s_TargetWatcherMapLock.lock();

        s_TargetWatcherMap.erase(target);

        s_TargetWatcherMapLock.unlock();
    }

public:
    class AsyncMonitorStatsChangeProc : public AsyncUVTask {
    public:
        AsyncMonitorStatsChangeProc(StatsWatcher* watcher)
            : m_watcher(watcher)
        {
        }
        ~AsyncMonitorStatsChangeProc()
        {
        }

    public:
        virtual int32_t post(int32_t v);

        virtual void invoke();

        static void freeSelfAfterUVHandleStop(uv_handle_t* handle)
        {
            StatsWatcher* proc = (StatsWatcher*)(handle->data);
            delete proc;
        }

    public:
        StatsWatcher* m_watcher;
        uv_timer_t m_timer_req;

    private:
        static void timer_callback(uv_timer_t* timer_req)
        {
            AsyncMonitorStatsChangeProc* p = NULL;
            p = ((AsyncMonitorStatsChangeProc*)((intptr_t)timer_req - (intptr_t)&p->m_timer_req));

            assert(&p->m_timer_req == timer_req);

            if (!p)
                return;

            uv_handle_t* handle = (uv_handle_t*)timer_req;
            if (p->m_watcher->m_closed) {
                handle->data = (void*)p;
                uv_close(handle, AsyncMonitorStatsChangeProc::freeSelfAfterUVHandleStop);
            } else
                p->m_watcher->checkStatsChangeInUVThread();
        }
    };

public:
    void onError(result_t hr, const char* msg)
    {
        _emit("error", new EventInfo(this, "error", hr, msg));
    }

public:
    void start()
    {
        if (m_closed)
            return;

        m_vholder = new ValueHolder(wrap());

        if (m_Persistent)
            isolate_ref();

        asyncCall(startWatchInNativeThread, this);
        watcherReadyWaitor.wait();
    };

    static result_t startWatchInNativeThread(StatsWatcher* watcher)
    {
        (new AsyncMonitorStatsChangeProc(watcher))->post(0);

        return 0;
    }

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
    result_t checkStatsChangeInUVThread()
    {
        Isolate* isolate = holder();

        result_t hr;
        bool existed;
        bool notReady = !watcherReadyWaitor.isSet();
        if (notReady)
            watcherReadyWaitor.set();

        hr = fs_base::cc_exists(m_target, existed, isolate);
        if (hr < 0)
            return hr;

        if (existed) {
            obj_ptr<Stat_base> fileStat;
            fs_base::cc_stat(m_target, fileStat, isolate);

            prev = cur;
            cur = fileStat;

            double pt;
            prev->get_mtimeMs(pt);
            double ct;
            cur->get_mtimeMs(ct);
            if (pt == ct)
                return 0;
        } else if (!notReady)
            return 0;

        Variant args[2];
        args[0] = cur;
        args[1] = prev;

        _emit("change", args, 2);

        return 0;
    }

public:
    int32_t getIntervalMS() { return m_intervalMS; }
    bool isPersistent() { return m_Persistent; }
    bool isUseBigInt() { return m_useBigInt; }

    exlib::atomic m_closed;

    exlib::Event watcherReadyWaitor;
    
protected:
    exlib::string m_target;

    bool m_Persistent;
    int32_t m_intervalMS;
    bool m_useBigInt;
    obj_ptr<ValueHolder> m_vholder;
    obj_ptr<Stat_base> prev;
    obj_ptr<Stat_base> cur;
};
}

#endif // _fj_StatsWATCHER_H
