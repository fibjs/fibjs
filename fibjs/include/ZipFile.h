/*
 * ZipFile.h
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

#pragma once

#include "ifs/ZipFile.h"
#include "ifs/SeekableStream.h"
#include "unzip/include/unzip.h"
#include "unzip/include/zip.h"

namespace fibjs {

class ZipFile : public ZipFile_base {
public:
    ZipFile(SeekableStream_base* strm, exlib::string mod, exlib::string codec);

public:
    // ZipFile_base
    virtual result_t namelist(std::vector<exlib::string>& retVal, AsyncEvent* ac);
    virtual result_t infolist(std::vector<obj_ptr<ZipFile_base::InfolistType>>& retVal, AsyncEvent* ac);
    virtual result_t getinfo(exlib::string member, obj_ptr<ZipFile_base::GetinfoType>& retVal, AsyncEvent* ac);
    virtual result_t read(exlib::string member, exlib::string password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t readAll(exlib::string password, std::vector<obj_ptr<ZipFile_base::ReadAllType>>& retVal, AsyncEvent* ac);
    virtual result_t extract(exlib::string member, exlib::string path, exlib::string password, AsyncEvent* ac);
    virtual result_t extract(exlib::string member, SeekableStream_base* strm, exlib::string password, AsyncEvent* ac);
    virtual result_t extractAll(exlib::string path, exlib::string password, AsyncEvent* ac);
    virtual result_t write(exlib::string filename, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
    virtual result_t write(SeekableStream_base* strm, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);

private:
    template <class T>
    result_t get_info(obj_ptr<T>& retVal);
    result_t extract(SeekableStream_base* strm, exlib::string password);
    result_t read(exlib::string password, obj_ptr<Buffer_base>& retVal);
    result_t write(exlib::string filename, exlib::string password, SeekableStream_base* strm);
    result_t getFileCrc(SeekableStream_base* strm, uint32_t& crc);
    result_t checkGuard(exlib::string path);

private:
    unzFile m_unz;
    zipFile m_zip;
    exlib::string m_codec;
    exlib::string m_mod;
    obj_ptr<SeekableStream_base> m_strm;
};

} /* namespace fibjs */
