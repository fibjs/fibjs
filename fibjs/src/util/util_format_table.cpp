/*
 * util_format_table.cpp
 *
 *  Created on: Aug 16, 2021
 *      Author: lion
 */

#include "util.h"

namespace fibjs {

inline int32_t getCharWidth(int32_t ch)
{
    const int eaw = u_getIntPropertyValue(ch, UCHAR_EAST_ASIAN_WIDTH);
    switch (eaw) {
    case U_EA_FULLWIDTH:
    case U_EA_WIDE:
        return 2;
    case U_EA_AMBIGUOUS:
    case U_EA_NEUTRAL:
        if (u_hasBinaryProperty(ch, UCHAR_EMOJI_PRESENTATION)) {
            return 2;
        }
    case U_EA_HALFWIDTH:
    case U_EA_NARROW:
    default:
        const auto zero_width_mask = U_GC_CC_MASK | U_GC_CF_MASK | U_GC_ME_MASK | U_GC_MN_MASK;
        if (ch != 0x00AD && ((U_MASK(u_charType(ch)) & zero_width_mask) || u_hasBinaryProperty(ch, UCHAR_EMOJI_MODIFIER))) {
            return 0;
        }
        return 1;
    }
}

inline int32_t getStringWidth(exlib::string& str)
{
    exlib::wstring32 str32 = utf8to32String(str);
    int32_t sz = 0;
    size_t len = str32.length();
    const char32_t* ptr = str32.c_str();

    for (size_t i = 0; i < len; i++) {
        char32_t ch = ptr[i];

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
    Isolate* isolate = Isolate::current(o);
    v8::Local<v8::Context> _context = isolate->context();

    obj_ptr<Buffer_base> buf = Buffer_base::getInstance(o);
    if (buf) {
        int32_t len = Buffer::Cast(buf)->length();

        for (int32_t i = 0; i < len; i++) {
            char buf[32];
            int32_t n = snprintf(buf, sizeof(buf), "%d", i);
            props.append(exlib::string(buf, n));
        }
    } else {
        JSArray keys = o->GetPropertyNames(_context);
        if (!keys.IsEmpty()) {
            int32_t len = keys->Length();
            for (int32_t i = 0; i < len; i++) {
                JSValue k = keys->Get(_context, i);
                exlib::string key;

                GetArgumentValue(isolate, k, key);
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
    char* p = str.data();

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

exlib::string object_format(Isolate* isolate, v8::Local<v8::Value> v, bool color, bool l2 = false)
{
    if (IsJSBuffer(v))
        return json_format(isolate, v, color);

    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Object> obj = v.As<v8::Object>();

    JSArray keys = obj->GetPropertyNames(_context);
    if (keys.IsEmpty())
        return "{}";

    v8::Local<v8::Value> v_toArray = obj->Get(_context, isolate->NewString("toArray")).FromMaybe(v8::Local<v8::Value>());
    if (!v_toArray.IsEmpty() && v_toArray->IsFunction()) {
        v8::Local<v8::Function> toArray = v_toArray.As<v8::Function>();
        TryCatch try_catch;
        v8::Local<v8::Value> v1 = toArray->Call(_context, obj, 0, NULL).FromMaybe(v8::Local<v8::Value>());
        if (!IsEmpty(v1) && v1->IsObject()) {
            v = v1;
            obj = v1.As<v8::Object>();
        }
    }

    if (v->IsArray()) {
        v8::Local<v8::Array> array = v.As<v8::Array>();
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
                    buf.append(json_format(isolate, v, color));
                else
                    buf.append(object_format(isolate, v, color, true));
            }

            if (len == len1 + 1)
                buf.append(", ... 1 more item");
            else if (len > len1 + 1) {
                char str_buf[256];
                snprintf(str_buf, sizeof(str_buf), ", ... %d more items", len - len1);
                buf.append(str_buf);
            }
            buf.append(" ]");

            return buf.str();
        }
    }

    if (v->IsTypedArray()) {
        v8::Local<v8::TypedArray> array = v.As<v8::TypedArray>();
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
                buf.append(json_format(isolate, v, color));
            }

            if (len == len1 + 1)
                buf.append(", ... 1 more item");
            else if (len > len1 + 1) {
                char str_buf[256];
                snprintf(str_buf, sizeof(str_buf), ", ... %d more items", len - len1);
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
        string_format(isolate, buf, v);

        buf.append(": ");

        v = obj->Get(_context, v);
        if (isSimpleValue(v))
            buf.append(json_format(isolate, v, color));
        else
            buf.append(object_format(isolate, v, color, true));
    }
    buf.append(" }");

    return buf.str();
}

exlib::string table_format(Isolate* isolate, v8::Local<v8::Value> obj, v8::Local<v8::Array> fields, bool color, bool encode_string)
{
    if (isSimpleValue(obj))
        return json_format(isolate, obj, color);

    v8::Local<v8::Object> o = obj.As<v8::Object>();

    v8::Local<v8::Context> _context = isolate->context();

    bool b_has_prop = false;
    std::vector<std::pair<QuickArray<exlib::string>&, int32_t>> props;

    std::unordered_map<exlib::string, QuickArray<exlib::string>> prop_cols;
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

            GetArgumentValue(isolate, k, row_key);

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
                    GetArgumentValue(isolate, v, val);
                    value_cols.append(val);
                } else
                    value_cols.append(json_format(isolate, v, color));
            }
        } else {
            v8::Local<v8::Object> ro = v.As<v8::Object>();
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
                        GetArgumentValue(isolate, rv, row_value);
                    else
                        row_value = json_format(isolate, rv, color);
                } else
                    row_value = object_format(isolate, rv, color);

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

    bool table = false;
    if (!options.IsEmpty())
        GetConfigValue(isolate, options, "table", table, true);

    if (table) {
        bool colors = false;
        if (!options.IsEmpty())
            GetConfigValue(isolate, options, "colors", colors, true);

        v8::Local<v8::Array> fields;
        if (!options.IsEmpty())
            GetConfigValue(isolate, options, "fields", fields, true);

        bool encode_string = true;
        if (!options.IsEmpty())
            GetConfigValue(isolate, options, "encode_string", encode_string, true);

        retVal = table_format(isolate, obj, fields, colors, encode_string);
    } else {
        bool colors = true;
        if (!options.IsEmpty())
            GetConfigValue(isolate, options, "colors", colors, true);

        int32_t depth = DEFAULT_DEPTH;
        if (!options.IsEmpty())
            GetConfigValue(isolate, options, "depth", depth, true);

        int32_t maxArrayLength = DEFAULT_MAX_ARRAY_LENGTH;
        if (!options.IsEmpty())
            GetConfigValue(isolate, options, "maxArrayLength", maxArrayLength, true);

        int32_t maxStringLength = DEFAULT_MAX_STRING_LENGTH;
        if (!options.IsEmpty())
            GetConfigValue(isolate, options, "maxStringLength", maxStringLength, true);

        retVal = json_format(isolate, obj, colors, depth, maxArrayLength, maxStringLength);
    }

    return 0;
}

}