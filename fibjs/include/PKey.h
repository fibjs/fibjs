/*
 * PKey.h
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#include "ifs/PKey.h"
#include "polarssl/config.h"
#include "polarssl/pk.h"

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
    virtual result_t genRsaKey(int32_t size, exlib::AsyncEvent *ac);
    virtual result_t genEcKey(const char *curve, exlib::AsyncEvent *ac);
    virtual result_t isPrivate(bool &retVal);
    virtual result_t publicKey(obj_ptr<PKey_base> &retVal);
    virtual result_t import(Buffer_base *DerKey, const char *password);
    virtual result_t import(const char *pemKey, const char *password);
    virtual result_t exportPem(std::string &retVal);
    virtual result_t exportDer(obj_ptr<Buffer_base> &retVal);
    virtual result_t encrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t decrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);

private:
    void clear();

private:
    pk_context m_key;
};

}

#endif
