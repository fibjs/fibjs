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
#include "PacketWriter.h"
#include <assert.h>

#define BYTEORDER_UINT16(_x) (_x)
#define BYTEORDER_UINT32(_x) (_x)

#include <ctype.h>

PacketWriter::PacketWriter(size_t _cbSize)
{
  m_buffStart = new char[_cbSize];
  m_buffEnd = m_buffStart + _cbSize;
  m_readCursor = m_buffStart;
  m_writeCursor = m_buffStart;
}

PacketWriter::~PacketWriter(void)
{
  delete m_buffStart;
}

// Push/increment write cursor
void PacketWriter::push(void *data, size_t cbData)
{
  assert (m_writeCursor + cbData  < m_buffEnd);

  memcpy (m_writeCursor, data, cbData);
  m_writeCursor += cbData;
}

// Pull/Increment read cursor
void PacketWriter::pull(size_t cbSize)
{
  assert (m_writeCursor - m_readCursor <= cbSize);
  m_readCursor += cbSize;
}

char *PacketWriter::getStart()
{
  return m_buffStart;
}

char *PacketWriter::getEnd()
{
  return m_buffEnd;
}

char *PacketWriter::getReadCursor()
{
  return m_readCursor;
}

char *PacketWriter::getWriteCursor()
{
  return m_writeCursor;
}

bool PacketWriter::isDone()
{
  return (m_readCursor == m_writeCursor);
}

void PacketWriter::reset()
{
  m_readCursor = m_buffStart;
  m_writeCursor = m_buffStart;

  // Reserve space for header
  writeLong(0);
}

void PacketWriter::writeLong (UINT32 value)
{
  *((UINT32*)m_writeCursor) = BYTEORDER_UINT32(value);
  m_writeCursor += 4;
}

void PacketWriter::writeByte (UINT8 value)
{
  *((UINT8*)m_writeCursor) = value;
  m_writeCursor ++;
}

void PacketWriter::writeNTString (const char *_str)
{
  while (*_str != '\0')
  {
    *(m_writeCursor++) = *(_str++);
  }
  *(m_writeCursor++) = '\0';
}

void PacketWriter::writeBytes (void *data, size_t cbData)
{
  memcpy (m_writeCursor, data, cbData);
  m_writeCursor += cbData;
}

void PacketWriter::finalize(int packetNumber)
{
  size_t packetLen = (m_writeCursor - m_readCursor - MYSQL_PACKET_HEADER_SIZE);

  *((UINT32 *)m_buffStart) = packetLen;
  *((UINT8 *)m_buffStart + 3) = packetNumber;

  //PrintBuffer (stdout, m_readCursor, (m_writeCursor - m_readCursor), 16);

}

size_t PacketWriter::getSize(void)
{
  return (m_buffEnd - m_buffStart);
}

size_t PacketWriter::setSize(size_t _cbSize)
{
  if((int)_cbSize < m_writeCursor - m_buffStart)
	return 0;

  size_t old_cbSize = m_buffEnd - m_buffStart;

  char* buffStart = new char[_cbSize];
  char* buffEnd = buffStart + _cbSize;

  memcpy(buffStart, m_buffStart, m_writeCursor - m_buffStart);
  m_readCursor = m_readCursor - m_buffStart + buffStart;
  m_writeCursor = m_writeCursor - m_buffStart + buffStart;

  delete m_buffStart;
  m_buffStart = buffStart;
  m_buffEnd = buffEnd;

  return old_cbSize;
}

