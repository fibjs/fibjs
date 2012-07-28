/*
 * DBField.h
 *
 *  Created on: Jul 28, 2012
 *      Author: lion
 */

#include <vector>

#ifndef DBFIELD_H_
#define DBFIELD_H_

namespace fibjs
{

class DBField: public obj_base
{
public:
	DBField(int32_t sz)
	{
		m_fields.resize(sz);
		m_types.resize(sz);
	}

public:
	void setField(int32_t i, int32_t type, std::string& s)
	{
		m_fields[i] = s;
		m_types[i] = type;
	}

	uint32_t index(const char* name)
	{
		uint32_t i;

		for (i = 0; i < (uint32_t) m_fields.size(); i++)
			if (!qstricmp(name, m_fields[i].c_str()))
				return i;

		return -1;
	}

	std::string& name(int32_t i)
	{
		return m_fields[i];
	}

private:
	std::vector<std::string> m_fields;
	std::vector<int32_t> m_types;
};

}

#endif /* DBFIELD_H_ */
