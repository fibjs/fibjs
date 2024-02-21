/*
 * error.cpp
 *
 *  Created on: Feb 11, 2024
 *      Author: lion
 */

#include "object.h"
#include "crypto_util.h"
#include "ifs/crypto.h"
#include <openssl/err.h>

namespace fibjs {

result_t openssl_error()
{
    return Runtime::setError(ERR_error_string(ERR_get_error(), NULL));
}

}
