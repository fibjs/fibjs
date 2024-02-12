/*
 * crypto.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/crypto.h"
#include "Cipher.h"
#include "Buffer.h"
#include "Digest.h"
#include "PKey.h"
#include "ECKey.h"
#include "PKey_rsa.h"
#include "X509Cert.h"
#include "X509Crl.h"
#include "X509Req.h"
#include "ssl.h"
#include <time.h>
#include <string.h>

namespace fibjs {

DECLARE_MODULE(crypto);

result_t crypto_base::loadCert(exlib::string filename, obj_ptr<X509Cert_base>& retVal)
{
    return X509Cert::loadFile(filename, retVal);
}

result_t crypto_base::loadCrl(exlib::string filename, obj_ptr<X509Crl_base>& retVal)
{
    return X509Crl::loadFile(filename, retVal);
}

result_t crypto_base::loadReq(exlib::string filename, obj_ptr<X509Req_base>& retVal)
{
    return X509Req::loadFile(filename, retVal);
}

result_t crypto_base::loadPKey(exlib::string filename, obj_ptr<PKey_base>& retVal)
{
    return PKey::loadFile(filename, retVal);
}

result_t crypto_base::generateKey(int32_t size, obj_ptr<PKey_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return PKey_rsa::generateKey(size, retVal);
}

result_t crypto_base::generateKey(exlib::string curve, obj_ptr<PKey_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return ECKey::generateKey(curve, retVal);
}

}
