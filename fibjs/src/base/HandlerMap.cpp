/*
 * HandlerMap.cpp
 *
 * JS <-> C++ conversion of the recursive HandlerMap value. Keeping this out of
 * utils.h is deliberate: Handler_base is a generated interface and utils.h is
 * included before it, so the conversion that calls Handler_base::load() lives
 * in a translation unit that can see the complete class.
 */

#include "object.h"
#include "HandlerMap.h"
#include "ifs/Handler.h"

namespace fibjs {

result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v,
    HandlerMapValue& vr, bool bStrict)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    // A native Handler (including Routing and Chain) is the first and most
    // specific alternative. A plain object is a nested HandlerMap below.
    obj_ptr<Handler_base> handler = Handler_base::getInstance(v);
    if (handler) {
        vr = handler;
        return result_t();
    }

    if (v->IsArray()) {
        vr = v.As<v8::Array>();
        return result_t();
    }

    if (v->IsString() || v->IsStringObject()) {
        exlib::string path;
        result_t hr = GetArgumentValue(isolate, v, path, bStrict);
        if (hr.failed())
            return hr;
        vr = path;
        return result_t();
    }

    if (v->IsFunction()) {
        vr = v.As<v8::Function>();
        return result_t();
    }

    if (IsJSObject(v)) {
        std::shared_ptr<HandlerMap> map = std::make_shared<HandlerMap>();
        result_t hr = GetArgumentValue(isolate, v, *map, bStrict);
        if (hr.failed())
            return hr;
        vr = map;
        return result_t();
    }

    return result_t::type_mismatch();
}

v8::Local<v8::Value> GetReturnValue(Isolate* isolate, HandlerMapValue& value)
{
    return std::visit([isolate](auto& item) -> v8::Local<v8::Value> {
        using T = std::decay_t<decltype(item)>;

        if constexpr (std::is_same_v<T, obj_ptr<Handler_base>>)
            return GetReturnValue(isolate, item);
        else if constexpr (std::is_same_v<T, v8::Local<v8::Array>>)
            return item;
        else if constexpr (std::is_same_v<T, std::shared_ptr<HandlerMap>>)
            return GetReturnValue(isolate, static_cast<OrderedMap<HandlerMapValue>&>(*item));
        else if constexpr (std::is_same_v<T, exlib::string>)
            return isolate->NewString(item);
        else if constexpr (std::is_same_v<T, v8::Local<v8::Function>>)
            return item;
        else
            return v8::Local<v8::Value>();
    }, value);
}

} // namespace fibjs
