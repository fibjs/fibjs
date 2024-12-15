/*
 * perf_hooks.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/perf_hooks.h"
#include <uv/include/uv.h>
#include "PerformanceObserver.h"
#include "Fiber.h"

namespace fibjs {

DECLARE_MODULE(perf_hooks);

static uint64_t s_startup = uv_hrtime();

inline double perf_now()
{
    return (double)(uv_hrtime() - s_startup) / 1000000.0;
}

result_t PerformanceObserver_base::_new(v8::Local<v8::Function> callback, obj_ptr<PerformanceObserver_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PerformanceObserver> pos = new PerformanceObserver();
    pos->wrap(This);
    retVal = pos;

    pos->m_callback.Reset(pos->holder()->m_isolate, callback);

    return 0;
}

result_t PerformanceObserver::observe(v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    std::vector<exlib::string> entryTypes;
    result_t hr;

    hr = GetConfigValue(isolate, options, "entryTypes", entryTypes, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (entryTypes.empty()) {
        exlib::string type;
        hr = GetConfigValue(isolate, options, "type", type, true);
        if (hr < 0)
            return hr;

        entryTypes.push_back(type);
    }

    isolate->m_perfLock.lock();
    for (auto& entryType : entryTypes) {
        if (m_entryTypes.end() == std::find(m_entryTypes.begin(), m_entryTypes.end(), entryType)) {
            m_entryTypes.push_back(entryType);

            auto pos = isolate->m_perfObservers.find(entryType);
            if (pos == isolate->m_perfObservers.end())
                isolate->m_perfObservers.insert({ entryType, { this } });
            else
                pos->second.push_back(this);
        }
    }
    isolate->m_perfLock.unlock();

    return 0;
}

result_t PerformanceObserver::disconnect()
{
    Isolate* isolate = holder();

    isolate->m_perfLock.lock();
    for (auto& entryType : m_entryTypes) {
        auto pos = isolate->m_perfObservers.find(entryType);
        if (pos != isolate->m_perfObservers.end()) {
            auto& observers = pos->second;
            observers.erase(std::remove(observers.begin(), observers.end(), this), observers.end());
        }
    }
    isolate->m_perfLock.unlock();

    return 0;
}

result_t PerformanceObserver::takeRecords(obj_ptr<NArray>& retVal)
{
    Isolate* isolate = holder();

    retVal = new NArray();

    isolate->m_perfLock.lock();
    for (auto& entry : m_entries)
        retVal->append(entry);
    m_entries.clear();
    isolate->m_perfLock.unlock();

    return 0;
}

void PerformanceObserver::emit(obj_ptr<PerformanceEntry_base> entry)
{
    m_entries.push_back(entry);

    if (m_running.exchange(true))
        return;

    Isolate* isolate = holder();
    isolate->sync([this, isolate]() -> int32_t {
        JSFiber::EnterJsScope s;
        v8::Local<v8::Context> context = isolate->context();

        v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate->m_isolate, m_callback);

        while (true) {
            std::vector<obj_ptr<PerformanceEntry_base>> entries;

            isolate->m_perfLock.lock();
            entries = std::move(m_entries);
            isolate->m_perfLock.unlock();

            if (entries.empty()) {
                m_running = false;
                return 0;
            }

            obj_ptr<PerformanceObserverEntryList> entryList = new PerformanceObserverEntryList(entries);
            v8::Local<v8::Value> args = entryList->wrap();
            callback->Call(context, wrap(), 1, &args);
        }

        return 0;
    });
}

result_t PerformanceObserverEntryList::getEntries(obj_ptr<NArray>& retVal)
{
    retVal = new NArray();

    for (auto& entry : m_entries)
        retVal->append(entry);

    return 0;
}

result_t PerformanceObserverEntryList::getEntriesByName(exlib::string name, exlib::string entryType, obj_ptr<NArray>& retVal)
{
    retVal = new NArray();

    for (auto& entry : m_entries) {
        exlib::string _name;
        entry->get_name(_name);
        if (_name == name) {
            if (entryType.empty()) {
                retVal->append(entry);
                continue;
            }

            exlib::string _entryType;
            entry->get_entryType(_entryType);
            if (_entryType == entryType)
                retVal->append(entry);
        }
    }

    return 0;
}

result_t PerformanceObserverEntryList::getEntriesByType(exlib::string entryType, obj_ptr<NArray>& retVal)
{
    retVal = new NArray();

    for (auto& entry : m_entries) {
        exlib::string _entryType;
        entry->get_entryType(_entryType);
        if (_entryType == entryType)
            retVal->append(entry);
    }

    return 0;
}

result_t performance_base::clearMarks(exlib::string name)
{
    Isolate* isolate = Isolate::current();

    isolate->m_perfLock.lock();
    if (name.empty())
        isolate->m_markEntries.clear();
    else
        isolate->m_markEntries.erase(name);
    isolate->m_perfLock.unlock();

    return 0;
}

result_t performance_base::clearMeasures(exlib::string name)
{
    return 0;
}

result_t performance_base::mark(exlib::string name, v8::Local<v8::Object> options)
{
    Isolate* isolate = Isolate::current();
    result_t hr;

    double _now = perf_now();
    hr = GetConfigValue(isolate, options, "startTime", _now, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    v8::Local<v8::Value> detail;
    GetConfigValue(isolate, options, "detail", detail);

    obj_ptr<PerformanceMark> mark = new PerformanceMark(name, _now, detail);

    isolate->m_perfLock.lock();
    isolate->m_markEntries.insert_or_assign(name, mark);
    auto pos = isolate->m_perfObservers.find("mark");
    if (pos != isolate->m_perfObservers.end()) {
        for (auto& observer : pos->second)
            observer->emit(mark);
    }
    isolate->m_perfLock.unlock();

    return 0;
}

result_t performance_base::measure(exlib::string name, exlib::string startMark, exlib::string endMark)
{
    Isolate* isolate = Isolate::current();
    result_t hr;

    isolate->m_perfLock.lock();

    double startTime;
    if (startMark.empty())
        startTime = 0;
    else {
        auto pos = isolate->m_markEntries.find(startMark);
        if (pos == isolate->m_markEntries.end()) {
            isolate->m_perfLock.unlock();
            return Runtime::setError("startMark '" + startMark + "' not found");
        }

        pos->second->get_startTime(startTime);
    }

    double endTime;
    if (endMark.empty())
        endTime = perf_now();
    else {
        auto pos = isolate->m_markEntries.find(endMark);
        if (pos == isolate->m_markEntries.end()) {
            isolate->m_perfLock.unlock();
            return Runtime::setError("endMark '" + endMark + "' not found");
        }

        pos->second->get_startTime(endTime);
    }

    obj_ptr<PerformanceMeasure> measure = new PerformanceMeasure(name, startTime, endTime - startTime);

    auto pos = isolate->m_perfObservers.find("measure");
    if (pos != isolate->m_perfObservers.end()) {
        for (auto& observer : pos->second)
            observer->emit(measure);
    }

    isolate->m_perfLock.unlock();

    return 0;
}

result_t performance_base::measure(exlib::string name, v8::Local<v8::Object> options)
{
    Isolate* isolate = Isolate::current();
    result_t hr;

    v8::Local<v8::Value> detail;
    GetConfigValue(isolate, options, "detail", detail);

    double startTime = 0;
    exlib::string startMark;
    v8::Local<v8::Value> start;
    hr = GetConfigValue(isolate, options, "start", start);
    if (hr >= 0) {
        if (start->IsString()) {
            GetArgumentValue(isolate, start, startMark, true);
        } else if (start->IsNumber()) {
            startTime = start->NumberValue(isolate->context()).FromMaybe(0);
        } else
            return Runtime::setError("start must be a string or number");
    }

    double durationTime = 0;
    hr = GetConfigValue(isolate, options, "duration", durationTime, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    double endTime;
    exlib::string endMark;
    if (hr >= 0) {
        endTime = startTime + durationTime;
        if (options->Has(isolate->context(), isolate->NewString("end")).FromMaybe(false))
            return Runtime::setError("end must not be specified when duration is specified");
    } else {
        v8::Local<v8::Value> end;
        hr = GetConfigValue(isolate, options, "end", end);
        if (hr >= 0) {
            if (end->IsString()) {
                GetArgumentValue(isolate, end, endMark, true);
                if (endMark.empty())
                    endTime = perf_now();
            } else if (end->IsNumber()) {
                endTime = end->NumberValue(isolate->context()).FromMaybe(0);
            } else
                return Runtime::setError("end must be a string or number");
        } else
            endTime = perf_now();
    }

    isolate->m_perfLock.lock();

    if (!startMark.empty()) {
        auto pos = isolate->m_markEntries.find(startMark);
        if (pos == isolate->m_markEntries.end()) {
            isolate->m_perfLock.unlock();
            return Runtime::setError("startMark '" + startMark + "' not found");
        }

        pos->second->get_startTime(startTime);
    }

    if (!endMark.empty()) {
        auto pos = isolate->m_markEntries.find(endMark);
        if (pos == isolate->m_markEntries.end()) {
            isolate->m_perfLock.unlock();
            return Runtime::setError("endMark '" + endMark + "' not found");
        }

        pos->second->get_startTime(endTime);
    }

    obj_ptr<PerformanceMeasure> measure = new PerformanceMeasure(name, startTime, endTime - startTime, detail);

    auto pos = isolate->m_perfObservers.find("measure");
    if (pos != isolate->m_perfObservers.end()) {
        for (auto& observer : pos->second)
            observer->emit(measure);
    }

    isolate->m_perfLock.unlock();

    return 0;
}

result_t performance_base::getEntries(obj_ptr<NArray>& retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = new NArray();

    isolate->m_perfLock.lock();
    for (auto& entry : isolate->m_markEntries)
        retVal->append(entry.second);
    isolate->m_perfLock.unlock();

    return 0;
}

result_t performance_base::getEntriesByType(exlib::string type, obj_ptr<NArray>& retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = new NArray();

    isolate->m_perfLock.lock();
    for (auto& entry : isolate->m_markEntries) {
        exlib::string _type;
        entry.second->get_entryType(_type);
        if (_type == type)
            retVal->append(entry.second);
    }
    isolate->m_perfLock.unlock();

    return 0;
}

result_t performance_base::getEntriesByName(exlib::string name, exlib::string type, obj_ptr<NArray>& retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = new NArray();

    isolate->m_perfLock.lock();
    for (auto& entry : isolate->m_markEntries) {
        exlib::string _name;
        entry.second->get_name(_name);
        if (_name == name) {
            if (type.empty()) {
                retVal->append(entry.second);
                continue;
            }

            exlib::string _type;
            entry.second->get_entryType(_type);
            if (_type == type)
                retVal->append(entry.second);
        }
    }
    isolate->m_perfLock.unlock();

    return 0;
}

result_t performance_base::now(double& retVal)
{
    retVal = perf_now();
    return 0;
}

}
