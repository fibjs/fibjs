/*
 * encoding_bson.h
 *
 *  Created on: Oct 30, 2012
 *      Author: lion
 */

#include "utils.h"
#include <mongo/bson.h>

#ifndef ENCODING_BSON_H_
#define ENCODING_BSON_H_

namespace fibjs
{

bool appendObject(bson *bb, v8::Handle<v8::Value> element);
result_t encodeObject(bson *bb, v8::Handle<v8::Value> element);
void encodeValue(bson *bb, const char *name, v8::Handle<v8::Value> element);
v8::Handle<v8::Object> decodeObject(const bson *bb);
v8::Handle<v8::Object> decodeObject(const char *buffer);

}

#endif /* ENCODING_BSON_H_ */
