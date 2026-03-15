/*
 * encoding_conv.cpp
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#include "object.h"
#include "encoding_conv.h"
#include "ifs/encoding.h"
#include <unicode/include/unicode/ucnv.h>

namespace fibjs {

encoding_conv::encoding_conv()
{
    m_charset = "utf-8";
}

encoding_conv::encoding_conv(exlib::string charset)
{
    if (charset.empty()) {
        m_charset = "utf-8";
    } else {
        const char* normalized = normalizeEncoding(charset);
        m_charset = normalized ? normalized : charset;
    }
}

encoding_conv::~encoding_conv()
{
}

void encoding_conv::open(const char* charset)
{
    const char* normalized = normalizeEncoding(charset);
    m_charset = normalized ? normalized : charset;
}

result_t encoding_conv::encode(exlib::string data, exlib::string& retVal)
{
    if (data.empty()) {
        retVal.clear();
        return 0;
    }

    if (ucs_encode(data, retVal) == 0)
        return 0;

    int32_t _sz;
    UErrorCode errorCode = U_ZERO_ERROR;

    _sz = ucnv_convert(m_charset.c_str(), "utf-8", NULL, 0, data.c_str(), data.length(), &errorCode);
    if (_sz) {
        retVal.resize(_sz);
        errorCode = U_ZERO_ERROR;
        ucnv_convert(m_charset.c_str(), "utf-8", retVal.data(), _sz, data.c_str(), data.length(), &errorCode);
        return 0;
    }

    return CHECK_ERROR(Runtime::setError("encoding: Unknown charset: '%s'.", m_charset.c_str()));
}

result_t encoding_conv::encode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;

    result_t hr = encode(data, strBuf);
    if (hr < 0)
        return hr;

    retVal = new Buffer(strBuf.c_str(), strBuf.length());

    return 0;
}

result_t encoding_conv::decode(const char* data, size_t sz, exlib::string& retVal)
{
    if (sz == 0) {
        retVal.clear();
        return 0;
    }

    if (ucs_decode(data, sz, retVal) == 0)
        return 0;

    int32_t _sz;
    UErrorCode errorCode = U_ZERO_ERROR;

    _sz = ucnv_convert("utf-8", m_charset.c_str(), NULL, 0, data, sz, &errorCode);
    if (_sz) {
        retVal.resize(_sz);
        errorCode = U_ZERO_ERROR;
        ucnv_convert("utf-8", m_charset.c_str(), retVal.data(), _sz, data, sz, &errorCode);
        return 0;
    }

    return CHECK_ERROR(Runtime::setError("encoding: Unknown charset: '%s'.", m_charset.c_str()));
}

result_t encoding_conv::decode(const exlib::string& data, exlib::string& retVal)
{
    return decode(data.c_str(), data.length(), retVal);
}

result_t encoding_conv::decode(Buffer_base* data, exlib::string& retVal)
{
    Buffer* buf = Buffer::Cast(data);
    return decode((const char*)buf->data(), buf->length(), retVal);
}

bool encoding_conv::is_encoding(exlib::string charset)
{
    if (is_ucs_encoding(charset))
        return true;

    const char* normalized = normalizeEncoding(charset);
    const char* name = normalized ? normalized : charset.c_str();

    UErrorCode err = U_ZERO_ERROR;
    UConverter* icu_ec = ucnv_open(name, &err);
    if (icu_ec) {
        ucnv_close(icu_ec);
        return true;
    }

    return false;
}

// WHATWG encoding label to canonical name mapping
// https://encoding.spec.whatwg.org/#names-and-labels
const char* encoding_conv::normalizeEncoding(const exlib::string& label)
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

    // ISO-8859-2
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

    // Not in WHATWG mapping
    return nullptr;
}

}
