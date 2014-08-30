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
#include "PacketReader.h"
#include "mysqldefs.h"
#include <assert.h>

#define BYTEORDER_UINT16(_x) (_x)
#define BYTEORDER_UINT32(_x) (_x)

PacketReader::PacketReader (size_t _cbSize)
{
  m_buffStart = new char[_cbSize];
  m_writeCursor = m_buffStart;
  m_buffEnd = m_buffStart + _cbSize;
  m_readCursor = m_buffStart;
  m_packetEnd = NULL;
}

PacketReader::~PacketReader (void)
{
  delete m_buffStart;
}

void PacketReader::skip()
{
  assert (m_packetEnd != NULL);
  assert (m_readCursor <= m_packetEnd);

  m_readCursor = m_packetEnd;

  if (m_readCursor == m_writeCursor)
  {
    //fprintf (stderr, "%s: Buffer is aligned, moving back\n", __FUNCTION__);

    m_readCursor = m_buffStart;
    m_writeCursor = m_buffStart;
    m_packetEnd = NULL;
  }
}

void PacketReader::push(size_t _cbData)
{
  //fprintf (stderr, "%s: Pushing %u bytes\n", __FUNCTION__, _cbData);
  m_writeCursor += _cbData;
}

char *PacketReader::getWritePtr()
{
  return m_writeCursor;
}

char *PacketReader::getStartPtr()
{
  return m_buffStart;
}

char *PacketReader::getEndPtr()
{
  return m_buffEnd;
}

extern void PrintBuffer(FILE *file, void *_offset, size_t len, int perRow);


bool PacketReader::havePacket()
{
  m_packetEnd = NULL;

  size_t len = (m_writeCursor - m_readCursor);

  if (len < MYSQL_PACKET_HEADER_SIZE)
  {
    return false;
  }

  UINT32 packetSize = readINT24();
  UINT32 packetNumber = readByte();

  if (len < MYSQL_PACKET_HEADER_SIZE + packetSize)
  {
    m_readCursor -= 4;
    //fprintf (stderr, "%s: Not enough bytes in buffer, have %u, want %u\n", __FUNCTION__, len, MYSQL_PACKET_HEADER_SIZE + packetSize);
    return false;
  }

  this->m_packetEnd = m_readCursor + packetSize;

  //fprintf (stderr, "%s: Have a packet %02x\n", __FUNCTION__, (*m_readCursor));
  //PrintBuffer (stderr, m_readCursor, (m_packetEnd - m_readCursor), 16);

  return true;
}

UINT8 PacketReader::readByte()
{
  assert (m_readCursor + 1 <= m_packetEnd || m_packetEnd == NULL);
  assert (m_packetEnd <= m_writeCursor);

  return (*m_readCursor++);
}

UINT16 PacketReader::readShort()
{
  assert (m_readCursor + 2 <= m_packetEnd);
  assert (m_packetEnd <= m_writeCursor);

  UINT16 ret = BYTEORDER_UINT16(*((UINT16*)m_readCursor));
  m_readCursor += 2;
  return ret;
}

UINT32 PacketReader::readINT24()
{
  assert (m_readCursor < m_packetEnd || m_packetEnd == NULL);
  assert (m_packetEnd < m_writeCursor);

  UINT32 ret = readByte() | (readByte() << 8) | (readByte() << 16);

  return ret;
}

UINT32 PacketReader::readLong()
{
  assert (m_readCursor + 4 <= m_packetEnd);
  assert (m_packetEnd <= m_writeCursor);

  UINT32 ret = BYTEORDER_UINT32(*((UINT32*)m_readCursor));
  m_readCursor += 4;
  return ret;
}

char *PacketReader::readNTString()
{
  assert (m_readCursor < m_packetEnd);
  assert (m_packetEnd <= m_writeCursor);

  char *ret = m_readCursor;

  while (m_readCursor < m_packetEnd)
  {
    if (*(m_readCursor++) == '\0')
    {
      return ret;
    }
  }

  assert (false);
  return NULL;
}


UINT8 *PacketReader::readBytes(size_t cbsize)
{
  assert (m_readCursor + cbsize <= m_packetEnd);
  assert (m_packetEnd <= m_writeCursor);

  UINT8 *ret = (UINT8 *) m_readCursor;
  m_readCursor += cbsize;

  return ret;
}

size_t PacketReader::getBytesLeft()
{
  return (m_packetEnd - m_readCursor);
}

void PacketReader::rewind(size_t num)
{
  m_readCursor -= num;
}


UINT8 *PacketReader::readLengthCodedBinary(size_t *_outLen)
{
  assert (m_readCursor < m_packetEnd);
  assert (m_packetEnd <= m_writeCursor);

  switch (*((UINT8 *) m_readCursor))
  {
  default:
    *_outLen = (size_t) *((UINT8 *) m_readCursor);
    m_readCursor ++;
    break;

  case 251:
    m_readCursor ++;
    *_outLen = 0;
    return NULL;

  case 252:
    m_readCursor ++;
    *_outLen = (size_t) *((UINT16 *) m_readCursor);
    m_readCursor += 2; 
    break;

  case 253:
    m_readCursor ++;
    *_outLen = (size_t) *((UINT32 *) m_readCursor);
    *_outLen &= 0xffffff;
    m_readCursor += 3; 
    break;

  case 254:
    m_readCursor ++;
    *_outLen = (size_t) *((UINT64 *) m_readCursor);
    m_readCursor += 8; 
    break;
  }

  UINT8 *ret = (UINT8*) m_readCursor;
  m_readCursor += (*_outLen);

  assert (m_readCursor <= m_packetEnd);

  return ret;
}

size_t PacketReader::getSize()
{
  return m_buffEnd - m_buffStart;
}

size_t PacketReader::setSize(size_t _cbSize)
{
  if((int)_cbSize < m_writeCursor - m_buffStart)
	return 0;

  size_t old_cbSize = m_buffEnd - m_buffStart;

  char* buffStart = new char[_cbSize];
  char* buffEnd = buffStart + _cbSize;

  memcpy(buffStart, m_buffStart, m_writeCursor - m_buffStart);
  m_readCursor = m_readCursor - m_buffStart + buffStart;
  m_writeCursor = m_writeCursor - m_buffStart + buffStart;
  m_packetEnd = m_packetEnd - m_buffStart + buffStart;

  delete m_buffStart;
  m_buffStart = buffStart;
  m_buffEnd = buffEnd;

  return old_cbSize;
}


UINT64 PacketReader::readLengthCodedInteger()
{
  assert (m_readCursor < m_packetEnd);
  assert (m_packetEnd <= m_writeCursor);

  UINT64 ret;

  switch (*((UINT8 *) m_readCursor))
  {
  default:
    ret = (UINT64) *((UINT8 *) m_readCursor);
    m_readCursor ++;
    assert (m_readCursor <= m_packetEnd);
    return ret;

  case 251:
    ret = 0;
    m_readCursor ++;
    assert (m_readCursor <= m_packetEnd);
    return ret;

  case 252:
    m_readCursor ++;
    ret = (UINT64) *((UINT16 *) m_readCursor);
    m_readCursor += 2;
    assert (m_readCursor <= m_packetEnd);
    return ret;

  case 253:
    m_readCursor ++;
    ret = (UINT64) *((UINT32 *) m_readCursor);
    ret &= 0xffffff;
    m_readCursor += 3;
    assert (m_readCursor <= m_packetEnd);
    return ret;

  case 254:
    m_readCursor ++;
    ret = (UINT64) *((UINT64 *) m_readCursor);
    m_readCursor += 8;
    assert (m_readCursor <= m_packetEnd);
    return ret;
  }

  return ret;
}



