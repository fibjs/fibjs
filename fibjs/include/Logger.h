/*
 * Logger.h
 *
 *  Created on: Sep 25, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/Logger.h"

namespace fibjs {

class Logger : public Logger_base {
public:
    Logger() { }
    Logger(exlib::string section)
        : m_section(section)
    {
    }

    Logger(exlib::string section, v8::Local<v8::Function> fn)
        : m_section(section)
    {
        m_fn.Reset(fn->GetIsolate(), fn);
    }

public:
    // Logger_base
    virtual result_t _function(exlib::string fmt, OptArgs args);
    virtual result_t _function(OptArgs args);
    virtual result_t get_section(exlib::string& retVal);
    virtual result_t get_enabled(bool& retVal);
    virtual result_t log(exlib::string fmt, OptArgs args);
    virtual result_t log(OptArgs args);
    virtual result_t debug(exlib::string fmt, OptArgs args);
    virtual result_t debug(OptArgs args);
    virtual result_t info(exlib::string fmt, OptArgs args);
    virtual result_t info(OptArgs args);
    virtual result_t notice(exlib::string fmt, OptArgs args);
    virtual result_t notice(OptArgs args);
    virtual result_t warn(exlib::string fmt, OptArgs args);
    virtual result_t warn(OptArgs args);
    virtual result_t warning(exlib::string fmt, OptArgs args);
    virtual result_t warning(OptArgs args);
    virtual result_t error(exlib::string fmt, OptArgs args);
    virtual result_t error(OptArgs args);
    virtual result_t crit(exlib::string fmt, OptArgs args);
    virtual result_t crit(OptArgs args);
    virtual result_t critical(exlib::string fmt, OptArgs args);
    virtual result_t critical(OptArgs args);
    virtual result_t alert(exlib::string fmt, OptArgs args);
    virtual result_t alert(OptArgs args);
    virtual result_t trace(exlib::string fmt, OptArgs args);
    virtual result_t trace(OptArgs args);
    virtual result_t dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options);

private:
    bool check_env();
    bool first_call();
    void fill_prefix();
    void _log(int32_t type, exlib::string fmt, OptArgs args);

private:
    exlib::string m_section;
    exlib::string m_prefix;
    exlib::string m_prefix_color;
    v8::Global<v8::Function> m_fn;
    bool m_first = true;
};
}
