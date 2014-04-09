/*
 * Cipher.h
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "ifs/Cipher.h"
#include <polarssl/cipher.h>

#ifndef _fj_CIPHER_H
#define _fj_CIPHER_H

namespace fibjs
{

class Cipher : public Cipher_base
{
public:
    Cipher(const cipher_info_t *ci);

public:
    // Cipher_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t cripto(object_base *v, exlib::AsyncEvent *ac);

public:
	static result_t setError(int ret);
	
private:
    const cipher_info_t *m_ci;
};

}

#endif // _fj_CIPHER_H
