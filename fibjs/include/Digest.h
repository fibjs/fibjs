/*
 * Digest.h
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "ifs/Digest.h"

#ifndef DIGEST_H_
#define DIGEST_H_

namespace fibjs
{

class Digest : public Digest_base
{
public:
    Digest(int32_t algo);
    Digest(int32_t algo, const char *key, int sz);

public:
    // Digest_base
    virtual result_t update(Buffer_base *data);
    virtual result_t update(const char *text);
    virtual result_t digest(Buffer_base *data, obj_ptr<Buffer_base> &retVal);
    virtual result_t digest(const char *text, obj_ptr<Buffer_base> &retVal);
    virtual result_t get_size(int32_t &retVal);

public:
    result_t digest(obj_ptr<Buffer_base> &retVal);

private:
    unsigned char m_ctx[256];
    int m_iAlgo;
    bool m_bMac;
    unsigned char m_ipad[128];
    unsigned char m_opad[128];
};

} /* namespace fibjs */
#endif /* DIGEST_H_ */
