/*
 * db.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "ifs/db.h"
#include "ifs/Buffer.h"

namespace fibjs
{

DECLARE_MODULE(db);

result_t db_base::open(const char *connString, obj_ptr<object_base> &retVal, AsyncEvent *ac)
{
    if (!qstrcmp(connString, "mysql:", 6))
        return openMySQL(connString, (obj_ptr<MySQL_base> &)retVal, ac);

    if (!qstrcmp(connString, "sqlite:", 7))
        return openSQLite(connString, (obj_ptr<SQLite_base> &)retVal, ac);

    if (!qstrcmp(connString, "redis:", 6))
        return openRedis(connString, (obj_ptr<Redis_base> &)retVal, ac);

    if (!qstrcmp(connString, "mongodb:", 8))
        return openMongoDB(connString, (obj_ptr<MongoDB_base> &)retVal, ac);

    if (!qstrcmp(connString, "leveldb:", 8))
        return openLevelDB(connString, (obj_ptr<LevelDB_base> &)retVal, ac);

    return CHECK_ERROR(CALL_E_INVALIDARG);
}

inline void _escape(const char *str, int32_t sz, bool mysql, qstring &retVal)
{
    int32_t len, l;
    const char *src;
    char *bstr;
    char ch;

    for (len = 0, src = str, l = sz; l > 0; len++, l--)
    {
        ch = (unsigned char) * src++;

        if (ch == '\'')
            len++;
        else if (mysql)
        {
            if (ch == 0 || ch == 0x1a || ch == '\r' || ch == '\n' || ch == '\\'
                    || ch == '\"')
                len++;
        }
    }

    retVal.resize(len);

    bstr = &retVal[0];

    for (src = str, l = sz; l > 0; l--)
    {
        ch = (unsigned char) * src++;

        if (ch == '\'')
        {
            *bstr++ = '\'';
            *bstr++ = '\'';
        }
        else if (mysql)
        {
            switch (ch)
            {
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
        }
        else
            *bstr++ = ch;
    }
}

void _appendValue(qstring &str, v8::Local<v8::Value> &v, bool mysql)
{
    bool bNumber = v->IsNumber() || v->IsNumberObject();

    if (bNumber)
    {
        v8::String::Utf8Value s1(v);
        str.append(*s1, s1.length());
    }
    else
    {
        qstring s;
        str += '\'';

        if (v->IsDate())
        {
            date_t d = v;
            d.sqlString(s);
        }
        else
        {
            v8::String::Utf8Value s1(v);
            _escape(*s1, s1.length(), mysql, s);
        }
        str.append(s);

        str += '\'';
    }
}

result_t _format(const char *sql, const v8::FunctionCallbackInfo<v8::Value> &args, bool mysql,
                 qstring &retVal)
{
    qstring str;
    const char *p, *p1;
    int32_t cnt = 1;

    while (*sql)
    {
        p = p1 = sql;
        while (*p1 && *p1 != '?')
            p1++;

        str.append(p, p1 - p);

        if (*p1)
        {
            p1++;

            if (cnt < args.Length())
            {
                v8::Local<v8::Value> v = args[cnt];

                if (v->IsFunction())
                    return CHECK_ERROR(CALL_E_TYPEMISMATCH);

                obj_ptr<Buffer_base> bin = Buffer_base::getInstance(v);

                if (bin)
                {
                    qstring s;

                    str.append("x\'", 2);
                    bin->hex(s);
                    str.append(s);
                    str += '\'';
                }
                else if (v->IsArray())
                {
                    v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
                    int32_t len = a->Length();
                    int32_t i;

                    str += '(';

                    for (i = 0; i < len; i++)
                    {
                        v8::Local<v8::Value> v1 = a->Get(i);

                        if (i > 0)
                            str += ',';
                        _appendValue(str, v1, mysql);
                    }

                    str += ')';
                }
                else
                    _appendValue(str, v, mysql);
            }
            else
                str.append("\'\'", 2);

            cnt++;
        }

        sql = p1;
    }

    retVal = str;
    return 0;
}

result_t db_base::format(const char *sql, const v8::FunctionCallbackInfo<v8::Value> &args,
                         qstring &retVal)
{
    return _format(sql, args, false, retVal);
}

result_t db_base::formatMySQL(const char *sql, const v8::FunctionCallbackInfo<v8::Value> &args,
                              qstring &retVal)
{
    return _format(sql, args, true, retVal);
}

result_t db_base::escape(const char *str, bool mysql, qstring &retVal)
{
    _escape(str, (int32_t) qstrlen(str), mysql, retVal);
    return 0;
}

}
