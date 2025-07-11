/*
 * HttpCollection.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpCollection.h"
#include "HttpUploadData.h"
#include "MemoryStream.h"
#include "Url.h"
#include <string.h>
#include <set>

namespace fibjs {

result_t HttpCollection::parse(exlib::string& str, const char* sep, const char* eq)
{
    const char* pstr = str.c_str();
    int32_t nSize = (int32_t)str.length();
    const char* pstrTemp;
    exlib::string strKey, strValue;
    int32_t sep_len = (int32_t)qstrlen(sep);
    int32_t eq_len = (int32_t)qstrlen(eq);
    bool found_eq;

    while (nSize) {
        pstrTemp = pstr;
        found_eq = false;

        while (nSize) {
            if (!qstrcmp(pstr, sep, sep_len))
                break;

            if (!qstrcmp(pstr, eq, eq_len)) {
                found_eq = true;
                break;
            }

            pstr++;
            nSize--;
        }

        if (pstr > pstrTemp)
            Url::decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strKey, true);
        else
            strKey.clear();

        if (nSize && found_eq) {
            nSize -= eq_len;
            pstr += eq_len;
        }

        pstrTemp = pstr;
        while (nSize && qstrcmp(pstr, sep, sep_len)) {
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
            nSize -= sep_len;
            pstr += sep_len;
        }

        if (!strKey.empty())
            add(strKey, strValue);
    }

    return 0;
}

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
            add(strKey, strValue);
    }

    return 0;
}

result_t HttpCollection::parseMultipart(exlib::string& str, const char* boundary)
{
    const char* pstr = str.c_str();
    size_t nSize = str.length();
    exlib::string strName;
    exlib::string strFileName;
    exlib::string strContentType;
    exlib::string strContentTransferEncoding;
    const char *p, *p1, *p2, *szQueryString;
    const char* pstrSplit;
    size_t uiSplitSize;
    char ch;

    boundary += 20;
    while (*boundary && *boundary == ' ')
        boundary++;

    if (qstricmp(boundary, "boundary=", 9))
        return 0;

    boundary += 9;
    uiSplitSize = qstrlen(boundary);

    pstrSplit = szQueryString = pstr;

    if (nSize < uiSplitSize + 2 || szQueryString[0] != '-'
        || szQueryString[1] != '-'
        || qstrcmp(szQueryString + 2, boundary, uiSplitSize))
        return 0;

    uiSplitSize += 2;
    szQueryString += uiSplitSize;
    nSize -= uiSplitSize;

    while (nSize) {
        strFileName.clear();
        strContentType.clear();
        strContentTransferEncoding.clear();

        while (nSize > 0) {
            ch = *szQueryString++;
            nSize--;
            if (ch != '\r')
                return 0;
            if (nSize > 0 && *szQueryString == '\n') {
                nSize--;
                szQueryString++;
            }

            p = szQueryString;
            while (nSize > 0 && *p != '\r') {
                nSize--;
                p++;
            }

            if (nSize == 0)
                break;

            p1 = szQueryString;
            szQueryString = p;

            if (p != p1) {
                if (p1 + 20 < p && !qstricmp(p1, "Content-Disposition:", 20)) {
                    p1 += 20;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    if (p1 + 10 >= p || qstricmp(p1, "form-data;", 10))
                        return 0;

                    p1 += 10;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    if (p1 + 5 >= p || qstricmp(p1, "name=", 5))
                        return 0;

                    p1 += 5;

                    while (p1 < p && *p1 == ' ')
                        p1++;

                    ch = ';';
                    if (*p1 == '\"') {
                        p1++;
                        ch = '\"';
                    }

                    p2 = p1;
                    while (p1 < p && *p1 != ch)
                        p1++;

                    strName.assign(p2, (size_t)(p1 - p2));

                    if (p1 < p && *p1 == '\"')
                        p1++;

                    if (p1 < p && *p1 == ';')
                        p1++;

                    while (p1 < p && *p1 == ' ')
                        p1++;

                    if (p1 + 9 < p && !qstricmp(p1, "filename=", 9)) {
                        p1 += 9;

                        while (p1 < p && *p1 == ' ')
                            p1++;

                        ch = ';';
                        if (*p1 == '\"') {
                            p1++;
                            ch = '\"';
                        }

                        p2 = p1;
                        while (p1 < p && *p1 != ch) {
                            if (*p1 == '/' || *p1 == '\\')
                                p2 = p1 + 1;
                            p1++;
                        }

                        strFileName.assign(p2, (size_t)(p1 - p2));
                    }
                } else if (p1 + 13 < p && !qstricmp(p1, "Content-Type:", 13)) {
                    p1 += 13;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    strContentType.assign(p1, (size_t)(p - p1));
                } else if (p1 + 26 < p && !qstricmp(p1, "Content-Transfer-Encoding:", 26)) {
                    p1 += 26;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    strContentTransferEncoding.assign(p1, (size_t)(p - p1));
                }
            } else {
                ch = *szQueryString++;
                nSize--;
                if (ch != '\r')
                    return 0;
                if (nSize > 0 && *szQueryString == '\n') {
                    nSize--;
                    szQueryString++;
                }
                break;
            }
        }

        p = szQueryString;
        p1 = p + nSize;
        while (p1 > p && (p = (char*)memchr(p, '-', p1 - p))
            && p1 > p + uiSplitSize && memcmp(p, pstrSplit, uiSplitSize))
            p++;

        if (!p || p1 <= p + uiSplitSize)
            break;

        nSize = (size_t)(p1 - p - uiSplitSize);
        p1 = szQueryString;
        szQueryString = p + uiSplitSize;

        if (p > p1) {
            p--;
            ch = *p;
            if (ch != '\n')
                return 0;

            if (p > p1 && *(p - 1) == '\r')
                p--;
        }

        if (!strName.empty()) {
            size_t uiSize = (size_t)(p - p1);
            exlib::string strTemp;
            Variant varTemp;

            strTemp.assign(p1, uiSize);

            if (strFileName.empty())
                varTemp = strTemp;
            else {
                obj_ptr<HttpUploadData> objTemp = new HttpUploadData();
                date_t tm;

                objTemp->m_name = strFileName;
                objTemp->m_type = strContentType;
                objTemp->m_encoding = strContentTransferEncoding;
                objTemp->m_body = new MemoryStream::CloneStream(strTemp, tm);

                varTemp = objTemp;
            }

            add(strName, varTemp);
        }
    }

    return 0;
}

} /* namespace fibjs */
