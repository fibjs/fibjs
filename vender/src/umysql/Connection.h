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
#ifndef __UMCONNECTION_H__
#define __UMCONNECTION_H___

#include <string>
#include "umysql.h"
#include "PacketReader.h"
#include "PacketWriter.h"



class Connection
{
  enum State
  {
    NONE,
    CONNECT,
    HANDSHAKE_RECV,
    HANDSHAKE_SEND,
    HANDSHAKE_REPLY,
    QUERY_WAIT,
    QUERY_SEND,
    QUERY_RECV,
    DISCONNECT,
    FAILED,
  };

private:
  State m_state;

  std::string m_host;
  int m_port;
  std::string m_username;
  std::string m_password;
  std::string m_database;
  bool m_autoCommit;
  MYSQL_CHARSETS m_charset;
  void *m_sockInst;
  PacketReader m_reader;
  PacketWriter m_writer;
  UINT32 m_clientCaps;
  std::string m_query;

  std::string m_errorMessage;
  int m_errno;
  int m_timeout;
  UMErrorType m_errorType;

  UMConnectionCAPI m_capi;

  int m_dbgMethodProgress;

public:


public:
  Connection(UMConnectionCAPI *_capi);
  ~Connection();
  bool connect(const char *_host, int _port, const char *_username, const char *_password, const char *_database, int *_autoCommit, MYSQL_CHARSETS _charset);
  //void handleSocketEvent (SocketEvents _evt);
  void *query(const char *_query, size_t _cbQuery, void* opt);
  bool getLastError (const char **_ppOutMessage, int *_outErrno, int *_outErrorType);

  int getRxBufferSize();
  int getTxBufferSize();
  int setRxBufferSize(int num);
  int setTxBufferSize(int num);

  bool isConnected(void);
  bool close(void);
  bool setTimeout(int timeout);

protected:
  void changeState(State _newState, const char *message);
  bool connectSocket();
  bool readSocket();
  bool writeSocket();
  bool processHandshake();
  void scramble(const char *_scramble1, const char *_scramble2, UINT8 _outToken[20]);
  bool recvPacket();
  bool sendPacket();

  void handleErrorPacket();
  void handleEOFPacket();
  void *handleResultPacket(int fieldCount, void* opt);
  void *handleOKPacket(void* opt);
  void setError (const char *_message, int _errno, UMErrorType _type);

protected:
};

#endif
