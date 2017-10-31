/*
 * ZipFile.h
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

#include "ifs/ZipFile.h"
#include "ifs/SeekableStream.h"
#include "unzip/include/unzip.h"
#include "unzip/include/zip.h"

#ifndef ZIPFILE_H_
#define ZIPFILE_H_

namespace fibjs {

class ZipFile : public ZipFile_base {
public:
    class Info : public NObject {
    public:
        Info(exlib::string name, unz_file_info64& info)
        {
            m_name = name;
            m_date.fromDosTime(info.dosDate);
        
            if (info.compression_method == 0)
                m_compress_type = "Stored";
            else if (info.compression_method == Z_DEFLATED)
                m_compress_type = "Deflate";
            else if (info.compression_method == Z_BZIP2ED)
                m_compress_type = "BZip2";
            else
                m_compress_type = "Unknown";
        
            m_file_size = info.uncompressed_size;
            m_compress_size = info.compressed_size;
            m_password = info.flag & 1;
        }

    public:
        result_t get_filename(exlib::string& retVal)
        {
            retVal = m_name;
            return 0;
        }
        
        result_t get_date(date_t& retVal)
        {
            retVal = m_date;
            return 0;
        }
        
        result_t get_compress_type(exlib::string& retVal)
        {
            retVal = m_compress_type;
            return 0;
        }
        
        result_t get_compress_size(int64_t& retVal)
        {
            retVal = m_compress_size;
            return 0;
        }
        
        result_t get_file_size(int64_t& retVal)
        {
            retVal = m_file_size;
            return 0;
        }
        
        result_t get_password(bool& retVal)
        {
            retVal = m_password;
            return 0;
        }
        
        result_t get_data(obj_ptr<Buffer_base>& retVal)
        {
            if (!m_data)
                return CALL_RETURN_NULL;
        
            retVal = m_data;
            return 0;
        }

    public:
        // object_base
        virtual result_t valueOf(v8::Local<v8::Value>& retVal)
        {
            add("filename", m_name);
            add("date", m_date);
            add("compress_type", m_compress_type);
            add("compress_size", m_compress_size);
            add("file_size", m_file_size);
            add("password", m_password);
            add("data", m_data);

            return NObject::valueOf(retVal);
        }

    public:
        exlib::string m_name;
        date_t m_date;
        exlib::string m_compress_type;
        int64_t m_compress_size;
        int64_t m_file_size;
        bool m_password;
        obj_ptr<Buffer_base> m_data;
    };

public:
    ZipFile(SeekableStream_base* strm, exlib::string mod, int32_t compress_type);

public:
    // ZipFile_base
    virtual result_t namelist(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t infolist(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t getinfo(exlib::string member, obj_ptr<NObject>& retVal, AsyncEvent* ac);
    virtual result_t read(exlib::string member, exlib::string password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t readAll(exlib::string password, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t extract(exlib::string member, exlib::string path, exlib::string password, AsyncEvent* ac);
    virtual result_t extract(exlib::string member, SeekableStream_base* strm, exlib::string password, AsyncEvent* ac);
    virtual result_t extractAll(exlib::string path, exlib::string password, AsyncEvent* ac);
    virtual result_t write(exlib::string filename, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
    virtual result_t write(SeekableStream_base* strm, exlib::string inZipName, exlib::string password, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);

private:
    result_t get_info(obj_ptr<Info>& retVal);
    result_t extract(SeekableStream_base* strm, exlib::string password);
    result_t read(exlib::string password, obj_ptr<Buffer_base>& retVal);
    result_t write(exlib::string filename, exlib::string password, SeekableStream_base* strm);
    result_t getFileCrc(SeekableStream_base* strm, uint32_t& crc);
    result_t checkGuard(exlib::string path);

private:
    unzFile m_unz;
    zipFile m_zip;
    int32_t m_compress_type;
    exlib::string m_mod;
    obj_ptr<SeekableStream_base> m_strm;
};

} /* namespace fibjs */
#endif /* ZIPFILE_H_ */
