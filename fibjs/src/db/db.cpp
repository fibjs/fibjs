/*
 * db.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/db.h"
#include "ifs/Buffer.h"

namespace fibjs {

DECLARE_MODULE(db);

result_t db_format(exlib::string method, v8::Local<v8::Object> opts, bool mysql, bool mssql,
    exlib::string& retVal);

result_t db_base::open(exlib::string connString, obj_ptr<object_base>& retVal, AsyncEvent* ac)
{
    if (!qstrcmp(connString.c_str(), "mysql:", 6))
        return openMySQL(connString, (obj_ptr<MySQL_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "mssql:", 6))
        return openMSSQL(connString, (obj_ptr<MSSQL_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "sqlite:", 7))
        return openSQLite(connString, (obj_ptr<SQLite_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "redis:", 6))
        return openRedis(connString, (obj_ptr<Redis_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "mongodb:", 8))
        return openMongoDB(connString, (obj_ptr<MongoDB_base>&)retVal, ac);

    if (!qstrcmp(connString.c_str(), "leveldb:", 8))
        return openLevelDB(connString, (obj_ptr<LevelDB_base>&)retVal, ac);

    return CHECK_ERROR(CALL_E_INVALIDARG);
}

inline exlib::string _escape(const char* str, int32_t sz, bool mysql)
{
    int32_t len, l;
    const char* src;
    char* bstr;
    char ch;
    exlib::string retVal;

    for (len = 0, src = str, l = sz; l > 0; len++, l--) {
        ch = (unsigned char)*src++;

        if (ch == '\'')
            len++;
        else if (mysql) {
            if (ch == 0 || ch == 0x1a || ch == '\r' || ch == '\n' || ch == '\\'
                || ch == '\"')
                len++;
        }
    }

    retVal.resize(len);

    bstr = retVal.c_buffer();

    for (src = str, l = sz; l > 0; l--) {
        ch = (unsigned char)*src++;

        if (ch == '\'') {
            *bstr++ = '\'';
            *bstr++ = '\'';
        } else if (mysql) {
            switch (ch) {
            case 0:
                *bstr++ = '\\';
                *bstr++ = '0';
                break;
            case 0x1a:
                *bstr++ = '\\';
                *bstr++ = 0x1a;
                break;

            case '\r':
                *bstr++ = '\\';
                *bstr++ = 'r';
                break;

            case '\n':
                *bstr++ = '\\';
                *bstr++ = 'n';
                break;

            case '\\':
                *bstr++ = '\\';
                *bstr++ = '\\';
                break;

            case '\"':
                *bstr++ = '\\';
                *bstr++ = '\"';
                break;

            default:
                *bstr++ = ch;
                break;
            }
        } else
            *bstr++ = ch;
    }

    return retVal;
}

inline exlib::string _escape(exlib::string v, bool mysql)
{
    return _escape(v.c_str(), v.length(), mysql);
}

void _appendValue(exlib::string& str, v8::Local<v8::Value>& v, bool mysql, bool mssql)
{
    obj_ptr<Buffer_base> bin = Buffer_base::getInstance(v);

    if (bin) {
        exlib::string s;

        if (mssql) {
            str.append("0x", 2);
            bin->hex(s);
            str.append(s);
        } else {
            str.append("x\'", 2);
            bin->hex(s);
            str.append(s);
            str += '\'';
        }
    } else if (v->IsArray()) {
        v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
        int32_t len = a->Length();
        int32_t i;

        str += '(';

        for (i = 0; i < len; i++) {
            JSValue v1 = a->Get(i);

            if (i > 0)
                str += ',';
            _appendValue(str, v1, mysql, mssql);
        }

        str += ')';
    } else if (v->IsNumber() || v->IsNumberObject()
        || v->IsBigInt() || v->IsBigIntObject()) {
        Isolate* isolate = Isolate::current();
        str.append(isolate->toString(v));
    } else if (v->IsUndefined() || v->IsNull()) {
        str.append("NULL", 4);
    } else {
        exlib::string s;
        str += '\'';

        if (v->IsDate()) {
            date_t d = v;
            d.sqlString(s);
        } else {
            Isolate* isolate = Isolate::current();
            s = _escape(isolate->toString(v), mysql);
        }
        str.append(s);

        str += '\'';
    }
}

result_t db_format(const char* sql, OptArgs args, bool mysql, bool mssql,
    exlib::string& retVal)
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

                _appendValue(str, v, mysql, mssql);
            } else
                str.append(1, '?');

            cnt++;
        }

        sql = p1;
    }

    retVal = str;
    return 0;
}

result_t db_base::format(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_format(sql.c_str(), args, false, false, retVal);
}

result_t db_base::formatMySQL(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_format(sql.c_str(), args, true, false, retVal);
}

result_t db_base::formatMSSQL(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_format(sql.c_str(), args, false, true, retVal);
}

inline exlib::string _escape_field(const char* str, int32_t sz, bool mysql, bool mssql)
{
    exlib::string retVal;

    retVal.resize(sz * 2 + 2);
    char* ptr = retVal.c_buffer();
    char quote_left = mssql ? '[' : '`';
    char quote_right = mssql ? ']' : '`';

    *ptr++ = quote_left;
    while (sz--) {
        char ch = *str++;
        if (mssql && ch == quote_right) {
            *ptr++ = '\\';
            *ptr++ = quote_right;
        } else if (!mssql && ch == '`') {
            *ptr++ = '`';
            *ptr++ = '`';
        } else if (ch == '.') {
            *ptr++ = quote_right;
            *ptr++ = '.';
            *ptr++ = quote_left;
        } else
            *ptr++ = ch;
    }
    *ptr++ = quote_right;

    retVal.resize(ptr - retVal.c_buffer());
    return retVal;
}

inline exlib::string _escape_field(exlib::string s, bool mysql, bool mssql)
{
    return _escape_field(s.c_str(), (int32_t)s.length(), mysql, mssql);
}

inline exlib::string _escape_field(v8::Local<v8::Value> v, bool mysql, bool mssql)
{
    Isolate* isolate = Isolate::current();
    return _escape_field(isolate->toString(v), mysql, mssql);
}

result_t _format_where(v8::Local<v8::Value> o, bool mysql, bool mssql, exlib::string& retVal, bool& retAnd);
result_t _format_where(v8::Local<v8::Array> o, bool bAnd, bool mysql, bool mssql, exlib::string& retVal, bool& retAnd)
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
        hr = _format_where(v, mysql, mssql, s, rAnd);
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

result_t _format_where(v8::Local<v8::Value> val, bool mysql, bool mssql, exlib::string& retVal, bool& retAnd)
{
    Isolate* isolate = Isolate::current();

    if (val->IsString() || val->IsStringObject()) {
        retVal = isolate->toString(val);
        return 0;
    }

    if (val->IsArray())
        return _format_where(v8::Local<v8::Array>::Cast(val), true, mysql, mssql, retVal, retAnd);

    if (!IsJSObject(val))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(val);
    exlib::string str;
    JSArray ks = o->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;
    bool bAnd = true;

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(i);
        JSValue v = o->Get(k);
        JSValue v1;
        bool bBetween = false;
        bool bIn = false;
        bool bField = false;
        exlib::string op("=");
        exlib::string key;
        const char* or_str = mssql ? "[$or]" : "`$or`";
        const char* and_str = mssql ? "[$and]" : "`$and`";

        if (v->IsFunction())
            return CHECK_ERROR(CALL_E_INVALIDARG);

        key = _escape_field(k, mysql, mssql);
        while (len == 1 && (!qstrcmp(key.c_str(), or_str, 5) || !qstrcmp(key.c_str(), and_str, 6))) {
            bAnd = key[2] == 'a';

            if (v->IsArray())
                return _format_where(v8::Local<v8::Array>::Cast(v), bAnd, mysql, mssql, retVal, retAnd);
            else if (IsJSObject(v)) {
                o = v8::Local<v8::Object>::Cast(v);
                ks = o->GetPropertyNames();
                len = ks->Length();
                if (len == 0)
                    break;

                k = ks->Get(0);
                v = o->Get(k);

                key = _escape_field(k, mysql, mssql);
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

            str.append(key + op + _escape_field(v, mysql, mssql));
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
                result_t hr = db_format("find", v8::Local<v8::Object>::Cast(v), mysql, mssql, sub_query);
                if (hr < 0)
                    return hr;

                str.append(key + op + '(' + sub_query + ')');
            } else {
                str.append(key + op);
                _appendValue(str, v, mysql, mssql);
                if (bBetween) {
                    str.append(" AND ");
                    _appendValue(str, v1, mysql, mssql);
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

result_t _format_name_list(v8::Local<v8::Value> v, exlib::string& retVal, bool mysql, bool mssql)
{
    exlib::string table;

    if (v->IsArray()) {
        v8::Local<v8::Array> tables = v8::Local<v8::Array>::Cast(v);

        int32_t len = tables->Length();
        int32_t i;

        if (len > 0) {
            for (i = 0; i < len; i++) {
                table.append(_escape_field(tables->Get(i), mysql, mssql));
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

            table.append(_escape_field(k, mysql, mssql) + " AS " + _escape_field(o->Get(k), mysql, mssql));
            if (i + 1 < len)
                table.append(", ", 2);
        }
    } else
        return CALL_E_TYPEMISMATCH;

    retVal = table;
    return 0;
}

result_t _format_name(v8::Local<v8::Object> opts, exlib::string name, exlib::string& retVal, bool mysql, bool mssql)
{
    Isolate* isolate = Isolate::current();
    result_t hr;
    v8::Local<v8::Value> v;

    hr = GetConfigValue(isolate->m_isolate, opts, name.c_str(), v);
    if (hr < 0)
        return hr;

    if (v->IsString() || v->IsStringObject()) {
        retVal = _escape_field(v, mysql, mssql);
        return 0;
    } else
        return _format_name_list(v, retVal, mysql, mssql);
}

result_t _format_order(Isolate* isolate, v8::Local<v8::Array> orders, exlib::string& retVal, bool mysql, bool mssql)
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
                key = _escape_field(*s + 1, s.length() - 1, mysql, mssql);
                desc = true;
            } else
                key = _escape_field(*s, s.length(), mysql, mssql);

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

result_t _format_find(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str("SELECT ");
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Value> v;
    v8::Local<v8::Array> a;
    exlib::string table;

    hr = _format_name(opts, "table", table, mysql, mssql);
    if (hr < 0)
        return hr;

    hr = GetConfigValue(isolate->m_isolate, opts, "fields", v);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        str.append("*");
    else if (v->IsString() || v->IsStringObject()) {
        str.append(isolate->toString(v));
    } else {
        exlib::string str1;

        hr = _format_name_list(v, str1, mysql, mssql);
        if (hr < 0)
            return hr;

        str.append(str1);
    }

    str.append(" FROM " + table);

    hr = GetConfigValue(isolate->m_isolate, opts, "where", v);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        exlib::string _where;
        bool retAnd;

        hr = _format_where(v, mysql, mssql, _where, retAnd);
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

        hr = _format_order(isolate, orders, order, mysql, mssql);
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

result_t _format_count(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Value> v;
    exlib::string table;

    hr = _format_name(opts, "table", table, mysql, mssql);
    if (hr < 0)
        return hr;

    str.append("SELECT COUNT(*) FROM " + table);

    hr = GetConfigValue(isolate->m_isolate, opts, "where", v);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        exlib::string _where;
        bool retAnd;

        hr = _format_where(v, mysql, mssql, _where, retAnd);
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

result_t _format_update(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    Isolate* isolate = Isolate::current();
    exlib::string table;

    hr = _format_name(opts, "table", table, mysql, mssql);
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

        _values.append(_escape_field(k, mysql, mssql) + "=");
        _appendValue(_values, v, mysql, mssql);

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

        hr = _format_where(v, mysql, mssql, _where, retAnd);
        if (hr < 0)
            return hr;

        if (!_where.empty())
            str.append(" WHERE " + _where);
    }

    retVal = str;
    return 0;
}

result_t _format_insert(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    Isolate* isolate = Isolate::current();
    exlib::string table;

    hr = _format_name(opts, "table", table, mysql, mssql);
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

        _fields.append(_escape_field(k, mysql, mssql));
        _appendValue(_values, v, mysql, mssql);

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

result_t _format_remove(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    Isolate* isolate = Isolate::current();
    exlib::string table;

    hr = _format_name(opts, "table", table, mysql, mssql);
    if (hr < 0)
        return hr;

    str.append("DELETE FROM " + table);

    v8::Local<v8::Value> v;
    hr = GetConfigValue(isolate->m_isolate, opts, "where", v);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        exlib::string _where;
        bool retAnd;

        hr = _format_where(v, mysql, mssql, _where, retAnd);
        if (hr < 0)
            return hr;

        if (!_where.empty())
            str.append(" WHERE " + _where);
    }

    retVal = str;
    return 0;
}

result_t _format_createTable(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    Isolate* isolate = Isolate::current();
    exlib::string table;

    hr = _format_name(opts, "table", table, mysql, mssql);
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

        _fields.append(_escape_field(k, mysql, mssql));
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
                _fields.append(mysql ? "LONGTEXT" : "TEXT");
        } else if (type == "number") {
            int32_t size = 8;

            if (!prop.IsEmpty()) {
                hr = GetConfigValue(isolate->m_isolate, prop, "size", size, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;
            }

            if (size == 4)
                _fields.append("FLOAT");
            else if (size == 8)
                _fields.append(mssql ? "REAL" : "DOUBLE");
            else
                return CHECK_ERROR(Runtime::setError("db: Number size must be 4|8."));
        } else if (type == "integer") {
            int32_t size = 4;

            if (!prop.IsEmpty()) {
                hr = GetConfigValue(isolate->m_isolate, prop, "size", size, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;
            }

            if (size == 1)
                _fields.append("TINYINT");
            else if (size == 2)
                _fields.append("SMALLINT");
            else if (size == 4)
                _fields.append("INT");
            else if (size == 8)
                _fields.append("BIGINT");
            else
                return CHECK_ERROR(Runtime::setError("db: Integer size must be 1|2|4|8."));
        } else if (type == "date") {
            bool _time = false;

            if (!prop.IsEmpty()) {
                hr = GetConfigValue(isolate->m_isolate, prop, "time", _time, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;
            }

            _fields.append(_time ? "DATETIME" : "DATE");
        } else if (type == "binary") {
            bool big = 0;

            if (!prop.IsEmpty()) {
                hr = GetConfigValue(isolate->m_isolate, prop, "big", big, true);
                if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
                    return hr;
            }

            if (mssql)
                _fields.append(big ? "IMAGE" : "VARBINARY(MAX)");
            else
                _fields.append(big ? "LONGBLOB" : "BLOB");
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
                _appendValue(_fields, v, mysql, mssql);
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

result_t _format_dropTable(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    exlib::string table;

    hr = _format_name(opts, "table", table, mysql, mssql);
    if (hr < 0)
        return hr;

    str.append("DROP TABLE " + table);

    retVal = str;
    return 0;
}

result_t _format_createIndex(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    Isolate* isolate = Isolate::current();
    exlib::string table, index;

    hr = _format_name(opts, "table", table, mysql, mssql);
    if (hr < 0)
        return hr;

    hr = _format_name(opts, "index", index, mysql, mssql);
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

    hr = _format_order(isolate, keys, _keys, mysql, mssql);
    if (hr < 0)
        return hr;

    if (_keys.empty())
        return CHECK_ERROR(Runtime::setError("db: No index keys specified."));
    str.append(_keys);
    str.append(1, ')');

    retVal = str;
    return 0;
}

result_t _format_dropIndex(v8::Local<v8::Object> opts, bool mysql, bool mssql, exlib::string& retVal)
{
    result_t hr;
    exlib::string str;
    exlib::string index;

    hr = _format_name(opts, "index", index, mysql, mssql);
    if (hr < 0)
        return hr;

    str.append("DROP INDEX " + index);

    retVal = str;
    return 0;
}

result_t db_format(exlib::string method, v8::Local<v8::Object> opts, bool mysql, bool mssql,
    exlib::string& retVal)
{
    if (method == "createTable")
        return _format_createTable(opts, mysql, mssql, retVal);
    else if (method == "dropTable")
        return _format_dropTable(opts, mysql, mssql, retVal);
    else if (method == "createIndex")
        return _format_createIndex(opts, mysql, mssql, retVal);
    else if (method == "dropIndex")
        return _format_dropIndex(opts, mysql, mssql, retVal);
    else if (method == "find")
        return _format_find(opts, mysql, mssql, retVal);
    else if (method == "count")
        return _format_count(opts, mysql, mssql, retVal);
    else if (method == "update")
        return _format_update(opts, mysql, mssql, retVal);
    else if (method == "insert")
        return _format_insert(opts, mysql, mssql, retVal);
    else if (method == "remove")
        return _format_remove(opts, mysql, mssql, retVal);

    return CHECK_ERROR(Runtime::setError("db: Unknown method."));
}

result_t db_base::format(exlib::string method, v8::Local<v8::Object> opts,
    exlib::string& retVal)
{
    return db_format(method, opts, false, false, retVal);
}

result_t db_base::formatMySQL(exlib::string method, v8::Local<v8::Object> opts,
    exlib::string& retVal)
{
    return db_format(method, opts, true, false, retVal);
}

result_t db_base::formatMSSQL(exlib::string method, v8::Local<v8::Object> opts,
    exlib::string& retVal)
{
    return db_format(method, opts, false, true, retVal);
}

result_t db_base::escape(exlib::string str, bool mysql, exlib::string& retVal)
{
    retVal = _escape(str.c_str(), (int32_t)str.length(), mysql);
    return 0;
}
}
