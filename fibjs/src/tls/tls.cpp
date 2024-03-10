/*
 * tls.cpp
 *
 *  Created on: Mar 1, 2024
 *      Author: lion
 */

#include "crypto_util.h"
#include "ifs/tls.h"
#include "TLSSocket.h"

namespace fibjs {

DECLARE_MODULE(tls);

void init_tls()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

result_t tls_base::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->m_ctx;
    return 0;
}

result_t tls_base::connect(v8::Local<v8::Object> optionns, obj_ptr<TLSSocket_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t tls_base::connect(exlib::string url, v8::Local<v8::Object> optionns, obj_ptr<TLSSocket_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t tls_base::connect(int32_t port, v8::Local<v8::Object> optionns, obj_ptr<TLSSocket_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t tls_base::connect(int32_t port, exlib::string host, v8::Local<v8::Object> optionns, obj_ptr<TLSSocket_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

}
