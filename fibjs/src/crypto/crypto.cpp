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
    unsigned long err = ERR_get_error();
    const char* reason = ERR_reason_error_string(err);

    if (reason) {
        // Node.js exposes the OpenSSL reason as ERR_OSSL_<REASON>.
        exlib::string code = "ERR_OSSL_";

        for (const char* p = reason; *p; p++) {
            char c = *p;
            if (c == ' ')
                code += '_';
            else
                code += (char)((c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c);
        }

        return Runtime::setError(ErrorPayload::make(errtype::kError)
                .with_code(code)
                .with_message(ERR_error_string(err, NULL)));
    }

    return Runtime::setError(ERR_error_string(err, NULL));
}

}
