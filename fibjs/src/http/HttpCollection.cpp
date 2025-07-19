/*
 * HttpCollection.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpCollection.h"
#include "MemoryStream.h"
#include "Url.h"
#include <string.h>
#include <set>

namespace fibjs {

result_t HttpCollection::parseCookie(exlib::string& str)
{
    const char* pstr = str.c_str();
    int32_t nSize = (int32_t)str.length();
    const char* pstrTemp;
    exlib::string strKey, strValue;

    while (nSize) {
        while (nSize && *pstr == ' ') {
            pstr++;
            nSize--;
        }

        pstrTemp = pstr;
        while (nSize && *pstr != '=' && *pstr != ';') {
            pstr++;
            nSize--;
        }

        if (pstr > pstrTemp)
            Url::decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strKey, true);
        else
            strKey.clear();

        if (nSize && *pstr == '=') {
            nSize--;
            pstr++;
        }

        pstrTemp = pstr;
        while (nSize && *pstr != ';') {
            pstr++;
            nSize--;
        }

        if (!strKey.empty()) {
            if (pstr > pstrTemp)
                Url::decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strValue, true);
            else
                strValue.clear();
        }

        if (nSize) {
            nSize--;
            pstr++;
        }

        if (!strKey.empty())
            append(strKey, strValue);
    }

    return 0;
}

} /* namespace fibjs */
