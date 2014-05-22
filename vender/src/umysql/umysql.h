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
#ifndef __UMYSQL_H__
#define __UMYSQL_H__

#include "mysqldefs.h"

#define EXPORTFUNCTION extern "C" __declspec(dllexport)

enum UMConnection_Ops
{
  UMC_READ,
  UMC_WRITE,
};

enum UMErrorType
{
  UME_OTHER,
  UME_MYSQL,
};

typedef struct 
{
  UINT8 type;
  UINT16 flags;
  UINT16 charset;
} UMTypeInfo;

typedef struct __UMConnectionCAPI
{
  void *(*getSocket)();
  void (*deleteSocket)(void *sock);
  void (*closeSocket)(void *sock);
  int (*connectSocket)(void *sock, const char *host, int port);
  int (*setTimeout)(void *sock, int timeoutSec);
  void (*clearException)(void);
  int (*recvSocket)(void *sock, char *buffer, int cbBuffer);
  int (*sendSocket)(void *sock, const char *buffer, int cbBuffer);

  void *(*createResult)(int columns, void* opt);
  void (*resultSetField)(void *result, int ifield, UMTypeInfo *ti, void *name, size_t cbName, void* opt);
  void (*resultRowBegin)(void *result, void* opt);
  int (*resultRowValue)(void *result, int icolumn, UMTypeInfo *ti, void *value, size_t cbValue, void* opt);
  int (*resultRowEnd)(void *result, void* opt);
  void (*destroyResult)(void *result, void* opt);
  void *(*resultOK)(UINT64 affected, UINT64 insertId, int serverStatus, const char *message, size_t len, void* opt);


} UMConnectionCAPI;


typedef void * UMConnection;

//#ifdef _WIN32
//#define EXPORT_ATTR __declspec(dllexport)
//#define EXPORT_ATTR __attribute__ ((dllexport))
//#define EXPORT_ATTR extern "C" __declspec(dllexport)
//#else
#define EXPORT_ATTR
//#endif

UMConnection UMConnection_Create(UMConnectionCAPI *_capi);
void UMConnection_Destroy(UMConnection _conn);
void *UMConnection_Query(UMConnection conn, const char *_query, size_t _cbQuery, void* opt = NULL);
int  UMConnection_Connect (UMConnection conn, const char *_host, int _port, const char *_username, const char *_password, const char *_database, int *_autoCommit, int _charset);
int UMConnection_GetLastError (UMConnection conn, const char **_ppOutMessage, int *_outErrno, int *_type);
int UMConnection_GetTxBufferSize (UMConnection conn);
int UMConnection_GetRxBufferSize (UMConnection conn);
int UMConnection_SetTxBufferSize (UMConnection conn, int num);
int UMConnection_SetRxBufferSize (UMConnection conn, int num);
int UMConnection_IsConnected (UMConnection conn);
int UMConnection_Close (UMConnection conn);
int UMConnection_SetTimeout(UMConnection conn, int timeout);

#endif
