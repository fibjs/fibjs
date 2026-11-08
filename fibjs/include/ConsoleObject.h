/*
 * ConsoleObject.h
 *
 *  Created on: Sep 25, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/ConsoleObject.h"
#include <unordered_map>

namespace fibjs {

class ConsoleObject : public ConsoleObject_base {
public:
    ConsoleObject() { }
    ConsoleObject(v8::Isolate* isolate, v8::Local<v8::Object> out, v8::Local<v8::Object> err)
    {
        m_stdout.Reset(isolate, out);
        m_stderr.Reset(isolate, err);
    }
    ConsoleObject(exlib::string section)
        : m_section(section)
    {
    }

    ConsoleObject(exlib::string section, v8::Local<v8::Function> fn)
        : m_section(section)
    {
        m_fn.Reset(fn->GetIsolate(), fn);
    }

public:
    // ConsoleObject_base
    virtual result_t _function(OptArgs args);
    virtual result_t get_section(exlib::string& retVal);
    virtual result_t get_enabled(bool& retVal);
    virtual result_t log(OptArgs args);
    virtual result_t debug(OptArgs args);
    virtual result_t info(OptArgs args);
    virtual result_t notice(OptArgs args);
    virtual result_t warn(OptArgs args);
    virtual result_t warning(OptArgs args);
    virtual result_t error(OptArgs args);
    virtual result_t crit(OptArgs args);
    virtual result_t critical(OptArgs args);
    virtual result_t alert(OptArgs args);
    virtual result_t trace(OptArgs args);
    virtual result_t dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options);
    virtual result_t table(v8::Local<v8::Value> obj);
    virtual result_t table(v8::Local<v8::Value> obj, v8::Local<v8::Array> fields);
    virtual result_t time(exlib::string label);
    virtual result_t timeElapse(exlib::string label);
    virtual result_t timeEnd(exlib::string label);
    virtual result_t timeLog(exlib::string label, OptArgs args);
    virtual result_t count(exlib::string label);
    virtual result_t countReset(exlib::string label);
    virtual result_t group(OptArgs args);
    virtual result_t groupCollapsed(OptArgs args);
    virtual result_t groupEnd();

private:
    bool check_env();
    bool first_call();
    void fill_prefix();
    void _log(int32_t type, OptArgs args);
    void _log(int32_t type, exlib::string str, bool is_color = false);
    void _out(int32_t type, exlib::string& msg);

    // The indent is applied where the prefix is prepended, so every text sink
    // (`_log` in both overloads) gets it and the coloured and plain paths agree.
    void apply_indent(exlib::string& str);

private:
    exlib::string m_section;
    exlib::string m_prefix;
    exlib::string m_prefix_color;
    v8::Global<v8::Function> m_fn;
    bool m_first = true;
    std::unordered_map<exlib::string, int64_t> m_timers;
    // Counters and indentation are per instance: the module-level console keeps
    // its own copies, so two `new Console(stream)` objects never share them.
    std::unordered_map<exlib::string, int32_t> m_counters;
    int32_t m_indent = 0;
    v8::Global<v8::Object> m_stdout;
    v8::Global<v8::Object> m_stderr;
};
}
