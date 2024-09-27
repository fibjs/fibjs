/*
 * Script.h
 *
 *  Created on: Oct 15, 2023
 *      Author: lion
 */

#pragma once

#include "ifs/Script.h"

namespace fibjs {

class Script : public Script_base {

public:
    // Script_base
    virtual result_t runInContext(v8::Local<v8::Object> contextifiedObject, v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal);
    virtual result_t runInNewContext(v8::Local<v8::Object> contextObject, v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal);
    virtual result_t runInThisContext(v8::Local<v8::Object> opts, v8::Local<v8::Value>& retVal);
    virtual result_t createCachedData(obj_ptr<Buffer_base>& retVal);

public:
    result_t init(exlib::string code, v8::Local<v8::Object> opts);

private:
    v8::Global<v8::UnboundScript> m_script;
};

}
