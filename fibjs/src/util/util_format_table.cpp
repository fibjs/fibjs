/*
 * util_format_table.cpp
 *
 *  Created on: Aug 16, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "utf8.h"
#include "qstring.h"
#include "QuickArray.h"
#include "StringBuffer.h"
#include "TextColor.h"
#include <map>

namespace fibjs {

void string_format(StringBuffer& strBuffer, v8::Local<v8::Value> v, bool color);
exlib::string json_format(v8::Local<v8::Value> obj, bool color, int32_t depth);

struct CharWidth {
    int32_t m_char;
    int32_t m_width;
};

static CharWidth s_char_width[] = {
    { 0x0, 1 }, { 0x61c, 0 }, { 0x61d, 1 }, { 0x1100, 2 }, { 0x1160, 1 }, { 0x180e, 0 },
    { 0x180f, 1 }, { 0x200b, 0 }, { 0x200d, 1 }, { 0x200e, 0 }, { 0x2010, 1 }, { 0x202a, 0 },
    { 0x202f, 1 }, { 0x2060, 0 }, { 0x2065, 1 }, { 0x2066, 0 }, { 0x2070, 1 }, { 0x231a, 2 },
    { 0x231c, 1 }, { 0x2329, 2 }, { 0x232b, 1 }, { 0x23e9, 2 }, { 0x23ed, 1 }, { 0x23f0, 2 },
    { 0x23f1, 1 }, { 0x23f3, 2 }, { 0x23f4, 1 }, { 0x25fd, 2 }, { 0x25ff, 1 }, { 0x2614, 2 },
    { 0x2616, 1 }, { 0x2648, 2 }, { 0x2654, 1 }, { 0x267f, 2 }, { 0x2680, 1 }, { 0x2693, 2 },
    { 0x2694, 1 }, { 0x26a1, 2 }, { 0x26a2, 1 }, { 0x26aa, 2 }, { 0x26ac, 1 }, { 0x26bd, 2 },
    { 0x26bf, 1 }, { 0x26c4, 2 }, { 0x26c6, 1 }, { 0x26ce, 2 }, { 0x26cf, 1 }, { 0x26d4, 2 },
    { 0x26d5, 1 }, { 0x26ea, 2 }, { 0x26eb, 1 }, { 0x26f2, 2 }, { 0x26f4, 1 }, { 0x26f5, 2 },
    { 0x26f6, 1 }, { 0x26fa, 2 }, { 0x26fb, 1 }, { 0x26fd, 2 }, { 0x26fe, 1 }, { 0x2705, 2 },
    { 0x2706, 1 }, { 0x270a, 2 }, { 0x270c, 1 }, { 0x2728, 2 }, { 0x2729, 1 }, { 0x274c, 2 },
    { 0x274d, 1 }, { 0x274e, 2 }, { 0x274f, 1 }, { 0x2753, 2 }, { 0x2756, 1 }, { 0x2757, 2 },
    { 0x2758, 1 }, { 0x2795, 2 }, { 0x2798, 1 }, { 0x27b0, 2 }, { 0x27b1, 1 }, { 0x27bf, 2 },
    { 0x27c0, 1 }, { 0x2b1b, 2 }, { 0x2b1d, 1 }, { 0x2b50, 2 }, { 0x2b51, 1 }, { 0x2b55, 2 },
    { 0x2b56, 1 }, { 0x2e80, 2 }, { 0x2e9a, 1 }, { 0x2e9b, 2 }, { 0x2ef4, 1 }, { 0x2f00, 2 },
    { 0x2fd6, 1 }, { 0x2ff0, 2 }, { 0x2ffc, 1 }, { 0x3000, 2 }, { 0x303f, 1 }, { 0x3041, 2 },
    { 0x3097, 1 }, { 0x3099, 2 }, { 0x3100, 1 }, { 0x3105, 2 }, { 0x3130, 1 }, { 0x3131, 2 },
    { 0x318f, 1 }, { 0x3190, 2 }, { 0x31e4, 1 }, { 0x31f0, 2 }, { 0x321f, 1 }, { 0x3220, 2 },
    { 0x3248, 1 }, { 0x3250, 2 }, { 0x4dc0, 1 }, { 0x4e00, 2 }, { 0xa48d, 1 }, { 0xa490, 2 },
    { 0xa4c7, 1 }, { 0xa960, 2 }, { 0xa97d, 1 }, { 0xac00, 2 }, { 0xd7a4, 1 }, { 0xd800, 2 },
    { 0x10000, 1 }, { 0x16fe0, 2 }, { 0x16fe5, 1 }, { 0x16ff0, 2 }, { 0x16ff2, 1 }, { 0x17000, 2 },
    { 0x187f8, 1 }, { 0x18800, 2 }, { 0x18cd6, 1 }, { 0x18d00, 2 }, { 0x18d09, 1 }, { 0x1b000, 2 },
    { 0x1b11f, 1 }, { 0x1b150, 2 }, { 0x1b153, 1 }, { 0x1b164, 2 }, { 0x1b168, 1 }, { 0x1b170, 2 },
    { 0x1b2fc, 1 }, { 0x1bca0, 0 }, { 0x1bca4, 1 }, { 0x1d15e, 2 }, { 0x1d160, 1 }, { 0x1d173, 0 },
    { 0x1d17b, 1 }, { 0x1d1bb, 2 }, { 0x1d1bd, 1 }, { 0x1f004, 2 }, { 0x1f005, 1 }, { 0x1f0cf, 2 },
    { 0x1f0d0, 1 }, { 0x1f18e, 2 }, { 0x1f18f, 1 }, { 0x1f191, 2 }, { 0x1f19b, 1 }, { 0x1f1e6, 2 },
    { 0x1f203, 1 }, { 0x1f210, 2 }, { 0x1f23c, 1 }, { 0x1f240, 2 }, { 0x1f249, 1 }, { 0x1f250, 2 },
    { 0x1f252, 1 }, { 0x1f260, 2 }, { 0x1f266, 1 }, { 0x1f300, 2 }, { 0x1f321, 1 }, { 0x1f32d, 2 },
    { 0x1f336, 1 }, { 0x1f337, 2 }, { 0x1f37d, 1 }, { 0x1f37e, 2 }, { 0x1f394, 1 }, { 0x1f3a0, 2 },
    { 0x1f3cb, 1 }, { 0x1f3cf, 2 }, { 0x1f3d4, 1 }, { 0x1f3e0, 2 }, { 0x1f3f1, 1 }, { 0x1f3f4, 2 },
    { 0x1f3f5, 1 }, { 0x1f3f8, 2 }, { 0x1f43f, 1 }, { 0x1f440, 2 }, { 0x1f441, 1 }, { 0x1f442, 2 },
    { 0x1f4fd, 1 }, { 0x1f4ff, 2 }, { 0x1f53e, 1 }, { 0x1f54b, 2 }, { 0x1f54f, 1 }, { 0x1f550, 2 },
    { 0x1f568, 1 }, { 0x1f57a, 2 }, { 0x1f57b, 1 }, { 0x1f595, 2 }, { 0x1f597, 1 }, { 0x1f5a4, 2 },
    { 0x1f5a5, 1 }, { 0x1f5fb, 2 }, { 0x1f650, 1 }, { 0x1f680, 2 }, { 0x1f6c6, 1 }, { 0x1f6cc, 2 },
    { 0x1f6cd, 1 }, { 0x1f6d0, 2 }, { 0x1f6d3, 1 }, { 0x1f6d5, 2 }, { 0x1f6d8, 1 }, { 0x1f6eb, 2 },
    { 0x1f6ed, 1 }, { 0x1f6f4, 2 }, { 0x1f6fd, 1 }, { 0x1f7e0, 2 }, { 0x1f7ec, 1 }, { 0x1f90c, 2 },
    { 0x1f93b, 1 }, { 0x1f93c, 2 }, { 0x1f946, 1 }, { 0x1f947, 2 }, { 0x1f979, 1 }, { 0x1f97a, 2 },
    { 0x1f9cc, 1 }, { 0x1f9cd, 2 }, { 0x1fa00, 1 }, { 0x1fa70, 2 }, { 0x1fa75, 1 }, { 0x1fa78, 2 },
    { 0x1fa7b, 1 }, { 0x1fa80, 2 }, { 0x1fa87, 1 }, { 0x1fa90, 2 }, { 0x1faa9, 1 }, { 0x1fab0, 2 },
    { 0x1fab7, 1 }, { 0x1fac0, 2 }, { 0x1fac3, 1 }, { 0x1fad0, 2 }, { 0x1fad7, 1 }, { 0x20000, 2 },
    { 0x2fffe, 1 }, { 0x30000, 2 }, { 0x3fffe, 1 }, { 0xe0001, 0 }, { 0xe0002, 1 }
};

inline int32_t getCharWidth(int32_t ch)
{
    int32_t p0 = 0;
    int32_t p1 = ARRAYSIZE(s_char_width) - 1;
    int32_t p;

    if (ch >= 0) {
        if (ch >= s_char_width[p1].m_char)
            p0 = p1;
        else
            while (p1 > p0 + 1) {
                p = (p0 + p1) / 2;
                if (ch >= s_char_width[p].m_char)
                    p0 = p;
                else
                    p1 = p;
            }
    }

    return s_char_width[p0].m_width;
}

inline int32_t getStringWidth(exlib::string& str)
{
    exlib::wstring32 str32 = utf8to32String(str);
    int32_t sz = 0;
    size_t len = str32.length();
    const exlib::wchar32* ptr = str32.c_str();

    for (size_t i = 0; i < len; i++) {
        exlib::wchar32 ch = ptr[i];

        if (ch == 0x1b) {
            for (i++; i < len && ptr[i] != 'm'; i++)
                ;
        } else
            sz += getCharWidth(ptr[i]);
    }

    return sz;
}

inline bool isSimpleValue(v8::Local<v8::Value> v)
{
    if (v.IsEmpty() || v->IsUndefined() || v->IsNull() || v->IsDate()
        || v->IsBoolean() || v->IsBooleanObject() || v->IsNumber() || v->IsNumberObject()
        || v->IsBigInt() || v->IsBigIntObject() || v->IsString() || v->IsStringObject()
        || v->IsRegExp() || v->IsPromise() || v->IsFunction() || v->IsSymbol())
        return true;

    return !v->IsObject();
}

inline void GetPropertyNames(v8::Local<v8::Object> o, QuickArray<exlib::string>& props)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();

    obj_ptr<Buffer_base> buf = Buffer_base::getInstance(o);
    if (buf) {
        int32_t len;

        buf->get_length(len);
        for (int32_t i = 0; i < len; i++) {
            char buf[32];
            int32_t n = sprintf(buf, "%ld", i);
            props.append(exlib::string(buf, n));
        }
    } else {
        JSArray keys = o->GetPropertyNames(_context);
        if (!keys.IsEmpty()) {
            int32_t len = keys->Length();
            for (int32_t i = 0; i < len; i++) {
                JSValue k = keys->Get(_context, i);
                exlib::string key;

                GetArgumentValue(isolate->m_isolate, k, key);
                props.append(key);
            }
        }
    }
}

inline exlib::string repeat(int32_t sz, bool bLine = false)
{
    // "────────────────────────────────", one "─" is "\xe2\x94\x80", there are 4 * 8 "─"
    static char s_line[] = "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
                           "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
                           "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
                           "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
                           "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
                           "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
                           "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
                           "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80";
    static char s_space[] = "                                ";
    const char* sp = bLine ? s_line : s_space;
    const int32_t bs = bLine ? 96 : 32;
    exlib::string str;

    if (bLine)
        sz *= 3;

    str.resize(sz);
    char* p = str.c_buffer();

    while (sz) {
        int32_t sz1 = sz;
        if (sz1 > bs)
            sz1 = bs;

        memcpy(p, sp, sz1);
        p += sz1;
        sz -= sz1;
    }

    return str;
}

inline bool is_number(exlib::string& str)
{
    const char* p = str.c_str();

    if (str.length() == 0)
        return false;

    if (str.length() > 1 && *p == '0')
        return false;

    while (qisdigit(*p++))
        ;
    return !*p;
}

inline bool compare_number(exlib::string& str, exlib::string& str1)
{
    if (str.length() > str1.length())
        return true;
    if (str.length() < str1.length())
        return false;
    return str > str1;
}

inline void append_split(std::vector<std::pair<QuickArray<exlib::string>&, int32_t>>& props,
    StringBuffer& buf, int32_t type)
{
    /**
     * "┌─", "─┬─", "─┐\n"
     * "├─", "─┼─", "─┤\n"
     * "└─", "─┴─", "─┘"
     */
    static const char* tab_string[][3] = {
        { "\xe2\x94\x8c\xe2\x94\x80", "\xe2\x94\x80\xe2\x94\xac\xe2\x94\x80", "\xe2\x94\x80\xe2\x94\x90\n" },
        { "\xe2\x94\x9c\xe2\x94\x80", "\xe2\x94\x80\xe2\x94\xbc\xe2\x94\x80", "\xe2\x94\x80\xe2\x94\xa4\n" },
        { "\xe2\x94\x94\xe2\x94\x80", "\xe2\x94\x80\xe2\x94\xb4\xe2\x94\x80", "\xe2\x94\x80\xe2\x94\x98" },
    };
    const char** ptrs = tab_string[type];

    buf.append(ptrs[0]);
    for (size_t i = 0; i < props.size(); i++) {
        auto& it = props[i];
        if (i > 0)
            buf.append(ptrs[1]);
        buf.append(repeat(it.second, true));
    }
    buf.append(ptrs[2]);
}

inline void append_value(exlib::string& str, int32_t width, int32_t max_width, StringBuffer& buf)
{
    buf.append(repeat((max_width - width) / 2));
    buf.append(str);
    buf.append(repeat((max_width - width) - (max_width - width) / 2));
}

exlib::string object_format(v8::Local<v8::Value> v, bool color, bool l2 = false)
{
    obj_ptr<Buffer_base> _buf = Buffer_base::getInstance(v);
    if (_buf)
        return json_format(v, color, 2);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);

    JSArray keys = obj->GetPropertyNames(_context);
    if (keys.IsEmpty())
        return "{}";

    v8::Local<v8::Function> toArray = v8::Local<v8::Function>::Cast(JSValue(obj->Get(_context, isolate->NewString("toArray"))));
    if (!toArray.IsEmpty() && toArray->IsFunction()) {
        TryCatch try_catch;
        v8::Local<v8::Value> v1;
        toArray->Call(_context, obj, 0, NULL).ToLocal(&v1);
        if (!IsEmpty(v1) && v1->IsObject()) {
            v = v1;
            obj = v8::Local<v8::Object>::Cast(v1);
        }
    }

    if (v->IsArray()) {
        v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(v);
        int32_t len = array->Length();

        if (len == 0)
            return "[]";
        else if (l2)
            return color_string(COLOR_CYAN, "[Array]", color);
        else {
            StringBuffer buf;
            int32_t len1 = len > 3 ? 3 : len;

            buf.append("[ ");
            for (int32_t i = 0; i < len1; i++) {
                if (i > 0)
                    buf.append(", ");

                JSValue v = array->Get(_context, i);
                if (isSimpleValue(v))
                    buf.append(json_format(v, color, 2));
                else
                    buf.append(object_format(v, color, true));
            }

            if (len == len1 + 1)
                buf.append(", ... 1 more item");
            else if (len > len1 + 1) {
                char str_buf[256];
                sprintf(str_buf, ", ... %d more items", len - len1);
                buf.append(str_buf);
            }
            buf.append(" ]");

            return buf.str();
        }
    }

    if (v->IsTypedArray()) {
        v8::Local<v8::TypedArray> array = v8::Local<v8::TypedArray>::Cast(v);
        int32_t len = (int32_t)array->Length();

        if (len == 0)
            return "[]";
        else if (l2)
            return color_string(COLOR_CYAN, "[TypedArray]", color);
        else {
            StringBuffer buf;
            int32_t len1 = len > 3 ? 3 : len;

            buf.append("[ ");
            for (int32_t i = 0; i < len1; i++) {
                if (i > 0)
                    buf.append(", ");

                JSValue v = array->Get(_context, i);
                buf.append(json_format(v, color, 2));
            }

            if (len == len1 + 1)
                buf.append(", ... 1 more item");
            else if (len > len1 + 1) {
                char str_buf[256];
                sprintf(str_buf, ", ... %d more items", len - len1);
                buf.append(str_buf);
            }
            buf.append(" ]");

            return buf.str();
        }
    }

    int32_t len = keys->Length();
    if (len == 0)
        return "{}";
    else if (l2 || len > 2)
        return color_string(COLOR_CYAN, "[Object]", color);

    StringBuffer buf;

    buf.append("{ ");
    for (int32_t i = 0; i < len; i++) {
        if (i > 0)
            buf.append(", ");

        JSValue v = keys->Get(_context, i);
        string_format(buf, v, false);

        buf.append(": ");

        v = obj->Get(_context, v);
        if (isSimpleValue(v))
            buf.append(json_format(v, color, 2));
        else
            buf.append(object_format(v, color, true));
    }
    buf.append(" }");

    return buf.str();
}

exlib::string table_format(v8::Local<v8::Value> obj, v8::Local<v8::Array> fields, bool color, bool encode_string)
{
    if (isSimpleValue(obj))
        return json_format(obj, color, 2);

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(obj);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();

    bool b_has_prop = false;
    std::vector<std::pair<QuickArray<exlib::string>&, int32_t>> props;

    std::map<exlib::string, QuickArray<exlib::string>> prop_cols;
    QuickArray<exlib::string> value_cols;
    QuickArray<exlib::string> idx_cols;

    idx_cols.append("(index)");
    GetPropertyNames(o, idx_cols);
    props.push_back(std::pair<QuickArray<exlib::string>&, int32_t>(idx_cols, 0));

    if (!fields.IsEmpty()) {
        b_has_prop = true;
        int32_t len = fields->Length();
        for (int32_t i = 0; i < len; i++) {
            JSValue k = fields->Get(_context, i);
            exlib::string row_key;

            GetArgumentValue(isolate->m_isolate, k, row_key);

            auto it = prop_cols.find(row_key);
            if (it == prop_cols.end()) {
                it = prop_cols.insert(std::make_pair(row_key, QuickArray<exlib::string>())).first;
                it->second.append(row_key);
                props.push_back(std::pair<QuickArray<exlib::string>&, int32_t>(it->second, 0));
            }
        }
    }

    if (!b_has_prop)
        value_cols.append("Values");

    for (int32_t i = 1; i < (int32_t)idx_cols.size(); i++) {
        JSValue v = o->Get(_context, isolate->NewString(idx_cols[i]));
        if (isSimpleValue(v)) {
            if (!b_has_prop) {
                value_cols.resize(i);
                if (!encode_string && (v->IsString() || v->IsStringObject())) {
                    exlib::string val;
                    GetArgumentValue(isolate->m_isolate, v, val);
                    value_cols.append(val);
                } else
                    value_cols.append(json_format(v, color, 2));
            }
        } else {
            v8::Local<v8::Object> ro = v8::Local<v8::Object>::Cast(v);
            QuickArray<exlib::string> row_props;

            GetPropertyNames(ro, row_props);
            for (int32_t ri = 0; ri < (int32_t)row_props.size(); ri++) {
                exlib::string row_key = row_props[ri];

                auto it = prop_cols.find(row_key);
                if (it == prop_cols.end()) {
                    if (b_has_prop)
                        continue;

                    it = prop_cols.insert(std::make_pair(row_key, QuickArray<exlib::string>())).first;
                    it->second.append(row_key);
                    props.push_back(std::pair<QuickArray<exlib::string>&, int32_t>(it->second, 0));
                }

                JSValue rv = ro->Get(_context, isolate->NewString(row_key));
                exlib::string row_value;
                if (isSimpleValue(rv)) {
                    if (!encode_string && (rv->IsString() || rv->IsStringObject()))
                        GetArgumentValue(isolate->m_isolate, rv, row_value);
                    else
                        row_value = json_format(rv, color, 2);
                } else
                    row_value = object_format(rv, color);

                it->second.resize(i);
                it->second.append(row_value);
            }
        }
    }

    if (!b_has_prop) {
        if (value_cols.size() > 1)
            props.push_back(std::pair<QuickArray<exlib::string>&, int32_t>(value_cols, 0));

        size_t sorted_pos = 1;
        for (size_t i = 1; i < props.size(); i++) {
            if (is_number(props[i].first[0])) {
                for (size_t j = i; j > sorted_pos; j--)
                    props[j].swap(props[j - 1]);

                for (size_t j = sorted_pos; j > 1; j--)
                    if (compare_number(props[j - 1].first[0], props[j].first[0]))
                        props[j].swap(props[j - 1]);
                    else
                        break;

                sorted_pos++;
            }
        }
    }

    QuickArray<QuickArray<int32_t>> cell_width;

    cell_width.resize(idx_cols.size());
    for (size_t i = 0; i < cell_width.size(); i++) {
        size_t col_count = props.size();
        QuickArray<int32_t>& col_line = cell_width[i];
        col_line.resize(col_count);

        for (size_t j = 0; j < col_count; j++)
            col_line[j] = 0;
    }

    for (size_t i = 0; i < props.size(); i++) {
        auto& it = props[i];
        int32_t sz = 0;
        cell_width[0][i] = sz;

        QuickArray<exlib::string>& col = it.first;
        size_t col_sz = col.size();

        for (size_t j = 0; j < col_sz; j++) {
            int32_t sz1 = getStringWidth(col[j]);
            cell_width[j][i] = sz1;
            if (sz1 > sz)
                sz = sz1;
        }

        it.second = sz;
    }

    StringBuffer buf;

    append_split(props, buf, 0);
    for (int32_t i = 0; i < (int32_t)idx_cols.size(); i++) {
        buf.append("\xe2\x94\x82\x20"); // "│ "
        for (size_t j = 0; j < props.size(); j++) {
            if (j > 0)
                buf.append("\x20\xe2\x94\x82\x20"); // " │ "

            auto& it = props[j];
            int32_t sz = cell_width[i][j];
            if (sz > 0)
                append_value(it.first[i], sz, it.second, buf);
            else
                buf.append(repeat(it.second));
        }
        buf.append("\x20\xe2\x94\x82\n"); // " │\n"

        if (i == 0)
            append_split(props, buf, 1);
    }
    append_split(props, buf, 2);

    return buf.str();
}

result_t util_base::inspect(v8::Local<v8::Value> obj, v8::Local<v8::Object> options, exlib::string& retVal)
{
    Isolate* isolate = Isolate::current();

    bool colors = false;
    GetConfigValue(isolate->m_isolate, options, "colors", colors, true);

    bool table = false;
    GetConfigValue(isolate->m_isolate, options, "table", table, true);

    if (table) {
        v8::Local<v8::Array> fields;
        GetConfigValue(isolate->m_isolate, options, "fields", fields, true);

        bool encode_string = true;
        GetConfigValue(isolate->m_isolate, options, "encode_string", encode_string, true);

        retVal = table_format(obj, fields, colors, encode_string);
    } else {
        int32_t depth = 2;
        GetConfigValue(isolate->m_isolate, options, "depth", depth, true);

        retVal = json_format(obj, colors, depth);
    }

    return 0;
}

}