/*
 * mssql.h
 *
 *  Created on: Oct 29, 2016
 *      Author: lion
 */

#pragma once

#include "ifs/DbConnection.h"
#include "utf8.h"

// #import "msado25.tlb" raw_interfaces_only, rename("EOF", "adoEOF")
#include "msado25.tlh"
#include "../db_tmpl.h"

namespace fibjs {

class mssql : public db_tmpl<DbConnection_base, false, true> {
public:
    virtual ~mssql();

public:
    // DbConnection_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    result_t connect(const char* server, const char* username, const char* password, const char* dbName);

private:
    inline result_t error(HRESULT hr)
    {
        ADODB::Errors* errs = NULL;

        ((ADODB::_Connection*)m_conn)->get_Errors(&errs);
        if (errs) {
            ADODB::Error* err = NULL;
            _variant_t i(0);

            errs->get_Item(i, &err);
            if (err) {
                BSTR msg = NULL;
                err->get_Description(&msg);
                err->Release();

                if (msg) {
                    exlib::string msga = utf16to8String(msg);
                    SysFreeString(msg);
                    return Runtime::setError(msga);
                }
            }

            errs->Release();
        }

        return hr;
    }
};

} /* namespace fibjs */
