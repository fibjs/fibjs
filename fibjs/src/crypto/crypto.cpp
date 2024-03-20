/*
 * crypto.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/crypto.h"
#include "crypto_util.h"
#include <openssl/err.h>

namespace fibjs {

DECLARE_MODULE(crypto);

result_t openssl_error()
{
    return Runtime::setError(ERR_error_string(ERR_get_error(), NULL));
}

}
