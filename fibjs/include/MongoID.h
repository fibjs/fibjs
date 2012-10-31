/*
 * MongoID.h
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "ifs/MongoID.h"
#include <mongo/bson.h>

#ifndef MONGOID_H_
#define MONGOID_H_

namespace fibjs
{

class MongoID: public MongoID_base
{
public:
	MongoID(const bson_oid_t* id) :
			m_id(*id)
	{
	}

	MongoID(const char* str)
	{
		bson_oid_from_string(&m_id, str);
	}

public:
	virtual result_t toString(std::string& retVal)
	{
		char buf[25];

		bson_oid_to_string(&m_id, buf);
		retVal.assign(buf, 24);

		return 0;
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Value>& retVal)
	{
		std::string str;

		toString(str);
		retVal = v8::String::New(str.c_str(), (int)str.length());

		return 0;
	}

public:
	bson_oid_t m_id;
};

} /* namespace fibjs */
#endif /* MONGOID_H_ */
