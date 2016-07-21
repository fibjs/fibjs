/*
 * MongoID.h
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "ifs/MongoID.h"
#include <mongo/include/bson.h>

#ifndef MONGOID_H_
#define MONGOID_H_

namespace fibjs
{

class MongoID: public MongoID_base
{
public:
    MongoID(const bson_oid_t *id) :
        m_id(*id)
    {
    }

    MongoID(exlib::string str)
    {
        if (!str.empty())
            bson_oid_from_string(&m_id, str.c_str());
        else
            bson_oid_gen(&m_id);
    }

public:
    virtual result_t toString(exlib::string &retVal)
    {
        char buf[25];

        bson_oid_to_string(&m_id, buf);
        retVal.assign(buf, 24);

        return 0;
    }

    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value> &retVal)
    {
        exlib::string str;

        toString(str);
        retVal = holder()->NewFromUtf8(str);

        return 0;
    }

public:
    bson_oid_t m_id;
};

} /* namespace fibjs */
#endif /* MONGOID_H_ */
