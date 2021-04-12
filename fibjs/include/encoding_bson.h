/*
 * encoding_bson.h
 *
 *  Created on: Oct 30, 2012
 *      Author: lion
 */

#pragma once

#include "utils.h"
#include <mongo/include/bson.h>

namespace fibjs {

bool appendObject(Isolate* isolate, bson* bb, v8::Local<v8::Value> element);
result_t encodeObject(Isolate* isolate, bson* bb, v8::Local<v8::Value> element);
void encodeValue(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element);
result_t decodeObject(Isolate* isolate, const bson* bb, v8::Local<v8::Object>& retVal);
result_t decodeObject(Isolate* isolate, const char* buffer, v8::Local<v8::Object>& retVal);
}
