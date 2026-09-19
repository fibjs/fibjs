/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "ifs/fs.h"
#include "file_path.h"
#include "path.h"
#include "FSWatcher.h"
#include "StatsWatcher.h"

namespace fibjs {

// 共享表：path → 唯一 watcher（对齐 Node 的 statWatchers），表内保存强引用
static exlib::spinlock s_TargetWatcherMapLock;
static std::unordered_map<exlib::string, obj_ptr<StatsWatcher>> s_TargetWatcherMap;

bool StatsWatcher::setTargetWatcher(exlib::string& target, StatsWatcher* watcher)
{
    s_TargetWatcherMapLock.lock();

    auto ret = s_TargetWatcherMap.insert(
        std::make_pair(target, obj_ptr<StatsWatcher>(watcher)));

    s_TargetWatcherMapLock.unlock();

    return ret.second;
}

bool StatsWatcher::getTargetWatcher(exlib::string& target, obj_ptr<StatsWatcher>& result)
{
    s_TargetWatcherMapLock.lock();

    auto it = s_TargetWatcherMap.find(target);
    if (it != s_TargetWatcherMap.end())
        result = it->second; // 锁内取引用

    s_TargetWatcherMapLock.unlock();

    return result != nullptr;
}

void StatsWatcher::removeTargetWatcher(exlib::string& target)
{
    // 先摘出，避免持锁析构
    obj_ptr<StatsWatcher> removed;

    s_TargetWatcherMapLock.lock();

    auto it = s_TargetWatcherMap.find(target);
    if (it != s_TargetWatcherMap.end()) {
        removed = it->second;
        s_TargetWatcherMap.erase(it);
    }

    s_TargetWatcherMapLock.unlock();
}

result_t fs_base::watch(exlib::string fname, obj_ptr<FSWatcher_base>& retVal)
{
    return watch(fname, v8::Local<v8::Function>(), retVal);
}

result_t fs_base::watch(exlib::string fname, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    return watch(fname, opts, callback, retVal);
}

result_t fs_base::watch(exlib::string fname, v8::Local<v8::Object> options, obj_ptr<FSWatcher_base>& retVal)
{
    return watch(fname, options, v8::Local<v8::Function>(), retVal);
}

result_t get_safe_abs_path(exlib::string fname, exlib::string& safe_name)
{
    return absolute_file_path_like(fname, safe_name);
}

result_t fs_base::watch(exlib::string fname, v8::Local<v8::Object> options, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    bool persistent = true;
    hr = GetConfigValue(options, "persistent", persistent, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    bool recursive = false;
    hr = GetConfigValue(options, "recursive", recursive, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string encoding = "utf8";
    hr = GetConfigValue(options, "encoding", encoding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    obj_ptr<FSWatcher> pFW = new FSWatcher(safe_name, callback, persistent, recursive, encoding);
    retVal = pFW;

    return pFW->start();
}

result_t fs_base::watchFile(exlib::string fname, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    return watchFile(fname, opts, callback, retVal);
}

result_t fs_base::watchFile(exlib::string fname, v8::Local<v8::Object> options, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    obj_ptr<StatsWatcher> pSW;
    StatsWatcher::getTargetWatcher(safe_name, pSW);
    if (pSW == NULL) {
        bool persistent = true;
        hr = GetConfigValue(options, "persistent", persistent, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        bool useBigInt = false;
        hr = GetConfigValue(options, "bigint", useBigInt, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        int32_t interval = DEFAULT_STATS_WATCH_INTERVAL;
        hr = GetConfigValue(options, "interval", interval, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        pSW = new StatsWatcher(safe_name, persistent, interval, useBigInt);
        retVal = pSW;

        hr = pSW->start();
        if (hr < 0)
            return hr;
    }

    pSW->bindChangeHandler(callback);

    retVal = pSW;

    return hr;
}

result_t fs_base::unwatchFile(exlib::string fname)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    obj_ptr<StatsWatcher> pSW;
    if (!StatsWatcher::getTargetWatcher(safe_name, pSW))
        return 0;

    pSW->removeChangeHandler();

    return 0;
}

result_t fs_base::unwatchFile(exlib::string fname, v8::Local<v8::Function> callback)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    obj_ptr<StatsWatcher> pSW;
    if (!StatsWatcher::getTargetWatcher(safe_name, pSW))
        return 0;

    pSW->removeChangeHandler(callback);

    return 0;
}
}
