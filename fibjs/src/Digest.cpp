/*
 * Digest.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "ifs/hash.h"
#include "Digest.h"
#include "Buffer.h"
#include <crypto/md2.h>
#include <crypto/md4.h>
#include <crypto/md5.h>
#include <crypto/sha1.h>
#include <crypto/sha2.h>
#include <crypto/sha4.h>
#include <crypto/ripemd.h>

namespace fibjs
{

static void sha224_starts(sha2_context *ctx)
{
	sha2_starts(ctx, 1);
}

static void sha256_starts(sha2_context *ctx)
{
	sha2_starts(ctx, 0);
}

static void sha384_starts(sha4_context *ctx)
{
	sha4_starts(ctx, 1);
}

static void sha512_starts(sha4_context *ctx)
{
	sha4_starts(ctx, 0);
}

static struct hashAlgo
{
	const char* Name;
	short Size;
	short PadSize;
	int (*Init)(void *);
	int (*Update)(void *, const unsigned char *, unsigned long);
	int (*Final)(void *, unsigned char *);
} s_HashAlgos[] =
{
{
"md2", 16, 64,
(int (*)(void *))md2_starts,
(int (*)(void *, const unsigned char *, unsigned long))md2_update,
(int (*)(void *, unsigned char *))md2_finish
},
{"md4", 16, 64,
(int (*)(void *))md4_starts,
(int (*)(void *, const unsigned char *, unsigned long))md4_update,
(int (*)(void *, unsigned char *))md4_finish
},
{"md5", 16, 64,
(int (*)(void *))md5_starts,
(int (*)(void *, const unsigned char *, unsigned long))md5_update,
(int (*)(void *, unsigned char *))md5_finish
},
{"ripemd128", 16, 64,
(int (*)(void *))ripemd128_init,
(int (*)(void *, const unsigned char *, unsigned long))ripemd_update,
(int (*)(void *, unsigned char *))ripemd_final
},
{"ripemd160", 20, 64,
(int (*)(void *))ripemd160_init,
(int (*)(void *, const unsigned char *, unsigned long))ripemd_update,
(int (*)(void *, unsigned char *))ripemd_final
},
{"ripemd256", 32, 64,
(int (*)(void *))ripemd256_init,
(int (*)(void *, const unsigned char *, unsigned long))ripemd_update,
(int (*)(void *, unsigned char *))ripemd_final
},
{"ripemd320", 40, 64,
(int (*)(void *))ripemd320_init,
(int (*)(void *, const unsigned char *, unsigned long))ripemd_update,
(int (*)(void *, unsigned char *))ripemd_final
},
{"sha1", 20, 64,
(int (*)(void *))sha1_starts,
(int (*)(void *, const unsigned char *, unsigned long))sha1_update,
(int (*)(void *, unsigned char *))sha1_finish
},
{"sha224", 28, 64,
(int (*)(void *))sha224_starts,
(int (*)(void *, const unsigned char *, unsigned long))sha2_update,
(int (*)(void *, unsigned char *))sha2_finish
},
{"sha256", 32, 64,
(int (*)(void *))sha256_starts,
(int (*)(void *, const unsigned char *, unsigned long))sha2_update,
(int (*)(void *, unsigned char *))sha2_finish
},
{"sha384", 48, 128,
(int (*)(void *))sha384_starts,
(int (*)(void *, const unsigned char *, unsigned long))sha4_update,
(int (*)(void *, unsigned char *))sha4_finish
},
{"sha512", 64, 128,
(int (*)(void *))sha512_starts,
(int (*)(void *, const unsigned char *, unsigned long))sha4_update,
(int (*)(void *, unsigned char *))sha4_finish
}
};

Digest::Digest(int32_t algo)
{
	m_iAlgo = algo;
	m_bMac = false;
	s_HashAlgos[m_iAlgo].Init(&m_ctx);
}

Digest::Digest(int32_t algo, const char* key, int sz)
{
	unsigned char ctx[256];
	unsigned char sum[64];
	int i;

	m_iAlgo = algo;
	m_bMac = true;

	hashAlgo& HA = s_HashAlgos[m_iAlgo];

	HA.Init(&m_ctx);

	if (sz > HA.PadSize)
	{
		HA.Init(&ctx);
		HA.Update(&ctx, (unsigned char*) key, sz);
		HA.Final(&ctx, sum);

		sz = HA.Size;
		key = (char*) sum;
	}

	memset(m_ipad, 0x36, HA.PadSize);
	memset(m_opad, 0x5C, HA.PadSize);

	for (i = 0; i < sz; i++)
	{
		m_ipad[i] = (unsigned char) (m_ipad[i] ^ key[i]);
		m_opad[i] = (unsigned char) (m_opad[i] ^ key[i]);
	}

	HA.Update(&m_ctx, m_ipad, HA.PadSize);
}

result_t Digest::update(obj_ptr<Buffer_base>& data)
{
	if (m_iAlgo < 0)
		return CALL_E_INVALID_CALL;

	std::string str;
	data->toString(str);

	s_HashAlgos[m_iAlgo].Update(&m_ctx, (const unsigned char*) str.c_str(),
			(int)str.length());

	return 0;
}

result_t Digest::update(const char* text)
{
	if (m_iAlgo < 0)
		return CALL_E_INVALID_CALL;

	s_HashAlgos[m_iAlgo].Update(&m_ctx, (const unsigned char*) text,
			(int)qstrlen(text));

	return 0;
}

result_t Digest::digest(obj_ptr<Buffer_base>& retVal)
{
	if (m_iAlgo < 0)
		return CALL_E_INVALID_CALL;

	hashAlgo& HA = s_HashAlgos[m_iAlgo];
	std::string strBuf;

	if (m_bMac)
	{
		unsigned char sum[64];

		HA.Final(&m_ctx, sum);

		HA.Init(&m_ctx);
		HA.Update(&m_ctx, m_opad, HA.PadSize);
		HA.Update(&m_ctx, sum, HA.Size);
	}

	m_iAlgo = -1;

	strBuf.resize(HA.Size);
	HA.Final(&m_ctx, (unsigned char*) &strBuf[0]);

	retVal = new Buffer(strBuf);

	return 0;
}

result_t Digest::digest(obj_ptr<Buffer_base>& data,
		obj_ptr<Buffer_base>& retVal)
{
	if (m_iAlgo < 0)
		return CALL_E_INVALID_CALL;

	update(data);
	return digest(retVal);
}

result_t Digest::digest(const char* text, obj_ptr<Buffer_base>& retVal)
{
	if (m_iAlgo < 0)
		return CALL_E_INVALID_CALL;

	update(text);
	return digest(retVal);
}

result_t Digest::get_size(int32_t& retVal)
{
	if (m_iAlgo < 0)
		return CALL_E_INVALID_CALL;

	retVal = s_HashAlgos[m_iAlgo].Size;
	return 0;
}

} /* namespace fibjs */
