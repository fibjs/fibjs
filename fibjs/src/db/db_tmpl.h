/*
 * db_tmpl.h
 *
 *  Created on: May 19, 2021
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "ifs/db.h"
#include "Buffer.h"

namespace fibjs {

template <class impl>
class db_format {
public:
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
    static void appendValue(exlib::string& str, v8::Local<v8::Value>& v)
    {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();

        if (IsJSBuffer(v) || v->IsArrayBuffer() || v->IsArrayBufferView() || v->IsTypedArray()) {
            obj_ptr<Buffer_base> bin;
            GetArgumentValue(isolate, v, bin);
            str.append(impl::escape_binary(bin.As<Buffer>()));
        } else if (v->IsArray()) {
            v8::Local<v8::Array> a = v.As<v8::Array>();
            int32_t len = a->Length();
            int32_t i;

            str += '(';

            for (i = 0; i < len; i++) {
                JSValue v1 = a->Get(context, i);

                if (i > 0)
                    str += ',';
                appendValue(str, v1);
            }

            str += ')';
        } else if (v->IsNumber() || v->IsNumberObject()
            || v->IsBigInt() || v->IsBigIntObject())
            str.append(isolate->toString(v));
        else if (v->IsBoolean() || v->IsBooleanObject())
            str.append(v->BooleanValue(isolate->m_isolate) ? "true" : "false");
        else if (v->IsUndefined() || v->IsNull())
            str.append("NULL", 4);
        else if (v->IsDate())
            str.append(impl::escape_date(v));
        else
            str.append(impl::escape_string(isolate->toString(v)));
    }
};

template <typename T>
inline result_t db_trans(T* pThis, exlib::string point, v8::Local<v8::Function> func, bool& retVal)
{
    METHOD_NAME("DbConnection.trans");
    v8::Local<v8::Value> v = pThis->wrap();
    result_t hr = 0;
    retVal = false;

    {
        METHOD_NAME("DbConnection.begin");
        hr = pThis->ac_begin(point);
    }
    if (hr < 0)
        return hr;

    v8::Local<v8::Value> result = func->Call(func->GetCreationContextChecked(), pThis->wrap(), 1, &v).FromMaybe(v8::Local<v8::Value>());

    if (result.IsEmpty()) {
        METHOD_NAME("DbConnection.rollback");
        pThis->ac_rollback(point);
        return CALL_E_JAVASCRIPT;
    }

    if (result->IsFalse()) {
        METHOD_NAME("DbConnection.rollback");
        return pThis->ac_rollback(point);
    } else {
        retVal = true;
        METHOD_NAME("DbConnection.commit");
        return pThis->ac_commit(point);
    }
}

template <class base, class impl>
class db_tmpl : public base {
public:
    db_tmpl()
        : m_conn(NULL)
    {
    }

public:
    result_t format(exlib::string sql, OptArgs args, exlib::string& retVal)
    {
        return db_format<impl>::format(sql.c_str(), args, retVal);
    }

    result_t use(exlib::string dbName, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;
        exlib::string s("USE ", 4);
        s.append(dbName);
        return execute(s, retVal, ac);
    }

    result_t begin(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("BEGIN", retVal, ac);

        exlib::string str("SAVEPOINT " + point);
        return execute(str, retVal, ac);
    }

    result_t commit(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("COMMIT", retVal, ac);

        exlib::string str("RELEASE SAVEPOINT " + point);
        return execute(str, retVal, ac);
    }

    result_t rollback(exlib::string point, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_LONGSYNC);

        obj_ptr<NArray> retVal;

        if (point.empty())
            return execute("ROLLBACK", retVal, ac);

        exlib::string str("ROLLBACK TO " + point);
        return execute(str, retVal, ac);
    }

    result_t trans(v8::Local<v8::Function> func, bool& retVal)
    {
        return trans("", func, retVal);
    }

    result_t trans(exlib::string point, v8::Local<v8::Function> func, bool& retVal)
    {
        return db_trans(this, point, func, retVal);
    }

    result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    result_t execute(exlib::string sql, OptArgs args, obj_ptr<NArray>& retVal,
        AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync()) {
            exlib::string str;
            result_t hr = format(sql, args, str);
            if (hr < 0)
                return hr;

            ac->m_ctx.resize(1);
            ac->m_ctx[0] = str;

            return CHECK_ERROR(CALL_E_LONGSYNC);
        }

        exlib::string str = ac->m_ctx[0].string();
        return execute(str, retVal, ac);
    }

    typedef result_t (*formater)(v8::Local<v8::Object> opts, exlib::string& retVal);

    result_t execute(formater fmt, v8::Local<v8::Object> opts,
        obj_ptr<NArray>& retVal, AsyncEvent* ac)
    {
        if (!m_conn)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync()) {
            exlib::string str;
            result_t hr = fmt(opts, str);
            if (hr < 0)
                return hr;

            ac->m_ctx.resize(1);
            ac->m_ctx[0] = str;

            return CHECK_ERROR(CALL_E_LONGSYNC);
        }

        exlib::string str = ac->m_ctx[0].string();
        return execute(str, retVal, ac);
    }

public:
    static exlib::string escape_string(exlib::string v)
    {
        const char* str = v.c_str();
        int32_t sz = (int32_t)v.length();
        int32_t len, l;
        const char* src;
        char* bstr;
        char ch;
        exlib::string retVal;

        for (len = 0, src = str, l = sz; l > 0; len++, l--) {
            ch = (unsigned char)*src++;

            if (ch == '\'')
                len++;
        }

        retVal.resize(len + 2);

        bstr = retVal.data();
        *bstr++ = '\'';

        for (src = str, l = sz; l > 0; l--) {
            ch = (unsigned char)*src++;

            if (ch == '\'')
                *bstr++ = '\'';
            *bstr++ = ch;
        }

        *bstr++ = '\'';

        return retVal;
    }

    static exlib::string escape_binary(Buffer* bin)
    {
        exlib::string retVal;
        exlib::string s;

        bin->hex(s);

        if (s.empty()) {
            retVal.append("''", 2);
        } else {
            retVal.append("0x", 2);
            retVal.append(s);
        }

        return retVal;
    }

    static exlib::string escape_date(v8::Local<v8::Value>& v)
    {
        exlib::string retVal;
        exlib::string s;

        retVal.append(1, '\'');

        date_t d = v;
        d.sqlString(s);
        retVal.append(s);

        retVal.append(1, '\'');

        return retVal;
    }

public:
    void* m_conn;
};
}