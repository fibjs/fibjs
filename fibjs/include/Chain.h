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
                            exlib::AsyncEvent *ac);

public:
    // Chain_base
    virtual result_t append(v8::Local<v8::Array> hdlrs);
    virtual result_t append(v8::Local<v8::Value> hdlr);

public:
    result_t append(Handler_base *hdlr)
    {
        m_array.append(hdlr);
        return 0;
    }

private:
    QuickArray<obj_ptr<Handler_base> > m_array;
};

} /* namespace fibjs */
#endif /* CHAIN_H_ */
