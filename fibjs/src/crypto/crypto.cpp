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

result_t openssl_error(const char* arg_name, const exlib::string& arg_value)
{
    unsigned long err = ERR_get_error();
    const char* reason = ERR_reason_error_string(err);
    exlib::string code;

    if (reason) {
        // Node.js exposes the OpenSSL reason as ERR_OSSL_<REASON>.
        code = "ERR_OSSL_";

        for (const char* p = reason; *p; p++) {
            char c = *p;
            if (c == ' ')
                code += '_';
            else
                code += (char)((c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c);
        }
    }

    result_t::builder b = result_t::make(CALL_E_EXCEPTION).with_message(ERR_error_string(err, NULL));

    if (!code.empty())
        b = std::move(b).with_code(code);
    if (arg_name && !arg_value.empty())
        b = std::move(b).arg(arg_name, arg_value);

    return result_t(std::move(b));
}

}
