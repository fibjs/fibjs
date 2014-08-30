/*
 * Digest.h
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "ifs/Digest.h"
#include <polarssl/polarssl/md.h>

#ifndef DIGEST_H_
#define DIGEST_H_

namespace fibjs
{

class Digest : public Digest_base
{
public:
    Digest(md_type_t algo);
    Digest(md_type_t algo, const char *key, int sz);
    ~Digest();

public:
    // Digest_base
    virtual result_t update(Buffer_base *data);
    virtual result_t digest(Buffer_base *data, obj_ptr<Buffer_base> &retVal);
    virtual result_t digest(obj_ptr<Buffer_base> &retVal);
    virtual result_t get_size(int32_t &retVal);

private:
    md_context_t m_ctx;
    int m_iAlgo;
    bool m_bMac;
};

} /* namespace fibjs */
#endif /* DIGEST_H_ */
