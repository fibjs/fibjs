/*
 * v8_api.h
 *
 *  Created on: Oct 27, 2017
 *      Author: lion
 */

#pragma once

namespace fibjs {

intptr_t RunMicrotaskSize(v8::Isolate* isolate);

void InvokeApiInterruptCallbacks(v8::Isolate* isolate);

struct V8FrameInfo {
    void* entry_fp;
    void* handle;
};

V8FrameInfo save_fi(v8::Isolate* isolate);

exlib::string traceInfo(v8::Isolate* isolate, int32_t deep, void* entry_fp, void* handle);
exlib::string traceInfo(v8::Isolate* isolate, int32_t deep);

void beginCoverage(v8::Isolate* isolate);
void pauseCoverage(v8::Isolate* isolate);
void WriteLcovData(v8::Isolate* isolate, FILE* file);

bool isFrozen(v8::Local<v8::Object> object);
void setAsyncFunctoin(v8::Local<v8::Function> func);

void* fetch_store_data(std::shared_ptr<v8::BackingStore> backing_store, v8::BackingStore::DeleterCallback deleter = NULL);

} /* namespace fibjs */
