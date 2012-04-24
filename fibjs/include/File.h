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
        close();
    }

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t write(obj_ptr<Buffer_base> data);
	virtual result_t flush();
	virtual result_t close();

public:
	// File_base
	virtual result_t get_name(std::string& retVal);
	virtual result_t stat(obj_ptr<Stat_base>& retVal);
	virtual result_t size(double& retVal);
	virtual result_t eof(bool& retVal);
	virtual result_t seek(double offset, int32_t whence);
	virtual result_t tell(double& retVal);
	virtual result_t rewind();
	virtual result_t truncate(double bytes);

public:
    result_t Open(const char* fname, const char* mode);
    result_t Write(const char* p, int sz);

private:
	result_t sync_read(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	result_t sync_write(obj_ptr<Buffer_base> data);

private:
	ASYNC_MEMBER2(File, sync_read);
	ASYNC_MEMBER1(File, sync_write);

private:
    std::string name;
    double m_data;
    FILE* m_file;
};

}

#endif // FILE_H
