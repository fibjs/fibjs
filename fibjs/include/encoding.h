/*
 * encoding_conv.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#pragma once

#include "Buffer.h"
#include "utf8.h"
#include "ifs/encoding.h"
#include "encoding_conv.h"

namespace fibjs {

void base64Encode(const char* data, size_t sz, bool url, exlib::string& retVal);
void base64Decode(const char* data, size_t sz, exlib::string& retVal);

// 直接解码到调用方给的缓冲（cap 字节），返回实际写入的字节数。
// hex/base64 的零绕行入口用它把结果写进新建 Buffer 的 backing store。
//   hex:     遇非法字符即停（node 语义），不写半字节
//   base64:  宽容模式 —— 跳过非法字符、'=' 终止、两种字母表都接受
size_t hexDecodeTo(const char* data, size_t sz, char* dst, size_t cap);
size_t base64DecodeTo(const char* data, size_t sz, char* dst, size_t cap);

// base58 的"不校验内容"长度计算，供 Buffer.byteLength 使用。
// 与 base58Decode 的区别：字母表外的字符被**过滤**而不是报错
// （对齐 node 对 hex/base64 的 byteLength 语义：只算长度，不判合法性）。
size_t base58ByteLength(const char* data, size_t sz);

/**
 * hex & base64 use encode: binary -> hex ,binary -> base64
 *
 * encoding_conv use decode: local encoding string -> unicode
 *
 */
result_t commonEncode(exlib::string codec, const char* data, size_t sz, exlib::string& retVal);
result_t commonEncode(exlib::string codec, exlib::string data, exlib::string& retVal);

result_t commonDecode(exlib::string codec, exlib::string data, exlib::string& retVal);

} /* namespace fibjs */
