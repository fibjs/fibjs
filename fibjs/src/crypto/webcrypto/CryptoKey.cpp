/*
 * CryptoKey.cpp
 *
 *  Created on: Aug 25, 2024
 *      Author: lion
 */

#include "object.h"
#include "CryptoKey.h"

namespace fibjs {

result_t CryptoKey::get_type(exlib::string& retVal)
{
    return m_key->get_type(retVal);
}

result_t CryptoKey::get_algorithm(v8::Local<v8::Object>& retVal)
{
    return m_algorithm->valueOf(retVal);
}

result_t CryptoKey::get_extractable(bool& retVal)
{
    retVal = m_extractable;
    return 0;
}

result_t CryptoKey::get_usages(std::vector<exlib::string>& retVal)
{
    for (auto& it : m_usageMap)
        retVal.push_back(it.first);

    return 0;
}

}
