/*
 * v8_api.h
 *
 *  Created on: Oct 27, 2017
 *      Author: lion
 */

#ifndef _V8_API_H_
#define _V8_API_H_

namespace fibjs {

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

} /* namespace fibjs */

#endif /* _V8_API_H_ */
