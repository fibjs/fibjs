/*
 * db_format.h
 *
 *  Created on: May 19, 2021
 *      Author: lion
 */

#pragma once

#include "object.h"

namespace fibjs {

template <class impl>
class db_format {
public:
    static result_t format(exlib::string method, v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        if (method == "createTable")
            return format_createTable(opts, retVal);
        else if (method == "dropTable")
            return format_dropTable(opts, retVal);
        else if (method == "createIndex")
            return format_createIndex(opts, retVal);
        else if (method == "dropIndex")
            return format_dropIndex(opts, retVal);
        else if (method == "find")
            return format_find(opts, retVal);
        else if (method == "count")
            return format_count(opts, retVal);
        else if (method == "update")
            return format_update(opts, retVal);
        else if (method == "insert")
            return format_insert(opts, retVal);
        else if (method == "remove")
            return format_remove(opts, retVal);

        return CHECK_ERROR(Runtime::setError("db: Unknown method."));
    }

    static result_t format(const char* sql, OptArgs args, exlib::string& retVal)
    {
        exlib::string str;
        const char *p, *p1;
        int32_t cnt = 0;
        int32_t argc = args.Length();

        while (*sql) {
            p = p1 = sql;
            while (*p1 && *p1 != '?')
                p1++;

            str.append(p, p1 - p);

            if (*p1) {
                p1++;

                if (cnt < argc) {
                    v8::Local<v8::Value> v = args[cnt];

                    if (v->IsFunction())
                        return CHECK_ERROR(CALL_E_INVALIDARG);

                    appendValue(str, v);
                } else
                    str.append(1, '?');

                cnt++;
            }

            sql = p1;
        }

        retVal = str;
        return 0;
    }

private:
    static exlib::string escape_field(const char* str, int32_t sz)
    {
        return impl::escape_field(str, sz);
    }

    static exlib::string escape_field(exlib::string s)
    {
        return escape_field(s.c_str(), (int32_t)s.length());
    }

    static exlib::string escape_field(v8::Local<v8::Value> v)
    {
        Isolate* isolate = Isolate::current();
        return escape_field(isolate->toString(v));
    }

    static void appendValue(exlib::string& str, v8::Local<v8::Value>& v)
    {
        obj_ptr<Buffer_base> bin = Buffer_base::getInstance(v);

        if (bin)
            str.append(impl::escape_binary(bin));
        else if (v->IsArray()) {
            v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
            int32_t len = a->Length();
            int32_t i;

            str += '(';

            for (i = 0; i < len; i++) {
                JSValue v1 = a->Get(i);

                if (i > 0)
                    str += ',';
                appendValue(str, v1);
            }

            str += ')';
        } else if (v->IsNumber() || v->IsNumberObject()
            || v->IsBigInt() || v->IsBigIntObject()) {
            Isolate* isolate = Isolate::current();
            str.append(isolate->toString(v));
        } else if (v->IsUndefined() || v->IsNull()) {
            str.append("NULL", 4);
        } else if (v->IsDate())
            str.append(impl::escape_date(v));
        else
            str.append(impl::escape_string(Isolate::current()->toString(v)));
    }

    static result_t format_where(v8::Local<v8::Array> o, bool bAnd, exlib::string& retVal, bool& retAnd)
    {
        exlib::string str;
        int32_t len = o->Length();
        int32_t i;
        result_t hr;
        bool rAnd;

        for (i = 0; i < len; i++) {
            exlib::string s;

            JSValue v = o->Get(i);
            if (!IsJSObject(v))
                return CHECK_ERROR(Runtime::setError("db: The argument of the [or/and] operation must be an object."));

            rAnd = bAnd;
            hr = format_where(v, s, rAnd);
            if (hr < 0)
                return hr;

            if (rAnd != bAnd)
                str.append('(' + s + ')');
            else
                str.append(s);

            if (i + 1 < len) {
                retAnd = bAnd;
                str.append(bAnd ? " AND " : " OR ");
            }
        }

        retVal = str;
        return 0;
    }

    static result_t format_where(v8::Local<v8::Value> val, exlib::string& retVal, bool& retAnd)
    {
        Isolate* isolate = Isolate::current();

        if (val->IsString() || val->IsStringObject()) {
            retVal = isolate->toString(val);
            return 0;
        }

        if (val->IsArray())
            return format_where(v8::Local<v8::Array>::Cast(val), true, retVal, retAnd);

        if (!IsJSObject(val))
            return CHECK_ERROR(CALL_E_INVALIDARG);

        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(val);
        exlib::string str;
        JSArray ks = o->GetPropertyNames();
        int32_t len = ks->Length();
        int32_t i;
        bool bAnd = true;

        exlib::string or_str = escape_field("$or", 3);
        exlib::string and_str = escape_field("$and", 4);

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(i);
            JSValue v = o->Get(k);
            JSValue v1;
            bool bBetween = false;
            bool bIn = false;
            bool bField = false;
            exlib::string op("=");
            exlib::string key;

            if (v->IsFunction())
                return CHECK_ERROR(CALL_E_INVALIDARG);

            key = escape_field(k);
            while (len == 1 && (!qstrcmp(key.c_str(), or_str.c_str(), 5) || !qstrcmp(key.c_str(), and_str.c_str(), 6))) {
                bAnd = key[2] == 'a';

                if (v->IsArray())
                    return format_where(v8::Local<v8::Array>::Cast(v), bAnd, retVal, retAnd);
                else if (IsJSObject(v)) {
                    o = v8::Local<v8::Object>::Cast(v);
                    ks = o->GetPropertyNames();
                    len = ks->Length();
                    if (len == 0)
                        break;

                    k = ks->Get(0);
                    v = o->Get(k);

                    key = escape_field(k);
                } else
                    return CHECK_ERROR(Runtime::setError("db: The argument of the [or/and] operation must be an object or an array."));
            }

            if (IsJSObject(v)) {
                v8::Local<v8::Object> ops = v8::Local<v8::Object>::Cast(v);
                JSArray opks = ops->GetPropertyNames();
                int32_t oplen = opks->Length();

                if (oplen != 1)
                    return CHECK_ERROR(Runtime::setError("db: The condition of the field " + key + " is illegal."));

                JSValue opv = opks->Get(0);
                v8::String::Utf8Value _ops(isolate->m_isolate, opv);

                if (!*_ops)
                    return CHECK_ERROR(Runtime::setError("db: The condition of the field " + key + " is illegal."));

                if (!qstrcmp(*_ops, "$eq"))
                    op = "=";
                else if (!qstrcmp(*_ops, "$ne"))
                    op = "<>";
                else if (!qstrcmp(*_ops, "$gt"))
                    op = ">";
                else if (!qstrcmp(*_ops, "$gte"))
                    op = ">=";
                else if (!qstrcmp(*_ops, "$lt"))
                    op = "<";
                else if (!qstrcmp(*_ops, "$lte"))
                    op = "<=";
                else if (!qstrcmp(*_ops, "$like"))
                    op = " LIKE ";
                else if (!qstrcmp(*_ops, "$not_like"))
                    op = " NOT LIKE ";
                else if (!qstrcmp(*_ops, "$in")) {
                    bIn = true;
                    op = " IN ";
                } else if (!qstrcmp(*_ops, "$not_in")) {
                    bIn = true;
                    op = " NOT IN ";
                } else if (!qstrcmp(*_ops, "$between")) {
                    bBetween = true;
                    op = " BETWEEN ";
                } else if (!qstrcmp(*_ops, "$not_between")) {
                    bBetween = true;
                    op = " NOT BETWEEN ";
                } else if (!qstrcmp(*_ops, "$field")) {
                    bField = true;
                } else
                    return CHECK_ERROR(Runtime::setError("db: The condition of the field " + key + " is illegal."));

                v = ops->Get(opv);

                if (!bField && !bIn && !bBetween && IsJSObject(v)) {
                    ops = v8::Local<v8::Object>::Cast(v);
                    opks = ops->GetPropertyNames();
                    oplen = opks->Length();

                    if (oplen != 1)
                        return CHECK_ERROR(Runtime::setError("db: The condition of the field " + key + " is illegal."));

                    opv = opks->Get(0);
                    v8::String::Utf8Value _ops(isolate->m_isolate, opv);

                    if (qstrcmp(*_ops, "$field"))
                        return CHECK_ERROR(Runtime::setError("db: The condition of the field " + key + " is illegal."));

                    bField = true;
                    v = ops->Get(opv);
                }
            }

            if (bField) {
                if (!v->IsString() && !v->IsStringObject())
                    return CHECK_ERROR(Runtime::setError("db: field must be a string."));

                str.append(key + op + escape_field(v));
            } else {
                if (bIn) {
                    if (!IsJSObject(v) && !v->IsArray())
                        return CHECK_ERROR(Runtime::setError("db: The argument of the [in/not_in] operation must be an array or a query."));
                } else if (bBetween) {
                    if (!v->IsArray())
                        return CHECK_ERROR(Runtime::setError("db: The argument of the [between/not_between] operation must be an array."));

                    JSArray vals = v8::Local<v8::Array>::Cast(v);
                    int32_t vals_len = vals->Length();
                    if (vals_len != 2)
                        return CHECK_ERROR(Runtime::setError("db: The argument size of the [between/not_between] operation must be 2."));

                    v = vals->Get(0);
                    v1 = vals->Get(1);
                } else if (IsJSObject(v))
                    return CHECK_ERROR(Runtime::setError("db: The argument of the [" + op + "] operation can not be a object."));

                if (IsJSObject(v)) {
                    exlib::string sub_query;
                    result_t hr = format("find", v8::Local<v8::Object>::Cast(v), sub_query);
                    if (hr < 0)
                        return hr;

                    str.append(key + op + '(' + sub_query + ')');
                } else {
                    str.append(key + op);
                    appendValue(str, v);
                    if (bBetween) {
                        str.append(" AND ");
                        appendValue(str, v1);
                    }
                }
            }

            if (i + 1 < len) {
                retAnd = bAnd;
                str.append(bAnd ? " AND " : " OR ");
            }
        }

        retVal = str;
        return 0;
    }

    static result_t format_name_list(v8::Local<v8::Value> v, exlib::string& retVal)
    {
        exlib::string table;

        if (v->IsArray()) {
            v8::Local<v8::Array> tables = v8::Local<v8::Array>::Cast(v);

            int32_t len = tables->Length();
            int32_t i;

            if (len > 0) {
                for (i = 0; i < len; i++) {
                    table.append(escape_field(tables->Get(i)));
                    if (i + 1 < len)
                        table.append(", ", 2);
                }
            }
        } else if (IsJSObject(v)) {
            v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
            JSArray ks = o->GetPropertyNames();
            int32_t len = ks->Length();
            int32_t i;

            for (i = 0; i < len; i++) {
                JSValue k = ks->Get(i);

                table.append(escape_field(k) + " AS " + escape_field(o->Get(k)));
                if (i + 1 < len)
                    table.append(", ", 2);
            }
        } else
            return CALL_E_TYPEMISMATCH;

        retVal = table;
        return 0;
    }

    static result_t format_name(v8::Local<v8::Object> opts, exlib::string name, exlib::string& retVal)
    {
        Isolate* isolate = Isolate::current();
        result_t hr;
        v8::Local<v8::Value> v;

        hr = GetConfigValue(isolate->m_isolate, opts, name.c_str(), v);
        if (hr < 0)
            return hr;

        if (v->IsString() || v->IsStringObject()) {
            retVal = escape_field(v);
            return 0;
        } else
            return format_name_list(v, retVal);
    }

    static result_t format_order(Isolate* isolate, v8::Local<v8::Array> orders, exlib::string& retVal)
    {
        int32_t len = orders->Length();
        int32_t i;
        exlib::string str;

        if (len > 0) {
            for (i = 0; i < len; i++) {
                JSValue ov = orders->Get(i);
                v8::String::Utf8Value s(isolate->m_isolate, ov);
                exlib::string key;
                bool desc = false;

                if (s.length() == 0)
                    return CHECK_ERROR(Runtime::setError("db: Field name cannot be empty."));
                if (**s == '-') {
                    if (s.length() == 1)
                        return CHECK_ERROR(Runtime::setError("db: Field name cannot be empty."));
                    key = escape_field(*s + 1, s.length() - 1);
                    desc = true;
                } else
                    key = escape_field(*s, s.length());

                str.append(key);
                if (desc)
                    str.append(" DESC");

                if (i + 1 < len)
                    str.append(", ", 2);
            }
        }

        retVal = str;

        return 0;
    }

    static result_t format_find(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str("SELECT ");
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Value> v;
        v8::Local<v8::Array> a;
        exlib::string table;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        hr = GetConfigValue(isolate->m_isolate, opts, "fields", v);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            str.append("*");
        else if (v->IsString() || v->IsStringObject()) {
            str.append(isolate->toString(v));
        } else {
            exlib::string str1;

            hr = format_name_list(v, str1);
            if (hr < 0)
                return hr;

            str.append(str1);
        }

        str.append(" FROM " + table);

        hr = GetConfigValue(isolate->m_isolate, opts, "where", v);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            exlib::string _where;
            bool retAnd;

            hr = format_where(v, _where, retAnd);
            if (hr < 0)
                return hr;

            if (!_where.empty())
                str.append(" WHERE " + _where);
        }

        v8::Local<v8::Array> orders;
        hr = GetConfigValue(isolate->m_isolate, opts, "order", orders, true);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            exlib::string order;

            if (hr < 0)
                return hr;

            hr = format_order(isolate, orders, order);
            if (hr < 0)
                return hr;

            if (!order.empty()) {
                str.append(" ORDER BY ");
                str.append(order);
            }
        }

        int64_t limit;
        hr = GetConfigValue(isolate->m_isolate, opts, "limit", limit);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            if (hr < 0)
                return hr;

            str.append(" LIMIT " + std::to_string(limit));
        }

        int64_t skip;
        hr = GetConfigValue(isolate->m_isolate, opts, "skip", skip);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            if (hr < 0)
                return hr;

            str.append(" OFFSET " + std::to_string(skip));
        }

        retVal = str;
        return 0;
    }

    static result_t format_count(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Value> v;
        exlib::string table;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        str.append("SELECT COUNT(*) FROM " + table);

        hr = GetConfigValue(isolate->m_isolate, opts, "where", v);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            exlib::string _where;
            bool retAnd;

            hr = format_where(v, _where, retAnd);
            if (hr < 0)
                return hr;

            if (!_where.empty())
                str.append(" WHERE " + _where);
        }

        int64_t skip;
        hr = GetConfigValue(isolate->m_isolate, opts, "skip", skip);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            if (hr < 0)
                return hr;

            str.append(" SKIP " + std::to_string(skip));
        }

        int64_t limit;
        hr = GetConfigValue(isolate->m_isolate, opts, "limit", limit);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            if (hr < 0)
                return hr;

            str.append(" LIMIT " + std::to_string(limit));
        }

        retVal = str;
        return 0;
    }

    static result_t format_update(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        Isolate* isolate = Isolate::current();
        exlib::string table;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        str.append("UPDATE " + table + " SET ");

        v8::Local<v8::Object> o;
        hr = GetConfigValue(isolate->m_isolate, opts, "values", o, true);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            return CHECK_ERROR(Runtime::setError("db: No updated values specified."));
        if (hr < 0)
            return hr;

        exlib::string _values;

        JSArray ks = o->GetPropertyNames();
        int32_t len = ks->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(i);
            JSValue v = o->Get(k);

            _values.append(escape_field(k) + "=");
            appendValue(_values, v);

            if (i + 1 < len)
                _values.append(", ", 2);
        }

        if (_values.empty())
            return CHECK_ERROR(Runtime::setError("db: No updated values specified."));
        str.append(_values);

        v8::Local<v8::Value> v;
        hr = GetConfigValue(isolate->m_isolate, opts, "where", v);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            exlib::string _where;
            bool retAnd;

            hr = format_where(v, _where, retAnd);
            if (hr < 0)
                return hr;

            if (!_where.empty())
                str.append(" WHERE " + _where);
        }

        retVal = str;
        return 0;
    }

    static result_t format_insert(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        Isolate* isolate = Isolate::current();
        exlib::string table;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        str.append("INSERT INTO " + table + " (");

        v8::Local<v8::Object> o;
        hr = GetConfigValue(isolate->m_isolate, opts, "values", o, true);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            return CHECK_ERROR(Runtime::setError("db: No updated values specified."));
        if (hr < 0)
            return hr;

        exlib::string _fields;
        exlib::string _values;

        JSArray ks = o->GetPropertyNames();
        int32_t len = ks->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(i);
            JSValue v = o->Get(k);

            _fields.append(escape_field(k));
            appendValue(_values, v);

            if (i + 1 < len) {
                _fields.append(", ", 2);
                _values.append(", ", 2);
            }
        }

        if (_values.empty())
            return CHECK_ERROR(Runtime::setError("db: No updated values specified."));
        str.append(_fields);
        str.append(") VALUES (");
        str.append(_values);
        str.append(1, ')');

        retVal = str;
        return 0;
    }

    static result_t format_remove(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        Isolate* isolate = Isolate::current();
        exlib::string table;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        str.append("DELETE FROM " + table);

        v8::Local<v8::Value> v;
        hr = GetConfigValue(isolate->m_isolate, opts, "where", v);
        if (hr != CALL_E_PARAMNOTOPTIONAL) {
            exlib::string _where;
            bool retAnd;

            hr = format_where(v, _where, retAnd);
            if (hr < 0)
                return hr;

            if (!_where.empty())
                str.append(" WHERE " + _where);
        }

        retVal = str;
        return 0;
    }

    static result_t format_createTable(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        Isolate* isolate = Isolate::current();
        exlib::string table;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        str.append("CREATE TABLE " + table + "(");

        v8::Local<v8::Object> o;
        hr = GetConfigValue(isolate->m_isolate, opts, "fields", o, true);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            return CHECK_ERROR(Runtime::setError("db: No table fields specified."));
        if (hr < 0)
            return hr;

        exlib::string _fields;

        JSArray ks = o->GetPropertyNames();
        int32_t len = ks->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(i);
            JSValue v = o->Get(k);
            exlib::string type;
            v8::Local<v8::Object> prop;

            hr = GetArgumentValue(v, type, true);
            if (hr < 0) {
                if (!IsJSObject(v))
                    return CHECK_ERROR(Runtime::setError("db: No field type specified."));
                prop = v8::Local<v8::Object>::Cast(v);

                hr = GetConfigValue(isolate->m_isolate, prop, "type", type, true);
                if (hr == CALL_E_PARAMNOTOPTIONAL)
                    return CHECK_ERROR(Runtime::setError("db: No field type specified."));
                if (hr < 0)
                    return hr;
            }

            _fields.append(escape_field(k));
            _fields.append(1, ' ');

            if (type == "text") {
                int32_t size = 0;

                if (!prop.IsEmpty()) {
                    hr = GetConfigValue(isolate->m_isolate, prop, "size", size, true);
                    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                        return hr;
                }

                if (size > 0) {
                    char str[32];

                    sprintf(str, "%d", size);
                    _fields.append("VARCHAR(");
                    _fields.append((const char*)str);
                    _fields.append(1, ')');
                } else
                    _fields.append(impl::data_type().TEXT);
            } else if (type == "number") {
                int32_t size = 8;

                if (!prop.IsEmpty()) {
                    hr = GetConfigValue(isolate->m_isolate, prop, "size", size, true);
                    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                        return hr;
                }

                if (size == 4)
                    _fields.append(impl::data_type().FLOAT);
                else if (size == 8)
                    _fields.append(impl::data_type().DOUBLE);
                else
                    return CHECK_ERROR(Runtime::setError("db: Number size must be 4|8."));
            } else if (type == "integer") {
                int32_t size = 4;

                if (!prop.IsEmpty()) {
                    hr = GetConfigValue(isolate->m_isolate, prop, "size", size, true);
                    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                        return hr;
                }

                if (size == 2)
                    _fields.append("SMALLINT");
                else if (size == 4)
                    _fields.append("INT");
                else if (size == 8)
                    _fields.append("BIGINT");
                else
                    return CHECK_ERROR(Runtime::setError("db: Integer size must be 2|4|8."));
            } else if (type == "date") {
                bool _time = false;

                if (!prop.IsEmpty()) {
                    hr = GetConfigValue(isolate->m_isolate, prop, "time", _time, true);
                    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                        return hr;
                }

                _fields.append(_time ? impl::data_type().DATETIME : "DATE");
            } else if (type == "binary") {
                bool big = 0;

                if (!prop.IsEmpty()) {
                    hr = GetConfigValue(isolate->m_isolate, prop, "big", big, true);
                    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                        return hr;
                }

                _fields.append(big ? impl::data_type().LONGBLOB : impl::data_type().BLOB);
            } else
                _fields.append(type);

            if (!prop.IsEmpty()) {
                bool required = false;
                hr = GetConfigValue(isolate->m_isolate, prop, "required", required, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;

                if (hr != CALL_E_PARAMNOTOPTIONAL)
                    _fields.append(required ? " NOT NULL" : " NULL");

                v = prop->Get(isolate->NewString("defaultValue"));
                if (!IsEmpty(v)) {
                    _fields.append(" DEFAULT ");
                    appendValue(_fields, v);
                }

                bool unique = false;
                hr = GetConfigValue(isolate->m_isolate, prop, "unique", unique, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;

                if (unique)
                    _fields.append(" UNIQUE");

                bool key = false;
                hr = GetConfigValue(isolate->m_isolate, prop, "key", key, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;

                if (key)
                    _fields.append(" PRIMARY KEY");
            }

            if (i + 1 < len)
                _fields.append(", ", 2);
        }

        if (_fields.empty())
            return CHECK_ERROR(Runtime::setError("db: No table fields specified."));
        str.append(_fields);
        str.append(1, ')');

        retVal = str;
        return 0;
    }

    static result_t format_dropTable(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        exlib::string table;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        str.append("DROP TABLE " + table);

        retVal = str;
        return 0;
    }

    static result_t format_createIndex(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        Isolate* isolate = Isolate::current();
        exlib::string table, index;

        hr = format_name(opts, "table", table);
        if (hr < 0)
            return hr;

        hr = format_name(opts, "index", index);
        if (hr < 0)
            return hr;

        str.append("CREATE INDEX " + index + " ON " + table + "(");

        v8::Local<v8::Array> keys;
        hr = GetConfigValue(isolate->m_isolate, opts, "keys", keys, true);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            return CHECK_ERROR(Runtime::setError("db: No index keys specified."));
        if (hr < 0)
            return hr;

        exlib::string _keys;

        hr = format_order(isolate, keys, _keys);
        if (hr < 0)
            return hr;

        if (_keys.empty())
            return CHECK_ERROR(Runtime::setError("db: No index keys specified."));
        str.append(_keys);
        str.append(1, ')');

        retVal = str;
        return 0;
    }

    static result_t format_dropIndex(v8::Local<v8::Object> opts, exlib::string& retVal)
    {
        result_t hr;
        exlib::string str;
        exlib::string index;

        hr = format_name(opts, "index", index);
        if (hr < 0)
            return hr;

        str.append("DROP INDEX " + index);

        retVal = str;
        return 0;
    }
};
}