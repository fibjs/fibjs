/*
 * TextEncoder.cpp
 *
 *  Created on: Jun 2, 2022
 *      Author: lion
 */

#include "object.h"
#include <TextEncoder.h>
#include "encoding_conv.h"
#include <unicode/include/unicode/ucnv.h>
#include <unicode/include/unicode/ucnv_err.h>
#include <unicode/include/unicode/ustring.h>
#include <algorithm>

namespace fibjs {

// Resolve encoding label to canonical WHATWG name, or validate via ICU
static bool resolveEncoding(const exlib::string& label, exlib::string& canonicalName)
{
    // First try WHATWG mapping
    const char* whatwgName = encoding_conv::normalizeEncoding(label);
    if (whatwgName) {
        canonicalName = whatwgName;
        return true;
    }

    // Validate: label must only contain valid encoding name characters
    // (ASCII printable, no control chars, no non-ASCII)
    for (size_t i = 0; i < label.length(); i++) {
        unsigned char c = (unsigned char)label[i];
        if (c < 0x20 || c > 0x7E)
            return false;
    }

    // Fallback: try ICU directly for non-WHATWG encodings (fibjs extension)
    UErrorCode err = U_ZERO_ERROR;
    UConverter* cnv = ucnv_open(label.c_str(), &err);
    if (U_SUCCESS(err) && cnv) {
        // Use ICU canonical name, lowercased
        UErrorCode nameErr = U_ZERO_ERROR;
        const char* icuName = ucnv_getName(cnv, &nameErr);
        if (U_SUCCESS(nameErr) && icuName) {
            exlib::string lower;
            lower.reserve(strlen(icuName));
            for (const char* p = icuName; *p; p++) {
                char c = *p;
                if (c >= 'A' && c <= 'Z')
                    c = c - 'A' + 'a';
                lower.append(1, c);
            }
            canonicalName = lower;
        } else {
            canonicalName = label;
        }
        ucnv_close(cnv);
        return true;
    }

    if (cnv)
        ucnv_close(cnv);
    return false;
}

// ==================== TextEncoder ====================

result_t TextEncoder_base::_new(exlib::string codec, v8::Local<v8::Object> opts, obj_ptr<TextEncoder_base>& retVal,
    v8::Local<v8::Object> This)
{
    exlib::string canonicalName;
    if (!resolveEncoding(codec, canonicalName)) {
        Isolate* isolate = Isolate::current(opts);
        isolate->m_isolate->ThrowException(v8::Exception::RangeError(
            isolate->NewString(("The encoding label provided ('" + codec + "') is invalid.").c_str())));
        return CALL_E_JAVASCRIPT;
    }

    retVal = new TextEncoder(canonicalName);
    return 0;
}

result_t TextEncoder::encode(exlib::string data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal)
{
    if (data.empty()) {
        retVal = new Buffer(NULL, 0);
        return 0;
    }

    // For utf-8, the internal string is already utf-8
    if (m_codec == "utf-8" || m_codec == "utf8") {
        retVal = new Buffer(data.c_str(), data.length());
        return 0;
    }

    // For other encodings, use encoding_conv
    return encoding_conv(m_codec).encode(data, retVal);
}

result_t TextEncoder::encodeInto(exlib::string source, Buffer_base* destination, v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    Buffer* buf = Buffer::Cast(destination);
    uint8_t* dest = buf->data();
    size_t destLen = buf->length();

    size_t read = 0;
    size_t written = 0;

    // fibjs strings are UTF-8, iterate by code points
    const uint8_t* src = (const uint8_t*)source.c_str();
    size_t srcLen = source.length();
    size_t i = 0;

    while (i < srcLen) {
        // Determine the byte length of the current UTF-8 character
        uint8_t ch = src[i];
        size_t charLen;

        if (ch < 0x80)
            charLen = 1;
        else if ((ch & 0xE0) == 0xC0)
            charLen = 2;
        else if ((ch & 0xF0) == 0xE0)
            charLen = 3;
        else if ((ch & 0xF8) == 0xF0)
            charLen = 4;
        else {
            // Invalid UTF-8 lead byte, skip
            i++;
            read++;
            continue;
        }

        // Check if the complete character fits in destination
        if (i + charLen > srcLen)
            break;
        if (written + charLen > destLen)
            break;

        // Copy the UTF-8 bytes
        memcpy(dest + written, src + i, charLen);
        written += charLen;
        i += charLen;

        // Count code units (UTF-16): BMP = 1, supplementary = 2
        if (charLen == 4)
            read += 2; // surrogate pair = 2 UTF-16 code units
        else
            read += 1;
    }

    v8::Local<v8::Object> result = v8::Object::New(isolate->m_isolate);
    result->Set(context, isolate->NewString("read"), v8::Number::New(isolate->m_isolate, (double)read)).IsJust();
    result->Set(context, isolate->NewString("written"), v8::Number::New(isolate->m_isolate, (double)written)).IsJust();

    retVal = result;
    return 0;
}

result_t TextEncoder::get_encoding(exlib::string& retVal)
{
    retVal = m_codec;
    return 0;
}

// ==================== TextDecoder ====================

result_t TextDecoder_base::_new(exlib::string codec, v8::Local<v8::Object> opts, obj_ptr<TextDecoder_base>& retVal,
    v8::Local<v8::Object> This)
{
    exlib::string canonicalName;
    if (!resolveEncoding(codec, canonicalName)) {
        Isolate* isolate = Isolate::current(opts);
        isolate->m_isolate->ThrowException(v8::Exception::RangeError(
            isolate->NewString(("The encoding label provided ('" + codec + "') is invalid.").c_str())));
        return CALL_E_JAVASCRIPT;
    }

    // Parse options
    bool fatal = false;
    bool ignoreBOM = false;

    GetConfigValue(opts, "fatal", fatal);
    GetConfigValue(opts, "ignoreBOM", ignoreBOM);

    retVal = new TextDecoder(canonicalName, fatal, ignoreBOM);
    return 0;
}

result_t TextDecoder::ensureConverter()
{
    if (m_cnv)
        return 0;

    UErrorCode err = U_ZERO_ERROR;
    m_cnv = ucnv_open(m_codec.c_str(), &err);
    if (U_FAILURE(err) || !m_cnv) {
        m_cnv = nullptr;
        return CHECK_ERROR(Runtime::setError("TextDecoder: Failed to create converter for '" + m_codec + "'."));
    }

    if (m_fatal) {
        UErrorCode cbErr = U_ZERO_ERROR;
        ucnv_setToUCallBack(m_cnv, UCNV_TO_U_CALLBACK_STOP, nullptr, nullptr, nullptr, &cbErr);
    }

    return 0;
}

result_t TextDecoder::decode(Buffer_base* data, v8::Local<v8::Object> opts, exlib::string& retVal)
{
    // Parse stream option and delegate to C++ API
    bool stream = false;
    GetConfigValue(opts, "stream", stream);
    return decode(data, !stream, retVal);
}

result_t TextDecoder::decode(Buffer_base* data, bool flush, exlib::string& retVal)
{
    result_t hr = ensureConverter();
    if (hr < 0)
        return hr;

    Buffer* buf = Buffer::Cast(data);
    const char* src = (const char*)buf->data();
    size_t srcLen = buf->length();

    if (srcLen == 0 && !flush) {
        retVal.clear();
        return 0;
    }

    // Allocate UChar buffer: worst case each byte becomes one UChar
    size_t ucharBufSize = srcLen + 1;
    std::vector<UChar> ucharBuf(ucharBufSize);

    const char* srcPtr = src;
    const char* srcEnd = src + srcLen;
    UChar* destPtr = ucharBuf.data();
    UChar* destEnd = ucharBuf.data() + ucharBufSize;

    UErrorCode err = U_ZERO_ERROR;
    ucnv_toUnicode(m_cnv, &destPtr, destEnd, &srcPtr, srcEnd, nullptr, flush, &err);

    if (err == U_BUFFER_OVERFLOW_ERROR) {
        // Retry with larger buffer
        size_t consumed = srcPtr - src;
        size_t produced = destPtr - ucharBuf.data();

        ucharBufSize = produced + (srcLen - consumed) * 4 + 32;
        ucharBuf.resize(ucharBufSize);

        destPtr = ucharBuf.data() + produced;
        destEnd = ucharBuf.data() + ucharBufSize;

        err = U_ZERO_ERROR;
        ucnv_toUnicode(m_cnv, &destPtr, destEnd, &srcPtr, srcEnd, nullptr, flush, &err);
    }

    if (U_FAILURE(err)) {
        // Reset converter state on error
        ucnv_reset(m_cnv);
        m_bomSeen = false;

        if (m_fatal) {
            Isolate* isolate = Isolate::current();
            if (isolate) {
                isolate->m_isolate->ThrowException(v8::Exception::TypeError(
                    isolate->NewString("The encoded data was not valid.")));
                return CALL_E_JAVASCRIPT;
            }
            return CHECK_ERROR(Runtime::setError("The encoded data was not valid."));
        }

        // Should not reach here with SUBSTITUTE callback, but handle gracefully
        retVal.clear();
        return 0;
    }

    size_t ucharLen = destPtr - ucharBuf.data();

    // Handle BOM stripping
    size_t startOffset = 0;
    if (!m_bomSeen && !m_ignoreBOM && ucharLen > 0) {
        if (ucharBuf[0] == 0xFEFF)
            startOffset = 1;
    }
    if (ucharLen > 0)
        m_bomSeen = true;

    // Convert UChar (UTF-16) to UTF-8
    if (ucharLen <= startOffset) {
        retVal.clear();
    } else {
        int32_t utf8Len = 0;
        UErrorCode u8err = U_ZERO_ERROR;

        // Pre-flight to get required length
        u_strToUTF8(nullptr, 0, &utf8Len, ucharBuf.data() + startOffset, (int32_t)(ucharLen - startOffset), &u8err);

        if (utf8Len > 0) {
            retVal.resize(utf8Len);
            u8err = U_ZERO_ERROR;
            u_strToUTF8(retVal.data(), utf8Len, nullptr, ucharBuf.data() + startOffset, (int32_t)(ucharLen - startOffset), &u8err);
        } else {
            retVal.clear();
        }
    }

    // If flush, reset BOM state for next stream
    if (flush)
        m_bomSeen = false;

    return 0;
}

result_t TextDecoder::decode(exlib::string& retVal)
{
    result_t hr = ensureConverter();
    if (hr < 0)
        return hr;

    // Flush remaining bytes in converter
    const char* srcPtr = nullptr;
    const char* srcEnd = nullptr;

    UChar ucharBuf[16];
    UChar* destPtr = ucharBuf;
    UChar* destEnd = ucharBuf + 16;

    UErrorCode err = U_ZERO_ERROR;
    ucnv_toUnicode(m_cnv, &destPtr, destEnd, &srcPtr, srcEnd, nullptr, true, &err);

    size_t ucharLen = destPtr - ucharBuf;

    if (U_FAILURE(err)) {
        ucnv_reset(m_cnv);
        m_bomSeen = false;

        if (m_fatal) {
            Isolate* isolate = holder();
            isolate->m_isolate->ThrowException(v8::Exception::TypeError(
                isolate->NewString("The encoded data was not valid.")));
            return CALL_E_JAVASCRIPT;
        }

        retVal.clear();
        return 0;
    }

    if (ucharLen == 0) {
        m_bomSeen = false;
        retVal.clear();
        return 0;
    }

    // Convert UChar to UTF-8
    int32_t utf8Len = 0;
    UErrorCode u8err = U_ZERO_ERROR;
    u_strToUTF8(nullptr, 0, &utf8Len, ucharBuf, (int32_t)ucharLen, &u8err);

    if (utf8Len > 0) {
        retVal.resize(utf8Len);
        u8err = U_ZERO_ERROR;
        u_strToUTF8(retVal.data(), utf8Len, nullptr, ucharBuf, (int32_t)ucharLen, &u8err);
    } else {
        retVal.clear();
    }

    m_bomSeen = false;
    return 0;
}

result_t TextDecoder::get_encoding(exlib::string& retVal)
{
    retVal = m_codec;
    return 0;
}

result_t TextDecoder::get_fatal(bool& retVal)
{
    retVal = m_fatal;
    return 0;
}

result_t TextDecoder::get_ignoreBOM(bool& retVal)
{
    retVal = m_ignoreBOM;
    return 0;
}

}
