/*
 * ZipFile.h
 *
 *  Created on: Mar 26, 2016
 *      Author: lion
 */

#include "ifs/ZipFile.h"
#include "ifs/ZipInfo.h"
#include "ifs/SeekableStream.h"
#include "unzip/include/unzip.h"
#include "unzip/include/zip.h"

#ifndef ZIPFILE_H_
#define ZIPFILE_H_

namespace fibjs
{

class ZipFile: public ZipFile_base
{
public:
	class Info: public ZipInfo_base
	{
	public:
		Info(const char* name, unz_file_info64& info);

	public:
		// ZipInfo_base
		virtual result_t get_filename(exlib::string& retVal);
		virtual result_t get_date(date_t& retVal);
		virtual result_t get_compress_type(exlib::string& retVal);
		virtual result_t get_compress_size(int64_t& retVal);
		virtual result_t get_file_size(int64_t& retVal);
		virtual result_t get_password(bool& retVal);
		virtual result_t get_data(obj_ptr<Buffer_base>& retVal);

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
	ZipFile(SeekableStream_base* strm, const char* mod, int32_t compress_type);

public:
	// ZipFile_base
	virtual result_t namelist(obj_ptr<List_base>& retVal, AsyncEvent* ac);
	virtual result_t infolist(obj_ptr<List_base>& retVal, AsyncEvent* ac);
	virtual result_t getinfo(const char* member, obj_ptr<ZipInfo_base>& retVal, AsyncEvent* ac);
	virtual result_t read(const char* member, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t readAll(obj_ptr<List_base>& retVal, AsyncEvent* ac);
    virtual result_t extract(const char* member, const char* path, AsyncEvent* ac);
    virtual result_t extract(const char* member, SeekableStream_base* strm, AsyncEvent* ac);
    virtual result_t extractAll(const char* path, AsyncEvent* ac);
	virtual result_t write(const char* filename, AsyncEvent* ac);
	virtual result_t write(Buffer_base* data, const char* inZipName, AsyncEvent* ac);
    virtual result_t write(SeekableStream_base* strm, const char* inZipName, AsyncEvent* ac);
	virtual result_t close(AsyncEvent* ac);

private:
	result_t get_info(obj_ptr<Info>& retVal);
	result_t extract(SeekableStream_base* strm, const char* password);
	result_t read(const char* password, obj_ptr<Buffer_base>& retVal);
	result_t write(const char* filename, SeekableStream_base* strm);

private:
	unzFile m_unz;
	zipFile m_zip;
	int32_t m_compress_type;
	qstring m_mod;
	obj_ptr<SeekableStream_base> m_strm;
};

} /* namespace fibjs */
#endif /* ZIPFILE_H_ */
