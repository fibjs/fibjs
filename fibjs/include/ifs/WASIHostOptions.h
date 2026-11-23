/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "../OrderedMap.h"
#include "ifs/MapTypes.h"
#include <boost/preprocessor.hpp>
#include "ifs/WASISocketOption.h"

namespace fibjs {

class WASIHostOptions : public obj_base {
public:
    enum { kFibjsOptions = 1 };
    static WASIHostOptions* getInstance(v8::Local<v8::Value> v) { return nullptr; }
    static result_t load(v8::Local<v8::Value> v, obj_ptr<WASIHostOptions>& retVal)
    {
        if (!IsJSObject(v))
            return result_t::type_mismatch();
        v8::Local<v8::Object> opt = v.As<v8::Object>();
        obj_ptr<WASIHostOptions> o = new WASIHostOptions();
        result_t hr;
        hr = GetConfigValue(opt, "version", o->version, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "args", o->args, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "env", o->env, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "preopens", o->preopens, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "sockets", o->sockets, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "stdin", o->stdin_, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "stdout", o->stdout_, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "stderr", o->stderr_, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        hr = GetConfigValue(opt, "returnOnExit", o->returnOnExit, true);
        if (hr.failed())
            return errhelp::from_options_record(hr);
        retVal = o;
        return result_t();
    }

public:
    exlib::string version{};
    std::optional<std::vector<v8::Local<v8::Value>>> args;
    std::optional<ValueMap> env;
    std::optional<StringMap> preopens;
    std::optional<std::vector<obj_ptr<WASISocketOption>>> sockets;
    std::optional<int32_t> stdin_ = 0;
    std::optional<int32_t> stdout_ = 1;
    std::optional<int32_t> stderr_ = 2;
    std::optional<bool> returnOnExit = true;
};
}
