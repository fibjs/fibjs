/**
 * @file util.h
 * @author PinelliaW (wylbzc4928@gmail.com)
 * @brief
 * @date 2023-10-12 14:56:16
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "object.h"
#include "ifs/util.h"
#include "utf8.h"
#include "qstring.h"
#include "QuickArray.h"
#include "StringBuffer.h"
#include "TextColor.h"
#include "Buffer.h"
#include <unordered_map>
#include <unicode/include/unicode/uchar.h>

namespace fibjs {

#define DEFAULT_DEPTH 2
#define DEFAULT_MAX_ARRAY_LENGTH 100

void string_format(StringBuffer& strBuffer, v8::Local<v8::Value> v, bool color);
exlib::string json_format(v8::Local<v8::Value> obj, bool color, int32_t depth, int32_t maxArrayLength);
exlib::string table_format(v8::Local<v8::Value> obj, v8::Local<v8::Array> fields, bool color, bool encode_string);
result_t util_format(exlib::string fmt, OptArgs args, bool color, exlib::string& retVal);

}