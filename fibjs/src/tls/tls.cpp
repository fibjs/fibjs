/*
 * tls.cpp
 *
 *  Created on: Mar 1, 2024
 *      Author: lion
 */

#include "crypto_util.h"
#include "ifs/tls.h"

namespace fibjs {

DECLARE_MODULE(tls);

void init_tls()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

}
