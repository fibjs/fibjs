/*
 * PerformanceObserver.h
 *
 *  Created on: Dec 15, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/PerformanceObserver.h"
#include "ifs/PerformanceMark.h"
#include "ifs/PerformanceMeasure.h"
#include "ifs/PerformanceObserverEntryList.h"

namespace fibjs {

class PerformanceObserver : public PerformanceObserver_base {
public:
    // PerformanceObserver_base
    virtual result_t observe(v8::Local<v8::Object> options);
    virtual result_t disconnect();
    virtual result_t takeRecords(obj_ptr<NArray>& retVal);

public:
    void emit(obj_ptr<PerformanceEntry_base> entry);

public:
    v8::Global<v8::Function> m_callback;

private:
    std::atomic<bool> m_running = false;
    std::vector<exlib::string> m_entryTypes;
    std::vector<obj_ptr<PerformanceEntry_base>> m_entries;
};

class PerformanceMark : public PerformanceMark_base {
public:
    PerformanceMark(exlib::string name, double startTime, v8::Local<v8::Value> detail)
        : m_name(name)
        , m_startTime(startTime)
        , m_detail(holder()->m_isolate, detail)
    {
        holder();
    }

    PerformanceMark(exlib::string name, double startTime)
        : m_name(name)
        , m_startTime(startTime)
    {
    }

public:
    // PerformanceEntry_base
    virtual result_t get_name(exlib::string& retVal)
    {
        retVal = m_name;
        return 0;
    }

    virtual result_t get_entryType(exlib::string& retVal)
    {
        retVal = "mark";
        return 0;
    }

    virtual result_t get_startTime(double& retVal)
    {
        retVal = m_startTime;
        return 0;
    }

    virtual result_t get_duration(double& retVal)
    {
        retVal = 0;
        return 0;
    }

public:
    // PerformanceMark_base
    virtual result_t get_detail(v8::Local<v8::Value>& retVal)
    {
        retVal = v8::Local<v8::Value>::New(holder()->m_isolate, m_detail);
        return 0;
    }

private:
    exlib::string m_name;
    double m_startTime;
    v8::Global<v8::Value> m_detail;
};

class PerformanceMeasure : public PerformanceMeasure_base {
public:
    PerformanceMeasure(exlib::string name, double startTime, double duration, v8::Local<v8::Value> detail)
        : m_name(name)
        , m_startTime(startTime)
        , m_duration(duration)
        , m_detail(holder()->m_isolate, detail)
    {
        holder();
    }

    PerformanceMeasure(exlib::string name, double startTime, double duration)
        : m_name(name)
        , m_startTime(startTime)
        , m_duration(duration)
    {
    }

public:
    // PerformanceEntry_base
    virtual result_t get_name(exlib::string& retVal)
    {
        retVal = m_name;
        return 0;
    }

    virtual result_t get_entryType(exlib::string& retVal)
    {
        retVal = "measure";
        return 0;
    }

    virtual result_t get_startTime(double& retVal)
    {
        retVal = m_startTime;
        return 0;
    }

    virtual result_t get_duration(double& retVal)
    {
        retVal = m_duration;
        return 0;
    }

public:
    // PerformanceMark_base
    virtual result_t get_detail(v8::Local<v8::Value>& retVal)
    {
        retVal = v8::Local<v8::Value>::New(holder()->m_isolate, m_detail);
        return 0;
    }

private:
    exlib::string m_name;
    double m_startTime;
    double m_duration;
    v8::Global<v8::Value> m_detail;
};

class PerformanceObserverEntryList : public PerformanceObserverEntryList_base {
public:
    PerformanceObserverEntryList(std::vector<obj_ptr<PerformanceEntry_base>>& entries)
        : m_entries(std::move(entries))
    {
    }

public:
    // PerformanceObserverEntryList_base
    virtual result_t getEntries(obj_ptr<NArray>& retVal);
    virtual result_t getEntriesByName(exlib::string name, exlib::string entryType, obj_ptr<NArray>& retVal);
    virtual result_t getEntriesByType(exlib::string entryType, obj_ptr<NArray>& retVal);

private:
    std::vector<obj_ptr<PerformanceEntry_base>> m_entries;
};

}
