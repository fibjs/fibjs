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

class StatsWatcher : public StatsWatcher_base {

public:
    // 定时器句柄持有者（uv 线程独占）。owner 持有强引用，
    // 保证句柄注册期间 watcher 不会被 GC 回收（句柄内存不放本对象内）。
    struct TimerSlot {
        uv_timer_t timer;
        obj_ptr<StatsWatcher> owner;
    };

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
    // 共享表（path → 唯一 watcher），实现见 fs_watch.cpp；查表即持有引用
    static bool setTargetWatcher(exlib::string& target, StatsWatcher* watcher);
    static bool getTargetWatcher(exlib::string& target, obj_ptr<StatsWatcher>& result);
    static void removeTargetWatcher(exlib::string& target);

public:
    void onError(result_t hr, const char* msg)
    {
        (new EventInfo(this, "error", hr, msg))->emit();
    }

public:
    // uv 线程：定时器回调
    static void on_timer(uv_timer_t* handle)
    {
        TimerSlot* slot = (TimerSlot*)handle->data;
        StatsWatcher* pThis = slot->owner;

        if (pThis->m_closed)
            pThis->close_timer();
        else
            pThis->checkStatsChangeOnTimerCb();
    }

    // uv 线程：句柄关闭后释放 slot（连带释放对 watcher 的引用）
    static void on_slot_closed(uv_handle_t* handle)
    {
        delete (TimerSlot*)handle->data;
    }

    // uv 线程：停止并关闭定时器，幂等
    void close_timer()
    {
        TimerSlot* slot = m_slot;

        if (slot && !uv_is_closing((uv_handle_t*)&slot->timer)) {
            uv_timer_stop(&slot->timer);
            uv_close((uv_handle_t*)&slot->timer, on_slot_closed);
        }
    }

    // 记账：不改 uv 句柄
    void close_impl()
    {
        if (m_closed)
            return;

        removeTargetWatcher(m_target);

        m_closed = true;

        if (m_Persistent)
            isolate_unref();

        if (m_vholder)
            m_vholder.Release();

        _emit("close");
    }

    result_t start()
    {
        if (m_closed || m_started)
            return 0;

        m_started = true;

        // 活跃期持有 JS wrapper（事件监听列表挂在 wrapper 上）
        m_vholder = new ValueHolder(wrap());

        if (m_Persistent)
            isolate_ref();

        return uv_call([&] {
            TimerSlot* slot = new TimerSlot();

            slot->owner = this; // 持有强引用
            slot->timer.data = slot;
            uv_timer_init(s_uv_loop, &slot->timer);
            m_slot = slot;

            int32_t uv_err_no = uv_timer_start(&slot->timer, on_timer, 0, getIntervalMS());
            if (uv_err_no != 0) {
                // start 失败：立刻关闭句柄
                close_timer();

                // 收尾回到 JS 侧执行（事件投递/释放 ValueHolder）
                obj_ptr<StatsWatcher> self = this;
                async([self, uv_err_no]() {
                    self->onError(CALL_E_INVALID_CALL, uv_strerror(uv_err_no));
                    self->close_impl();
                });
            }

            return uv_err_no;
        });
    };

public:
    void bindChangeHandler(v8::Local<v8::Function> callback)
    {
        ex_assert(!callback.IsEmpty());

        exlib::string _chname = "change";
        JSTrigger self = JSTrigger(this);
        self.putFunction(self.GetHiddenList(_chname, true), callback, _chname);
    }
    void removeChangeHandler(v8::Local<v8::Function> callback)
    {
        v8::Local<v8::Object> triggers;
        this->off(holder()->NewString("change"), callback, triggers);

        int32_t count;
        listenerCount(holder()->NewString("change"), count);

        if (count == 0)
            close();
    }
    void removeChangeHandler()
    {
        v8::Local<v8::Object> triggers;
        this->off(holder()->NewString("change"), triggers);

        close();
    }

public:
    // StatsWatcher_base
    virtual result_t close()
    {
        if (m_closed)
            return 0;

        close_impl();

        // 句柄关闭 post 给 uv 线程；slot 保活到 close 回调
        if (m_slot)
            uv_post([this] {
                close_timer();
            });

        return 0;
    };

    virtual result_t stop()
    {
        return close();
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
        if (m_closed || holder()->is_terminating())
            return 0;

        async([this]() {
            if (m_closed || holder()->is_terminating())
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

            if (m_closed || holder()->is_terminating())
                return;

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
    // 在 start() 的 uv_call 内写入后只读；只在 uv 线程解引用
    TimerSlot* m_slot = nullptr;

    // 仅 JS 线程使用
    bool m_started = false;
};
}
