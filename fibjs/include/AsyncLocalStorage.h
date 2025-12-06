/*
 * AsyncLocalStorage.h
 *
 *  Created on: Dec 2, 2025
 *      Author: lion
 */

#include "ifs/AsyncLocalStorage.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

class ALSOptions : public obj_base {
public:
    LOAD_OPTIONS(ALSOptions, (defaultValue)(name));

public:
    std::optional<v8::Local<v8::Value>> defaultValue;
    std::optional<v8::Local<v8::Value>> name;
};

class AsyncLocalStorage : public AsyncLocalStorage_base {
public:
    // AsyncLocalStorage_base
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t disable();
    virtual result_t getStore(v8::Local<v8::Value>& retVal);
    virtual result_t enterWith(v8::Local<v8::Value> store);
    virtual result_t run(v8::Local<v8::Value> store, v8::Local<v8::Function> callback, OptArgs args, v8::Local<v8::Value>& retVal);
    virtual result_t exit(v8::Local<v8::Function> callback, OptArgs args, v8::Local<v8::Value>& retVal);

public:
    int32_t m_id;
    bool m_disabled = false;
    v8::Global<v8::Value> m_defaultValue;
    exlib::string m_name;
};

}
