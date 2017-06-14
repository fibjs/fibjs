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

inline void _escape(const char* str, int32_t sz, bool mysql, exlib::string& retVal)
{
    int32_t len, l;
    const char* src;
    char* bstr;
    char ch;

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

    bstr = &retVal[0];

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
}

void _appendValue(exlib::string& str, v8::Local<v8::Value>& v, bool mysql)
{
    bool bNumber = v->IsNumber() || v->IsNumberObject();

    if (bNumber) {
        v8::String::Utf8Value s1(v);
        str.append(*s1, s1.length());
    } else {
        exlib::string s;
        str += '\'';

        if (v->IsDate()) {
            date_t d = v;
            d.sqlString(s);
        } else {
            v8::String::Utf8Value s1(v);
            _escape(*s1, s1.length(), mysql, s);
        }
        str.append(s);

        str += '\'';
    }
}

inline result_t SQL_hex(Buffer_base* bin, exlib::string& retVal)
{
    exlib::string data;
    bin->toString(data);

    static char HexChar[] = "0123456789abcdef";
    int32_t i, pos;
    int32_t len = (int32_t)data.length();

    if (len > 0) {
        retVal.resize(len * 2);

        pos = 0;
        for (i = 0; i < len; i++) {
            retVal[pos * 2] = HexChar[(unsigned char)data[i] >> 4];
            retVal[pos * 2 + 1] = HexChar[(unsigned char)data[i] & 0xf];
            pos++;
        }
    }

    return 0;
}

result_t _format(const char* sql, const v8::FunctionCallbackInfo<v8::Value>& args,
    bool mysql, bool mssql, exlib::string& retVal)
{
    exlib::string str;
    const char *p, *p1;
    int32_t cnt = 1;

    while (*sql) {
        p = p1 = sql;
        while (*p1 && *p1 != '?')
            p1++;

        str.append(p, p1 - p);

        if (*p1) {
            p1++;

            if (cnt < args.Length()) {
                v8::Local<v8::Value> v = args[cnt];

                if (v->IsFunction())
                    return CHECK_ERROR(CALL_E_INVALIDARG);

                obj_ptr<Buffer_base> bin = Buffer_base::getInstance(v);

                if (bin) {
                    exlib::string s;

                    if (mssql) {
                        str.append("0x", 2);
                        SQL_hex(bin, s);
                        str.append(s);
                    } else {
                        str.append("x\'", 2);
                        SQL_hex(bin, s);
                        str.append(s);
                        str += '\'';
                    }
                } else if (v->IsArray()) {
                    v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
                    int32_t len = a->Length();
                    int32_t i;

                    str += '(';

                    for (i = 0; i < len; i++) {
                        v8::Local<v8::Value> v1 = a->Get(i);

                        if (i > 0)
                            str += ',';
                        _appendValue(str, v1, mysql);
                    }

                    str += ')';
                } else
                    _appendValue(str, v, mysql);
            } else
                str.append("\'\'", 2);

            cnt++;
        }

        sql = p1;
    }

    retVal = str;
    return 0;
}

result_t db_base::format(exlib::string sql, const v8::FunctionCallbackInfo<v8::Value>& args,
    exlib::string& retVal)
{
    return _format(sql.c_str(), args, false, false, retVal);
}

result_t db_base::formatMySQL(exlib::string sql, const v8::FunctionCallbackInfo<v8::Value>& args,
    exlib::string& retVal)
{
    return _format(sql.c_str(), args, true, false, retVal);
}

result_t db_base::formatMSSQL(exlib::string sql, const v8::FunctionCallbackInfo<v8::Value>& args,
    exlib::string& retVal)
{
    return _format(sql.c_str(), args, false, true, retVal);
}

result_t db_base::escape(exlib::string str, bool mysql, exlib::string& retVal)
{
    _escape(str.c_str(), (int32_t)str.length(), mysql, retVal);
    return 0;
}
}
