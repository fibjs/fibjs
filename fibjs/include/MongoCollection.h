/*
 * MongoCollection.h
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "ifs/MongoCollection.h"

#ifndef MONGOCOLLECTION_H_
#define MONGOCOLLECTION_H_

namespace fibjs
{

class MongoCollection: public MongoCollection_base
{
public:
	MongoCollection();
	virtual ~MongoCollection();
};

} /* namespace fibjs */
#endif /* MONGOCOLLECTION_H_ */
