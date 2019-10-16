/*
 * v8_api.h
 *
 *  Created on: Oct 27, 2017
 *      Author: lion
 */

#ifndef _V8_API_H_
#define _V8_API_H_

namespace fibjs {

v8::Local<v8::BigInt> BigInt_New(v8::Isolate* isolate, uint64_t value);
int64_t BigInt_AsInt64(v8::Isolate* isolate, v8::Local<v8::BigInt> value, bool* lossless = NULL);
uint64_t BigInt_AsUint64(v8::Isolate* isolate, v8::Local<v8::BigInt> value, bool* lossless = NULL);

v8::Local<v8::String> JSON_Stringify(v8::Isolate* isolate,
    v8::Local<v8::Value> json_object, v8::Local<v8::Function> json_replacer);

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

bool isFrozen(v8::Handle<v8::Object> object);

} /* namespace fibjs */

#endif /* _V8_API_H_ */
