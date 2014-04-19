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
#include "Connection.h"
#include <assert.h>
#include <string>
#include "SHA1.h"
#include <stdio.h>
#include <time.h>
#include <alloca.h>

#ifdef _WIN32
#define snprintf _snprintf
#endif

//#define PRINTMARK() fprintf(stderr, "%08x:%s:%s MARK(%d)\n", GetTickCount(), __FILE__, __FUNCTION__, __LINE__)		
#define PRINTMARK() 		

Connection::Connection (UMConnectionCAPI *_capi) 
  :	m_reader(MYSQL_RX_BUFFER_SIZE)
  , m_writer(MYSQL_TX_BUFFER_SIZE)
{
  PRINTMARK();

  m_timeout = -1;
  m_state = NONE;
  m_sockInst = NULL;
  m_errno = -1;
  memcpy (&m_capi, _capi, sizeof (UMConnectionCAPI));
  m_dbgMethodProgress = 0;
  m_errorType = UME_OTHER;
}

Connection::~Connection()
{
  PRINTMARK();

  if (m_sockInst)
  {
    PRINTMARK();
    m_capi.closeSocket(m_sockInst);
    m_capi.deleteSocket(m_sockInst);
  }

}

void Connection::scramble(const char *_scramble1, const char *_scramble2, UINT8 _outToken[20])
{
  std::string seed;
  seed += _scramble1;
  seed += _scramble2;

  CSHA1 passdg;
  UINT8 stage1_hash[20];
  passdg.Update ( (UINT8 *) m_password.c_str(), m_password.size());
  passdg.Final();
  passdg.GetHash(stage1_hash);


  CSHA1 stage2dg;
  UINT8 stage2_hash[20];
  stage2dg.Update (stage1_hash, 20);
  stage2dg.Final();
  stage2dg.GetHash(stage2_hash);

  CSHA1 finaldg;
  UINT8 final_hash[20];
  finaldg.Update( (UINT8*) seed.c_str(), seed.size());
  finaldg.Update(stage2_hash, 20);
  finaldg.Final();
  finaldg.GetHash(final_hash);

  for (int index = 0; index < 20; index ++)
  {
    _outToken[index] = final_hash[index] ^ stage1_hash[index]; 
  }
}


bool Connection::readSocket()
{
  size_t bytesToRecv = m_reader.getEndPtr() - m_reader.getWritePtr();
  assert (bytesToRecv <= m_reader.getEndPtr() - m_reader.getWritePtr()); 

  if (bytesToRecv == 0)
  {
    // Socket buffer got full!
    setError("Socket receive buffer full", 0, UME_OTHER);
    return false;
  }
  else
    if (bytesToRecv > 65536)
    {
      bytesToRecv = 65536;
    }


    int recvResult = m_capi.recvSocket(m_sockInst, m_reader.getWritePtr(), bytesToRecv);

    if (recvResult == -1)
    {
      return false;
    }
    else
      if (recvResult == 0)
      {
        setError("Connection reset by peer when receiving", 0, UME_OTHER);
        return false;
      }

      m_reader.push (recvResult);

      return true;
}

bool Connection::writeSocket()
{
  size_t bytesToSend = m_writer.getWriteCursor() - m_writer.getReadCursor();

  assert (bytesToSend > 0);
  assert (bytesToSend < m_writer.getEnd() - m_writer.getStart());

  int sendResult = m_capi.sendSocket(m_sockInst, m_writer.getReadCursor(), bytesToSend);

  if (sendResult == -1)
  {
    return false;
  }
  else
    if (sendResult == 0)
    {
      setError("Connection reset by peer when receiving", 0, UME_OTHER);
      return false;
    }

    m_writer.pull(sendResult);
    return true;
}

bool Connection::close(void)
{
  PRINTMARK();

  if (m_sockInst)
  {
    PRINTMARK();

    if (m_writer.isDone())
    {
      m_writer.reset();
      m_writer.writeByte(MC_QUIT);
      m_writer.finalize(0);

      if (!sendPacket())
      {	
        m_capi.clearException();
      }
    }

    if (m_sockInst)
    {
      m_capi.closeSocket(m_sockInst);
      m_capi.clearException();
      m_capi.deleteSocket(m_sockInst);
      m_sockInst = NULL;
      return true;
    }
  }

  return true;
}

bool Connection::connectSocket()
{
  if (!m_capi.connectSocket(m_sockInst, m_host.c_str(), m_port))
  {
    return false;
  }

  PRINTMARK();
  return true;
}

bool Connection::processHandshake()
{
  // Parse data
  UINT8 protocolVersion = m_reader.readByte();

  if (protocolVersion == 0xff)
  {
    setError("Too many connections reported by server", 0, UME_OTHER);
    return false;
  }
  else
    if (protocolVersion != MYSQL_PROTOCOL_VERSION)
    {
      setError("Protocol version not supported(1)", 0, UME_OTHER);
      return false;
    }

    char *serverVersion = m_reader.readNTString();
    UINT32 threadId = m_reader.readLong();
    char *scrambleBuff = (char *) m_reader.readBytes(8);

    UINT8 filler1 = m_reader.readByte();
    UINT16 serverCaps = m_reader.readShort();

    if (!(serverCaps & MCP_PROTOCOL_41))
    {
      setError("Authentication < 4.1 not supported", 1, UME_OTHER);
      return false;
    }

    UINT8 serverLang = m_reader.readByte();
    UINT16 serverStatus = m_reader.readShort();
    UINT8 *filler2 = m_reader.readBytes(13);


    char *scrambleBuff2 = NULL;
    if (m_reader.getBytesLeft ())
    {
      scrambleBuff2 = (char *) m_reader.readNTString();
    }
    else
    {
      setError("Authentication < 4.1 not supported", 2, UME_OTHER);
      return false;
    }

    m_clientCaps = serverCaps;

    m_clientCaps  &= ~MCP_COMPRESS;
    m_clientCaps  &= ~MCP_NO_SCHEMA;
    m_clientCaps &= ~MCP_SSL;

    if (!(serverCaps & MCP_CONNECT_WITH_DB) && !m_database.empty())
    {
      setError("Protocol < 4.1 not supported", 3, UME_OTHER);
      return false;
    }

    if ((serverCaps & MCP_CONNECT_WITH_DB) && m_database.empty())
    {
      m_clientCaps &= ~MCP_CONNECT_WITH_DB;
    }

    m_reader.skip();

    m_writer.reset();
    m_writer.writeLong (m_clientCaps);
    m_writer.writeLong (MYSQL_PACKET_SIZE);

    if (m_charset != MCS_UNDEFINED)
    {
      m_writer.writeByte( (UINT8) (int) m_charset);
    }
    else
    {
      m_writer.writeByte(serverLang);
    }

    for (int filler = 0; filler < 23; filler ++)
      m_writer.writeByte(0x00);

    m_writer.writeNTString(m_username.c_str ());

    if (!m_password.empty())
    {
      m_writer.writeByte(20);

      UINT8 token[20];
      scramble(scrambleBuff, scrambleBuff2, token);
      m_writer.writeBytes(token, 20);
    }
    else
    {
      m_writer.writeByte(0x00);
    }

    if (!m_database.empty())
    {
      m_writer.writeNTString(m_database.c_str());
    }

    m_writer.finalize(1);

    return true;
}

bool Connection::recvPacket()
{
  while (true)
  {
    if (m_reader.havePacket())
    {
      break;
    }

    if (!readSocket())
    {
      return false;
    }

    if (m_reader.havePacket())
    {
      break;
    }
  }

  return true;
}

bool Connection::isConnected(void)
{
  return (m_sockInst != NULL);
}


bool Connection::sendPacket()
{
  while (true)
  {
    if (!writeSocket())
    {
      return false;
    }

    if (m_writer.isDone())
    {
      break;
    }
  }

  return true;
}

void Connection::setError (const char *_message, int _errno, UMErrorType _type)
{
  m_errorMessage = _message;
  m_errno = _errno;
  m_errorType = _type;

  PRINTMARK();

  if (_type != UME_MYSQL)
  {

    if (m_sockInst)
    {
      PRINTMARK();
      m_capi.closeSocket(m_sockInst);
      m_capi.deleteSocket(m_sockInst);
      m_sockInst = NULL;
    }
  }
}


bool Connection::getLastError (const char **_ppOutMessage, int *_outErrno, int *_outErrorType)
{
  if (m_errno == -1)
  {
    return false;
  }

  *_ppOutMessage = m_errorMessage.c_str();
  *_outErrorType = (int) m_errorType;
  *_outErrno = m_errno;

  m_errno = -1;

  return true;
}


bool Connection::connect(const char *_host, int _port, const char *_username, const char *_password, const char *_database, int *_autoCommit, MYSQL_CHARSETS _charset)
{
  m_dbgMethodProgress ++;

  if (m_dbgMethodProgress > 1)
  {
    /*
    NOTE: We don't call setError here because it will close the socket worsening the concurrent access error making it impossible to trace */
    m_errorMessage = "Concurrent access in connect method";
    m_errno = 0;
    m_errorType = UME_OTHER;
    m_dbgMethodProgress --;
    return false;
  }


  if (m_sockInst != NULL)
  {
    m_dbgMethodProgress --;
    setError ("Socket already connected", 0, UME_OTHER);
    return false;
  }

  m_host = _host ? _host : "localhost";
  m_port = _port ? _port : 3306;
  m_username = _username ? _username : "";
  m_password = _password ? _password : "";
  m_database = _database ? _database : "";
  m_autoCommit = _autoCommit ? (*_autoCommit) != 0 : false;
  m_charset = _charset;

  PRINTMARK();
  m_sockInst = m_capi.getSocket();

  if (m_sockInst == NULL)
  {
    m_dbgMethodProgress --;
    return false;
  }

  if (m_timeout != -1)
  {
    if (!setTimeout (m_timeout))
    {
      m_dbgMethodProgress --;
      return false;
    }
  }

  if (!connectSocket())
  {
    m_dbgMethodProgress --;
    return false;
  }

  PRINTMARK();
  if (!recvPacket())
  {
    m_dbgMethodProgress --;
    return false;
  }

  PRINTMARK();
  if (!processHandshake())
  {
    m_dbgMethodProgress --;
    return false;
  }

  PRINTMARK();
  if (!sendPacket())
  {
    m_dbgMethodProgress --;
    return false;
  }

  PRINTMARK();
  m_writer.reset();

  if (!recvPacket())
  {
    m_dbgMethodProgress --;
    return false;
  }

  PRINTMARK();
  UINT8 result = m_reader.readByte();
  if (result == 0xff)
  {
    handleErrorPacket();
    m_dbgMethodProgress --;
    return false;
  }

  m_reader.skip();

  PRINTMARK();
  if (_autoCommit)
  {
    PRINTMARK();
    char strTemp[256 + 1];
    PRINTMARK();
    size_t len = snprintf (strTemp, 256, "SET AUTOCOMMIT = %d", *_autoCommit);
    PRINTMARK();
    m_writer.reset();
    m_writer.writeByte(MC_QUERY);
    m_writer.writeBytes ( (void *) strTemp, len);
    m_writer.finalize(0);

    PRINTMARK();
    if (!sendPacket())
    {
      m_dbgMethodProgress --;
      return false;
    }

    PRINTMARK();
    if (!recvPacket())
    {
      m_dbgMethodProgress --;
      return false;
    }
    m_reader.skip();
  }

  PRINTMARK();
  m_state = QUERY_WAIT;
  m_dbgMethodProgress --;

  return true;
}

void *Connection::handleOKPacket(void* opt)
{
  UINT64 affectedRows = m_reader.readLengthCodedInteger();
  UINT64 insertId = m_reader.readLengthCodedInteger();
  UINT16 serverStatus = m_reader.readShort();
  UINT16 warningCount = m_reader.readShort();
  size_t len = m_reader.getBytesLeft();
  UINT8 *message = m_reader.readBytes(m_reader.getBytesLeft());

  return m_capi.resultOK(affectedRows, insertId, serverStatus, (char *) message, len, opt);
}

void Connection::handleErrorPacket()
{
  UINT16 errnum = m_reader.readShort();
  UINT8  stateMarker = m_reader.readByte();
  UINT8 *sqlstate = m_reader.readBytes(5);

  size_t len = m_reader.getBytesLeft();

  UINT8 *message = m_reader.readBytes(len);

  std::string errorMessage((char *) message, len);
  setError (errorMessage.c_str (), (int) errnum, UME_MYSQL);
}

void *Connection::handleResultPacket(int _fieldCount, void* opt)
{
  m_reader.rewind(1);
  UINT64 fieldCount = m_reader.readLengthCodedInteger();
  m_reader.skip();

  int iField = 0;

  void *resultSet = m_capi.createResult(_fieldCount, opt);

  // Read Field packets

  UMTypeInfo *typeInfo = (UMTypeInfo *) alloca(fieldCount * sizeof(UMTypeInfo));
  int iTypeInfo = 0;

  while (true)
  {

    if (!this->recvPacket())
    {
      m_capi.destroyResult(resultSet, opt);
      return NULL;
    }

    size_t cb_catalog;
    size_t cb_db;
    size_t cb_table;
    size_t cb_org_table;
    size_t cb_name;
    size_t cb_org_name;
    size_t cb_default;

    UINT8 result = m_reader.readByte();

    if (result == 0xfe)
    {
      m_reader.skip();
      break;
    }

    m_reader.rewind(1);

    UINT8 *catalog = m_reader.readLengthCodedBinary(&cb_catalog);
    UINT8 *db = m_reader.readLengthCodedBinary(&cb_db);
    UINT8 *table = m_reader.readLengthCodedBinary(&cb_table);
    UINT8 *org_table = m_reader.readLengthCodedBinary(&cb_org_table);
    UINT8 *name = m_reader.readLengthCodedBinary(&cb_name);
    UINT8 *org_name = m_reader.readLengthCodedBinary(&cb_org_name);

    UINT8 filler = m_reader.readByte();
    UINT16 charset = m_reader.readShort();
    UINT32 length = m_reader.readLong();
    UINT8 type = m_reader.readByte();
    UINT16 flags = m_reader.readShort();
    UINT8 decimals = m_reader.readByte();
    UINT16 filler2 = m_reader.readShort();

    //UINT8 *def = m_reader.readLengthCodedBinary(&cb_default);

    typeInfo[iTypeInfo].type = type;
    typeInfo[iTypeInfo].flags = flags;
    typeInfo[iTypeInfo].charset = charset;
    iTypeInfo ++;

    m_capi.resultSetField(resultSet, iField, &typeInfo[iTypeInfo - 1], name, cb_name, opt);
    iField ++;
    m_reader.skip();

  }

  // Read row data

  int cRows = 0;

  while (true)
  {

    if (!this->recvPacket())
    {
      m_capi.destroyResult(resultSet, opt);
      return NULL;
    }

    UINT8 result = m_reader.readByte();

    if (result == 0xfe)
    {
      m_reader.skip();
      break;
    }

    m_reader.rewind(1);

    size_t cb_column;


    m_capi.resultRowBegin(resultSet, opt);

    for (int index = 0; index < _fieldCount; index ++)
    {
      UINT8 *columnValue = m_reader.readLengthCodedBinary(&cb_column);
      if (!m_capi.resultRowValue (resultSet, index, &typeInfo[index], columnValue, cb_column, opt))
      {
        m_capi.destroyResult(resultSet, opt);
        return NULL;
      }
    }

    if(!m_capi.resultRowEnd(resultSet, opt))
    {
        m_capi.destroyResult(resultSet, opt);
        return NULL;
    }
    cRows ++;
    m_reader.skip();


  }

  return resultSet;
}


void *Connection::query(const char *_query, size_t _cbQuery, void* opt)
{
  m_dbgMethodProgress ++;

  if (m_dbgMethodProgress > 1)
  {
    /*
    NOTE: We don't call setError here because it will close the socket worsening the concurrent access error making it impossible to trace */
    m_errorMessage = "Concurrent access in query method";
    m_errno = 0;
    m_errorType = UME_OTHER;
    m_dbgMethodProgress --;
    return NULL;
  }

  if (m_sockInst == NULL)
  {
    PRINTMARK();
    setError ("Not connected", 0, UME_OTHER);
    m_dbgMethodProgress --;
    return NULL;
  }

  size_t len = _cbQuery;

  if (len > m_writer.getSize () - (MYSQL_PACKET_HEADER_SIZE + 1))
  {
    PRINTMARK();
    setError ("Query too big", 0, UME_OTHER);
    m_dbgMethodProgress --;
    return NULL;
  }

  m_writer.reset();
  m_writer.writeByte(MC_QUERY);
  m_writer.writeBytes ( (void *) _query, len);
  m_writer.finalize(0);

  if (!sendPacket())
  {
    PRINTMARK();
    m_dbgMethodProgress --;
    return NULL;
  }

  if (!recvPacket())
  {
    PRINTMARK();
    m_dbgMethodProgress --;
    return NULL;
  }

  UINT8 result = m_reader.readByte();

  switch (result)
  {
  case 0x00:
    PRINTMARK();
    m_dbgMethodProgress --;
    return handleOKPacket(opt);

  case 0xff:
    PRINTMARK();
    handleErrorPacket();
    m_dbgMethodProgress --;
    return NULL;

  case 0xfe:
    PRINTMARK();
    setError ("Unexpected EOF when decoding result", 0, UME_OTHER);
    m_dbgMethodProgress --;
    return NULL;


  default:
    PRINTMARK();
    m_dbgMethodProgress --;
    return handleResultPacket((int)result, opt);
  }

  PRINTMARK();
  m_dbgMethodProgress --;
  return NULL;
}


int Connection::getRxBufferSize()
{
  return (int) m_reader.getSize();
}

int Connection::getTxBufferSize()
{
  return (int) m_writer.getSize();
}

int Connection::setRxBufferSize(int num)
{
  return (int) m_reader.setSize(num);
}

int Connection::setTxBufferSize(int num)
{
  return (int) m_writer.setSize(num);
}

bool Connection::setTimeout(int timeout)
{
  m_timeout = timeout;

  if (m_sockInst)
  {
    if (!m_capi.setTimeout(m_sockInst, timeout))
    {
      return false;
    }
  }

  return true;
}
