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
#include "ssl.h"
#include <time.h>
#include <string.h>

namespace fibjs {

DECLARE_MODULE(crypto);

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
