/*
Copyright (c) 2011, Jonas Tarnstrom and ESN Social Software AB
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
must display the following acknowledgement:
This product includes software developed by ESN Social Software AB (www.esn.me).
4. Neither the name of the ESN Social Software AB nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ESN SOCIAL SOFTWARE AB ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ESN SOCIAL SOFTWARE AB BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Portions of code from gevent-MySQL
Copyright (C) 2010, Markus Thurlin
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of Hyves (Startphone Ltd.) nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "umysql.h"

#include "Connection.h"

EXPORT_ATTR UMConnection UMConnection_Create(UMConnectionCAPI *_capi)
{
  return (UMConnection) new Connection(_capi);
}

EXPORT_ATTR void UMConnection_Destroy(UMConnection conn)
{
  delete (Connection *)conn;
}

EXPORT_ATTR void * UMConnection_Query(UMConnection conn, const char *_query, size_t _cbQuery, void* opt)
{
  return ((Connection *)conn)->query(_query, _cbQuery, opt);
}

EXPORT_ATTR int UMConnection_Connect (UMConnection conn, const char *_host, int _port, const char *_username, const char *_password, const char *_database, int *_autoCommit, int _charset)
{
  return ((Connection *)conn)->connect(_host, _port, _username, _password, _database, _autoCommit, (MYSQL_CHARSETS) _charset) ? 1 : 0;
}

EXPORT_ATTR int UMConnection_SetTimeout(UMConnection conn, int timeout)
{
  return ((Connection *)conn)->setTimeout(timeout) ? 1 : 0;
}

EXPORT_ATTR int UMConnection_GetLastError (UMConnection conn, const char **_ppOutMessage, int *_outErrno, int *_type)
{
  return ((Connection *)conn)->getLastError(_ppOutMessage, _outErrno, _type) ? 1 : 0;
}

EXPORT_ATTR int UMConnection_GetTxBufferSize (UMConnection conn)
{
  return ((Connection *)conn)->getTxBufferSize();
}

EXPORT_ATTR int UMConnection_GetRxBufferSize (UMConnection conn)
{
  return ((Connection *)conn)->getRxBufferSize();
}

EXPORT_ATTR int UMConnection_SetTxBufferSize (UMConnection conn, int num)
{
  return ((Connection *)conn)->setTxBufferSize(num);
}

EXPORT_ATTR int UMConnection_SetRxBufferSize (UMConnection conn, int num)
{
  return ((Connection *)conn)->setRxBufferSize(num);
}

EXPORT_ATTR int UMConnection_IsConnected (UMConnection conn)
{
  return ((Connection *)conn)->isConnected() ? 1 : 0;
}

EXPORT_ATTR int UMConnection_Close (UMConnection conn)
{
  return ((Connection *)conn)->close() ? 1 : 0;
}

