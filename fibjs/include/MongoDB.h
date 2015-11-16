/*
 * MongoDB.h
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "ifs/MongoDB.h"
#include <mongo/include/mongo.h>
extern "C"
{
#include <mongo/include/gridfs.h>
}

#ifndef MONGODB_H_
#define MONGODB_H_

namespace fibjs
{

class MongoDB: public MongoDB_base
{
public:
    MongoDB()
    {
        mongo_init(&m_conn);
    }

    ~MongoDB()
    {
        if (mongo_is_connected(&m_conn))
            asyncCall(mongo_destroy, &m_conn);
    }

public:
    // MongoDB_base
    virtual result_t getCollection(const char *name, obj_ptr<MongoCollection_base> &retVal);
    virtual result_t runCommand(v8::Local<v8::Object> cmd, v8::Local<v8::Object> &retVal);
    virtual result_t runCommand(const char *cmd, v8::Local<v8::Value> arg, v8::Local<v8::Object> &retVal);
    virtual result_t _named_getter(const char *property, obj_ptr<MongoCollection_base> &retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array> &retVal);
    virtual result_t get_fs(obj_ptr<GridFS_base> &retVal);
    virtual result_t oid(const char *hexStr, obj_ptr<MongoID_base> &retVal);
    virtual result_t close(AsyncEvent* ac);

public:
    result_t open(const char *connString);
    result_t error();
    result_t bsonHandler(bson *command, v8::Local<v8::Object> &retVal);
    result_t _runCommand(bson *command, bson &out, AsyncEvent *ac);
    ASYNC_MEMBERVALUE2(MongoDB, _runCommand, bson *, bson);

public:
    mongo m_conn;
    std::string m_ns;
};

} /* namespace fibjs */
#endif /* MONGODB_H_ */
