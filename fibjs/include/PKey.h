/*
 * PKey.h
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#include "ifs/PKey.h"
#include <mbedtls/mbedtls/config.h>
#include <mbedtls/mbedtls/pk.h>

#ifndef _fj_PKEY_H
#define _fj_PKEY_H

namespace fibjs
{

class PKey : public PKey_base
{
public:
    PKey();
    ~PKey();

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // PKey_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t get_keySize(int32_t &retVal);
    virtual result_t get_publicKey(obj_ptr<PKey_base> &retVal);
    virtual result_t genRsaKey(int32_t size, AsyncEvent *ac);
    virtual result_t genEcKey(const char *curve, AsyncEvent *ac);
    virtual result_t isPrivate(bool &retVal);
    virtual result_t clone(obj_ptr<PKey_base> &retVal);
    virtual result_t importKey(Buffer_base *DerKey, const char *password);
    virtual result_t importKey(const char *pemKey, const char *password);
    virtual result_t importFile(const char* filename, const char* password);
    virtual result_t exportPem(std::string &retVal);
    virtual result_t exportDer(obj_ptr<Buffer_base> &retVal);
    virtual result_t encrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t decrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t sign(Buffer_base *data, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t verify(Buffer_base *sign, Buffer_base *data, bool &retVal, AsyncEvent *ac);

public:
    result_t copy(const mbedtls_pk_context &key);

private:
    void clear();

public:
    mbedtls_pk_context m_key;
};

}

#endif
