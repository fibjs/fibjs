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
		mongo_init(&m_mongo);
	}

	~MongoDB()
	{
		mongo_destroy(&m_mongo);
	}

public:
	// MongoDB_base
	virtual result_t getCollection(const char* name, obj_ptr<MongoCollection_base>& retVal);

private:
	mongo m_mongo;
};

} /* namespace fibjs */
#endif /* MONGODB_H_ */
