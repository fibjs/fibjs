/*
 * MongoDB.h
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "ifs/MongoDB.h"
#include <mongo/mongo.h>

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
	virtual result_t getCollection(const char* name, obj_ptr<MongoCollection_base>& retVal);
	virtual result_t _named_getter(const char* property, obj_ptr<MongoCollection_base>& retVal);

public:
	result_t open(const char* connString);
	result_t error();

public:
	mongo m_conn;
	std::string m_ns;
};

} /* namespace fibjs */
#endif /* MONGODB_H_ */
