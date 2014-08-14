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

    ~File()
    {
        if (m_fd != -1)
        {
            if (exlib::Service::hasService())
                ac_close();
            else
            {
                asyncEvent ac;
                close(&ac);
            }
        }
    }

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, exlib::AsyncEvent *ac);
    virtual result_t close(exlib::AsyncEvent *ac);
    virtual result_t copyTo(Stream_base *stm, int64_t bytes, int64_t &retVal, exlib::AsyncEvent *ac);

public:
    // SeekableStream_base
    virtual result_t seek(int64_t offset, int32_t whence);
    virtual result_t tell(int64_t &retVal);
    virtual result_t rewind();
    virtual result_t size(int64_t &retVal);
    virtual result_t readAll(obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t stat(obj_ptr<Stat_base> &retVal, exlib::AsyncEvent *ac);

public:
    // File_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t truncate(int64_t bytes, exlib::AsyncEvent *ac);
    virtual result_t eof(bool &retVal);
    virtual result_t flush(exlib::AsyncEvent *ac);
    virtual result_t chmod(int32_t mode, exlib::AsyncEvent *ac);

public:
    result_t open(const char *fname, const char *flags, exlib::AsyncEvent *ac);
    result_t Write(const char *p, int sz);

protected:
    std::string name;
    FILE *m_pipe;
    int m_fd;
};

}

#endif // FILE_H
