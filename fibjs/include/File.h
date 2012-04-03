#include "ifs/File.h"

#ifndef _fj_FILE_H
#define _fj_FILE_H

namespace fibjs
{

class File : public File_base
{
public:
    File() : m_data(0), m_file(NULL)
    {
    }

    ~File()
    {
        Close();
    }

public:
    result_t get_readable(bool& retVal);
    result_t get_writable(bool& retVal);
    result_t read(int32_t size, obj_ptr<Buffer_base>& retVal);
    result_t write(obj_ptr<Buffer_base> data);

public:
    result_t get_data(double& retVal);
    result_t set_data(double newVal);
    result_t getData(double& retVal);


public:
    result_t Open(const char* fname, const char* mode);
    result_t Close();

private:
    double m_data;
    FILE* m_file;
};

}

#endif // FILE_H
