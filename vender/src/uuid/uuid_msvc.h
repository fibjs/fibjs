/*
**  OSSP uuid - Universally Unique Identifier
**  Copyright (c) 2004-2008 Ralf S. Engelschall <rse@engelschall.com>
**  Copyright (c) 2004-2008 The OSSP Project <http://www.ossp.org/>
**
**  This file is part of OSSP uuid, a library for the generation
**  of UUIDs which can found at http://www.ossp.org/pkg/lib/uuid/
**
**  Permission to use, copy, modify, and distribute this software for
**  any purpose with or without fee is hereby granted, provided that
**  the above copyright notice and this permission notice appear in all
**  copies.
**
**  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
**  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
**  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
**  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
**  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
**  SUCH DAMAGE.
**
**  uuid.h: library API definition
*/

#ifndef __UUID_H__
#define __UUID_H__

/* workaround conflicts with system headers */
#define uuid_t       __vendor_uuid_t
#define uuid_create  __vendor_uuid_create
#define uuid_compare __vendor_uuid_compare
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#undef  uuid_t
#undef  uuid_create
#undef  uuid_compare

/* required system headers */
#include <string.h>

/* minimum C++ support */
#ifdef __cplusplus
#define DECLARATION_BEGIN extern "C" {
#define DECLARATION_END   }
#else
#define DECLARATION_BEGIN
#define DECLARATION_END
#endif

DECLARATION_BEGIN

/* OSSP uuid version (compile-time information) */
#define UUID_VERSION  0x106202

/* encoding octet stream lengths */
#define UUID_LEN_BIN  (128 /*bit*/ / 8 /*bytes*/)
#define UUID_LEN_STR  (128 /*bit*/ / 4 /*nibbles*/ + 4 /*hyphens*/)
#define UUID_LEN_SIV  (39  /*int(log(10,exp(2,128)-1)+1) digits*/)

/* API return codes */
typedef enum {
    UUID_RC_OK   = 0,        /* everything ok    */
    UUID_RC_ARG  = 1,        /* invalid argument */
    UUID_RC_MEM  = 2,        /* out of memory    */
    UUID_RC_SYS  = 3,        /* system error     */
    UUID_RC_INT  = 4,        /* internal error   */
    UUID_RC_IMP  = 5         /* not implemented  */
} uuid_rc_t;

/* UUID make modes */
enum {
    UUID_MAKE_V1 = (1 << 0), /* DCE 1.1 v1 UUID */
    UUID_MAKE_V3 = (1 << 1), /* DCE 1.1 v3 UUID */
    UUID_MAKE_V4 = (1 << 2), /* DCE 1.1 v4 UUID */
    UUID_MAKE_V5 = (1 << 3), /* DCE 1.1 v5 UUID */
    UUID_MAKE_MC = (1 << 4)  /* enforce multi-cast MAC address */
};

/* UUID import/export formats */
typedef enum {
    UUID_FMT_BIN = 0,        /* binary representation (import/export) */
    UUID_FMT_STR = 1,        /* string representation (import/export) */
    UUID_FMT_SIV = 2,        /* single integer value  (import/export) */
    UUID_FMT_TXT = 3         /* textual description   (export only)   */
} uuid_fmt_t;

/* UUID abstract data type */
struct uuid_st;
typedef struct uuid_st uuid_t;

/* UUID object handling */
extern uuid_rc_t     uuid_create   (      uuid_t **_uuid);
extern uuid_rc_t     uuid_destroy  (      uuid_t  *_uuid);
extern uuid_rc_t     uuid_clone    (const uuid_t  *_uuid, uuid_t **_clone);

/* UUID generation */
extern uuid_rc_t     uuid_load     (      uuid_t  *_uuid, const char *_name);
extern uuid_rc_t     uuid_make     (      uuid_t  *_uuid, unsigned int _mode, ...);

/* UUID comparison */
extern uuid_rc_t     uuid_isnil    (const uuid_t  *_uuid,                       int *_result);
extern uuid_rc_t     uuid_compare  (const uuid_t  *_uuid, const uuid_t *_uuid2, int *_result);

/* UUID import/export */
extern uuid_rc_t     uuid_import   (      uuid_t  *_uuid, uuid_fmt_t _fmt, const void  *_data_ptr, size_t  _data_len);
extern uuid_rc_t     uuid_export   (const uuid_t  *_uuid, uuid_fmt_t _fmt,       void  *_data_ptr, size_t *_data_len);

/* library utilities */
extern char         *uuid_error    (uuid_rc_t _rc);
extern unsigned long uuid_version  (void);

DECLARATION_END

#endif /* __UUID_H__ */

