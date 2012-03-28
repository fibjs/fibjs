#include "ifs/file.h"

#ifndef FILE_H
#define FILE_H

namespace fibjs
{

class File : public file_base
{
public:
    File() : m_data(0)
    {
    }

public:
    result_t get_data(double& retVal);
    result_t set_data(double newVal);
    result_t getData(double& retVal);

private:
    double m_data;
};

}

#endif // FILE_H
