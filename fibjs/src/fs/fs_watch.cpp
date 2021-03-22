/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "ifs/fs.h"
#include "path.h"
#include "FSWatcher.h"
#include "StatsWatcher.h"

namespace fibjs {

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
    result_t hr = 0;
    path_base::normalize(fname, safe_name);
    bool is_abs;
    path_base::isAbsolute(safe_name, is_abs);

    if (!is_abs)
        hr = _resolve(safe_name);

    return hr;
}

result_t fs_base::watch(exlib::string fname, v8::Local<v8::Object> options, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    Isolate* isolate = Isolate::current();
    bool persistent = true;
    hr = GetConfigValue(isolate->m_isolate, options, "persistent", persistent, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    bool recursive = false;
    hr = GetConfigValue(isolate->m_isolate, options, "recursive", recursive, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    obj_ptr<FSWatcher> pFW = new FSWatcher(safe_name, callback, persistent, recursive);
    retVal = pFW;

    pFW->start();

    return 0;
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

    obj_ptr<StatsWatcher> pSW = StatsWatcher::getTargetWatcher(safe_name);
    if (pSW == NULL) {
        Isolate* isolate = Isolate::current();
        bool persistent = true;
        hr = GetConfigValue(isolate->m_isolate, options, "persistent", persistent, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        bool useBigInt = false;
        hr = GetConfigValue(isolate->m_isolate, options, "bigint", useBigInt, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        int32_t interval = DEFAULT_STATS_WATCH_INTERVAL;
        hr = GetConfigValue(isolate->m_isolate, options, "interval", interval, true);
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

    obj_ptr<StatsWatcher> pSW = StatsWatcher::getTargetWatcher(safe_name);
    if (pSW == NULL)
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

    obj_ptr<StatsWatcher> pSW = StatsWatcher::getTargetWatcher(safe_name);
    if (pSW == NULL)
        return 0;

    pSW->removeChangeHandler(callback);

    return 0;
}
}
