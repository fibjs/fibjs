/*
 * Chain.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/Chain.h"
#include "QuickArray.h"

#ifndef CHAIN_H_
#define CHAIN_H_

namespace fibjs
{

class Chain: public Chain_base
{
public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            AsyncEvent *ac);

public:
    // object_base
    virtual result_t dispose()
    {
        int32_t i, sz = (int32_t)m_array.size();

        for (i = 0; i < sz; i ++)
            m_array[i].dispose();

        return 0;
    }

public:
    // Chain_base
    virtual result_t append(v8::Local<v8::Array> hdlrs);
    virtual result_t append(v8::Local<v8::Value> hdlr);

public:
    result_t append(Handler_base *hdlr)
    {
        int32_t no = (int32_t)m_array.size();

        char strBuf[32];
        sprintf(strBuf, "handler_%d", no);

        SetPrivate(strBuf, hdlr->wrap());
        m_array.append(hdlr);

        return 0;
    }

private:
    QuickArray<naked_ptr<Handler_base> > m_array;
};

} /* namespace fibjs */
#endif /* CHAIN_H_ */
