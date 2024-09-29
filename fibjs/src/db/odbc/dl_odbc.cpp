/*
 * dl_odbc.cpp
 *
 *  Created on: May 22, 2021
 *      Author: lion
 */

#include <exlib/include/dl_func.h>
#include "dl_odbc.h"

static void* odbc_handle;
#define odbc_func(func) dl_def_func(odbc_handle, ODBC_LIB, func)

SQLRETURN SQL_API SQLAllocEnv(SQLHENV* EnvironmentHandle)
{
    odbc_func(SQLAllocEnv);
    return s_SQLAllocEnv(EnvironmentHandle);
}

SQLRETURN SQL_API SQLAllocConnect(SQLHENV EnvironmentHandle, SQLHDBC* ConnectionHandle)
{
    odbc_func(SQLAllocConnect);
    return s_SQLAllocConnect(EnvironmentHandle, ConnectionHandle);
}

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC ConnectionHandle, SQLHSTMT* StatementHandle)
{
    odbc_func(SQLAllocStmt);
    return s_SQLAllocStmt(ConnectionHandle, StatementHandle);
}

SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT StatementHandle, SQLUSMALLINT Option)
{
    odbc_func(SQLFreeStmt);
    return s_SQLFreeStmt(StatementHandle, Option);
}

SQLRETURN SQL_API SQLFreeConnect(SQLHDBC ConnectionHandle)
{
    odbc_func(SQLFreeConnect);
    return s_SQLFreeConnect(ConnectionHandle);
}

SQLRETURN SQL_API SQLFreeEnv(SQLHENV EnvironmentHandle)
{
    odbc_func(SQLFreeEnv);
    return s_SQLFreeEnv(EnvironmentHandle);
}

SQLRETURN SQL_API SQLDriverConnectA(SQLHDBC hdbc, SQLHWND hwnd,
    SQLCHAR* szConnStrIn, SQLSMALLINT cbConnStrIn,
    SQLCHAR* szConnStrOut, SQLSMALLINT cbConnStrOutMax, SQLSMALLINT* pcbConnStrOut,
    SQLUSMALLINT fDriverCompletion)
{
    odbc_func(SQLDriverConnectA);
    return s_SQLDriverConnectA(hdbc, hwnd, szConnStrIn, cbConnStrIn,
        szConnStrOut, cbConnStrOutMax, pcbConnStrOut, fDriverCompletion);
}

SQLRETURN SQL_API SQLDisconnect(SQLHDBC hdbc)
{
    odbc_func(SQLDisconnect);
    return s_SQLDisconnect(hdbc);
}

SQLRETURN SQL_API SQLExecute(SQLHSTMT StatementHandle)
{
    odbc_func(SQLExecute);
    return s_SQLExecute(StatementHandle);
}

SQLRETURN SQL_API SQLExecDirectA(SQLHSTMT StatementHandle, SQLCHAR* StatementText, SQLINTEGER TextLength)
{
    odbc_func(SQLExecDirectA);
    return s_SQLExecDirectA(StatementHandle, StatementText, TextLength);
}

SQLRETURN SQL_API SQLExecDirectW(SQLHSTMT StatementHandle, SQLWCHAR* StatementText, SQLINTEGER TextLength)
{
    odbc_func(SQLExecDirectW);
    return s_SQLExecDirectW(StatementHandle, StatementText, TextLength);
}

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT StatementHandle, SQLSMALLINT* ColumnCount)
{
    odbc_func(SQLNumResultCols);
    return s_SQLNumResultCols(StatementHandle, ColumnCount);
}


SQLRETURN SQL_API SQLColAttributesW(SQLHSTMT hstmt, SQLUSMALLINT icol,
    SQLUSMALLINT fDescType, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax,
    SQLSMALLINT* pcbDesc, SQLLEN* pfDesc)
{
    odbc_func(SQLColAttributesW);
    return s_SQLColAttributesW(hstmt, icol, fDescType, rgbDesc, cbDescMax, pcbDesc, pfDesc);
}

SQLRETURN SQL_API SQLRowCount(SQLHSTMT StatementHandle, SQLLEN* RowCount)
{
    odbc_func(SQLRowCount);
    return s_SQLRowCount(StatementHandle, RowCount);
}

SQLRETURN SQL_API SQLFetch(SQLHSTMT StatementHandle)
{
    odbc_func(SQLFetch);
    return s_SQLFetch(StatementHandle);
}

SQLRETURN SQL_API SQLGetData(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType,
    SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN* StrLen_or_Ind)
{
    odbc_func(SQLGetData);
    return s_SQLGetData(StatementHandle, ColumnNumber, TargetType, TargetValue,
        BufferLength, StrLen_or_Ind);
}

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT hstmt)
{
    odbc_func(SQLMoreResults);
    return s_SQLMoreResults(hstmt);
}

SQLRETURN SQL_API SQLGetDiagFieldA(SQLSMALLINT HandleType, SQLHANDLE Handle,
    SQLSMALLINT RecNumber, SQLSMALLINT DiagIdentifier,
    SQLPOINTER DiagInfo, SQLSMALLINT BufferLength, SQLSMALLINT* StringLength)
{
    odbc_func(SQLGetDiagFieldA);
    return s_SQLGetDiagFieldA(HandleType, Handle, RecNumber, DiagIdentifier,
        DiagInfo, BufferLength, StringLength);
}

SQLRETURN SQL_API SQLGetDiagRecA(SQLSMALLINT HandleType, SQLHANDLE Handle,
    SQLSMALLINT RecNumber, SQLCHAR* Sqlstate,
    SQLINTEGER* NativeError, SQLCHAR* MessageText,
    SQLSMALLINT BufferLength, SQLSMALLINT* TextLength)
{
    odbc_func(SQLGetDiagRecA);
    return s_SQLGetDiagRecA(HandleType, Handle, RecNumber, Sqlstate,
        NativeError, MessageText, BufferLength, TextLength);
}
