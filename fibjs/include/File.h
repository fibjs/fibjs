#include "ifs/File.h"
#include "Stat.h"

#ifndef _fj_FILE_H
#define _fj_FILE_H

namespace fibjs
{

class File : public File_base
{
public:
    File(FILE *pipe) : m_pipe(pipe), m_fd(_fileno(pipe))
    {
    }

    File() : m_pipe(NULL), m_fd(-1)
    {
    }

    ~File();

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, AsyncEvent *ac);
    virtual result_t close(AsyncEvent *ac);
    virtual result_t copyTo(Stream_base *stm, int64_t bytes, int64_t &retVal, AsyncEvent *ac);

public:
    // SeekableStream_base
    virtual result_t seek(int64_t offset, int32_t whence);
    virtual result_t tell(int64_t &retVal);
    virtual result_t rewind();
    virtual result_t size(int64_t &retVal);
    virtual result_t readAll(obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t stat(obj_ptr<Stat_base> &retVal, AsyncEvent *ac);

public:
    // File_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t truncate(int64_t bytes, AsyncEvent *ac);
    virtual result_t eof(bool &retVal);
    virtual result_t flush(AsyncEvent *ac);
    virtual result_t chmod(int32_t mode, AsyncEvent *ac);

public:
    result_t open(const char *fname, const char *flags, AsyncEvent *ac);
    result_t Write(const char *p, int32_t sz);

protected:
    std::string name;
    FILE *m_pipe;
    int32_t m_fd;
};

}

#endif // FILE_H
