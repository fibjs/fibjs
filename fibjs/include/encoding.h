/*
 * encoding_iconv.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#pragma once

#include "Buffer.h"
#include "utf8.h"
#include "ifs/encoding.h"
#include "encoding_iconv.h"

namespace fibjs {

result_t base64Encode(const char* data, size_t sz, bool url, exlib::string& retVal);
result_t base64Decode(exlib::string data, exlib::string& retVal);

/**
 * hex & base64 use encode: binary -> hex ,binary -> base64
 *
 * iconv use decode: local encoding string -> unicode
 *
 */
result_t commonEncode(exlib::string codec, exlib::string data, exlib::string& retVal);

} /* namespace fibjs */
