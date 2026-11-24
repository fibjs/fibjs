/*
 * ZipFile.h
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

#pragma once

#include <unordered_map>

#include "ifs/ZipFile.h"
#include "ifs/SeekableStream.h"
#include "unzip/include/unzip.h"
#include "unzip/include/zip.h"

namespace fibjs {

class StreamIO;

class ZipFile : public ZipFile_base {
public:
    struct index_entry {
        unz64_file_pos pos;
        exlib::string filename;
        exlib::string compress_type;
        date_t date;
        int64_t compress_size;
        int64_t file_size;
        bool password;
    };

public:
    ZipFile(SeekableStream_base* strm, exlib::string mod, exlib::string codec);
    ~ZipFile();

public:
    // Helpers for the fs zip VFS: walk the central directory once, then read a
    // member by position without locating it by name again. The expected name
    // is checked against the directory entry so a stale position (the archive
    // changed under the cache) is rejected instead of returning another
    // entry's bytes.
    result_t get_index(std::unordered_map<exlib::string, index_entry>& retVal);
    result_t read_at(const unz64_file_pos& pos, const exlib::string& member, exlib::string password, obj_ptr<Buffer_base>& retVal);

public:
    // ZipFile_base
    virtual result_t namelist(std::vector<exlib::string>& retVal, AsyncHandle ac);
    virtual result_t infolist(std::vector<obj_ptr<ZipFile_base::InfolistType>>& retVal, AsyncHandle ac);
    virtual result_t getinfo(exlib::string member, obj_ptr<ZipFile_base::GetinfoType>& retVal, AsyncHandle ac);
    virtual result_t read(exlib::string member, exlib::string password, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    virtual result_t readAll(exlib::string password, std::vector<obj_ptr<ZipFile_base::ReadAllType>>& retVal, AsyncHandle ac);
    virtual result_t extract(exlib::string member, exlib::string path, exlib::string password, AsyncHandle ac);
    virtual result_t extract(exlib::string member, SeekableStream_base* strm, exlib::string password, AsyncHandle ac);
    virtual result_t extractAll(exlib::string path, exlib::string password, AsyncHandle ac);
    virtual result_t write(exlib::string filename, exlib::string inZipName, exlib::string password, AsyncHandle ac);
    virtual result_t write(Buffer_base* data, exlib::string inZipName, exlib::string password, AsyncHandle ac);
    virtual result_t write(SeekableStream_base* strm, exlib::string inZipName, exlib::string password, AsyncHandle ac);
    virtual result_t close(AsyncHandle ac);

private:
    template <class T>
    result_t get_info(obj_ptr<T>& retVal);
    result_t extract(SeekableStream_base* strm, exlib::string password);
    result_t read(exlib::string password, obj_ptr<Buffer_base>& retVal, const exlib::string* expect_name = NULL);
    result_t write(exlib::string filename, exlib::string password, SeekableStream_base* strm);
    result_t getFileCrc(SeekableStream_base* strm, uint32_t& crc);
    result_t checkGuard(exlib::string path);
    result_t locate(exlib::string member);
    result_t build_index();

private:
    unzFile m_unz;
    zipFile m_zip;
    exlib::string m_codec;
    exlib::string m_mod;
    obj_ptr<SeekableStream_base> m_strm;
    StreamIO* m_sio = NULL;
    bool m_index_built = false;
    std::unordered_map<exlib::string, index_entry> m_index;
};

} /* namespace fibjs */
