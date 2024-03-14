/*
 * ssl.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "object.h"
#include "ssl.h"
#include <mbedtls/mbedtls/error.h>

namespace fibjs {

_ssl g_ssl;

result_t _ssl::setError(int32_t ret)
{
    char msg[128];

    mbedtls_strerror(ret, msg, sizeof(msg));
    return Runtime::setError(msg);
}

}
