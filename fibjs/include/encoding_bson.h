/*
 * encoding_bson.h
 *
 *  Created on: Oct 30, 2012
 *      Author: lion
 */

#include "utils.h"
#include <mongo/include/bson.h>

#ifndef ENCODING_BSON_H_
#define ENCODING_BSON_H_

namespace fibjs {

bool appendObject(Isolate* isolate, bson* bb, v8::Local<v8::Value> element);
result_t encodeObject(Isolate* isolate, bson* bb, v8::Local<v8::Value> element);
void encodeValue(Isolate* isolate, bson* bb, const char* name, v8::Local<v8::Value> element);
v8::Local<v8::Object> decodeObject(Isolate* isolate, const bson* bb);
v8::Local<v8::Object> decodeObject(Isolate* isolate, const char* buffer);
}

#endif /* ENCODING_BSON_H_ */
