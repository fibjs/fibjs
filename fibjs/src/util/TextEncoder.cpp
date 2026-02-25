/*
 * TextEncoder.cpp
 *
 *  Created on: Jun 2, 2022
 *      Author: lion
 */

#include "object.h"
#include <TextEncoder.h>
#include "encoding_iconv.h"
#include <unicode/include/unicode/ucnv.h>
#include <unicode/include/unicode/ucnv_err.h>
#include <unicode/include/unicode/ustring.h>
#include <algorithm>

namespace fibjs {

// WHATWG encoding label to canonical name mapping
// https://encoding.spec.whatwg.org/#names-and-labels
static const char* normalizeEncoding(const exlib::string& label)
{
    // Trim and lowercase the label
    exlib::string name;
    name.reserve(label.length());
    for (size_t i = 0; i < label.length(); i++) {
        char c = label[i];
        // Skip leading/trailing ASCII whitespace
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')
            continue;
        // Convert to lowercase
        if (c >= 'A' && c <= 'Z')
            c = c - 'A' + 'a';
        name.append(1, c);
    }

    // UTF-8 aliases
    if (name == "unicode-1-1-utf-8" || name == "unicode11utf8"
        || name == "unicode20utf8" || name == "utf-8" || name == "utf8"
        || name == "x-unicode20utf8")
        return "utf-8";

    // UTF-16LE aliases
    if (name == "utf-16le" || name == "utf-16" || name == "ucs-2" || name == "unicode"
        || name == "unicodefeff" || name == "csunicode" || name == "iso-10646-ucs-2")
        return "utf-16le";

    // UTF-16BE aliases
    if (name == "utf-16be" || name == "unicodefffe")
        return "utf-16be";

    // GBK / GB2312 / GB18030 aliases
    if (name == "gb2312" || name == "gb_2312" || name == "gb_2312-80"
        || name == "gbk" || name == "chinese" || name == "csgb2312"
        || name == "csiso58gb231280" || name == "euc-cn" || name == "iso-ir-58"
        || name == "x-gbk")
        return "gbk";
    if (name == "gb18030")
        return "gb18030";

    // Big5 aliases
    if (name == "big5" || name == "big5-hkscs" || name == "cn-big5"
        || name == "csbig5" || name == "x-x-big5")
        return "big5";

    // EUC-JP aliases
    if (name == "euc-jp" || name == "cseucpkdfmtjapanese" || name == "x-euc-jp")
        return "euc-jp";

    // Shift_JIS aliases
    if (name == "shift_jis" || name == "csshiftjis" || name == "ms_kanji"
        || name == "ms932" || name == "shift-jis" || name == "sjis"
        || name == "windows-31j" || name == "x-sjis")
        return "shift_jis";

    // EUC-KR aliases
    if (name == "euc-kr" || name == "cseuckr" || name == "csksc56011987"
        || name == "iso-ir-149" || name == "korean" || name == "ks_c_5601-1987"
        || name == "ks_c_5601-1989" || name == "ksc5601" || name == "ksc_5601"
        || name == "windows-949")
        return "euc-kr";

    // ISO-2022-JP aliases
    if (name == "iso-2022-jp" || name == "csiso2022jp")
        return "iso-2022-jp";

    // ISO-8859-1 / windows-1252 (WHATWG maps iso-8859-1 to windows-1252)
    if (name == "iso-8859-1" || name == "iso8859-1" || name == "iso88591"
        || name == "iso_8859-1" || name == "iso_8859-1:1987"
        || name == "l1" || name == "latin1" || name == "us-ascii"
        || name == "ascii" || name == "ansi_x3.4-1968"
        || name == "cp819" || name == "csisolatin1" || name == "ibm819"
        || name == "iso-ir-100" || name == "windows-1252"
        || name == "x-cp1252" || name == "cp1252")
        return "windows-1252";

    // ISO-8859-2 / windows-1250
    if (name == "iso-8859-2" || name == "iso8859-2" || name == "iso88592"
        || name == "iso_8859-2" || name == "iso_8859-2:1987"
        || name == "l2" || name == "latin2" || name == "csisolatin2"
        || name == "iso-ir-101")
        return "iso-8859-2";

    // ISO-8859-3
    if (name == "iso-8859-3" || name == "iso8859-3" || name == "iso88593"
        || name == "iso_8859-3" || name == "iso_8859-3:1988"
        || name == "l3" || name == "latin3" || name == "csisolatin3"
        || name == "iso-ir-109")
        return "iso-8859-3";

    // ISO-8859-4
    if (name == "iso-8859-4" || name == "iso8859-4" || name == "iso88594"
        || name == "iso_8859-4" || name == "iso_8859-4:1988"
        || name == "l4" || name == "latin4" || name == "csisolatin4"
        || name == "iso-ir-110")
        return "iso-8859-4";

    // ISO-8859-5
    if (name == "iso-8859-5" || name == "iso8859-5" || name == "iso88595"
        || name == "iso_8859-5" || name == "iso_8859-5:1988"
        || name == "csisolatincyrillic" || name == "cyrillic"
        || name == "iso-ir-144")
        return "iso-8859-5";

    // ISO-8859-6
    if (name == "iso-8859-6" || name == "iso8859-6" || name == "iso88596"
        || name == "iso_8859-6" || name == "iso-ir-127"
        || name == "arabic" || name == "csisolatinarabic"
        || name == "ecma-114" || name == "asmo-708")
        return "iso-8859-6";

    // ISO-8859-7
    if (name == "iso-8859-7" || name == "iso8859-7" || name == "iso88597"
        || name == "iso_8859-7" || name == "iso-ir-126"
        || name == "greek" || name == "greek8" || name == "csisolatingreek"
        || name == "ecma-118" || name == "elot_928"
        || name == "sun_eu_greek")
        return "iso-8859-7";

    // ISO-8859-8
    if (name == "iso-8859-8" || name == "iso8859-8" || name == "iso88598"
        || name == "iso_8859-8" || name == "iso-ir-138"
        || name == "hebrew" || name == "csisolatinhebrew"
        || name == "iso-8859-8-i" || name == "logical" || name == "visual")
        return "iso-8859-8";

    // ISO-8859-10
    if (name == "iso-8859-10" || name == "iso8859-10" || name == "iso885910"
        || name == "iso_8859-10" || name == "iso-ir-157"
        || name == "l6" || name == "latin6" || name == "csisolatin6")
        return "iso-8859-10";

    // ISO-8859-13
    if (name == "iso-8859-13" || name == "iso8859-13" || name == "iso885913"
        || name == "iso_8859-13")
        return "iso-8859-13";

    // ISO-8859-14
    if (name == "iso-8859-14" || name == "iso8859-14" || name == "iso885914"
        || name == "iso_8859-14")
        return "iso-8859-14";

    // ISO-8859-15
    if (name == "iso-8859-15" || name == "iso8859-15" || name == "iso885915"
        || name == "iso_8859-15" || name == "l9" || name == "latin9"
        || name == "csisolatin9")
        return "iso-8859-15";

    // ISO-8859-16
    if (name == "iso-8859-16" || name == "iso8859-16")
        return "iso-8859-16";

    // Windows code pages
    if (name == "windows-1250" || name == "cp1250" || name == "x-cp1250")
        return "windows-1250";
    if (name == "windows-1251" || name == "cp1251" || name == "x-cp1251")
        return "windows-1251";
    if (name == "windows-1253" || name == "cp1253" || name == "x-cp1253")
        return "windows-1253";
    if (name == "windows-1254" || name == "cp1254" || name == "x-cp1254"
        || name == "iso-8859-9" || name == "iso8859-9" || name == "iso88599"
        || name == "iso_8859-9" || name == "l5" || name == "latin5"
        || name == "csisolatin5" || name == "iso-ir-148")
        return "windows-1254";
    if (name == "windows-1255" || name == "cp1255" || name == "x-cp1255")
        return "windows-1255";
    if (name == "windows-1256" || name == "cp1256" || name == "x-cp1256")
        return "windows-1256";
    if (name == "windows-1257" || name == "cp1257" || name == "x-cp1257")
        return "windows-1257";
    if (name == "windows-1258" || name == "cp1258" || name == "x-cp1258")
        return "windows-1258";

    // KOI8
    if (name == "koi8-r" || name == "cskoi8r" || name == "koi" || name == "koi8")
        return "koi8-r";
    if (name == "koi8-u" || name == "koi8-ru")
        return "koi8-u";

    // IBM866
    if (name == "ibm866" || name == "866" || name == "cp866" || name == "csibm866")
        return "ibm866";

    // macintosh
    if (name == "macintosh" || name == "mac" || name == "csmacintosh"
        || name == "x-mac-roman")
        return "macintosh";

    // x-mac-cyrillic
    if (name == "x-mac-cyrillic" || name == "x-mac-ukrainian")
        return "x-mac-cyrillic";

    // Not in WHATWG mapping, try to use ICU directly
    return nullptr;
}

// Resolve encoding label to canonical WHATWG name, or validate via ICU
static bool resolveEncoding(const exlib::string& label, exlib::string& canonicalName)
{
    // First try WHATWG mapping
    const char* whatwgName = normalizeEncoding(label);
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

    // For other encodings, use encoding_iconv
    return encoding_iconv(m_codec).encode(data, retVal);
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
    result_t hr = ensureConverter();
    if (hr < 0)
        return hr;

    // Parse stream option
    bool stream = false;
    GetConfigValue(opts, "stream", stream);
    bool flush = !stream;

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
            Isolate* isolate = holder();
            isolate->m_isolate->ThrowException(v8::Exception::TypeError(
                isolate->NewString("The encoded data was not valid.")));
            return CALL_E_JAVASCRIPT;
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
