/*
 * dl_odbc.h
 *
 *  Created on: May 22, 2021
 *      Author: lion
 */

#ifdef _WIN32
#include <windows.h>
#include "sql.h"
#include "sqlext.h"
#define ODBC_LIB "odbc32.dll"
#else
#include "unix_sql.h"
#include "unix_sqlext.h"
#ifdef __APPLE__
#define ODBC_LIB "libodbc.dylib"
#else
#define ODBC_LIB "libodbc.so.2"
#endif
#endif
