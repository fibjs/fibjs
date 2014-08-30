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
        mongo_destroy(&m_conn);
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
    virtual result_t close();

public:
    result_t open(const char *connString);
    result_t error();
    result_t run_command(bson *command, v8::Local<v8::Object> &retVal);

public:
    mongo m_conn;
    std::string m_ns;
};

} /* namespace fibjs */
#endif /* MONGODB_H_ */
