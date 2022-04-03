/*
 * statistics.cpp
 *
 *  Created on: Apr 4, 2022
 *      Author: lion
 */

#include "object.h"
#include "ifs/profiler.h"

namespace fibjs {

result_t profiler_base::getHeapCodeStatistics(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::HeapCodeStatistics hcs;

    if (!isolate->m_isolate->GetHeapCodeAndMetadataStatistics(&hcs))
        return CHECK_ERROR(CALL_E_INTERNAL);

    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

    o->Set(context, isolate->NewString("code_and_metadata_size"), v8::Number::New(isolate->m_isolate, hcs.code_and_metadata_size()));
    o->Set(context, isolate->NewString("bytecode_and_metadata_size"), v8::Number::New(isolate->m_isolate, hcs.bytecode_and_metadata_size()));
    o->Set(context, isolate->NewString("external_script_source_size"), v8::Number::New(isolate->m_isolate, hcs.external_script_source_size()));

    retVal = o;

    return 0;
}

result_t profiler_base::getHeapSpaceStatistics(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();

    size_t sz = isolate->m_isolate->NumberOfHeapSpaces();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate);

    for (size_t i = 0; i < sz; i++) {
        v8::HeapSpaceStatistics hss;

        if (!isolate->m_isolate->GetHeapSpaceStatistics(&hss, i))
            return CHECK_ERROR(CALL_E_INTERNAL);

        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

        o->Set(context, isolate->NewString("space_name"), isolate->NewString(hss.space_name()));
        o->Set(context, isolate->NewString("space_size"), v8::Number::New(isolate->m_isolate, hss.space_size()));
        o->Set(context, isolate->NewString("space_used_size"), v8::Number::New(isolate->m_isolate, hss.space_used_size()));
        o->Set(context, isolate->NewString("space_available_size"), v8::Number::New(isolate->m_isolate, hss.space_available_size()));
        o->Set(context, isolate->NewString("physical_space_size"), v8::Number::New(isolate->m_isolate, hss.physical_space_size()));

        a->Set(context, i, o);
    }

    retVal = a;

    return 0;
}

result_t profiler_base::getHeapStatistics(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::HeapStatistics hs;

    isolate->m_isolate->GetHeapStatistics(&hs);

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

    o->Set(context, isolate->NewString("total_heap_size"), v8::Number::New(isolate->m_isolate, hs.total_heap_size()));
    o->Set(context, isolate->NewString("total_heap_size_executable"), v8::Number::New(isolate->m_isolate, hs.total_heap_size_executable()));
    o->Set(context, isolate->NewString("total_physical_size"), v8::Number::New(isolate->m_isolate, hs.total_physical_size()));
    o->Set(context, isolate->NewString("total_available_size"), v8::Number::New(isolate->m_isolate, hs.total_available_size()));
    o->Set(context, isolate->NewString("used_heap_size"), v8::Number::New(isolate->m_isolate, hs.used_heap_size()));
    o->Set(context, isolate->NewString("heap_size_limit"), v8::Number::New(isolate->m_isolate, hs.heap_size_limit()));
    o->Set(context, isolate->NewString("malloced_memory"), v8::Number::New(isolate->m_isolate, hs.malloced_memory()));
    o->Set(context, isolate->NewString("external_memory"), v8::Number::New(isolate->m_isolate, hs.external_memory()));
    o->Set(context, isolate->NewString("peak_malloced_memory"), v8::Number::New(isolate->m_isolate, hs.peak_malloced_memory()));
    o->Set(context, isolate->NewString("number_of_native_contexts"), v8::Number::New(isolate->m_isolate, hs.number_of_native_contexts()));
    o->Set(context, isolate->NewString("number_of_detached_contexts"), v8::Number::New(isolate->m_isolate, hs.number_of_detached_contexts()));

    retVal = o;

    return 0;
}

}
