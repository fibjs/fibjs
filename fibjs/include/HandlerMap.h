/*
 * HandlerMap.h
 *
 * The recursive C++ value of the IDL HandlerMap. Keys are route patterns and
 * values are one of the handler forms the mq layer accepts. std::shared_ptr is
 * used only for the nested-map branch, so the variant can name an incomplete
 * HandlerMap while the class is being defined.
 */

#pragma once

#include <memory>
#include <variant>

#include <v8.h>

#include "OrderedMap.h"
#include "obj_ptr.h"
#include "result.h"

namespace fibjs {

class Handler_base;
class HandlerMap;

// EventEmitter is part of object.h, before the generated MapTypes.h is
// included, so its typed listener map lives in this core header.
using EventMap = OrderedMap<v8::Local<v8::Function>>;

using HandlerMapValue = std::variant<
    obj_ptr<Handler_base>,
    v8::Local<v8::Array>,
    std::shared_ptr<HandlerMap>,
    exlib::string,
    v8::Local<v8::Function>>;

class HandlerMap : public OrderedMap<HandlerMapValue> {
public:
    HandlerMap() = default;
};

// Implemented in src/base/HandlerMap.cpp; Handler_base must be complete there.
result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v,
    HandlerMapValue& vr, bool bStrict = false);
v8::Local<v8::Value> GetReturnValue(Isolate* isolate, HandlerMapValue& value);

} // namespace fibjs
