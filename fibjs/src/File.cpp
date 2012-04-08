/*
 * File.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"

#ifdef SEEK_SET
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#endif

#include "File.h"

namespace fibjs
{

result_t File::get_readable(bool& retVal)
{
    return 0;
}

result_t File::get_writable(bool& retVal)
{
    return 0;
}

result_t File::read(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    return 0;
}

result_t File::write(obj_ptr<Buffer_base> data)
{
    return 0;
}

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

result_t File::Open(const char* fname, const char* mode)
{
    Close();

    m_file = fopen(fname, mode);
    if(m_file == NULL)
        return LastError();

    return 0;
}

result_t File::Close()
{
    if(m_file)
        fclose(m_file);

    return 0;
}

}
