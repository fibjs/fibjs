/*
 * HttpUploadCollection.cpp
 *
 *  Created on: Aug 28, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpUploadCollection.h"
#include "HttpUploadData.h"
#include "MemoryStream.h"
#include "List.h"
#include "Map.h"
#include <string.h>

namespace fibjs {

void HttpUploadCollection::parse(exlib::string& str, const char* boundary)
{
    const char* pstr = str.c_str();
    int32_t nSize = (int32_t)str.length();
    exlib::string strName;
    exlib::string strFileName;
    exlib::string strContentType;
    exlib::string strContentTransferEncoding;
    const char *p, *p1, *p2, *szQueryString;
    const char* pstrSplit;
    int32_t uiSplitSize;
    char ch;

    boundary += 20;
    while (*boundary && *boundary == ' ')
        boundary++;

    if (qstricmp(boundary, "boundary=", 9))
        return;

    boundary += 9;
    uiSplitSize = (int32_t)qstrlen(boundary);

    pstrSplit = szQueryString = pstr;

    if (nSize < uiSplitSize + 2 || szQueryString[0] != '-'
        || szQueryString[1] != '-'
        || qstrcmp(szQueryString + 2, boundary, uiSplitSize))
        return;

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
                return;
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
                        return;

                    p1 += 10;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    if (p1 + 5 >= p || qstricmp(p1, "name=", 5))
                        return;

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

                    strName.assign(p2, (int32_t)(p1 - p2));

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

                        strFileName.assign(p2, (int32_t)(p1 - p2));
                    }
                } else if (p1 + 13 < p && !qstricmp(p1, "Content-Type:", 13)) {
                    p1 += 13;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    strContentType.assign(p1, (int32_t)(p - p1));
                } else if (p1 + 26 < p && !qstricmp(p1, "Content-Transfer-Encoding:", 26)) {
                    p1 += 26;
                    while (p1 < p && *p1 == ' ')
                        p1++;
                    strContentTransferEncoding.assign(p1, (int32_t)(p - p1));
                }
            } else {
                ch = *szQueryString++;
                nSize--;
                if (ch != '\r')
                    return;
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

        nSize = (int32_t)(p1 - p - uiSplitSize);
        p1 = szQueryString;
        szQueryString = p + uiSplitSize;

        p--;
        ch = *p;
        if (ch != '\n')
            return;
        if (*(p - 1) == '\r')
            p--;

        if (!strName.empty()) {
            int32_t uiSize = (int32_t)(p - p1);
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
}

result_t HttpUploadCollection::clear()
{
    m_count = 0;
    return 0;
}

result_t HttpUploadCollection::has(exlib::string name, bool& retVal)
{
    int32_t i;

    retVal = false;
    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str())) {
            retVal = true;
            break;
        }

    return 0;
}

result_t HttpUploadCollection::first(exlib::string name, Variant& retVal)
{
    int32_t i;

    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str())) {
            retVal = m_values[i];
            return 0;
        }

    return CALL_RETURN_UNDEFINED;
}

result_t HttpUploadCollection::all(exlib::string name, obj_ptr<List_base>& retVal)
{
    obj_ptr<List> list;
    int32_t i;

    list = new List();

    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str()))
            list->append(m_values[i]);

    retVal = list;
    return 0;
}

result_t HttpUploadCollection::add(exlib::string name, Variant value)
{
    m_names[m_count] = name;
    m_values[m_count] = value;
    m_count++;

    return 0;
}

result_t HttpUploadCollection::add(Map_base* map)
{
    return ((Map*)map)->map(this, &HttpUploadCollection::add);
}

result_t HttpUploadCollection::set(exlib::string name, Variant value)
{
    int32_t i;
    bool bFound = false;

    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str())) {
            m_values[i] = value;
            bFound = true;
            break;
        }

    if (bFound) {
        int32_t p = ++i;

        for (; i < m_count; i++)
            if (qstricmp(m_names[i].c_str(), name.c_str())) {
                if (i != p) {
                    m_names[p] = m_names[i];
                    m_values[p] = m_values[i];
                }

                p++;
            }

        m_count = p;
    } else
        return add(name, value);

    return 0;
}

result_t HttpUploadCollection::set(Map_base* map)
{
    return ((Map*)map)->map(this, &HttpUploadCollection::set);
}

result_t HttpUploadCollection::remove(exlib::string name)
{
    int32_t i;
    int32_t p = 0;

    for (i = 0; i < m_count; i++)
        if (qstricmp(m_names[i].c_str(), name.c_str())) {
            if (i != p) {
                m_names[p] = m_names[i];
                m_values[p] = m_values[i];
            }

            p++;
        }

    m_count = p;

    return 0;
}

result_t HttpUploadCollection::_named_getter(const char* property,
    Variant& retVal)
{
    return first(property, retVal);
}

result_t HttpUploadCollection::_named_enumerator(v8::Local<v8::Array>& retVal)
{
    int32_t i;
    Isolate* isolate = holder();

    retVal = v8::Array::New(isolate->m_isolate);
    for (i = 0; i < m_count; i++)
        retVal->Set(i, isolate->NewFromUtf8(m_names[i]));

    return 0;
}

result_t HttpUploadCollection::_named_setter(const char* property,
    Variant newVal)
{
    return set(property, newVal);
}

result_t HttpUploadCollection::_named_deleter(const char* property,
    v8::Local<v8::Boolean>& retVal)
{
    int32_t n = m_count;
    remove(property);
    return n > m_count;
}

} /* namespace fibjs */
