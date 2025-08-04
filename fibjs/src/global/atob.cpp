/*
 * atob.cpp
 *
 *  Created on: Aug 5, 2025
 *      Author: fibjs team
 */

#include "object.h"
#include "ifs/global.h"
#include "Runtime.h"

namespace fibjs {

// Node.js compatible UTF-8 to Latin1 conversion and validation
static result_t utf8ToLatin1(const exlib::string& utf8Str, exlib::string& latin1Str)
{
    const unsigned char* src = (const unsigned char*)utf8Str.c_str();
    const unsigned char* end = src + utf8Str.length();

    // Clear and reserve space for output
    latin1Str.clear();
    latin1Str.reserve(utf8Str.length());

    while (src < end) {
        uint32_t codepoint;
        size_t bytesConsumed;

        // Decode UTF-8 sequence to Unicode codepoint
        if (*src <= 0x7F) {
            // 1-byte sequence (ASCII): 0xxxxxxx
            codepoint = *src;
            bytesConsumed = 1;
        } else if ((*src & 0xE0) == 0xC0) {
            // 2-byte sequence: 110xxxxx 10xxxxxx
            if (src + 1 >= end || (src[1] & 0xC0) != 0x80) {
                return CHECK_ERROR(Runtime::setError("Failed to execute 'btoa': Invalid UTF-8 sequence."));
            }
            codepoint = ((*src & 0x1F) << 6) | (src[1] & 0x3F);
            bytesConsumed = 2;
        } else if ((*src & 0xF0) == 0xE0) {
            // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
            if (src + 2 >= end || (src[1] & 0xC0) != 0x80 || (src[2] & 0xC0) != 0x80) {
                return CHECK_ERROR(Runtime::setError("Failed to execute 'btoa': Invalid UTF-8 sequence."));
            }
            codepoint = ((*src & 0x0F) << 12) | ((src[1] & 0x3F) << 6) | (src[2] & 0x3F);
            bytesConsumed = 3;
        } else if ((*src & 0xF8) == 0xF0) {
            // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            if (src + 3 >= end || (src[1] & 0xC0) != 0x80 || (src[2] & 0xC0) != 0x80 || (src[3] & 0xC0) != 0x80) {
                return CHECK_ERROR(Runtime::setError("Failed to execute 'btoa': Invalid UTF-8 sequence."));
            }
            codepoint = ((*src & 0x07) << 18) | ((src[1] & 0x3F) << 12) | ((src[2] & 0x3F) << 6) | (src[3] & 0x3F);
            bytesConsumed = 4;
        } else {
            return CHECK_ERROR(Runtime::setError("Failed to execute 'btoa': Invalid UTF-8 sequence."));
        }

        // Check if codepoint is within Latin1 range (0-255)
        if (codepoint > 255) {
            return CHECK_ERROR(Runtime::setError("Failed to execute 'btoa': The string to be encoded contains characters outside of the Latin1 range."));
        }

        // Convert Unicode codepoint to Latin1 byte using exlib::string
        latin1Str += (char)codepoint;
        src += bytesConsumed;
    }

    return 0;
}

// Node.js compatible Base64 encoding
static void nodeBase64Encode(const char* data, size_t sz, exlib::string& retVal)
{
    static const char* encodingTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (sz == 0) {
        retVal.clear();
        return;
    }

    size_t outputLen = (sz + 2) / 3 * 4;
    retVal.resize(outputLen);
    char* output = retVal.data();

    size_t i = 0, j = 0;

    // Process complete 3-byte groups
    while (i + 2 < sz) {
        uint32_t octet_a = (unsigned char)data[i];
        uint32_t octet_b = (unsigned char)data[i + 1];
        uint32_t octet_c = (unsigned char)data[i + 2];

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j] = encodingTable[(triple >> 18) & 63];
        output[j + 1] = encodingTable[(triple >> 12) & 63];
        output[j + 2] = encodingTable[(triple >> 6) & 63];
        output[j + 3] = encodingTable[triple & 63];

        i += 3;
        j += 4;
    }

    // Handle remaining 1 or 2 bytes
    if (i < sz) {
        uint32_t octet_a = (unsigned char)data[i];
        uint32_t octet_b = (i + 1 < sz) ? (unsigned char)data[i + 1] : 0;
        uint32_t octet_c = 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j] = encodingTable[(triple >> 18) & 63];
        output[j + 1] = encodingTable[(triple >> 12) & 63];

        if (i + 1 < sz) { // We have 2 bytes
            output[j + 2] = encodingTable[(triple >> 6) & 63];
            output[j + 3] = '=';
        } else { // We have only 1 byte
            output[j + 2] = '=';
            output[j + 3] = '=';
        }
        j += 4;
    }

    retVal.resize(j);
}

// Node.js compatible Base64 decoding with validation
static result_t nodeBase64Decode(const std::string& input, std::string& retVal)
{
    static const int decodeTable[128] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0-15
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 16-31
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, // 32-47 (+, -, /)
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, // 48-63 (0-9)
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, // 64-79 (A-O)
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, // 80-95 (P-Z, _)
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 96-111 (a-o)
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 // 112-127 (p-z)
    };

    // Remove whitespace characters
    std::string cleanInput;
    for (char ch : input) {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\f' && ch != '\v') {
            cleanInput += ch;
        }
    }

    const char* data = cleanInput.c_str();
    size_t len = cleanInput.length();

    if (len == 0) {
        retVal.clear();
        return 0;
    }

    // Validate input characters
    for (size_t i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)data[i];
        if (ch >= 128 || (decodeTable[ch] == -1 && ch != '=')) {
            return CHECK_ERROR(Runtime::setError("Failed to execute 'atob': The string to be decoded is not correctly encoded."));
        }
    }

    // Calculate approximate output length
    size_t maxOutputLen = (len * 3) / 4 + 3; // Add some padding for safety
    retVal.resize(maxOutputLen);
    char* output = retVal.data();

    size_t i = 0, j = 0;
    uint32_t accumulator = 0;
    int bitsAccumulated = 0;

    while (i < len) {
        unsigned char ch = (unsigned char)data[i++];

        if (ch == '=') {
            break; // Padding reached
        }

        if (ch >= 128 || decodeTable[ch] == -1) {
            continue; // Skip invalid characters (already validated above)
        }

        accumulator = (accumulator << 6) | decodeTable[ch];
        bitsAccumulated += 6;

        if (bitsAccumulated >= 8) {
            output[j++] = (char)(accumulator >> (bitsAccumulated - 8));
            bitsAccumulated -= 8;
        }
    }

    retVal.resize(j);
    return 0;
}

result_t global_base::btoa(exlib::string data, exlib::string& retVal)
{
    // Convert UTF-8 to Latin1 and validate
    exlib::string latin1Data;
    result_t hr = utf8ToLatin1(data, latin1Data);
    if (hr < 0) {
        return hr;
    }

    // Encode to Base64
    exlib::string base64Result;
    nodeBase64Encode(latin1Data.c_str(), latin1Data.length(), base64Result);
    retVal = base64Result;
    return 0;
}

result_t global_base::atob(exlib::string data, exlib::string& retVal)
{
    // Decode Base64
    std::string latin1Data;
    std::string stdData(data.c_str(), data.length());
    result_t hr = nodeBase64Decode(stdData, latin1Data);
    if (hr < 0) {
        return hr;
    }

    // Convert Latin1 bytes back to UTF-8 string
    const unsigned char* src = (const unsigned char*)latin1Data.c_str();
    size_t len = latin1Data.length();

    // For binary data containing null bytes, we need to build the result carefully
    exlib::string utf8Result;
    utf8Result.reserve(len * 2); // Worst case: 2 bytes per character

    for (size_t i = 0; i < len; i++) {
        unsigned char byte = src[i];

        if (byte <= 0x7F) {
            // ASCII character - single byte in UTF-8
            utf8Result.append(1, (char)byte);
        } else {
            // Latin1 character (128-255) - encode as 2-byte UTF-8 sequence
            utf8Result.append(1, (char)(0xC0 | (byte >> 6)));
            utf8Result.append(1, (char)(0x80 | (byte & 0x3F)));
        }
    }

    retVal = utf8Result;
    return 0;
}

}
