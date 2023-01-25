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
#include <string.h>

namespace fibjs {

void HttpUploadCollection::parse(exlib::string& str, const char* boundary)
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
        return;

    boundary += 9;
    uiSplitSize = qstrlen(boundary);

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

        nSize = (size_t)(p1 - p - uiSplitSize);
        p1 = szQueryString;
        szQueryString = p + uiSplitSize;

        if (p > p1) {
            p--;
            ch = *p;
            if (ch != '\n')
                return;

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
}

result_t HttpUploadCollection::clear()
{
    size_t i;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];
        _pair.first.clear();
        _pair.second.clear();
    }

    m_count = 0;

    return 0;
}

result_t HttpUploadCollection::has(exlib::string name, bool& retVal)
{
    size_t i;

    retVal = false;
    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_map[i].first.c_str(), name.c_str())) {
            retVal = true;
            break;
        }

    return 0;
}

result_t HttpUploadCollection::first(exlib::string name, Variant& retVal)
{
    size_t i;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];

        if (!qstricmp(_pair.first.c_str(), name.c_str())) {
            retVal = _pair.second;
            return 0;
        }
    }

    return CALL_RETURN_NULL;
}

result_t HttpUploadCollection::all(exlib::string name, obj_ptr<NObject>& retVal)
{
    if (!name.empty()) {
        obj_ptr<NArray> list;

        all(name, list);
        retVal = list;
    } else
        all(retVal);

    return 0;
}

result_t HttpUploadCollection::add(exlib::string name, Variant value)
{
    if (m_map.size() < m_count + 1)
        m_map.resize(m_count + 1);

    m_map[m_count] = pair(name, value);
    m_count++;

    return 0;
}

result_t HttpUploadCollection::add(v8::Local<v8::Object> map)
{
    v8::Local<v8::Context> context = map->GetCreationContextChecked();
    JSArray ks = map->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;
    Isolate* isolate = holder();

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(context, i);
        JSValue v = map->Get(context, k);

        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (v->IsArray())
            add(isolate->toString(k), v8::Local<v8::Array>::Cast(v));
        else
            add(isolate->toString(k), (Variant)v);
    }

    return 0;
}

result_t HttpUploadCollection::add(exlib::string name, v8::Local<v8::Array> values)
{
    v8::Local<v8::Context> context = values->GetCreationContextChecked();
    int32_t len = values->Length();
    int32_t i;

    for (i = 0; i < len; i++)
        add(name, (Variant)JSValue(values->Get(context, i)));

    return 0;
}

result_t HttpUploadCollection::set(exlib::string name, Variant value)
{
    remove(name);
    return add(name, value);
}

result_t HttpUploadCollection::set(v8::Local<v8::Object> map)
{
    v8::Local<v8::Context> context = map->GetCreationContextChecked();
    JSArray ks = map->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;
    Isolate* isolate = holder();

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(context, i);
        JSValue v = map->Get(context, k);

        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (v->IsArray())
            set(isolate->toString(k), v8::Local<v8::Array>::Cast(v));
        else
            set(isolate->toString(k), (Variant)v);
    }

    return 0;
}

result_t HttpUploadCollection::set(exlib::string name, v8::Local<v8::Array> values)
{
    v8::Local<v8::Context> context = values->GetCreationContextChecked();
    int32_t len = values->Length();
    int32_t i;

    remove(name);
    for (i = 0; i < len; i++)
        add(name, (Variant)JSValue(values->Get(context, i)));

    return 0;
}

result_t HttpUploadCollection::remove(exlib::string name)
{
    size_t i;
    size_t p = 0;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];

        if (qstricmp(_pair.first.c_str(), name.c_str())) {
            if (i != p)
                m_map[p] = _pair;

            p++;
        }
    }

    m_count = p;

    return 0;
}

result_t HttpUploadCollection::_delete(exlib::string name)
{
    return remove(name);
}

result_t HttpUploadCollection::sort()
{
    if (m_count)
        std::sort(m_map.begin(), m_map.begin() + m_count, [](pair& a, pair& b) {
            return a.first < b.first;
        });

    return 0;
}

result_t HttpUploadCollection::keys(obj_ptr<NArray>& retVal)
{
    obj_ptr<NArray> _keys = new NArray();
    size_t i;

    for (i = 0; i < m_count; i++)
        _keys->append(m_map[i].first);

    retVal = _keys;

    return 0;
}

result_t HttpUploadCollection::values(obj_ptr<NArray>& retVal)
{
    obj_ptr<NArray> _keys = new NArray();
    size_t i;

    for (i = 0; i < m_count; i++)
        _keys->append(m_map[i].second);

    retVal = _keys;
    return 0;
}

result_t HttpUploadCollection::_named_getter(exlib::string property,
    Variant& retVal)
{
    return first(property, retVal);
}

result_t HttpUploadCollection::_named_enumerator(v8::Local<v8::Array>& retVal)
{
    size_t i;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    retVal = v8::Array::New(isolate->m_isolate);
    for (i = 0; i < m_count; i++)
        retVal->Set(context, (int32_t)i, isolate->NewString(m_map[i].first)).Check();

    return 0;
}

result_t HttpUploadCollection::_named_setter(exlib::string property,
    Variant newVal)
{
    return set(property, newVal);
}

result_t HttpUploadCollection::_named_deleter(exlib::string property,
    v8::Local<v8::Boolean>& retVal)
{
    size_t n = m_count;
    remove(property);
    return n > m_count;
}

} /* namespace fibjs */
