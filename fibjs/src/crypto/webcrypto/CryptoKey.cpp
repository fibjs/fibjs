/*
 * CryptoKey.cpp
 *
 *  Created on: Aug 25, 2024
 *      Author: lion
 */

#include "object.h"
#include "CryptoKey.h"

namespace fibjs {

result_t CryptoKey::get_type(exlib::string& retVal)
{
    return m_key->get_type(retVal);
}

result_t CryptoKey::get_algorithm(v8::Local<v8::Object>& retVal)
{
    return m_algorithm->valueOf(retVal);
}

result_t CryptoKey::get_extractable(bool& retVal)
{
    retVal = m_extractable;
    return 0;
}

result_t CryptoKey::get_usages(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, m_usageMap.size());

    int32_t cnt = 0;
    for (auto& it : m_usageMap)
        arr->Set(isolate->m_isolate->GetCurrentContext(), cnt++, v8::String::NewFromUtf8(isolate->m_isolate, it.first.c_str(), v8::NewStringType::kNormal).ToLocalChecked());

    retVal = arr;

    return 0;
}

}
