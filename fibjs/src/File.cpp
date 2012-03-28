#include "File.h"

namespace fibjs
{

result_t File::get_data(double& retVal)
{
    retVal = m_data;
    return 0;
}

result_t File::set_data(double newVal)
{
    m_data = newVal;
    return 0;
}

result_t File::getData(double& retVal)
{
    retVal = m_data;
    return 0;
}

}
