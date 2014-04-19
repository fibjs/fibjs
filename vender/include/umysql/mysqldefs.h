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

#ifndef __MYSQLDEFS_H__
#define __MYSQLDEFS_H__

#ifdef _WIN32
//-----------------------------------------------------------------------------
// WIN32 stuff 
//-----------------------------------------------------------------------------
#include <Windows.h>

#else
//-----------------------------------------------------------------------------
// POSIX stuff 
//-----------------------------------------------------------------------------
#include <sys/types.h>

#ifndef u_int8_t
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
typedef unsigned long long u_int64_t;
#endif

typedef u_int8_t UINT8;
typedef u_int16_t UINT16;
typedef u_int32_t UINT32;
typedef u_int64_t UINT64;

typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum MYSQL_COMMANDS
{
  MC_SLEEP = 0,
  MC_QUIT = 1,
  MC_INIT_DB = 2,
  MC_QUERY = 3,
  MC_LIST = 4,
};

enum MYSQL_PACKETREAD
{
  MPR_NONE = 0,
  MPR_MORE = 1,
  MPR_ERROR = 2,
  MPR_TRUE = 4,
  MPR_START = 8,
  MPR_END = 16,
  MPR_EOF = 32,
};

enum MYSQL_FIELDTYPE
{
  MFTYPE_DECIMAL = 0x00,
  MFTYPE_TINY = 0x01,
  MFTYPE_SHORT = 0x02,
  MFTYPE_LONG = 0x03,
  MFTYPE_FLOAT = 0x04,
  MFTYPE_DOUBLE = 0x05,
  MFTYPE_NULL = 0x06,
  MFTYPE_TIMESTAMP = 0x07,
  MFTYPE_LONGLONG = 0x08,
  MFTYPE_INT24 = 0x09,
  MFTYPE_DATE = 0x0a,
  MFTYPE_TIME = 0x0b,
  MFTYPE_DATETIME = 0x0c,
  MFTYPE_YEAR = 0x0d,
  MFTYPE_NEWDATE = 0x0e,
  MFTYPE_VARCHAR = 0x0f,
  MFTYPE_BIT = 0x10,
  MFTYPE_NEWDECIMAL = 0xf6,
  MFTYPE_ENUM = 0xf7,
  MFTYPE_SET = 0xf8,
  MFTYPE_TINY_BLOB = 0xf9,
  MFTYPE_MEDIUM_BLOB = 0xfa,
  MFTYPE_LONG_BLOB = 0xfb,
  MFTYPE_BLOB = 0xfc,
  MFTYPE_VAR_STRING = 0xfd,
  MFTYPE_STRING = 0xfe,
  MFTYPE_GEOMETRY = 0xff,
};

enum MYSQL_CAPABILITIES
{
  MCP_LONG_PASSWORD			= (1 << 0), // new more secure passwords 
  MCP_FOUND_ROWS				= (1 << 1), //Found instead of affected rows
  MCP_LONG_FLAG					= (1 << 2), //Get all column flags */
  MCP_CONNECT_WITH_DB		= (1 << 3), // One can specify db on connect */
  MCP_NO_SCHEMA					=	(1 << 4), //  /* Don't allow database.table.column */
  MCP_COMPRESS					= (1 << 5), // Can use compression protocol */
  MCP_ODBC							= (1 << 6), // Odbc client */
  MCP_LOCAL_FILES				= (1 << 7), // Can use LOAD DATA LOCAL */
  MCP_IGNORE_SPACE			= (1 << 8), // Ignore spaces before '(' */
  MCP_PROTOCOL_41				= (1 << 9), // New 4.1 protocol */
  MCP_INTERACTIVE				= (1 << 10), // This is an interactive client */
  MCP_SSL								= (1 << 11), //Switch to SSL after handshake */
  MCP_IGNORE_SIGPIPE		= (1 << 12), // IGNORE sigpipes */
  MCP_TRANSACTIONS			= (1 << 13), // Client knows about transactions */
  MCP_RESERVED					= (1 << 14), // Old flag for 4.1 protocol  */
  MCP_SECURE_CONNECTION	= (1 << 15), // New 4.1 authentication */
  MCP_MULTI_STATEMENTS	=	(1 << 16), // Enable/disable multi-stmt support */
  MCP_MULTI_RESULTS			= (1 << 17), // Enable/disable multi-results */
};

enum MYSQL_CHARSETS
{
  MCS_UNDEFINED = 0,
  MCS_big5_chinese_ci = 1,
  MCS_big5_bin = 84,
  MCS_dec8_swedish_ci = 3,
  MCS_dec8_bin = 69,
  MCS_cp850_general_ci = 4,
  MCS_cp850_bin = 80,
  MCS_hp8_english_ci = 6,
  MCS_hp8_bin = 72,
  MCS_koi8r_general_ci = 7,
  MCS_koi8r_bin = 74,
  MCS_latin1_german1_ci = 5,
  MCS_latin1_swedish_ci = 8,
  MCS_latin1_danish_ci = 15,
  MCS_latin1_german2_ci = 31,
  MCS_latin1_bin = 47,
  MCS_latin1_general_ci = 48,
  MCS_latin1_general_cs = 49,
  MCS_latin1_spanish_ci = 94,
  MCS_latin2_czech_cs =  2,
  MCS_latin2_general_ci = 9,
  MCS_latin2_hungarian_ci = 21,
  MCS_latin2_croatian_ci = 27,
  MCS_latin2_bin = 77,
  MCS_swe7_swedish_ci = 10,
  MCS_swe7_bin = 82,
  MCS_ascii_general_ci = 11,
  MCS_ascii_bin = 65,
  MCS_ujis_japanese_ci = 12,
  MCS_ujis_bin = 91,
  MCS_sjis_japanese_ci = 13,
  MCS_sjis_bin = 88,
  MCS_hebrew_general_ci = 16,
  MCS_hebrew_bin = 71,
  MCS_tis620_thai_ci = 18,
  MCS_tis620_bin = 89,
  MCS_euckr_korean_ci = 19,
  MCS_euckr_bin = 85,
  MCS_koi8u_general_ci = 22,
  MCS_koi8u_bin = 75,
  MCS_gb2312_chinese_ci = 24,
  MCS_gb2312_bin = 86,
  MCS_greek_general_ci = 25,
  MCS_greek_bin = 70,
  MCS_cp1250_general_ci = 26,
  MCS_cp1250_czech_cs = 34,
  MCS_cp1250_croatian_ci = 44,
  MCS_cp1250_bin = 66,
  MCS_cp1250_polish_ci = 99,
  MCS_gbk_chinese_ci = 28,
  MCS_gbk_bin = 87,
  MCS_latin5_turkish_ci = 30,
  MCS_latin5_bin = 78,
  MCS_armscii8_general_ci = 32,
  MCS_armscii8_bin = 64,
  MCS_utf8_general_ci = 33,
  MCS_utf8_bin = 83,
  MCS_utf8_unicode_ci = 192,
  MCS_utf8_icelandic_ci = 193,
  MCS_utf8_latvian_ci = 194,
  MCS_utf8_romanian_ci = 195,
  MCS_utf8_slovenian_ci = 196,
  MCS_utf8_polish_ci = 197,
  MCS_utf8_estonian_ci = 198,
  MCS_utf8_spanish_ci = 199,
  MCS_utf8_swedish_ci = 200,
  MCS_utf8_turkish_ci = 201,
  MCS_utf8_czech_ci = 202,
  MCS_utf8_danish_ci = 203,
  MCS_utf8_lithuanian_ci = 204,
  MCS_utf8_slovak_ci = 205,
  MCS_utf8_spanish2_ci = 206,
  MCS_utf8_roman_ci = 207,
  MCS_utf8_persian_ci = 208,
  MCS_utf8_esperanto_ci = 209,
  MCS_utf8_hungarian_ci = 210,
  MCS_utf8_sinhala_ci = 211,
  MCS_ucs2_general_ci = 35,
  MCS_ucs2_bin = 90,
  MCS_ucs2_unicode_ci = 128,
  MCS_ucs2_icelandic_ci = 129,
  MCS_ucs2_latvian_ci = 130,
  MCS_ucs2_romanian_ci = 131,
  MCS_ucs2_slovenian_ci = 132,
  MCS_ucs2_polish_ci = 133,
  MCS_ucs2_estonian_ci = 134,
  MCS_ucs2_spanish_ci = 135,
  MCS_ucs2_swedish_ci = 136,
  MCS_ucs2_turkish_ci = 137,
  MCS_ucs2_czech_ci = 138,
  MCS_ucs2_danish_ci = 139,
  MCS_ucs2_lithuanian_ci = 140,
  MCS_ucs2_slovak_ci = 141,
  MCS_ucs2_spanish2_ci = 142,
  MCS_ucs2_roman_ci = 143,
  MCS_ucs2_persian_ci = 144,
  MCS_ucs2_esperanto_ci = 145,
  MCS_ucs2_hungarian_ci = 146,
  MCS_ucs2_sinhala_ci = 147,
  MCS_cp866_general_ci = 36,
  MCS_cp866_bin = 68,
  MCS_keybcs2_general_ci = 37,
  MCS_keybcs2_bin = 73,
  MCS_macce_general_ci = 38,
  MCS_macce_bin = 43,
  MCS_macroman_general_ci = 39,
  MCS_macroman_bin = 53,
  MCS_cp852_general_ci = 40,
  MCS_cp852_bin = 81,
  MCS_latin7_estonian_cs = 20,
  MCS_latin7_general_ci = 41,
  MCS_latin7_general_cs = 42,
  MCS_latin7_bin = 79,
  MCS_utf8mb4_general_ci = 45,
  MCS_utf8mb4_bin = 46,
  MCS_utf8mb4_unicode_ci = 224,
  MCS_utf8mb4_icelandic_ci = 225,
  MCS_utf8mb4_latvian_ci = 226,
  MCS_utf8mb4_romanian_ci = 227,
  MCS_utf8mb4_slovenian_ci = 228,
  MCS_utf8mb4_polish_ci = 229,
  MCS_utf8mb4_estonian_ci = 230,
  MCS_utf8mb4_spanish_ci = 231,
  MCS_utf8mb4_swedish_ci = 232,
  MCS_utf8mb4_turkish_ci = 233,
  MCS_utf8mb4_czech_ci = 234,
  MCS_utf8mb4_danish_ci = 235,
  MCS_utf8mb4_lithuanian_ci = 236,
  MCS_utf8mb4_slovak_ci = 237,
  MCS_utf8mb4_spanish2_ci = 238,
  MCS_utf8mb4_roman_ci = 239,
  MCS_utf8mb4_persian_ci = 240,
  MCS_utf8mb4_esperanto_ci = 241,
  MCS_utf8mb4_hungarian_ci = 242,
  MCS_utf8mb4_sinhala_ci = 243,
  MCS_cp1251_bulgarian_ci = 14,
  MCS_cp1251_ukrainian_ci = 23,
  MCS_cp1251_bin = 50,
  MCS_cp1251_general_ci = 51,
  MCS_cp1251_general_cs = 52,
  MCS_utf16_general_ci = 54,
  MCS_utf16_bin = 55,
  MCS_utf16_unicode_ci = 101,
  MCS_utf16_icelandic_ci = 102,
  MCS_utf16_latvian_ci = 103,
  MCS_utf16_romanian_ci = 104,
  MCS_utf16_slovenian_ci = 105,
  MCS_utf16_polish_ci = 106,
  MCS_utf16_estonian_ci = 107,
  MCS_utf16_spanish_ci = 108,
  MCS_utf16_swedish_ci = 109,
  MCS_utf16_turkish_ci = 110,
  MCS_utf16_czech_ci = 111,
  MCS_utf16_danish_ci = 112,
  MCS_utf16_lithuanian_ci = 113,
  MCS_utf16_slovak_ci = 114,
  MCS_utf16_spanish2_ci = 115,
  MCS_utf16_roman_ci = 116,
  MCS_utf16_persian_ci = 117,
  MCS_utf16_esperanto_ci = 118,
  MCS_utf16_hungarian_ci = 119,
  MCS_utf16_sinhala_ci = 120,
  MCS_cp1256_general_ci = 57,
  MCS_cp1256_bin = 67,
  MCS_cp1257_lithuanian_ci = 29,
  MCS_cp1257_bin = 58,
  MCS_cp1257_general_ci = 59,
  MCS_utf32_general_ci = 60,
  MCS_utf32_bin = 61,
  MCS_utf32_unicode_ci = 160,
  MCS_utf32_icelandic_ci = 161,
  MCS_utf32_latvian_ci = 162,
  MCS_utf32_romanian_ci = 163,
  MCS_utf32_slovenian_ci = 164,
  MCS_utf32_polish_ci = 165,
  MCS_utf32_estonian_ci = 166,
  MCS_utf32_spanish_ci = 167,
  MCS_utf32_swedish_ci = 168,
  MCS_utf32_turkish_ci = 169,
  MCS_utf32_czech_ci = 170,
  MCS_utf32_danish_ci = 171,
  MCS_utf32_lithuanian_ci = 172,
  MCS_utf32_slovak_ci = 173,
  MCS_utf32_spanish2_ci = 174,
  MCS_utf32_roman_ci = 175,
  MCS_utf32_persian_ci = 176,
  MCS_utf32_esperanto_ci = 177,
  MCS_utf32_hungarian_ci = 178,
  MCS_utf32_sinhala_ci = 179,
  MCS_binary = 63,
  MCS_geostd8_general_ci = 92,
  MCS_geostd8_bin = 93,
  MCS_cp932_japanese_ci = 95,
  MCS_cp932_bin = 96,
  MCS_eucjpms_japanese_ci = 97,
  MCS_eucjpms_bin = 98,
};

enum MYSQL_FIELDFLAG
{
  MFFLAG_NOT_NULL_FLAG = 0x0001,
  MFFLAG_PRI_KEY_FLAG = 0x0002,
  MFFLAG_UNIQUE_KEY_FLAG = 0x0004,
  MFFLAG_MULTIPLE_KEY_FLAG = 0x0008,
  MFFLAG_BLOB_FLAG = 0x0010,
  MFFLAG_UNSIGNED_FLAG = 0x0020,
  MFFLAG_ZEROFILL_FLAG = 0x0040,
  MFFLAG_BINARY_FLAG = 0x0080,
  MFFLAG_ENUM_FLAG = 0x0100,
  MFFLAG_AUTO_INCREMENT_FLAG = 0x0200,
  MFFLAG_TIMESTAMP_FLAG = 0x0400,
  MFFLAG_SET_FLAG = 0x0800,
};

#define MYSQL_PACKET_HEADER_SIZE 4
#define MYSQL_PROTOCOL_VERSION 0x0a
#define MYSQL_PACKET_SIZE (1024 * 1024 * 16)

#define MYSQL_TX_BUFFER_SIZE ((1024 * 1024 * 4) + MYSQL_PACKET_HEADER_SIZE)
#define MYSQL_RX_BUFFER_SIZE (MYSQL_PACKET_SIZE + MYSQL_PACKET_HEADER_SIZE)

#endif
