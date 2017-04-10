/*
 * Digest.h
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "ifs/Digest.h"
#include <mbedtls/mbedtls/md.h>

#ifndef DIGEST_H_
#define DIGEST_H_

namespace fibjs {

class Digest : public Digest_base {
public:
    Digest(mbedtls_md_type_t algo);
    Digest(mbedtls_md_type_t algo, const char* key, int32_t sz);
    ~Digest();

public:
    // Digest_base
    virtual result_t update(Buffer_base* data);
    virtual result_t digest(Buffer_base* data, obj_ptr<Buffer_base>& retVal);
    virtual result_t digest(obj_ptr<Buffer_base>& retVal);
    virtual result_t get_size(int32_t& retVal);

private:
    mbedtls_md_context_t m_ctx;
    int32_t m_iAlgo;
    bool m_bMac;
};

} /* namespace fibjs */
#endif /* DIGEST_H_ */
