/*
 * URLSearchParams.cpp
 *
 *  Created on: Jul 14, 2025
 *      Author: lion
 */

#include "object.h"
#include "URLSearchParams.h"
#include "ifs/encoding.h"
#include "StringBuffer.h"
#include <algorithm>

namespace fibjs {

result_t URLSearchParams_base::_new(obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new URLSearchParams();
    return 0;
}

result_t URLSearchParams_base::_new(exlib::string init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<URLSearchParams> params = new URLSearchParams();
    retVal = params;
    const char* p = init.c_str();
    if (*p == '?')
        init = init.substr(1);
    return params->parse(init);
}

result_t URLSearchParams_base::_new(v8::Local<v8::Object> init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new URLSearchParams();
    return retVal->append(init);
}

result_t URLSearchParams_base::_new(v8::Local<v8::Array> init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new URLSearchParams();
    return retVal->append(init);
}

result_t URLSearchParams_base::_new(URLSearchParams_base* init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<URLSearchParams> headers = new URLSearchParams();
    retVal = headers;
    return headers->init(init);
}

result_t URLSearchParams_base::_new(Variant init, obj_ptr<URLSearchParams_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    obj_ptr<URLSearchParams> params = new URLSearchParams();
    retVal = params;

    v8::Local<v8::Value> v = init;

    // WebIDL: the init argument may be a sequence of pairs. Any JS iterable
    // (FormData, Map, Set, another URLSearchParams ...) is materialized through
    // Array.from and then consumed as [[name, value], ...].
    if (v->IsObject()) {
        v8::Local<v8::Object> o = v.As<v8::Object>();
        v8::Local<v8::Value> iterFn;

        if (o->Get(context, v8::Symbol::GetIterator(isolate->m_isolate)).ToLocal(&iterFn)
            && iterFn->IsFunction()) {
            v8::Local<v8::Value> arrCtor;

            if (context->Global()->Get(context, isolate->NewString("Array")).ToLocal(&arrCtor)
                && arrCtor->IsObject()) {
                v8::Local<v8::Value> fromFn;

                if (arrCtor.As<v8::Object>()->Get(context, isolate->NewString("from")).ToLocal(&fromFn)
                    && fromFn->IsFunction()) {
                    v8::Local<v8::Value> argv[] = { v };
                    v8::Local<v8::Value> entries;

                    if (!fromFn.As<v8::Function>()->Call(context, arrCtor, 1, argv).ToLocal(&entries))
                        return CALL_E_JAVASCRIPT;

                    if (entries->IsArray()) {
                        result_t hr = params->append(entries.As<v8::Array>());
                        if (hr == CALL_E_BADVARTYPE)
                            return Runtime::setError(kTypeError,
                                "Failed to construct 'URLSearchParams': sequence elements must be pairs.");
                        return hr;
                    }
                }
            }
        }
    }

    // Fallback: treat the value as a query string (historical loose conversion).
    exlib::string str;
    init.toString(str);
    if (!str.empty() && str[0] == '?')
        str = str.substr(1);
    return params->parse(str);
}

result_t URLSearchParams::get_size(int32_t& retVal)
{
    // WHATWG URL: the number of name-value pairs, duplicates included
    retVal = (int32_t)m_map.size();
    return 0;
}

result_t URLSearchParams::toString(exlib::string& retVal)
{
    StringBuffer bufs;

    for (size_t i = 0; i < m_map.size(); i++) {
        if (i > 0)
            bufs.append("&");

        pair& _pair = m_map[i];

        exlib::string encodedKey, encodedValue;
        encoding_base::encodeURIComponent(_pair.first, true, encodedKey);
        encoding_base::encodeURIComponent(_pair.second.string(), true, encodedValue);

        bufs.append(encodedKey);
        bufs.append("=");
        bufs.append(encodedValue);
    }

    retVal = bufs.str();
    return 0;
}

result_t URLSearchParams::has(exlib::string name, v8::Local<v8::Value> value, bool& retVal)
{
    if (value.IsEmpty() || value->IsUndefined())
        return HttpCollectionTmpl<URLSearchParams_base, true>::has(name, retVal);

    exlib::string expected = Isolate::current()->toString(value);

    retVal = false;
    for (size_t i = 0; i < m_map.size(); i++) {
        pair& entry = m_map[i];

        if (entry.first == name && entry.second.string() == expected) {
            retVal = true;
            break;
        }
    }

    return 0;
}

result_t URLSearchParams::_delete(exlib::string name, v8::Local<v8::Value> value)
{
    if (value.IsEmpty() || value->IsUndefined())
        return HttpCollectionTmpl<URLSearchParams_base, true>::_delete(name);

    exlib::string expected = Isolate::current()->toString(value);

    auto it = std::remove_if(m_map.begin(), m_map.end(),
        [&name, &expected](const pair& entry) {
            return entry.first == name && entry.second.string() == expected;
        });
    m_map.erase(it, m_map.end());
    return 0;
}

}