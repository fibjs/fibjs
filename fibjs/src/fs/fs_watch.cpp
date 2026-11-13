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
    return watch(fname, (WatchOptions*)nullptr, callback, retVal);
}

result_t fs_base::watch(exlib::string fname, WatchOptions* options, obj_ptr<FSWatcher_base>& retVal)
{
    return watch(fname, options, v8::Local<v8::Function>(), retVal);
}

result_t get_safe_abs_path(exlib::string fname, exlib::string& safe_name)
{
    return absolute_file_path_like(fname, safe_name);
}

result_t fs_base::watch(exlib::string fname, WatchOptions* options, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)).failed())
        return result_t();

    bool persistent = options ? options->persistent.value_or(true) : true;
    bool recursive = options ? options->recursive.value_or(false) : false;
    exlib::string encoding = options ? options->encoding.value_or("utf8") : "utf8";

    obj_ptr<FSWatcher> pFW = new FSWatcher(safe_name, callback, persistent, recursive, encoding);
    retVal = pFW;

    return pFW->start();
}

result_t fs_base::watchFile(exlib::string fname, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal)
{
    return watchFile(fname, (WatchFileOptions*)nullptr, callback, retVal);
}

result_t fs_base::watchFile(exlib::string fname, WatchFileOptions* options, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)).failed())
        return result_t();

    obj_ptr<StatsWatcher> pSW;
    StatsWatcher::getTargetWatcher(safe_name, pSW);
    if (pSW == NULL) {
        bool persistent = options ? options->persistent.value_or(true) : true;
        bool useBigInt = options ? options->bigint.value_or(false) : false;
        int32_t interval = options ? options->interval.value_or(DEFAULT_STATS_WATCH_INTERVAL) : DEFAULT_STATS_WATCH_INTERVAL;

        pSW = new StatsWatcher(safe_name, persistent, interval, useBigInt);
        retVal = pSW;

        hr = pSW->start();
        if (hr.failed())
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
    if ((hr = get_safe_abs_path(fname, safe_name)).failed())
        return result_t();

    obj_ptr<StatsWatcher> pSW;
    if (!StatsWatcher::getTargetWatcher(safe_name, pSW))
        return result_t();

    pSW->removeChangeHandler();

    return result_t();
}

result_t fs_base::unwatchFile(exlib::string fname, v8::Local<v8::Function> callback)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)).failed())
        return result_t();

    obj_ptr<StatsWatcher> pSW;
    if (!StatsWatcher::getTargetWatcher(safe_name, pSW))
        return result_t();

    pSW->removeChangeHandler(callback);

    return result_t();
}
}
