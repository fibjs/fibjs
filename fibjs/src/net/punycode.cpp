/*
 * punycode.cpp
 *
 *  Created on: Aug 22, 2017
 *      Author: lion
 */

#include "ifs/punycode.h"
#include "utf8.h"
#include <ada.h>

namespace fibjs {

DECLARE_MODULE(punycode);

result_t punycode_base::encode(exlib::string domain, exlib::string& retVal)
{
    exlib::wstring32 wdomain = utf8to32String(domain);
    std::string out;

    ada::idna::utf32_to_punycode(wdomain, out);

    retVal = out;

    return 0;
}

result_t punycode_base::decode(exlib::string domain, exlib::string& retVal)
{
    std::u32string out;
    ada::idna::punycode_to_utf32(domain, out);

    retVal = utf32to8String(out);

    return 0;
}

result_t punycode_base::toASCII(exlib::string domain, exlib::string& retVal)
{
    retVal = ada::idna::to_ascii(domain);
    return 0;
}

result_t punycode_base::toUnicode(exlib::string domain, exlib::string& retVal)
{
    retVal = ada::idna::to_unicode(domain);
    return 0;
}

}