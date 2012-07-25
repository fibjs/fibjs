/*
 * Runtime.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "utils.h"

#ifndef RUNTIME_H_
#define RUNTIME_H_

namespace fibjs
{

class Runtime
{
public:
	static Runtime& now();
	static void reg(Runtime* rt);

	static result_t setError(std::string& err)
	{
		Runtime::now().m_error = err;
		return CALL_E_EXCEPTION;
	}

	static result_t setError(const char* err = NULL)
	{
		Runtime::now().m_error.assign(err ? err : "");
		return CALL_E_EXCEPTION;
	}

	static const std::string& error()
	{
		return Runtime::now().m_error;
	}

private:
	std::string m_error;
};

} /* namespace fibjs */
#endif /* RUNTIME_H_ */
