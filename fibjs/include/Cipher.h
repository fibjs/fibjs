/*
 * Cipher.h
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "ifs/Cipher.h"
#include <mbedtls/polarssl/cipher.h>

#ifndef _fj_CIPHER_H
#define _fj_CIPHER_H

namespace fibjs
{

class Cipher : public Cipher_base
{
public:
    Cipher(const cipher_info_t *ci);
    ~Cipher();

public:
    // Cipher_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t get_keySize(int32_t &retVal);
    virtual result_t get_ivSize(int32_t &retVal);
    virtual result_t get_blockSize(int32_t &retVal);
    virtual result_t paddingMode(int32_t mode);
    virtual result_t encrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t decrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);

public:
    result_t init(std::string &key, std::string &iv);

private:
    void reset();
    result_t process(const operation_t operation, Buffer_base *data, obj_ptr<Buffer_base> &retVal);

private:
    const cipher_info_t *m_info;
    cipher_context_t m_ctx;
    std::string m_key;
    std::string m_iv;
};

}

#endif // _fj_CIPHER_H
