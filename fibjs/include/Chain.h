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
        return 0;
    }

public:
    // Chain_base
    virtual result_t append(v8::Local<v8::Array> hdlrs);
    virtual result_t append(v8::Local<v8::Value> hdlr);

public:
    result_t append(Handler_base *hdlr)
    {
        Isolate* isolate = Isolate::now();
        v8::Local<v8::String> k = v8::String::NewFromUtf8(isolate->isolate, "handler");
        v8::Local<v8::Value> v = wrap()->GetHiddenValue(k);
        v8::Local<v8::Array> a;

        if (IsEmpty(v))
        {
            a = v8::Array::New(isolate->isolate);
            wrap()->SetHiddenValue(k, a);
        }
        else
            a = v8::Local<v8::Array>::Cast(v);

        a->Set((int32_t)m_array.size(), hdlr->wrap());
        m_array.append(hdlr);

        return 0;
    }

private:
    QuickArray<naked_ptr<Handler_base> > m_array;
};

} /* namespace fibjs */
#endif /* CHAIN_H_ */
