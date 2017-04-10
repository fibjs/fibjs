/*
 * mssql.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/db.h"

#ifdef _WIN32

#include "mssql.h"
#include "Url.h"
#include "DBResult.h"
#include "Buffer.h"
#include "date.h"

namespace fibjs {

result_t db_base::openMSSQL(exlib::string connString, obj_ptr<MSSQL_base>& retVal,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "mssql:", 6))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr < 0)
        return hr;

    obj_ptr<mssql> conn = new mssql();

    hr = conn->connect(u->m_hostname.c_str(), u->m_username.c_str(), u->m_password.c_str(),
        u->m_pathname.length() > 0 ? u->m_pathname.c_str() + 1 : "");
    if (hr < 0)
        return hr;

    retVal = conn;
    return 0;
}

static result_t close_conn(ADODB::_Connection* conn)
{
    conn->Close();
    conn->Release();
    return 0;
}

mssql::~mssql()
{
    if (m_conn) {
        asyncCall(close_conn, m_conn);
        m_conn = NULL;
    }
}

result_t mssql::connect(const char* server, const char* username,
    const char* password, const char* dbName)
{
    if (m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    HRESULT hr;

    hr = CoCreateInstance(__uuidof(ADODB::Connection), NULL,
        CLSCTX_INPROC_SERVER, __uuidof(ADODB::_Connection),
        (void**)&m_conn);
    if (FAILED(hr))
        return hr;

    exlib::string connStr;

    connStr.append("Provider=\'sqloledb\';Data Source=\'");
    connStr.append(server);
    connStr.append("\';Initial Catalog=\'");
    connStr.append(*dbName ? dbName : "master");
    connStr.append("\';");

    bstr_t bstrConn(UTF8_W(connStr));
    bstr_t bstrUser(UTF8_W(username));
    bstr_t bstrPass(UTF8_W(password));

    hr = m_conn->Open(bstrConn, bstrUser, bstrPass,
        ADODB::adConnectUnspecified);
    if (FAILED(hr))
        return error(hr);

    m_conn->put_CommandTimeout(0);

    return 0;
}

result_t mssql::get_type(exlib::string& retVal)
{
    retVal = "mssql";
    return 0;
}

result_t mssql::close(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    m_conn->Close();
    m_conn->Release();
    m_conn = NULL;

    return 0;
}

result_t mssql::use(exlib::string dbName, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    bstr_t bstrName(utf8to16String(dbName).c_str());
    HRESULT hr;

    hr = m_conn->put_DefaultDatabase(bstrName);
    if (FAILED(hr))
        return error(hr);

    return 0;
}

result_t mssql::begin(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    HRESULT hr;
    long level = 0;

    hr = m_conn->BeginTrans(&level);
    if (FAILED(hr))
        return error(hr);

    return 0;
}

result_t mssql::commit(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    HRESULT hr;

    hr = m_conn->CommitTrans();
    if (FAILED(hr))
        return error(hr);

    return 0;
}

result_t mssql::rollback(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    HRESULT hr;

    hr = m_conn->RollbackTrans();
    if (FAILED(hr))
        return error(hr);

    return 0;
}

result_t mssql::execute(const char* sql, int32_t sLen,
    obj_ptr<DBResult_base>& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    HRESULT hr;
    ADODB::_Recordset* rs = NULL;
    bstr_t bstrCom(utf8to16String(sql, sLen).c_str());
    _variant_t affected((long)0);

    hr = m_conn->Execute(bstrCom, &affected, ADODB::adCmdText, &rs);
    if (FAILED(hr))
        return error(hr);

    obj_ptr<DBResult> res;

    VARIANT_BOOL bEof = VARIANT_TRUE;
    rs->get_adoEOF(&bEof);
    if (bEof == VARIANT_FALSE) {
        ADODB::Fields* fields = NULL;

        rs->get_Fields(&fields);
        if (FAILED(hr)) {
            rs->Release();
            return error(hr);
        }

        long columns;
        fields->get_Count(&columns);

        if (columns > 0) {
            int32_t i;

            res = new DBResult(columns);

            for (i = 0; i < columns; i++) {
                ADODB::Field* field = NULL;
                _variant_t vi((long)i);
                BSTR bstrName = NULL;

                fields->get_Item(vi, &field);

                field->get_Name(&bstrName);
                res->setField(i, utf16to8String(bstrName));

                field->Release();
            }

            while (bEof == VARIANT_FALSE) {
                int32_t i;

                res->beginRow();
                for (i = 0; i < columns; i++) {
                    Variant v;
                    ADODB::Field* field = NULL;
                    _variant_t vi((long)i);

                    fields->get_Item(vi, &field);
                    if (field) {
                        _variant_t value;

                        field->get_Value(&value);

                        switch (value.vt) {
                        case VT_NULL:
                            v.setNull();
                            break;
                        case VT_BOOL:
                            v = (value.boolVal != VARIANT_FALSE);
                            break;
                        case VT_I2:
                            v = value.iVal;
                            break;
                        case VT_UI2:
                            v = value.uiVal;
                            break;
                        case VT_I4:
                        case VT_INT:
                            v = value.lVal;
                            break;
                        case VT_UI4:
                        case VT_UINT:
                            v = (int64_t)value.ulVal;
                            break;
                        case VT_R4:
                            v = value.dblVal;
                            break;
                        case VT_BSTR:
                            v = utf16to8String(value.bstrVal);
                            break;
                        case VT_DATE: {
                            date_t d;
                            SYSTEMTIME st;

                            VariantTimeToSystemTime(value.date, &st);
                            d.create(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
                            d.toUTC();

                            v = d;
                            break;
                        }
                        case VT_DECIMAL: {
                            double myDouble = value;
                            v = myDouble;
                            break;
                        }
                        case VT_UI1 | VT_ARRAY:
                            v = new Buffer(value.parray->pvData, value.parray->rgsabound[0].cElements);
                            break;
                        }

                        field->Release();
                    }

                    res->rowValue(i, v);
                }
                res->endRow();

                rs->MoveNext();
                rs->get_adoEOF(&bEof);
            }
        } else
            res = new DBResult(0, affected);

        fields->Release();

    } else
        res = new DBResult(0, affected);

    rs->Release();

    res->freeze();
    retVal = res;

    return 0;
}

result_t mssql::execute(exlib::string sql, obj_ptr<DBResult_base>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    return execute(sql.c_str(), (int32_t)sql.length(), retVal);
}

result_t mssql::execute(exlib::string sql, const v8::FunctionCallbackInfo<v8::Value>& args,
    obj_ptr<DBResult_base>& retVal)
{
    exlib::string str;
    result_t hr = format(sql, args, str);
    if (hr < 0)
        return hr;

    return ac_execute(str, retVal);
}

result_t mssql::format(exlib::string sql, const v8::FunctionCallbackInfo<v8::Value>& args,
    exlib::string& retVal)
{
    return db_base::formatMSSQL(sql, args, retVal);
}

} /* namespace fibjs */

#else

namespace fibjs {

result_t db_base::openMSSQL(exlib::string connString, obj_ptr<MSSQL_base>& retVal,
    AsyncEvent* ac)
{
    return CALL_E_INVALIDARG;
}

} /* namespace fibjs */

#endif