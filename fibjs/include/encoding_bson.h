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

void encodeObject(bson *bb, const v8::Handle<v8::Value> element);

}

#endif /* ENCODING_BSON_H_ */
