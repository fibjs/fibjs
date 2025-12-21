/*
 * serialize.cpp
 *
 *  Created on: Dec 21, 2025
 *      Author: lion
 */

#include "object.h"
#include "ifs/v8.h"
#include "Buffer.h"

namespace fibjs {

result_t v8_base::serialize(v8::Local<v8::Value> value, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::ValueSerializer serializer(isolate->m_isolate);

    serializer.WriteHeader();

    v8::Maybe<bool> result = serializer.WriteValue(context, value);
    if (result.IsNothing() || !result.FromJust())
        return CALL_E_JAVASCRIPT;

    std::pair<uint8_t*, size_t> buffer = serializer.Release();

    retVal = new Buffer((const char*)buffer.first, buffer.second);
    free(buffer.first);

    return 0;
}

result_t v8_base::deserialize(Buffer_base* data, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string buf;
    data->toString(buf);

    v8::ValueDeserializer deserializer(isolate->m_isolate,
        (const uint8_t*)buf.c_str(), buf.length());

    v8::Maybe<bool> headerResult = deserializer.ReadHeader(context);
    if (headerResult.IsNothing() || !headerResult.FromJust())
        return CALL_E_JAVASCRIPT;

    v8::MaybeLocal<v8::Value> result = deserializer.ReadValue(context);
    if (result.IsEmpty())
        return CALL_E_JAVASCRIPT;

    retVal = result.ToLocalChecked();

    return 0;
}

}
