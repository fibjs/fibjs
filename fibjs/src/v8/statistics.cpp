/*
 * statistics.cpp
 *
 *  Created on: Apr 4, 2022
 *      Author: lion
 */

#include "object.h"
#include "ifs/v8.h"

namespace fibjs {

result_t v8_base::getHeapCodeStatistics(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::HeapCodeStatistics hcs;

    if (!isolate->m_isolate->GetHeapCodeAndMetadataStatistics(&hcs))
        return CHECK_ERROR(CALL_E_INTERNAL);

    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

    o->Set(context, isolate->NewString("code_and_metadata_size"), v8::Number::New(isolate->m_isolate, (double)hcs.code_and_metadata_size())).IsJust();
    o->Set(context, isolate->NewString("bytecode_and_metadata_size"), v8::Number::New(isolate->m_isolate, (double)hcs.bytecode_and_metadata_size())).IsJust();
    o->Set(context, isolate->NewString("external_script_source_size"), v8::Number::New(isolate->m_isolate, (double)hcs.external_script_source_size())).IsJust();

    retVal = o;

    return 0;
}

result_t v8_base::getHeapSpaceStatistics(std::vector<obj_ptr<GetHeapSpaceStatisticsType>>& retVal)
{
    Isolate* isolate = Isolate::current();

    size_t sz = isolate->m_isolate->NumberOfHeapSpaces();

    for (size_t i = 0; i < sz; i++) {
        v8::HeapSpaceStatistics hss;

        if (!isolate->m_isolate->GetHeapSpaceStatistics(&hss, i))
            return CHECK_ERROR(CALL_E_INTERNAL);

        obj_ptr<GetHeapSpaceStatisticsType> item = new GetHeapSpaceStatisticsType();

        item->space_name = hss.space_name();
        item->space_size = (double)hss.space_size();
        item->space_used_size = (double)hss.space_used_size();
        item->space_available_size = (double)hss.space_available_size();
        item->physical_space_size = (double)hss.physical_space_size();

        retVal.push_back(item);
    }

    return 0;
}

result_t v8_base::getHeapStatistics(obj_ptr<GetHeapStatisticsType>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::HeapStatistics hs;

    isolate->m_isolate->GetHeapStatistics(&hs);

    retVal = new GetHeapStatisticsType();

    retVal->total_heap_size = (double)hs.total_heap_size();
    retVal->total_heap_size_executable = (double)hs.total_heap_size_executable();
    retVal->total_physical_size = (double)hs.total_physical_size();
    retVal->total_available_size = (double)hs.total_available_size();
    retVal->used_heap_size = (double)hs.used_heap_size();
    retVal->heap_size_limit = (double)hs.heap_size_limit();
    retVal->malloced_memory = (double)hs.malloced_memory();
    retVal->external_memory = (double)hs.external_memory();
    retVal->peak_malloced_memory = (double)hs.peak_malloced_memory();
    retVal->number_of_native_contexts = (double)hs.number_of_native_contexts();
    retVal->number_of_detached_contexts = (double)hs.number_of_detached_contexts();

    return 0;
}

}
