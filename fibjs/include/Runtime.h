/*
 * Runtime.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "utils.h"
#include "DateCache.h"

#ifndef RUNTIME_H_
#define RUNTIME_H_

namespace fibjs
{

class Runtime
{
public:
    static Runtime &now();
    static void reg(Runtime *rt);

    static result_t setError(result_t hr)
    {
        Runtime &rt = Runtime::now();

        rt.m_code = hr;
        return rt.m_code;
    }

    static result_t setError(std::string err)
    {
        Runtime &rt = Runtime::now();

        rt.m_code = CALL_E_EXCEPTION;
        rt.m_error = err;
        return rt.m_code;
    }

    static result_t setError(const char *err = NULL)
    {
        Runtime &rt = Runtime::now();

        rt.m_code = CALL_E_EXCEPTION;
        rt.m_error.assign(err ? err : "");
        return rt.m_code;
    }

    static const std::string &errMessage()
    {
        return Runtime::now().m_error;
    }

    static result_t errNumber()
    {
        return Runtime::now().m_code;
    }

public:
    DateCache *m_pDateCache;

private:
    result_t m_code;
    std::string m_error;
};

} /* namespace fibjs */
#endif /* RUNTIME_H_ */
