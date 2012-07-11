/*
 * uuidVar.h
 *
 *  Created on: Jul 6, 2012
 *      Author: lion
 */

#include "ifs/uuidValue.h"
#include "uuid.h"

#ifndef UUIDVAR_H_
#define UUIDVAR_H_

namespace fibjs
{

class uuidVar : public uuidValue_base
{
public:
	uuidVar() : m_uid(NULL)
	{
	}

	virtual ~uuidVar();

public:
	// object_base
	result_t toString(std::string& retVal);

public:
	// uuidValue_base
	result_t data(obj_ptr<Buffer_base>& retVal);
	result_t detail(std::string& retVal);

public:
	uuid_st *m_uid;
};

} /* namespace fibjs */
#endif /* UUIDVAR_H_ */
