/*
 * pg_escape.h
 *
 * PostgreSQL literal rules shared by the psql and KingbaseES engines: text
 * doubles the quote, binary values use the BYTEA hex form and booleans are
 * the true/false keywords.
 */

#pragma once

#include "ifs/DbConnection.h"
#include "utf8.h"

#include "../odbc/Odbc.h"

namespace fibjs {

class pg_escape {
public:
    static exlib::string escape_binary(Buffer* bin)
    {
        exlib::string retVal;
        exlib::string s;

        bin->hex(s);

        retVal.append("\'\\x", 3);
        retVal.append(s);
        retVal.append("\'::BYTEA", 8);

        return retVal;
    }

    static exlib::string escape_bool(bool v)
    {
        return v ? "true" : "false";
    }

    static exlib::string escape_date_value(double v)
    {
        exlib::string retVal;
        exlib::string s;

        retVal.append("TIMESTAMP \'", 11);

        date_t d(v);
        d.sqlString(s);
        retVal.append(s);

        retVal.append("\'", 1);

        return retVal;
    }

    static exlib::string escape_date(v8::Local<v8::Value>& v)
    {
        date_t d = v;
        return escape_date_value(d.date());
    }
};

} /* namespace fibjs */
