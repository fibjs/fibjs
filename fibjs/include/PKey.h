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
    virtual result_t genEcKey(int32_t size, exlib::AsyncEvent *ac);

private:
    void clear();

private:
    pk_context m_key;
};

}

#endif
